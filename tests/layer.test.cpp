#include <cpphots/layer.h>
#include <cpphots/time_surface.h>
#include <cpphots/events_utils.h>
#include <cpphots/classification.h>
#include <cpphots/clustering/cosine.h>

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


void set_centroids_nolearning(cpphots::Layer& layer) {

    layer.clearCentroids();

    cpphots::TimeSurfaceType c0(5, 5);
    c0 << 0.14268096, 0.16352641, 0.31639171, 0.50144441, 0.62164556,
          0.14278092, 0.20342093, 0.30996117, 0.56791866, 0.72610382,
          0.13264092, 0.18700444, 1.        , 0.51986262, 0.61111458,
          0.14740364, 0.18013568, 0.3467993 , 0.51867516, 0.63013684,
          0.0982774 , 0.19056831, 0.33077784, 0.46010118, 0.49291126;
    layer.addCentroid(c0.transpose());

    cpphots::TimeSurfaceType c1(5, 5);
    c1 << 6.46095900e-01, 7.37075586e-01, 2.30594041e-01, 4.29082662e-02, 1.98467911e-02,
          6.30938891e-01, 8.40823431e-01, 1.49802923e-01, 4.91213544e-02, 1.13857652e-02,
          5.37715036e-01, 7.15146816e-01, 1.00000000e+00, 2.71822306e-02, 6.06425018e-03,
          4.15795470e-01, 5.15580516e-01, 4.58331737e-01, 6.82087474e-02, 3.86408755e-04,
          2.41990917e-01, 2.87232990e-01, 2.87492378e-01, 1.00675253e-01, 6.09832073e-03;
    layer.addCentroid(c1.transpose());

    cpphots::TimeSurfaceType c2(5, 5);
    c2 << 0.33077019, 0.47714208, 0.51865324, 0.27678448, 0.12099174,
          0.5240164 , 0.63254814, 0.60641147, 0.21836176, 0.0728873 ,
          0.63551627, 0.79059511, 1.        , 0.07013124, 0.03739288,
          0.72618183, 0.74771765, 0.1163944 , 0.04274514, 0.01505505,
          0.69037351, 0.53182554, 0.09496877, 0.00998256, 0.        ;
    layer.addCentroid(c2.transpose());

    cpphots::TimeSurfaceType c3(5, 5);
    c3 << 0.23351855, 0.41286048, 0.47567516, 0.13020455, 0.02475676,
          0.38016762, 0.56713505, 0.6544299 , 0.07400296, 0.02425302,
          0.33837284, 0.61666306, 1.        , 0.09966997, 0.03848708,
          0.40436463, 0.62608022, 0.83653835, 0.26608844, 0.03747   ,
          0.3668632 , 0.60860181, 0.80455126, 0.40047835, 0.04008343;
    layer.addCentroid(c3.transpose());

    cpphots::TimeSurfaceType c4(5, 5);
    c4 << 0.24795398, 0.46543157, 0.64472238, 0.73375373, 0.39218719,
          0.31364299, 0.59423629, 0.77175757, 0.6298749 , 0.15885801,
          0.4208679 , 0.66448617, 1.        , 0.24320644, 0.05466736,
          0.52129229, 0.77554678, 0.73019761, 0.09352665, 0.02568038,
          0.57661421, 0.77543757, 0.51517947, 0.0886765 , 0.01128504;
    layer.addCentroid(c4.transpose());

    cpphots::TimeSurfaceType c5(5, 5);
    c5 << 0.01062577, 0.03637437, 0.12304534, 0.07055056, 0.06711633,
          0.01893299, 0.08265546, 0.28087492, 0.10979399, 0.02775173,
          0.03101859, 0.07722714, 1.        , 0.13519165, 0.21201036,
          0.04806788, 0.06072652, 0.14698101, 0.15995923, 0.6639988 ,
          0.15660742, 0.10866447, 0.0895708 , 0.28393882, 0.32582679;
    layer.addCentroid(c5.transpose());

    cpphots::TimeSurfaceType c6(5, 5);
    c6 << 0.35748569, 0.64602345, 0.73206059, 0.3043873 , 0.01976889,
          0.35884044, 0.55367877, 0.80622972, 0.30639917, 0.05596716,
          0.31929985, 0.50589968, 1.        , 0.27810873, 0.03406283,
          0.17838012, 0.27257049, 0.42261202, 0.39383068, 0.09756165,
          0.11699865, 0.16756312, 0.1723387 , 0.29296432, 0.14198433;
    layer.addCentroid(c6.transpose());

    cpphots::TimeSurfaceType c7(5, 5);
    c7 << 0.11928518, 0.15646176, 0.33761618, 0.38499849, 0.41138663,
          0.2198144 , 0.25438581, 0.42858229, 0.64292542, 0.38890317,
          0.24283558, 0.42508951, 1.        , 0.67590288, 0.37137282,
          0.33899783, 0.49542624, 0.73563155, 0.66684405, 0.13452593,
          0.35660329, 0.64379783, 0.65548024, 0.33611779, 0.06304927;
    layer.addCentroid(c7.transpose());

}


