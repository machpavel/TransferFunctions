#ifndef FILE_WRITER_H_
#define FILE_WRITER_H_

#include <fstream>
#include <itkVector.h>

template<typename T>
class FileWriter
{
public:
  FileWriter(const std::string & filename) : filename(filename), writer(std::ofstream(this->filename, std::ifstream::binary))
  {
    this->Init();
  }

  void CloseFile()
  {
    this->writer.close();
  }

  void ReopenFile()
  {
    this->writer = std::ofstream(this->filename, std::ifstream::binary);
    this->Init();
  }

  std::string GetFileName()
  {
    return this->filename;
  }

  ~FileWriter()
  {
    this->writer.close();
  }


  void Write(T chunk)
  {
    this->writer.write((char*)&chunk, sizeof(T));
  }

  template<typename OtherType>
  void WriteOtherType(OtherType chunk)
  {
    this->writer.write((char*)&chunk, sizeof(OtherType));
  }

private:
  void Init()
  {
    // writing of endianness; currently not used further
    char endianness = 1;
    this->writer.write(&endianness, sizeof(char));
  }

  std::string filename;
  std::ofstream writer;
};

template<>
void FileWriter<Constants::EigenValuesType>::Write(Constants::EigenValuesType chunk)
{
  for (size_t i = 0; i < Constants::EigenValuesType::GetVectorDimension(); ++i)
  {
    this->writer.write((char*)&chunk[i], sizeof(Constants::EigenValuesType::ValueType));
  }
}

#endif // FILE_WRITER_H_