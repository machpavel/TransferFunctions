#ifndef IMAGE_DUMP_SERIALIZER_H_
#define IMAGE_DUMP_SERIALIZER_H_

#include <sstream>

#include <itkImportImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkPNGImageIOFactory.h>
#include <itkPNGImageIO.h>
#include <itkPNGImageIO.h>
#include <itkAdaptiveHistogramEqualizationImageFilter.h>

#include "Constants.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "ImageDumpBase.h"


template<typename PixelType = Constants::GlobalPixelType, unsigned int Dimension = 3>
class ImageDumpSerializer : public ImageDumpBase
{
public:
  typedef itk::Image<PixelType, Dimension> ImageType;
  typedef typename ImageType::ConstPointer ImageConstPointer;
  typedef typename ImageType::Pointer ImagePointer;
  typedef itk::ImportImageFilter<PixelType, Dimension> ImageFilterType;
  typedef itk::Image<PixelType, 2> SliceType;
  typedef itk::ExtractImageFilter<ImageType, SliceType> FilterType;

  ImageDumpSerializer(const std::string & filename) : writer(FileWriter<PixelType>(filename))
  {
  }

  void SerializeImage(ImagePointer exporter)
  {
    ImageType::RegionType region = exporter->GetLargestPossibleRegion();

    ImageType::SizeType size = region.GetSize();
    ImageType::IndexType index = region.GetIndex();
    ImageType::SpacingType spacing = exporter->GetSpacing();

    this->SerializeHeader(exporter, size, spacing);


    // TODO: Rewrite as a single write operation


    size_t width = size[0];
    size_t height = size[1];
    size_t depth = size[2];

    for (size_t z = index[2]; z < depth + index[2]; ++z)
    {
      for (size_t y = index[1]; y < height + index[1]; ++y)
      {
        for (size_t x = index[0]; x < width + index[0]; ++x)
        {
          itk::Image<PixelType, Dimension>::IndexType index;
          index[0] = x;
          index[1] = y;
          index[2] = z;

          this->writer.Write(exporter->GetPixel(index));
        }
      }
    }

    this->writer.WriteOtherType<unsigned int>(Constants::DUMP_END_MAGIC_NUMBER);
  }

  void WriteImageAsSlices(ImageConstPointer exporter)
  {
    size_t width = this->maximums[0] - this->minimums[0];
    size_t height = this->maximums[1] - this->minimums[1];
    size_t depth = this->maximums[2] - this->minimums[2];

    FilterType::Pointer filter = FilterType::New();

    itk::Image<PixelType, 3>::SizeType sliceSize;
    sliceSize[0] = height;
    sliceSize[1] = width;
    sliceSize[2] = 0;

    itk::PNGImageIO::Pointer pngio;
    typedef  itk::ImageFileWriter<SliceType> WriterType;

    for (size_t z = 0; z < depth; ++z)
    {
      itk::Image<PixelType, 3>::IndexType sliceIndex;
      sliceIndex[0] = 0;
      sliceIndex[1] = 0;
      sliceIndex[2] = z;
      FilterType::InputImageRegionType region;
      region.SetSize(sliceSize);
      region.SetIndex(sliceIndex);
      filter->SetExtractionRegion(region);
      filter->SetInput(exporter);

#if ITK_VERSION_MAJOR >= 4
      filter->SetDirectionCollapseToIdentity(); // This is required.
#endif
      filter->Update();

      SliceType::Pointer sliceImage = filter->GetOutput();

      /*itk::ImageRegionIterator<SliceType> imageIterator(sliceImage, sliceImage->GetLargestPossibleRegion());

      PixelType max = 0;
      for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator)
      {
        if (imageIterator.Value() > max)
        {
          max = imageIterator.Value();
        }
      }

      double multiplier = (double)255 / (double)max;


      for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator)
      {
        PixelType currentValue = imageIterator.Value();
        PixelType newValue = currentValue * multiplier;
        imageIterator.Set(newValue);
      }


      typedef  itk::AdaptiveHistogramEqualizationImageFilter<SliceType> AdaptiveHistogramEqualizationImageFilterType;
      AdaptiveHistogramEqualizationImageFilterType::Pointer adaptiveHistogramEqualizationImageFilter = AdaptiveHistogramEqualizationImageFilterType::New();
      adaptiveHistogramEqualizationImageFilter->SetInput(sliceImage);
      adaptiveHistogramEqualizationImageFilter->SetRadius(1);
      adaptiveHistogramEqualizationImageFilter->Update();

      SliceType::ConstPointer sliceImageEnhanced = adaptiveHistogramEqualizationImageFilter->GetOutput();


      itk::ImageRegionConstIterator<SliceType> imageIterator(sliceImage, sliceImage->GetLargestPossibleRegion());

      while (!imageIterator.IsAtEnd())
      {
        // Get the value of the current pixel
        unsigned char val = imageIterator.Get();
        std::stringstream stream;
        stream << (float)val << " ";
        OutputDebugString(stream.str().c_str());

        ++imageIterator;
      }*/

      WriterType::Pointer writer = WriterType::New();

      std::stringstream filename;
      filename << "e:\\slice" << z << ".png";

      writer->SetFileName(filename.str());
      writer->SetImageIO(pngio);
      writer->SetInput(sliceImage);

      typedef itk::PNGImageIO ImageIOType;
      ImageIOType::Pointer pngImageIO = ImageIOType::New();
      writer->SetImageIO(pngImageIO);
      
      try
      {
        writer->Update();
      }
      catch (itk::ExceptionObject ex)
      {
        std::cout << ex.GetDescription() << std::endl;
      }
    }
  }

  void CloseFile()
  {
    this->writer.CloseFile();
  }

  void ReopenFile()
  {
    this->writer.ReopenFile();
  }

  std::string GetFilename()
  {
    return this->writer.GetFileName();
  }

private:
  void SerializeHeader(ImagePointer exporter, typename ImageType::SizeType size, typename ImageType::SpacingType spacing)
  {
    this->writer.WriteOtherType<unsigned int>(Constants::DUMP_START_MAGIC_NUMBER);

    this->writer.WriteOtherType<unsigned int>(Constants::ACTUAL_FORMAT_VERSION);

    this->writer.WriteOtherType<unsigned int>(this->datasetType);

    this->writer.WriteOtherType<unsigned int>(Dimension);

    this->writer.WriteOtherType<unsigned int>(elementTypeID);

    for (size_t i = 0; i < Dimension; ++i)
    {
      this->writer.WriteOtherType<int>(/*this->minimums[i]*/0);
      this->writer.WriteOtherType<int>(/*this->maximums[i]*/size[i]);
      this->writer.WriteOtherType<float>(/*this->elementExtents[i]*/spacing[i]);
    }

    this->writer.WriteOtherType<unsigned int>(Constants::DUMP_HEADER_END_MAGIC_NUMBER);
  }

  //CustomItkImage<PixelType, Dimension> image;
  FileWriter<PixelType> writer;
};

#endif // IMAGE_DUMP_SERIALIZER_H_