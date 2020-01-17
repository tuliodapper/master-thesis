package wise_mnet.node.application.userApp;

simple userApp like 
    wise_mnet.node.application.wiseCameraApplication.WiseCameraApplication {

parameters:
	@class(userApp);
	bool synchronizedStartup = default (true);
	bool neighbourDiscover = default (true);
	bool showCamImage = default (true);
	string applicationID = default ("userApp");
	bool collectTraceInfo = default (true);
	bool collectOutputInfo = default (false);
	int priority = default (1);
	int maxAppPacketSize = default (30);    // in bytes
	int packetHeaderOverhead = default (8); // in bytes
	int constantDataPayload = default (2);  // in bytes

	string nextRecipient = default ("0");	// Destination for packets produced in this node. 
						// This parameter can be used to create an
						// application-level static routing. This way we can
						// have a multi-hop throughput test   

	double latencyHistogramMax = default (300);
	int latencyHistogramBuckets = default (15);

	bool checkConnected 		= default (false);
   	bool sendDataPackets 		= default (false);
   	bool moveRandomPositions 	= default (false);
    string loadPath 			= default ("");
	double packet_rate 			= default (0);	// packets per second, by default we transmit no packets
	double startupDelay 		= default (0);	// delay in seconds before the app stars producing packets
	int firstUserNodeId 		= default(1);
	int numUserNodes 			= default(0);
	double simTimeLimit 		= default(0.0);

gates:
    output toApplicationDirect[];
    input fromApplicationDirect[];
    output toCommunicationModule;
    output toSensorDeviceManager;
    output toMobilityManager;
    input fromMobilityManager;
    input fromCommunicationModule;
    input fromSensorDeviceManager;
    input fromResourceManager;
}
