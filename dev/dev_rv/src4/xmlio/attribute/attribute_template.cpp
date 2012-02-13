/* ************************************************************************** *
 *      Copyright © IPSL/LSCE, XMLIOServer, Avril 2010 - Octobre 2011         *
 * ************************************************************************** */

/**
 * \file    attribute_template.cpp
 * \brief   Gestion des attributs typés d'objet (implémentation).
 * \author  Hervé Ozdoba
 * \version 0.4
 * \date    1er Juin 2011
 */

#ifndef __XIOS_NO_EXTERN

// C++ standard headers
#include <string>
#include <iostream>
#include <cfloat>

// boost headers
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>
#include <boost/lexical_cast.hpp>

#endif // __XIOS_NO_EXTERN

// XMLIOServer headers
#include "attribute_template.hpp"
#include "attribute_template_impl.hpp"
#include "attribute_impl.hpp"

// /////////////////////////////// Définitions ////////////////////////////// //

namespace xmlioserver {
namespace tree {

   //--------------------------------------------------------------------------
   //-------- Spécialisations des templates pour la fonction [toString] -------
   //--------------------------------------------------------------------------
   
   template <>
      std::string CAttributeTemplate<bool>::toString(void) const
   {
      std::ostringstream oss;
      if (!this->isEmpty() && this->hasId())
      {
         if (this->getValue())
            oss << this->getName() << "=\".TRUE.\"";
         else
            oss << this->getName() << "=\".FALSE.\"";
      }
      return (oss.str());
   }

   //--------------------------------------------------------------------------
   //------ Spécialisations des templates pour la fonction [fromString] -------
   //--------------------------------------------------------------------------

   template <> // Chaîne de caractères.
      void CAttributeTemplate<std::string>::fromString(const std::string & _str)
   { 
      this->setValue(_str); 
   }

   template <> // Entier
      void CAttributeTemplate<int>::fromString(const std::string & _str)
   {
      try
      {
         this->setValue(boost::lexical_cast<int>(_str));
      }
      catch(boost::bad_lexical_cast &)
      {
         XIOS_ERROR("void CAttributeTemplate<int>::fromString(const StdString & _str)",
                     << "[ str = " << _str << " ] Bad cast !");
      }
   }

   template <> // Double
      void CAttributeTemplate<double>::fromString(const std::string & _str)
   {
      if (_str.find("max") != std::string::npos)
      {
         this->setValue(DBL_MAX);
         return;
      }
      if (_str.find("min") != std::string::npos)
      {
         this->setValue(DBL_MIN);
         return;
      }
      
      try
      {
         this->setValue(boost::lexical_cast<double>(_str));
      }
      catch(boost::bad_lexical_cast &)
      {
         XIOS_ERROR("void CAttributeTemplate<double>::fromString(const StdString & str)",
                    << "[ str = " << _str << " ] Bad cast !");
      }
   }

   template <> // Booléen
      void CAttributeTemplate<bool>::fromString(const std::string & _str)
   {
      if (_str.find(".TRUE.") != std::string::npos)
         this->setValue(true);
      else
         this->setValue(false);
   }

   //--------------------------------------------------------------------------

