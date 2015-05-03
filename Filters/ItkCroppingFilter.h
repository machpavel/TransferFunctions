#ifndef FILTERS_ITK_CROPPING_FILTER_H_
#define FILTERS_ITK_CROPPING_FILTER_H_

#include <itkExtractImageFilter.h>

#include "../ItkImageFilter.h"
#include "../Constants.h"

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ItkCroppingFilter : public ItkImageFilter<PixelType, Dimension>
{
public:
  typedef itk::ExtractImageFilter<ImageType, ImageType> ExtractImageFilterType;
  //typedef itk::ImageRegion< Dimension > RegionType;

  ItkCroppingFilter(typename ImagePointer image) : ItkImageFilter(image)
  {
  }

  typename ImagePointer GetCroppingFilterImage()
  {
    std::cout << "enter bottom left corner of the region:" << std::endl;
    std::cout << "x: ";
    size_t x = 0;
    std::cin >> x;
    std::cout << "y: ";
    size_t y = 0;
    std::cin >> y;
    std::cout << "z: ";
    size_t z = 0;
    std::cin >> z;

    ExtractImageFilterType::InputImageRegionType::IndexType index;
    index[0] = x;
    index[1] = y;
    index[2] = z;

    std::cout << "enter size of the region:" << std::endl;
    std::cout << "x: ";
    x = 0;
    std::cin >> x;
    std::cout << "y: ";
    y = 0;
    std::cin >> y;
    std::cout << "z: ";
    z = 0;
    std::cin >> z;

    ExtractImageFilterType::InputImageRegionType::SizeType size;
    size[0] = x;
    size[1] = y;
    size[2] = z;

    ExtractImageFilterType::InputImageRegionType region(index, size);

    ExtractImageFilterType::Pointer extractionImageFilter = ExtractImageFilterType::New();
    extractionImageFilter->SetExtractionRegion(region);
    extractionImageFilter->SetInput(this->image);
    extractionImageFilter->SetInput(this->image.GetPointer());
    extractionImageFilter->Update();

    return extractionImageFilter->GetOutput();
  }

  virtual ~ItkCroppingFilter()
  {
  }

  virtual typename ImagePointer GetFilterImage() override
  {
    return this->GetCroppingFilterImage();
  }

  virtual std::string GetFilterName() override
  {
    return "Cropping";
  }
};

#endif // FILTERS_ITK_CROPPING_FILTER_H_