void set_centroids_learning(cpphots::Layer& layer) {

    layer.clearCentroids();

    cpphots::TimeSurfaceType c0(5, 5);
    c0 << 0.204, 0.654, 0.884, 0.92 , 0.   ,
          0.   , 0.267, 0.692, 0.936, 0.   ,
          0.14 , 0.492, 1.   , 0.   , 0.   ,
          0.   , 0.098, 0.751, 0.   , 0.   ,
          0.   , 0.   , 0.   , 0.   , 0.   ;
    layer.addCentroid(c0.transpose());

    cpphots::TimeSurfaceType c1(5, 5);
    c1 << 0.731, 0.69 , 0.936, 0.966, 0.   ,
          0.696, 0.666, 0.784, 0.892, 0.   ,
          0.396, 0.792, 1.   , 0.966, 0.   ,
          0.958, 0.684, 0.828, 0.   , 0.   ,
          0.376, 0.811, 0.981, 0.   , 0.   ;
    layer.addCentroid(c1.transpose());

    cpphots::TimeSurfaceType c2(5, 5);
    c2 << 0.868, 0.   , 0.   , 0.   , 0.   ,
          0.   , 0.   , 0.   , 0.   , 0.   ,
          0.   , 0.   , 1.   , 0.   , 0.   ,
          0.   , 0.   , 0.   , 0.   , 0.   ,
          0.   , 0.   , 0.   , 0.   , 0.   ;
    layer.addCentroid(c2.transpose());

    cpphots::TimeSurfaceType c3(5, 5);
    c3 << 0.   , 0.452, 0.267, 0.818, 0.874,
          0.   , 0.835, 0.146, 0.775, 0.595,
          0.   , 0.   , 1.   , 0.225, 1.   ,
          0.   , 0.   , 0.   , 0.   , 0.929,
          0.   , 0.   , 0.   , 0.   , 0.   ;
    layer.addCentroid(c3.transpose());

    cpphots::TimeSurfaceType c4(5, 5);
    c4 << 0.   , 0.756, 0.985, 0.   , 0.   ,
          0.   , 0.985, 0.719, 0.   , 0.   ,
          0.653, 0.   , 1.   , 0.   , 0.   ,
          0.09 , 0.729, 0.   , 0.   , 0.   ,
          0.702, 0.   , 0.   , 0.   , 0.   ;
    layer.addCentroid(c4.transpose());

    cpphots::TimeSurfaceType c5(5, 5);
    c5 << 0.   , 0.   , 0.103, 0.   , 0.142,
          0.   , 0.   , 0.163, 0.   , 0.   ,
          0.136, 0.   , 1.   , 0.   , 0.   ,
          0.   , 0.   , 0.   , 0.   , 0.134,
          0.   , 0.502, 0.   , 0.   , 0.   ;
    layer.addCentroid(c5.transpose());

    cpphots::TimeSurfaceType c6(5, 5);
    c6 << 0.374, 0.376, 0.744, 0.   , 0.   ,
          0.567, 0.386, 0.431, 0.   , 0.   ,
          0.724, 0.842, 1.   , 0.   , 0.   ,
          0.741, 0.876, 0.   , 0.   , 0.   ,
          0.993, 0.   , 0.902, 0.   , 0.   ;
    layer.addCentroid(c6.transpose());

    cpphots::TimeSurfaceType c7(5, 5);
    c7 << 0.   , 0.   , 0.   , 0.   , 0.   ,
          0.   , 0.   , 0.   , 0.003, 0.   ,
          0.   , 0.   , 1.   , 0.   , 0.   ,
          0.   , 0.   , 0.   , 0.   , 0.   ,
          0.   , 0.   , 0.   , 0.838, 0.   ;
    layer.addCentroid(c7.transpose());

}


