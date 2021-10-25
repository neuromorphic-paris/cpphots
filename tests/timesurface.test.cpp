#include <cpphots/time_surface.h>
#include <cpphots/events_utils.h>

#include <gtest/gtest.h>


TEST(TestTimeSurfaceSize, Square) {
    cpphots::LinearTimeSurface ts(32, 32, 2, 2, 1000);
    EXPECT_EQ(ts.getWx(), 5);
    EXPECT_EQ(ts.getWy(), 5);
}

TEST(TestTimeSurfaceSize, Asymmetric) {
    cpphots::LinearTimeSurface ts(32, 32, 5, 2, 1000);
    EXPECT_EQ(ts.getWx(), 11);
    EXPECT_EQ(ts.getWy(), 5);
}

TEST(TestTimeSurfaceSize, Rx0) {
    cpphots::LinearTimeSurface ts(32, 32, 0, 2, 1000);
    EXPECT_EQ(ts.getWx(), 32);
    EXPECT_EQ(ts.getWy(), 5);
}

TEST(TestTimeSurfaceSize, Ry0) {
    cpphots::LinearTimeSurface ts(32, 32, 2, 0, 1000);
    EXPECT_EQ(ts.getWx(), 5);
    EXPECT_EQ(ts.getWy(), 32);
}


TEST(TestTimeSurface, Processing) {

    // load data
    cpphots::Events events = cpphots::loadFromFile("tests/data/trcl0.es");

    // create time surface
    cpphots::LinearTimeSurface ts(32, 32, 2, 2, 1000);

    cpphots::TimeSurfaceScalarType normsum = 0.;
    cpphots::TimeSurfaceScalarType goodsum = 0.;

    unsigned int processed = 0;
    unsigned int goodevents = 0;
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        auto nts = ts.updateAndCompute(ev.t, ev.x, ev.y);
        cpphots::TimeSurfaceScalarType norm = nts.first.matrix().norm();
        normsum += norm;
        if (nts.second) {
            goodsum += norm;
            goodevents++;
        }
        processed++;
    }

    EXPECT_NEAR(normsum, 4740.313427652784, 0.1);
    EXPECT_NEAR(goodsum, 4562.696117657931, 0.1);
    EXPECT_EQ(goodevents, 1783);

}

#ifdef CPPHOTS_ASSERTS
TEST(TestTimeSurface, WrongCoordinates) {

    cpphots::LinearTimeSurface ts(20, 10, 2, 2, 10);

    EXPECT_DEATH({
        ts.compute({0, 30, 30, 0});
    }, "Assertion");

}
#endif

TEST(TestTimeSurface, FullContext) {

    {
        cpphots::LinearTimeSurface ts(200, 100, 10, 5, 10);

        auto c = ts.getContext();
        EXPECT_EQ(c.rows(), 100);
        EXPECT_EQ(c.cols(), 200);
        EXPECT_NEAR(c.sum(), -10*200*100, 0.001);

        auto fc = ts.getFullContext();
        EXPECT_EQ(fc.rows(), 110);
        EXPECT_EQ(fc.cols(), 220);
        EXPECT_NEAR(fc.sum(), -10*220*110, 0.001);

    }

    {

        cpphots::LinearTimeSurface ts(10, 10, 0, 0, 10);

        auto fc = ts.getContext();
        EXPECT_EQ(fc.rows(), 10);
        EXPECT_EQ(fc.cols(), 10);
        EXPECT_NEAR(fc.sum(), -10*10*10, 0.001);

        ts.update(2, 2, 2);
        EXPECT_NEAR(ts.sampleContext(2).sum(), 1, 0.001);

        ts.update(4, 4, 4);
        EXPECT_NEAR(ts.sampleContext(4).sum(), 1.8, 0.001);

    }

}


TEST(TestWeightedTimeSurface, Processing) {

    // load data
    cpphots::Events events = cpphots::loadFromFile("tests/data/trcl0.es");

    // create time surface
    cpphots::WeightedLinearTimeSurface ts(32, 32, 2, 2, 1000, cpphots::TimeSurfaceType::Constant(32, 32, 0.5f));

    cpphots::TimeSurfaceScalarType normsum = 0.;
    cpphots::TimeSurfaceScalarType goodsum = 0.;

    unsigned int processed = 0;
    unsigned int goodevents = 0;
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        auto nts = ts.updateAndCompute(ev.t, ev.x, ev.y);
        cpphots::TimeSurfaceScalarType norm = nts.first.matrix().norm();
        normsum += norm;
        if (nts.second) {
            goodsum += norm;
            goodevents++;
        }
        processed++;
    }

    EXPECT_NEAR(normsum, 4740.313427652784 * 0.5f, 0.1);
    EXPECT_NEAR(goodsum, 4562.696117657931 * 0.5f, 0.1);
    EXPECT_EQ(goodevents, 1783);

}

