/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

#ifndef __MPI_INTERFACE_HPP__
#define __MPI_INTERFACE_HPP__

/**
 * \file    mpi_interface.hpp
 * \brief   Gestion des communications MPI via une surcouche interne (entête).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    28 Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// M(essage) P(assing) I(nterface) headers
#include <mpi.h>

// C++ standard headers
#include <vector>

#endif //__XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
   /// \brief Espace de nommage pour les communications via la bibliothèque MPI.
   namespace comm
   {
      typedef MPI_Fint     MPI_F_Comm; /*!< \brief Identifiant de communicateur MPI (Fortran).   */
      typedef MPI_Fint    MPI_F_Group; /*!< \brief Identifiant de groupe MPI (Fortran).          */
      typedef MPI_Fint  MPI_F_Request; /*!< \brief Identifiant de requête MPI (Fortran).         */
      typedef MPI_Fint  *MPI_F_Status; /*!< \brief Identifiant de statut MPI (Fortran).          */
      typedef MPI_Fint MPI_F_DataType; /*!< \brief Identifiant de type de données MPI (Fortran). */
      
      /**
       * \class CMPIManager
       * \brief Surcouche interne de la bibliàothèque M(essage) P(assing) I(nterface).
       */
      class CMPIManager
      {
         public : // Initialisation & Finalisation
         
            static void Initialise(int * argc, char *** argv);
            static void Finalize(void);

         public : // Communicateurs
         
            static int GetCommRank(MPI_Comm _comm = CMPIManager::GetCommWorld());
            static int GetCommSize(MPI_Comm _comm = CMPIManager::GetCommWorld());
            static MPI_Comm CreateComm(MPI_Group _group, MPI_Comm _pcomm = CMPIManager::GetCommWorld());
            static MPI_Comm GetCommWorld(void);

         public : // Autre
         
            static void Barrier(MPI_Comm _comm = CMPIManager::GetCommWorld());
            
            static bool DispatchClient(bool       _is_server,
                                       MPI_Comm & _comm_client,
                                       MPI_Comm & _comm_client_server,
                                       MPI_Comm & _comm_server,
                                       MPI_Comm   _comm_parent = CMPIManager::GetCommWorld());

         public : // Groupes
         
            static MPI_Group GetGroupWorld(void);
            static MPI_Group CreateSubGroup(MPI_Group _pgroup, const std::vector<int> & _ranks);
            static MPI_Group CreateSubGroup(MPI_Group _pgroup, int _min_rank, int _max_rank, int _intval = 1);

         public : // Tests 
         
            static bool IsMaster(MPI_Comm _comm = CMPIManager::GetCommWorld());
            static bool IsRank(int _rank, MPI_Comm _comm = CMPIManager::GetCommWorld());

         public : // Communication simple
         
            static void Send (MPI_Comm _comm, int _dest_rank, char * _data,
                              std::size_t _size, MPI_Request & _request);
            static void Wait (MPI_Request & _request);
            static bool Test (MPI_Request & _request);

            static bool HasReceivedData(MPI_Comm _comm, int _src_rank);
            static std::size_t GetReceivedDataSize(MPI_Comm _comm, int _src_rank);
            static void Receive(MPI_Comm _comm, int _src_rank, char * _data);
            
            static void AllGather(int _indata, std::vector<int> & _outdata,
                                  MPI_Comm _comm = CMPIManager::GetCommWorld());

            static void AllGather(const std::vector<int> & _indata,
                                        std::vector<int> & _outdata,
                                  MPI_Comm _comm = CMPIManager::GetCommWorld());


         public : // Communication 'complexe'
         
            /*static void SendLinearBuffer(MPI_Comm _comm, int _dest_rank, CLinearBuffer & _lbuffer, MPI_Request & _request);
            static void ReceiveLinearBuffer(MPI_Comm _comm, int _src_rank, CLinearBuffer & _lbuffer);
            static boost::shared_ptr<CLinearBuffer> ReceiveLinearBuffer(MPI_Comm _comm, int _src_rank);
            static void ReceiveCircularBuffer(MPI_Comm _comm, int _src_rank, CCircularBuffer & _cbuffer);*/

         public : // Mémoire (non fonctionnel ....)
         
            static void AllocMemory(void * _data, std::size_t _size);
            static void FreeMemory (void * _data);
         
      }; // class CMPIManager
      
   } // namespace comm
} // namespace xmlioserver

#endif //__MPI_INTERFACE_HPP__
