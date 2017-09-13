#! /usr/bin/python3

import os
import re
import sys
import configparser
import MySQLdb
import shlex
import subprocess
from collections import deque

DATABASE_ACCESS_CFG = "db_access.ini"
CREATE_TABLES_SCRIPT = "create_data_tables.sql"

TEST_SPLIT_TOKEN = " test results:"
VARIANT_SPLIT_TOKEN = "Variant \d+?:"
SUBTEST_SPLIT_TOKEN = "Subtest \d+?:"
P_VALUE_RE = re.compile(r"statistic p-value: ([^\s]+)")

# Map for translating of test index to test id as used in RTT.
# Following table is specific to Dieharder
TEST_IDX_TO_ID_DH = {
    1: 0,
    2: 1,
    3: 2,
    4: 3,
    5: 4,
    6: 8,
    7: 9,
    8: 10,
    9: 11,
    10: 12,
    11: 13,
    12: 15,
    13: 16,
    14: 17,
    15: 100,
    16: 101,
    17: 102,
    18: 200,
    19: 201,
    20: 202,
    21: 203,
    22: 204,
    23: 205,
    24: 206,
    25: 207,
    26: 208,
    27: 209
}


class DbLoginInfo:
    def __init__(self, db_cfg_path):
        db_cfg = configparser.ConfigParser()

        try:
            db_cfg.read(db_cfg_path)
            if len(db_cfg.sections()) == 0:
                raise FileNotFoundError("can't read: {}".format(db_cfg_path))

            self.name = DbLoginInfo.get_option_or_raise(db_cfg, "Database", "Name")
            self.host = DbLoginInfo.get_option_or_raise(db_cfg, "Database", "Host")
            self.port = int(DbLoginInfo.get_option_or_raise(db_cfg, "Database", "Port"))
            self.user = DbLoginInfo.get_option_or_raise(db_cfg, "Database", "User")
            self.password = DbLoginInfo.get_option_or_raise(db_cfg, "Database", "Password")

        except BaseException as ex:
            raise BaseException("parsing database login info: {}".format(ex))

    @staticmethod
    def get_option_or_raise(cfg, section, option):
        rval = cfg.get(section, option)
        if len(rval) == 0:
            raise ValueError("option {} in section {} is empty"
                             .format(ex))

        return rval


def execute_sql_script(db_info, script_path):
    cmd = "mysql --user={} --password={} --host={} --port={}"\
        .format(db_info.user, db_info.password, db_info.host, db_info.port)
    with open(script_path) as in_script:
        rval = subprocess.call(shlex.split(cmd), stdin=in_script)

    if rval != 0:
        raise RuntimeError("Sql script ({}) execution returned: {}"
                           .format(script_path, rval))


def insert_atomic_test(conn, idx, test_id, variant_idx, subtest_idx, statistic_idx):
    with conn.cursor() as db:
        # Test whether the atomic test is already in the database
        sql_select = "SELECT * FROM atomic_tests WHERE idx=%s"
        db.execute(sql_select, (idx, ))
        if db.rowcount != 0:
            # Do nothing, return
            return

        sql_insert = "INSERT INTO atomic_tests VALUES(%s,%s,%s,%s,%s)"
        db.execute(sql_insert, (idx, test_id, variant_idx, subtest_idx, statistic_idx))


def insert_second_lvl_result(conn, result, run_idx, atomic_test_idx):
    with conn.cursor() as db:
        sql_insert = "INSERT INTO second_lvl_results(result, run_idx, atomic_test_idx) " \
                     "VALUES (%s, %s, %s)"
        db.execute(sql_insert, (result, run_idx, atomic_test_idx))


def main():
    if len(sys.argv) != 2:
        print("[USAGE] {} <dir-with-reports>".format(sys.argv[0]))
        print("        Script will first drop data in existing database\n"
              "        and fill in with new data from reports in directory\n"
              "        <dir-with-reports>.")
        sys.exit(1)

    # noinspection PyBroadException
    try:
        db_info = DbLoginInfo(DATABASE_ACCESS_CFG)
        # Tearing down and creating database anew
        execute_sql_script(db_info, CREATE_TABLES_SCRIPT)
        # Connecting to the database
        conn = MySQLdb.connect(host=db_info.host, port=db_info.port, db=db_info.name,
                               user=db_info.user, passwd=db_info.password)
        conn.autocommit = False
    except Exception as ex:
        print("Database error: {}".format(ex))
        sys.exit(1)

    directory = sys.argv[1]
    files = sorted(os.listdir(directory))
    run_idx = 0
    try:
        for f_name in files:
            with open(os.path.join(directory, f_name)) as f:
                f_cont = f.read()

            run_idx += 1

            # Getting separate tests from file
            tests = deque(f_cont.split(TEST_SPLIT_TOKEN))
            tests.popleft()

            test_idx = 0
            atomic_test_idx = 0
            for test in tests:
                test_idx += 1
                # Other batteries doesn't need idx->id translation
                # test_id = TEST_IDX_TO_ID_DH[test_idx]
                test_id = test_idx
                # Getting separate variants from test
                variants = deque(test.split(VARIANT_SPLIT_TOKEN))
                if len(variants) > 1:
                    variants.popleft()

                variant_idx = 0
                for variant in variants:
                    variant_idx += 1
                    # Getting separate subtests from variant
                    subtests = deque(variant.split(SUBTEST_SPLIT_TOKEN))
                    if len(subtests) > 1:
                        subtests.popleft()

                    subtest_idx = 0
                    for subtest in subtests:
                        subtest_idx += 1
                        # Getting second level p-values from subtest
                        match = P_VALUE_RE.findall(subtest)
                        subtest_p_values = [float(m) for m in match]

                        statistic_idx = 0
                        for result in subtest_p_values:
                            statistic_idx += 1
                            atomic_test_idx += 1
                            # Inserting atomic test (if needed) and second level result
                            insert_atomic_test(conn, atomic_test_idx, test_id,
                                               variant_idx, subtest_idx, statistic_idx)
                            insert_second_lvl_result(conn, result, run_idx, atomic_test_idx)

    except Exception as ex:
        print("Something happened: {}".format(ex))
        conn.rollback()

    conn.commit()
    conn.close()

if __name__ == "__main__":
    main()
