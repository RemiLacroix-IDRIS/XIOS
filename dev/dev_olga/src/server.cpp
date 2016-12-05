#include "globalScopeData.hpp"
#include "xios_spl.hpp"
#include "cxios.hpp"
#include "server.hpp"
#include "client.hpp"
#include "type.hpp"
#include "context.hpp"
#include "object_template.hpp"
#include "oasis_cinterface.hpp"
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string.hpp>
#include "mpi.hpp"
#include "tracer.hpp"
#include "timer.hpp"
#include "event_scheduler.hpp"

namespace xios
{
    MPI_Comm CServer::intraComm ;
    list<MPI_Comm> CServer::interCommLeft ;
    list<MPI_Comm> CServer::interCommRight ;
    list<MPI_Comm> CServer::interComm ;
    std::list<MPI_Comm> CServer::contextInterComms;
    int CServer::nbSndSrvPools = (CXios::serverLevel == 0) ? 0 : 1;
    int CServer::poolNb = 0;
    bool CServer::isRoot = false ;
    int CServer::rank = INVALID_RANK;
    int CServer::rankSndServers = 0;
    StdOFStream CServer::m_infoStream;
    StdOFStream CServer::m_errorStream;
    map<string,CContext*> CServer::contextList ;
    bool CServer::finished=false ;
    bool CServer::is_MPI_Initialized ;
    CEventScheduler* CServer::eventScheduler = 0;

//---------------------------------------------------------------
/*!
 * \fn void CServer::initialize(void)
 * Function creates intra and inter comm for each initialized server pool
 */
    void CServer::initialize(void)
    {
      int initialized ;
      MPI_Initialized(&initialized) ;
      if (initialized) is_MPI_Initialized=true ;
      else is_MPI_Initialized=false ;


      // Not using OASIS
      if (!CXios::usingOasis)
      {

        if (!is_MPI_Initialized)
        {
          MPI_Init(NULL, NULL);
        }
        CTimer::get("XIOS").resume() ;

        boost::hash<string> hashString ;
        unsigned long hashServer1 = hashString(CXios::xiosCodeIdPrm);
        unsigned long hashServer2 = hashString(CXios::xiosCodeIdSnd);
        unsigned long hashServer = (CXios::serverLevel < 2)  ? hashServer1 : hashServer2;

        unsigned long* hashAll ;
        unsigned long* hashAllServers ;

//        int rank ;
        int size ;
        int myColor ;
        int i,c ;
        MPI_Comm newComm, serversInterComm;

        MPI_Comm_size(CXios::globalComm, &size) ;
        MPI_Comm_rank(CXios::globalComm, &rank);

        hashAll=new unsigned long[size] ;
        MPI_Allgather(&hashServer, 1, MPI_LONG, hashAll, 1, MPI_LONG, CXios::globalComm) ;

        map<unsigned long, int> colors, colorsServers ;
        map<unsigned long, int> leaders ;
        map<unsigned long, int>::iterator it ;
//        map<unsigned long, int> leadersServers ;
        vector<int> leadersServers;

        for(i=0,c=0;i<size;i++)
        {
          if (colors.find(hashAll[i])==colors.end())
          {
            colors[hashAll[i]]=c ;
            leaders[hashAll[i]]=i ;
            c++ ;
          }
        }

        nbSndSrvPools = size - leaders[hashServer2];    // one proc per secondary-server pool

        // Taking into account multiple pools on secondary server
        if (nbSndSrvPools > 1)
        {
          if (CXios::serverLevel > 1)
          {
            int nbProcs = size - leaders[hashServer2];
            int remain = nbProcs % nbSndSrvPools;
            int procsPerPool = nbProcs / nbSndSrvPools;
            rankSndServers = rank - leaders[hashServer2];
            StdString strTmp = CXios::xiosCodeIdSnd;

            if (remain == 0)
            {
              poolNb = rankSndServers/procsPerPool;
            }
            else
            {
              if (rankSndServers <= (procsPerPool + 1) * remain)
                poolNb = rankSndServers/(procsPerPool+1);
              else
              {
                poolNb = remain + 1;
                rankSndServers -= (procsPerPool + 1) * remain;
                rankSndServers -= procsPerPool;
                poolNb += rankSndServers/procsPerPool;
              }
            }
            strTmp += boost::lexical_cast<string>(poolNb+1);  // add 1 to avoid hashing zero
            hashServer = hashString(strTmp);
            hashServer2 = hashString(strTmp);
          }
        }

        if (nbSndSrvPools > 1)
        {
          myColor = size;
          MPI_Comm_split(MPI_COMM_WORLD, myColor, rank, &serversInterComm) ;
        }
        else
        {
          myColor=colors[hashServer] ;
          MPI_Comm_split(MPI_COMM_WORLD, myColor, rank, &intraComm) ;
        }

        if (nbSndSrvPools > 1)
        {
          int sizeServers;
//          int rankServers;
//          MPI_Comm_rank(serversInterComm, &rankServers) ;
          MPI_Comm_size(serversInterComm, &sizeServers) ;
          hashAllServers=new unsigned long[sizeServers] ;
          MPI_Allgather(&hashServer, 1, MPI_LONG, hashAllServers, 1, MPI_LONG, serversInterComm) ;

          for(i=0, c=0; i<sizeServers; i++)
          {
            if (colorsServers.find(hashAllServers[i])==colorsServers.end())
            {
              colorsServers[hashAllServers[i]]=c ;
//              leadersServers[hashAllServers[i]]= leaders[hashServer1] + i ;
              leadersServers.push_back( leaders[hashServer1] + i );
              c++ ;
            }
          }
          myColor=colorsServers[hashServer] ;
          MPI_Comm_split(serversInterComm, myColor, rank, &intraComm) ;
        }


        if (CXios::serverLevel == 0)
        {
          int clientLeader;
          for(it=leaders.begin();it!=leaders.end();it++)
          {
            if (it->first!=hashServer)
            {
              clientLeader=it->second ;
              int intraCommSize, intraCommRank ;
              MPI_Comm_size(intraComm,&intraCommSize) ;
              MPI_Comm_rank(intraComm,&intraCommRank) ;
              info(50)<<"intercommCreate::server "<<rank<<" intraCommSize : "<<intraCommSize
                       <<" intraCommRank :"<<intraCommRank<<"  clientLeader "<< clientLeader<<endl ;

              MPI_Intercomm_create(intraComm, 0, CXios::globalComm, clientLeader, 0, &newComm) ;
               interCommLeft.push_back(newComm) ;
               interComm.push_back(newComm) ;
            }
          }
        }
        else
        {
          if ((CXios::serverLevel == 1))
          {
            // Creating interComm with client (interCommLeft)
            int clientLeader;
            int srvSndLeader;
            for(it=leaders.begin();it!=leaders.end();it++)
            {
              if (it->first != hashServer2)
              {
                if (it->first != hashServer1)
                {
                  clientLeader=it->second ;
                  int intraCommSize, intraCommRank ;
                  MPI_Comm_size(intraComm, &intraCommSize) ;
                  MPI_Comm_rank(intraComm, &intraCommRank) ;
                  info(50)<<"intercommCreate::server "<<rank<<" intraCommSize : "<<intraCommSize
                           <<" intraCommRank :"<<intraCommRank<<"  clientLeader "<< clientLeader<<endl ;
                  MPI_Intercomm_create(intraComm, 0, CXios::globalComm, clientLeader, 0, &newComm) ;
                  interCommLeft.push_back(newComm) ;
                  interComm.push_back(newComm) ;
                }
              }
              else
              {
                srvSndLeader = it->second;
              }
            }

            // Creating interComm with secondary server pool(s) (interCommRight)
//            if (nbSndSrvPools < 1)
            if (nbSndSrvPools < 2)
            {
              CClient::initializeClientOnServer(rank, intraComm, srvSndLeader);
              interCommRight.push_back(CClient::getInterComm());
              interComm.push_back(CClient::getInterComm());
            }
            else
            {
//              for(it=leadersServers.begin();it!=leadersServers.end();it++)
//              {
//                if (it->first != hashServer)
//                {
//                  srvSndLeader = it->second;
//                  CClient::initializeClientOnServer(rank, intraComm, srvSndLeader);
//                  interCommRight.push_back(CClient::getInterComm());
//                  interComm.push_back(CClient::getInterComm());
//                }
//              }

              for(int i = 1; i < leadersServers.size(); ++i)
              {
                srvSndLeader = leadersServers[i];
                CClient::initializeClientOnServer(rank, intraComm, srvSndLeader);
                interCommRight.push_back(CClient::getInterComm());
                interComm.push_back(CClient::getInterComm());
              }
            }
          } // primary server

          else // secondary server pool(s)
          {
            int clientLeader;
            if (nbSndSrvPools < 2)
//            if (nbSndSrvPools < 1)
            {
              for(it=leaders.begin();it!=leaders.end();it++)
              {
                if (it->first == hashServer1)
                {
                  clientLeader=it->second ;
                  int intraCommSize, intraCommRank ;
                  MPI_Comm_size(intraComm, &intraCommSize) ;
                  MPI_Comm_rank(intraComm, &intraCommRank) ;
                  info(50)<<"intercommCreate::server "<<rank<<" intraCommSize : "<<intraCommSize
                           <<" intraCommRank :"<<intraCommRank<<"  clientLeader "<< clientLeader<<endl ;

                  MPI_Intercomm_create(intraComm, 0, CXios::globalComm, clientLeader, 0, &newComm) ;
                  interCommLeft.push_back(newComm) ;
                  interComm.push_back(newComm) ;

                  break;
                }
              }
            }
            else
            {
//              for(it=leadersServers.begin();it!=leadersServers.end();it++)
              {
 //               if (it->first == hashServer1)
                {
//                  clientLeader=it->second ;
                  clientLeader = leadersServers[0];
                  int intraCommSize, intraCommRank ;
                  MPI_Comm_size(intraComm, &intraCommSize) ;
                  MPI_Comm_rank(intraComm, &intraCommRank) ;
                  info(50)<<"intercommCreate::server "<<rank<<" intraCommSize : "<<intraCommSize
                           <<" intraCommRank :"<<intraCommRank<<"  clientLeader "<< clientLeader<<endl ;

                  MPI_Intercomm_create(intraComm, 0, CXios::globalComm, clientLeader, 0, &newComm) ;
                  interCommLeft.push_back(newComm) ;
                  interComm.push_back(newComm) ;
                }
              }
            }
          } // secondary server
        } // CXios::serverLevel != 0

        if (nbSndSrvPools > 1) delete [] hashAllServers;
        delete [] hashAll ;

      }
      // using OASIS
      else
      {
//        int rank ,size;
        int size;
        if (!is_MPI_Initialized) oasis_init(CXios::xiosCodeId);

        CTimer::get("XIOS").resume() ;
        MPI_Comm localComm;
        oasis_get_localcomm(localComm);
        MPI_Comm_dup(localComm, &intraComm);

        MPI_Comm_rank(intraComm,&rank) ;
        MPI_Comm_size(intraComm,&size) ;
        string codesId=CXios::getin<string>("oasis_codes_id") ;

        vector<string> splitted ;
        boost::split( splitted, codesId, boost::is_any_of(","), boost::token_compress_on ) ;
        vector<string>::iterator it ;

        MPI_Comm newComm ;
        int globalRank ;
        MPI_Comm_rank(CXios::globalComm,&globalRank);

        for(it=splitted.begin();it!=splitted.end();it++)
        {
          oasis_get_intercomm(newComm,*it) ;
          if (rank==0) MPI_Send(&globalRank,1,MPI_INT,0,0,newComm) ;
          MPI_Comm_remote_size(newComm,&size);
          interComm.push_back(newComm) ;
        }
	      oasis_enddef() ;
      }

      MPI_Comm_rank(intraComm, &rank) ;
      if (rank==0) isRoot=true;
      else isRoot=false;
      
      eventScheduler = new CEventScheduler(intraComm) ;
    }

