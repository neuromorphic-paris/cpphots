#include "cpphots/clustering/gmm.h"

#include <iomanip>

#include <gmm.hpp>
#include <gmm_algorithms/s_gmm.hpp>
#include <gmm_algorithms/u_s_gmm.hpp>
#include <data_types.hpp>

#include "cpphots/assert.h"


namespace cpphots {

//////////////////////////
// Blaze-stream management
char peekNext(std::istream& in) {

    char ch = in.peek();

    while (std::isspace(ch)) {
        in.get();
        ch = in.peek();
    }

    return ch;

}

void matrixToStream(std::ostream& out, const BlazeMatrix& mat, bool writesize = true) {

    if (blaze::isEmpty(mat)) {
        out << 'X' << std::endl;
        return;
    }

    if (writesize)
        out << mat.rows() << " " << mat.columns() << std::endl;

    for (uint16_t i = 0; i < mat.rows(); i++) {
        for (size_t j = 0; j < mat.columns(); j++) {
            out << mat(i, j) << " ";
        }
        out << std::endl;
    }

}

BlazeMatrix matrixFromStream(std::istream& in, size_t rows, size_t cols) {

    BlazeMatrix mat;

    mat.resize(rows, cols);
    for (uint16_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            in >> mat(i, j);
        }
    }

    return mat;

}

BlazeMatrix matrixFromStream(std::istream& in) {

    char ch = peekNext(in);

    if (ch == 'X') {
        in.get();
        return BlazeMatrix();
    }

    size_t rows, cols;
    in >> rows;
    in >> cols;

    return matrixFromStream(in, rows, cols);

}

void vectorToStream(std::ostream& out, const BlazeVector& vec, bool writesize = true) {

    if (blaze::isEmpty(vec)) {
        out << 'X' << std::endl;
        return;
    }

    if (writesize)
        out << vec.size() << std::endl;

    for (size_t i = 0; i < vec.size(); i++) {
        out << vec[i] << " ";
    }
    out << std::endl;

}

BlazeVector vectorFromStream(std::istream& in, size_t size) {

    BlazeVector vec;
    vec.resize(size);

    for (uint16_t i = 0; i < size; i++) {
        in >> vec[i];
    }

    return vec;

}

BlazeVector vectorFromStream(std::istream& in) {

    char ch = peekNext(in);

    if (ch == 'X') {
        in.get();
        return BlazeVector();
    }

    size_t size;
    in >> size;

    return vectorFromStream(in, size);

}


//////////////////////////
// Blaze-Eigen conversions
BlazeVector tsToVector(const TimeSurfaceType& surface) {

    BlazeVector vec(surface.size());

    for (size_t i = 0; i < surface.rows(); i++) {
        for (size_t j = 0; j < surface.cols(); j++) {
            vec[i * surface.cols() + j] = surface(i,j);
        }
    }

    return vec;

}

TimeSurfaceType vectorToTS(const BlazeVector& vec, size_t r, size_t c) {

    TimeSurfaceType ts(r, c);

    for (size_t i = 0; i < r; i++) {
        for (size_t j = 0; j < c; j++) {
            ts(i, j) = vec[i * c + j];
        }
    }

    return ts;

}


GMMClusterer::GMMClusterer() {}

GMMClusterer::GMMClusterer(GMMType type, uint16_t clusters, uint16_t truncated_clusters, uint16_t clusters_considered, TimeSurfaceScalarType eps, unsigned int max_iterations)
    :type(type), clusters(clusters), truncated_clusters(truncated_clusters), clusters_considered(clusters_considered), eps(eps), max_iterations(max_iterations) {

    set = std::make_shared<dataset<TimeSurfaceScalarType>>();

    reset();

}

uint16_t GMMClusterer::cluster(const TimeSurfaceType& surface) {

    ClustererOfflineMixin::cluster(surface);

    if (isLearning()) {
        return 0;
    }

    // otherwise we cluster
    cpphots_assert(algo);
    uint16_t k = predict(tsToVector(surface));
    updateHistogram(k);
    return k;

}

uint16_t GMMClusterer::getNumClusters() const {
    return clusters;
}

