#include <random>

#include <cpphots/types.h>
#include <cpphots/clustering/kmeans.h>
#include <cpphots/clustering/utils.h>

#include <gtest/gtest.h>


TEST(TestKMeans, Train) {

    cpphots::KMeansClusterer clust(4);

    std::vector<cpphots::TimeSurfaceType> data(400);
    for (size_t i = 0; i < 100; i++) {
        data[i] = cpphots::TimeSurfaceType::Constant(1, 1, 25.0);
        data[i+100] = cpphots::TimeSurfaceType::Constant(1, 1, 50.0);
        data[i+200] = cpphots::TimeSurfaceType::Constant(1, 1, 75.0);
        data[i+300] = cpphots::TimeSurfaceType::Constant(1, 1, 100.0);
    }

    // cpphots::ClustererUniformSeeding(clust, data);
    clust.addCentroid(cpphots::TimeSurfaceType::Constant(1, 1, 25.0));
    clust.addCentroid(cpphots::TimeSurfaceType::Constant(1, 1, 50.0));
    clust.addCentroid(cpphots::TimeSurfaceType::Constant(1, 1, 75.0));
    clust.addCentroid(cpphots::TimeSurfaceType::Constant(1, 1, 100.0));

    EXPECT_TRUE(clust.hasCentroids());

    clust.train(data);

    auto centroids = clust.getCentroids();
    std::vector<int> _cent;

    for (const auto& c : centroids) {
        _cent.push_back(int(c(0, 0)));
    }

    EXPECT_EQ(_cent, std::vector<int>({25, 50, 75, 100}));

}

TEST(TestKMeans, SaveLoad) {

    cpphots::KMeansClusterer clusterer1(20);

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
    cpphots::KMeansClusterer clusterer2;

    istream >> clusterer2;

    EXPECT_TRUE(clusterer2.hasCentroids());

    for (uint16_t i = 0; i < 1000; i++) {
        cpphots::TimeSurfaceType ts = cpphots::TimeSurfaceType::Random(3, 3) + 1.f /2.f;
        clusterer1.cluster(ts);
        clusterer2.cluster(ts);
    }

    EXPECT_EQ(clusterer1.getHistogram(), clusterer2.getHistogram());

}