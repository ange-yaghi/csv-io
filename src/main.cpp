#include "../include/csv_data.h"

#include <string.h>
#include <iostream>

int main() {
    atg_csv::CsvData csv;
    atg_csv::CsvData::ErrorInfo err;

    for (int i = 0; i < 2048; ++i) {
        csv.loadCsv("../test/resources/valid_csv.csv", &err);
        csv.writeCsv("../test/resources/write_test.csv", &err);
        csv.loadCsv("../test/resources/write_test.csv", &err);

        if (csv.m_columns != 3 && csv.m_rows != 2) {
            std::cout << "hello";
        }
    }
}
