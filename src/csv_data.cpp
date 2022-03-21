#include "../include/csv_data.h"

#include <string.h>

atg_csv::CsvData::CsvData() {
    /* void */
}

atg_csv::CsvData::~CsvData() {
    destroy();
}

void atg_csv::CsvData::initialize(int rows, int columns, int initCapacity) {
    m_rows = rows;
    m_columns = columns;
    m_data = new size_t[(size_t)rows * columns];
    m_buffer = new char[initCapacity];
    m_bufferCapacity = initCapacity;
    m_writePosition = 0;
    m_writeEntry = 0;
}

void atg_csv::CsvData::write(const char *entry) {
    const size_t l = strlen(entry);
    const size_t i_end = m_writePosition + l;
    if (i_end >= m_bufferCapacity) {
        resize(i_end * 2);
    }

    memcpy(m_buffer + m_writePosition, entry, (l + 1) * sizeof(char));
    m_data[m_writeEntry] = m_writePosition;

    m_writePosition += (l + 1);
    ++m_writeEntry;
}

void atg_csv::CsvData::destroy() {
    if (m_buffer == nullptr) return;

    delete[] m_buffer;
    delete[] m_data;

    m_buffer = nullptr;
    m_data = nullptr;
}

void atg_csv::CsvData::loadCsv(const char *fname, Error *err) {
    /* void */
}

void atg_csv::CsvData::writeCsv(const char *fname, Error *err) {
    /* void */
}

void atg_csv::CsvData::resize(size_t newCapacity) {
    char *newBuffer = new char[newCapacity];
    memcpy(newBuffer, m_buffer, m_writePosition * sizeof(char));

    delete[] m_buffer;

    m_buffer = newBuffer;
    m_bufferCapacity = newCapacity;
}

void atg_csv::CsvData::loadCsv(std::istream &is) {
    size_t bufferSize = 4;
    char *buffer = new char[bufferSize];

    int currentLine = 0;

    while (is.peek() != -1) {
        ++currentLine;
        bufferSize = readLine(is, &buffer, bufferSize);
    }
}

size_t atg_csv::CsvData::readLine(std::istream &is, char **buffer, size_t bufferSize) {
    std::istream::sentry se(is, true);
    std::streambuf *sb = is.rdbuf();

    char *&target = *buffer;

    if (se) {
        for (int i = 0;; ++i) {
            if (i == bufferSize - 2) {
                char *newBuffer = new char[bufferSize * 2];
                memcpy(newBuffer, target, bufferSize);
                delete[] target;

                target = newBuffer;
                bufferSize *= 2;
            }

            const int c = sb->sbumpc();
            switch (c) {
            case '\n':
                target[i] = 0;
                return bufferSize;
            case '\r':
                target[i] = 0;
                if (sb->sgetc() == '\n') sb->sbumpc();
                return bufferSize;
            case EOF:
                target[i] = 0;
                if (i == 0) is.setstate(std::ios::eofbit);
                return bufferSize;
            default:
                (*buffer)[i] = static_cast<char>(c);
            }
        }
    }
    else {
        return bufferSize;
    }
}
