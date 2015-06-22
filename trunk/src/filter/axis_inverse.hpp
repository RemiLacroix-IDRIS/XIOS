#ifndef __XIOS_AXIS_ALGORITHM_INVERSE_HPP__
#define __XIOS_AXIS_ALGORITHM_INVERSE_HPP__

#include "concrete_algo.hpp"
#include "axis.hpp"

namespace xios {

class CAxisInverse : public CConcreteAlgo
{
public:
  CAxisInverse(CAxis* axisDestination, CAxis* axisSource);

  virtual ~CAxisInverse() {}

  virtual void computeIndexSourceMapping(const std::map<int, std::vector<int> >& transformationMappingOfPreviousAlgo);

protected:
  std::vector<int> axisDestGlobalIndex_;

private:
  int axisDestGlobalSize_;
};

}
#endif // __XIOS_AXIS_ALGORITHM_INVERSE_HPP__
