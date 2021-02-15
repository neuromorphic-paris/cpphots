#include "commons.h"

#include <gtest/gtest.h>


TEST(TestCommons, RandomEventGenerator) {

    RandomEventGenerator reg(100, 200, 300);

    for (int i = 0; i < 10000; i++) {
        auto ev = reg.generateEvent();
        EXPECT_LT(ev.x, 100);
        EXPECT_LT(ev.y, 200);
        EXPECT_LT(ev.p, 300);
    }

    reg = RandomEventGenerator(10, 100, 20, 200, 30, 300);

    for (int i = 0; i < 10000; i++) {
        auto ev = reg.generateEvent();
        EXPECT_GE(ev.x, 10);
        EXPECT_LT(ev.x, 100);
        EXPECT_GE(ev.y, 20);
        EXPECT_LT(ev.y, 200);
        EXPECT_GE(ev.p, 30);
        EXPECT_LT(ev.p, 300);
    }

    reg = RandomEventGenerator(10, 10, 10, 10);

    uint64_t last_t = 0;

    for (int i = 0; i < 10000; i++) {
        auto ev = reg.generateEvent();
        EXPECT_GE(ev.t, last_t);
        last_t = ev.t;
    }

}


TEST(TestCommons, MockClusterer) {

    cpphots::TimeSurfaceType ts;

    std::vector<uint16_t> ks{3, 4, 6, 6, 7, 5, 3, 4};
    MockClusterer mc(ks);

    for (size_t i = 0; i < 10000; i++) {
        auto k = mc.cluster(ts);
        EXPECT_EQ(k, ks[i % ks.size()]);
    }

    ks = std::vector<uint16_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    mc = MockClusterer(10);
    
    for (size_t i = 0; i < 10000; i++) {
        auto k = mc.cluster(ts);
        EXPECT_EQ(k, ks[i % ks.size()]);
    }

    std::vector<uint32_t> vec(10, 1000);

    EXPECT_EQ(mc.getHistogram(), vec);

}