#include "../include/csv_data.h"

#include <string.h>
#include <iostream>

int main() {
    constexpr int testSize = 1024;

    atg_csv::CsvData csv;
    csv.initialize(testSize, testSize, testSize * testSize * 17);

    for (int i = 0; i < testSize; ++i) {
        for (int j = 0; j < testSize; ++j) {
            csv.write("Pain is nothing.");
        }
    }

    for (int i = 0; i < testSize; ++i) {
        for (int j = 0; j < testSize; ++j) {
            if (strcmp(csv.readEntry(i, j), "Pain is nothing.") != 0) {
                std::cout << "Fail\n";
            }
        }
    }
}
