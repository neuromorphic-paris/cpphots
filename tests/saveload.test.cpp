#include <sstream>

#include <cpphots/time_surface.h>
#include <cpphots/layer.h>
#include <cpphots/network.h>
#include <cpphots/load.h>
#include <cpphots/events_utils.h>
#include <cpphots/clustering/cosine.h>
#include <cpphots/layer_modifiers.h>

#include <gtest/gtest.h>


TEST(TestSaveLoad, SimpleTSLoad) {

    std::string ts_string("!LINEARTIMESURFACE\n5 5 2 2 5 5 1.2 4\n");

    {
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

    {
        std::stringstream instream(ts_string);
        auto ts = cpphots::loadTSFromStream(instream);

        auto context = ts->getFullContext();
        EXPECT_EQ(context.cols(), 9);
        EXPECT_EQ(context.rows(), 9);

        std::stringstream outstream;
        outstream << *ts;

        EXPECT_EQ(ts_string, outstream.str());
    }

}

TEST(TestSaveLoad, SimpleWTSLoad) {

    cpphots::TimeSurfaceType w = cpphots::TimeSurfaceType::Constant(32, 32, 0.5);

    cpphots::WeightedLinearTimeSurface ts1(32, 32, 2, 2, 1000, w);

    std::ostringstream outstream1;
    outstream1 << ts1;

    {
        std::istringstream instream(outstream1.str());

        cpphots::WeightedLinearTimeSurface ts2;
        instream >> ts2;

        std::stringstream outstream2;
        outstream2 << ts2;

        EXPECT_EQ(outstream1.str(), outstream2.str());
    }

    {
        std::istringstream instream(outstream1.str());

        auto ts2 = cpphots::loadTSFromStream(instream);

        std::stringstream outstream2;
        outstream2 << *ts2;

        EXPECT_EQ(outstream1.str(), outstream2.str());
    }

}

TEST(TestSaveLoad, TSProcess) {

    // load data
    cpphots::Events events = cpphots::loadFromFile("tests/data/trcl0.es");

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

    {
        std::istringstream in(out.str());
        cpphots::TimeSurfacePool tsp2;
        in >> tsp2;

        auto [sx, sy] = tsp2.getSurface(1)->getSize();

        EXPECT_EQ(sx, 30);
        EXPECT_EQ(sy, 50);
    }

    {
        std::istringstream in(out.str());
        auto tsp2 = cpphots::loadTSPoolFromStream(in);

        auto [sx, sy] = tsp2->getSurface(1)->getSize();

        EXPECT_EQ(sx, 30);
        EXPECT_EQ(sy, 50);

        delete tsp2;
    }

}

TEST(TestSaveLoad, LSaveLoad) {

    cpphots::Layer layer1(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 32, 32, 1, 2, 1000),
                          new cpphots::CosineClusterer(8));

    auto seeding = cpphots::ClustererRandomSeeding(3, 5);
    seeding(layer1, {});

    std::stringstream outstream;
    outstream << layer1;

    cpphots::Layer layer2;

    std::stringstream instream(outstream.str());
    instream >> layer2;

    ASSERT_EQ(layer1.getNumClusters(), layer2.getNumClusters());

    auto surface = layer2.getSurface(1);
    ASSERT_EQ(surface->getWx(), 3);
    ASSERT_EQ(surface->getWy(), 5);

    ASSERT_TRUE(layer2.hasCentroids());

}

TEST(TestLoad, Network) {

    cpphots::Network net1;
    net1.createLayer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 32, 32, 1, 2, 1000),
                     new cpphots::CosineClusterer(8));
    net1.createLayer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(8, 32, 32, 2, 4, 2000),
                     new cpphots::CosineClusterer(16));

    // auto pools1 = net1.viewFull<cpphots::TimeSurfacePool>();
    // auto clust1 = net1.viewFull<cpphots::interfaces::Clusterer>();

    auto seeding1 = cpphots::ClustererRandomSeeding(3, 5);
    seeding1(net1[0], {});
    auto seeding2 = cpphots::ClustererRandomSeeding(5, 9);
    seeding2(net1[1], {});

    std::stringstream outstream;
    outstream << net1;

    std::stringstream instream(outstream.str());

    cpphots::Network net2;
    instream >> net2;

    // auto pools2 = net2.viewFull<cpphots::TimeSurfacePool>();
    // auto clust2 = net2.viewFull<cpphots::interfaces::Clusterer>();

    EXPECT_EQ(net1.getNumLayers(), net2.getNumLayers());

    EXPECT_EQ(net1[0].getNumClusters(), net2[0].getNumClusters());
    EXPECT_EQ(net1[1].getNumClusters(), net2[1].getNumClusters());

    {
        auto surface = net2[0].getSurface(1);
        EXPECT_EQ(surface->getWx(), 3);
        EXPECT_EQ(surface->getWy(), 5);
    }

    {
        auto surface = net2[1].getSurface(7);
        EXPECT_EQ(surface->getWx(), 5);
        EXPECT_EQ(surface->getWy(), 9);
    }

    EXPECT_TRUE(net2[0].hasCentroids());
    EXPECT_TRUE(net2[1].hasCentroids());

}

