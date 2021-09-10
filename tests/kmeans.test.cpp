#include <random>

#include <cpphots/types.h>
#include <cpphots/clustering/kmeans.h>
#include <cpphots/clustering/utils.h>

#include <gtest/gtest.h>

TEST(TestKmeans, Train) {

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