TEST(TestWeightedTimeSurface, Basic) {

    {
        cpphots::LinearTimeSurface lts(10, 10, 2, 2, 10);
        cpphots::WeightedLinearTimeSurface wts(10, 10, 2, 2, 10, cpphots::TimeSurfaceType::Constant(10, 10, 0.5f));

        cpphots::TimeSurfaceType ts_diff = lts.updateAndCompute(2, 2, 2).first * 0.5f - wts.updateAndCompute(2, 2, 2).first;
        EXPECT_NEAR(ts_diff.matrix().norm(), 0.f, 0.001);

        cpphots::TimeSurfaceType c_diff = lts.sampleContext(3) * 0.5f - wts.sampleContext(3);
        EXPECT_NEAR(c_diff.matrix().norm(), 0.f, 0.001);
    }

    {
        cpphots::LinearTimeSurface lts1(10, 10, 2, 2, 10);
        cpphots::LinearTimeSurface lts2(10, 10, 2, 2, 10);

        cpphots::TimeSurfaceType w = cpphots::TimeSurfaceType::Zero(10, 20);
        w.block(0, 0, 10, 10) = cpphots::TimeSurfaceType::Constant(10, 10, 0.25);
        w.block(0, 10, 10, 10) = cpphots::TimeSurfaceType::Constant(10, 10, 0.75);

        cpphots::WeightedLinearTimeSurface wts(20, 10, 2, 2, 10, w);

        lts1.update(2, 2, 2);
        lts2.update(2, 2, 2);
        wts.update(2, 2, 2);
        wts.update(2, 12, 2);

        cpphots::TimeSurfaceType res = cpphots::TimeSurfaceType::Zero(10, 20);
        res.block(0, 0, 10, 10) = lts1.sampleContext(2) * 0.25 - wts.sampleContext(2).block(0, 0, 10, 10);
        res.block(0, 10, 10, 10) = lts2.sampleContext(2) * 0.75 - wts.sampleContext(2).block(0, 10, 10, 10);

        EXPECT_NEAR(res.matrix().norm(), 0.f, 0.001);

        wts.update(20, 9, 5);
        auto ts = wts.updateAndCompute(20, 10, 5);
        EXPECT_NEAR(ts.first.sum(), 1.f, 0.001);
        EXPECT_EQ(ts.first.count(), 2);
    }

}

TEST(TestWeightedTimeSurface, FullContext) {

    size_t sz1 = 2600;
    size_t sz2 = 20;

    cpphots::TimeSurfaceType w = cpphots::TimeSurfaceType::Zero(1, sz1 + sz2);
    w.block(0, 0, 1, sz1) = cpphots::TimeSurfaceType::Constant(1, sz1, 0.1);
    w.block(0, sz1, 1, sz2) = cpphots::TimeSurfaceType::Constant(1, sz2, 0.9);

    cpphots::WeightedLinearTimeSurface wts(sz1+sz2, 1, 0, 0, 25000., w);

    auto ts = wts.updateAndCompute(2800022, 2606, 0);

    EXPECT_NEAR(ts.first.sum(), 0.9f, 0.001f);

}


TEST(TestTimeSurfacePool, Processing) {

    // load data
    cpphots::Events events = cpphots::loadFromFile("tests/data/trcl0.es");

    // create time surface
    auto ts = cpphots::create_pool<cpphots::LinearTimeSurface>(2, 32, 32, 2, 2, 1000);

    cpphots::TimeSurfaceScalarType normsum = 0.;
    cpphots::TimeSurfaceScalarType goodsum = 0.;

    unsigned int processed = 0;
    unsigned int goodevents = 0;
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        auto nts = ts.updateAndCompute(ev.t, ev.x, ev.y, ev.p);
        cpphots::TimeSurfaceScalarType norm = nts.first.matrix().norm();
        normsum += norm;
        if (nts.second) {
            goodsum += norm;
            goodevents++;
        }
        processed++;
    }

    EXPECT_NEAR(normsum, 4740.313427652784, 0.1);
    EXPECT_NEAR(goodsum, 4562.696117657931, 0.1);
    EXPECT_EQ(goodevents, 1783);

}