class TestLayerProcessing : public ::testing::Test {

protected:

    void SetUp() override {
        events = cpphots::loadFromFile("tests/data/trcl0.es");
        layer.addTSPool(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(1, 32, 32, 2, 2, 1000));
        layer.createClusterer<cpphots::CosineClusterer>(8);
    }

    cpphots::Events events;
    cpphots::Layer layer;

};

TEST_F(TestLayerProcessing, WithoutLearning) {

    set_centroids_nolearning(layer);

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

    set_centroids_learning(layer);

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


class TestLayerSeeding : public ::testing::Test {

protected:

    void SetUp() override {
        events = cpphots::loadFromFile("tests/data/trcl0.es");
        layer.addTSPool(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000));
        layer.createClusterer<cpphots::CosineClusterer>(8);
    }

    cpphots::Events events;
    cpphots::Layer layer;

};

TEST_F(TestLayerSeeding, Uniform) {

    cpphots::layerSeedCentroids(cpphots::ClustererUniformSeeding, layer, events);
    ASSERT_TRUE(layer.hasCentroids());

    for (const auto& c : layer.getCentroids()) {
        ASSERT_GE(c.minCoeff(), 0.0);
        ASSERT_LE(c.maxCoeff(), 1.0);
    }

    layer.clearCentroids();
    ASSERT_FALSE(layer.hasCentroids());

    cpphots::layerSeedCentroids(cpphots::ClustererUniformSeeding, layer, {events, events});
    ASSERT_TRUE(layer.hasCentroids());

    for (const auto& c : layer.getCentroids()) {
        ASSERT_GE(c.minCoeff(), 0.0);
        ASSERT_LE(c.maxCoeff(), 1.0);
    }

}

TEST_F(TestLayerSeeding, PlusPLus) {

    cpphots::layerSeedCentroids(cpphots::ClustererPlusPlusSeeding, layer, events);
    ASSERT_TRUE(layer.hasCentroids());

    for (const auto& c : layer.getCentroids()) {
        ASSERT_GE(c.minCoeff(), 0.0);
        ASSERT_LE(c.maxCoeff(), 1.0);
    }

    layer.clearCentroids();
    ASSERT_FALSE(layer.hasCentroids());

    cpphots::layerSeedCentroids(cpphots::ClustererPlusPlusSeeding, layer, {events, events});
    ASSERT_TRUE(layer.hasCentroids());

    for (const auto& c : layer.getCentroids()) {
        ASSERT_GE(c.minCoeff(), 0.0);
        ASSERT_LE(c.maxCoeff(), 1.0);
    }

}

TEST_F(TestLayerSeeding, AFKMC2) {

    cpphots::layerSeedCentroids(cpphots::ClustererAFKMC2Seeding(5), layer, events);
    ASSERT_TRUE(layer.hasCentroids());

    for (const auto& c : layer.getCentroids()) {
        ASSERT_GE(c.minCoeff(), 0.0);
        ASSERT_LE(c.maxCoeff(), 1.0);
    }

    layer.clearCentroids();
    ASSERT_FALSE(layer.hasCentroids());

    cpphots::layerSeedCentroids(cpphots::ClustererAFKMC2Seeding(5), layer, {events, events});
    ASSERT_TRUE(layer.hasCentroids());

    for (const auto& c : layer.getCentroids()) {
        ASSERT_GE(c.minCoeff(), 0.0);
        ASSERT_LE(c.maxCoeff(), 1.0);
    }

}

TEST_F(TestLayerSeeding, Random) {

    cpphots::layerSeedCentroids(cpphots::ClustererRandomSeeding(5, 5), layer, events);
    ASSERT_TRUE(layer.hasCentroids());

    for (const auto& c : layer.getCentroids()) {
        ASSERT_GE(c.minCoeff(), 0.0);
        ASSERT_LE(c.maxCoeff(), 1.0);
    }

    layer.clearCentroids();
    ASSERT_FALSE(layer.hasCentroids());

    cpphots::layerSeedCentroids(cpphots::ClustererRandomSeeding(5, 5), layer, {events, events});
    ASSERT_TRUE(layer.hasCentroids());

    for (const auto& c : layer.getCentroids()) {
        ASSERT_GE(c.minCoeff(), 0.0);
        ASSERT_LE(c.maxCoeff(), 1.0);
    }

}