void GMMClusterer::addCentroid(const TimeSurfaceType& centroid) {

    // resize blaze matrix if necessary
    if (mean.rows() == 0) {
        mean.resize(clusters, centroid.size());
    }

    // update shape if necessary
    if (ts_shape.first == 0) {
        ts_shape = {centroid.rows(), centroid.cols()};
    }

    blaze::row(mean, last_centroid) = tsToVector(centroid);
    last_centroid++;

}

std::vector<TimeSurfaceType> GMMClusterer::getCentroids() const {

    std::vector<TimeSurfaceType> centroids;

    for (size_t i = 0; i < clusters; i++) {
        centroids.push_back(vectorToTS(blaze::row(mean, i), ts_shape.first, ts_shape.second));
    }

    return centroids;
}

void GMMClusterer::clearCentroids() {
    last_centroid = 0;
}

bool GMMClusterer::hasCentroids() const {
    return last_centroid == clusters;
}

void GMMClusterer::train(const std::vector<TimeSurfaceType>& tss) {

    if (tss.size() == 0) {
        return;
    }

    // convert to blaze
    set->data.resize(tss.size(), tss[0].size());
    for (size_t i = 0; i < tss.size(); i++) {
        blaze::row(set->data, i) = tsToVector(tss[i]);
    }

    set->weight = blaze::uniform<blaze::rowVector>(set->data.rows(), 1.0);
    set->shape = {set->data.rows(), set->data.columns()};

    switch (type) {

    case S_GMM:
        algo = std::make_shared<S_gmm<TimeSurfaceScalarType>>(*set, mean, truncated_clusters, 1, 1.0, clusters_considered, 0);
        break;

    case U_S_GMM:
        algo = std::make_shared<u_S_gmm<TimeSurfaceScalarType>>(*set, mean, truncated_clusters, 1, 1.0, clusters_considered, 0);
        break;

    default:
        throw std::runtime_error("Wrong GMM type");
    }

    algo->set_variance(std::pow(blaze::stddev(set->data), 2));

    // fit
    fit();

    // delete set
    set = std::make_shared<dataset<TimeSurfaceScalarType>>();

}

void GMMClusterer::fit() {

    TimeSurfaceScalarType prv = 0.0;
    TimeSurfaceScalarType cur = 0.0;
    TimeSurfaceScalarType change = 0.0;

    int iterations = 0;

    while (true) {

        algo->em(iterations, 0, eps, change);
        ++iterations;

        cur = algo->get_free_energy();

        // free energy convergence criterion
        change = std::abs((cur - prv) / cur);
        if (iterations > 1 && change < eps) {
            break;
        }

        // epochs
        if (iterations >= max_iterations) {
            break;
        }

        prv = cur;

    }

}

