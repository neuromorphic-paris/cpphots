#include "cpphots/interfaces/clustering.h"

namespace cpphots {

namespace interfaces {

std::vector<uint32_t> ClustererBase::getHistogram() const {
    return hist;
}

void ClustererBase::reset() {
    hist.clear();
    hist.resize(getNumClusters());
}

void ClustererBase::updateHistogram(uint16_t k) {
    hist[k]++;
}

}

}