TEST(TestSaveLoad, ArrayLayer) {

    cpphots::ArrayLayer mod1;

    std::stringstream outstream;
    outstream << mod1;

    {
        std::stringstream instream(outstream.str());

        cpphots::ArrayLayer mod2;
        instream >> mod2;
    }

    {
        std::stringstream instream(outstream.str());

        auto mod2 = cpphots::loadRemapperFromStream(instream);
        delete mod2;
    }

}

TEST(TestSaveLoad, SerializingLayer) {

    cpphots::SerializingLayer mod1(10, 20);

    std::stringstream outstream;
    outstream << mod1;

    {
        std::stringstream instream(outstream.str());

        cpphots::SerializingLayer mod2;
        instream >> mod2;

        EXPECT_EQ(mod1.getSize().first, mod2.getSize().first);
        EXPECT_EQ(mod1.getSize().second, mod2.getSize().second);
    }

    {
        std::stringstream instream(outstream.str());

        cpphots::SerializingLayer* mod2 = dynamic_cast<cpphots::SerializingLayer*>(cpphots::loadRemapperFromStream(instream));

        EXPECT_EQ(mod1.getSize().first, mod2->getSize().first);
        EXPECT_EQ(mod1.getSize().second, mod2->getSize().second);

        delete mod2;
    }

}

TEST(TestSaveLoad, SuperCell) {

    cpphots::SuperCell mod1(49, 9, 5);

    std::stringstream outstream;
    outstream << mod1;

    {
        std::stringstream instream(outstream.str());

        cpphots::SuperCell mod2;
        instream >> mod2;

        EXPECT_EQ(mod1.getSize().first, mod2.getSize().first);
        EXPECT_EQ(mod1.getSize().second, mod2.getSize().second);

        EXPECT_EQ(mod1.getCellSizes().first, mod2.getCellSizes().first);
        EXPECT_EQ(mod1.getCellSizes().second, mod2.getCellSizes().second);
    }

    {
        std::stringstream instream(outstream.str());

        auto mod2 = cpphots::loadSuperCellFromStream(instream);

        EXPECT_EQ(mod1.getSize().first, mod2->getSize().first);
        EXPECT_EQ(mod1.getSize().second, mod2->getSize().second);

        EXPECT_EQ(mod1.getCellSizes().first, mod2->getCellSizes().first);
        EXPECT_EQ(mod1.getCellSizes().second, mod2->getCellSizes().second);

        delete mod2;
    }

}

TEST(TestSaveLoad, SuperCellAverage) {

    cpphots::SuperCellAverage mod1(49, 9, 5);

    std::stringstream outstream;
    outstream << mod1;

    {
        std::stringstream instream(outstream.str());

        cpphots::SuperCellAverage mod2;
        instream >> mod2;

        EXPECT_EQ(mod1.getSize().first, mod2.getSize().first);
        EXPECT_EQ(mod1.getSize().second, mod2.getSize().second);

        EXPECT_EQ(mod1.getCellSizes().first, mod2.getCellSizes().first);
        EXPECT_EQ(mod1.getCellSizes().second, mod2.getCellSizes().second);
    }

        {
        std::stringstream instream(outstream.str());

        auto mod2 = cpphots::loadSuperCellFromStream(instream);

        EXPECT_EQ(mod1.getSize().first, mod2->getSize().first);
        EXPECT_EQ(mod1.getSize().second, mod2->getSize().second);

        EXPECT_EQ(mod1.getCellSizes().first, mod2->getCellSizes().first);
        EXPECT_EQ(mod1.getCellSizes().second, mod2->getCellSizes().second);
    }

}