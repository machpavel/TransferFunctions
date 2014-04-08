#ifndef ITK_IMAGE_H_
#define ITK_IMAGE_H_

#include "itkImage.h"

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class CustomItkImage : public itk::Image<PixelType, Dimension>
{
public:
  CustomItkImage() : itk::Image<PixelType, Dimension>() { }
};


#endif // ITK_IMAGE_H_