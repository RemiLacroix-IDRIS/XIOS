#include "axis.hpp"

#include "attribute_template.hpp"
#include "object_template.hpp"
#include "group_template.hpp"
#include "message.hpp"
#include "type.hpp"
#include "context.hpp"
#include "context_client.hpp"
#include "xios_spl.hpp"
#include "inverse_axis.hpp"
#include "zoom_axis.hpp"
#include "interpolate_axis.hpp"
#include "server_distribution_description.hpp"
#include "client_server_mapping_distributed.hpp"

namespace xios {

   /// ////////////////////// Définitions ////////////////////// ///

   CAxis::CAxis(void)
      : CObjectTemplate<CAxis>()
      , CAxisAttributes(), isChecked(false), relFiles(), baseRefObject(), areClientAttributesChecked_(false)
      , isDistributed_(false), hasBounds_(false)
      , transformationMap_(), global_zoom_begin(0), global_zoom_size(0)
   {
   }

   CAxis::CAxis(const StdString & id)
      : CObjectTemplate<CAxis>(id)
      , CAxisAttributes(), isChecked(false), relFiles(), baseRefObject(), areClientAttributesChecked_(false)
      , isDistributed_(false), hasBounds_(false)
      , transformationMap_(), global_zoom_begin(0), global_zoom_size(0)
   {
   }

   CAxis::~CAxis(void)
   { /* Ne rien faire de plus */ }

   ///---------------------------------------------------------------

   const std::set<StdString> & CAxis::getRelFiles(void) const
   {
      return (this->relFiles);
   }

   bool CAxis::IsWritten(const StdString & filename) const
   {
      return (this->relFiles.find(filename) != this->relFiles.end());
   }

   bool CAxis::isDistributed(void) const
   {
      return isDistributed_;
   }

   void CAxis::addRelFile(const StdString & filename)
   {
      this->relFiles.insert(filename);
   }

   //----------------------------------------------------------------

   StdString CAxis::GetName(void)   { return (StdString("axis")); }
   StdString CAxis::GetDefName(void){ return (CAxis::GetName()); }
   ENodeType CAxis::GetType(void)   { return (eAxis); }

   //----------------------------------------------------------------

   CAxis* CAxis::createAxis()
   {
     CAxis* axis = CAxisGroup::get("axis_definition")->createChild();
     return axis;
   }

   void CAxis::checkAttributes(void)
   {
      if (this->size.isEmpty())
         ERROR("CAxis::checkAttributes(void)",
               << "Attribute <size> of the axis [ id = '" << getId() << "' , context = '" << CObjectFactory::GetCurrentContextId() << "' ] must be specified");
      StdSize size = this->size.getValue();

      isDistributed_ = !this->ibegin.isEmpty() || !this->ni.isEmpty();

      if (!this->ibegin.isEmpty())
      {
        StdSize ibegin = this->ibegin.getValue();
        if ((ibegin < 0) || (ibegin > size-1))
          ERROR("CAxis::checkAttributes(void)",
                << "Attribute <ibegin> of the axis [ id = '" << getId() << "' , context = '" << CObjectFactory::GetCurrentContextId() << "' ] must be non-negative and smaller than size-1");
      }
      else this->ibegin.setValue(0);

      if (!this->ni.isEmpty())
      {
        StdSize ni = this->ni.getValue();
        if ((ni < 0) || (ni > size))
          ERROR("CAxis::checkAttributes(void)",
                << "Attribute <ni> of the axis [ id = '" << getId() << "' , context = '" << CObjectFactory::GetCurrentContextId() << "' ] must be non-negative and smaller than size");
      }
      else this->ni.setValue(size);

      StdSize true_size = value.numElements();
      if (this->ni.getValue() != true_size)
         ERROR("CAxis::checkAttributes(void)",
               << "The array \'value\' of axis [ id = '" << getId() << "' , context = '" << CObjectFactory::GetCurrentContextId() << "' ] has a different size that the one defined by the \'size\' attribute");

      this->checkData();
      this->checkZoom();
      this->checkMask();
      this->checkBounds();
   }