TEST(TestWeightedTimeSurfacePool, Processing) {

    // load data
    cpphots::Events events = cpphots::loadFromFile("tests/data/trcl0.es");

    // create time surface
    auto ts = cpphots::create_pool<cpphots::WeightedLinearTimeSurface>(2, 32, 32, 2, 2, 1000, cpphots::TimeSurfaceType::Constant(32, 32, 0.5));

    cpphots::TimeSurfaceScalarType normsum = 0.;
    cpphots::TimeSurfaceScalarType goodsum = 0.;

    unsigned int processed = 0;
    unsigned int goodevents = 0;
    for (auto& ev : events) {
        if (ev.p == 0) {
            continue;
        }
        auto nts = ts.updateAndCompute(ev.t, ev.x, ev.y, ev.p);
        cpphots::TimeSurfaceScalarType norm = nts.first.matrix().norm();
        normsum += norm;
        if (nts.second) {
            goodsum += norm;
            goodevents++;
        }
        processed++;
    }

    EXPECT_NEAR(normsum, 4740.313427652784 * 0.5f, 0.1);
    EXPECT_NEAR(goodsum, 4562.696117657931 * 0.5f, 0.1);
    EXPECT_EQ(goodevents, 1783);

}

TEST(TestTimeSurfacePool, FullContext) {

    auto tsp = cpphots::create_pool<cpphots::LinearTimeSurface>(2, 10, 10, 0, 0, 10);

    tsp.update(2, 2, 2, 0);
    auto ctxs = tsp.sampleContexts(2);
    EXPECT_EQ(ctxs.size(), 2);
    EXPECT_NEAR(ctxs[0].sum(), 1.0, 0.001);
    EXPECT_NEAR(ctxs[1].sum(), 0.0, 0.001);

    tsp.update(4, 4, 4, 1);
    ctxs = tsp.sampleContexts(4);
    EXPECT_NEAR(ctxs[0].sum(), 0.8, 0.001);
    EXPECT_NEAR(ctxs[1].sum(), 1.0, 0.001);

}

#ifdef CPPHOTS_ASSERTS
TEST(TestTimeSurfacePool, WrongPolarity) {

    auto tsp = cpphots::create_pool<cpphots::LinearTimeSurface>(2, 10, 10, 0, 0, 10);

    EXPECT_DEATH({
        tsp.updateAndCompute({0, 0, 0, 2});
    }, "Assertion");

}
#endif


class TimeSurfacePoolClone : public ::testing::Test {

protected:

    void SetUp() override {
        orig_pool = cpphots::create_pool<cpphots::LinearTimeSurface>(2, 10, 10, 0, 0, 10);
        orig_ts = orig_pool.getSurface(0);
    }

    cpphots::TimeSurfacePool orig_pool;
    cpphots::TimeSurfacePtr orig_ts;

};

TEST_F(TimeSurfacePoolClone, CopyConstructor) {

    cpphots::TimeSurfacePool pool(orig_pool);
    cpphots::TimeSurfacePtr ts = pool.getSurface(0);
    EXPECT_EQ(pool.getNumSurfaces(), orig_pool.getNumSurfaces());
    EXPECT_NE(ts, orig_ts);

}

TEST_F(TimeSurfacePoolClone, MoveConstructor) {

    size_t n = orig_pool.getNumSurfaces();

    cpphots::TimeSurfacePool pool(std::move(orig_pool));
    cpphots::TimeSurfacePtr ts = pool.getSurface(0);
    EXPECT_EQ(pool.getNumSurfaces(), n);
    EXPECT_EQ(orig_pool.getNumSurfaces(), 0);
    EXPECT_EQ(ts, orig_ts);

}

TEST_F(TimeSurfacePoolClone, CopyAssignment) {

    cpphots::TimeSurfacePool pool;
    pool = orig_pool;
    cpphots::TimeSurfacePtr ts = pool.getSurface(0);
    EXPECT_EQ(pool.getNumSurfaces(), orig_pool.getNumSurfaces());
    EXPECT_NE(ts, orig_ts);

}

TEST_F(TimeSurfacePoolClone, MoveAssignment) {

    size_t n = orig_pool.getNumSurfaces();

    cpphots::TimeSurfacePool pool;
    pool = std::move(orig_pool);
    cpphots::TimeSurfacePtr ts = pool.getSurface(0);
    EXPECT_EQ(pool.getNumSurfaces(), n);
    EXPECT_EQ(orig_pool.getNumSurfaces(), 0);
    EXPECT_EQ(ts, orig_ts);

}