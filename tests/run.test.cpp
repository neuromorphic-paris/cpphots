#include <cpphots/run.h>
#include <cpphots/time_surface.h>

#include "commons.h"

#include <gtest/gtest.h>


class TestProcess : public ::testing::Test {

protected:

    void SetUp() override {
        layer.addTSPool(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 50, 40, 2, 2, 100));
        layer.addClusterer(new MockClusterer(4));

        RandomEventGenerator ev_gen(50, 40, 2, 10);
        ev100.resize(100);
        std::generate(ev100.begin(), ev100.end(), [&ev_gen] () { return ev_gen.generateEvent();});
        ev200.resize(200);
        std::generate(ev200.begin(), ev200.end(), [&ev_gen] () { return ev_gen.generateEvent();});
    }

    static uint32_t sum_histogram(const std::vector<uint32_t>& hist) {
        return std::accumulate(hist.begin(), hist.end(), 0);
    }

    cpphots::Layer layer;
    cpphots::Events ev100, ev200;

};

TEST_F(TestProcess, NoReset) {

    cpphots::process(layer, ev100, false, true);
    uint32_t hsum = sum_histogram(layer.getHistogram());

    EXPECT_EQ(hsum, 100);

    cpphots::process(layer, ev200, false, true);
    hsum = sum_histogram(layer.getHistogram());

    EXPECT_EQ(hsum, 300);

    layer.reset();

    cpphots::process(layer, {ev100, ev200}, false, true);
    hsum = sum_histogram(layer.getHistogram());

    EXPECT_EQ(hsum, 300);

}

TEST_F(TestProcess, WithReset) {

    cpphots::process(layer, ev100, true, true);
    uint32_t hsum = sum_histogram(layer.getHistogram());

    EXPECT_EQ(hsum, 100);

    cpphots::process(layer, ev200, true, true);
    hsum = sum_histogram(layer.getHistogram());

    EXPECT_EQ(hsum, 200);

    layer.reset();

    cpphots::process(layer, {ev100, ev200}, true, true);
    hsum = sum_histogram(layer.getHistogram());

    EXPECT_EQ(hsum, 200);

}