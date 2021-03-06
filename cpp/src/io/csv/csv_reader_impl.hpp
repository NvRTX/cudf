/*
 * Copyright (c) 2019, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cudf/cudf.h>

#include "csv_common.h"

#include <string>
#include <vector>

#include <cudf/legacy/table.hpp>
#include <io/utilities/datasource.hpp>
#include <io/utilities/wrapper_utils.hpp>
#include "type_conversion.cuh"

namespace cudf {
namespace io {
namespace csv {

/**---------------------------------------------------------------------------*
 * @brief Class used to parse Json input and convert it into gdf columns
 *---------------------------------------------------------------------------**/
class reader::Impl {
private:
  const reader_options args_;

  device_buffer<char> data;         ///< device: the raw unprocessed CSV data - loaded as a large char * array.
  rmm::device_vector<uint64_t> row_offsets;

  gdf_size_type num_records = 0;    ///< Number of rows with actual data

 // dataframe dimensions
  long num_bits = 0;       ///< The number of 64-bit bitmaps (different than valid).
  int num_active_cols = 0; ///< Number of columns that will be return to user.
  int num_actual_cols = 0; ///< Number of columns in the file --- based on the number of columns in header.

  // Parsing options
  ParseOptions opts{};                            ///< Whole dataset parsing options
  thrust::host_vector<column_parse::flags> h_column_flags;  ///< Per-column parsing flags
  rmm::device_vector<column_parse::flags> d_column_flags;   ///< Per-column parsing flags (device memory)

  rmm::device_vector<SerialTrieNode> d_trueTrie;  ///< device: serialized trie of values to recognize as true
  rmm::device_vector<SerialTrieNode> d_falseTrie; ///< device: serialized trie of values to recognize as false
  rmm::device_vector<SerialTrieNode> d_naTrie;    ///< device: serialized trie of NA values

  // Intermediate data
  std::vector<std::string> col_names; ///< Array of column names.
  std::vector<char> header;           ///< Header row data, for parsing column names.

  // Specifying which part of the file to parse
  size_t byte_range_offset = 0; ///< Offset into the data to start parsing.
  size_t byte_range_size = 0;   ///< Length of the data of interest to parse.
  gdf_size_type nrows = -1;     ///< Number of rows to read. -1 for all rows.
  gdf_size_type skiprows = 0;   ///< Number of rows to skip from the start.
  gdf_size_type skipfooter = 0; ///< Number of rows to skip from the end.

public:
  /**---------------------------------------------------------------------------*
   * @brief Constructor; throws if arguments are not supported
   *---------------------------------------------------------------------------**/
  explicit Impl(reader_options const &args);

  /**---------------------------------------------------------------------------*
   * @brief Parse the input CSV file as specified with the args_ data member
   *
   * @return cudf::table object that contains the array of gdf_columns
   *---------------------------------------------------------------------------**/
  table read();

  /**---------------------------------------------------------------------------*
   * @brief Read and return only the specified range of bytes.
   *
   * Reads the row that starts before or at the end of the range, even if it ends
   * after the end of the range.
   *
   * @param[in] offset Offset of the byte range to read.
   * @param[in] size Size of the byte range to read. Set to zero to read to
   * the end of the file.
   *
   * @return cudf::table object that contains the array of gdf_columns
   *---------------------------------------------------------------------------**/
  table read_byte_range(size_t offset, size_t size);

  /**---------------------------------------------------------------------------*
   * @brief Read and return only the specified range of rows.
   * 
   * Set num_skip_footer to zero when using num_rows parameter.
   *
   * @param[in] num_skip_header Number of rows at the start of the files to skip.
   * @param[in] num_skip_footer Number of rows at the bottom of the file to skip.
   * @param[in] num_rows Number of rows to read. Value of -1 indicates all rows.
   * 
   * @return cudf::table object that contains the array of gdf_columns
   *---------------------------------------------------------------------------**/
  table read_rows(gdf_size_type num_skip_header, gdf_size_type num_skip_footer, gdf_size_type num_rows);

 private:
  /**
   * @brief Finds row positions within the specified input data
   *
   * This function scans the input data to record the row offsets (relative to
   * the start of the input data) and the symbol or character that begins that
   * row. A row is actually the data/offset between two termination symbols.
   *
   * @param[in] h_data Uncompressed input data in host memory
   * @param[in] h_size Number of bytes of uncompressed input data
   **/
  void gather_row_offsets(const char *h_data, size_t h_size);

  /**
   * @brief Filters and discards row positions that are not used
   *
   * @param[in] h_data Uncompressed input data in host memory
   * @param[in] h_size Number of bytes of uncompressed input data
   *
   * @return First and last row positions
   **/
  std::pair<uint64_t, uint64_t> select_rows(const char *h_data, size_t h_size);

  void setColumnNamesFromCsv();

  /**
   * @brief Returns a detected or parsed list of column dtypes
   *
   * @return std::vector<gdf_dtype> List of column dtypes
   **/
  std::pair<std::vector<gdf_dtype>, std::vector<gdf_dtype_extra_info>> gather_column_dtypes();

  /**
   * @brief Converts the row-column data and outputs to gdf_columns
   *
   * @param[in,out] columns List of gdf_columns
   **/
  void decode_data(const std::vector<gdf_column_wrapper> &columns);
};

}  // namespace csv
}  // namespace io
}  // namespace cudf
