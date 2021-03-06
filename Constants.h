#ifndef CONSTANTS_H_
#define CONSTANTS_H_

class Constants
{
public:
  typedef unsigned short GlobalPixelType;

  static const unsigned int DUMP_START_MAGIC_NUMBER = 0xFEEDDEAF;
  static const unsigned int DUMP_HEADER_END_MAGIC_NUMBER = 0xDEADBEAF;
  static const unsigned int DUMP_END_MAGIC_NUMBER = 0x0DEADBEE;
  static const unsigned int DUMP_SLICE_BEGIN_MAGIC_NUMBER= 0xAAAAFFFF;
  static const unsigned int DUMP_SLICE_END_MAGIC_NUMBER = 0xBBBBFFFF;
  static const unsigned int ACTUAL_FORMAT_VERSION = 1;

  static const unsigned int ELEMENT_ID_VECTOR_3_FLOAT = 57;
  static const unsigned int ELEMENT_ID_UNSIGNED_INT = 4;
  static const unsigned int DATASET_TYPE = 10;

  static const GlobalPixelType RANGE_NORMALIZATION_CONSTANT = 1000;

  typedef itk::Vector<float, 3> EigenValuesType;
};

#endif // CONSTANTS_H_