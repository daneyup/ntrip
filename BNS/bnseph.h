#ifndef BNSEPH_H
#define BNSEPH_H

#include <newmat.h>

#include <QtCore>
#include <QThread>
#include <QtNetwork>


class t_eph {
 public:
  virtual ~t_eph() {};

  bool    isNewerThan(const t_eph* eph) const;
  QString prn() const {return _prn;}

  virtual void position(int GPSweek, double GPSweeks, ColumnVector& xc,
                        ColumnVector& vv) const = 0;
  virtual void read(const QStringList& lines) = 0;
  virtual int  IOD() const = 0;
 protected:  
  QString _prn;
  int     _GPSweek;
  double  _GPSweeks;
};

class t_ephGlo : public t_eph {
 public:
  t_ephGlo() {
    _xv.ReSize(6); 
  };
  virtual ~t_ephGlo() {};
  virtual void read(const QStringList& lines);
  virtual void position(int GPSweek, double GPSweeks, ColumnVector& xc,
                        ColumnVector& vv) const;
  virtual int  IOD() const;
 private:
  static ColumnVector glo_deriv(double /* tt */, const ColumnVector& xv);
  ColumnVector _xv;

  double _E;                  /* [days]   */
  double _tau;                /* [s]      */
  double _gamma;              /*          */
  double _x_pos;              /* [km]     */
  double _x_velocity;         /* [km/s]   */
  double _x_acceleration;     /* [km/s^2] */
  double _y_pos;              /* [km]     */
  double _y_velocity;         /* [km/s]   */
  double _y_acceleration;     /* [km/s^2] */
  double _z_pos;              /* [km]     */
  double _z_velocity;         /* [km/s]   */
  double _z_acceleration;     /* [km/s^2] */
  double _health;             /* 0 = O.K. */
  double _frequency_number;   /* ICD-GLONASS data position */
};


class t_ephGPS : public t_eph {
 public:
  t_ephGPS() {};
  virtual ~t_ephGPS() {};
  virtual void read(const QStringList& lines);
  virtual void position(int GPSweek, double GPSweeks, ColumnVector& xc,
                        ColumnVector& vv) const;
  virtual int  IOD() const {return int(_IODE);}

 private:
  double  _TOW;              //  [s]    
  double  _TOC;              //  [s]    
  double  _TOE;              //  [s]    
  double  _IODE;             
  double  _IODC;             

  double  _clock_bias;       //  [s]    
  double  _clock_drift;      //  [s/s]  
  double  _clock_driftrate;  //  [s/s^2]

  double  _Crs;              //  [m]    
  double  _Delta_n;          //  [rad/s]
  double  _M0;               //  [rad]  
  double  _Cuc;              //  [rad]  
  double  _e;                //         
  double  _Cus;              //  [rad]  
  double  _sqrt_A;           //  [m^0.5]
  double  _Cic;              //  [rad]  
  double  _OMEGA0;           //  [rad]  
  double  _Cis;              //  [rad]  
  double  _i0;               //  [rad]  
  double  _Crc;              //  [m]    
  double  _omega;            //  [rad]  
  double  _OMEGADOT;         //  [rad/s]
  double  _IDOT;             //  [rad/s]

  double  _TGD;              //  [s]    
};

class t_bnseph : public QThread {
 Q_OBJECT
 public:
  t_bnseph(QObject* parent = 0);
  virtual ~t_bnseph();  
  virtual void run();  

 signals:
  void newEph(t_eph* eph);
  void newMessage(const QByteArray msg);
  void error(const QByteArray msg);
 
 private:
  void reconnect();
  void readEph();
  QTcpSocket* _socket;
};
#endif
