#ifndef TESTGROUP_H
#define TESTGROUP_H

#include <stdlib.h>
#include <inttypes.h>
#include <vector>

#include "atomictest.h"
#include "constants.h"

class TestGroup {
public:
    TestGroup() = delete;

    TestGroup(const uint64_t & atomicTestIdx)
        : TestGroup(std::vector<uint64_t>({atomicTestIdx}))
    {}

    TestGroup(const std::vector<uint64_t> & atomicTestIndices)
        :_partialAlpha(calcPartialAlpha(atomicTestIndices.size())),
         _atomicTestIndices(atomicTestIndices)
    {}

    static std::vector<TestGroup> createInitialGroups(const std::vector<AtomicTest> & tests);

    static TestGroup merge(const TestGroup & g1, const TestGroup & g2);

    bool isFail(const std::vector<AtomicTest> & tests, const uint64_t runIdx) const;


    std::vector<uint64_t> getAtomicTestIndices() const;

private:
    double _partialAlpha;

    std::vector<uint64_t> _atomicTestIndices;

    static double calcPartialAlpha(size_t groupSize);
};

#endif // TESTGROUP_H
