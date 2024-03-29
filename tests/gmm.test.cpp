#include <random>

#include <cpphots/types.h>
#include <cpphots/clustering/gmm.h>
#include <cpphots/clustering/utils.h>

#include <gtest/gtest.h>


TEST(GMMClustering, Processing) {

    cpphots::GMMClusterer clusterer(cpphots::GMMClusterer::S_GMM, 20, 5, 10, 0.01, 10);

    cpphots::ClustererRandomSeeding(3, 3)(clusterer, {});

    EXPECT_TRUE(clusterer.hasCentroids());

    // train
    std::srand((unsigned int) std::time(0));

    clusterer.toggleLearning(true);

    for (uint16_t i = 0; i < 500; i++) {
        clusterer.cluster(cpphots::TimeSurfaceType::Random(3, 3) + 1.f /2.f);
    }

    clusterer.toggleLearning(false);

    // exec
    for (uint16_t i = 0; i < 1000; i++) {
        clusterer.cluster(cpphots::TimeSurfaceType::Random(3, 3) + 1.f /2.f);
    }

    std::vector<uint32_t> zeros{0, 0, 0, 0, 0};
    EXPECT_NE(clusterer.getHistogram(), zeros);

}

TEST(GMMClustering, SaveLoad) {

    cpphots::GMMClusterer clusterer1(cpphots::GMMClusterer::U_S_GMM, 20, 5, 10, 0.01, 10);

    cpphots::ClustererRandomSeeding(3, 3)(clusterer1, {});

    std::srand((unsigned int) std::time(0));

    clusterer1.toggleLearning(true);

    for (uint16_t i = 0; i < 500; i++) {
        clusterer1.cluster(cpphots::TimeSurfaceType::Random(3, 3) + 1.f /2.f);
    }

    clusterer1.toggleLearning(false);

    std::ostringstream ostream;
    ostream << clusterer1;

    std::istringstream istream(ostream.str());
    cpphots::GMMClusterer clusterer2;

    istream >> clusterer2;

    EXPECT_TRUE(clusterer2.hasCentroids());

    for (uint16_t i = 0; i < 1000; i++) {
        cpphots::TimeSurfaceType ts = cpphots::TimeSurfaceType::Random(3, 3) + 1.f /2.f;
        clusterer1.cluster(ts);
        clusterer2.cluster(ts);
    }

    EXPECT_EQ(clusterer1.getHistogram(), clusterer2.getHistogram());

}

TEST(GMMClustering, Batches) {

    cpphots::GMMClusterer clusterer(cpphots::GMMClusterer::U_S_GMM, 20, 5, 10, 0.01, 10);

    cpphots::ClustererRandomSeeding(3, 3)(clusterer, {});

    // first batch
    clusterer.toggleLearning(true);

    std::srand((unsigned int) std::time(0));

    for (uint16_t i = 0; i < 500; i++) {
        clusterer.cluster(cpphots::TimeSurfaceType::Random(3, 3) + 1.f /2.f);
    }

    clusterer.toggleLearning(false);

    // second batch
    clusterer.toggleLearning(true);

    std::srand((unsigned int) std::time(0));

    for (uint16_t i = 0; i < 500; i++) {
        clusterer.cluster(cpphots::TimeSurfaceType::Random(3, 3) + 1.f /2.f);
    }

    clusterer.toggleLearning(false);

}