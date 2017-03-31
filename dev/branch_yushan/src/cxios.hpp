#ifndef __XIOS_HPP__
#define __XIOS_HPP__

#include "xios_spl.hpp"
#include "mpi.hpp"
#include "registry.hpp"
#ifdef _usingEP
//#include "ep_declaration.hpp"
#endif


namespace xios
{
  /*!
  \class CXios
  */
  class CXios
  {
    public:
      CXios();
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
      static const string rootFile; //!< Configuration filename

      static const string xiosCodeId ; //!< Identity for XIOS

      static const string clientFile; //!< Filename template for client

      static const string serverFile; //!< Filename template for server

      static bool isClient ; //!< Check if xios is client

      static bool isServer ; //!< Check if xios is server

      #pragma omp threadprivate(isClient, isServer)

      static MPI_Comm globalComm ; //!< Global communicator
      #pragma omp threadprivate(globalComm)

      static bool printLogs2Files; //!< Printing out logs into files

      static bool usingOasis ; //!< Using Oasis

      static bool usingServer ; //!< Using server (server mode)

      static double bufferSizeFactor; //!< Factor used to tune the buffer size

      static const double defaultBufferSizeFactor; //!< Default factor value

      static StdSize minBufferSize; //!< Minimum buffer size

      static bool isOptPerformance; //!< Check if buffer size is for performance (as large as possible)

      #pragma omp threadprivate(printLogs2Files, usingOasis, usingServer, bufferSizeFactor, minBufferSize, isOptPerformance)
      
      static CRegistry* globalRegistry ; //!< global registry which is wrote by the root process of the servers

      
      
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
