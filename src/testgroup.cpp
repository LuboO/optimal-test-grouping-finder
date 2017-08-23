#include "testgroup.h"

#include <set>
#include <cmath>
#include <stdexcept>

const double TestGroup::ALPHA = 0.01;
const double TestGroup::EPS   = 1e-8;

std::vector<TestGroup> TestGroup::createInitialGroups(const std::vector<AtomicTest> & tests) {
    std::vector<TestGroup> initialGroups;
    initialGroups.reserve(tests.size());

    /* Group with a single atomic test index will be created for each given test */
    for(const auto & test : tests) {
        initialGroups.push_back({test.getIdx()});
    }
    return initialGroups;
}

TestGroup TestGroup::merge(const TestGroup & g1, const TestGroup & g2) {
    /* Merge vectors of indices into single set of unique values */
    std::set<uint64_t> uniqueIndices;
    uniqueIndices.insert(g1._atomicTestIndices.begin(), g1._atomicTestIndices.end());
    uniqueIndices.insert(g2._atomicTestIndices.begin(), g2._atomicTestIndices.end());

    /* Return brand new test group */
    return TestGroup({uniqueIndices.begin(), uniqueIndices.end()});
}

bool TestGroup::isFail(const std::vector<AtomicTest> & tests, const uint64_t runIdx) const {
    double pvalue;
    for(const auto & idx : _atomicTestIndices) {
        /* The tests in the vector are ordered based on the idx.
         * Test on position n-1 will have idx = n */
        pvalue = tests.at(idx - 1).getResult(runIdx);
        /* Evaluate the p-value here. If any p-value is outside
         * of interval [pAlpha, 1) the group is failing. */
        if(pvalue < _partialAlpha - EPS || pvalue > 1 - EPS)
            return true;
    }

    /* There wasn't any failing p-value in this group.
     * Return false as the group is not fail. */
    return false;
}

double TestGroup::calcPartialAlpha(size_t groupSize) {
    /* Sanity check, just in case */
    if(groupSize == 0) {
        throw std::runtime_error("provided vector must contain at least one element");
    }
    /* Partial alpha is calculated based in the test group */
    /* pAlpha = 1 - (1 - Alpha)^(1 / size) */
    double exponent = 1.0 / (double)groupSize;
    return 1.0 - (std::pow(1.0 - ALPHA, exponent));
}
