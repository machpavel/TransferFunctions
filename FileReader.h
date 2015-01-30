#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <fstream>
#include <itkVector.h>

#include "Constants.h"

class FileReader
{
public:
  FileReader(const std::string & filename);

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

  template<typename T>
  void Read(Constants::EigenValuesType* pointer, size_t count)
  {
    Constants::EigenValuesType::ValueType* array = new Constants::EigenValuesType::ValueType[count * Constants::EigenValuesType::GetVectorDimension()];
    this->reader.read((char*)array, sizeof(Constants::EigenValuesType::ValueType) * Constants::EigenValuesType::GetVectorDimension() * count);

    for (size_t i = 0; i < count; ++i, ++pointer)
    {
      *pointer = Constants::EigenValuesType();
      pointer->SetElement(0, array[3*i]);
      pointer->SetElement(1, array[3*i + 1]);
      pointer->SetElement(2, array[3*i + 2]);
    }

    delete[] array;
  }

private:
  void Init();

  std::ifstream reader;
};

#endif // FILE_READER_H_