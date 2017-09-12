#ifndef TESTGROUP_H
#define TESTGROUP_H

#include <stdlib.h>
#include <inttypes.h>
#include <vector>
#include <iostream>

#include "atomictest.h"
#include "constants.h"

class TestGroup {
public:
    TestGroup() = delete;

    TestGroup(const uint64_t & atomicTestIdx,
              const AtomicTestList & atomicTestList)
        : TestGroup(std::vector<uint64_t>({atomicTestIdx}), atomicTestList)
    {}

    TestGroup(const std::vector<uint64_t> & atomicTestIndices,
              const AtomicTestList & atomicTestList)
        : _partialAlpha(calcPartialAlpha(atomicTestIndices.size())),
          _atomicTestIndices(atomicTestIndices),
          _atomicTestList(&atomicTestList)
    {}

    static std::vector<TestGroup> createInitialGroups(const AtomicTestList & tests);

    static TestGroup merge(const TestGroup & g1, const TestGroup & g2);

    size_t getRunsCount() const;

    bool isFail(const uint64_t runIdx) const;

    void printGroup(std::ostream & out) const;

private:
    double _partialAlpha;

    std::vector<uint64_t> _atomicTestIndices;

    const AtomicTestList * _atomicTestList;

    static double calcPartialAlpha(size_t groupSize);
};

#endif // TESTGROUP_H
