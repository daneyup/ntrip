
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <getopt.h>

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include "gen-cpp/RtnetData.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace com::gpssolutions::rtnet;
using namespace std;
using namespace boost;

class t_stationCrd {
 public:
  double _x;
  double _y;
  double _z;
};

map<string, t_stationCrd> _stationCrd;

// Handler Class Definition
//////////////////////////////////////////////////////////////////////////////
class RtnetClientHandler : public RtnetDataIf {
 public:
  RtnetClientHandler() {}
  ~RtnetClientHandler() {}

  void startDataStream() {}
  void registerRtnet(const RtnetInformation& info) {}
  void handleZDAmb(const vector<ZDAmb>& ambList) {}
  void handleDDAmbresBaselines(const vector<DDAmbresBaseline>& ambList) {}
  void handleSatelliteXYZ(const vector< SatelliteXYZ>& svXYZList);
  void handleStationInfo(const vector<StationInfo>& stationList);
  void handleStationAuxInfo(const vector<StationAuxInfo>& stationAuxList) {}
  void handleDGPSCorr(const vector<DGPSCorr>& dgpsList) {}
  void handleSatelliteClock(const vector<SatelliteClock>& svList) {}
  void handleEpochResults(const RtnetEpoch& epoch);
};

// Read Command-line Options
////////////////////////////////////////////////////////////////////////////////
void parseCmdLine(int argc, char* argv[], map<string, string>& OPT) {

  static struct option longOptions[] = {
    {"host", required_argument, 0, 'h'},
    {"port", required_argument, 0, 'p'},
  };

  while (true) {
    int index = 0;
    int iarg = getopt_long(argc, argv, "h:p:", longOptions, &index);
    if (iarg == -1) {
      break;
    }
    if (optarg) {
      OPT[longOptions[index].name] = string(optarg);
    }
    else {
      OPT[longOptions[index].name] = "y";
    }
  }
}

// Program
//////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {

  // Parse Input Options
  // -------------------
  map<string, string> OPT;
  parseCmdLine(argc, argv, OPT);
  if (OPT.find("port") == OPT.end()) {
    cerr << "usage: rtnetThriftClient [--host <host>] --port <port>" << endl;
    return 1;
  }
  string host = OPT.find("host") == OPT.end() ? "localhost" : OPT["host"];
  int    port; istringstream(OPT["port"]) >> port;

  shared_ptr<TSocket>     socket(new TSocket(host, port));
  shared_ptr<TTransport>  transport(new TBufferedTransport(socket)); 
  shared_ptr<TProtocol>   protocol(new TBinaryProtocol(transport));
  shared_ptr<RtnetDataIf> dataHandler(new RtnetClientHandler());
  shared_ptr<TProcessor>  processor(new RtnetDataProcessor(dataHandler));

  try {
    transport->open();

    while (processor->process(protocol,protocol,0)) {}

    transport->close();
  } 
  catch (TException& e) {
    cerr << "Caught an exception generated by Thrift: " << e.what() << endl;
    return 1;
  } 
  catch (...) {
    cerr << "Unknown exception" << endl;
    return 1;
  }

  return 0;
}

// Handle Satellite Positions
//////////////////////////////////////////////////////////////////////////////
void RtnetClientHandler::
handleSatelliteXYZ(const vector<SatelliteXYZ>& svXYZList) {
  cout.setf(ios::fixed);
  for (unsigned ii = 0; ii < svXYZList.size(); ii++) {
//    const SatelliteXYZ& sat = svXYZList[ii];
//    cout << unsigned(sat.ID) << ' '
//         << setprecision(3) << sat.xyz.x << ' '
//         << setprecision(3) << sat.xyz.y << ' '
//         << setprecision(3) << sat.xyz.z << endl;
  }
//  cout << endl;
}

// Handle Station Info
//////////////////////////////////////////////////////////////////////////////
void RtnetClientHandler::
handleStationInfo(const vector<StationInfo>& stationList) {
  for (unsigned ii = 0; ii < stationList.size(); ii++) {
    const StationInfo& staInfo = stationList[ii];
    _stationCrd[staInfo.ID]._x = staInfo.xyz.x;
    _stationCrd[staInfo.ID]._y = staInfo.xyz.y;
    _stationCrd[staInfo.ID]._z = staInfo.xyz.z;
  }
}

// Handle Eoch Results
//////////////////////////////////////////////////////////////////////////////
void RtnetClientHandler::
handleEpochResults(const RtnetEpoch& epoch) {
  for (unsigned ii = 0; ii < epoch.stationResultList.size(); ii++) {
    const StationResults& staRes = epoch.stationResultList[ii];
    cout << staRes.stationName << ' '
         << (int) staRes.nsv_gps_used << ' ' << (int) staRes.nsv_glonass_used << ' ';
    if (_stationCrd.find(staRes.stationName) != _stationCrd.end()) {
      cout << _stationCrd[staRes.stationName]._x << ' '
           << _stationCrd[staRes.stationName]._y << ' '
           << _stationCrd[staRes.stationName]._z;
    }
    cout << endl;
  }
}
