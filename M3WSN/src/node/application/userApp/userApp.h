#ifndef THROUGHPUTTEST_H_
#define THROUGHPUTTEST_H_

#include "WiseBaseApplication.h"
#include "VirtualMobilityManager.h"

enum userAppTimers {
	SEND_PACKET 	= 1,
	RECEIVER_SIDE	= 2,
	SENDER_SIDE	= 3,
};

struct  statisticsSink{
	int round;
	int recPkts;
	int hops;
	int total;
};

struct  statisticsSource{
	int round;
	int primaryPath;
	int broadcast;
	int total;
};

/*
 * Base class for criticality-based sensor nodes.
 */
class userApp: public WiseBaseApplication {
private:
	double packet_rate;
	string primaryPath;
	double startupDelay;
	float packet_spacing;
	int dataSN;

	int firstUserNodeId;
	int numUserNodes;

public:
	userApp();
	virtual ~userApp();

protected:
	virtual void initialize();
	virtual void startup() {}
	virtual void finish();
	virtual void finishSpecific() {};
	virtual void fromNetworkLayer(cPacket* pkt, const char* src, double rssi, double lqi);
	virtual void handleMobilityControlMessage(MobilityManagerMessage *);
	virtual void handleSensorReading(WiseSensorMessage* msg);
	virtual void handleDirectApplicationMessage(WiseApplicationPacket* pkt);
	void timerFiredCallback(int);

	double simTimeLimit;
	bool isSink;
	int type;
	vector <statisticsSink> statisticsReceiver;
	vector <statisticsSource> statisticsSender;
	int round;
	int nPktPrimary;
	int nPktBroadcast;
	int nRecPkt;
};

#endif /* CRITAPP_H_ */