   void CAxis::checkData()
   {
      if (data_begin.isEmpty()) data_begin.setValue(0);
      if (!data_n.isEmpty() && data_n.getValue() <= 0)
      {
        ERROR("CAxis::checkData(void)",
              << "Data dimension is negative (data_n).");
      }
      else if (data_n.isEmpty())
        data_n.setValue(ni.getValue());

      if (data_index.isEmpty())
      {
        int dn = data_n.getValue();
        data_index.resize(dn);
        for (int i = 0; i < dn; ++i) data_index(i) = (i+1);
      }
   }

   void CAxis::checkZoom(void)
   {
     if (0 == global_zoom_size) global_zoom_size = this->size.getValue();
   }

   void CAxis::checkMask()
   {
      int begin_mask = 0,
          end_mask = ni.getValue()-1;

      if (!zoom_begin.isEmpty())
      {
         int zoom_end = zoom_begin.getValue() + zoom_size.getValue() - 1;

         begin_mask = std::max(ibegin.getValue(), zoom_begin.getValue());
         end_mask   = std::min(ibegin.getValue() + ni.getValue()-1, zoom_end);

         begin_mask -= ibegin.getValue();
         end_mask   -= ibegin.getValue();
      }


      if (!mask.isEmpty())
      {
         if (mask.extent(0) != ni)
            ERROR("CAxis::checkMask(void)",
                  << "the mask has not the same size than the local axis" << endl
                  << "Local size is " << ni << "x" << endl
                  << "Mask size is " << mask.extent(0) << "x");
         for (int i = 0; i < ni; ++i)
         {
           if (i < begin_mask && i > end_mask)  mask(i) = false;
         }
      }
      else // (!mask.hasValue())
      { // Si aucun masque n'est défini,
        // on en crée un nouveau qui valide l'intégralité du domaine.
         mask.resize(ni);
         for (int i = 0; i < ni.getValue(); ++i)
         {
               if (i >= begin_mask && i <= end_mask)
                 mask(i) = true;
               else  mask(i) = false;
         }
      }
   }

  void CAxis::checkBounds()
  {
    if (!bounds.isEmpty())
    {
      if (bounds.extent(0) != ni || bounds.extent(1) != 2)
          ERROR("CAxis::checkAttributes(void)",
                << "The bounds array of the axis [ id = '" << getId() << "' , context = '" << CObjectFactory::GetCurrentContextId() << "' ] must be of dimension axis size x 2" << endl
                << "Axis size is " << ni << endl
                << "Bounds size is "<< bounds.extent(0) << " x " << bounds.extent(1));
      hasBounds_ = true;
    }
    else hasBounds_ = false;
  }


  bool CAxis::dispatchEvent(CEventServer& event)
   {
      if (SuperClass::dispatchEvent(event)) return true;
      else
      {
        switch(event.type)
        {
           case EVENT_ID_SERVER_ATTRIBUT :
             recvServerAttribut(event);
             return true;
             break;
           case EVENT_ID_INDEX:
            recvIndex(event);
            return true;
            break;
          case EVENT_ID_DISTRIBUTED_VALUE:
            recvDistributedValue(event);
            return true;
            break;
          case EVENT_ID_NON_DISTRIBUTED_VALUE:
            recvNonDistributedValue(event);
            return true;
            break;
           default :
             ERROR("bool CContext::dispatchEvent(CEventServer& event)",
                    << "Unknown Event");
           return false;
         }
      }
   }

   void CAxis::checkAttributesOnClient(const std::vector<int>& globalDim, int orderPositionInGrid,
                                       CServerDistributionDescription::ServerDistributionType distType)
   {
     if (this->areClientAttributesChecked_) return;

     this->checkAttributes();

     this->areClientAttributesChecked_ = true;
   }