   template<> // Tableau
      void CAttributeTemplate<boost::shared_ptr<boost::multi_array<double, 1> > >
	 ::fromString(const std::string & _str)
   { 
      boost::shared_ptr<boost::multi_array<double, 1> > array_sptr 
	  (new boost::multi_array<double, 1>(boost::extents[1], boost::fortran_storage_order()));
      
      boost::multi_array<double, 1> & array = *array_sptr;
      this->setValue(array_sptr);

      std::istringstream iss(_str) ;
      char c = '\0'; int size = 0;
      double d = 0.,valsup = 0., valinf = 0.;
      std::vector<double> vect;

      iss >> d; vect.push_back(d);
      if (!iss.eof ())
      {
         iss >> c;
         switch (c)
         {
            case ',' : // Le tableau est généré valeur par valeur.
               iss.unget();
               while(!iss.eof ())
               { // On récupère chacune des valeurs une par une jusqu'à ce que le buffer soit vide.
                  iss >> c >> d;
                  if (c != ',')
                     XIOS_ERROR("CAttributeTemplate<ARRAY(...)>::fromString(const StdString & str)",
                                << "[ str = " << _str << " ] bad definition of array !");
                  vect.push_back(d);
               }
               size = vect.size();
               break;
            case '(' : // Le tableau est généré automatiquement.
               if (!iss.eof ())
               { // on récupère la borne supérieure
                  valinf = d;
                  iss >> size >> c >> d;
                  if ((c != ')') || (size <= 0))
                     XIOS_ERROR("CAttributeTemplate<ARRAY(...)>::fromString(const StdString & str)",
                                << "[ str = " << _str << " ] bad definition of array !");
                  valsup = d;
               }
               d = (valsup - valinf) / (double)(size - 1);
               for (int j = 1; j <= size; j++)
                  vect.push_back(valinf + j * d);
               break;
            default :
               XIOS_ERROR("CAttributeTemplate<ARRAY(...)>::fromString(const StdString & str)",
                           << "[ str = " << _str << " ] bad definition of array !");
         }
      }

      array.resize(boost::extents[size]);
      for (int i = 0; i < size; i++)
         array[i] = vect[i];

   }

   //--------------------------------------------------------------------------
   //-------- Spécialisations des templates pour la fonction [toBinary] -------
   //--------------------------------------------------------------------------

   template <> // Chaîne de caractères.
      void CAttributeTemplate<std::string>::toBinary (std::ostream & _os) const
   {
      std::string str = this->getValue();
      std::size_t size = str.size();
      _os.write (reinterpret_cast<const char*>(&size) , sizeof(std::size_t));
      _os.write (str.data(), size * sizeof(char));
   }

   template <> // Entier
      void CAttributeTemplate<int>::toBinary(std::ostream & _os) const
   {
      int value = this->getValue();
      _os.write (reinterpret_cast<const char*>(&value) , sizeof(int));
   }

   template <> // Booléen
      void CAttributeTemplate<bool>::toBinary(std::ostream & _os) const
   {
      bool value = this->getValue();
      _os.write (reinterpret_cast<const char*>(&value) , sizeof(bool));
   }

   template <> // Double
      void CAttributeTemplate<double>::toBinary(std::ostream & _os) const
   {
      double value = this->getValue();
      _os.write (reinterpret_cast<const char*>(&value) , sizeof(double));
   }

   //--------------------------------------------------------------------------
   //------ Spécialisations des templates pour la fonction [fromBinary] -------
   //--------------------------------------------------------------------------

   template <> // Chaîne de caractères.
      void CAttributeTemplate<std::string>::fromBinary(std::istream & _is)
   {
      std::size_t size = 0;
      _is.read (reinterpret_cast<char*>(&size), sizeof(std::size_t));
      std::string value(size, ' ');
      _is.read (const_cast<char *>(value.data()), size * sizeof(char));
      this->setValue(value);
   }

   template <> // Entier
      void CAttributeTemplate<int>::fromBinary(std::istream & _is)
   {
      int value = 0;
      _is.read (reinterpret_cast<char*>(&value), sizeof(int));
      this->setValue(value);
   }

   template <> // Booléen
      void CAttributeTemplate<bool>::fromBinary(std::istream & _is)
   {
      bool value = false;
      _is.read (reinterpret_cast<char*>(&value), sizeof(bool));
      this->setValue(value);
   }

   template <> // Double
      void CAttributeTemplate<double>::fromBinary(std::istream & _is)
   {
      double value = 0.;
      _is.read (reinterpret_cast<char*>(&value), sizeof(double));
      this->setValue(value);
   }
  
} // namespace tree
} // namespace xmlioserver