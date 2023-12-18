#pragma once

#include <stdio.h>
#include <assert.h>
#include <string.h>

/// @brief The enumeration of cases whether the file is parced or not
enum partition
{
    NOT_PARTED = 0, ///< No partition, lines_array is nullptr
    PARTED     = 1, ///< Buffer is parted by new line symbols
};

/// @brief Struct for a line
/// It contains pointer to a line and its length excluding null-terminated symbol.
struct line_struct
{
    char* line;
    size_t number_of_elements;
};

/// @brief Container for file input.
/// It contains pointer to a buffer with everything from file, its size,
/// number of lines and pointer to array of lines.
struct file_input
{
    char* buffer;
    size_t buffer_size;
    size_t number_of_lines;
    struct line_struct* lines_array;
};

/// @brief Gets file size.
/// @param fp Pointer of type FILE* to file.
/// @return Size of the buffer.
size_t GetFileSize(FILE* fp);

/// @brief  Provides memory for a buffer.
/// @param buffer_size Number of elements in buffer.
/// @return Pointer to new buffer.
char* MakeBuffer(size_t buffer_size);

/// @brief Reads file to buffer. Calls for GetFileSize() and MakeBuffer() functions.
/// @param file_name Name of file to open and read from.
/// @param buffer_info A struct-container for input from the file.
void ReadFileToBuffer(const char* file_name, struct file_input* buffer_info);

/// @brief Gets number of lines in the buffer. Changes new line symbols into null-terminated symbols.
/// @param buffer_info A struct-container for input from the file.
void GetNumberOfLines(struct file_input* buffer_info);

/// @brief Initializes and fills array of structs line_struct, pointed to buffer's strings.
/// @param buffer_info A struct-container for input from the file.
void MakeArrayOfStrings(struct file_input* buffer_info);

/// @brief Frees all calloced memory and nulls them.
/// @param buffer_info A struct-container for input from the file.
void FreeFileInput(struct file_input* buffer_info);

/// @brief Starts the process of getting file input.
/// Calls for ReadFileToBuffer(), GetNumberOfLines() and (optional) MakeArrayOfStrings.
/// @param file_name Name of file to open and read from.
/// @param buffer_info A struct-container for input from the file.
/// @param is_parted Tells whether the buffer should be parted or not.
void GetFileInput(const char* file_name, struct file_input* buffer_info,
                  enum partition is_separated);

void PrintParray(line_struct* pointer_array, size_t number_of_lines);