   // Send all checked attributes to server
   void CAxis::sendCheckedAttributes(const std::vector<int>& globalDim, int orderPositionInGrid,
                                     CServerDistributionDescription::ServerDistributionType distType)
   {
     if (!this->areClientAttributesChecked_) checkAttributesOnClient(globalDim,
                                                                     orderPositionInGrid,
                                                                     distType);
     CContext* context = CContext::getCurrent();

     if (this->isChecked) return;
     if (context->hasClient)
     {
       sendServerAttribut(globalDim, orderPositionInGrid, distType);
       sendValue();
     }

     this->isChecked = true;
   }

  void CAxis::sendValue()
  {
     if (ni.getValue() == size.getValue())
     {
       sendNonDistributedValue();
     }
     else
     {
       computeConnectedServer();
       sendDistributedValue();
     }
  }

  void CAxis::computeConnectedServer()
  {
    CContext* context = CContext::getCurrent();
    CContextClient* client = context->client;
    int nbServer = client->serverSize;
    int range, clientSize = client->clientSize;

    CArray<size_t,1> globalIndexAxis(ni);
    size_t ibegin = this->ibegin.getValue();
    int zoom_end = global_zoom_begin+global_zoom_size-1;
    std::vector<size_t> globalAxisZoom;
    for (size_t idx = 0; idx < ni; ++idx)
    {
      size_t globalIndex = ibegin + idx;
      globalIndexAxis(idx) = globalIndex;
      if (globalIndex >= global_zoom_begin && globalIndex <= zoom_end) globalAxisZoom.push_back(globalIndex);
    }

    std::vector<int> nGlobDomain(1);
    nGlobDomain[0] = size.getValue();

    size_t globalSizeIndex = 1, indexBegin, indexEnd;
    for (int i = 0; i < nGlobDomain.size(); ++i) globalSizeIndex *= nGlobDomain[i];
    indexBegin = 0;
    for (int i = 0; i < clientSize; ++i)
    {
      range = globalSizeIndex / clientSize;
      if (i < (globalSizeIndex%clientSize)) ++range;
      if (i == client->clientRank) break;
      indexBegin += range;
    }
    indexEnd = indexBegin + range - 1;

    CServerDistributionDescription serverDescription(nGlobDomain);
    serverDescription.computeServerGlobalIndexInRange(nbServer, std::make_pair<size_t,size_t>(indexBegin, indexEnd), 0);
    CClientServerMapping* clientServerMap = new CClientServerMappingDistributed(serverDescription.getGlobalIndexRange(), client->intraComm);
    clientServerMap->computeServerIndexMapping(globalIndexAxis);
    const std::map<int, std::vector<size_t> >& globalIndexAxisOnServer = clientServerMap->getGlobalIndexOnServer();

    std::map<int, std::vector<size_t> >::const_iterator it = globalIndexAxisOnServer.begin(),
                                                       ite = globalIndexAxisOnServer.end();
    std::vector<size_t>::const_iterator itbVec = (globalAxisZoom).begin(),
                                        iteVec = (globalAxisZoom).end();
    indSrv_.clear();
    for (; it != ite; ++it)
    {
      int rank = it->first;
      const std::vector<size_t>& globalIndexTmp = it->second;
      int nb = globalIndexTmp.size();

      for (int i = 0; i < nb; ++i)
      {
        if (std::binary_search(itbVec, iteVec, globalIndexTmp[i]))
        {
          indSrv_[rank].push_back(globalIndexTmp[i]);
        }
      }
    }

    connectedServerRank_.clear();
    for (it = globalIndexAxisOnServer.begin(); it != ite; ++it) {
      connectedServerRank_.push_back(it->first);
    }

    if (!indSrv_.empty())
    {
      connectedServerRank_.clear();
      for (it = indSrv_.begin(); it != indSrv_.end(); ++it)
        connectedServerRank_.push_back(it->first);
    }
    nbConnectedClients_ = clientServerMap->computeConnectedClients(client->serverSize, client->clientSize, client->intraComm, connectedServerRank_);
    delete clientServerMap;
  }

