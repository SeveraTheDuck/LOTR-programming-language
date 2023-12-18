#include "FileOpenLib.h"

void PrintParray(line_struct* pointer_array, size_t number_of_lines)
{
    assert(pointer_array);

    for (size_t i = 0; i < number_of_lines; ++i)
    {
        assert(i < number_of_lines);
        printf("%s\n", pointer_array[i].line);
    }
    return;
}

size_t GetFileSize(FILE* fp)
{
    assert(fp);

    fseek(fp, 0, SEEK_END);
    size_t buffer_size = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return buffer_size;
}

char* MakeBuffer(size_t buffer_size)
{
    char* buffer = (char*)calloc(buffer_size + 1, sizeof(char));
    assert(buffer);

    return buffer;
}

void ReadFileToBuffer(const char* file_name, struct file_input* buffer_info)
{
    assert(buffer_info);

    FILE* fp = fopen(file_name, "rb");
    assert(fp);

    buffer_info->buffer_size = GetFileSize(fp);
    buffer_info->buffer      = MakeBuffer(buffer_info->buffer_size);

    fread(buffer_info->buffer, sizeof(char), buffer_info->buffer_size, fp);

    fclose(fp);
    fp = NULL;

    return;
}

void GetNumberOfLines(struct file_input* buffer_info)
{
    assert(buffer_info);
    assert(buffer_info->buffer);

    buffer_info->number_of_lines = 0;
    for (size_t buffer_index = 0; buffer_index < buffer_info->buffer_size; ++buffer_index)
    {
        if (buffer_info->buffer[buffer_index] == ' ')
        {
            ++(buffer_info->number_of_lines);
        }
    }

    if (buffer_info->buffer[buffer_info->buffer_size - 1] != ' ')
    {
        buffer_info->buffer[buffer_info->buffer_size]  = '\0';        // there're buffer_size + 1 elements, so it's
        ++(buffer_info->number_of_lines);                             // legal to call for buffer[buffer_size]
    }

    return;
}

void MakeArrayOfStrings(struct file_input* buffer_info)
{
    assert(buffer_info);
    assert(buffer_info->buffer);

    buffer_info->lines_array = (line_struct*)calloc(buffer_info->number_of_lines, sizeof(line_struct));
    assert(buffer_info->lines_array);

    size_t current_line = 0;
    buffer_info->lines_array[current_line++].line = buffer_info->buffer;

    for (size_t buffer_index = 0; buffer_index < buffer_info->buffer_size; ++buffer_index)
    {
        ++buffer_info->lines_array[current_line - 1].number_of_elements;
        if (buffer_info->buffer[buffer_index] == ' ' ||
            buffer_info->buffer[buffer_index] == '\n')
        {
            buffer_info->buffer[buffer_index] =  '\0';
            if (current_line < buffer_info->number_of_lines)
            {
                --buffer_info->lines_array[current_line - 1].number_of_elements;
                buffer_info->lines_array[current_line++].line = buffer_info->buffer + buffer_index + 1;
            }
        }
    }
    return;
}

void FreeFileInput(struct file_input* buffer_info)
{
    assert(buffer_info);

    if (buffer_info->buffer)
        free(buffer_info->buffer);
    buffer_info->buffer = NULL;

    if (buffer_info->lines_array)
        free(buffer_info->lines_array);
    buffer_info->lines_array = NULL;

    buffer_info = NULL;

    return;
}

void GetFileInput(const char* file_name, struct file_input* buffer_info,
                  enum partition is_separated)
{
    assert(buffer_info);

    ReadFileToBuffer(file_name, buffer_info);
    GetNumberOfLines(buffer_info);

    if (is_separated)
    {
        MakeArrayOfStrings(buffer_info);
    }

    return;
}
