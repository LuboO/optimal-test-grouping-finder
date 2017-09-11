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

    TestGroup(const AtomicTest & atomicTest)
        : TestGroup(std::vector<AtomicTest>({atomicTest}))
    {}

    TestGroup(const std::vector<AtomicTest> & atomicTests)
        : _partialAlpha(calcPartialAlpha(atomicTests.size())),
          _atomicTests(atomicTests)
    {}

    static std::vector<TestGroup> createInitialGroups(const std::vector<AtomicTest> & tests);

    static TestGroup merge(const TestGroup & g1, const TestGroup & g2);

    bool isFail(const uint64_t runIdx) const;

    void printAtomicIndices() const;

private:
    double _partialAlpha;

    std::vector<AtomicTest> _atomicTests;

    static double calcPartialAlpha(size_t groupSize);
};

#endif // TESTGROUP_H