    void CServer::finalize(void)
    {

      CTimer::get("XIOS").suspend() ;
     
      delete eventScheduler ;

      for (std::list<MPI_Comm>::iterator it = contextInterComms.begin(); it != contextInterComms.end(); it++)
        MPI_Comm_free(&(*it));

//      for (std::list<MPI_Comm>::iterator it = interComm.begin(); it != interComm.end(); it++)
//        MPI_Comm_free(&(*it));

      for (std::list<MPI_Comm>::iterator it = interCommLeft.begin(); it != interCommLeft.end(); it++)
        MPI_Comm_free(&(*it));

      for (std::list<MPI_Comm>::iterator it = interCommRight.begin(); it != interCommRight.end(); it++)
        MPI_Comm_free(&(*it));

      MPI_Comm_free(&intraComm);

      if (!is_MPI_Initialized)
      {
        if (CXios::usingOasis) oasis_finalize();
        else MPI_Finalize() ;
      }
      report(0)<<"Performance report : Time spent for XIOS : "<<CTimer::get("XIOS server").getCumulatedTime()<<endl  ;
      report(0)<<"Performance report : Time spent in processing events : "<<CTimer::get("Process events").getCumulatedTime()<<endl  ;
      report(0)<<"Performance report : Ratio : "<<CTimer::get("Process events").getCumulatedTime()/CTimer::get("XIOS server").getCumulatedTime()*100.<<"%"<<endl  ;
    }

