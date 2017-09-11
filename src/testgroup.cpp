#include "testgroup.h"

#include <set>
#include <cmath>
#include <stdexcept>

std::vector<TestGroup> TestGroup::createInitialGroups(const std::vector<AtomicTest> & tests) {
    std::vector<TestGroup> initialGroups;
    initialGroups.reserve(tests.size());

    /* Group with a single atomic test index will be created for each given test */
    for(const auto & test : tests) {
        initialGroups.push_back({test});
    }
    return initialGroups;
}

TestGroup TestGroup::merge(const TestGroup & g1, const TestGroup & g2) {
    /* Merge atomic tests into single set of unique test */
    std::set<AtomicTest> uniqueTests;
    uniqueTests.insert(g1._atomicTests.begin(), g1._atomicTests.end());
    uniqueTests.insert(g2._atomicTests.begin(), g2._atomicTests.end());

    /* Return brand new test group */
    return TestGroup({uniqueTests.begin(), uniqueTests.end()});
}

bool TestGroup::isFail(const uint64_t runIdx) const {
    double pvalue;
    for(const auto & test : _atomicTests) {
        pvalue = test.getResult(runIdx);
        /* Evaluate the p-value here. If any p-value is outside
         * of interval [pAlpha, 1) the group is failing. */
        if(pvalue < _partialAlpha - Constants::EPS || pvalue > 1 - Constants::EPS)
            return true;
    }

    /* There wasn't any failing p-value in this group.
     * Return false as the group is not fail. */
    return false;
}

void TestGroup::printAtomicIndices() const {
    for(const auto & test : _atomicTests) {
        std::cout << test.getIdx() << ", ";
    }
}

double TestGroup::calcPartialAlpha(size_t groupSize) {
    /* Sanity check, just in case */
    if(groupSize <= 0) {
        throw std::runtime_error("provided vector must contain at least one element");
    }
    /* Partial alpha is calculated based in the test group */
    /* pAlpha = 1 - (1 - Alpha)^(1 / size) */
    double exponent = 1.0 / (double)groupSize;
    return 1.0 - (std::pow(1.0 - Constants::ALPHA, exponent));
}
