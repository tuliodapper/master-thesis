package castalia.node.communication.routing.SDN.CN;

simple CNRouting like castalia.node.communication.routing.iRouting {
parameters:
	// Inherithed
	bool 	collectTraceInfo 		= default (false);
	bool 	collectOutputInfo 		= default (false);
	int 	maxNetFrameSize 		= default (0);	// bytes;
	int 	netDataFrameOverhead	= default (10);	// bytes;
	int 	netBufferSize 			= default (100);	// number of messages

	// Own
	int 	TIME_TO_LIVE			= default (5);	// Destination address
	int 	MAX_RETRANSMITIONS		= default (3);
	double 	TIMEOUT_TRACE 			= default (0.1);
	double 	TIMEOUT_PACKETS 		= default (0.1);
	double 	TIMEOUT_KEEP_ALIVE 		= default (0.1);
	double 	INTERVAL_COUNTER 		= default (0.1);
	double 	TIMEOUT_REQUEST 		= default (0.1);
	int 	firstRelayNodeId		= default (1);	// bytes;
	int 	lastRelayNodeId 		= default (1);	// number of messages
gates:
	output	toCommunicationModule;
	output	toMacModule;
	input	fromCommunicationModule;
	input	fromMacModule;
	input	fromCommModuleResourceMgr;
}