     void CServer::eventLoop(void)
     {
       bool stop=false ;

       CTimer::get("XIOS server").resume() ;
       while(!stop)
       {

         if (isRoot)
         {
           listenContext();
           if (!finished) listenFinalize() ;
         }
         else
         {
           listenRootContext();
           if (!finished) listenRootFinalize() ;
         }

         contextEventLoop() ;
         if (finished && contextList.empty()) stop=true ;
         eventScheduler->checkEvent() ;

       }
       CTimer::get("XIOS server").suspend() ;
     }

     void CServer::listenFinalize(void)
     {
        list<MPI_Comm>::iterator it, itr;
        int msg ;
        int flag ;

        for(it=interCommLeft.begin();it!=interCommLeft.end();it++)
        {
           MPI_Status status ;
           traceOff() ;
           MPI_Iprobe(0,0,*it,&flag,&status) ;
           traceOn() ;
           if (flag==true)
           {
             MPI_Recv(&msg,1,MPI_INT,0,0,*it,&status) ;
             info(20)<<" CServer : Receive client finalize"<<endl ;

             // If primary server, send finalize to secondary server pool(s)
             for(itr=interCommRight.begin(); itr!=interCommRight.end(); itr++)
             {
               MPI_Send(&msg,1,MPI_INT,0,0,*itr) ;

//               MPI_Comm_free(&(*itr));
//               interCommRight.erase(itr) ;
             }

              MPI_Comm_free(&(*it));
//              interComm.erase(it) ;
              interCommLeft.erase(it) ;
              break ;
            }
         }

        if (interCommLeft.empty())
//        if (interComm.empty())
         {
           int i,size ;
           MPI_Comm_size(intraComm,&size) ;
           MPI_Request* requests= new MPI_Request[size-1] ;
           MPI_Status* status= new MPI_Status[size-1] ;

           for(int i=1;i<size;i++) MPI_Isend(&msg,1,MPI_INT,i,4,intraComm,&requests[i-1]) ;
           MPI_Waitall(size-1,requests,status) ;

           finished=true ;
           delete [] requests ;
           delete [] status ;
         }
     }


