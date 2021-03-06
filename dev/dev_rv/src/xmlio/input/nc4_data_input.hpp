#ifndef __XMLIO_NC4_DATA_INPUT__
#define __XMLIO_NC4_DATA_INPUT__

/// xmlioserver headers ///
#include "xmlioserver_spl.hpp"
#include "inetcdf4.hpp"

namespace xmlioserver
{
   namespace io
   {
      /// ////////////////////// Déclarations ////////////////////// ///

      class CNc4DataInput
      {
         public :

            /// Constructeurs ///
            CNc4DataInput(void);
            CNc4DataInput(const CNc4DataInput & datainput);       // Not implemented.
            CNc4DataInput(const CNc4DataInput * const datainput); // Not implemented.

            /// Destructeur ///
            virtual ~CNc4DataInput(void);

      }; // class CNc4DataInput

   } // namespace io
} // namespace xmlioserver

#endif //__XMLIO_NC4_DATA_INPUT__
