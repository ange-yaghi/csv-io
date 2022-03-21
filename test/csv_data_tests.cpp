#include <gtest/gtest.h>

#include <string>

#include "../include/csv_data.h"

TEST(CsvDataTests, SanityTest) {
    atg_csv::CsvData csv;
    csv.initialize(1024, 1024);

    for (int i = 0; i < 1024; ++i) {
        for (int j = 0; j < 1024; ++j) {
            csv.write("Pain is nothing.");
        }
    }

    for (int i = 0; i < 1024; ++i) {
        for (int j = 0; j < 1024; ++j) {
            EXPECT_EQ(strcmp(csv.readEntry(i, j), "Pain is nothing."), 0);
        }
    }
}

TEST(CsvDataTests, PerformanceBenchmark) {
    std::string *data = new std::string[1024 * 1024];

    for (int i = 0; i < 1024; ++i) {
        for (int j = 0; j < 1024; ++j) {
            data[i * 1024 + j] = "Pain is nothing.";
        }
    }

    for (int i = 0; i < 1024; ++i) {
        for (int j = 0; j < 1024; ++j) {
            EXPECT_EQ(strcmp(data[i * 1024 + j].c_str(), "Pain is nothing."), 0);
        }
    }
}
