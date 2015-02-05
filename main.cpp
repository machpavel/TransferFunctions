#include <iostream>
#include <boost/program_options.hpp>

#include <itkImage.h>
#include <itkImportImageFilter.h>

#include <itkImageFileWriter.h>

#include "ImageDumpDeserializer.h"
#include "ImageDumpSerializer.h"
#include "ItkImageFilter.h"
#include "Filters/FilterDecision.h"

#include "DicomImport.h"

int main(int argc, char * argv[])
{
  // Declare the supported options.
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("input", boost::program_options::value<std::string>(), "input filename")
    ("output", boost::program_options::value<std::string>(), "output filename")
    ("secondary", boost::program_options::value<std::string>(), "secondary dataset filename (used for storing and loading eigenvalues)")
    ("loadDicom", boost::program_options::value<std::string>(), "load DICOM series and store it in a file; no filters applied")
    ;

  boost::program_options::variables_map vm;
  boost::program_options::positional_options_description p;
  p.add("input", 1);
  p.add("output", 1);
  p.add("secondary", 1);
  p.add("loadDicom", 1);

  boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  boost::program_options::notify(vm);    

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  std::string secondaryFilename = vm.count("secondary") ? vm["secondary"].as<std::string>() : std::string("secondary.dump");

  if (vm.count("input") && vm.count("output"))
  {
    if ((vm.count("loadDicom") == 0))
    {
      typedef Constants::GlobalPixelType PixelType;
      const unsigned int Dimension = 3;
      typedef itk::Image<PixelType, Dimension> ImageType;
      typedef itk::ImportImageFilter< PixelType, Dimension >   ImportFilterType;


      ImageDumpDeserializer<> *deserializer = new ImageDumpDeserializer<>(vm["input"].as<std::string>());
      ImageType::Pointer image = deserializer->DeserializeImage();

      ImageDumpSerializer<> *serializer = new ImageDumpSerializer<>(vm["output"].as<std::string>());
      serializer->SetMinimums(deserializer->GetMinimums());
      serializer->SetMaximums(deserializer->GetMaximums());
      serializer->SetElementExtents(deserializer->GetElementExtents());
      serializer->SetDatasetType(deserializer->GetDatasetType());
      serializer->SetElementTypeID(deserializer->GetElementTypeID());

      ItkImageFilter<PixelType, Dimension>* filter = nullptr;

      std::string read;

      do
      {
        std::cout << "enter filter name or type \"exit\"" << std::endl;
        std::getline(std::cin, read);

        if (read != "exit")
        {
          filter = FilterDecision<PixelType, Dimension>::GetFilter(read, image, serializer, secondaryFilename);

          if (filter != nullptr)
          {
            std::cout << "using " << filter->GetFilterName() << " filter" << std::endl;
            image = filter->GetFilterImage();
          }
          else
          {
            std::cout << "given wrong filter name; possible values are:" << std::endl;
            FilterDecision<PixelType, Dimension>::PrintFilterNames();
          }
        }

        //serializer->WriteImageAsSlices(image);
      } while (read != "exit");

      delete filter;

      serializer->SerializeImage(image);

      delete deserializer;
      delete serializer;
    }
    else
    {
      DicomImport<> importer(vm["input"].as<std::string>(), vm["output"].as<std::string>());
      importer.Convert();
    }

    return EXIT_SUCCESS;
  }
  else
  {
    return EXIT_FAILURE;
  }
}