     void CServer::listenRootFinalize()
     {
        int flag ;
        MPI_Status status ;
        int msg ;

        traceOff() ;
        MPI_Iprobe(0,4,intraComm, &flag, &status) ;
        traceOn() ;
        if (flag==true)
        {
           MPI_Recv(&msg,1,MPI_INT,0,4,intraComm,&status) ;
           finished=true ;
        }
      }

     void CServer::listenContext(void)
     {

       MPI_Status status ;
       int flag ;
       static void* buffer ;
       static MPI_Request request ;
       static bool recept=false ;
       int rank ;
       int count ;

       if (recept==false)
       {
         traceOff() ;
         MPI_Iprobe(MPI_ANY_SOURCE,1,CXios::globalComm, &flag, &status) ;
         traceOn() ;
         if (flag==true)
         {
           rank=status.MPI_SOURCE ;
           MPI_Get_count(&status,MPI_CHAR,&count) ;
           buffer=new char[count] ;
           MPI_Irecv(buffer,count,MPI_CHAR,rank,1,CXios::globalComm,&request) ;
           recept=true ;
         }
       }
       else
       {
         traceOff() ;
         MPI_Test(&request,&flag,&status) ;
         traceOn() ;
         if (flag==true)
         {
           rank=status.MPI_SOURCE ;
           MPI_Get_count(&status,MPI_CHAR,&count) ;
           recvContextMessage(buffer,count) ;
           delete [] buffer ;
           recept=false ;
         }
       }
     }

