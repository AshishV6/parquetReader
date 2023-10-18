#include <iostream>
#include <cassert>
#include <memory>
#include <parquet/api/reader.h>
#include <parquet/types.h>

const char PARQUET_FILENAME[] = "catalog_sales.parquet";
int counter = 0;

int main(int argc, char** argv) {
  try {
    // Creating a ParquetReader instance
    std::unique_ptr<parquet::ParquetFileReader> parquet_reader = parquet::ParquetFileReader::OpenFile(PARQUET_FILENAME, false);

    std::cout << "Parquet file opened successfully!" << std::endl;

    // Getting the File MetaData
    std::shared_ptr<parquet::FileMetaData> file_metadata = parquet_reader->metadata();

    int num_row_groups = file_metadata->num_row_groups();
    assert(num_row_groups == 2);

    int num_columns = file_metadata->num_columns();
    assert(num_columns == 34);

    // Iterating over all the RowGroups in the file
    for (int r = 0; r < num_row_groups; ++r) {
      // Getting the RowGroup Reader
      std::shared_ptr<parquet::RowGroupReader> row_group_reader = parquet_reader->RowGroup(r);

      for (int col_id = 17; col_id < 18; ++col_id) {
        std::shared_ptr<parquet::ColumnReader> column_reader = row_group_reader->Column(col_id);

        parquet::Type::type column_type = column_reader->descr()->physical_type();

        if (column_type == parquet::Type::INT32) {
          std::vector<int32_t> int32_values;

          parquet::Int32Reader* int32_reader = static_cast<parquet::Int32Reader*>(column_reader.get());
          int32_t value;
          while (int32_reader->HasNext()) {
            try {
              int64_t values_read = int32_reader->ReadBatch(1, nullptr, nullptr, &value, &values_read);
              int32_values.push_back(value);
              counter++;
            } catch (const std::exception& e) {
              break;
            }
            std::cout << "INT32 Values:" << std::endl;
            for (const auto& value : int32_values) {
                std::cout << value << " ";
            }
            std::cout << std::endl;
          }
        } else if (column_type == parquet::Type::DOUBLE) {
          std::vector<double> double_values;

          parquet::DoubleReader* double_reader = static_cast<parquet::DoubleReader*>(column_reader.get());
          double value;
          while (double_reader->HasNext()) {
            try {
              int64_t values_read = double_reader->ReadBatch(1, nullptr, nullptr, &value, &values_read);
              double_values.push_back(value);
              counter++;
            } catch (const std::exception& e) {
              break;
            }
            std::cout << "DOUBLE Values:" << std::endl;
            for (const auto& value : double_values) {
                std::cout << value << " ";
            }
            std::cout << std::endl;
          }
        }
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Parquet read error: " << e.what() << std::endl;
    return -1;
  }

  std::cout << counter << std::endl;
  std::cout << "Parquet file Reading Complete" << std::endl;

  return 0;
}
