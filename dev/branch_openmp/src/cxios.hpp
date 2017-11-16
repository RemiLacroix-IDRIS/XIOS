#ifndef __XIOS_HPP__
#define __XIOS_HPP__

#include "xios_spl.hpp"
#include "mpi.hpp"
#include "registry.hpp"

namespace xios
{
  /*!
  \class CXios
  */
  class CXios
  {
    public:
      static void initialize(void) ;
      static void initClientSide(const string & codeId, ep_lib::MPI_Comm& localComm, ep_lib::MPI_Comm& returnComm) ;
      static void initServerSide(void) ;
      static void clientFinalize(void) ;
      static void parseFile(const string& filename) ;

      template <typename T>
      static T getin(const string& id,const T& defaultValue) ;

      template <typename T>
      static T getin(const string& id) ;

    public:
      static const string rootFile ; //!< Configuration filename
      static const string xiosCodeId ; //!< Identity for XIOS
      static const string clientFile; //!< Filename template for client
      static const string serverFile; //!< Filename template for server
      //#pragma omp threadprivate(rootFile, xiosCodeId, clientFile, serverFile)

      static bool isClient ; //!< Check if xios is client
      #pragma omp threadprivate(isClient)
      static bool isServer ; //!< Check if xios is server
      #pragma omp threadprivate(isServer)

      static ep_lib::MPI_Comm globalComm ; //!< Global communicator
      #pragma omp threadprivate(globalComm)

      static bool printLogs2Files; //!< Printing out logs into files
      #pragma omp threadprivate(printLogs2Files)
      static bool usingOasis ; //!< Using Oasis
      #pragma omp threadprivate(usingOasis)
      static bool usingServer ; //!< Using server (server mode)
      #pragma omp threadprivate(usingServer)
      static double bufferSizeFactor; //!< Factor used to tune the buffer size
      #pragma omp threadprivate(bufferSizeFactor)
      static const double defaultBufferSizeFactor; //!< Default factor value
      static StdSize minBufferSize; //!< Minimum buffer size
      #pragma omp threadprivate(minBufferSize)
      static bool isOptPerformance; //!< Check if buffer size is for performance (as large as possible)
      #pragma omp threadprivate(isOptPerformance)
      static CRegistry* globalRegistry ; //!< global registry which is wrote by the root process of the servers
      #pragma omp threadprivate(globalRegistry)
      static double recvFieldTimeout; //!< Time to wait for data before issuing an error when receiving a field
      #pragma omp threadprivate(recvFieldTimeout)

    public:
      //! Setting xios to use server mode
      static void setUsingServer();

      //! Setting xios NOT to use server mode
      static void setNotUsingServer();

      //! Initialize server (if any)
      static  void initServer();

    private:
      //! Parse only Xios part of configuration file
      static void parseXiosConfig();
  } ;
}

//#include "cxios_impl.hpp"
#endif // __XIOS_HPP__
