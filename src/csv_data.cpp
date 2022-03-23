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
    enum class State {
        Element,
        QuotedEntry,
        Entry,
        EscapedCharacter,
    };

    std::istream::sentry se(is, true);
    std::streambuf *sb = is.rdbuf();

    CharBuffer buffer;

    const char del = ',';

    int record = 0;
    int recordWidth = 0;

    State state = State::Element;

    if (se) {
        while (true) {
            State nextState = state;

            const int c = sb->sbumpc();
            if (state == State::Element) {
                if (c == del) {
                    ++recordWidth;

                    nextState = State::Element;

                    write("");
                }
                else if (c == '\n') {
                    ++record;

                    recordWidth = 0;
                    nextState = State::Element;

                    write("");
                }
                else if (c == '\r') {
                    nextState = State::Element;
                }
                else if (c == '"') {
                    ++recordWidth;

                    nextState = State::QuotedEntry;

                    buffer.reset();                    
                }
                else {
                    ++recordWidth;

                    nextState = State::Entry;

                    buffer.reset();
                    buffer.write(c);
                }
            }
            else if (state == State::Entry) {
                if (c == del) {
                    ++recordWidth;

                    nextState = State::Entry;

                    buffer.write(0);
                    write(buffer.buffer);
                    buffer.reset();
                }
                else if (c == '\n') {
                    nextState = State::Entry;

                    buffer.write(0);
                    write(buffer.buffer);
                    buffer.reset();
                }
                else if (c == '\r') {
                    nextState = State::Element;
                }
                else if (c == '"') {
                    /* error */
                }

            }
            else if (state == State::QuotedEntry) {

            }
        }
    }
    else {
        return bufferSize;
    }
}

void atg_csv::CsvData::writeToBuffer(char **buffer, int *bufferSize, int c) {

}

bool atg_csv::CsvData::isWhitespace(char c) {
    switch (c) {
    case ' ':
    case '\n':
    case '\r':
    case '\t':
        return true;
    default:
        return false;
    }
}
