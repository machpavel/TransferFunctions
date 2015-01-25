#ifndef IEIGENVALUES_FILTER_VISITOR_
#define IEIGENVALUES_FILTER_VISITOR_

template<typename PixelType = Constants::GlobalPixelType>
class IEigenValuesFilterVisitor
{
public:
  virtual void Initialize() = 0;
  virtual PixelType Visit(PixelType lambda1, PixelType lambda2, PixelType lambda3) = 0;
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

  PixelType Visit(PixelType lambda1, PixelType lambda2, PixelType lambda3) override
  {
    return std::abs(lambda1) / (std::abs(lambda2) + std::abs(lambda3) + 0.001); // to avoid division by zero
  }
};

#endif