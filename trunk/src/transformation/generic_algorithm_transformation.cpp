/*!
   \file generic_algorithm_transformation.hpp
   \author Ha NGUYEN
   \since 14 May 2015
   \date 21 Mars 2016

   \brief Interface for all transformation algorithms.
 */
#include "generic_algorithm_transformation.hpp"
#include "context.hpp"
#include "context_client.hpp"
#include "client_client_dht_template.hpp"

namespace xios {

CGenericAlgorithmTransformation::CGenericAlgorithmTransformation()
 : transformationMapping_(), transformationWeight_(), transformationPosition_(), idAuxInputs_()
{
}

/*!
  This function computes the global indexes of grid source, which the grid destination is in demand.
  \param[in] elementPositionInGrid position of an element in a grid .E.g: if grid is composed of domain and axis (in order),
                then position of axis in grid is 2 (since a domain is considered to contain 2 elements (axis)
  \param[in] gridDestGlobalDim global size of each dimension of grid source (all dimension must have the same size except of the one on which transformation is performed)
  \param[in] gridSrcGlobalDim dimension size of source grid (it should share the same size for all dimension, maybe except the domain on which transformation is performed)
  \param[in] globalLocalIndexGridDestSendToServer global and local index mapping of grid destination on the current client to send to server
  \param[in/out] globaIndexWeightFromDestToSource mapping between transformed global index of grid destination
             and the weighted value as well as global index from grid index source
*/
//void CGenericAlgorithmTransformation::computeGlobalSourceIndex(int elementPositionInGrid,
//                                                               const std::vector<int>& gridDestGlobalDim,
//                                                               const std::vector<int>& gridSrcGlobalDim,
//                                                               const GlobalLocalMap& globalLocalIndexGridDestSendToServer,
//                                                               DestinationIndexMap& globaIndexWeightFromDestToSource)
//{
//  bool isTransPosEmpty = transformationPosition_.empty();
//  for (size_t idxTrans = 0; idxTrans < transformationMapping_.size(); ++idxTrans)
//  {
//    TransformationIndexMap::const_iterator itbTransMap = transformationMapping_[idxTrans].begin(), itTransMap,
//                                                     iteTransMap = transformationMapping_[idxTrans].end();
//    TransformationWeightMap::const_iterator itTransWeight = transformationWeight_[idxTrans].begin();
//
//    // If transformation position exists
//    TransformationIndexMap::const_iterator itTransPos, iteTransPos;
//    if (!isTransPosEmpty)
//    {
//      itTransPos  = transformationPosition_[idxTrans].begin(),
//      iteTransPos = transformationPosition_[idxTrans].end();
//    }
//    std::vector<int> emptyTransPos;
//
//    std::vector<std::vector<size_t> > globalIndexSrcGrid;
//    std::vector<std::pair<size_t,int> > globalLocalIndexDest;
//    for (itTransMap = itbTransMap; itTransMap != iteTransMap; ++itTransMap, ++itTransWeight)
//    {
//      if (!isTransPosEmpty)
//      {
//        this->computeGlobalGridIndexFromGlobalIndexElement(itTransMap->first,
//                                                           itTransMap->second,
//                                                           itTransPos->second,
//                                                           elementPositionInGrid,
//                                                           gridDestGlobalDim,
//                                                           gridSrcGlobalDim,
//                                                           globalLocalIndexGridDestSendToServer,
//                                                           globalLocalIndexDest,
//                                                           globalIndexSrcGrid);
//        ++itTransPos;
//      }
//      else
//      {
//        this->computeGlobalGridIndexFromGlobalIndexElement(itTransMap->first,
//                                                           itTransMap->second,
//                                                           emptyTransPos,
//                                                           elementPositionInGrid,
//                                                           gridDestGlobalDim,
//                                                           gridSrcGlobalDim,
//                                                           globalLocalIndexGridDestSendToServer,
//                                                           globalLocalIndexDest,
//                                                           globalIndexSrcGrid);
//      }
//      std::vector<std::pair<size_t,int> >::const_iterator it = globalLocalIndexDest.begin(), ite = globalLocalIndexDest.end();
//      const std::vector<double>& currentVecWeight = itTransWeight->second;
//
//      for (size_t idx = 0; it != ite; ++it, ++idx)
//      {
//        size_t srcGridSize = globalIndexSrcGrid[idx].size();
////        globaIndexWeightFromDestToSource[(it->first)].resize(srcGridSize);
//        DestinationGlobalIndex& tmp = globaIndexWeightFromDestToSource[(it->first)];
//        tmp.resize(srcGridSize);
//        for (int i = 0; i < srcGridSize; ++i)
//        {
//          tmp[i].first = it->second;
//          tmp[i].second = make_pair(globalIndexSrcGrid[idx][i], currentVecWeight[i]);
////          globaIndexWeightFromDestToSource[(it->first)][i] = (make_pair(it->second, make_pair(globalIndexSrcGrid[idx][i], currentVecWeight[i])));
//        }
//      }
//    }
//  }
//}

/*!
  This function computes the global indexes of grid source, which the grid destination is in demand.
  \param[in] elementPositionInGrid position of an element in a grid .E.g: if grid is composed of domain and axis (in order),
                then position of axis in grid is 1 and domain is positioned at 0.
  \param[in] gridSrc Grid source
  \param[in] gridDst Grid destination
  \param[in] globaIndexWeightFromSrcToDst mapping of each global index source and weight to index destination
*/
void CGenericAlgorithmTransformation::computeGlobalSourceIndex(int elementPositionInGrid,
                                                               CGrid* gridSrc,
                                                               CGrid* gridDst,
                                                               SourceDestinationIndexMap& globaIndexWeightFromSrcToDst)
 {
  CContext* context = CContext::getCurrent();
  CContextClient* client = context->client;
  int nbClient = client->clientSize;

  typedef boost::unordered_map<int, std::vector<std::pair<int,double> > > SrcToDstMap;
  bool isTransPosEmpty = transformationPosition_.empty();
  for (size_t idxTrans = 0; idxTrans < transformationMapping_.size(); ++idxTrans)
  {
    TransformationIndexMap::const_iterator itbTransMap = transformationMapping_[idxTrans].begin(), itTransMap,
                                           iteTransMap = transformationMapping_[idxTrans].end();
    TransformationWeightMap::const_iterator itbTransWeight = transformationWeight_[idxTrans].begin(), itTransWeight;
    SrcToDstMap src2DstMap;
    src2DstMap.rehash(std::ceil(transformationMapping_[idxTrans].size()/src2DstMap.max_load_factor()));

    int indexSrcSize = 0;
    itTransWeight = itbTransWeight;
    for (itTransMap = itbTransMap; itTransMap != iteTransMap; ++itTransMap, ++itTransWeight)
    {
       indexSrcSize += (itTransMap->second).size();
    }

    CArray<size_t,1> indexSrc(indexSrcSize);
    int indexSrcIndex = 0;
    // Build mapping between global source element index and global destination element index.
    itTransWeight = itbTransWeight;
    for (itTransMap = itbTransMap; itTransMap != iteTransMap; ++itTransMap, ++itTransWeight)
    {
      const std::vector<int>& srcIndex = itTransMap->second;
      const std::vector<double>& weight = itTransWeight->second;
      for (int idx = 0; idx < srcIndex.size(); ++idx)
      {
        src2DstMap[srcIndex[idx]].push_back(make_pair(itTransMap->first, weight[idx]));
        indexSrc(indexSrcIndex) = srcIndex[idx];
        ++indexSrcIndex;
      }
    }

    std::vector<CAxis*> axisListDestP = gridDst->getAxis();
    std::vector<CDomain*> domainListDestP = gridDst->getDomains();
    CArray<bool,1> axisDomainDstOrder = gridDst->axis_domain_order;
    std::vector<CAxis*> axisListSrcP = gridSrc->getAxis();
    std::vector<CDomain*> domainListSrcP = gridSrc->getDomains();
    CArray<bool,1> axisDomainSrcOrder = gridDst->axis_domain_order;

    CArray<size_t,1> transPos;
    if (!isTransPosEmpty)
    {
      transPos.resize(transformationPosition_[idxTrans].size());
      TransformationPositionMap::const_iterator itPosMap = transformationPosition_[idxTrans].begin(),
                                               itePosMap = transformationPosition_[idxTrans].end();
      for (int idx = 0; itPosMap != itePosMap; ++itPosMap, ++idx)
        transPos(idx) = itPosMap->second[0];
    }
    // Find out global index source of transformed element on corresponding process.
    std::vector<boost::unordered_map<int,std::vector<size_t> > > globalElementIndexOnProc(axisDomainDstOrder.numElements());
    int axisIndex = 0, domainIndex = 0;
    for (int idx = 0; idx < axisDomainDstOrder.numElements(); ++idx)
    {
      if (idx == elementPositionInGrid)
        computeExchangeGlobalIndex(indexSrc, globalElementIndexOnProc[idx]);
      if (axisDomainDstOrder(idx)) // It's domain
      {
        if (idx != elementPositionInGrid)
          computeExchangeDomainIndex(domainListDestP[domainIndex],
                                     domainListSrcP[domainIndex],
                                     transPos,
                                     globalElementIndexOnProc[idx]);
        ++domainIndex;

      }
      else //it's an axis
      {
        if (idx != elementPositionInGrid)
          computeExchangeAxisIndex(axisListDestP[axisIndex],
                                   axisListSrcP[axisIndex],
                                   transPos,
                                   globalElementIndexOnProc[idx]);
        ++axisIndex;

      }
    }

    std::vector<std::vector<bool> > elementOnProc(axisDomainDstOrder.numElements(), std::vector<bool>(nbClient, false));

    boost::unordered_map<int,std::vector<size_t> >::const_iterator it, itb, ite;
    for (int idx = 0; idx < globalElementIndexOnProc.size(); ++idx)
    {
      itb = globalElementIndexOnProc[idx].begin();
      ite = globalElementIndexOnProc[idx].end();
      for (it = itb; it != ite; ++it) elementOnProc[idx][it->first] = true;
    }

    // Determine procs which contain global source index
    std::vector<bool> intersectedProc(nbClient, true);
    for (int idx = 0; idx < axisDomainDstOrder.numElements(); ++idx)
    {
      std::transform(elementOnProc[idx].begin(), elementOnProc[idx].end(),
                     intersectedProc.begin(), intersectedProc.begin(),
                     std::logical_and<bool>());
    }

    std::vector<int> srcRank;
    for (int idx = 0; idx < nbClient; ++idx)
    {
      if (intersectedProc[idx]) srcRank.push_back(idx);
    }

    // Ok, now compute global index of grid source and ones of grid destination
    computeGlobalGridIndexMapping(elementPositionInGrid,
                                  srcRank,
                                  src2DstMap,
                                  gridDst,
                                  gridSrc,
                                  globalElementIndexOnProc,
                                  globaIndexWeightFromSrcToDst);
  }
 }

/*!
  Compute mapping of global index of grid source and grid destination
  \param [in] elementPositionInGrid position of element in grid. E.x: grid composed of domain and axis, domain has position 0 and axis 1.
  \param [in] srcRank rank of client from which we demand global index of element source
  \param [in] src2DstMap mapping of global index of element source and global index of element destination
  \param[in] gridSrc Grid source
  \param[in] gridDst Grid destination
  \param[in] globalElementIndexOnProc Global index of element source on different client rank
  \param[out] globaIndexWeightFromSrcToDst Mapping of global index of grid source and grid destination
*/
void CGenericAlgorithmTransformation::computeGlobalGridIndexMapping(int elementPositionInGrid,
                                                                   const std::vector<int>& srcRank,
                                                                   boost::unordered_map<int, std::vector<std::pair<int,double> > >& src2DstMap,
                                                                   CGrid* gridSrc,
                                                                   CGrid* gridDst,
                                                                   std::vector<boost::unordered_map<int,std::vector<size_t> > >& globalElementIndexOnProc,
                                                                   SourceDestinationIndexMap& globaIndexWeightFromSrcToDst)
{
  std::vector<CAxis*> axisListDestP = gridDst->getAxis();
  std::vector<CDomain*> domainListDestP = gridDst->getDomains();
  CArray<bool,1> axisDomainDstOrder = gridDst->axis_domain_order;
  std::vector<CAxis*> axisListSrcP = gridSrc->getAxis();
  std::vector<CDomain*> domainListSrcP = gridSrc->getDomains();
  CArray<bool,1> axisDomainSrcOrder = gridDst->axis_domain_order;
  size_t nbElement = axisDomainSrcOrder.numElements();
  std::vector<size_t> nGlobSrc(nbElement), nGlobDst(nbElement);
  size_t globalSrcSize = 1, globalDstSize = 1;
  int domainIndex = 0;
  int axisIndex = 0;
  for (int idx = 0; idx < nbElement; ++idx)
  {
    nGlobSrc[idx] = globalSrcSize;
    nGlobDst[idx] = globalDstSize;
    bool isDomain = axisDomainSrcOrder(idx);

    // If this is a domain
    if (isDomain)
    {
      globalSrcSize *= domainListSrcP[domainIndex]->nj_glo.getValue() * domainListSrcP[domainIndex]->ni_glo.getValue();
      globalDstSize *= domainListDestP[domainIndex]->nj_glo.getValue() * domainListDestP[domainIndex]->ni_glo.getValue();
      ++domainIndex;
    }
    else // So it's an axis
    {
      globalSrcSize *= axisListSrcP[axisIndex]->n_glo.getValue();
      globalDstSize *= axisListDestP[axisIndex]->n_glo.getValue();
      ++axisIndex;
    }
  }

  for (int i = 0; i < srcRank.size(); ++i)
  {
    size_t ssize = 1;
    int rankSrc = srcRank[i];
    for (int idx = 0; idx < nbElement; ++idx)
    {
      ssize *= (globalElementIndexOnProc[idx][rankSrc]).size();
    }

    std::vector<int> idxLoop(nbElement,0);
    std::vector<int> currentIndexSrc(nbElement, 0);
    std::vector<int> currentIndexDst(nbElement, 0);
    int innnerLoopSize = (globalElementIndexOnProc[0])[rankSrc].size();
    size_t idx = 0;
    while (idx < ssize)
    {
      for (int ind = 0; ind < nbElement; ++ind)
      {
        if (idxLoop[ind] == (globalElementIndexOnProc[ind])[rankSrc].size())
        {
          idxLoop[ind] = 0;
          ++idxLoop[ind+1];
        }

        currentIndexDst[ind] = currentIndexSrc[ind] = (globalElementIndexOnProc[ind])[rankSrc][idxLoop[ind]];
      }

      for (int ind = 0; ind < innnerLoopSize; ++ind)
      {
        currentIndexSrc[0] = (globalElementIndexOnProc[0])[rankSrc][ind];
        int globalElementDstIndexSize = 0;
        if (1 == src2DstMap.count(currentIndexSrc[elementPositionInGrid]))
        {
          globalElementDstIndexSize = src2DstMap[currentIndexSrc[elementPositionInGrid]].size();
        }
        std::vector<size_t> globalDstVecIndex(globalElementDstIndexSize,0);
        size_t globalSrcIndex = 0;
        for (int idxElement = 0; idxElement < nbElement; ++idxElement)
        {
          if (idxElement == elementPositionInGrid)
          {
            for (int k = 0; k < globalElementDstIndexSize; ++k)
            {
              globalDstVecIndex[k] += src2DstMap[currentIndexSrc[elementPositionInGrid]][k].first * nGlobDst[idxElement];
            }
          }
          else
          {
            for (int k = 0; k < globalElementDstIndexSize; ++k)
            {
              globalDstVecIndex[k] += currentIndexDst[idxElement] * nGlobDst[idxElement];
            }
          }
          globalSrcIndex += currentIndexSrc[idxElement] * nGlobSrc[idxElement];
        }

        for (int k = 0; k < globalElementDstIndexSize; ++k)
        {
          globaIndexWeightFromSrcToDst[rankSrc][globalSrcIndex].push_back(make_pair(globalDstVecIndex[k],src2DstMap[currentIndexSrc[elementPositionInGrid]][k].second ));
        }
        ++idxLoop[0];
      }
      idx += innnerLoopSize;
    }
  }
}

/*!
  Find out proc and global index of axis source which axis destination is on demande
  \param[in] axisDst Axis destination
  \param[in] axisSrc Axis source
  \param[in] destGlobalIndexPositionInGrid Relative position of axis corresponds to other element of grid.
  \param[out] globalAxisIndexOnProc Global index of axis source on different procs
*/
void CGenericAlgorithmTransformation::computeExchangeAxisIndex(CAxis* axisDst,
                                                               CAxis* axisSrc,
                                                               CArray<size_t,1>& destGlobalIndexPositionInGrid,
                                                               boost::unordered_map<int,std::vector<size_t> >& globalAxisIndexOnProc)
{
  CContext* context = CContext::getCurrent();
  CContextClient* client=context->client;
  int clientRank = client->clientRank;
  int clientSize = client->clientSize;

  size_t globalIndex;
  int nIndexSize = axisSrc->index.numElements();
  CClientClientDHTInt::Index2VectorInfoTypeMap globalIndex2ProcRank;
  globalIndex2ProcRank.rehash(std::ceil(nIndexSize/globalIndex2ProcRank.max_load_factor()));
  for (int idx = 0; idx < nIndexSize; ++idx)
  {
    globalIndex = axisSrc->index(idx);
    globalIndex2ProcRank[globalIndex].push_back(clientRank);
  }

  CClientClientDHTInt dhtIndexProcRank(globalIndex2ProcRank, client->intraComm);
  CArray<size_t,1> globalAxisIndex(axisDst->index.numElements());
  for (int idx = 0; idx < globalAxisIndex.numElements(); ++idx)
  {
    globalAxisIndex(idx) = axisDst->index(idx);
  }
  dhtIndexProcRank.computeIndexInfoMapping(globalAxisIndex);

  std::vector<int> countIndex(clientSize,0);
  const CClientClientDHTInt::Index2VectorInfoTypeMap& computedGlobalIndexOnProc = dhtIndexProcRank.getInfoIndexMap();
  CClientClientDHTInt::Index2VectorInfoTypeMap::const_iterator itb = computedGlobalIndexOnProc.begin(), it,
                                                               ite = computedGlobalIndexOnProc.end();
  for (it = itb; it != ite; ++it)
  {
    const std::vector<int>& procList = it->second;
    for (int idx = 0; idx < procList.size(); ++idx) ++countIndex[procList[idx]];
  }

  globalAxisIndexOnProc.rehash(std::ceil(clientSize/globalAxisIndexOnProc.max_load_factor()));
  for (int idx = 0; idx < clientSize; ++idx)
  {
    if (0 != countIndex[idx])
    {
      globalAxisIndexOnProc[idx].resize(countIndex[idx]);
      countIndex[idx] = 0;
    }
  }

  for (it = itb; it != ite; ++it)
  {
    const std::vector<int>& procList = it->second;
    for (int idx = 0; idx < procList.size(); ++idx)
    {
      globalAxisIndexOnProc[procList[idx]][countIndex[procList[idx]]] = it->first;
      ++countIndex[procList[idx]];
    }
  }
}

/*!
  Find out proc and global index of domain source which domain destination is on demande
  \param[in] domainDst Domain destination
  \param[in] domainSrc Domain source
  \param[in] destGlobalIndexPositionInGrid Relative position of domain corresponds to other element of grid.
  \param[out] globalDomainIndexOnProc Global index of domain source on different procs
*/
void CGenericAlgorithmTransformation::computeExchangeDomainIndex(CDomain* domainDst,
                                                                 CDomain* domainSrc,
                                                                 CArray<size_t,1>& destGlobalIndexPositionInGrid,
                                                                 boost::unordered_map<int,std::vector<size_t> >& globalDomainIndexOnProc)
{
  CContext* context = CContext::getCurrent();
  CContextClient* client=context->client;
  int clientRank = client->clientRank;
  int clientSize = client->clientSize;

  int niGlob = domainSrc->ni_glo.getValue();
  int njGlob = domainSrc->nj_glo.getValue();
  size_t globalIndex;
  int nIndexSize = domainSrc->i_index.numElements(), i_ind, j_ind;
  CClientClientDHTInt::Index2VectorInfoTypeMap globalIndex2ProcRank;
  globalIndex2ProcRank.rehash(std::ceil(nIndexSize/globalIndex2ProcRank.max_load_factor()));
  for (int idx = 0; idx < nIndexSize; ++idx)
  {
    i_ind=domainSrc->i_index(idx) ;
    j_ind=domainSrc->j_index(idx) ;

    globalIndex = i_ind + j_ind * niGlob;
    globalIndex2ProcRank[globalIndex].push_back(clientRank);
  }

  CArray<size_t,1> globalDomainIndex;
  if (destGlobalIndexPositionInGrid.isEmpty())
  {
    globalDomainIndex.resize(domainDst->i_index.numElements());
    nIndexSize = domainDst->i_index.numElements();

    for (int idx = 0; idx < nIndexSize; ++idx)
    {
      i_ind=domainDst->i_index(idx) ;
      j_ind=domainDst->j_index(idx) ;

      globalDomainIndex(idx) = i_ind + j_ind * niGlob;
    }
  }
  else
  {
    globalDomainIndex  = destGlobalIndexPositionInGrid;
//    for (int idx = 0; idx < destGlobalIndexPositionInGrid.size(); ++idx)
//    {
//      globalDomainIndex(idx) = destGlobalIndexPositionInGrid[idx];
//    }
  }

  CClientClientDHTInt dhtIndexProcRank(globalIndex2ProcRank, client->intraComm);
  dhtIndexProcRank.computeIndexInfoMapping(globalDomainIndex);

  std::vector<int> countIndex(clientSize,0);
  const CClientClientDHTInt::Index2VectorInfoTypeMap& computedGlobalIndexOnProc = dhtIndexProcRank.getInfoIndexMap();
  CClientClientDHTInt::Index2VectorInfoTypeMap::const_iterator itb = computedGlobalIndexOnProc.begin(), it,
                                                               ite = computedGlobalIndexOnProc.end();
  for (it = itb; it != ite; ++it)
  {
    const std::vector<int>& procList = it->second;
    for (int idx = 0; idx < procList.size(); ++idx) ++countIndex[procList[idx]];
  }

  globalDomainIndexOnProc.rehash(std::ceil(clientSize/globalDomainIndexOnProc.max_load_factor()));
  for (int idx = 0; idx < clientSize; ++idx)
  {
    if (0 != countIndex[idx])
    {
      globalDomainIndexOnProc[idx].resize(countIndex[idx]);
      countIndex[idx] = 0;
    }
  }

  for (it = itb; it != ite; ++it)
  {
    const std::vector<int>& procList = it->second;
    for (int idx = 0; idx < procList.size(); ++idx)
    {
      globalDomainIndexOnProc[procList[idx]][countIndex[procList[idx]]] = it->first;
      ++countIndex[procList[idx]];
    }
  }
}

/*!
  Compute index mapping between element source and element destination with an auxiliary inputs which determine
position of each mapped index in global index of grid destination.
  \param [in] dataAuxInputs auxiliary inputs
*/
void CGenericAlgorithmTransformation::computeIndexSourceMapping(const std::vector<CArray<double,1>* >& dataAuxInputs)
{
  computeIndexSourceMapping_(dataAuxInputs);
}

std::vector<StdString> CGenericAlgorithmTransformation::getIdAuxInputs()
{
  return idAuxInputs_;
}

}
