#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "xios_spl.hpp"
#include "mpi.hpp"

namespace xios
{
  class CClient
  {
    public:
      static void initialize(const string& codeId, ep_lib::MPI_Comm& localComm, ep_lib::MPI_Comm& returnComm);
      static void finalize(void);
      static void registerContext(const string& id, ep_lib::MPI_Comm contextComm);

      static ep_lib::MPI_Comm intraComm;
      #pragma omp threadprivate(intraComm)
   
      static ep_lib::MPI_Comm interComm;
      #pragma omp threadprivate(interComm)

      //static std::list<MPI_Comm> contextInterComms;
      static std::list<ep_lib::MPI_Comm> *contextInterComms_ptr;
      #pragma omp threadprivate(contextInterComms_ptr)

      static int serverLeader;
      #pragma omp threadprivate(serverLeader)
      
      static bool is_MPI_Initialized ;
      #pragma omp threadprivate(is_MPI_Initialized)

      //! Get rank of the current process
      static int getRank();

      //! Open a file stream to write the info logs
      static void openInfoStream(const StdString& fileName);
      //! Write the info logs to standard output
      static void openInfoStream();
      //! Close the info logs file if it opens
      static void closeInfoStream();

      //! Open a file stream to write the error log
      static void openErrorStream(const StdString& fileName);
      //! Write the error log to standard error output
      static void openErrorStream();
      //! Close the error log file if it opens
      static void closeErrorStream();

    protected:
      static int rank;
      #pragma omp threadprivate(rank)

      static StdOFStream m_infoStream;
      #pragma omp threadprivate(m_infoStream)
      static StdOFStream m_errorStream;
      #pragma omp threadprivate(m_errorStream)

      static StdOFStream array_infoStream[16];

      static void openStream(const StdString& fileName, const StdString& ext, std::filebuf* fb);
  };
}

#endif
