/*!
   \file axis_algorithm_transformation.hpp
   \author Ha NGUYEN
   \since 14 May 2015
   \date 29 June 2015

   \brief Interface for all axis transformation algorithms.
 */

#include "axis_algorithm_transformation.hpp"
#include "axis_algorithm_inverse.hpp"
#include "axis_algorithm_zoom.hpp"
#include "context.hpp"
#include "context_client.hpp"
#include "client_client_dht_template.hpp"
#include "axis.hpp"

namespace xios {

CAxisAlgorithmTransformation::CAxisAlgorithmTransformation(CAxis* axisDestination, CAxis* axisSource)
 : CGenericAlgorithmTransformation(), axisDest_(axisDestination), axisSrc_(axisSource)
{
  axisDestGlobalSize_ = axisDestination->n_glo.getValue();
  int niDest = axisDestination->n.getValue();
  int ibeginDest = axisDestination->begin.getValue();

  for (int idx = 0; idx < niDest; ++idx)
    if ((axisDestination->mask)(idx)) axisDestGlobalIndex_.push_back(ibeginDest+idx);
}

CAxisAlgorithmTransformation::~CAxisAlgorithmTransformation()
{
}

void CAxisAlgorithmTransformation::computeIndexSourceMapping_(const std::vector<CArray<double,1>* >& dataAuxInputs)
{
}

/*!
  Compute global index of axis on different processes
  \param [in] globalAxisIndex global index of axis source
  \param [out] globalAxisIndexOnProc processes which contain the corresponding global index of axis source
*/
void CAxisAlgorithmTransformation::computeExchangeGlobalIndex(const CArray<size_t,1>& globalAxisIndex,
                                                              CClientClientDHTInt::Index2VectorInfoTypeMap& globalAxisIndexOnProc)
{
  CContext* context = CContext::getCurrent();
  CContextClient* client=context->client;
  int clientRank = client->clientRank;
  int clientSize = client->clientSize;

  size_t globalIndex;
  int nIndexSize = axisSrc_->index.numElements();
  CClientClientDHTInt::Index2VectorInfoTypeMap globalIndex2ProcRank;
  globalIndex2ProcRank.rehash(std::ceil(nIndexSize/globalIndex2ProcRank.max_load_factor()));
  for (int idx = 0; idx < nIndexSize; ++idx)
  {
    globalIndex = axisSrc_->index(idx);
    globalIndex2ProcRank[globalIndex].resize(1);
    globalIndex2ProcRank[globalIndex][0] = clientRank;
  }

  CClientClientDHTInt dhtIndexProcRank(globalIndex2ProcRank, client->intraComm);
  dhtIndexProcRank.computeIndexInfoMapping(globalAxisIndex);

  globalAxisIndexOnProc = dhtIndexProcRank.getInfoIndexMap();
}

}
