#include <cpphots/layer.h>
#include <cpphots/events_utils.h>
#include <cpphots/classification.h>

#include <gtest/gtest.h>


bool operator==(const cpphots::Features& f1, const cpphots::Features& f2) {
    if (f1.size() != f2.size())
        return false;
    bool ret = true;
    for (size_t i = 0; i < f1.size(); i++) {
        ret = ret && (f1[i] == f2[i]);
    }
    return ret;
}


void set_prototypes_nolearning(cpphots::HOTSClusterer& layer) {

    layer.clearPrototypes();

    cpphots::TimeSurfaceType p0(5, 5);
    p0 << 0.14268096, 0.16352641, 0.31639171, 0.50144441, 0.62164556,
          0.14278092, 0.20342093, 0.30996117, 0.56791866, 0.72610382,
          0.13264092, 0.18700444, 1.        , 0.51986262, 0.61111458,
          0.14740364, 0.18013568, 0.3467993 , 0.51867516, 0.63013684,
          0.0982774 , 0.19056831, 0.33077784, 0.46010118, 0.49291126;
    layer.addPrototype(p0.transpose());

    cpphots::TimeSurfaceType p1(5, 5);
    p1 << 6.46095900e-01, 7.37075586e-01, 2.30594041e-01, 4.29082662e-02, 1.98467911e-02,
          6.30938891e-01, 8.40823431e-01, 1.49802923e-01, 4.91213544e-02, 1.13857652e-02,
          5.37715036e-01, 7.15146816e-01, 1.00000000e+00, 2.71822306e-02, 6.06425018e-03,
          4.15795470e-01, 5.15580516e-01, 4.58331737e-01, 6.82087474e-02, 3.86408755e-04,
          2.41990917e-01, 2.87232990e-01, 2.87492378e-01, 1.00675253e-01, 6.09832073e-03;
    layer.addPrototype(p1.transpose());

    cpphots::TimeSurfaceType p2(5, 5);
    p2 << 0.33077019, 0.47714208, 0.51865324, 0.27678448, 0.12099174,
          0.5240164 , 0.63254814, 0.60641147, 0.21836176, 0.0728873 ,
          0.63551627, 0.79059511, 1.        , 0.07013124, 0.03739288,
          0.72618183, 0.74771765, 0.1163944 , 0.04274514, 0.01505505,
          0.69037351, 0.53182554, 0.09496877, 0.00998256, 0.        ;
    layer.addPrototype(p2.transpose());

    cpphots::TimeSurfaceType p3(5, 5);
    p3 << 0.23351855, 0.41286048, 0.47567516, 0.13020455, 0.02475676,
          0.38016762, 0.56713505, 0.6544299 , 0.07400296, 0.02425302,
          0.33837284, 0.61666306, 1.        , 0.09966997, 0.03848708,
          0.40436463, 0.62608022, 0.83653835, 0.26608844, 0.03747   ,
          0.3668632 , 0.60860181, 0.80455126, 0.40047835, 0.04008343;
    layer.addPrototype(p3.transpose());

    cpphots::TimeSurfaceType p4(5, 5);
    p4 << 0.24795398, 0.46543157, 0.64472238, 0.73375373, 0.39218719,
          0.31364299, 0.59423629, 0.77175757, 0.6298749 , 0.15885801,
          0.4208679 , 0.66448617, 1.        , 0.24320644, 0.05466736,
          0.52129229, 0.77554678, 0.73019761, 0.09352665, 0.02568038,
          0.57661421, 0.77543757, 0.51517947, 0.0886765 , 0.01128504;
    layer.addPrototype(p4.transpose());

    cpphots::TimeSurfaceType p5(5, 5);
    p5 << 0.01062577, 0.03637437, 0.12304534, 0.07055056, 0.06711633,
          0.01893299, 0.08265546, 0.28087492, 0.10979399, 0.02775173,
          0.03101859, 0.07722714, 1.        , 0.13519165, 0.21201036,
          0.04806788, 0.06072652, 0.14698101, 0.15995923, 0.6639988 ,
          0.15660742, 0.10866447, 0.0895708 , 0.28393882, 0.32582679;
    layer.addPrototype(p5.transpose());

    cpphots::TimeSurfaceType p6(5, 5);
    p6 << 0.35748569, 0.64602345, 0.73206059, 0.3043873 , 0.01976889,
          0.35884044, 0.55367877, 0.80622972, 0.30639917, 0.05596716,
          0.31929985, 0.50589968, 1.        , 0.27810873, 0.03406283,
          0.17838012, 0.27257049, 0.42261202, 0.39383068, 0.09756165,
          0.11699865, 0.16756312, 0.1723387 , 0.29296432, 0.14198433;
    layer.addPrototype(p6.transpose());

    cpphots::TimeSurfaceType p7(5, 5);
    p7 << 0.11928518, 0.15646176, 0.33761618, 0.38499849, 0.41138663,
          0.2198144 , 0.25438581, 0.42858229, 0.64292542, 0.38890317,
          0.24283558, 0.42508951, 1.        , 0.67590288, 0.37137282,
          0.33899783, 0.49542624, 0.73563155, 0.66684405, 0.13452593,
          0.35660329, 0.64379783, 0.65548024, 0.33611779, 0.06304927;
    layer.addPrototype(p7.transpose());

}


