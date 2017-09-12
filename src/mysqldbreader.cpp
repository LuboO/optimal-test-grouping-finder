#include "mysqldbreader.h"

#include <memory>
#include <sstream>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

//std::vector<AtomicTest> MySQLDbReader::getDataEntries() const {
AtomicTestList MySQLDbReader::getDataEntries() const {
    /* Preparing database connection */
    auto driver = get_driver_instance();
    auto con = std::unique_ptr<sql::Connection>(
                driver->connect(_host, _user, _passwd));
    con->setSchema(_name);
    con->setAutoCommit(false);

    std::vector<AtomicTest> tests;

    /* Reading and storing database entries */
    auto stmt = std::unique_ptr<sql::Statement>(con->createStatement());
    auto testsEntries = std::unique_ptr<sql::ResultSet>(
                stmt->executeQuery("SELECT * FROM atomic_tests "
                                   "ORDER BY idx"));

    tests.reserve(testsEntries->rowsCount());

    /* Creating AtomicTest objects without their results */
    while(testsEntries->next()) {
        tests.push_back({testsEntries->getUInt64("idx"),
                         testsEntries->getUInt64("test_id"),
                         testsEntries->getUInt64("variant_idx"),
                         testsEntries->getUInt64("subtest_idx"),
                         testsEntries->getUInt64("statistic_idx")});
    }

    /* Adding results to the atomic tests */
    auto getTestResults = std::unique_ptr<sql::PreparedStatement>(
                con->prepareStatement("SELECT result FROM second_lvl_results "
                                      "WHERE atomic_test_idx=? "
                                      "ORDER BY run_idx"));
    for(auto & test : tests) {
        getTestResults->setUInt64(1, test.getIdx());
        auto testResults = std::unique_ptr<sql::ResultSet>(
                    getTestResults->executeQuery());

        std::vector<double> res;
        res.reserve(testResults->rowsCount());

        while(testResults->next()) {
            res.push_back(testResults->getDouble("result"));
        }
        test.setResults(std::move(res));
    }
    return AtomicTestList(tests);
}
