#ifndef __XMLIO_CGregorianCalendar__
#define __XMLIO_CGregorianCalendar__

/// xmlioserver headers ///
#include "xmlioserver_spl.hpp"
#include "calendar.hpp"

namespace xmlioserver
{
   namespace date
   {
      /// ////////////////////// Déclarations ////////////////////// ///
      class CGregorianCalendar : public CCalendar
      {
            /// Typedef ///
            typedef CCalendar SuperClass;

         public :

            /// Constructeur ///
            CGregorianCalendar(void);                                   // Not implemented yet.
            CGregorianCalendar(const StdString & dateStr);
            CGregorianCalendar(int yr = 0, int mth = 1, int d   = 1,
                               int hr = 0, int min = 0, int sec = 0);
            CGregorianCalendar(const CGregorianCalendar & calendar);       // Not implemented yet.
            CGregorianCalendar(const CGregorianCalendar * calendar);       // Not implemented yet.

            /// Accesseurs ///
            virtual int getYearTotalLength(const CDate & date) const;
            virtual int getMonthLength(const CDate & date) const;
            virtual StdString getType(void) const;

            /// Destructeur ///
            virtual ~CGregorianCalendar(void);

      }; // class CGregorianCalendar

   } // namespace date
} // namespace xmlioserver

#endif // __XMLIO_CGregorianCalendar__
