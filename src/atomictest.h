#ifndef ATOMICTEST_H
#define ATOMICTEST_H

#include <inttypes.h>
#include <stdexcept>
#include <vector>
#include <iostream>

class AtomicTest {
public:
    AtomicTest() = delete;

    AtomicTest(uint64_t idx, uint64_t testId, uint64_t variantIdx,
               uint64_t subtestIdx, uint64_t statisticIdx)
        : _idx(idx), _testId(testId), _variantIdx(variantIdx),
          _subtestIdx(subtestIdx), _statisticIdx(statisticIdx)
    {}

    uint64_t getIdx() const {
        return _idx;
    }

    uint64_t getTestId() const{
        return _testId;
    }

    double getResult(const uint64_t & runIdx) const {
        return _results.at(runIdx - 1);
    }

    std::vector<double> getResults() const {
        return _results;
    }

    uint64_t getRunsCount() const {
        return _results.size();
    }

    void setResults(const std::vector<double> & results) {
        _results = results;
    }

    void print(std::ostream & out) const {
        out << "Atomic IDX: " << _idx << "; "
            << "Test ID: " << _testId << "; "
            << "Variant IDX: " << _variantIdx << "; "
            << "Subtest IDX: " << _subtestIdx << "; "
            << "Statistic IDX: " << _statisticIdx << std::endl;
    }

private:
    uint64_t _idx;
    uint64_t _testId;
    uint64_t _variantIdx;
    uint64_t _subtestIdx;
    uint64_t _statisticIdx;
    std::vector<double> _results;
};

/**
 * @brief The AtomicTestList class Helper class that holds vector of AtomicTests and enforces correct access
 * to the elements based on their indices
 */
class AtomicTestList {
public:
    AtomicTestList() = delete;

    AtomicTestList(const std::vector<AtomicTest> & list)
        : _list(list)
    {
        if(_list.empty())
            throw std::runtime_error("list can't be empty");
    }

    double getAtomicTestResult(const uint64_t & atomicTestIdx, const uint64_t & runIdx) const {
        return getAtomicTestByIdx(atomicTestIdx).getResult(runIdx);
    }

    void printAtomicTest(std::ostream & out, const uint64_t & atomicTestIdx) const {
        getAtomicTestByIdx(atomicTestIdx).print(out);
    }

    size_t getRunsCount() const {
        return _list.front().getRunsCount();
    }

    /* Operator for accessing the underlying vector directly.
     * Only constant reference is returned. */
    friend const std::vector<AtomicTest> & operator*(const AtomicTestList & o) {
        return o._list;
    }

private:
    std::vector<AtomicTest> _list;

    const AtomicTest & getAtomicTestByIdx(const uint64_t & idx) const {
        const auto & t = _list.at(idx - 1);
        /* Preventive sanity check */
        if(t.getIdx() != idx) {
            throw std::runtime_error("AtomicTestList was incorrectly initialized: element "
                                     "on position idx - 1 have _idx != idx");
        }
        return t;
    }
};

#endif // ATOMICTEST_H
