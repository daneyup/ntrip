/* -------------------------------------------------------------------------
 * BKG NTRIP Client
 * -------------------------------------------------------------------------
 *
 * Class:      bncNetQueryV1
 *
 * Purpose:    Blocking Network Requests (NTRIP Version 1)
 *
 * Author:     L. Mervart
 *
 * Created:    27-Dec-2008
 *
 * Changes:    
 *
 * -----------------------------------------------------------------------*/

#include <iostream>
#include <iomanip>

#include "bncnetqueryv1.h"
#include "bncsettings.h"

using namespace std;

#define BNCVERSION "1.7"

// Constructor
////////////////////////////////////////////////////////////////////////////
bncNetQueryV1::bncNetQueryV1() {
  _socket  = 0;
  _timeOut = 20000;
}

// Destructor
////////////////////////////////////////////////////////////////////////////
bncNetQueryV1::~bncNetQueryV1() {
  delete _socket;
}

// 
////////////////////////////////////////////////////////////////////////////
void bncNetQueryV1::stop() {
#ifndef sparc
  if (_socket) {
    _socket->abort();
  }
#endif
  _status = finished;
}

// 
////////////////////////////////////////////////////////////////////////////
void bncNetQueryV1::waitForRequestResult(const QUrl&, QByteArray&) {
}

// 
////////////////////////////////////////////////////////////////////////////
void bncNetQueryV1::waitForReadyRead(QByteArray& outData) {
  if (_socket && _socket->state() == QAbstractSocket::ConnectedState) {
    while (true) {
      int nBytes = _socket->bytesAvailable();
      if (nBytes > 0) {
        outData = _socket->readAll();
        return;
      }
      else if (!_socket->waitForReadyRead(_timeOut)) {
        QString errStr = _socket->errorString();
        if (errStr.isEmpty()) {
          errStr = "Read timeout";
	}
        delete _socket;
        _socket = 0;
        _status = error;
        emit newMessage(_url.path().toAscii().replace(0,1,"")
                        + ": " + errStr.toAscii(), true);
        return;
      }
    }
  }
}

// Connect to Caster, send the Request
////////////////////////////////////////////////////////////////////////////
void bncNetQueryV1::startRequest(const QUrl& url, const QByteArray& gga) {

  _status = running;

  delete _socket;
  _socket = new QTcpSocket();

  // Default scheme and path
  // -----------------------
  _url = url;
  if (_url.scheme().isEmpty()) {
    _url.setScheme("http");
  }
  if (_url.path().isEmpty()) {
    _url.setPath("/");
  }

  // Connect the Socket
  // ------------------
  bncSettings settings;
  QString proxyHost = settings.value("proxyHost").toString();
  int     proxyPort = settings.value("proxyPort").toInt();
 
  if ( proxyHost.isEmpty() ) {
    _socket->connectToHost(_url.host(), _url.port());
  }
  else {
    _socket->connectToHost(proxyHost, proxyPort);
  }
  if (!_socket->waitForConnected(_timeOut)) {
    delete _socket; 
    _socket = 0;
    _status = error;
    return;
  }

  // Send Request
  // ------------
  QString uName = QUrl::fromPercentEncoding(_url.userName().toAscii());
  QString passW = QUrl::fromPercentEncoding(_url.password().toAscii());
  QByteArray userAndPwd;

  if(!uName.isEmpty() || !passW.isEmpty()) {
    userAndPwd = "Authorization: Basic " + (uName.toAscii() + ":" +
    passW.toAscii()).toBase64() + "\r\n";
  }

  QByteArray reqStr;
  if ( proxyHost.isEmpty() ) {
    if (_url.path().indexOf("/") != 0) _url.setPath("/");
    reqStr = "GET " + _url.path().toAscii() + " HTTP/1.0\r\n"
             + "User-Agent: NTRIP BNC/" BNCVERSION "\r\n"
             + userAndPwd + "\r\n";
  } else {
    reqStr = "GET " + _url.toEncoded() + " HTTP/1.0\r\n"
             + "User-Agent: NTRIP BNC/" BNCVERSION "\r\n"
             + "Host: " + _url.host().toAscii() + "\r\n"
             + userAndPwd + "\r\n";
  }

  // NMEA string to handle VRS stream
  // --------------------------------
  if (!gga.isEmpty()) {
    reqStr += gga + "\r\n";
  }

  _socket->write(reqStr, reqStr.length());

  if (!_socket->waitForBytesWritten(_timeOut)) {
    delete _socket;
    _socket = 0;
    _status = error;
    emit newMessage(_url.path().toAscii().replace(0,1,"")
                    + ": Write timeout", true);
    return;
  }

  // Read Caster Response
  // --------------------
  bool proxyResponse = false;
  QStringList response;
  while (true) {
    if (_socket->canReadLine()) {
      QString line = _socket->readLine();

      if (line.indexOf("ICY 200 OK") == -1 && 
          line.indexOf("HTTP")       != -1 && 
          line.indexOf("200 OK")     != -1 ) {
        proxyResponse = true;
      }

      if (!proxyResponse && !line.trimmed().isEmpty()) {
        response.push_back(line);
      }

      if (line.trimmed().isEmpty()) {
        if (proxyResponse) {
          proxyResponse = false;
	}
	else {
          break;
	}
      }

      if (line.indexOf("Unauthorized") != -1) {
        break;
      }

      if (!proxyResponse                    &&
          line.indexOf("200 OK")      != -1 &&
          line.indexOf("SOURCETABLE") == -1) {
        response.clear();
        if (_socket->canReadLine()) {
          _socket->readLine();
	}
	break;
      }
    }
    else if (!_socket->waitForReadyRead(_timeOut)) {
      delete _socket;
      _socket = 0;
      _status = error;
      emit newMessage(_url.path().toAscii().replace(0,1,"") 
                      + ": Response timeout", true);
      return;
    }
  }
  if (response.size() > 0) {
    delete _socket;
    _socket = 0;
    _status = error;
    emit newMessage(_url.path().toAscii().replace(0,1,"") 
                    + ": Wrong caster response\n" 
                    + response.join("").toAscii(), true);
  }
}

// Send NMEA String
////////////////////////////////////////////////////////////////////////////
void bncNetQueryV1::sendNMEA(const QByteArray& /* gga */) {
  emit newMessage(_url.path().toAscii().replace(0,1,"") 
                  + ": sending NMEA not impelmented for NTRIP v1", true);
}