     void CServer::recvContextMessage(void* buff,int count)
     {
       static map<string,contextMessage> recvContextId;

       map<string,contextMessage>::iterator it ;

       CBufferIn buffer(buff,count) ;
       string id ;
       int clientLeader ;
       int nbMessage ;

       buffer>>id>>nbMessage>>clientLeader ;

       it=recvContextId.find(id) ;
       if (it==recvContextId.end())
       {
         contextMessage msg={0,0} ;
         pair<map<string,contextMessage>::iterator,bool> ret ;
         ret=recvContextId.insert(pair<string,contextMessage>(id,msg)) ;
         it=ret.first ;
       }
       it->second.nbRecv+=1 ;
       it->second.leaderRank+=clientLeader ;

       if (it->second.nbRecv==nbMessage)
       {
         int size ;
         MPI_Comm_size(intraComm,&size) ;
         MPI_Request* requests= new MPI_Request[size-1] ;
         MPI_Status* status= new MPI_Status[size-1] ;

         for(int i=1;i<size;i++)
         {
            MPI_Isend(buff,count,MPI_CHAR,i,2,intraComm,&requests[i-1]) ;
         }
         MPI_Waitall(size-1,requests,status) ;
         registerContext(buff,count,it->second.leaderRank) ;

         recvContextId.erase(it) ;
         delete [] requests ;
         delete [] status ;

       }
     }

     void CServer::listenRootContext(void)
     {

       MPI_Status status ;
       int flag ;
       static void* buffer ;
       static MPI_Request request ;
       static bool recept=false ;
       int rank ;
       int count ;
       const int root=0 ;

       if (recept==false)
       {
         traceOff() ;
         MPI_Iprobe(root,2,intraComm, &flag, &status) ;
         traceOn() ;
         if (flag==true)
         {
           MPI_Get_count(&status,MPI_CHAR,&count) ;
           buffer=new char[count] ;
           MPI_Irecv(buffer,count,MPI_CHAR,root,2,intraComm,&request) ;
           recept=true ;
         }
       }
       else
       {
         MPI_Test(&request,&flag,&status) ;
         if (flag==true)
         {
           MPI_Get_count(&status,MPI_CHAR,&count) ;
           registerContext(buffer,count) ;

           delete [] buffer ;
           recept=false ;
         }
       }
     }

