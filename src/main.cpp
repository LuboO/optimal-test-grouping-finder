#include <iostream>
#include <string>

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

uint64_t RUNS = 1000;

int main() try {
    MySQLDbReader reader("db_access.ini");
    auto tests = reader.getDataEntries();
    auto grouping = TestGroup::createInitialGroups(tests);
    /*auto grouping = std::vector<TestGroup>({
                                             std::vector<uint64_t>({1}),
                                             std::vector<uint64_t>({2}),
                                             std::vector<uint64_t>({3}),
                                             std::vector<uint64_t>({4}),
                                             std::vector<uint64_t>({5}),
                                             std::vector<uint64_t>({6}),
                                             std::vector<uint64_t>({7}),
                                             std::vector<uint64_t>({8}),
                                             std::vector<uint64_t>({9}),
                                             std::vector<uint64_t>({10}),
                                             std::vector<uint64_t>({11}),
                                             std::vector<uint64_t>({12, 13}),
                                             std::vector<uint64_t>({14, 15}),
                                             std::vector<uint64_t>({16, 17}),
                                             std::vector<uint64_t>({18}),
                                             std::vector<uint64_t>({19}),
                                             std::vector<uint64_t>(makeRange(20, 49)),
                                             std::vector<uint64_t>(makeRange(50, 61)),
                                             std::vector<uint64_t>(makeRange(62, 65)),
                                             std::vector<uint64_t>(makeRange(66, 69)),
                                             std::vector<uint64_t>(makeRange(70, 102)),
                                             std::vector<uint64_t>({103}),
                                             std::vector<uint64_t>({104}),
                                             std::vector<uint64_t>({105}),
                                             std::vector<uint64_t>({106, 107}),
                                             std::vector<uint64_t>({108, 109}),
                                             std::vector<uint64_t>({110})
                                         });*/


    FailCountFreqEval<ChiSquareTwoSample> fcfe(grouping, tests);
    std::cout << "Fitness:   " << fcfe.getFitness() << std::endl;
    std::cout << "Statistic: " << fcfe.getStatResult() << std::endl;

    /* I know there shouldn't be return, but I am using it for
     * debugging - I need an exit point. */
    return 0;
} catch(std::exception & ex) {
    std::cout << "[ERROR] " << ex.what() << std::endl;
}
