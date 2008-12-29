// Part of BNC, a utility for retrieving decoding and
// converting GNSS data streams from NTRIP broadcasters.
//
// Copyright (C) 2007
// German Federal Agency for Cartography and Geodesy (BKG)
// http://www.bkg.bund.de
// Czech Technical University Prague, Department of Geodesy
// http://www.fsv.cvut.cz
//
// Email: euref-ip@bkg.bund.de
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

/* -------------------------------------------------------------------------
 * BKG NTRIP Client
 * -------------------------------------------------------------------------
 *
 * Class:      bncutils
 *
 * Purpose:    Auxiliary Functions
 *
 * Author:     L. Mervart
 *
 * Created:    30-Aug-2006
 *
 * Changes:    
 *
 * -----------------------------------------------------------------------*/

#include <iostream>
#include <ctime>
#include <math.h>

#include <QRegExp>
#include <QStringList>
#include <QDateTime>

#include "bncutils.h"
#include "bncapp.h"

using namespace std;

// 
////////////////////////////////////////////////////////////////////////////
void expandEnvVar(QString& str) {

  QRegExp rx("(\\$\\{.+\\})");

  if (rx.indexIn(str) != -1) {
    QStringListIterator it(rx.capturedTexts());
    if (it.hasNext()) {
      QString rxStr  = it.next();
      QString envVar = rxStr.mid(2,rxStr.length()-3);
      str.replace(rxStr, qgetenv(envVar.toAscii()));
    }
  }

}

// 
////////////////////////////////////////////////////////////////////////////
QDateTime dateAndTimeFromGPSweek(int GPSWeek, double GPSWeeks) {

  static const QDate zeroEpoch(1980, 1, 6);
 
  QDate date(zeroEpoch);
  QTime time(0,0,0,0);

  int weekDays = int(GPSWeeks) / 86400;
  date = date.addDays( GPSWeek * 7 + weekDays );
  time = time.addMSecs( int( (GPSWeeks - 86400 * weekDays) * 1e3 ) );

  return QDateTime(date,time);
}

// 
////////////////////////////////////////////////////////////////////////////
void currentGPSWeeks(int& week, double& sec) {

  QDateTime currDateTime;
  int leapsecond = 0;

  if ( ((bncApp*) qApp)->_currentDateAndTimeGPS ) {
    currDateTime = *(((bncApp*) qApp)->_currentDateAndTimeGPS);
  }
  else {
    currDateTime = QDateTime::currentDateTime().toUTC();
    if (currDateTime.date().year() >= 2009) {
      leapsecond = 15;
    }
    else {
      leapsecond = 14;
    }
  }

  QDate     currDate = currDateTime.date();
  QTime     currTime = currDateTime.time();

  week = int( (double(currDate.toJulianDay()) - 2444244.5) / 7 );

  sec = (currDate.dayOfWeek() % 7) * 24.0 * 3600.0 + 
        currTime.hour()                   * 3600.0 + 
        currTime.minute()                 *   60.0 + 
        currTime.second()                          +
        currTime.msec()                   / 1000.0 +
        leapsecond;
}

// 
////////////////////////////////////////////////////////////////////////////
QDateTime currentDateAndTimeGPS() {
  int    GPSWeek;
  double GPSWeeks;
  currentGPSWeeks(GPSWeek, GPSWeeks);
  return dateAndTimeFromGPSweek(GPSWeek, GPSWeeks);
}

// 
////////////////////////////////////////////////////////////////////////////
QByteArray ggaString(const QByteArray& latitude, const QByteArray& longitude) {

  double lat = strtod(latitude,NULL);
  double lon = strtod(longitude,NULL);

  const char* flagN="N";
  const char* flagE="E";
  if (lon >180.) {lon=(lon-360.)*(-1.); flagE="W";}
  if ((lon < 0.) && (lon >= -180.))  {lon=lon*(-1.); flagE="W";}
  if (lon < -180.)  {lon=(lon+360.); flagE="E";}
  if (lat < 0.)  {lat=lat*(-1.); flagN="S";}
  QTime ttime(QDateTime::currentDateTime().toUTC().time());
  int lat_deg = (int)lat;  
  double lat_min=(lat-lat_deg)*60.;
  int lon_deg = (int)lon;  
  double lon_min=(lon-lon_deg)*60.;
  int hh = 0 , mm = 0;
  double ss = 0.0;
  hh=ttime.hour();
  mm=ttime.minute();
  ss=(double)ttime.second()+0.001*ttime.msec();
  QString gga;
  gga += "$GPGGA,";
  gga += QString("%1%2%3,").arg((int)hh, 2, 10, QLatin1Char('0')).arg((int)mm, 2, 10, QLatin1Char('0')).arg((int)ss, 2, 10, QLatin1Char('0'));
  gga += QString("%1%2,").arg((int)lat_deg,2, 10, QLatin1Char('0')).arg(lat_min, 7, 'f', 4, QLatin1Char('0'));
  gga += flagN;
  gga += QString(",%1%2,").arg((int)lon_deg,3, 10, QLatin1Char('0')).arg(lon_min, 7, 'f', 4, QLatin1Char('0'));
  gga += flagE + QString(",1,05,1.00,+00100,M,10.000,M,,");
  int xori;
  char XOR = 0;
  char *Buff =gga.toAscii().data();
  int iLen = strlen(Buff);
  for (xori = 0; xori < iLen; xori++) {
    XOR ^= (char)Buff[xori];
  }
  gga += QString("*%1").arg(XOR, 2, 16, QLatin1Char('0'));

  return gga.toAscii();
}
