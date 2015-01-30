#ifndef IEIGENVALUES_FILTER_VISITOR_
#define IEIGENVALUES_FILTER_VISITOR_

template<typename PixelType = Constants::GlobalPixelType>
class IEigenValuesFilterVisitor
{
public:
  typedef double EigenvaluesType;

  virtual void Initialize() = 0;
  virtual PixelType Visit(EigenvaluesType lambda1, EigenvaluesType lambda2, EigenvaluesType lambda3) = 0;
};

template<typename PixelType = Constants::GlobalPixelType>
class EivenValuesLinearCombinationVisitor : IEigenValuesFilterVisitor<PixelType>
{
public:
  void Initialize() override
  {
    std::cout << "enter linear combination coeficients for the eigenvectors components" << std::endl;

    std::cout << "lambda1 coeficient: ";
    std::cin >> this->lambda1;

    std::cout << "lambda2 coeficient: ";
    std::cin >> this->lambda2;

    std::cout << "lambda3 coeficient: ";
    std::cin >> this->lambda3;
    std::cout << std::endl;
  }

  PixelType Visit(PixelType lambda1, PixelType lambda2, PixelType lambda3) override
  {
    return this->lambda1*lambda1 + this->lambda2*lambda2 + this->lambda3*lambda3;
  }

private:
  double lambda1;
  double lambda2;
  double lambda3;
};

template<typename PixelType = Constants::GlobalPixelType>
class EivenValuesVesselnessVisitor : IEigenValuesFilterVisitor<PixelType>
{
public:
  void Initialize() override
  {
  }

  PixelType Visit(EigenvaluesType lambda1, EigenvaluesType lambda2, EigenvaluesType lambda3) override
  {
    this->SortEigenValuesAbsoluteValue(lambda1, lambda2, lambda3);

    return (std::abs(lambda2) + std::abs(lambda3)) / std::abs(lambda1 + std::numeric_limits<PixelType>::epsilon()); // to avoid division by zero
  }

private:
  void SortEigenValuesAbsoluteValue(EigenvaluesType& lambda1, EigenvaluesType& lambda2, EigenvaluesType& lambda3)
  {
    // simple bubble sort
    while (abs(lambda1) > abs(lambda2) || abs(lambda2) > abs(lambda3))
    {
      if (abs(lambda1) > abs(lambda2))
      {
        std::swap(lambda1, lambda2);
      }
      if (abs(lambda2) > abs(lambda3))
      {
        std::swap(lambda2, lambda3);
      }
    }
  }
};

#endif