#include <sstream>

#include <cpphots/time_surface.h>
#include <cpphots/layer.h>
#include <cpphots/network.h>

#include <gtest/gtest.h>


TEST(TestSaveLoad, SimpleTSLoad) {

    std::string ts_string("5 5 2 2 5 5 1.2 4");

    cpphots::TimeSurface ts;
    std::stringstream instream(ts_string);
    instream >> ts;

    auto context = ts.getFullContext();
    EXPECT_EQ(context.cols(), 9);
    EXPECT_EQ(context.rows(), 9);

    std::stringstream outstream;
    outstream << ts;

    EXPECT_EQ(ts_string, outstream.str());

}


TEST(TestSaveLoad, TSProcess) {

    // load data
    cpphots::Events events = cpphots::loadFromFile("data/trcl0.es");

    // create time surface
    cpphots::TimeSurface ts(32, 32, 2, 2, 1000);

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

TEST(TestSaveLoad, LSaveLoad) {

    // cpphots::Layer layer1(32, 32, 1, 2, 1000, 2, 8);
    auto layer1 = cpphots::create_layer(cpphots::TimeSurfacePool(32, 32, 1, 2, 1000, 2),
                                        cpphots::HOTSClusterer(8));

    auto initializer = cpphots::ClustererRandomInitializer(3, 5);
    initializer(layer1, {});

    std::stringstream outstream;
    outstream << layer1;

    cpphots::Layer<cpphots::TimeSurfacePool,
                   cpphots::HOTSClusterer> layer2;

    std::stringstream instream(outstream.str());
    instream >> layer2;

    // ASSERT_EQ(layer1.getDescription(), layer2.getDescription());
    ASSERT_EQ(layer1.getNumClusters(), layer2.getNumClusters());

    auto surface = layer2.getSurface(1);
    ASSERT_EQ(surface.getWx(), 3);
    ASSERT_EQ(surface.getWy(), 5);

    ASSERT_TRUE(layer2.isInitialized());

}
