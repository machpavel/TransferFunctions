#ifndef IEIGENVALUES_FILTER_VISITOR_
#define IEIGENVALUES_FILTER_VISITOR_

template<typename PixelType = Constants::GlobalPixelType>
class IEigenValuesFilterVisitor
{
public:
  typedef double EigenvaluesType;

  virtual void Initialize() = 0;
  virtual PixelType Visit(EigenvaluesType lambda1, EigenvaluesType lambda2, EigenvaluesType lambda3) = 0;

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

  void SortEigenValuesDecreasing(EigenvaluesType& lambda1, EigenvaluesType& lambda2, EigenvaluesType& lambda3)
  {
    // simple bubble sort
    while (lambda1 < lambda2 || lambda2 < lambda3)
    {
      if (lambda1 < lambda2)
      {
        std::swap(lambda1, lambda2);
      }
      if (lambda2 < lambda3)
      {
        std::swap(lambda2, lambda3);
      }
    }
  }
};

template<typename PixelType = Constants::GlobalPixelType>
class EigenValuesLinearCombinationVisitor : IEigenValuesFilterVisitor<PixelType>
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
  EigenvaluesType lambda1;
  EigenvaluesType lambda2;
  EigenvaluesType lambda3;
};

template<typename PixelType = Constants::GlobalPixelType>
class EigenValuesFrangiVesselnessVisitor : IEigenValuesFilterVisitor<PixelType>
{
public:

  //static const PixelType RANGE_NORMALIZATION_CONSTANT = 1000;

  void Initialize() override
  {
#if _DEBUG
    this->max = 0;
#endif

    std::cout << "alpha: ";
    std::cin >> this->alpha;

    std::cout << "beta: ";
    std::cin >> this->beta;

    std::cout << "gamma: ";
    std::cin >> this->gamma;
    std::cout << std::endl;
  }

  EigenvaluesType ExponencialFormula(EigenvaluesType a, EigenvaluesType b)
  {
    return exp(-((a*a) / (2*b*b)));
  }

  PixelType Visit(EigenvaluesType lambda1, EigenvaluesType lambda2, EigenvaluesType lambda3) override
  {
    this->SortEigenValuesAbsoluteValue(lambda1, lambda2, lambda3);

    this->R_A = abs(lambda2) / abs(lambda3);
    this->R_B = abs(lambda1) / abs(lambda2*lambda3);
    this->S = sqrt(lambda1*lambda1 + lambda2*lambda2 + lambda3*lambda3);

    if (lambda2 < 0 && lambda3 < 0)
    {
      EigenvaluesType retval = (1 - ExponencialFormula(R_A, alpha));// *ExponencialFormula(R_B, beta) * (1 - ExponencialFormula(S, gamma));

      return retval * 1000;
    }
    else
    {
      return (float)0;
    }
  }

  EigenvaluesType alpha;
  EigenvaluesType beta;
  EigenvaluesType gamma;

  EigenvaluesType R_A;
  EigenvaluesType R_B;
  EigenvaluesType S;

#if _DEBUG
  EigenvaluesType max;
#endif
};

template<typename PixelType = Constants::GlobalPixelType>
class EigenValuesVesselnessVisitor : IEigenValuesFilterVisitor<PixelType>
{
public:

  void Initialize() override
  {
    std::cout << "alpha1: ";
    std::cin >> this->alpha1;

    std::cout << "alpha2: ";
    std::cin >> this->alpha2;
    std::cout << std::endl;
  }

  EigenvaluesType VesselnessFormula(EigenvaluesType alpha, EigenvaluesType lambda2)
  {
    return exp(-((lambda2 * lambda2) / (2 * alpha * lambda2 * alpha * lambda2))) * Constants::RANGE_NORMALIZATION_CONSTANT;
  }

  PixelType Visit(EigenvaluesType lambda1, EigenvaluesType lambda2, EigenvaluesType lambda3) override
  {
    // based on: http://www.spl.harvard.edu/archive/spl-pre2007/pages/papers/yoshi/node3.html

    this->SortEigenValuesDecreasing(lambda1, lambda2, lambda3);

    // lambda2 > lambda3: wtf? it is given by the condition...
    //bool lambdaC = min(-lambda2, -lambda3) == -lambda2;

    if (lambda2 != 0)
    {
      if (lambda1 <= 0)
      {
        return this->VesselnessFormula(this->alpha1, lambda2);
      }
      else
      {
        return this->VesselnessFormula(this->alpha2, lambda2);
      }
    }
    else
    {
      return 0;
    }
  }

  EigenvaluesType alpha1;
  EigenvaluesType alpha2;
};

template<typename PixelType = Constants::GlobalPixelType>
class EigenValuesNaiveVesselnessVisitor : IEigenValuesFilterVisitor<PixelType>
{
public:
  void Initialize() override
  {
  }

  PixelType Visit(EigenvaluesType lambda1, EigenvaluesType lambda2, EigenvaluesType lambda3) override
  {
    this->SortEigenValuesAbsoluteValue(lambda1, lambda2, lambda3);

    // lambda2 and lambda3 should have high absolute values, while lambda1 shoud be closest to zero
    return (std::abs(lambda2) + std::abs(lambda3)) / std::abs(lambda1);
  }
};

#endif