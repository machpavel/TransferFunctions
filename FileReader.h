#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <fstream>

class FileReader
{
public:
  FileReader(std::string & filename);

  ~FileReader();

  template<typename T>
  T Read()
  {
    T read;
    this->reader.read((char*)&read, sizeof(T));

    return read;
  }

  template<typename T>
  void Read(T* pointer, size_t count)
  {
    this->reader.read((char*)pointer, sizeof(T) * count);
  }

private:
  void Init();

  std::ifstream reader;
};

#endif // FILE_READER_H_