uint16_t GMMClusterer::predict(const BlazeVector& vec, int top_k) {

    size_t M = mean.rows();
    size_t D = mean.columns();

    std::vector<int> best_m(top_k);

    if (blaze::isEmpty(algo->get_alpha()) && blaze::isEmpty(algo->get_covariance())) {

        // condition for algorithms with uniform priors
        BlazeVector distances(M);
        for (int m = 0; m < M; m++) {
            distances[m] = blaze::sqrNorm(vec - blaze::row(mean, m));
        }

        // find top k clusters
        for (auto& bm: best_m) {
            bm = static_cast<int>(blaze::argmin(distances));
            distances[bm] = std::numeric_limits<TimeSurfaceScalarType>::max();
        }

    } else if (!blaze::isEmpty(algo->get_alpha()) && blaze::isEmpty(algo->get_covariance())) {

        // condition for algorithms with prior learning
        BlazeVector distances(M);
        for (int m = 0; m < M; m++) {
            distances[m] = blaze::sqrNorm(vec - blaze::row(mean, m));
        }
        distances *= - 0.5 / algo->get_variance();
        TimeSurfaceScalarType lim = blaze::max(distances);
        distances -= lim;
        distances += - 0.5 * D * blaze::log(2.0 * M_PI * algo->get_variance());
        distances += blaze::log(algo->get_alpha());

        // find top k clusters
        for (auto& bm: best_m) {
            bm = static_cast<int>(blaze::argmax(distances));
            distances[bm] = std::numeric_limits<TimeSurfaceScalarType>::lowest();
        }
    } else if (blaze::isEmpty(algo->get_alpha()) && !blaze::isEmpty(algo->get_covariance())) {

        // condition for algorithms with uniform priors and tied covariances
        blaze::DynamicMatrix<TimeSurfaceScalarType, blaze::rowMajor> inv_covariance;

        // invert covariance matrix via cholesky if using a tied covariance model
        if (!blaze::isEmpty(algo->get_covariance())) {
            inv_covariance = algo->get_covariance();
            blaze::invert<blaze::byLLH>(inv_covariance);
        }

        BlazeVector errors(M);
        for (int m = 0; m < M; m++) {
            blaze::DynamicVector<TimeSurfaceScalarType, blaze::rowVector> error = vec - blaze::row(mean, m);
            errors[m] = error * inv_covariance * blaze::trans(error);
        }

        errors *= - 0.5;
        TimeSurfaceScalarType lim = blaze::max(errors);
        errors -= lim;
        errors += - 0.5 * D * blaze::log(2.0 * M_PI * blaze::det(algo->get_covariance()));
        errors -= blaze::log(M);

        // find top k clusters
        for (auto& bm: best_m) {
            bm = static_cast<int>(blaze::argmax(errors));
            errors[bm] = std::numeric_limits<TimeSurfaceScalarType>::lowest();
        }
    }

    return best_m[0];

}

void GMMClusterer::toStream(std::ostream& out) const {

    writeMetacommand(out, "GMMCLUSTERER");

    out << type << " ";
    out << clusters << " ";
    out << clusters_considered << " ";
    out << truncated_clusters << " ";
    out << last_centroid << " ";
    out << learning << " ";
    out << std::setprecision(std::numeric_limits<TimeSurfaceScalarType>::max_digits10) << eps << " ";
    out << max_iterations << " ";
    out << ts_shape.first << " ";
    out << ts_shape.second << std::endl;

    // mean
    matrixToStream(out, mean, false);

    // algo
    if (algo) {
        out << true << " ";
        out << algo->get_variance() << std::endl;
        matrixToStream(out, algo->get_covariance(), true);
        vectorToStream(out, algo->get_alpha(), true);
    } else {
        out << false << std::endl;
    }

}

void GMMClusterer::fromStream(std::istream& in) {

    matchMetacommandOptional(in, "GMMCLUSTERER");

    int type_int;
    in >> type_int;
    type = static_cast<GMMType>(type_int);
    in >> clusters;
    in >> clusters_considered;
    in >> truncated_clusters;
    in >> last_centroid;
    in >> learning;
    in >> eps;
    in >> max_iterations;
    in >> ts_shape.first;
    in >> ts_shape.second;

    // mean
    mean = matrixFromStream(in, clusters, ts_shape.first * ts_shape.second);

    // algo
    bool alg;
    in >> alg;

    if (alg) {

        set = std::make_shared<dataset<TimeSurfaceScalarType>>();

        // recreate GMM algo
        set->data = mean;
        set->weight = blaze::uniform<blaze::rowVector>(set->data.rows(), 1.0);
        set->shape = {set->data.rows(), set->data.columns()};

        switch (type) {

        case S_GMM:
            algo = std::make_shared<S_gmm<TimeSurfaceScalarType>>(*set, mean, truncated_clusters, 1, 1.0, clusters_considered, false);
            break;

        case U_S_GMM:
            algo = std::make_shared<u_S_gmm<TimeSurfaceScalarType>>(*set, mean, truncated_clusters, 1, 1.0, clusters_considered, false);
            break;

        default:
            throw std::runtime_error("Wrong GMM type");
        }

        TimeSurfaceScalarType variance;
        in >> variance;
        algo->set_variance(variance);
        algo->get_covariance() = matrixFromStream(in);
        algo->get_alpha() = vectorFromStream(in);

    }

    reset();

}

}