  void CAxis::sendNonDistributedValue()
  {
    CContext* context = CContext::getCurrent();
    CContextClient* client = context->client;
    CEventClient event(getType(),EVENT_ID_NON_DISTRIBUTED_VALUE);

    int zoom_end = global_zoom_begin+global_zoom_size-1;
    int nb =0;
    for (size_t idx = 0; idx < ni; ++idx)
    {
      size_t globalIndex = ibegin + idx;
      if (globalIndex >= global_zoom_begin && globalIndex <= zoom_end) ++nb;
    }

    CArray<double,1> val(nb);
    nb = 0;
    for (size_t idx = 0; idx < ni; ++idx)
    {
      size_t globalIndex = ibegin + idx;
      if (globalIndex >= global_zoom_begin && globalIndex <= zoom_end)
      {
        val(nb) = value(idx);
        ++nb;
      }
    }

    if (client->isServerLeader())
    {
      std::list<CMessage> msgs;

      const std::list<int>& ranks = client->getRanksServerLeader();
      for (std::list<int>::const_iterator itRank = ranks.begin(), itRankEnd = ranks.end(); itRank != itRankEnd; ++itRank)
      {
        // Use const int to ensure CMessage holds a copy of the value instead of just a reference
        msgs.push_back(CMessage());
        CMessage& msg = msgs.back();
        msg << this->getId();
        msg << val;
        event.push(*itRank,1,msg);
      }
      client->sendEvent(event);
    }
    else client->sendEvent(event);
  }

  void CAxis::sendDistributedValue(void)
  {
    int ns, n, i, j, ind, nv, idx;
    CContext* context = CContext::getCurrent();
    CContextClient* client=context->client;

    // send value for each connected server
    CEventClient eventIndex(getType(), EVENT_ID_INDEX);
    CEventClient eventVal(getType(), EVENT_ID_DISTRIBUTED_VALUE);

    list<CMessage> list_msgsIndex, list_msgsVal;
    list<CArray<int,1> > list_indi;
    list<CArray<double,1> > list_val;
    list<CArray<double,2> > list_bounds;

    std::map<int, std::vector<size_t> >::const_iterator it, iteMap;
    iteMap = indSrv_.end();
    for (int k = 0; k < connectedServerRank_.size(); ++k)
    {
      int nbData = 0;
      int rank = connectedServerRank_[k];
      it = indSrv_.find(rank);
      if (iteMap != it)
        nbData = it->second.size();

      list_indi.push_back(CArray<int,1>(nbData));
      list_val.push_back(CArray<double,1>(nbData));

      if (hasBounds_)
      {
        list_bounds.push_back(CArray<double,2>(2,nbData));
      }

      CArray<int,1>& indi = list_indi.back();
      CArray<double,1>& val = list_val.back();

      for (n = 0; n < nbData; ++n)
      {
        idx = static_cast<int>(it->second[n]);
        ind = idx - ibegin;

        val(n) = value(ind);
        indi(n) = idx;

        if (hasBounds_)
        {
          CArray<double,2>& boundsVal = list_bounds.back();
          boundsVal(0, n) = bounds(0,n);
          boundsVal(1, n) = bounds(1,n);
        }
      }

      list_msgsIndex.push_back(CMessage());
      list_msgsIndex.back() << this->getId() << list_indi.back();

      list_msgsVal.push_back(CMessage());
      list_msgsVal.back() << this->getId() << list_val.back();

      if (hasBounds_)
      {
        list_msgsVal.back() << list_bounds.back();
      }

      eventIndex.push(rank, nbConnectedClients_[rank], list_msgsIndex.back());
      eventVal.push(rank, nbConnectedClients_[rank], list_msgsVal.back());
    }

    client->sendEvent(eventIndex);
    client->sendEvent(eventVal);
  }

  void CAxis::recvIndex(CEventServer& event)
  {
    list<CEventServer::SSubEvent>::iterator it;
    for (it = event.subEvents.begin(); it != event.subEvents.end(); ++it)
    {
      CBufferIn* buffer = it->buffer;
      string domainId;
      *buffer >> domainId;
      get(domainId)->recvIndex(it->rank, *buffer);
    }
  }

  void CAxis::recvIndex(int rank, CBufferIn& buffer)
  {
    buffer >> indiSrv_[rank];
  }

