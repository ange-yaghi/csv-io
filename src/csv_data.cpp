#include "../include/csv_data.h"

#include <string.h>
#include <fstream>

atg_csv::CsvData::CsvData() {
    /* void */
}

atg_csv::CsvData::~CsvData() {
    destroy();
}

void atg_csv::CsvData::initialize(int initElements, int initCapacity) {
    m_data = new size_t[initElements];
    m_entryCapacity = initElements;

    m_buffer = new char[initCapacity];
    m_bufferCapacity = initCapacity;

    m_rows = 0;
    m_columns = 0;
    m_writePosition = 0;
    m_writeEntry = 0;
}

void atg_csv::CsvData::write(const char *entry) {
    const size_t l = strlen(entry);
    const size_t i_end = m_writePosition + l;
    if (i_end >= m_bufferCapacity) {
        resize(i_end * 2);
    }

    if (m_writeEntry >= m_entryCapacity) {
        resizeElements((m_entryCapacity + 1) * 2);
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

    m_bufferCapacity = 0;
    m_entryCapacity = 0;
}

atg_csv::CsvData::ErrorCode atg_csv::CsvData::loadCsv(
    const char *fname,
    ErrorInfo *err,
    char del)
{
    std::fstream inputFile(fname, std::ios::in | std::ios::binary);

    if (!inputFile.is_open()) {
        return ErrorCode::CouldNotOpenFile;
    }
    else {
        const ErrorCode result = loadCsv(inputFile, err, del);

        inputFile.close();

        return result;
    }
}

atg_csv::CsvData::ErrorCode atg_csv::CsvData::writeCsv(
    const char *fname,
    ErrorInfo *err,
    char del)
{
    std::fstream outputFile(fname, std::ios::out | std::ios::binary);

    std::istream::sentry se(outputFile, true);
    std::streambuf *sb = outputFile.rdbuf();

    if (!outputFile.is_open() || !se) {
        return ErrorCode::CouldNotOpenFile;
    }
    else {
        for (int i = 0; i < m_rows; ++i) {
            for (int j = 0; j < m_columns; ++j) {
                const char *entry = readEntry(i, j);
                const bool hasQuotes = strchr(entry, '"') != nullptr;

                if (hasQuotes) sb->sputc('\"');

                for (int i = 0; entry[i] != 0; ++i) {
                    if (entry[i] == '\"') {
                        sb->sputn("\"\"", 2);
                    }
                    else {
                        sb->sputc(entry[i]);
                    }
                }

                if (hasQuotes) sb->sputc('\"');

                if (j != m_columns - 1) {
                    sb->sputc(del);
                }
                else if (i != m_rows - 1) {
                    sb->sputc('\n');
                }
            }
        }

        outputFile.close();

        return ErrorCode::Success;
    }
}

void atg_csv::CsvData::resize(size_t newCapacity) {
    char *newBuffer = new char[newCapacity];
    memcpy(newBuffer, m_buffer, m_writePosition * sizeof(char));

    delete[] m_buffer;

    m_buffer = newBuffer;
    m_bufferCapacity = newCapacity;
}

void atg_csv::CsvData::resizeElements(size_t elementCapacity) {
    size_t *newBuffer = new size_t[elementCapacity];
    memcpy(newBuffer, m_data, m_writeEntry * sizeof(size_t));

    delete[] m_data;

    m_data = newBuffer;
    m_entryCapacity = elementCapacity;
}

atg_csv::CsvData::ErrorCode atg_csv::CsvData::loadCsv(
    std::istream &is,
    ErrorInfo *err,
    char del)
{
    enum class State {
        Element,
        QuotedEntry,
        QuotedEntryClosingQuote,
        Entry,
        Done
    };

#define NEXT_LINE() ++line; col = 0;
#define CHECK_COLUMN_COUNT()                                            \
        if (tableColumns == 0) {                                        \
            tableColumns = recordWidth;                                 \
        }                                                               \
        else if (tableColumns != 0 && recordWidth != tableColumns) {    \
            if (err != nullptr) {                                       \
                err->line = line;                                       \
            }                                                           \
            errCode = ErrorCode::InconsistentColumnCount;  break;       \
        }
#define UNEXPECTED_CHARACTER()                                          \
        if (err != nullptr) {                                           \
            err->line = line;                                           \
            err->column = col;                                          \
            errCode = ErrorCode::UnexpectedCharacter;  break;           \
        }
#define UNEXPECTED_EOF()                                                \
        if (err != nullptr) {                                           \
            err->line = line;                                           \
            err->column = col;                                          \
            errCode = ErrorCode::UnexpectedEndOfFile;  break;           \
        }

    std::istream::sentry se(is, true);
    std::streambuf *sb = is.rdbuf();

    initialize(4, 4);

    CharBuffer buffer;
    buffer.initialize(128);

    ErrorCode errCode = ErrorCode::Success;

    int line = 1;
    int col = 0;

    int tableColumns = 0;
    int tableRows = 1;

    int recordWidth = 0;

    State state = State::Element;

    if (se) {
        while (true) {
            State nextState = state;

            ++col;
            const int c = sb->sbumpc();
            if (state == State::Element) {
                if (c == del) {
                    ++recordWidth;

                    nextState = State::Element;

                    write("");
                }
                else if (c == '\n') {
                    ++tableRows;
                    ++recordWidth;

                    nextState = State::Element;

                    write("");

                    CHECK_COLUMN_COUNT();
                    NEXT_LINE();

                    recordWidth = 0;
                }
                else if (c == '\r') {
                    nextState = State::Element;
                }
                else if (c == '"') {
                    ++recordWidth;

                    nextState = State::QuotedEntry;

                    buffer.reset();                    
                }
                else if (c == EOF) {
                    nextState = State::Done;
                    ++recordWidth;

                    CHECK_COLUMN_COUNT();
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
                    nextState = State::Element;

                    buffer.write(0);
                    write(buffer.buffer);
                    buffer.reset();
                }
                else if (c == '\n') {
                    nextState = State::Element;

                    ++tableRows;

                    buffer.write(0);
                    write(buffer.buffer);
                    buffer.reset();

                    CHECK_COLUMN_COUNT();
                    NEXT_LINE();

                    recordWidth = 0;
                }
                else if (c == '\r') {
                    nextState = State::Entry;
                }
                else if (c == '"') {
                    UNEXPECTED_CHARACTER();
                }
                else if (c == EOF) {
                    nextState = State::Done;

                    buffer.write(0);
                    write(buffer.buffer);
                    buffer.reset();

                    CHECK_COLUMN_COUNT();
                }
                else {
                    nextState = State::Entry;

                    buffer.write(c);
                }
            }
            else if (state == State::QuotedEntry) {
                if (c == '"') {
                    nextState = State::QuotedEntryClosingQuote;
                }
                else if (c == EOF) {
                    UNEXPECTED_EOF();
                }
                else if (c == '\r') {
                    nextState = State::QuotedEntry;
                }
                else {
                    nextState = State::QuotedEntry;

                    buffer.write(c);

                    if (c == '\n') {
                        NEXT_LINE();
                    }
                }
            }
            else if (state == State::QuotedEntryClosingQuote) {
                if (c == '"') {
                    nextState = State::QuotedEntry;

                    buffer.write('"');
                }
                else if (c == del) {
                    nextState = State::Element;

                    buffer.write(0);
                    write(buffer.buffer);
                    buffer.reset();
                }
                else if (c == '\n') {
                    nextState = State::Element;

                    ++tableRows;

                    buffer.write(0);
                    write(buffer.buffer);
                    buffer.reset();

                    CHECK_COLUMN_COUNT();
                    NEXT_LINE();

                    recordWidth = 0;
                }
                else if (c == EOF) {
                    nextState = State::Done;

                    buffer.write(0);
                    write(buffer.buffer);

                    CHECK_COLUMN_COUNT();
                }
                else {
                    UNEXPECTED_CHARACTER();
                }
            }
            else if (state == State::Done) {
                break;
            }

            state = nextState;
        }

        if (errCode == ErrorCode::Success) {
            m_rows = tableRows;
            m_columns = tableColumns;
        }
    }

    buffer.destroy();

    return errCode;
}

void atg_csv::CsvData::CharBuffer::initialize(int bufferSize) {
    this->bufferSize = bufferSize;
    this->writeIndex = 0;
    this->buffer = new char[this->bufferSize];
}

void atg_csv::CsvData::CharBuffer::write(char c) {
    if (this->writeIndex >= this->bufferSize) {
        char *newBuffer = new char[((size_t)this->bufferSize + 1) * 2];
        memcpy(newBuffer, this->buffer, this->bufferSize * sizeof(char));

        delete[] this->buffer;
        this->buffer = newBuffer;

        this->bufferSize = (this->bufferSize + 1) * 2;
    }

    this->buffer[this->writeIndex++] = c;
}

void atg_csv::CsvData::CharBuffer::reset() {
    this->writeIndex = 0;
}

void atg_csv::CsvData::CharBuffer::destroy() {
    delete[] this->buffer;

    this->buffer = nullptr;
    this->bufferSize = 0;
}