#ifdef CPPHOTS_ASSERTS
TEST(TestLayer, NoSeeding) {

    cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000),
                         new cpphots::CosineClusterer(8));
    EXPECT_DEATH({
        layer.process({0, 0, 0, 0}, true);
    }, "Assertion");

}
#endif


TEST(TestLayer, SkipValidityCheck) {

    cpphots::Events events = cpphots::loadFromFile("tests/data/trcl0.es");

    cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000),
                         new cpphots::CosineClusterer(8));

    auto seeding = cpphots::ClustererRandomSeeding(5, 5);
    seeding(layer, {});

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


#ifdef CPPHOTS_ASSERTS
TEST(TestLayer, AssertPool) {

    cpphots::Layer layer;

    ASSERT_DEATH({
        layer.process({0, 0, 0, 0});
    }, "Assertion");

}

TEST(TestLayer, AssertClustering) {

    cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000));

    ASSERT_DEATH({
        layer.getNumClusters();
    }, "Assertion");

}
#endif


TEST(TestLayer, TSAccess) {

    cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000),
                         new cpphots::CosineClusterer(8));

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

// TODO: re-enable this
// TEST(TestLayer, Wrongsize) {

//     ASSERT_THROW(cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 10, 20, 5, 5, 10000),
//                                        HasSize(10, 30)),
//                  std::invalid_argument);

//     ASSERT_NO_THROW(cpphots::create_layer(cpphots::create_pool<cpphots::LinearTimeSurface>(2, 10, 20, 5, 5, 10000),
//                                           HasSize(10, 20)));

// }

TEST(TestLayer, CreateLayer) {

    {
        cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(8, 10, 20, 5, 5, 10000));
        EXPECT_EQ(layer.getSize().first, 10);
        EXPECT_EQ(layer.getSize().second, 20);
        EXPECT_EQ(layer.getNumSurfaces(), 8);
    }

    {
        cpphots::Layer layer(cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(8, 10, 20, 5, 5, 10000),
                             new cpphots::CosineClusterer(12));
        EXPECT_EQ(layer.getNumClusters(), 12);
    }

}


class TimeLayerClone : public ::testing::Test {

protected:

    void SetUp() override {
        orig_pool = cpphots::create_pool_ptr<cpphots::LinearTimeSurface>(2, 10, 10, 0, 0, 10);
        orig_layer.addTSPool(orig_pool);
    }

    cpphots::interfaces::TimeSurfacePoolCalculator* orig_pool;
    cpphots::Layer orig_layer;

};

TEST_F(TimeLayerClone, CopyConstructor) {

    cpphots::Layer layer(orig_layer);
    cpphots::interfaces::TimeSurfacePoolCalculator* pool = &layer.getTSPool();
    EXPECT_NE(pool, nullptr);
    EXPECT_NE(pool, orig_pool);

}

TEST_F(TimeLayerClone, MoveConstructor) {

    cpphots::Layer layer(std::move(orig_layer));
    cpphots::interfaces::TimeSurfacePoolCalculator* pool = &layer.getTSPool();
    EXPECT_NE(pool, nullptr);
    EXPECT_THROW(orig_layer.getTSPool(), std::runtime_error);

}

TEST_F(TimeLayerClone, CopyAssignment) {

    cpphots::Layer layer;
    layer = orig_layer;
    cpphots::interfaces::TimeSurfacePoolCalculator* pool = &layer.getTSPool();
    EXPECT_NE(pool, nullptr);
    EXPECT_NE(pool, orig_pool);

}

TEST_F(TimeLayerClone, MoveAssignment) {

    cpphots::Layer layer;
    layer = std::move(orig_layer);
    cpphots::interfaces::TimeSurfacePoolCalculator* pool = &layer.getTSPool();
    EXPECT_NE(pool, nullptr);
    EXPECT_THROW(orig_layer.getTSPool(), std::runtime_error);

}