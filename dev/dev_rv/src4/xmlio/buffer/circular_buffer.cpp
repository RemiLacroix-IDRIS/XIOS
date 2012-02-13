/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
 
#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <fstream>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "circular_buffer.hpp" 
#include "buffer_impl.hpp" 

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace comm {
  
   // ------------------------------ Constructeurs -----------------------------
   
   CCircularBuffer::CCircularBuffer(std::size_t _size)
      : SuperClass(_size)
      , p_write(0), p_read(0), p_unused(0)
      , p_wait(0), locked_data(0)
      , nbrequest(0)
   { /* Ne rien faire de plus */ }

   CCircularBuffer::CCircularBuffer(const CCircularBuffer & _cbuffer_ref)
      : SuperClass(_cbuffer_ref)
      , p_write(_cbuffer_ref.p_write), p_read(_cbuffer_ref.p_read), p_unused(_cbuffer_ref.p_unused)
      , p_wait(_cbuffer_ref.p_wait), locked_data(_cbuffer_ref.locked_data)
      , nbrequest(_cbuffer_ref.nbrequest)
   { /* Ne rien faire de plus */ }
   
   CCircularBuffer::CCircularBuffer(const CCircularBuffer * const _cbuffer_ptr)
      : SuperClass(_cbuffer_ptr)
      , p_write(_cbuffer_ptr->p_write), p_read(_cbuffer_ptr->p_read), p_unused(_cbuffer_ptr->p_unused)
      , p_wait(_cbuffer_ptr->p_wait), locked_data(_cbuffer_ptr->locked_data)
      , nbrequest(_cbuffer_ptr->nbrequest)
   { /* Ne rien faire de plus */ }
   
   // ------------------------------- Destructeur ------------------------------
   
   CCircularBuffer::~CCircularBuffer(void)
   { /* Ne rien faire de plus */ }
   
   // ------------------------------ Remise à zéro -----------------------------
   
   void CCircularBuffer::clear(void)
   {
      this-> p_write     = 0;
      this-> p_read      = 0;
      this-> p_unused    = 0;
      this-> p_wait      = 0;
      this-> locked_data = 0;
      this-> nbrequest   = 0;
   }

   // ------------------------------- Accesseurs -------------------------------

   std::size_t CCircularBuffer::getNextRequestSize(void) const
   {
      if (!this->hasRequest())
         XIOS_ERROR("CCircularBuffer::getNextRequestSize()",
                     << " invalid call !");
      long int nbargs  = 0;
      std::size_t startpos = this-> p_read;
      std::size_t currsize = 2 * SuperClass::getRequestedSize(nbargs);
      nbargs    = SuperClass::getInt(startpos + currsize);
      currsize += SuperClass::getRequestedSize(nbargs);
      
      for (long int i = 0; i < nbargs; i++)
      {
         CBufferData bufdata;
         SuperClass::getBufferData(bufdata, startpos + currsize);
         currsize += (bufdata.size + DATA_HEADER_SIZE);
      }
      return (currsize);
   }
        
   CLinearBuffer CCircularBuffer::getNextRequest(void)
   {
      std::size_t startpos = this-> p_read;
      std::size_t currsize = this->getNextRequestSize();
     
      this->movePRead(currsize);
      this->nbrequest--;
      //std::cout <<  this->nbrequest << std::endl;
     
      return (CLinearBuffer(SuperClass::getData(startpos), currsize));
   }
   
   // ------------------------------- Mutateurs --------------------------------
   
   void CCircularBuffer::appendRequest(const CLinearBuffer & _brequest)
   {
      std::size_t usedsize   = _brequest.getUsedSize();
      if (!this->isAvailable(usedsize))
         XIOS_ERROR("CCircularBuffer::appendRequest(brequest)",
               << " invalid call !");
      std::size_t data_begin = this->prepareNextDataPosition(usedsize);
      SuperClass::setData(_brequest.getData(), usedsize, data_begin);
      this->updateNbRequests(data_begin, data_begin + usedsize);
   }

   char * CCircularBuffer::prepareNextData(std::size_t data_size)
   {
      return (SuperClass::getData(this->prepareNextDataPosition(data_size)));
   }
   
   std::size_t CCircularBuffer::prepareNextDataPosition(std::size_t data_size)
   {
      this->p_wait = this-> p_write;    
      this->locked_data = data_size;   
      if (!this->isAvailable(data_size))
         XIOS_ERROR("CCircularBuffer::prepareNextDataPosition(data_size)",
               << " invalid call !");
      if ((this->p_write + this->locked_data) > SuperClass::getSize())
           this->p_wait = 0;

      this->movePWrite(this->locked_data);
      return (this->p_wait);
   }
   
   // ------------------ Déplacements de pointeurs internes -------------------
   
   void CCircularBuffer::movePRead(std::size_t data_size)
   {
      this-> p_read += data_size;
      if ((this-> p_read == this-> p_unused) &&
          (this-> p_read == this-> p_write))
      {
         this->clear();
         return;
      }
      
      if (this-> p_read == this-> p_unused)
      {
         this-> p_unused = this-> p_write;
         this-> p_read   = 0;
         return;
      }
      if (this-> p_read == this-> p_write)
      {
         this->clear();
      }
   }
   
   void CCircularBuffer::movePWrite(std::size_t data_size)
   {
      if ((this-> p_write + data_size) > SuperClass::getSize())
      {
         this-> p_unused = this-> p_write;
         this-> p_write  = data_size;
         if (this-> p_read < (data_size))
            XIOS_ERROR("CCircularBuffer::movePWrite(data_size)",
                       << " invalid position 1 !");
      }
      else
      {
         if ((this-> p_read > this-> p_write) &&
             (this-> p_read < (this-> p_write + data_size)))
            XIOS_ERROR("CCircularBuffer::movePWrite(data_size)",
                        << " invalid position 2 !");
         this-> p_write += data_size;
         if (this->p_read < this->p_write)
            this-> p_unused = this-> p_write;
      }
   }

   // --------------------------- Tests sur le buffer --------------------------
   
   bool CCircularBuffer::hasRequest(void) const
   { 
      return (this->nbrequest != 0); 
   }
   
   bool CCircularBuffer::isAvailable(std::size_t data_size) const
   {
      if (this->p_write == this->p_unused)
         return (((SuperClass::getSize() - this->p_write) >= data_size) ||
                (this->p_read >= data_size));
      else
         return ((this->p_read - this->p_write) >= data_size);
   }

   // ------------------------- Ecriture fichier -------------------------------

   void CCircularBuffer::printToTextFile (const std::string & _filename)
   {
      std::ofstream ofs(_filename.c_str());
      this->printToTextStream(ofs);
      ofs.close();
   }
   
   void CCircularBuffer::printToTextStream (std::ostream & _ostr)
   {
      std::size_t _p_write     = p_write,
                  _p_read      = p_read,
                  _p_unused    = p_unused,
		  _p_wait      = p_wait,
		  _locked_data = locked_data,
                  _nbrequest   = nbrequest;

      while (this->hasRequest())
      {
         this->getNextRequest().printToTextStream(_ostr);
         _ostr << std::endl;
      }

      p_write     = _p_write;
      p_read      = _p_read;
      p_unused    = _p_unused;
      p_wait      = _p_wait;
      locked_data = _locked_data;
      nbrequest   = _nbrequest;
   }

   // ------------------------------- Accesseurs -------------------------------
   
   std::size_t CCircularBuffer::getNumberOfRequest(void) const
   { 
      return (this->nbrequest); 
   }

   std::size_t CCircularBuffer::getReadPosition(void) const
   { 
      return (this->p_read); 
   }

   std::size_t CCircularBuffer::getWritePosition(void) const
   { 
      return (this->p_write); 
   }

   std::size_t CCircularBuffer::getUnusedPosition(void) const
   { 
      return (this->p_unused); 
   }
   
   std::size_t CCircularBuffer::getWaitPosition(void) const
   { 
      return (this->p_wait); 
   }


   // ------------------------- Mise à jour des données -----------------------
   
   void CCircularBuffer::updateNbRequests(void)
   {
      this->updateNbRequests(this->p_wait, this->p_wait + this->locked_data);
   }

   void CCircularBuffer::updateNbRequests(std::size_t _data_begin, std::size_t _data_end)
   {
      this-> p_wait        = 0;
      this-> locked_data   = 0;
      std::size_t position = _data_begin;        
      while (position != _data_end)
      {
         this->nbrequest++;
         position = SuperClass::getNextDataPosition(position); // manager id           
         position = SuperClass::getNextDataPosition(position); // method id           
         SuperClass::updateBufferData(position);
         long int nbarg = SuperClass::getInt(position);
         position = SuperClass::getNextDataPosition(position);
         for (long int i = 0; i < nbarg; i++)
            position = SuperClass::getNextDataPosition(position);
            
         if (position > _data_end)
           XIOS_ERROR("CCircularBuffer::updateNbRequests(std::size_t data_begin, std::size_t data_end)",
                      << "[ position courante" << position
                      << ", fin de traitement" << _data_end << " ] "
                      << "Impossible de mettre à jour la liste des requêtes !");
      }
   }
      
} // namespace comm
} // namespace xmlioserver