#ifndef IMAGE_DUMP_BASE_
#define IMAGE_DUMP_BASE_

#include <vector>

class ImageDumpBase
{
public:
  std::vector<int> GetMinimums();
  void SetMinimums(std::vector<int> minimums);
  std::vector<int> GetMaximums();
  void SetMaximums(std::vector<int> maximums);
  std::vector<float> GetElementExtents();
  void SetElementExtents(std::vector<float> elementExtents);
  unsigned int GetDatasetType();
  void SetDatasetType(unsigned int datasetType);
  unsigned int GetElementTypeID();
  void SetElementTypeID(unsigned int elementTypeID);

protected:
  ImageDumpBase() : datasetType(0), elementTypeID(0)
  {
  }

  unsigned int datasetType;
  unsigned int elementTypeID;
  std::vector<int> minimums;
  std::vector<int> maximums;
  std::vector<float> elementExtents;
};

#endif // IMAGE_DUMP_BASE_