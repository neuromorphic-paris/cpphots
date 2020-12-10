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

    float normsum = 0.;
    float goodsum = 0.;

    unsigned int processed = 0;
    unsigned int goodevents = 0;
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        auto nts = ts.updateAndCompute(ev.t, ev.x, ev.y);
        float norm = nts.first.matrix().norm();
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
