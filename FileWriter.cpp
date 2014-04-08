#include "FileWriter.h"

FileWriter::FileWriter(std::string & filename) : writer(std::ofstream(filename, std::ifstream::binary))
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