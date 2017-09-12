#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>

#include "mysqldbreader.h"
#include "testgroup.h"
#include "failcountfreqeval.h"
#include "chisquaretwosample.h"

std::vector<uint64_t> makeRange(uint64_t begin, uint64_t end) {
    std::vector<uint64_t> rval;
    for(uint64_t i = begin; i <= end; ++i) {
        rval.push_back(i);
    }
    return rval;
}

std::vector<TestGroup> mergeGroupsAtIndices(const std::vector<TestGroup> & grouping,
                                            size_t idxA, size_t idxB) {
    if(idxA >= grouping.size())
        throw std::out_of_range("idxA");
    if(idxB >= grouping.size())
        throw std::out_of_range("idxB");
    if(idxA >= idxB)
        throw std::runtime_error("idxA >= idxB");

    std::vector<TestGroup> rval(grouping);
    rval[idxA] = TestGroup::merge(grouping.at(idxA), grouping.at(idxB));
    rval.erase(rval.begin() + idxB);

    return rval;
}

typedef FailCountFreqEval<ChiSquareTwoSample> FitnessEval;

std::vector<TestGroup> findOptimalPairing(const std::vector<TestGroup> & grouping) {
    /* optGrouping is set to original grouping at the beginning and changed only
     * if the new tried grouping has better fitness. If not, original grouping is returned. */
    auto optGrouping = grouping;
    auto optFitness = FitnessEval(grouping).getFitness();

    std::vector<TestGroup> currGrouping;
    double currFitness;

    /* Iterating all pairs, looking for the best one. */
    for(size_t idxA = 0; idxA < grouping.size(); ++idxA) {
        for(size_t idxB = idxA + 1; idxB < grouping.size(); ++idxB) {
            currGrouping = mergeGroupsAtIndices(grouping, idxA, idxB);
            currFitness = FitnessEval(currGrouping).getFitness();
            if(currFitness > optFitness) {
                optGrouping = std::move(currGrouping);
                optFitness = currFitness;
            }
        }
    }

    return optGrouping;
}

int main(int argc, char * argv[]) try {
    if(argc != 2) {
        std::cout << "[USAGE] " << argv[0] << " <database-ini-file>" << std::endl;
        return 1;
    }

    /* Loading tests into memory. */
    MySQLDbReader reader(argv[1]);
    auto tests = reader.getDataEntries();

    /* This is our starting point in looking for the best fitness. */
    auto finalGrouping = TestGroup::createInitialGroups(tests);

    size_t finalGroupingSize;
    int ctr = 0;

    for(;;) {
        ++ctr;
        finalGroupingSize = finalGrouping.size();
        finalGrouping = findOptimalPairing(finalGrouping);
        std::cout << "Optimal fitness after " << ctr << " pairing(s): "
                  << FitnessEval(finalGrouping).getFitness() << std::endl;

        /* This is the ending condition. Size of the grouping didn't change,
         * therefore no better grouping was found by pairing any two groups.
         * At this point, the algorithm ends and results are printed. */
        if(finalGrouping.size() == finalGroupingSize) {
            break;
        }
    }
    FitnessEval finalStatistic = FitnessEval(finalGrouping);

    /* Outputting final information */
    std::cout << std::endl;
    std::cout << "Achieved fitness:     " << finalStatistic.getFitness() << std::endl;
    std::cout << "Chi-square statistic: " << finalStatistic.getStatResult() << std::endl;
    std::cout << "Grouping size:        " << finalGrouping.size() << std::endl << std::endl;

    std::cout << "===== Expected failure frequency =====" << std::endl;
    for(const auto & v : finalStatistic.getExpFailCountFreq()) {
        std::cout << v << ", ";
    }
    std::cout << std::endl;
    std::cout << "===== Observed failure frequency =====" << std::endl;
    for(const auto & v : finalStatistic.getObsFailCountFreq()) {
        std::cout << v << ", ";
    }
    std::cout << std::endl << std::endl;

    std::cout << "===================================" << std::endl;
    std::cout << "===== Final grouping of tests =====" << std::endl;
    std::cout << "===================================" << std::endl;
    for(const auto & group : finalGrouping) {
        group.printGroup(std::cout);
    }

    return 0; // exit

} catch(std::exception & ex) {
    std::cout << "[ERROR] " << ex.what() << std::endl;
}


// This is the grouping used in RTT for Dieharder battery.
// It's fitness is around 0.37... Can I find better?
//auto grouping = std::vector<TestGroup>({
//                                         std::vector<uint64_t>({1}),
//                                         std::vector<uint64_t>({2}),
//                                         std::vector<uint64_t>({3}),
//                                         std::vector<uint64_t>({4}),
//                                         std::vector<uint64_t>({5}),
//                                         std::vector<uint64_t>({6}),
//                                         std::vector<uint64_t>({7}),
//                                         std::vector<uint64_t>({8}),
//                                         std::vector<uint64_t>({9}),
//                                         std::vector<uint64_t>({10}),
//                                         std::vector<uint64_t>({11}),
//                                         std::vector<uint64_t>({12, 13}),
//                                         std::vector<uint64_t>({14, 15}),
//                                         std::vector<uint64_t>({16, 17}),
//                                         std::vector<uint64_t>({18}),
//                                         std::vector<uint64_t>({19}),
//                                         std::vector<uint64_t>(makeRange(20, 49)),
//                                         std::vector<uint64_t>(makeRange(50, 61)),
//                                         std::vector<uint64_t>(makeRange(62, 65)),
//                                         std::vector<uint64_t>(makeRange(66, 69)),
//                                         std::vector<uint64_t>(makeRange(70, 102)),
//                                         std::vector<uint64_t>({103}),
//                                         std::vector<uint64_t>({104}),
//                                         std::vector<uint64_t>({105}),
//                                         std::vector<uint64_t>({106, 107}),
//                                         std::vector<uint64_t>({108, 109}),
//                                         std::vector<uint64_t>({110})
//                                     });
