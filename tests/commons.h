#ifndef CPPHOTS_TESTS_COMMONS_H
#define CPPHOTS_TESTS_COMMONS_H

#include <random>

#include <cpphots/events_utils.h>
#include <cpphots/clustering.h>


/**
 * @brief Random event generator
 */
class RandomEventGenerator {

public:

    RandomEventGenerator(uint16_t xmax = std::numeric_limits<uint16_t>::max(),
                         uint16_t ymax = std::numeric_limits<uint16_t>::max(),
                         uint16_t pmax = std::numeric_limits<uint16_t>::max(),
                         uint64_t dt = 0)
        :RandomEventGenerator(0, xmax, 0, ymax, 0, pmax, dt) {}

    RandomEventGenerator(uint16_t xmin, uint16_t xmax,
                         uint16_t ymin, uint16_t ymax,
                         uint16_t pmin, uint16_t pmax,
                         uint64_t dt = 0)
        :dt(dt) {

            distx = std::uniform_int_distribution<uint16_t>(xmin, xmax-1);
            disty = std::uniform_int_distribution<uint16_t>(ymin, ymax-1);
            distp = std::uniform_int_distribution<uint16_t>(pmin, pmax-1);

            if (dt > 0) {
                distt = std::uniform_int_distribution<uint64_t>(0, dt);
            } else {
                distt = std::uniform_int_distribution<uint64_t>(0, std::numeric_limits<uint64_t>::max());
            }

            reset();

        }

    cpphots::event generateEvent() {
        
        cpphots::event ev;

        if (dt > 0) {
            last_t += distt(gen);
            ev.t = last_t;
        } else {
            ev.t = distt(gen);
        }

        ev.x = distx(gen);
        ev.y = disty(gen);
        ev.p = distp(gen);

        return ev;

    }

    cpphots::event operator()(){
        return generateEvent();
    }

    void reset() {
        last_t = 0;
    }

private:    
    uint64_t dt;
    uint16_t last_t;

    std::mt19937 gen = std::mt19937{std::random_device{}()};

    std::uniform_int_distribution<uint16_t> distx, disty, distp;
    std::uniform_int_distribution<uint64_t> distt;

};


/**
 * @brief Predictable clusterer
 */
class MockClusterer : public cpphots::ClustererBase {

public:

    MockClusterer(std::vector<uint16_t> ks)
        :ks(ks) {
        reset();
    }

    MockClusterer(uint16_t maxk) {
        ks.resize(maxk);
        std::iota(ks.begin(), ks.end(), 0);
        reset();
    }

    uint16_t cluster(const cpphots::TimeSurfaceType&) override {
        uint16_t r = ks[next++];
        if (next >= ks.size()) {
            next = 0;
        }
        updateHistogram(r);
        return r;
    }

    uint16_t getNumClusters() const override {
        return *std::max_element(ks.begin(), ks.end()) + 1;
    }

    void addPrototype(const cpphots::TimeSurfaceType& proto) override {}

    std::vector<cpphots::TimeSurfaceType> getPrototypes() const override {
        return {};
    }

    void clearPrototypes() override {}

    bool isInitialized() const override {
        return true;
    }

    bool toggleLearning(bool) override {
        return false;
    }

private:
    std::vector<uint16_t> ks;
    size_t next = 0;

};

#endif