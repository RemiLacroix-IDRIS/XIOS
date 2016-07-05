/*!
   \file max.cpp
   \author Ha NGUYEN
   \since 27 June 2016
   \date 27 June 2016

   \brief max reduction
 */
#include "max.hpp"

namespace xios {

CMaxReductionAlgorithm::CMaxReductionAlgorithm()
  : CReductionAlgorithm()
{
}

CReductionAlgorithm* CMaxReductionAlgorithm::create()
{
  return (new CMaxReductionAlgorithm());
}

bool CMaxReductionAlgorithm::registerTrans()
{
  return registerOperation(TRANS_REDUCE_MAX, CMaxReductionAlgorithm::create);
}

void CMaxReductionAlgorithm::apply(const std::vector<std::pair<int,double> >& localIndex,
                                   const double* dataInput,
                                   CArray<double,1>& dataOut,
                                   std::vector<bool>& flagInitial)
{
  int nbLocalIndex = localIndex.size();
  int currentlocalIndex = 0;
  double currentWeight  = 0.0;
  for (int idx = 0; idx < nbLocalIndex; ++idx)
  {
    currentlocalIndex = localIndex[idx].first;
    currentWeight     = localIndex[idx].second;
    if (flagInitial[currentlocalIndex])
    {
      dataOut(currentlocalIndex) = *(dataInput + idx);
      flagInitial[currentlocalIndex] = false;
    }
    else
    {
      dataOut(currentlocalIndex) = std::max(*(dataInput + idx), dataOut(currentlocalIndex));
    }
  }
}

}
