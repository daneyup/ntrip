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

#ifndef BNCGETTHREAD_H
#define BNCGETTHREAD_H

#include <QThread>
#include <QtNetwork>
#include <QDateTime>
#include <QFile>

#include "RTCM/GPSDecoder.h"
#include "RTCM3/rtcm3torinex.h"
#include "bncconst.h"

class bncRinex;
class QextSerialPort;
class bncNetQuery;

class bncGetThread : public QThread {
 Q_OBJECT

 public:
   bncGetThread(const QByteArray&  rawInpFileName, const QByteArray& format);
   bncGetThread(const QUrl& mountPoint, 
                const QByteArray& format,
                const QByteArray& latitude,
                const QByteArray& longitude,
                const QByteArray& nmea, 
                const QByteArray& ntripVersion, int iMount);

 protected:
   ~bncGetThread();

 public:
   void terminate();

   QByteArray staID() const {return _staID;}
   QUrl       mountPoint() const {return _mountPoint;}
   QByteArray latitude() const {return _latitude;}
   QByteArray longitude() const {return _longitude;}
   QByteArray ntripVersion() const {return _ntripVersion;}

 signals:
   void newBytes(QByteArray staID, double nbyte);
   void newObs(QByteArray staID, bool firstObs, p_obs obs);
   void newAntCrd(QByteArray staID, double xx, double yy, double zz, QByteArray antType);
   void newMessage(QByteArray msg, bool showOnScreen);
   void getThreadFinished(QByteArray staID);

 protected:
   virtual void run();

 private:

   class latencyChecker : public QObject {
   Q_OBJECT
   public:
     latencyChecker(QByteArray staID);
     ~latencyChecker();
     void check(GPSDecoder* decoder);
   signals:
     void newMessage(QByteArray msg, bool showOnScreen);
   private:
     void callScript(const char* comment);
     int        _inspSegm;
     int        _adviseFail;
     int        _adviseReco;
     int        _perfIntr;
     int        _numSucc;
     int        _secSucc;
     int        _secFail;
     int        _initPause;
     int        _currPause;
     int        _oldSecGPS;
     int        _newSecGPS;
     int        _numGaps;
     int        _diffSecGPS;
     int        _numLat;
     bool       _makePause;
     bool       _wrongEpoch;
     bool       _decode;
     bool       _begCorrupt;
     bool       _endCorrupt;
     bool       _followSec;
     double     _maxDt;
     double     _sumLat;
     double     _sumLatQ;
     double     _meanDiff;
     double     _minLat;
     double     _maxLat;
     double     _curLat;
     QByteArray _staID;
     QString    _adviseScript;
     QString    _checkMountPoint;
     QString    _begDateCor;
     QString    _begTimeCor;
     QString    _begDateOut;
     QString    _begTimeOut;
     QString    _endDateCor;
     QString    _endTimeCor;
     QString    _endDateOut;
     QString    _endTimeOut;
     QDateTime  _decodeTime;
     QDateTime  _decodeSucc;
     QDateTime  _decodeFailure;
     QDateTime  _decodeStart;
     QDateTime  _decodeStop;
     QDateTime  _decodePause;
   };

   void  initialize();
   t_irc tryReconnect();
   void  scanRTCM();

   GPSDecoder*     _decoder;
   bncNetQuery*    _query;
   QUrl            _mountPoint;
   QByteArray      _staID;
   QByteArray      _staID_orig;
   QByteArray      _format;
   QByteArray      _latitude;
   QByteArray      _longitude;
   QByteArray      _nmea;
   QByteArray      _ntripVersion;
   int             _nextSleep;
   int             _iMount;
   int             _samplingRate;
   bncRinex*       _rnx;
   QFile*          _rawOutFile;
   QFile*          _rawInpFile;
   QextSerialPort* _serialPort;
   bool            _isToBeDeleted;
   latencyChecker* _latencyChecker;
};

#endif
