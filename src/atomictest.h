#ifndef ATOMICTEST_H
#define ATOMICTEST_H

#include <inttypes.h>
#include <vector>

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

    double getResult(const uint64_t & runIdx) const {
        return _results.at(runIdx - 1);
    }

    std::vector<double> getResults() const {
        return _results;
    }

    void setResults(const std::vector<double> & results) {
        _results = results;
    }

private:
    uint64_t _idx;
    uint64_t _testId;
    uint64_t _variantIdx;
    uint64_t _subtestIdx;
    uint64_t _statisticIdx;
    std::vector<double> _results;
};

#endif // ATOMICTEST_H
