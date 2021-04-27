#include <sstream>

#include <cpphots/time_surface.h>
#include <cpphots/layer.h>
#include <cpphots/network.h>
#include <cpphots/load.h>

#include <gtest/gtest.h>


TEST(TestSaveLoad, SimpleTSLoad) {

    std::string ts_string("!LINEARTIMESURFACE\n5 5 2 2 5 5 1.2 4\n");

    cpphots::LinearTimeSurface ts;
    std::stringstream instream(ts_string);
    instream >> ts;

    auto context = ts.getFullContext();
    EXPECT_EQ(context.cols(), 9);
    EXPECT_EQ(context.rows(), 9);

    std::stringstream outstream;
    outstream << ts;

    EXPECT_EQ(ts_string, outstream.str());

}

TEST(TestSaveLoad, SimpleWTSLoad) {

    std::string ts_string("5 5 2 2 5 5 1.2 4");

    cpphots::TimeSurfaceType w = cpphots::TimeSurfaceType::Constant(32, 32, 0.5);

    cpphots::WeightedLinearTimeSurface ts1(32, 32, 2, 2, 1000, w);

    std::ostringstream outstream1;
    outstream1 << ts1;

    cpphots::WeightedLinearTimeSurface ts2;

    std::istringstream instream(outstream1.str());
    instream >> ts2;

    std::stringstream outstream2;
    outstream2 << ts2;

    EXPECT_EQ(outstream1.str(), outstream2.str());

}

TEST(TestSaveLoad, TSProcess) {

    // load data
    cpphots::Events events = cpphots::loadFromFile("data/trcl0.es");

    // create time surface
    cpphots::LinearTimeSurface ts(32, 32, 2, 2, 1000);

    std::stringstream instream("32 32 2 2 5 5 1000.0 4");
    instream >> ts;

    cpphots::TimeSurfaceScalarType normsum = 0.;
    cpphots::TimeSurfaceScalarType goodsum = 0.;

    unsigned int processed = 0;
    unsigned int goodevents = 0;
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        auto nts = ts.updateAndCompute(ev.t, ev.x, ev.y);
        cpphots::TimeSurfaceScalarType norm = nts.first.matrix().norm();
        normsum += norm;
        if (nts.second) {
            goodsum += norm;
            goodevents++;
        }
        processed++;
    }

    ASSERT_NEAR(normsum, 4740.313427652784, 0.1);
    ASSERT_NEAR(goodsum, 4562.696117657931, 0.1);
    EXPECT_EQ(goodevents, 1783);

}

TEST(TestSaveLoad, TSPool) {

    auto tsp1 = cpphots::create_pool<cpphots::LinearTimeSurface>(2, 30, 50, 2, 2, 1000);

    std::ostringstream out;
    out << tsp1;

    std::istringstream in(out.str());
    cpphots::TimeSurfacePool tsp2;
    in >> tsp2;

    auto [sx, sy] = tsp1.getSurface(1)->getSize();

    EXPECT_EQ(sx, 30);
    EXPECT_EQ(sy, 50);

}

TEST(TestSaveLoad, LSaveLoad) {

    auto layer1 = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 32, 32, 1, 2, 1000),
                                        cpphots::HOTSClusterer(8));

    auto initializer = cpphots::ClustererRandomInitializer(3, 5);
    initializer(layer1, {});

    std::stringstream outstream;
    outstream << layer1;

    cpphots::Layer<cpphots::TimeSurfacePool,
                   cpphots::HOTSClusterer> layer2;

    std::stringstream instream(outstream.str());
    instream >> layer2;

    ASSERT_EQ(layer1.getNumClusters(), layer2.getNumClusters());

    auto surface = layer2.getSurface(1);
    ASSERT_EQ(surface->getWx(), 3);
    ASSERT_EQ(surface->getWy(), 5);

    ASSERT_TRUE(layer2.isInitialized());

}

TEST(TestLoad, Layer) {

    auto layer1 = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 32, 32, 1, 2, 1000),
                                        cpphots::HOTSClusterer(8));

    auto initializer = cpphots::ClustererRandomInitializer(3, 5);
    initializer(layer1, {});

    std::stringstream outstream;
    outstream << layer1;

    std::stringstream instream(outstream.str());

    auto layer2 = cpphots::loadLayerFromStream(instream);

    cpphots::TimeSurfacePool* pool2 = dynamic_cast<cpphots::TimeSurfacePool*>(layer2.get());
    cpphots::HOTSClusterer* clust2 = dynamic_cast<cpphots::HOTSClusterer*>(layer2.get());

    ASSERT_EQ(layer1.getNumClusters(), clust2->getNumClusters());

    auto surface = pool2->getSurface(1);
    ASSERT_EQ(surface->getWx(), 3);
    ASSERT_EQ(surface->getWy(), 5);

    ASSERT_TRUE(clust2->isInitialized());

}

TEST(TestLoad, Network) {

    cpphots::Network net1;
    net1.addLayer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 32, 32, 1, 2, 1000),
                  cpphots::HOTSClusterer(8));
    net1.addLayer(cpphots::create_pool<cpphots::LinearTimeSurface>(8, 32, 32, 2, 4, 2000),
                  cpphots::HOTSClusterer(16));

    auto pools1 = net1.viewFull<cpphots::TimeSurfacePool>();
    auto clust1 = net1.viewFull<cpphots::ClustererBase>();

    auto initializer1 = cpphots::ClustererRandomInitializer(3, 5);
    initializer1(*clust1[0], {});
    auto initializer2 = cpphots::ClustererRandomInitializer(5, 9);
    initializer2(*clust1[1], {});

    std::stringstream outstream;
    outstream << net1;

    std::stringstream instream(outstream.str());

    cpphots::Network net2;
    instream >> net2;

    auto pools2 = net2.viewFull<cpphots::TimeSurfacePool>();
    auto clust2 = net2.viewFull<cpphots::ClustererBase>();

    EXPECT_EQ(net1.getNumLayers(), net2.getNumLayers());

    EXPECT_EQ(clust1[0]->getNumClusters(), clust2[0]->getNumClusters());
    EXPECT_EQ(clust1[1]->getNumClusters(), clust2[1]->getNumClusters());

    {
        auto surface = pools2[0]->getSurface(1);
        EXPECT_EQ(surface->getWx(), 3);
        EXPECT_EQ(surface->getWy(), 5);
    }

    {
        auto surface = pools2[1]->getSurface(7);
        EXPECT_EQ(surface->getWx(), 5);
        EXPECT_EQ(surface->getWy(), 9);
    }

    EXPECT_TRUE(clust2[0]->isInitialized());
    EXPECT_TRUE(clust2[1]->isInitialized());

}