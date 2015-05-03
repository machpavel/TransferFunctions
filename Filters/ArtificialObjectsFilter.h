#ifndef ARTIFICIAL_OBJECTS_FILTER
#define ARTIFICIAL_OBJECTS_FILTER

#include "../ItkImageFilter.h"
#include "../Constants.h"

#include <itkSpatialObjectToImageFilter.h>
#include <itkGroupSpatialObject.h>
#include <itkEllipseSpatialObject.h>
#include <itkCylinderSpatialObject.h>
#include <itkVesselTubeSpatialObject.h>
#include <itkMersenneTwisterRandomVariateGenerator.h>

#include <itkMath.h>

template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ArtificialObjectsFilter : public ItkImageFilter<PixelType, Dimension>
{
public:

  typedef itk::EllipseSpatialObject<Dimension> EllipseType;
  typedef itk::CylinderSpatialObject CylinderType;
  typedef itk::GroupSpatialObject<Dimension> GroupType;

  typedef itk::VesselTubeSpatialObject<3> VesselTubeType;
  typedef itk::VesselTubeSpatialObjectPoint<3> VesselTubePointType;
  
  typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType> GaussianFilterType;

  typedef typename itk::SpatialObject<Dimension>::TransformType TransformType;

  typedef itk::SpatialObjectToImageFilter<GroupType, ImageType> SpatialObjectToImageFilterType;

  ArtificialObjectsFilter(typename ImagePointer image) : ItkImageFilter(image), size(this->image->GetLargestPossibleRegion().GetSize())
  {
    this->spacing[0] = 1;
    this->spacing[1] = 1;
    this->spacing[2] = 1;
  }

  virtual ~ArtificialObjectsFilter()
  {
  }

  virtual typename ImagePointer GetFilterImage() override
  {
    SpatialObjectToImageFilterType::Pointer imageFilter = SpatialObjectToImageFilterType::New();

    //  The SpatialObjectToImageFilter requires that the user defines the grid
    //  parameters of the output image. This includes the number of pixels along
    //  each dimension, the pixel spacing, image direction and
    
    imageFilter->SetSize(this->size);
    imageFilter->SetSpacing(this->spacing);

    TransformType::OutputVectorType translation;
    translation[0] = 64;
    translation[1] = -150;
    translation[2] = -50;
    CylinderType::Pointer cylinder1 = this->GetCylinder(translation, 10);

    translation[0] = 196;
    CylinderType::Pointer cylinder2 = this->GetCylinder(translation, 10);

    translation[0] = 128;
    translation[1] = 196;
    translation[2] = 50;
    //EllipseType::Pointer ellipse1 = this->GetEllipse(translation, 2);

    translation[2] = 150;

    const size_t radius = 2;
    EllipseType::ArrayType radiusArray;
    radiusArray[0] = 40 * radius;
    radiusArray[1] = 5 * radius;
    radiusArray[2] = 20 * radius;

    EllipseType::Pointer ellipse2 = this->GetEllipse(translation, radiusArray);



    const size_t xBegin = 128;

    VesselTubeType::Pointer vesselTube = VesselTubeType::New();
    VesselTubeType::PointListType list;
    for (size_t i = 0; i<50; ++i)
    {
      VesselTubePointType p;

      int xDirection = (i % 2 == 0) ? 1 : -1;

      p.SetPosition(xBegin + (xDirection * 5), 50, 10*i);
      p.SetRadius(5);
      p.SetAlpha1(i);
      p.SetAlpha2(i + 3);
      p.SetAlpha3(i + 2);
      p.SetMedialness(10*i);
      p.SetRidgeness(10*i);
      p.SetBranchness(10*i);
      p.SetMark(true);
      p.SetColor(1, 1, 1, 1);
      list.push_back(p);
    }
    vesselTube->GetProperty()->SetName("VesselTube");
    vesselTube->SetId(1);
    vesselTube->SetPoints(list);
    vesselTube->SetDefaultInsideValue(1000);
    vesselTube->SetDefaultOutsideValue(1000);


    GroupType::Pointer group = GroupType::New();
//    group->AddSpatialObject(ellipse1);
    group->AddSpatialObject(ellipse2);
    group->AddSpatialObject(cylinder1);
    group->AddSpatialObject(cylinder2);
    group->AddSpatialObject(vesselTube);

    imageFilter->SetInput(group);

    imageFilter->SetUseObjectValue(true);
    imageFilter->SetOutsideValue(0);
    imageFilter->Update();

    this->image = imageFilter->GetOutput();


    itk::ImageRegionIterator<ImageType> imageIterator(this->image, this->image->GetLargestPossibleRegion());
    ImageType::SizeType size = this->image->GetLargestPossibleRegion().GetSize();

    /*// create gradient on the left cylinder
    while (!imageIterator.IsAtEnd())
    {
      if (imageIterator.GetIndex()[0] < size[2] / 2 && imageIterator.Value() != 0)
      {
        imageIterator.Set(imageIterator.GetIndex()[2] * 10);
      }

      ++imageIterator;
    }*/

    imageIterator.GoToBegin();

    // now create background on the left side
    while (!imageIterator.IsAtEnd())
    {
      if (imageIterator.Value() == 0)
      {
        if (imageIterator.GetIndex()[0] < size[2] / 2)
        {
          imageIterator.Set(500);
        }
        else
        {
          imageIterator.Set(100);
        }
      }

      ++imageIterator;
    }



    // add random noise

    double noiseness = 0;

    std::cout << "Enter noise probability: ";
    std::cin >> noiseness;
    noiseness = 1 - noiseness;

    typedef itk::Statistics::MersenneTwisterRandomVariateGenerator GeneratorType;
    GeneratorType::Pointer generator = GeneratorType::New();
    generator->Initialize();
    while (!imageIterator.IsAtEnd())
    {
      if (generator->GetVariateWithClosedRange() > noiseness)
      {
        imageIterator.Set(generator->GetVariateWithClosedRange() * 2000);
      }

      ++imageIterator;
    }



    /*GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
    gaussianFilter->SetInput(this->image);

    float variance;
    std::cout << "Enter gaussian filter variance: ";
    std::cin >> variance;

    gaussianFilter->SetVariance(variance);
    gaussianFilter->Update();

    this->image = gaussianFilter->GetOutput();*/

    return this->image;
  }

  typename CylinderType::Pointer GetCylinder(typename TransformType::OutputVectorType translation, double radius)
  {
    CylinderType::Pointer cylinder = CylinderType::New();
    cylinder->SetRadius(radius);
    cylinder->SetHeight(2000);
    cylinder->SetDefaultInsideValue(1000);
    cylinder->SetDefaultOutsideValue(1000);

    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    TransformType::OutputVectorType rotationAxes;
    rotationAxes[0] = 10;
    rotationAxes[1] = 0;
    rotationAxes[2] = 0;

    transform->Translate(translation, false);

    transform->Rotate3D(rotationAxes, itk::Math::pi_over_2);

    cylinder->SetObjectToParentTransform(transform);

    return cylinder;
  }

  typename EllipseType::Pointer GetEllipse(typename TransformType::OutputVectorType translation, double radius)
  {
    EllipseType::Pointer ellipse = EllipseType::New();
    EllipseType::ArrayType radiusArray;
    radiusArray[0] = 10 * radius;
    radiusArray[1] = 20 * radius;
    radiusArray[2] = 30 * radius;
    ellipse->SetRadius(radiusArray);

    TransformType::Pointer transform = TransformType::New();

    transform->SetIdentity();
    transform->Translate(translation, false);

    ellipse->SetObjectToParentTransform(transform);
    ellipse->SetDefaultInsideValue(1000);
    ellipse->SetDefaultOutsideValue(1000);

    return ellipse;
  }

  typename EllipseType::Pointer GetEllipse(typename TransformType::OutputVectorType translation, typename EllipseType::ArrayType radius)
  {
    EllipseType::Pointer ellipse = EllipseType::New();
    ellipse->SetRadius(radius);

    TransformType::Pointer transform = TransformType::New();

    transform->SetIdentity();
    transform->Translate(translation, false);

    ellipse->SetObjectToParentTransform(transform);
    ellipse->SetDefaultInsideValue(1000);
    ellipse->SetDefaultOutsideValue(1000);

    return ellipse;
  }

  virtual std::string GetFilterName() override
  {
    return "Artificial objects";
  }

private:
  typename ImageType::SizeType size;
  typename ImageType::SpacingType spacing;
};

#endif // ARTIFICIAL_OBJECTS_FILTER