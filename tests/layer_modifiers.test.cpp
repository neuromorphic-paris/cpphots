#include <cpphots/layer_modifiers.h>
#include <cpphots/layer.h>
#include <cpphots/time_surface.h>

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

TEST(TestModifiers, SuperCell) {

    cpphots::SuperCell sup(103, 50, 4);

    auto [supw, suph] = sup.getCellSizes();

    ASSERT_EQ(supw, 25);
    ASSERT_EQ(suph, 12);

    for (uint16_t x = 0; x < 103; x++) {
        for (uint16_t y = 0; y < 50; y++) {
            auto cell = sup.findCell(x, y);
            if (x < 100 && y < 48) {
                EXPECT_NE(cell, cpphots::invalid_coordinates);
            } else {
                EXPECT_EQ(cell, cpphots::invalid_coordinates);
            }
        }
    }

}


TEST(TestModifiersLayer, ArrayLayer) {

    cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 100, 100, 5, 5, 10000));
    layer.createClusterer<MockClusterer>(10);
    layer.createRemapper<cpphots::ArrayLayer>();

    RandomEventGenerator reg(100, 100, 2);

    for (int i = 0; i < 10000; i++) {
        auto evin = reg();
        auto evout = layer.process(evin, true);
        EXPECT_EQ(evout.t, evin.t);
        EXPECT_EQ(evout.x, i % 10);
        EXPECT_EQ(evout.y, evin.y);
        EXPECT_EQ(evout.p, 0);
    }

}

TEST(TestModifiersLayer, SerializingLayer) {

    cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 10, 10, 5, 5, 10000));
    layer.createClusterer<MockClusterer>(10);
    layer.createRemapper<cpphots::SerializingLayer>(10, 10);

    RandomEventGenerator reg(10, 10, 2);

    bool raised = false;
    for (int i = 0; i < 1000; i++) {
        auto evin = reg();
        try {
            auto evout = layer.process(evin, true);
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

    cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 100, 100, 5, 5, 10000));
    layer.createClusterer<MockClusterer>(10);
    layer.createRemapper<cpphots::SerializingLayer>(100, 100);

    RandomEventGenerator reg(100, 100, 2);

    bool raised = false;
    for (int i = 0; i < 10; i++) {
        auto evin = reg();
        try {
            auto evout = layer.process(evin, true);
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

TEST(TestModifiers, SuperCellCoordinates) {

    cpphots::SuperCell sup(50, 50, 5);

    std::vector<std::pair<PointType, PointType>> cases{
        {{9, 5}, {1, 1}},
        {{14, 10}, {2, 2}},
        {{8, 36}, {1, 7}},
        {{39, 40}, {7, 8}},
        {{24, 15}, {4, 3}},
        {{48, 48}, {9, 9}},
        {{48, 0}, {9, 0}},
        {{0, 48}, {0, 9}},
        {{0, 0}, {0, 0}}
    };

    for (auto [tp, tr] : cases) {
        auto cell = sup.findCell(tp.first, tp.second);
        EXPECT_EQ(cell, tr);
    }

}


TEST(TestModifiersLayer, SuperCell) {

    cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 50, 50, 5, 5, 10000));
    layer.createClusterer<MockClusterer>(10);
    layer.createSuperCell<cpphots::SuperCell>(50, 50, 5);

    auto evt = layer.process(10, 8, 36, 0, true);

    EXPECT_EQ(evt.x, 1);
    EXPECT_EQ(evt.y, 7);

}