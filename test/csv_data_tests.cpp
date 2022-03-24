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

TEST(CsvDataTests, LoadCsvSanityCheck) {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;
    atg_csv::CsvData::ErrorCode result =
        csv.loadCsv("../../test/resources/valid_csv.csv", &err);

    EXPECT_EQ(result, atg_csv::CsvData::ErrorCode::Success);

    EXPECT_EQ(csv.m_columns, 3);
    EXPECT_EQ(csv.m_rows, 2);

    EXPECT_EQ(strcmp(csv.readEntry(0, 0), "col0"), 0);
    EXPECT_EQ(strcmp(csv.readEntry(0, 1), "col1"), 0);
    EXPECT_EQ(strcmp(csv.readEntry(0, 2), ""), 0);
    EXPECT_EQ(strcmp(csv.readEntry(1, 0), "10"), 0);
    EXPECT_EQ(strcmp(csv.readEntry(1, 1), " with leading/trailing spaces "), 0);
    EXPECT_EQ(strcmp(csv.readEntry(1, 2), " \"with quotes\" "), 0);
}

TEST(CsvDataTests, MissingFile) {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;
    atg_csv::CsvData::ErrorCode result =
        csv.loadCsv("../../test/resources/fake_file.csv", &err);

    EXPECT_EQ(result, atg_csv::CsvData::ErrorCode::CouldNotOpenFile);
}

TEST(CsvDataTests, InconsistentRows0) {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;
    atg_csv::CsvData::ErrorCode result =
        csv.loadCsv("../../test/resources/inconsistent_rows_0.csv", &err);

    EXPECT_EQ(result, atg_csv::CsvData::ErrorCode::InconsistentColumnCount);
    EXPECT_EQ(err.line, 2);
}

TEST(CsvDataTests, InconsistentRows1) {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;
    atg_csv::CsvData::ErrorCode result =
        csv.loadCsv("../../test/resources/inconsistent_rows_1.csv", &err);

    EXPECT_EQ(result, atg_csv::CsvData::ErrorCode::InconsistentColumnCount);
    EXPECT_EQ(err.line, 2);
}

TEST(CsvDataTests, UnexpectedCharacter0) {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;
    atg_csv::CsvData::ErrorCode result =
        csv.loadCsv("../../test/resources/unexpected_character_0.csv", &err);

    EXPECT_EQ(result, atg_csv::CsvData::ErrorCode::UnexpectedCharacter);
    EXPECT_EQ(err.line, 1);
    EXPECT_EQ(err.column, 12);
}

TEST(CsvDataTests, UnexpectedCharacter1) {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;
    atg_csv::CsvData::ErrorCode result =
        csv.loadCsv("../../test/resources/unexpected_character_1.csv", &err);

    EXPECT_EQ(result, atg_csv::CsvData::ErrorCode::UnexpectedCharacter);
    EXPECT_EQ(err.line, 1);
    EXPECT_EQ(err.column, 11);
}

TEST(CsvDataTests, UnexpectedEOF) {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;
    atg_csv::CsvData::ErrorCode result =
        csv.loadCsv("../../test/resources/unexpected_eof.csv", &err);

    EXPECT_EQ(result, atg_csv::CsvData::ErrorCode::UnexpectedEndOfFile);
    EXPECT_EQ(err.line, 2);
    EXPECT_EQ(err.column, 2);
}

TEST(CsvDataTests, SingleColumn) {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;
    atg_csv::CsvData::ErrorCode result =
        csv.loadCsv("../../test/resources/single_column.csv", &err);

    EXPECT_EQ(csv.m_rows, 2);
    EXPECT_EQ(csv.m_columns, 1);

    EXPECT_EQ(result, atg_csv::CsvData::ErrorCode::Success);
}

TEST(CsvDataTests, WriteTest) {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;
    csv.loadCsv("../../test/resources/valid_csv.csv", &err);
    atg_csv::CsvData::ErrorCode result =
        csv.writeCsv("../../test/resources/write_test.csv", &err);
    csv.loadCsv("../../test/resources/write_test.csv", &err);

    EXPECT_EQ(csv.m_columns, 3);
    EXPECT_EQ(csv.m_rows, 2);

    EXPECT_EQ(result, atg_csv::CsvData::ErrorCode::Success);

    EXPECT_EQ(strcmp(csv.readEntry(0, 0), "col0"), 0);
    EXPECT_EQ(strcmp(csv.readEntry(0, 1), "col1"), 0);
    EXPECT_EQ(strcmp(csv.readEntry(0, 2), ""), 0);
    EXPECT_EQ(strcmp(csv.readEntry(1, 0), "10"), 0);
    EXPECT_EQ(strcmp(csv.readEntry(1, 1), " with leading/trailing spaces "), 0);
    EXPECT_EQ(strcmp(csv.readEntry(1, 2), " \"with quotes\" "), 0);
}
