#ifndef FAILCOUNTFREQEVAL_H
#define FAILCOUNTFREQEVAL_H

#include <vector>
#include <stdexcept>
#include <tuple>
#include <boost/math/distributions/binomial.hpp>

#include "constants.h"
#include "testgroup.h"

/**
 * @brief The FailCountFreqEval class Will evaluate the fitness of a given grouping.
 * Compares provided and expected failure count frequency of the grouping using
 * any two sample statistic (with static method calcStatistic). The p-value of the
 * statistic is the resulting fitness of the evaluated grouping.
 */
template <class TTwoSampleTest>
class FailCountFreqEval {
public:
    FailCountFreqEval() = delete;

    FailCountFreqEval<TTwoSampleTest>(
            const std::vector<TestGroup> & grouping,
            const std::vector<AtomicTest> & tests) {
        if(grouping.empty()) {
            throw std::runtime_error("grouping can't be empty");
        }
        if(tests.empty()) {
            throw std::runtime_error("tests can't be empty");
        }
        _runsCount = tests.front().getRunsCount();
        _obsFailCountFreq = calcObsFailCountFreq(grouping, tests, _runsCount);
        _expFailCountFreq = calcExpFailCountFreq(grouping.size(), _runsCount);
        std::tie(_statResult, _statPValue) = TTwoSampleTest::calcStatistic(
                    _obsFailCountFreq, _expFailCountFreq);
    }

    std::vector<double> getFailCountFreq() const {
        return _obsFailCountFreq;
    }

    double getStatResult() const {
        return _statResult;
    }

    double getStatPValue() const {
        return _statPValue;
    }

    double getFitness() const {
        return _statPValue;
    }

private:
    std::vector<double> _obsFailCountFreq;

    std::vector<double> _expFailCountFreq;

    uint64_t _runsCount;

    double _statResult;

    double _statPValue;

    static std::vector<double> calcObsFailCountFreq(
            const std::vector<TestGroup> & grouping,
            const std::vector<AtomicTest> & tests,
            uint64_t runsCount){
        /* Each group can fail at most once. So failure count (n) will
         * be 0 <= n <= grouping.size */
        std::vector<double> failCountFreq(grouping.size() + 1, 0.0);

        /* All tests  in groupings have same count of runs in them -
         * so take the count from the first one. */
        int failCount;
        for(uint64_t runIdx = 1; runIdx <= runsCount; ++runIdx) {
            failCount = 0;
            for(const auto & group : grouping) {
                if(group.isFail(tests, runIdx)) {
                    ++failCount;
                }
            }
            ++failCountFreq[failCount];
        }
        return failCountFreq;
    }

    static std::vector<double> calcExpFailCountFreq(
            uint64_t groupingSize, uint64_t runsCount) {
        using namespace boost::math;

        int n = groupingSize;
        std::vector<double> rval(groupingSize + 1);
        for(int x = 0; x <= n; ++x) {
            rval[x] = runsCount * pdf(binomial(n, Constants::ALPHA), x);
        }
        return rval;
    }
};

#endif // FAILCOUNTFREQEVAL_H
