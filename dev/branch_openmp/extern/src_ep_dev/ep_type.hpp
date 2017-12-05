#ifndef EP_TYPE_HPP_INCLUDED
#define EP_TYPE_HPP_INCLUDED


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <map>
#include <omp.h>
#include <vector>
#include <numeric>
#include <bitset>
//#include <memory.h>
#include <algorithm>
#include <assert.h>
#include <math.h>
#include <string.h>

#ifdef _Debug
#define Debug(x) std::cout << x << std::endl
#else
#define Debug(x)
#endif

#define BUFFER_SIZE 10000

//#define NUM_THREADS 12

typedef std::pair< int, int > SIZE_RANK_INFO; // < rank, size>

typedef std::vector< std::pair<int, int> > RANK_MAP;  // at(ep_rank) = <ep_rank_local, mpi_rank>

typedef std::vector<std::pair< std::pair<int, int>, std::pair<int, int> > > INTERCOMM_RANK_MAP;


typedef struct
{
  int first;
  int second;
  int third;
} Triple_int;

namespace ep_lib
{
  //#define EP_UNDEFINED -32766

  class ep_communicator;
  class ep_intercomm;
  class OMPbarrier;
  typedef ep_communicator* EP_Comm;
  class MPI_Comm;


  class MPI_Status
  {
    public:

      int ep_datatype;
      int ep_src;
      int ep_tag;
      void* mpi_status;
  };

  class MPI_Message
  {
    public:

      int mpi_message;
      int ep_src;
      int ep_tag;
      void* mpi_status;

      MPI_Message() {}
      MPI_Message(int message): mpi_message(message) {}
  };

  typedef std::list<MPI_Message > Message_list;


  class OMPbarrier
  {
    private:
      int nbThreads;          //<The number of threads for this barrier
      int currentNbThread;    //<The current number of threads waiting
      bool sense;             //<Direct barrier feedback protection
      omp_lock_t mutex;       //<To have an atomic int

      OMPbarrier(OMPbarrier&){}
      OMPbarrier& operator=(OMPbarrier&){return *this;}

    public:
      /** Constructor with the number of threads */
      explicit OMPbarrier(const int inNbThreads)
          : nbThreads(inNbThreads), currentNbThread(0), sense(false) {
          omp_init_lock( &mutex );
      }

      /** Destructor, release the omp lock */
      ~OMPbarrier(){
          omp_destroy_lock( &mutex );
      }

      /** Perform a barrier */
      void wait(){
          const bool mySense = sense;
          omp_set_lock( &mutex );
          const int nbThreadsArrived = (++currentNbThread);
          omp_unset_lock( &mutex );

          if(nbThreadsArrived == nbThreads) {
              currentNbThread = 0;
              sense = !sense;
              #pragma omp flush
          }
          else {
              volatile const bool* const ptSense = &sense;
              while( (*ptSense) == mySense){
              }
          }
      }


      /** Change the number of threads */
      void setNbThreads(const int inNbThread){
          omp_set_lock( &mutex );
          nbThreads = inNbThread;
          omp_unset_lock( &mutex );
      }
  };

  class ep_intercomm
  {
    public:

    int *mpi_inter_comm;

    RANK_MAP *intercomm_rank_map;
    RANK_MAP *local_rank_map;
    RANK_MAP *remote_rank_map;


    SIZE_RANK_INFO size_rank_info[3];


    MPI_Comm *local_comm;
    int intercomm_tag;

    ep_intercomm();
    bool operator == (ep_intercomm right);
    bool operator != (ep_intercomm right);
    


  };


  class ep_communicator
  {
    public:

    SIZE_RANK_INFO size_rank_info[3]; // 0: ep_rank,     ep_size
                                      // 1: ep_rank_loc, num_ep
                                      // 2: mpi_rank,    mpi_size


    MPI_Comm *comm_list;

    Message_list *message_queue;


    int comm_label;

    ep_intercomm *intercomm;

    ep_communicator();
    bool operator == (ep_communicator right);
    bool operator != (ep_communicator right);
    
  };


  struct BUFFER
  {
    void * void_buffer[12];
  };


  class MPI_Comm
  {
    public:

    bool is_ep;
    bool is_intercomm;

    BUFFER     *my_buffer;
    OMPbarrier *ep_barrier;
    RANK_MAP   *rank_map;
    int* mpi_comm;
    EP_Comm ep_comm_ptr;
    MPI_Comm *mem_bridge;
    int* mpi_bridge;

    MPI_Comm();  
    //MPI_Comm(const MPI_Comm &comm);
    MPI_Comm(int* comm);


    bool operator == (MPI_Comm right);
    bool operator != (MPI_Comm right);
    bool is_null();

  };


  class MPI_Info
  {
    public:

      int mpi_info;

      MPI_Info(){ }
      MPI_Info(int info): mpi_info(info) {}
  };


  class MPI_Request
  {
    public:

      int mpi_request;

      int type;	//! type of the non-blocking communication. 1: Isend; 2:Irecv; 3:Imrecv; 4:Issend
      void* buf;

      int ep_src;
      int ep_tag;
      int ep_datatype;

      MPI_Comm comm;	//! EP communicator related to the communication

      MPI_Request() {}
      MPI_Request(int request): mpi_request(request) {}
      bool operator == (MPI_Request right);

  };

  
  class MPI_Aint
  {
    public:

    unsigned long mpi_aint;

    MPI_Aint() {}
    MPI_Aint(int a): mpi_aint(a) {}
  };

  class MPI_Fint
  {
    public:

    int mpi_fint;

    MPI_Fint() {}
    MPI_Fint(int f): mpi_fint(f) {}
    
  };


  static MPI_Comm *passage;

  static int TAG = 40000;

  static std::list<std::pair<std::pair<int, int>, MPI_Comm * > > tag_list;

  static std::map<std::pair<int, int>, MPI_Comm >  fc_comm_map;

  static std::map<std::pair<int, int>, MPI_Comm >  *fc_comm_map_ptr;
  #pragma omp threadprivate(fc_comm_map_ptr)
            //    <MPI_Fint,thread_num>   EP_Comm

}



#endif // EP_TYPE_HPP_INCLUDED

