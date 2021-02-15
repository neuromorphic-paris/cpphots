#include <cpphots/layer_modifiers.h>
#include <cpphots/layer.h>

#include <gtest/gtest.h>

#include "commons.h"


using PointType = std::pair<uint16_t, uint16_t>;

TEST(TestModifiers, ArrayLayer) {

    cpphots::ArrayLayer al;

    RandomEventGenerator reg(100, 200, 10);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> distk(0, 20);


    for (int i = 0; i < 10000; i++) {
        auto evin = reg();
        uint16_t k = distk(gen);
        auto evout = al.remapEvent(evin, k);
        EXPECT_EQ(evout.t, evin.t);
        EXPECT_EQ(evout.x, k);
        EXPECT_EQ(evout.y, evin.y);
        EXPECT_EQ(evout.p, 0);
    }

}

TEST(TestModifiers, SerializingLayer) {

    cpphots::SerializingLayer sl(20, 30);

    RandomEventGenerator reg(100, 200, 10);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> distk(0, 20);


    for (int i = 0; i < 10000; i++) {
        auto evin = reg();
        uint16_t k = distk(gen);
        auto evout = sl.remapEvent(evin, k);
        EXPECT_EQ(evout.t, evin.t);
        EXPECT_EQ(evout.x, 20*30*k + 20*evin.y + evin.x);
        EXPECT_EQ(evout.y, 0);
        EXPECT_EQ(evout.p, 0);
    }

}

TEST(TestModifiers, Averaging) {
    
    cpphots::Averaging avg(50, 50, 5, 1);

    std::vector<std::pair<PointType, std::vector<PointType>>> cases{
        {{9, 5}, {{2, 1}}},
        {{14, 10}, {{3, 2}}},
        {{8, 36}, {{1, 8}, {2, 8}, {1, 9}, {2, 9}}},
        {{39, 40}, {{9, 9}, {9, 10}}},
        {{24, 15}, {{5, 3}, {6, 3}}},
        {{48, 48}, {{11, 11}}},
        {{48, 0}, {{11, 0}}},
        {{0, 48}, {{0, 11}}},
        {{0, 0}, {{0, 0}}}
    };

    for (auto [tp, tr] : cases) {
        auto cells = avg.findCells(tp.first, tp.second);
        EXPECT_EQ(cells, tr);
    }

}


TEST(TestModifiersLayer, ArrayLayer) {

    auto layer = cpphots::create_layer(cpphots::TimeSurfacePool(100, 100, 5, 5, 10000, 2),
                                       MockClusterer(10),
                                       cpphots::ArrayLayer{});

    RandomEventGenerator reg(100, 100, 2);

    for (int i = 0; i < 10000; i++) {
        auto evin = reg();
        auto evout = layer.process(evin, true)[0];
        EXPECT_EQ(evout.t, evin.t);
        EXPECT_EQ(evout.x, i % 10);
        EXPECT_EQ(evout.y, evin.y);
        EXPECT_EQ(evout.p, 0);
    }

}

TEST(TestModifiersLayer, SerializingLayer) {

    auto layer = cpphots::create_layer(cpphots::TimeSurfacePool(10, 10, 5, 5, 10000, 2),
                                       MockClusterer(10),
                                       cpphots::SerializingLayer(10, 10));

    RandomEventGenerator reg(10, 10, 2);

    bool raised = false;
    for (int i = 0; i < 1000; i++) {
        auto evin = reg();
        try {
            auto evout = layer.process(evin, true)[0];
            EXPECT_EQ(evout.t, evin.t);
            EXPECT_EQ(evout.x, 10*10*(i % 10) + 10*evin.y + evin.x);
            EXPECT_EQ(evout.y, 0);
            EXPECT_EQ(evout.p, 0);
        } catch (std::runtime_error re) {
            raised = true;
            break;
        }
    }
    EXPECT_FALSE(raised);

}

TEST(TestModifiersLayer, SerializingLayerException) {

    auto layer = cpphots::create_layer(cpphots::TimeSurfacePool(100, 100, 5, 5, 10000, 2),
                                       MockClusterer(10),
                                       cpphots::SerializingLayer(100, 100));

    RandomEventGenerator reg(100, 100, 2);

    bool raised = false;
    for (int i = 0; i < 10; i++) {
        auto evin = reg();
        try {
            auto evout = layer.process(evin, true)[0];
            EXPECT_EQ(evout.t, evin.t);
            EXPECT_EQ(evout.x, 100*100*(i % 10) + 100*evin.y + evin.x);
            EXPECT_EQ(evout.y, 0);
            EXPECT_EQ(evout.p, 0);
        } catch (std::runtime_error re) {
            raised = true;
            break;
        }
    }
    EXPECT_TRUE(raised);

}