package castalia.node.communication.routing.SDN.RN;

simple RNRouting like castalia.node.communication.routing.iRouting {
parameters:
	// Inherithed
	bool 	collectTraceInfo 		= default (false);
	bool 	collectOutputInfo 		= default (false);
	int 	maxNetFrameSize 		= default (0);	// bytes;
	int 	netDataFrameOverhead	= default (10);	// bytes;
	int 	netBufferSize 			= default (10);	// number of messages

	// Own
	int 	TIME_TO_LIVE			= default (5);
	int 	MAX_RETRANSMITIONS		= default (3);
	double 	POSITION_RANGE_USER		= default (0.1);
	double 	INTERVAL_COUNTER 		= default (0.1);
	double 	TIMEOUT_TRACE 			= default (0.1);
	double 	TIMEOUT_KEEP_ALIVE 		= default (0.1);
	double 	TIMEOUT_PACKET_IN 		= default (0.1);
	double 	TIMEOUT_RESPONSE 		= default (0.1);
	double 	TIMEOUT_NOTIFY_USER 	= default (0.1);
	double 	TIMEOUT_STATUS 			= default (0.1);
	double 	TIMEOUT_TELEMETRY		= default (0.01);
	double 	TIMEOUT_BEACON			= default (0.1);

	bool 	sendTelemetry 			= default (true);	// number of messages
	bool 	sendBeacon				= default (false);
	
gates:
	output	toCommunicationModule;
	output	toMacModule;
	input	fromCommunicationModule;
	input	fromMacModule;
	input	fromCommModuleResourceMgr;
}
