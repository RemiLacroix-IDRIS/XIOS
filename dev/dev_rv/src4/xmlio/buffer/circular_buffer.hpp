/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
 
#ifndef __CIRCULAR_BUFFER_HPP__
#define __CIRCULAR_BUFFER_HPP__ 

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <vector>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"
#include "buffer.hpp"
#include "linear_buffer.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

namespace xmlioserver
{
   namespace comm
   {
      class CCircularBuffer : public CBuffer
      {

         public : // Définition de type

            typedef CBuffer SuperClass;

         public : // Construteurs

            explicit CCircularBuffer(std::size_t _size = 1E6);
            CCircularBuffer(const CCircularBuffer & _cbuffer_ref);
            CCircularBuffer(const CCircularBuffer * const _cbuffer_ptr);

         public : // Mutateurs

            virtual void clear(void);

            char * prepareNextData(std::size_t _data_size);
            std::size_t prepareNextDataPosition(std::size_t _data_size);

            void appendRequest(const CLinearBuffer & _brequest);

	    void updateNbRequests(void);
            void updateNbRequests(std::size_t _data_begin, std::size_t _data_end);

         public : // Tests

            bool hasRequest(void) const ;
            bool isAvailable(std::size_t _data_size) const;

         public : // Accesseurs

            CLinearBuffer getNextRequest  (void);
            std::size_t getNumberOfRequest(void) const;

            std::size_t getReadPosition  (void) const;
            std::size_t getWritePosition (void) const;
            std::size_t getUnusedPosition(void) const;
	    std::size_t getWaitPosition  (void) const;

         public : // Sortie fichier ascii

            virtual void printToTextFile   (const std::string & _filename);
            virtual void printToTextStream (std::ostream & _ostr);

         public : // Destructeur

            virtual ~CCircularBuffer(void);

         private : // Mutateurs privés

            void movePWrite(std::size_t _data_size);
            void movePRead (std::size_t _data_size);

         private : // Accesseurs privés

            std::size_t getNextRequestSize(void) const;

         private : // Propriétés privées

            std::size_t p_write, p_read, p_unused, p_wait, locked_data, nbrequest;

      }; // class CCircularBuffer
   } // namespace comm
} // namespace xmlioserver
#endif //__CIRCULAR_BUFFER_HPP__