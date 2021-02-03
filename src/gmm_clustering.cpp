#include "cpphots/gmm_clustering.h"

#include <iomanip>

#include <gmm.hpp>
#include <gmm_algorithms/s_gmm.hpp>
#include <gmm_algorithms/u_s_gmm.hpp>
#include <data_types.hpp>


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

GMMClusterer::GMMClusterer(GMMType type, uint16_t clusters, uint16_t truncated_clusters, uint16_t clusters_considered, TimeSurfaceScalarType eps)
    :type(type), clusters(clusters), truncated_clusters(truncated_clusters), clusters_considered(clusters_considered), eps(eps) {

    set = std::make_shared<dataset<TimeSurfaceScalarType>>();

    reset();

}

uint16_t GMMClusterer::cluster(const TimeSurfaceType& surface) {

    // if learning we just add it to the dataset
    if (learning) {

        if (set->data.rows() == 0) {
            set->data.resize(100, surface.size());
        }

        if (last_data >= set->data.rows()) {
            set->data.resize(2*set->data.rows(), surface.size());
        }

        blaze::row(set->data, last_data) = tsToVector(surface);
        last_data++;

        return 0;  // not valid

    }

    // otherwise we cluster
    uint16_t k = predict(tsToVector(surface));
    updateHistogram(k);
    return k;

}

uint16_t GMMClusterer::getNumClusters() const {
    return clusters;
}

void GMMClusterer::addPrototype(const TimeSurfaceType& proto) {

    // resize blaze matrix if necessary
    if (mean.rows() == 0) {
        mean.resize(clusters, proto.size());
    }

    // update shape if necessary
    if (ts_shape.first == 0) {
        ts_shape = {proto.rows(), proto.cols()};
    }

    blaze::row(mean, last_proto) = tsToVector(proto);
    last_proto++;

}

std::vector<TimeSurfaceType> GMMClusterer::getPrototypes() const {

    std::vector<TimeSurfaceType> prototypes;

    for (size_t i = 0; i < clusters; i++) {
        prototypes.push_back(vectorToTS(blaze::row(mean, i), ts_shape.first, ts_shape.second));
    }

    return prototypes;
}

void GMMClusterer::clearPrototypes() {
    last_proto = 0;
}

bool GMMClusterer::isInitialized() const {
    return last_proto == clusters;
}

bool GMMClusterer::toggleLearning(bool enable) {

    bool old_learning = learning;
    learning = enable;

    if (!enable) {
        // create algorithm

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

        algo->set_variance(std::pow(blaze::stddev(set->data), 2));

        // fit
        fit();

        // delete set
        set = std::make_shared<dataset<TimeSurfaceScalarType>>();

    }

    return old_learning;

}

void GMMClusterer::fit() {

    TimeSurfaceScalarType prv;
    TimeSurfaceScalarType cur;

    int iterations = 0;
    TimeSurfaceScalarType criterion_scaling;

    while (true) {

        algo->em(iterations, 0);
        ++iterations;

        cur = algo->get_free_energy();
        if (iterations == 2) {
            criterion_scaling = std::abs((cur - prv));
        }

        if (eps < 1) {
            if (iterations > 1) {
                // free energy convergence criterion
                if ((std::abs((cur - prv) / cur) < eps * criterion_scaling)) {
                    break;
                }
            }
        } else {
            // epochs
            if (iterations >= eps) {
                break;
            }
        }
        prv = cur;

    }

}

uint16_t GMMClusterer::predict(const BlazeVector& vec, int top_k) {

    blaze::DynamicMatrix<int,blaze::rowMajor> labels;

    blaze::DynamicMatrix<TimeSurfaceScalarType, blaze::rowMajor> inv_covariance;

    // invert covariance matrix via cholesky if using a tied covariance model
    if (!blaze::isEmpty(algo->get_covariance())) {
        inv_covariance = algo->get_covariance();
        blaze::invert<blaze::byLLH>(inv_covariance);
    }


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
    out << last_data << " ";
    out << last_proto << " ";
    out << learning << " ";
    out << std::setprecision(std::numeric_limits<TimeSurfaceScalarType>::max_digits10) << eps << " ";
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
    in >> last_data;
    in >> last_proto;
    in >> learning;
    in >> eps;
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