#include "FileReader.h"

FileReader::FileReader(const std::string & filename) : reader(std::ifstream(filename, std::ifstream::binary))
{
  this->Init();
}

FileReader::~FileReader()
{
  this->reader.close();
}

void FileReader::Init()
{
  // reading of endianness; currently not used further
  char endianness = 1;
  this->reader.read(&endianness, sizeof(char));
}