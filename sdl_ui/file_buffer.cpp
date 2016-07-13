#include "file_buffer.h"

#include <fstream>

file_buffer::file_buffer(const std::string &name)
{
    std::ifstream _file{name, std::ios::binary | std::ios::in};
    _file.seekg(0, _file.end);
    _length = _file.tellg();
    _file.seekg(0, _file.beg);
    _buffer = new uint8_t[_length];

    _file.read((char*)_buffer, _length);
    _file.close();
}

file_buffer::~file_buffer() 
{
    delete[] _buffer;
}

uint32_t file_buffer::length()
{
    return _length;
}

