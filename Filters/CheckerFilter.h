#ifndef CHECKER_FILTER_H_
#define CHECKER_FILTER_H_

#include <itkExtractImageFilter.h>

#include "../ItkImageFilter.h"
#include "../Constants.h"

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class CheckerFilter : public ItkImageFilter <PixelType, Dimension >
{
public:

  CheckerFilter(ImagePointer image) : ItkImageFilter(image)
  {
  }

  virtual ImagePointer GetFilterImage() override
  {
    std::cout << "enter field size: ";
    size_t fieldSize = 0;
    std::cin >> fieldSize;

    ImageType::RegionType region = this->image->GetLargestPossibleRegion();
    ImageType::SizeType size = region.GetSize();

    size_t width = size[0];
    size_t height = size[1];
    size_t depth = size[2];
    ImageType::IndexType initialPosition = region.GetIndex();

    for (size_t z = initialPosition[2]; z < depth + initialPosition[2]; ++z)
    {
      for (size_t y = initialPosition[1]; y < height + initialPosition[1]; ++y)
      {
        for (size_t x = initialPosition[0]; x < width + initialPosition[0]; ++x)
        {
          itk::Image<PixelType, Dimension>::IndexType index;
          index[0] = x;
          index[1] = y;
          index[2] = z;

          size_t xFieldIndex = (x - initialPosition[0]) / fieldSize;
          size_t yFieldIndex = (y - initialPosition[1]) / fieldSize;
          size_t zFieldIndex = (z - initialPosition[2]) / fieldSize;

          bool xEven = (xFieldIndex % 2) == 0;
          bool yEven = (yFieldIndex % 2) == 0;
          bool zEven = (zFieldIndex % 2) == 0;

          this->image->SetPixel(index, xEven ^ yEven ^ zEven ? PixelType(-5) : PixelType(-2));
        }
      }
    }

    return this->image;
  }

  virtual std::string GetFilterName() override
  {
    return std::string("checker filter");
  }
};

#endif // CHECKER_FILTER_H_