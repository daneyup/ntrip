/* -------------------------------------------------------------------------
 * BKG NTRIP Client
 * -------------------------------------------------------------------------
 *
 * Class:      bncSettings
 *
 * Purpose:    Subclasses the QSettings
 *
 * Author:     L. Mervart
 *
 * Created:    25-Jan-2009
 *
 * Changes:    
 *
 * -----------------------------------------------------------------------*/

#include <QStringList>

#include "bncsettings.h"

// Constructor
////////////////////////////////////////////////////////////////////////////
bncSettings::bncSettings() : QSettings() {

  if (allKeys().size() == 0) {
    setValue("adviseFail",       "15");
    setValue("adviseReco",       "5");
    setValue("adviseScript",     "");
    setValue("autoStart",        "0");
    setValue("binSampl",         "0");
    setValue("casterUrlList", (QStringList() 
                               << "http://user:pass@www.euref-ip.net:2101" 
                               << "http://user:pass@www.igs-ip.net:2101"));
    setValue("corrIntr",         "1 day");
    setValue("corrPath",         "");
    setValue("corrPort",         "");
    setValue("corrTime",         "5");
    setValue("ephIntr",          "1 day");
    setValue("ephPath",          "");
    setValue("ephV3",            "0");
    setValue("logFile",          "");
    setValue("makePause",        "0");
    setValue("miscMount",        "");  
    setValue("mountPoints",      "");
    setValue("ntripVersion",     "1");
    setValue("obsRate",          "");
    setValue("onTheFlyInterval", "1 day");
    setValue("outEphPort",       "");
    setValue("outFile",          "");
    setValue("outPort",          "");
    setValue("outUPort",         "");
    setValue("perfIntr",         "");
    setValue("proxyHost",        "");
    setValue("proxyPort",        "");
    setValue("rnxAppend",        "0");
    setValue("rnxIntr",          "1 day");
    setValue("rnxPath",          "");
    setValue("rnxSampl",         "0");
    setValue("rnxScript",        "");
    setValue("rnxSkel",          "SKL");
    setValue("rnxV3",            "0");
    setValue("scanRTCM",         "0");
    setValue("serialAutoNMEA",   "0");
    setValue("serialBaudRate",   "9600");
    setValue("serialDataBits",   "8");
    setValue("serialMountPoint", "");
    setValue("serialParity",     "NONE");
    setValue("serialPortName",   "");
    setValue("serialStopBits",   "1");
    setValue("startTab",         "0");
    setValue("waitTime",         "5");
  }
}

