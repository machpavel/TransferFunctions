#include "FileWriter.h"

FileWriter::FileWriter(const std::string & filename) : writer(std::ofstream(filename, std::ifstream::binary)), filename(filename)
{
  this->Init();
}

FileWriter::~FileWriter()
{
  this->writer.close();
}

void FileWriter::Init()
{
  // writing of endianness; currently not used further
  char endianness = 1;
  this->writer.write(&endianness, sizeof(char));
}

void FileWriter::CloseFile()
{
  this->writer.close();
}

void FileWriter::ReopenFile()
{
  this->writer = std::ofstream(this->filename, std::ifstream::binary);
  this->Init();
}