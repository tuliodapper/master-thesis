#ifndef MOBILEAPP_H_
#define MOBILEAPP_H_

#include "sim.h"
#include "RoutingPacket_m.h"
#include "WiseBaseApplication.h"
#include "VirtualMobilityManager.h"
#include <vector>

enum ValueReportingTimers {
	SEND_DATA_PACKET	= 1,
	CHANGE_LOCATION		= 2
};

class MobileApp: public WiseBaseApplication {
public:
	MobileApp();
	virtual ~MobileApp();

protected:

	virtual void initialize();
	virtual void startup() {}
	virtual void finish();
	virtual void fromNetworkLayer(cPacket* pkt, const char* src, double rssi, double lqi);
    virtual void handleSensorReading(WiseSensorMessage* msg);
	void timerFiredCallback(int);

	SimMessage simMessage;
	SimMobility simMobility;
	mobility_info_t mobilityInfo;
	message_info_t messageInfo;
	void callBeaconSimplePkt(bool first = false);
	void callChangeLocation(bool first = false);

	int idTransmission = 0;	
	bool connected = false;
	bool checkConnected = true;

	
	// Video Message Type

	struct video{
		int nodeId;
		int sn;
		int id;
		FILE *pFile;
	};

	struct fec_frames {
		u_char** payload;
		int* idx;
		std::vector < uint64_t > packet_uid;
		int k;		// number of packets without redundancy
		int n;		// number of redundant pkts
		int eid;	// evalvid id
		int fhs;	// FEC header size
		int fds;	// FEC data size
		int count;	// number of received packets
		int nodeId;	// source node Id
		int videoId;
	};

	struct fec_parameters {
		unsigned int discard_eid_packets;
		int nodeId;
	};

	std::list<video> set;
	std::list<video>::iterator crtlIterator;
	vector <fec_frames> pktPool;
	vector <fec_parameters> discardEidPkts;

	void processMessageTypeVideo(WiseApplicationPacket * rcvPackets);
	bool FillPacketPool(int, int, const u_char *, int, int);
	bool ClearPacketPool(int);
	bool enoughPacketsReceived(int, int);
	void writeRdTrace(int id,int seqNum);
	bool rebuildFECBlock(int, int, int);
	void AddReceivedTrace(double, bool, TraceInfo tParam);
	
	//VirtualMobilityManager* mobilityModule;
	//VirtualMobilityManager* dnMobilityModule;

};

#endif /* CRITAPP_H_ */
