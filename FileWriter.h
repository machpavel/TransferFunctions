#ifndef FILE_WRITER_H_
#define FILE_WRITER_H_

#include <fstream>

class FileWriter
{
public:
  FileWriter(const std::string & filename);

  ~FileWriter();

  template<typename T>
  void Write(T chunk)
  {
    this->writer.write((char*)&chunk, sizeof(T));
  }

  template<typename T>
  void Write(T* pointer, size_t count)
  {
    this->writer.write((char*)pointer, sizeof(T) * count);
  }

private:
  void Init();

  std::ofstream writer;
};

#endif // FILE_WRITER_H_