void set_prototpes_learning(cpphots::HOTSClusterer& layer) {

    layer.clearPrototypes();

    cpphots::TimeSurfaceType pl0(5, 5);
    pl0 << 0.204, 0.654, 0.884, 0.92 , 0.   ,
           0.   , 0.267, 0.692, 0.936, 0.   ,
           0.14 , 0.492, 1.   , 0.   , 0.   ,
           0.   , 0.098, 0.751, 0.   , 0.   ,
           0.   , 0.   , 0.   , 0.   , 0.   ;
    layer.addPrototype(pl0.transpose());

    cpphots::TimeSurfaceType pl1(5, 5);
    pl1 << 0.731, 0.69 , 0.936, 0.966, 0.   ,
           0.696, 0.666, 0.784, 0.892, 0.   ,
           0.396, 0.792, 1.   , 0.966, 0.   ,
           0.958, 0.684, 0.828, 0.   , 0.   ,
           0.376, 0.811, 0.981, 0.   , 0.   ;
    layer.addPrototype(pl1.transpose());

    cpphots::TimeSurfaceType pl2(5, 5);
    pl2 << 0.868, 0.   , 0.   , 0.   , 0.   ,
           0.   , 0.   , 0.   , 0.   , 0.   ,
           0.   , 0.   , 1.   , 0.   , 0.   ,
           0.   , 0.   , 0.   , 0.   , 0.   ,
           0.   , 0.   , 0.   , 0.   , 0.   ;
    layer.addPrototype(pl2.transpose());

    cpphots::TimeSurfaceType pl3(5, 5);
    pl3 << 0.   , 0.452, 0.267, 0.818, 0.874,
           0.   , 0.835, 0.146, 0.775, 0.595,
           0.   , 0.   , 1.   , 0.225, 1.   ,
           0.   , 0.   , 0.   , 0.   , 0.929,
           0.   , 0.   , 0.   , 0.   , 0.   ;
    layer.addPrototype(pl3.transpose());

    cpphots::TimeSurfaceType pl4(5, 5);
    pl4 << 0.   , 0.756, 0.985, 0.   , 0.   ,
           0.   , 0.985, 0.719, 0.   , 0.   ,
           0.653, 0.   , 1.   , 0.   , 0.   ,
           0.09 , 0.729, 0.   , 0.   , 0.   ,
           0.702, 0.   , 0.   , 0.   , 0.   ;
    layer.addPrototype(pl4.transpose());

    cpphots::TimeSurfaceType pl5(5, 5);
    pl5 << 0.   , 0.   , 0.103, 0.   , 0.142,
           0.   , 0.   , 0.163, 0.   , 0.   ,
           0.136, 0.   , 1.   , 0.   , 0.   ,
           0.   , 0.   , 0.   , 0.   , 0.134,
           0.   , 0.502, 0.   , 0.   , 0.   ;
    layer.addPrototype(pl5.transpose());

    cpphots::TimeSurfaceType pl6(5, 5);
    pl6 << 0.374, 0.376, 0.744, 0.   , 0.   ,
           0.567, 0.386, 0.431, 0.   , 0.   ,
           0.724, 0.842, 1.   , 0.   , 0.   ,
           0.741, 0.876, 0.   , 0.   , 0.   ,
           0.993, 0.   , 0.902, 0.   , 0.   ;
    layer.addPrototype(pl6.transpose());

    cpphots::TimeSurfaceType pl7(5, 5);
    pl7 << 0.   , 0.   , 0.   , 0.   , 0.   ,
           0.   , 0.   , 0.   , 0.003, 0.   ,
           0.   , 0.   , 1.   , 0.   , 0.   ,
           0.   , 0.   , 0.   , 0.   , 0.   ,
           0.   , 0.   , 0.   , 0.838, 0.   ;
    layer.addPrototype(pl7.transpose());

}


class TestLayerProcessing : public ::testing::Test {

protected:

    void SetUp() override {
        events = cpphots::loadFromFile("data/trcl0.es");
        layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(1, 32, 32, 2, 2, 1000),
                                      cpphots::HOTSClusterer(8));
    }

    cpphots::Events events;
    cpphots::Layer<cpphots::TimeSurfacePool,
                   cpphots::HOTSClusterer> layer;

};

TEST_F(TestLayerProcessing, WithoutLearning) {

    set_prototypes_nolearning(layer);

    layer.toggleLearning(false);
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        cpphots::event mev = ev;
        mev.p = 0;
        layer.process(mev);
    }

    cpphots::Features expected{188, 205, 281, 233, 229, 276, 194, 177};
    EXPECT_EQ(layer.getHistogram(), expected);

}

