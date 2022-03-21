#ifndef ATG_CSV_IO_CSV_DATA_H
#define ATG_CSV_IO_CSV_DATA_H

#include <istream>

namespace atg_csv {
    class CsvData {
        public:
            enum class ErrorCode {
                FileNotFound,
                InsufficientPermissions
            };

            struct Error {
                int line;
                int column;
                const char *msg;
            };

        public:
            CsvData();
            ~CsvData();

            void initialize(int rows, int columns, int initCapacity = 1024);
            void write(const char *entry);
            void destroy();

            void loadCsv(const char *fname, Error *err = nullptr);
            void writeCsv(const char *fname, Error *err = nullptr);

            void resize(size_t newCapacity);

            inline const char *readEntry(int row, int col) const {
                return m_data[row * m_columns + col] + m_buffer;
            }

            inline void setWriteEntry(int row, int col) {
                m_writeEntry = row * m_columns + col;
            }

            inline void setWritePosition(int row, int col) {
                m_writePosition = m_data[row * m_columns + col];
            }

        protected:
            void loadCsv(std::istream &is);
            static size_t readLine(std::istream &is, char **buffer, size_t bufferSize);

        protected:
            size_t *m_data = nullptr;
            int m_rows = 0;
            int m_columns = 0;

            char *m_buffer = nullptr;
            size_t m_bufferCapacity = 0;

            size_t m_writePosition = 0;
            int m_writeEntry = 0;
    };
} /* namespace atg_csv */

#endif /* ATG_CSV_IO_CSV_DATA_H */
