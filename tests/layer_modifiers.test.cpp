#include <cpphots/layer_modifiers.h>

#include <gtest/gtest.h>


using PointType = std::pair<uint16_t, uint16_t>;


TEST(TestAveraging, FindCells) {
    
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