  void CAxis::recvDistributedValue(CEventServer& event)
  {
    list<CEventServer::SSubEvent>::iterator it;
    for (it = event.subEvents.begin(); it != event.subEvents.end(); ++it)
    {
      CBufferIn* buffer = it->buffer;
      string domainId;
      *buffer >> domainId;
      get(domainId)->recvDistributedValue(it->rank, *buffer);
    }
  }

  void CAxis::recvDistributedValue(int rank, CBufferIn& buffer)
  {
    CArray<int,1> &indi = indiSrv_[rank];
    CArray<double,1> val;
    CArray<double,2> boundsVal;

    buffer >> val;
    if (hasBounds_) buffer >> boundsVal;

    int i, j, ind_srv;
    for (int ind = 0; ind < indi.numElements(); ++ind)
    {
      i = indi(ind);
      ind_srv = i - zoom_begin_srv;
      value_srv(ind_srv) = val(ind);
      if (hasBounds_)
      {
        bound_srv(0,ind_srv) = boundsVal(0, ind);
        bound_srv(1,ind_srv) = boundsVal(1, ind);
      }
    }
  }

   void CAxis::recvNonDistributedValue(CEventServer& event)
  {
    list<CEventServer::SSubEvent>::iterator it;
    for (it = event.subEvents.begin(); it != event.subEvents.end(); ++it)
    {
      CBufferIn* buffer = it->buffer;
      string domainId;
      *buffer >> domainId;
      get(domainId)->recvNonDistributedValue(it->rank, *buffer);
    }
  }

  void CAxis::recvNonDistributedValue(int rank, CBufferIn& buffer)
  {
    CArray<double,1> val;
    buffer >> val;

    for (int ind = 0; ind < val.numElements(); ++ind)
    {
      value_srv(ind) = val(ind);
      if (hasBounds_)
      {
        bound_srv(0,ind) = bounds(0,ind);
        bound_srv(1,ind) = bounds(1,ind);
      }
    }
  }

  void CAxis::sendServerAttribut(const std::vector<int>& globalDim, int orderPositionInGrid,
                                 CServerDistributionDescription::ServerDistributionType distType)
  {
    CContext* context = CContext::getCurrent();
    CContextClient* client = context->client;

    CServerDistributionDescription serverDescription(globalDim);

    int nbServer = client->serverSize;

    serverDescription.computeServerDistribution(nbServer, false, distType);
    std::vector<std::vector<int> > serverIndexBegin = serverDescription.getServerIndexBegin();
    std::vector<std::vector<int> > serverDimensionSizes = serverDescription.getServerDimensionSizes();

    CEventClient event(getType(),EVENT_ID_SERVER_ATTRIBUT);
    if (client->isServerLeader())
    {
      std::list<CMessage> msgs;

      const std::list<int>& ranks = client->getRanksServerLeader();
      for (std::list<int>::const_iterator itRank = ranks.begin(), itRankEnd = ranks.end(); itRank != itRankEnd; ++itRank)
      {
        // Use const int to ensure CMessage holds a copy of the value instead of just a reference
        const int begin = serverIndexBegin[*itRank][orderPositionInGrid];
        const int ni    = serverDimensionSizes[*itRank][orderPositionInGrid];
        const int end   = begin + ni - 1;

        msgs.push_back(CMessage());
        CMessage& msg = msgs.back();
        msg << this->getId();
        msg << ni << begin << end;
        msg << global_zoom_begin << global_zoom_size;

        event.push(*itRank,1,msg);
      }
      client->sendEvent(event);
    }
    else client->sendEvent(event);
  }

  void CAxis::recvServerAttribut(CEventServer& event)
  {
    CBufferIn* buffer = event.subEvents.begin()->buffer;
    string axisId;
    *buffer >> axisId;
    get(axisId)->recvServerAttribut(*buffer);
  }

