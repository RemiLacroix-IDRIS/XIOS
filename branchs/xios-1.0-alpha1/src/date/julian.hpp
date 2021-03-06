#ifndef __XMLIO_CJulianCalendar__
#define __XMLIO_CJulianCalendar__

/// xmlioserver headers ///
#include "xmlioserver_spl.hpp"
#include "calendar.hpp"

namespace xmlioserver
{
   namespace date
   {
      /// ////////////////////// Déclarations ////////////////////// ///
      class CJulianCalendar : public CCalendar
      {
            /// Typedef ///
            typedef CCalendar SuperClass;

         public :

            /// Constructeur ///
            CJulianCalendar(void);                                   // Not implemented yet.
            CJulianCalendar(const StdString & dateStr);
            CJulianCalendar(int yr = 0, int mth = 1, int d   = 1,
                            int hr = 0, int min = 0, int sec = 0);
            CJulianCalendar(const CJulianCalendar & calendar);       // Not implemented yet.
            CJulianCalendar(const CJulianCalendar * calendar);       // Not implemented yet.

            /// Accesseurs ///
            virtual int getYearTotalLength(const CDate & date) const;
            virtual int getMonthLength(const CDate & date) const;
            virtual StdString getType(void) const;

            /// Destructeur ///
            virtual ~CJulianCalendar(void);

      }; // class CJulianCalendar

   } // namespace date
} // namespace xmlioserver

#endif // __XMLIO_CJulianCalendar__
