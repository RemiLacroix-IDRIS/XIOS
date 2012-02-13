/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */
 
#ifndef __LINEAR_BUFFER_HPP__
#define __LINEAR_BUFFER_HPP__ 

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <vector>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "xmlioserver_spl.hpp"
#include "buffer.hpp"

// ////////////////////////////// Déclarations ///////////////////////////// //

#define _NULL_ARG xmlioserver::comm::CLinearBuffer::NULL_ARG

namespace xmlioserver
{
   namespace comm
   {
      class CLinearBuffer : public CBuffer
      {
         public : // Définition de type

            typedef CBuffer SuperClass;

         public :

            static long int * NULL_ARG;
            friend class CMPIManager;

         public :// Construteurs


            explicit CLinearBuffer(std::size_t size = 1E5);
            CLinearBuffer(char * data, std::size_t _size);
            CLinearBuffer(const CLinearBuffer & _lbuffer_ref);
            CLinearBuffer(const CLinearBuffer * const _lbuffer_ptr);

         public :// Accesseurs

            std::size_t getUsedSize(void) const;
            std::size_t getUnusedSize(void) const;

            std::size_t getNumberOfStoredData(void) const;
            std::vector<std::size_t> getPositionsOfStoredData(void) const;
            std::vector<std::size_t> getSizesOfStoredData(void) const;
            std::vector<std::pair<CBufferDataType, bool> > getTypesOfStoredData(void) const;

         //--------------------------------------------------------------
	    
            char     getChar  (std::size_t _position) const;
            bool     getBool  (std::size_t _position) const;
            float    getFloat (std::size_t _position) const;
            double   getDouble(std::size_t _position) const;
            long int getInt   (std::size_t _position) const;

            std::string getString(std::size_t _position) const;

            boost::shared_ptr<boost::multi_array<char, 1> >
	       getCharArray  (std::size_t _position) const;
            boost::shared_ptr<boost::multi_array<bool, 1> >
	        getBoolArray  (std::size_t _position) const;
            boost::shared_ptr<boost::multi_array<float, 1> >
	        getFloatArray (std::size_t _position) const;
            boost::shared_ptr<boost::multi_array<double, 1> >
	        getDoubleArray(std::size_t _position) const;
            boost::shared_ptr<boost::multi_array<long int, 1> >
	        getIntArray   (std::size_t _position) const;

         //--------------------------------------------------------------

            void getRequestInfos(std::size_t   _position,
				 long int    & _managerId,
				 long int    & _methodId,
				 long int    & _nbargs);

         //--------------------------------------------------------------
         public : // Mutateurs

            void appendChar  (char     _value) ;
            void appendBool  (bool     _value) ;
            void appendFloat (float    _value) ;
            void appendDouble(double   _value) ;
            void appendInt   (long int _value) ;

            void appendString(const std::string & _value) ;

            void appendCharArray  
	       (boost::shared_ptr<boost::multi_array<char, 1> > _value) ;
            void appendBoolArray  
	       (boost::shared_ptr<boost::multi_array<bool, 1> > _value) ;
            void appendFloatArray 
	       (boost::shared_ptr<boost::multi_array<float, 1> > _value) ;
            void appendDoubleArray
	       (boost::shared_ptr<boost::multi_array<double, 1> > _value) ;
            void appendIntArray  
             (boost::shared_ptr<boost::multi_array<long int, 1> > _value) ;

         //--------------------------------------------------------------

            void appendRequestInfos(const long int & _managerId,
                                    const long int & _methodId,
                                    const long int & _nbargs);

         //--------------------------------------------------------------
	    
            virtual void clear(void);

         public :// Test ///
            template <typename A1, typename A2, typename A3, typename A4>
               inline std::size_t getRequestedSize(A1 * _arg1 = NULL_ARG,
                                                   A2 * _arg2 = NULL_ARG,
                                                   A3 * _arg3 = NULL_ARG,
                                                   A4 * _arg4 = NULL_ARG) const;

         public : // Sortie fichier ascii

            virtual void printToTextFile (const std::string & _filename);
            virtual void printToTextStream (std::ostream & _ostr);

         public : // Destructeur

            virtual ~CLinearBuffer(void);

         protected : // Accesseurs protégés

            template <class T>
               inline void getLData
		 (T & _data, std::size_t _position) const;
		 
            template <class T>
               inline void getLDataArray
		 (boost::shared_ptr<boost::multi_array<T, 1> > data, std::size_t _position) const;

         protected : // Mutateurs protégés

            template <class T>
               inline void appendLData(const T & _data);
            template <class T>
               inline void appendLDataArray
		 (boost::shared_ptr<boost::multi_array<T, 1> > _data);

         private : // Traitement

            void computeBufferData(void);

         private : // Propriétés privées

            std::vector<std::size_t> bdata;
            std::size_t p_write;

      }; // class CLinearBuffer
   } // namespace comm
} // namespace xmlioserver
#endif //__LINEAR_BUFFER_HPP__