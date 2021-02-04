#include <cpphots/time_surface.h>
#include <cpphots/events_utils.h>

#include <gtest/gtest.h>


TEST(TestTimeSurfaceSize, Square) {
    cpphots::TimeSurface ts(32, 32, 2, 2, 1000);
    EXPECT_EQ(ts.getWx(), 5);
    EXPECT_EQ(ts.getWy(), 5);
}

TEST(TestTimeSurfaceSize, Asymmetric) {
    cpphots::TimeSurface ts(32, 32, 5, 2, 1000);
    EXPECT_EQ(ts.getWx(), 11);
    EXPECT_EQ(ts.getWy(), 5);
}

TEST(TestTimeSurfaceSize, Rx0) {
    cpphots::TimeSurface ts(32, 32, 0, 2, 1000);
    EXPECT_EQ(ts.getWx(), 32);
    EXPECT_EQ(ts.getWy(), 5);
}

TEST(TestTimeSurfaceSize, Ry0) {
    cpphots::TimeSurface ts(32, 32, 2, 0, 1000);
    EXPECT_EQ(ts.getWx(), 5);
    EXPECT_EQ(ts.getWy(), 32);
}


TEST(TestTimeSurface, Processing) {

    // load data
    cpphots::Events events = cpphots::loadFromFile("data/trcl0.es");

    // create time surface
    cpphots::TimeSurface ts(32, 32, 2, 2, 1000);

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

    EXPECT_NEAR(normsum, 4740.313427652784, 0.1);
    EXPECT_NEAR(goodsum, 4562.696117657931, 0.1);
    EXPECT_EQ(goodevents, 1783);

}

TEST(TestTimeSurface, FullContext) {

    cpphots::TimeSurface ts(10, 10, 0, 0, 10);

    auto fc = ts.getFullContext();
    EXPECT_EQ(fc.rows(), 10);
    EXPECT_EQ(fc.cols(), 10);

    ts.update(2, 2, 2);

    EXPECT_NEAR(ts.sampleFullContext(2).sum(), 1, 0.001);

    ts.update(4, 4, 4);

    EXPECT_NEAR(ts.sampleFullContext(4).sum(), 1.8, 0.001);

}


TEST(TestTimeSurfacePool, Processing) {

    // load data
    cpphots::Events events = cpphots::loadFromFile("data/trcl0.es");

    // create time surface
    cpphots::TimeSurfacePool ts(32, 32, 2, 2, 1000, 2);

    cpphots::TimeSurfaceScalarType normsum = 0.;
    cpphots::TimeSurfaceScalarType goodsum = 0.;

    unsigned int processed = 0;
    unsigned int goodevents = 0;
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        auto nts = ts.updateAndCompute(ev.t, ev.x, ev.y, ev.p);
        cpphots::TimeSurfaceScalarType norm = nts.first.matrix().norm();
        normsum += norm;
        if (nts.second) {
            goodsum += norm;
            goodevents++;
        }
        processed++;
    }

    EXPECT_NEAR(normsum, 4740.313427652784, 0.1);
    EXPECT_NEAR(goodsum, 4562.696117657931, 0.1);
    EXPECT_EQ(goodevents, 1783);

}

TEST(TestTimeSurfacePool, FullContext) {

    cpphots::TimeSurfacePool tsp(10, 10, 0, 0, 10, 2);

    tsp.update(2, 2, 2, 0);
    auto ctxs = tsp.sampleFullContexts(2);
    EXPECT_EQ(ctxs.size(), 2);
    EXPECT_NEAR(ctxs[0].sum(), 1.0, 0.001);
    EXPECT_NEAR(ctxs[1].sum(), 0.0, 0.001);

    tsp.update(4, 4, 4, 1);
    ctxs = tsp.sampleFullContexts(4);
    EXPECT_NEAR(ctxs[0].sum(), 0.8, 0.001);
    EXPECT_NEAR(ctxs[1].sum(), 1.0, 0.001);

}