     void CServer::registerContext(void* buff, int count, int leaderRank)
     {
       string contextId;
       CBufferIn buffer(buff, count);
       buffer >> contextId;
       CContext* context;

       info(20) << "CServer : Register new Context : " << contextId << endl;

       if (contextList.find(contextId) != contextList.end())
         ERROR("void CServer::registerContext(void* buff, int count, int leaderRank)",
               << "Context '" << contextId << "' has already been registred");

       MPI_Comm contextInterComm;
       MPI_Intercomm_create(intraComm,0,CXios::globalComm,leaderRank,10+leaderRank,&contextInterComm);

       MPI_Comm inter;
       MPI_Intercomm_merge(contextInterComm,1,&inter);
       MPI_Barrier(inter);

       context=CContext::create(contextId);
       contextList[contextId]=context;
       context->initServer(intraComm,contextInterComm);
       contextInterComms.push_back(contextInterComm);

       if (CXios::serverLevel == 1)
       {
//         CClient::registerContext(contextId, intraComm);
         CClient::registerContextOnSrvPools(contextId, intraComm);
       }

       MPI_Comm_free(&inter);

     }

     void CServer::contextEventLoop(void)
     {
       bool finished ;
       map<string,CContext*>::iterator it ;

       for(it=contextList.begin();it!=contextList.end();it++)
       {
         finished=it->second->checkBuffersAndListen();
         if (finished)
         {
           contextList.erase(it) ;
           break ;
         }
       }
     }

     //! Get rank of the current process
     int CServer::getRank()
     {
       return rank;
     }

    /*!
    * Open a file specified by a suffix and an extension and use it for the given file buffer.
    * The file name will be suffix+rank+extension.
    * 
    * \param fileName[in] protype file name
    * \param ext [in] extension of the file
    * \param fb [in/out] the file buffer
    */
    void CServer::openStream(const StdString& fileName, const StdString& ext, std::filebuf* fb)
    {
      StdStringStream fileNameClient;
      int numDigit = 0;
      int size = 0;
      int mpiRank;
      MPI_Comm_size(CXios::globalComm, &size);
      while (size)
      {
        size /= 10;
        ++numDigit;
      }

      if (nbSndSrvPools < 2)
        mpiRank = getRank();
      else
        mpiRank = rankSndServers;
      fileNameClient << fileName << "_" << std::setfill('0') << std::setw(numDigit) << mpiRank << ext;
      fb->open(fileNameClient.str().c_str(), std::ios::out);
      if (!fb->is_open())
        ERROR("void CServer::openStream(const StdString& fileName, const StdString& ext, std::filebuf* fb)",
              << std::endl << "Can not open <" << fileNameClient << "> file to write the server log(s).");
    }

    /*!
    * \brief Open a file stream to write the info logs
    * Open a file stream with a specific file name suffix+rank
    * to write the info logs.
    * \param fileName [in] protype file name
    */
    void CServer::openInfoStream(const StdString& fileName)
    {
      std::filebuf* fb = m_infoStream.rdbuf();
      openStream(fileName, ".out", fb);

      info.write2File(fb);
      report.write2File(fb);
    }

    //! Write the info logs to standard output
    void CServer::openInfoStream()
    {
      info.write2StdOut();
      report.write2StdOut();
    }

    //! Close the info logs file if it opens
    void CServer::closeInfoStream()
    {
      if (m_infoStream.is_open()) m_infoStream.close();
    }

    /*!
    * \brief Open a file stream to write the error log
    * Open a file stream with a specific file name suffix+rank
    * to write the error log.
    * \param fileName [in] protype file name
    */
    void CServer::openErrorStream(const StdString& fileName)
    {
      std::filebuf* fb = m_errorStream.rdbuf();
      openStream(fileName, ".err", fb);

      error.write2File(fb);
    }

    //! Write the error log to standard error output
    void CServer::openErrorStream()
    {
      error.write2StdErr();
    }

    //! Close the error log file if it opens
    void CServer::closeErrorStream()
    {
      if (m_errorStream.is_open()) m_errorStream.close();
    }
}
