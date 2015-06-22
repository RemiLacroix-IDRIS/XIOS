#include "generic_algorithm_transformation.hpp"

namespace xios {

CGenericAlgorithmTransformation::CGenericAlgorithmTransformation()
 : transformationMapping_()
{
}

/*!
  This function computes the global indexes of grid source, which the grid destination is in demand.
  \param[in] elementPositionInGrid position of an element in a grid .E.g: if grid is composed of domain and axis (in order),
                then position of axis in grid is 2 (since a domain is considered to contain 2 elements (axis)
  \param[in] gridDestGlobalDim global size of each dimension of grid source (all dimension must have the same size except of the one on which transformation is performed)
  \param[in] globalIndexGridDestSendToServer global index of grid destination on the current client to send to server
  \param[in/out] globaIndexMapFromDestToSource mapping between transformed global index of grid destination
                    and the demanded global index of grid source
*/
void CGenericAlgorithmTransformation::computeGlobalSourceIndex(int elementPositionInGrid,
                                                             const std::vector<int>& gridDestGlobalDim,
                                                             const CArray<size_t,1>& globalIndexGridDestSendToServer,
                                                             std::map<size_t, std::set<size_t> >& globaIndexMapFromDestToSource)
{
  std::map<int, std::vector<int> >::const_iterator itbTransMap = transformationMapping_.begin(),
                                                   itTransMap = itbTransMap,
                                                   iteTransMap = transformationMapping_.end();
  std::vector<int>::const_iterator itbVec, itVec, iteVec;
  std::vector<CArray<size_t,1> > globalIndexSrcGrid((itTransMap->second).size());
  CArray<size_t,1> globalIndexDestGrid;

  for (itTransMap = itbTransMap; itTransMap != iteTransMap; ++itTransMap)
  {
    this->computeGlobalIndexFromGlobalIndexElement(itTransMap->first,
                                                   itTransMap->second,
                                                   elementPositionInGrid,
                                                   gridDestGlobalDim,
                                                   globalIndexGridDestSendToServer,
                                                   globalIndexDestGrid,
                                                   globalIndexSrcGrid);
    size_t globalIndexSize = globalIndexDestGrid.numElements();
    for (size_t idx = 0; idx < globalIndexSize; ++idx)
    {
      for (int i = 0; i < globalIndexSrcGrid.size(); ++i)
      {
        globaIndexMapFromDestToSource[globalIndexDestGrid(idx)].insert(globalIndexSrcGrid[i](idx));
      }
    }
  }
}

}