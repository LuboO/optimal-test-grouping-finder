#include <iostream>
#include <string>

#include "mysqldbreader.h"
#include "testgroup.h"

uint64_t fact(uint64_t n) {
    uint64_t rval = 1;
    for(uint64_t i = 1; i <= n; ++i) {
        rval *= i;
    }
    return rval;
}

uint64_t combin(uint64_t n, uint64_t k) {
    uint64_t rval = 1;
    for(uint64_t i = n; i > (n - k); i--) {
        rval *= i;
    }
    rval /= fact(k);
    return rval;
}

uint64_t RUNS = 1000;

int main() try {
    MySQLDbReader reader("db_access.ini");
    auto tests = reader.getDataEntries();
    auto groups = TestGroup::createInitialGroups(tests);

    /* Here is correctly calculated failure frequency. */
    /* I would finish this, but I am hungry and
     * I am going to watch Terminator 2: The Judgement Day! */
    std::vector<int> failureFrequency(groups.size());
    int failCount;
    for(uint64_t runIdx = 1; runIdx <= RUNS; runIdx++) {
        failCount = 0;
        for(const auto & g : groups) {
            if(g.isFail(tests, runIdx)) {
                ++failCount;
            }
        }
        ++failureFrequency[failCount];
    }

    return 0;
} catch(std::exception & ex) {
    std::cout << "[ERROR] " << ex.what() << std::endl;
}
