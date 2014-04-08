
cmake_minimum_required(VERSION 2.8)

project(TransferFunctions)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
endif()

add_executable(TransferFunctions MACOSX_BUNDLE main.cpp Constants.h CustomItkImage.h FileReader.h FileWriter.h ImageDumpBase.h ImageDumpDeserializer.h ImageDumpSerializer.h FileReader.cpp FileWriter.cpp ImageDumpBase.cpp)

if( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(TransferFunctions ITKReview ${ITK_LIBRARIES})
else( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(TransferFunctions ${ITK_LIBRARIES})
endif( "${ITK_VERSION_MAJOR}" LESS 4 )