TEST_F(TestLayerProcessing, WithLearning) {

    set_prototpes_learning(layer);

    // learn
    layer.reset();
    layer.toggleLearning(true);
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        cpphots::event mev = ev;
        mev.p = 0;
        layer.process(mev);
    }

    cpphots::Features expected_learning{233, 167, 187, 207, 326, 278, 271, 114};
    EXPECT_EQ(layer.getHistogram(), expected_learning);

    // post learning
    layer.reset();
    layer.toggleLearning(false);
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        cpphots::event mev = ev;
        mev.p = 0;
        layer.process(mev);
    }

    // after learning: [206 272 202 277 226 181 310 109] (cosine) or [211 173 197 209 295 293 284 121] (L2)
    cpphots::Features expected_afterlearning{211, 173, 197, 209, 295, 293, 284, 121};
    EXPECT_EQ(layer.getHistogram(), expected_afterlearning);

}


class TestLayerInitialization : public ::testing::Test {

protected:

    void SetUp() override {
        events = cpphots::loadFromFile("data/trcl0.es");
        layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000),
                                      cpphots::HOTSClusterer(8));
    }

    cpphots::Events events;
    cpphots::Layer<cpphots::TimeSurfacePool,
                   cpphots::HOTSClusterer> layer;

};

TEST_F(TestLayerInitialization, Uniform) {

    cpphots::layerInitializePrototypes(cpphots::ClustererUniformInitializer, layer, layer, events);
    ASSERT_TRUE(layer.isInitialized());

    layer.clearPrototypes();
    cpphots::layerInitializePrototypes(cpphots::ClustererUniformInitializer, layer, layer, {events, events});
    ASSERT_TRUE(layer.isInitialized());

}

TEST_F(TestLayerInitialization, PlusPLus) {

    cpphots::layerInitializePrototypes(cpphots::ClustererPlusPlusInitializer, layer, layer, events);
    ASSERT_TRUE(layer.isInitialized());

    layer.clearPrototypes();
    cpphots::layerInitializePrototypes(cpphots::ClustererPlusPlusInitializer, layer, layer, {events, events});
    ASSERT_TRUE(layer.isInitialized());

}

TEST_F(TestLayerInitialization, AFKMC2) {

    cpphots::layerInitializePrototypes(cpphots::ClustererAFKMC2Initializer(5), layer, layer, events);
    ASSERT_TRUE(layer.isInitialized());

    layer.clearPrototypes();
    cpphots::layerInitializePrototypes(cpphots::ClustererAFKMC2Initializer(5), layer, layer, {events, events});
    ASSERT_TRUE(layer.isInitialized());

}

TEST_F(TestLayerInitialization, Random) {

    cpphots::layerInitializePrototypes(cpphots::ClustererRandomInitializer(5, 5), layer, layer, events);
    ASSERT_TRUE(layer.isInitialized());

    layer.clearPrototypes();
    cpphots::layerInitializePrototypes(cpphots::ClustererRandomInitializer(5, 5), layer, layer, {events, events});
    ASSERT_TRUE(layer.isInitialized());

}


TEST(TestLayer, NoInitialization) {

    auto layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000),
                                       cpphots::HOTSClusterer(8));

    ASSERT_THROW(layer.process({0, 0, 0, 0}, true), std::runtime_error);

}


TEST(TestLayer, SkipValidityCheck) {

    cpphots::Events events = cpphots::loadFromFile("data/trcl0.es");

    auto layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000),
                                       cpphots::HOTSClusterer(8));

    auto initializer = cpphots::ClustererRandomInitializer(5, 5);
    initializer(layer, {});

    for (auto& ev : events) {
        layer.process(ev, true);
    }
    auto hist = layer.getHistogram();
    uint32_t hcount = 0;
    for (auto& h : hist) {
        hcount += h;
    }
    EXPECT_EQ(events.size(), hcount);

}


TEST(TestLayer, WrongPolarity) {

    auto layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000),
                                       cpphots::HOTSClusterer(8));

    auto initializer = cpphots::ClustererRandomInitializer(5, 5);
    initializer(layer, {});

    ASSERT_THROW(layer.process({0, 0, 0, 2}), std::invalid_argument);

}


TEST(TestLayer, TSAccess) {

    auto layer = cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000),
                                       cpphots::HOTSClusterer(8));

    auto ts1 = layer.getSurface(0)->updateAndCompute(10, 2, 2);
    auto ts2 = layer.compute(10, 2, 2, 0);

    bool v = ts1.first.isApprox(ts2.first);

    ASSERT_TRUE(v);

    ts2 = layer.updateAndCompute(20, 3, 3, 1);
    ts1 = layer.getSurface(1)->updateAndCompute(20, 3, 3);

    v = ts1.first.isApprox(ts2.first);

    ASSERT_TRUE(v);

}

struct HasSize {

    HasSize(uint16_t w, uint16_t h)
        :w(w), h(h) {}

    std::pair<uint16_t, uint16_t> getSize() const {
        return {w, h};
    }

    uint16_t w, h;

};

TEST(TestLayer, Wrongsize) {

    ASSERT_THROW(cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 10, 20, 5, 5, 10000),
                                       HasSize(10, 30)),
                 std::invalid_argument);

    ASSERT_NO_THROW(cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 10, 20, 5, 5, 10000),
                                          HasSize(10, 20)));

}
