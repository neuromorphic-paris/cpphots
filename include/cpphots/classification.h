/**
 * @file classification.h
 * @brief Collection of classes to perform classification on the histograms obtained from HOTS
 */

#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include <vector>
#include <string>
#include <unordered_map>
#include <ostream>


namespace cpphots {

/**
 * @brief Features for the classification
 */
using Features = std::vector<uint32_t>;

/**
 * @brief Stream insertion operator for Features
 * 
 * This functions puts the features on the stream, with enclosing square brakets and separated by commas
 * 
 * @param out output stream
 * @param feats some Features
 * @return the stream
 */
std::ostream& operator<<(std::ostream& out, const cpphots::Features& feats);


/**
 * @brief An interface for a generic distance-based classification method
 * 
 * Given a new features this classifier will search the stored set of feature to find the closest one.
 * 
 * The classifier can maintain a list of labels which can be the output of the classification.
 * Alternatively, it can work with class indexes.
 * 
 * Prior to using any of the classify methods, the features should be initialized with #setClassFeatures.
 */
class Classifier {

public:
    /**
     * @brief Construct a new Classifier object
     * 
     * The constructed classifier will only work with class indexes.
     * 
     * @param n_classes the number of classes for the classification task
     */
    explicit Classifier(size_t n_classes);

    /**
     * @brief Construct a new Classifier object
     * 
     * The constructed classifier will work with both class labels and indexes.
     * 
     * @param l_classes a list of class labels
     */
    explicit Classifier(const std::vector<std::string>& l_classes);

    /**
     * @brief Copy-construct a new Classifier object
     * 
     * This constructed classifier will copy the class-features associations from the argument.
     * The argument should be a classifier where the #setClassFeatures method has already been called.
     * 
     * @param other an alrady set-up classifier
     */
    explicit Classifier(Classifier* other);

    /**
     * @brief Assign features to a class
     * 
     * @param cid index of the class
     * @param feats features to assign to the class
     */
    void setClassFeatures(size_t cid, const Features& feats);

    /**
     * @brief Assign features to a class
     * 
     * @param clabel label of the class
     * @param feats features to assign to the class
     */
    void setClassFeatures(const std::string& clabel, const Features& feats);

    /**
     * @brief Classify features
     * 
     * This method outputs the ID of the predicted class.
     * 
     * @param feats features
     * @return index the predicted class
     */
    size_t classifyID(const Features& feats) const;

    /**
     * @brief Classify features
     * 
     * This method outputs the label of the predicted class.
     * This method will raise an erro if the Classifier has not been constructed using labels.
     * 
     * @param feats features
     * @return label of the predicted class
     */
    std::string classifyName(const Features& feats) const;

private:
    std::vector<Features> class_feats;
    std::vector<std::string> class_names;
    std::unordered_map<std::string, size_t> reverse_class_names;

    virtual double computeDistance(const Features& f1, const Features& f2) const = 0;

};


/**
 * @brief Euclidean distance classifier
 * 
 * This classifier uses the L2 Euclidean norm to compute the distance between features.
 */
class StandardClassifier : public Classifier {

public:
    using Classifier::Classifier;

private:
    double computeDistance(const Features& f1, const Features& f2) const override;

};


/**
 * @brief Normalized distance classifier
 * 
 * This classifier uses an Euclidean distance, normalized on the total values of the features, to compute the distance between features.
 */
class NormalizedClassifier : public Classifier {

public:
    using Classifier::Classifier;

private:
    double computeDistance(const Features& f1, const Features& f2) const override;

};


/**
 * @brief Bhattacharyya distance classifier
 * 
 * This classifier uses the Bhattacharyya distance to compute the distance between features.
 */
class BhattacharyyaClassifier : public Classifier {

public:
    using Classifier::Classifier;

private:
    double computeDistance(const Features& f1, const Features& f2) const override;

};

}

#endif