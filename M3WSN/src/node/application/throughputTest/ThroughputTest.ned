package wise_mnet.node.application.throughputTest;

simple ThroughputTest like 
    wise_mnet.node.application.wiseCameraApplication.WiseCameraApplication {

parameters:
	@class(ThroughputTest);
	bool synchronizedStartup = default (true);
	bool neighbourDiscover = default (true);
	bool showCamImage = default (true);
	string applicationID = default ("ThroughputTest");
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

	double packet_rate = default (0);	// packets per second, by default we transmit no packets
	double startupDelay = default (0);	// delay in seconds before the app stars producing packets

	double latencyHistogramMax = default (300);
	int latencyHistogramBuckets = default (15);
    bool isSink = default (false);

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
