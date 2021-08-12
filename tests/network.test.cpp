#include <cpphots/network.h>
#include <cpphots/time_surface.h>
#include <cpphots/run.h>

#include "commons.h"

#include <gtest/gtest.h>


class TestNetwork : public ::testing::Test {

protected:

    void SetUp() override {

        network.createLayer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 50, 40, 2, 2, 100),
                            new MockClusterer(4));

        network.createLayer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(4, 50, 40, 2, 2, 100),
                            new MockClusterer(10));

        RandomEventGenerator ev_gen(50, 40, 2, 10);
        evs.resize(1000);
        std::generate(evs.begin(), evs.end(), [&ev_gen] () { return ev_gen.generateEvent();});

    }

    cpphots::Network network;
    cpphots::Events evs;

};

TEST_F(TestNetwork, Process) {

    for (auto& ev : evs) {
        network.process(ev, true);
    }

    auto hist = network.back().getHistogram();
    uint32_t hsum = std::accumulate(hist.begin(), hist.end(), 0);

    EXPECT_EQ(hsum, 1000);

}

TEST_F(TestNetwork, Iterator) {

    for (auto& ev : evs) {
        network.process(ev, true);
    }

    auto hist_proc = network.back().getHistogram();
    uint32_t hsum_proc = std::accumulate(hist_proc.begin(), hist_proc.end(), 0);

    network.reset();

    for (auto& ev : evs) {

        cpphots::Events cevs{ev};

        for (auto& l : network) {
            cevs = cpphots::process(l, cevs, false, true);
        }

    }

    auto hist_it = network.back().getHistogram();
    uint32_t hsum_it = std::accumulate(hist_it.begin(), hist_it.end(), 0);

    EXPECT_EQ(hsum_it, hsum_proc);
    EXPECT_EQ(hist_it, hist_proc);

}