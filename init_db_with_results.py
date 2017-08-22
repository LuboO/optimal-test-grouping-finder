#! /usr/bin/python3

import os
import re
import sys
from collections import deque


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


def main():
    if len(sys.argv) != 2:
        print("[USAGE] {} <dir-with-reports>".format(sys.argv[0]))
        print("        Script will first drop data in existing database\n"
              "        and fill in with new data from reports in directory\n"
              "        <dir-with-reports>.")
        sys.exit(1)

    directory = sys.argv[1]
    files = sorted(os.listdir(directory))

    re_p_value = re.compile(r"statistic p-value: ([^\s]+)")
    test_split_token = " test results:"
    variant_split_token = "Variant "
    subtest_split_token = "Subtest "

    for f_name in files:
        with open(os.path.join(directory, f_name)) as f:
            f_cont = f.read()

        # Getting separate tests from file
        tests = deque(f_cont.split(test_split_token))
        tests.popleft()

        test_idx = 0
        for test in tests:
            test_idx += 1
            test_id = TEST_IDX_TO_ID_DH[test_idx]
            # Getting separate variants from test
            variants = deque(test.split(variant_split_token))
            if len(variants) > 1:
                variants.popleft()

            variant_idx = 0
            for variant in variants:
                variant_idx += 1
                # Getting separate subtests from variant
                subtests = deque(variant.split(subtest_split_token))
                if len(subtests) > 1:
                    subtests.popleft()

                subtest_idx = 0
                for subtest in subtests:
                    subtest_idx += 1
                    # Getting second level p-values from subtest
                    match = re_p_value.findall(subtest)
                    subtest_p_values = [float(m) for m in match]

                    for p in subtest_p_values:
                        print("Test ID: {}; Variant idx: {}; Subtest idx: {}, P-Value: {}"
                              .format(test_id, variant_idx, subtest_idx, p))


if __name__ == "__main__":
    main()
