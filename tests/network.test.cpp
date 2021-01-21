#include <cpphots/network.h>

#include <gtest/gtest.h>


struct A{};


TEST(TestNetwork, View) {

    cpphots::Network network;
    cpphots::Layer<cpphots::TimeSurfacePool, A> l1;
    cpphots::Layer<cpphots::TimeSurfacePool> l2;
    cpphots::Layer<cpphots::TimeSurfacePool, A> l3;
    cpphots::Layer<cpphots::TimeSurfacePool> l4;
    network.addLayer(l1);
    network.addLayer(l2);
    network.addLayer(l3);
    network.addLayer(l4);

    auto a = network.view<A>();
    EXPECT_EQ(a.size(), 2);
    std::vector<A*> expected{&l1, &l3};
    EXPECT_EQ(a, expected);

    a = network.viewFull<A>();
    EXPECT_EQ(a.size(), 4);
    expected = std::vector<A*>{&l1, nullptr, &l3, nullptr};
    EXPECT_EQ(a, expected);

}

TEST(TestNetwork, GetLayer) {

    cpphots::Network network;
    cpphots::Layer<cpphots::TimeSurfacePool, A> l1;
    cpphots::Layer<cpphots::TimeSurfacePool> l2;
    network.addLayer(l1);
    network.addLayer(l2);

    ASSERT_THROW(A& a2 = network.getLayer<A>(1), std::bad_cast);

    A& a = network.getLayer<A>(0);
    ASSERT_EQ(std::addressof(a), std::addressof(l1));

    cpphots::LayerBase& lr1 = network.getLayer(0);
    cpphots::LayerBase& lr2 = network.getLayer(1);

    ASSERT_EQ(std::addressof(lr1), std::addressof(l1));
    ASSERT_EQ(std::addressof(lr2), std::addressof(l2));

    ASSERT_EQ(std::addressof(network.back<A>()), std::addressof(l1));
    ASSERT_EQ(std::addressof(network.back()), std::addressof(l2));

}