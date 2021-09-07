/**
 * @file layer.h
 * @brief Implementation of HOTS layers
 */
#ifndef CPPHOTS_LAYER_H
#define CPPHOTS_LAYER_H

#include "assert.h"
#include "types.h"
#include "clustering_utils.h"
#include "interfaces/all.h"


namespace cpphots {

/**
 * @brief Composable layer
 * 
 * This class implements a HOTS layer that can be built from
 * different components.
 * 
 * The four components that can be added must implement one the following interfaces:
 *  - interfaces::TimeSurfacePoolCalculator (required)
 *  - interfaces::Clusterer
 *  - interfaces::EventRemapper
 *  - interfaces::SuperCell
 * 
 * If one of the components is a subclass of interfaces::Clusterer, clustering
 * is also performed on every event.
 * 
 * Layer bahaves as both a interfaces::TimeSurfacePoolCalculator and a interfaces::Clusterer.
 */
class Layer : public virtual interfaces::TimeSurfacePoolCalculator, public virtual interfaces::Clusterer {

public:
    /**
     * @brief Construct a new Layer
     * 
     * All parameters are optional, but a pool must be provided before calling Layer::process.
     * 
     * The Layer takes ownership of all the components.
     * 
     * @param tspool time surface pool calculator
     * @param clusterer clusterer
     * @param remapper event remapper
     * @param supercell supercell modifier
     */
    Layer(interfaces::TimeSurfacePoolCalculator* tspool = nullptr,
          interfaces::Clusterer* clusterer = nullptr,
          interfaces::EventRemapper* remapper = nullptr,
          interfaces::SuperCell* supercell = nullptr);

    /**
     * @brief Destroy the Layer object, deleting its components
     */
    ~Layer();

    /**
     * Copy constructor
     * 
     * @param other layer to be copied
     */
    Layer(const Layer& other);

    /**
     * Move constructor
     * 
     * @param other objelayerct to be moved
     */
    Layer(Layer&& other);

    /**
     * Copy assignment
     * 
     * @param other layer to be copied
     */
    Layer& operator=(const Layer& other);

    /**
     * Move assignment
     * 
     * @param other layer to be moved
     */
    Layer& operator=(Layer&& other);

    /**
     * @brief Add a time surfaces pool calculator
     * 
     * Old one is deleted. The Layer takes ownership of the component.
     * 
     * @param tspool time surface pool calculator
     */
    void addTSPool(interfaces::TimeSurfacePoolCalculator* tspool);

    /**
     * @brief Create a new time surfaces pool object and add it to the layer
     * 
     * @tparam T type of component
     * @tparam Args type of the arguments of the constructor
     * @param args arguments to construct the component
     */
    template<typename T, typename... Args>
    void createTSPool(const Args&... args) {
        delete tspool;

        tspool = new T(args...);

    }

    /**
     * @brief Get the time surfaces pool component, if present
     * 
     * If the component is not present, an exception will be raised.
     * 
     * @return the requested component
     */
    interfaces::TimeSurfacePoolCalculator& getTSPool();

    /**
     * @brief Get the time surfaces pool component, if present
     * 
     * If the component is not present, an exception will be raised.
     * 
     * @return the requested component
     */
    const interfaces::TimeSurfacePoolCalculator& getTSPool() const;

    /**
     * @brief Add a clusterer
     * 
     * Old one is deleted. The Layer takes ownership of the component.
     * 
     * @param clusterer clusterer
     */
    void addClusterer(interfaces::Clusterer* clusterer);

    /**
     * @brief Create a new clusterer and add it to the layer
     * 
     * @tparam T type of component
     * @tparam Args type of the arguments of the constructor
     * @param args arguments to construct the component
     */
    template<typename T, typename... Args>
    void createClusterer(const Args&... args) {
        delete clusterer;

        clusterer = new T(args...);

    }

    /**
     * @brief Get the clusterer component, if present
     * 
     * If the component is not present, an exception will be raised.
     * 
     * @return the requested component
     */
    interfaces::Clusterer& getClusterer();

    /**
     * @brief Get the clusterer component, if present
     * 
     * If the component is not present, an exception will be raised.
     * 
     * @return the requested component
     */
    const interfaces::Clusterer& getClusterer() const;

    /**
     * @brief Add a remapper modifier
     * 
     * Old one is deleted. The Layer takes ownership of the component.
     * 
     * @param remapper event remapper
     */
    void addRemapper(interfaces::EventRemapper* remapper);

    /**
     * @brief Create a new event remapper and add it to the layer
     * 
     * @tparam T type of component
     * @tparam Args type of the arguments of the constructor
     * @param args arguments to construct the component
     */
    template<typename T, typename... Args>
    void createRemapper(const Args&... args) {
        delete remapper;

        remapper = new T(args...);

    }

    /**
     * @brief Get the event remapper component, if present
     * 
     * If the component is not present, an exception will be raised.
     * 
     * @return the requested component
     */
    interfaces::EventRemapper& getRemapper();

    /**
     * @brief Get the event remapper component, if present
     * 
     * If the component is not present, an exception will be raised.
     * 
     * @return the requested component
     */
    const interfaces::EventRemapper& getRemapper() const;

    /**
     * @brief Add a supercell modifier
     * 
     * Old one is deleted. The Layer takes ownership of the component.
     * 
     * @param supercell supercell modifier
     */
    void addSuperCell(interfaces::SuperCell* supercell);

    /**
     * @brief Create a new supercell and add it to the layer
     * 
     * @tparam T type of component
     * @tparam Args type of the arguments of the constructor
     * @param args arguments to construct the component
     */
    template<typename T, typename... Args>
    void createSuperCell(const Args&... args) {
        delete supercell;

        supercell = new T(args...);

    }

