package castalia.node.communication.routing.SDN.UN;

simple UNRouting like castalia.node.communication.routing.iRouting {
parameters:
	// Inherithed
	bool 	collectTraceInfo 		= default (false);
	bool 	collectOutputInfo 		= default (false);
	int 	maxNetFrameSize 		= default (0);	// bytes;
	int 	netDataFrameOverhead	= default (10);	// bytes;
	int 	netBufferSize 			= default (10);	// number of messages

	// Own
	int 	TIME_TO_LIVE			= default (5);	// Destination address
	double 	TIMEOUT_TRACE 			= default (0.1);
	double 	TIMEOUT_HELLO 			= default (0.1);
	double 	TIMEOUT_CHECK 			= default (0.1);
	double 	TIMEOUT_REFRESH 		= default (0.1);
	double 	TIMEOUT_STATUS 			= default (0.1);
	double 	TIMEOUT_BEACON			= default (0.1);
	
	bool 	sendBeacon				= default (false);
gates:
	output	toCommunicationModule;
	output	toMacModule;
	input	fromCommunicationModule;
	input	fromMacModule;
	input	fromCommModuleResourceMgr;
}