  void CAxis::recvServerAttribut(CBufferIn& buffer)
  {
    int ni_srv, begin_srv, end_srv, global_zoom_begin_tmp, global_zoom_size_tmp;

    buffer>>ni_srv>>begin_srv>>end_srv>>global_zoom_begin_tmp>>global_zoom_size_tmp;
    global_zoom_begin = global_zoom_begin_tmp;
    global_zoom_size  = global_zoom_size_tmp;
    int global_zoom_end = global_zoom_begin + global_zoom_size - 1;

    zoom_begin_srv = global_zoom_begin > begin_srv ? global_zoom_begin : begin_srv ;
    zoom_end_srv   = global_zoom_end < end_srv ? global_zoom_end : end_srv ;
    zoom_size_srv  = zoom_end_srv - zoom_begin_srv + 1;

    if (zoom_size_srv<=0)
    {
      zoom_begin_srv = 0; zoom_end_srv = 0; zoom_size_srv = 0;
    }

    if (size == ni)
    {
      zoom_begin_srv = global_zoom_begin;
      zoom_end_srv   = global_zoom_end; //zoom_end;
      zoom_size_srv  = zoom_end_srv - zoom_begin_srv + 1;
    }
    value_srv.resize(zoom_size_srv);
    bound_srv.resize(2,zoom_size_srv);
  }

  bool CAxis::hasTransformation()
  {
    return (!transformationMap_.empty());
  }

  void CAxis::setTransformations(const TransMapTypes& axisTrans)
  {
    transformationMap_ = axisTrans;
  }

  CAxis::TransMapTypes CAxis::getAllTransformations(void)
  {
    return transformationMap_;
  }

  /*!
    Check the validity of all transformations applied on axis
  This functions is called AFTER all inherited attributes are solved
  */
  void CAxis::checkTransformations()
  {
    TransMapTypes::const_iterator itb = transformationMap_.begin(), it,
                                  ite = transformationMap_.end();
    for (it = itb; it != ite; ++it)
    {
      (it->second)->checkValid(this);
    }
  }

  void CAxis::solveInheritanceTransformation()
  {
    if (this->hasTransformation()) return;

    std::vector<CAxis*> refAxis;
    CAxis* refer_sptr;
    CAxis* refer_ptr = this;
    while (refer_ptr->hasDirectAxisReference())
    {
      refAxis.push_back(refer_ptr);
      refer_sptr = refer_ptr->getDirectAxisReference();
      refer_ptr  = refer_sptr;
      if (refer_ptr->hasTransformation()) break;
    }

    if (refer_ptr->hasTransformation())
      for (int idx = 0; idx < refAxis.size(); ++idx)
        refAxis[idx]->setTransformations(refer_ptr->getAllTransformations());
  }

  void CAxis::parse(xml::CXMLNode & node)
  {
    SuperClass::parse(node);

    if (node.goToChildElement())
    {
      StdString inverseAxisDefRoot("inverse_axis_definition");
      StdString inverse("inverse_axis");
      StdString zoomAxisDefRoot("zoom_axis_definition");
      StdString zoom("zoom_axis");
      StdString interpAxisDefRoot("interpolate_axis_definition");
      StdString interp("interpolate_axis");
      do
      {
        if (node.getElementName() == inverse) {
          CInverseAxis* tmp = (CInverseAxisGroup::get(inverseAxisDefRoot))->createChild();
          tmp->parse(node);
          transformationMap_.push_back(std::make_pair(TRANS_INVERSE_AXIS,tmp));
        } else if (node.getElementName() == zoom) {
          CZoomAxis* tmp = (CZoomAxisGroup::get(zoomAxisDefRoot))->createChild();
          tmp->parse(node);
          transformationMap_.push_back(std::make_pair(TRANS_ZOOM_AXIS,tmp));
        }
        else if (node.getElementName() == interp) {
          CInterpolateAxis* tmp = (CInterpolateAxisGroup::get(interpAxisDefRoot))->createChild();
          tmp->parse(node);
          transformationMap_.push_back(std::make_pair(TRANS_INTERPOLATE_AXIS,tmp));
        }
      } while (node.goToNextElement()) ;
      node.goToParentElement();
    }
  }

  DEFINE_REF_FUNC(Axis,axis)

   ///---------------------------------------------------------------

} // namespace xios
