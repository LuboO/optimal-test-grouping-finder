DROP DATABASE IF EXISTS default_exp_results;

CREATE DATABASE IF NOT EXISTS default_exp_results;
USE default_exp_results;

BEGIN;

CREATE TABLE IF NOT EXISTS atomic_tests (
    `idx`           BIGINT UNSIGNED PRIMARY KEY NOT NULL,
    `test_id`       BIGINT UNSIGNED NOT NULL,
    `variant_idx`   BIGINT UNSIGNED NOT NULL,
    `subtest_idx`   BIGINT UNSIGNED NOT NULL,
    `statistic_idx` BIGINT UNSIGNED NOT NULL
) ENGINE = INNODB;

CREATE TABLE IF NOT EXISTS second_lvl_results (
    `id`                  BIGINT UNSIGNED PRIMARY KEY NOT NULL AUTO_INCREMENT,
    `result`              DOUBLE NOT NULL,
    `run_idx`             BIGINT UNSIGNED NOT NULL, 
    `atomic_test_idx`     BIGINT UNSIGNED NOT NULL,
    FOREIGN KEY (`atomic_test_idx`) REFERENCES atomic_tests(`idx`) 
        ON DELETE CASCADE 
        ON UPDATE CASCADE
) ENGINE = INNODB;

COMMIT;