    /**
     * @brief Get the supercell component, if present
     * 
     * If the component is not present, an exception will be raised.
     * 
     * @return the requested component
     */
    interfaces::SuperCell& getSuperCell();

    /**
     * @brief Get the supercell component, if present
     * 
     * If the component is not present, an exception will be raised.
     * 
     * @return the requested component
     */
    const interfaces::SuperCell& getSuperCell() const;

    /**
     * @brief Process an event and return zero or more events
     * 
     * @param t time of the event
     * @param x horizontal coordinate of the event
     * @param y vertical coordinate of the event
     * @param p polarity of the event
     * @param skip_check if true consider the event as valid
     * @return the emitted events, possibly empty
     */
    Events process(uint64_t t, uint16_t x, uint16_t y, uint16_t p, bool skip_check = false);

    /**
     * @brief Process an event and return zero or more events
     * 
     * @param ev the event
     * @param skip_check if true consider the event as valid
     * @return the emitted events, possibly empty
     */
    Events process(const event& ev, bool skip_check = false) {
        return process(ev.t, ev.x, ev.y, ev.p, skip_check);
    }

    /**
     * @brief Chech if the layer can cluster time surfaces
     * 
     * @return true if the layer has a clusterer
     * @return false otherwise
     */
    bool canCluster() const;

    // interfaces::TimeSurfacePoolCalculator
    void update(uint64_t t, uint16_t x, uint16_t y, uint16_t p) override {
        tspool->update(t, x, y, p);
    }

    void update(const event& ev) override {
        tspool->update(ev);
    }

    std::pair<TimeSurfaceType, bool> compute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) const override {
        return tspool->compute(t, x, y, p);
    }

    std::pair<TimeSurfaceType, bool> compute(const event& ev) const override {
        return tspool->compute(ev);
    }

    std::pair<TimeSurfaceType, bool> updateAndCompute(uint64_t t, uint16_t x, uint16_t y, uint16_t p) override {
        return tspool->updateAndCompute(t, x, y, p);
    }

    std::pair<TimeSurfaceType, bool> updateAndCompute(const event& ev) override {
        return tspool->updateAndCompute(ev);
    }

    std::pair<uint16_t, uint16_t> getSize() const override {
        return tspool->getSize();
    }

    TimeSurfacePtr& getSurface(size_t idx) override {
        return tspool->getSurface(idx);
    }

    const TimeSurfacePtr& getSurface(size_t idx) const override {
        return tspool->getSurface(idx);
    }

    std::vector<TimeSurfaceType> sampleContexts(uint64_t t) override {
        return tspool->sampleContexts(t);
    }

    size_t getNumSurfaces() override {
        return tspool->getNumSurfaces();
    }

    // interfaces::Clusterer
    uint16_t cluster(const TimeSurfaceType& surface) override {
        cpphots_assert(clusterer != nullptr);
        return clusterer->cluster(surface);
    }

    uint16_t getNumClusters() const override {
        cpphots_assert(clusterer != nullptr);
        return clusterer->getNumClusters();
    }

    void addCentroid(const TimeSurfaceType& centroid) override {
        cpphots_assert(clusterer != nullptr);
        clusterer->addCentroid(centroid);
    }

    std::vector<TimeSurfaceType> getCentroids() const override {
        cpphots_assert(clusterer != nullptr);
        return clusterer->getCentroids();
    }

    void clearCentroids() override {
        cpphots_assert(clusterer != nullptr);
        return clusterer->clearCentroids();
    }

    bool hasCentroids() const override {
        cpphots_assert(clusterer != nullptr);
        return clusterer->hasCentroids();
    }

    bool toggleLearning(bool enable = true) override {\
        cpphots_assert(clusterer != nullptr);
        return clusterer->toggleLearning(enable);
    }

    std::vector<uint32_t> getHistogram() const override {
        cpphots_assert(clusterer != nullptr);
        return clusterer->getHistogram();
    }

    /**
     * @brief Reset the layer
     * 
     * Resets both the time surfaces pool and the clusterer.
     */
    void reset() override;

    /**
     * @copydoc interfaces::Streamable::toStream
     * 
     * Calls toStream on the components that support it.
     */
    void toStream(std::ostream& out) const override;

    /**
     * @copydoc interfaces::Streamable::fromStream
     * 
     * Calls fromStream on the components that support it.
     */
    void fromStream(std::istream& in) override;

    Layer* clone() const override;

private:
    interfaces::TimeSurfacePoolCalculator* tspool = nullptr;
    interfaces::Clusterer* clusterer = nullptr;
    interfaces::EventRemapper* remapper = nullptr;
    interfaces::SuperCell* supercell = nullptr;

    void delete_components();

};


/**
 * @brief Seed centroids from a stream of events
 * 
 * @param seeding the seeding algorithm
 * @param layer Layer to be seeded
 * @param events the stream of events to be used
 * @param valid_only use only valid time surfaces for the seeding
 */
void layerSeedCentroids(const ClustererSeedingType& seeding, Layer& layer, const Events& events, bool valid_only = true);


/**
 * @brief Seed centroids from a vector of streams of events
 * 
 * @param seeding the seeding algorithm
 * @param layer layer to be seeded
 * @param event_streams the vector of streams of events to be used
 * @param valid_only use only valid time surfaces for the seeding
 */
void layerSeedCentroids(const ClustererSeedingType& seeding, Layer& layer, const std::vector<Events>& event_streams, bool valid_only = true);

}

#endif