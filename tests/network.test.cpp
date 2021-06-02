#include <cpphots/network.h>
#include <cpphots/time_surface.h>

#include <gtest/gtest.h>


struct A{};


TEST(TestNetwork, View) {

    cpphots::Network network;
    cpphots::Layer<cpphots::TimeSurfacePool, A>* l1 = new cpphots::Layer<cpphots::TimeSurfacePool, A>();
    cpphots::Layer<cpphots::TimeSurfacePool>* l2 = new cpphots::Layer<cpphots::TimeSurfacePool>();
    cpphots::Layer<cpphots::TimeSurfacePool, A>* l3 = new cpphots::Layer<cpphots::TimeSurfacePool, A>();
    cpphots::Layer<cpphots::TimeSurfacePool>* l4 = new cpphots::Layer<cpphots::TimeSurfacePool>();
    network.addLayer(cpphots::LayerPtr(l1));
    network.addLayer(cpphots::LayerPtr(l2));
    network.addLayer(cpphots::LayerPtr(l3));
    network.addLayer(cpphots::LayerPtr(l4));

    auto a = network.view<A>();
    EXPECT_EQ(a.size(), 2);
    std::vector<A*> expected{l1, l3};
    EXPECT_EQ(a, expected);

    a = network.viewFull<A>();
    EXPECT_EQ(a.size(), 4);
    expected = std::vector<A*>{l1, nullptr, l3, nullptr};
    EXPECT_EQ(a, expected);

}

TEST(TestNetwork, GetLayer) {

    cpphots::Network network;
    cpphots::Layer<cpphots::TimeSurfacePool, A>* l1 = new cpphots::Layer<cpphots::TimeSurfacePool, A>();
    cpphots::Layer<cpphots::TimeSurfacePool>* l2 = new cpphots::Layer<cpphots::TimeSurfacePool>();
    network.addLayer(cpphots::LayerPtr(l1));
    network.addLayer(cpphots::LayerPtr(l2));

    ASSERT_THROW(A& a2 = network.getLayer<A>(1), std::bad_cast);

    A& a = network.getLayer<A>(0);
    EXPECT_EQ(std::addressof(a), l1);

    cpphots::LayerBase& lr1 = network.getLayer(0);
    cpphots::LayerBase& lr2 = network.getLayer(1);

    EXPECT_EQ(std::addressof(lr1), l1);
    EXPECT_EQ(std::addressof(lr2), l2);

    EXPECT_EQ(std::addressof(network.back<A>()), l1);
    EXPECT_EQ(std::addressof(network.back()), l2);

}

TEST(TestNetwork, Subnetwork) {

    cpphots::Network network;
    cpphots::Layer<cpphots::TimeSurfacePool>* l0 = new cpphots::Layer<cpphots::TimeSurfacePool>();
    cpphots::Layer<cpphots::TimeSurfacePool>* l1 = new cpphots::Layer<cpphots::TimeSurfacePool>();
    cpphots::Layer<cpphots::TimeSurfacePool>* l2 = new cpphots::Layer<cpphots::TimeSurfacePool>();
    cpphots::Layer<cpphots::TimeSurfacePool>* l3 = new cpphots::Layer<cpphots::TimeSurfacePool>();
    network.addLayer(cpphots::LayerPtr(l0));
    network.addLayer(cpphots::LayerPtr(l1));
    network.addLayer(cpphots::LayerPtr(l2));
    network.addLayer(cpphots::LayerPtr(l3));

    auto net2 = network.getSubnetwork(0, 2);
    EXPECT_EQ(net2.getNumLayers(), 2);
    EXPECT_EQ(std::addressof(net2.getLayer(0)), l0);
    EXPECT_EQ(std::addressof(net2.getLayer(1)), l1);

    auto net3 = network.getSubnetwork(1);
    EXPECT_EQ(net3.getNumLayers(), 3);
    EXPECT_EQ(std::addressof(net3.getLayer(0)), l1);
    EXPECT_EQ(std::addressof(net3.getLayer(1)), l2);
    EXPECT_EQ(std::addressof(net3.getLayer(2)), l3);

    auto net4 = network.getSubnetwork(0, -1);
    EXPECT_EQ(net4.getNumLayers(), 3);
    EXPECT_EQ(std::addressof(net4.getLayer(0)), l0);
    EXPECT_EQ(std::addressof(net4.getLayer(1)), l1);
    EXPECT_EQ(std::addressof(net4.getLayer(2)), l2);

}