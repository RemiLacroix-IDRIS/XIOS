#ifndef __CONTEXT_SERVER_HPP__
#define __CONTEXT_SERVER_HPP__
#include "xios_spl.hpp"
#include "event_server.hpp"
#include "buffer_server.hpp"
#include "mpi.hpp"

namespace xios
{
  class CContext ;

  class CContextServer
  {
    public:

    CContextServer(CContext* parent, ep_lib::MPI_Comm intraComm, ep_lib::MPI_Comm interComm) ;
    bool eventLoop(bool enableEventsProcessing = true);
    void listen(void) ;
    bool listenPendingRequest(ep_lib::MPI_Status& status);
    void checkPendingRequest(void) ;
    void processRequest(int rank, char* buff,int count) ;
    void processEvents(void) ;
    void dispatchEvent(CEventServer& event) ;
    void setPendingEvent(void) ;
    bool hasPendingEvent(void) ;
    bool hasFinished(void);

    ep_lib::MPI_Comm intraComm ;
    int intraCommSize ;
    int intraCommRank ;

    ep_lib::MPI_Comm interComm ;
    int commSize ;

    map<int,CServerBuffer*> buffers ;
    map<int,ep_lib::MPI_Request> pendingRequest ;
    map<int,char*> bufferRequest ;

    map<size_t,CEventServer*> events ;
    size_t currentTimeLine ;
    CContext* context ;
      
    bool finished ;
    bool pendingEvent ;
    bool scheduled  ;    /*!< event of current timeline is alreading scheduled ? */
    size_t hashId ;
    ~CContextServer() ;

    private:
      std::map<int, StdSize> mapBufferSize_;
  } ;

}

#endif
