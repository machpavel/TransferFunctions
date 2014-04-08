#include "ImageDumpBase.h"

std::vector<int> ImageDumpBase::GetMinimums()
{
  return this->minimums;
}

void ImageDumpBase::SetMinimums(std::vector<int> minimums)
{
  this->minimums = minimums;
}

std::vector<int> ImageDumpBase::GetMaximums()
{
  return this->maximums;
}

void ImageDumpBase::SetMaximums(std::vector<int> maximums)
{
  this->maximums = maximums;
}

std::vector<float> ImageDumpBase::GetElementExtents()
{
  return this->elementExtents;
}

void ImageDumpBase::SetElementExtents(std::vector<float> elementExtents)
{
  this->elementExtents = elementExtents;
}

unsigned int ImageDumpBase::GetDatasetType()
{
  return this->datasetType;
}

void ImageDumpBase::SetDatasetType(unsigned int datasetType)
{
  this->datasetType = datasetType;
}

unsigned int ImageDumpBase::GetElementTypeID()
{
  return this->elementTypeID;
}

void ImageDumpBase::SetElementTypeID(unsigned int elementTypeID)
{
  this->elementTypeID = elementTypeID;
}