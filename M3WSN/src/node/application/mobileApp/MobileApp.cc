#include "MobileApp.h"
#include "WiseDebug.h"
#include "sim.h"

Define_Module(MobileApp);

MobileApp::MobileApp() {
}

MobileApp::~MobileApp() {
}

void MobileApp::initialize() {

	WiseBaseApplication::initialize();
	bool sendDataPackets		= par("sendDataPackets").boolValue();
	bool moveRandomPositions 	= par("moveRandomPositions").boolValue();
	string loadPath 			= par("loadPath").stringValue();
	checkConnected 				= par("checkConnected").boolValue();

	//    cModule* mod    = getParentModule()->getModuleByRelativePath("MobilityManager");
	//    mobilityModule  = check_and_cast<VirtualMobilityManager*>(mod);

	if (sendDataPackets){
		simMessage = SimMessage();
		simMessage.Initialize(self, loadPath);
		callBeaconSimplePkt(true);
	}

	if (moveRandomPositions){
		simMobility = SimMobility();
		simMobility.Initialize(self, loadPath);
		callChangeLocation(true);
	}
}

void MobileApp::callBeaconSimplePkt(bool first){
	// cout << "callBeaconSimplePkt" << endl;
    if (simMessage.isThereNextMessageInfo())
    {
        // cout << "nextMessageInfo | id=" << self << " dest=" << nextMessageInfo.destination << endl;
        setTimer(SEND_DATA_PACKET, simMessage.getNextMessageInfoDelay());
    }
}

void MobileApp::callChangeLocation(bool first){
    // cout << "callChangeLocation" << endl;
	if (simMobility.isThereNextMobilityInfo())
	{
		// cout << "nextMobilityInfo | id=" << self << " x=" << nextMobilityInfo.x << " y=" << nextMobilityInfo.y << endl;
		setTimer(CHANGE_LOCATION, simMobility.getNextMobilityInfoDelay());
	}
}

void MobileApp::finish() {
	WiseBaseApplication::finish();
	cOwnedObject *Del=NULL;
	int OwnedSize = this->defaultListSize();
	for(int i=0; i<OwnedSize; i++){
		Del = this->defaultListGet(0);
		this->drop(Del);
		delete Del;
	}
}

void MobileApp::timerFiredCallback(int index){
	switch (index) {
		case CHANGE_LOCATION:{

			mobilityInfo = simMobility.retNextMobilityInfo();

			int x = mobilityInfo.x;
			int y = mobilityInfo.y;

			MobilityManagerMessage* m = new MobilityManagerMessage("move", MOBILE_MESSAGE);
			m->setXCoorDestination(x);
			m->setYCoorDestination(y);
			send(m, "toMobilityManager");

			callChangeLocation();

			break;
		}
		case SEND_DATA_PACKET:{

        	messageInfo = simMessage.retNextMessageInfo();

			if (checkConnected){
		    	WiseApplicationPacket *appMsg = new WiseApplicationPacket("APPLICATION_MESSAGE", APPLICATION_MESSAGE);
		    	toNetworkLayer(appMsg, SELF_NETWORK_ADDRESS);
	    	}

			if (not checkConnected or connected) {

				idTransmission++;

				cout 	<< "nodeId: " 			<< self << " | "
						<< "idTransmission: "	<< idTransmission << " | "
						<< "idVideo: 		"	<< messageInfo.idVideo << " | "
						<< "destination: " 		<< messageInfo.destination << endl;

				if (messageInfo.messageType = MESSAGE_TYPE_VIDEO) {
					WiseApplicationPacket* appMsg = new WiseApplicationPacket("REQUEST MULTIMEDIA PACKET", MULTIMEDIA_REQUEST_MESSAGE);
					appMsg->setIdNode(self);
					appMsg->setIdVideo(messageInfo.idVideo);
			    	appMsg->setByteLength(messageInfo.byteLength);
			    	appMsg->setMessageType(messageInfo.messageType);
			    	appMsg->setDestination(messageInfo.destination);
			    	appMsg->setIdTransmission(idTransmission);
					send(appMsg, "toSensorDeviceManager");
				} else {
			    	WiseApplicationPacket *appMsg = new WiseApplicationPacket("SENDING SIMPLE PACKET", APPLICATION_PACKET);
			    	appMsg->setByteLength(messageInfo.byteLength);
			    	appMsg->setMessageType(messageInfo.messageType);
			    	appMsg->setIdTransmission(idTransmission);
			    	toNetworkLayer(appMsg, to_string(messageInfo.destination).c_str());
				}

			} else {
				// TO DO: Gerar log
			}

			callBeaconSimplePkt();

		}
	}
}

void MobileApp::handleSensorReading(WiseSensorMessage* msg) {
    int msgKind = msg->getKind();
    switch (msgKind) {
        case MULTIMEDIA:{
            WiseApplicationPacket *pktTrace = new WiseApplicationPacket("SENDING MULTIMEDIA DATA", APPLICATION_PACKET);
            pktTrace->setByteLength(msg->getByteLength());
            pktTrace->setMessageType(messageInfo.messageType);
            pktTrace->setIdFrame(msg->getIdFrame());
            pktTrace->setFrame(msg->getFrame());
            pktTrace->setInfo(msg->getInfo());
            pktTrace->setFecPktArraySize(msg->getFecPktArraySize());
            for (int i = 0; i<msg->getFecPktArraySize(); i++)
                pktTrace->setFecPkt(i, msg->getFecPkt(i));
            pktTrace->setIdVideo(msg->getInfo().seqNum);
            pktTrace->setIdTransmission(msg->getInfo().idTransmission);
            pktTrace->setRelevance(msg->getRelevance());
            toNetworkLayer(pktTrace, to_string(messageInfo.destination).c_str());
            trace() << "Video id " << pktTrace->getIdVideo() << " frame id " << pktTrace->getFrame() << ", " << pktTrace->getInfo().frameType << "-frame " << pktTrace->getByteLength() << " bytes";
            break;
        }
    }
}

void MobileApp::fromNetworkLayer(cPacket* msg, const char* src, double rssi, double lqi) {
	switch (msg->getKind()) {
		case APPLICATION_MESSAGE:{
			ApplicationPacket *appmsg = check_and_cast<ApplicationPacket*>(msg);
			connected = appmsg->getConnected();
			break;
		}
		case APPLICATION_PACKET:{
			WiseApplicationPacket *rcvPackets = check_and_cast<WiseApplicationPacket*>(msg);
			if (rcvPackets->getMessageType() == MESSAGE_TYPE_VIDEO) {
				processMessageTypeVideo(rcvPackets);
			}
			break;
		}
		case MOBILE_MESSAGE:{
			ApplicationPacket *appmsg = check_and_cast<ApplicationPacket*>(msg);
			MobilityManagerMessage* m = new MobilityManagerMessage("move", MOBILE_MESSAGE);
			m->setXCoorDestination(appmsg->getX());
			m->setYCoorDestination(appmsg->getY());
            send(m, "toMobilityManager");
			break;
		}
	}
}

void MobileApp::processMessageTypeVideo(WiseApplicationPacket * rcvPackets) {
	TraceInfo tParam = rcvPackets->getInfo();
	//se o tamanho do fec_header for 0, singifica que eh um pacote sem fec
	if(rcvPackets->getFecPktArraySize() == 0){
		trace() << "APP- Node " << self << " received a frame number " << rcvPackets->getInfo().idFrame << " from node " << rcvPackets->getInfo().nodeId << " for video id " << rcvPackets->getIdVideo() << " -- whithout FEC";
		int indexPool = -1;
		for(int i =0; i<pktPool.size(); i++){
			if(pktPool[i].nodeId == rcvPackets->getInfo().nodeId){
				indexPool = i;
				break;
			}
		}
		if (indexPool != -1){
			rebuildFECBlock(tParam.nodeId, tParam.seqNum, indexPool);
			for(int i =0; i<discardEidPkts.size(); i++){
				if(discardEidPkts[i].nodeId == rcvPackets->getInfo().nodeId){
					discardEidPkts[i].discard_eid_packets = 0;
					trace() << "    discard_eid_packets " << discardEidPkts[i].discard_eid_packets;
					break;
				}
			}
		}
		// adicionar como pacote sem fec
		AddReceivedTrace(SIMTIME_DBL(simTime()), true, tParam);
		WiseApplicationPacket *pktTrace = new WiseApplicationPacket("received video frame", APPLICATION_STATISTICS_MESSAGE);
		pktTrace->setIdVideo(tParam.seqNum);
		pktTrace->setIdNode(tParam.nodeId);
		pktTrace->setIdFrame(1);
		toNetworkLayer(pktTrace, BROADCAST_NETWORK_ADDRESS);
		trace() << "whithout FEC included => node id " << tParam.nodeId << " seq number " << tParam.seqNum << " frame id " << tParam.idFrame << "\n";
	} else{
		trace() << "APP- Node " << self << " received a frame number " << rcvPackets->getInfo().idFrame << " from node " << rcvPackets->getInfo().nodeId << " for video id " << rcvPackets->getIdVideo() << " -- whith FEC";;
		u_char* temp_packet = (u_char*)malloc(rcvPackets->getFecPktArraySize());
		for(int i = 0; i < rcvPackets->getFecPktArraySize(); i++)
			temp_packet[i] = rcvPackets->getFecPkt(i);

		trace() << "packet data -- fec_header_size: " << rcvPackets->getFecPkt(0) << " k: " << rcvPackets->getFecPkt(1) << " n: " << rcvPackets->getFecPkt(2) << " i: " << rcvPackets->getFecPkt(3) << " evalvid id: " << rcvPackets->getFecPkt(4);

		int indexEidPkts = -1;
		for(int i =0; i<discardEidPkts.size(); i++){
			if(discardEidPkts[i].nodeId == rcvPackets->getInfo().nodeId){
				indexEidPkts = i;
				break;
			}
		}
		if(indexEidPkts == -1){
			fec_parameters temp;
			temp.discard_eid_packets = 0;
			temp.nodeId = rcvPackets->getInfo().nodeId;
			discardEidPkts.push_back(temp);
			indexEidPkts = discardEidPkts.size() - 1;
		}

		int indexPool = -1;
		for(int i =0; i<pktPool.size(); i++){
			if(pktPool[i].nodeId == rcvPackets->getInfo().nodeId){
				indexPool = i;
				break;
			}
		}

		if (discardEidPkts[indexEidPkts].discard_eid_packets == 0 && indexPool == -1){
			trace() << "    significa que nao tenho nenhum bloco de fec na memoria para esse src, comecar um";
			FillPacketPool(rcvPackets->getIdFrame(), rcvPackets->getByteLength()-temp_packet[0], temp_packet, rcvPackets->getInfo().nodeId, rcvPackets->getIdVideo());
			int indexPool = pktPool.size() - 1;
			trace() << "    -- criando novo bloco para receber pacotes eid: " << pktPool[indexPool].eid << " k: " << pktPool[indexPool].k << " n: " << pktPool[indexPool].n << " nodeId: " << pktPool[indexPool].nodeId << " pktPool size " << pktPool.size();
		trace() << "end";
		} else if (discardEidPkts[indexEidPkts].discard_eid_packets == 0 && indexPool > -1){
			// se for ==0 eh sinal que ainda nao antingiu o numero suficiente de pacotes
			// porem, preciso testar se ainda estou recebendo os pacotes com o mesmo EvalvidID
			// caso discard_eid_packets for > 0 ela retem o EID do ultimo bloco de fec recebido
			// possibilitando descartar caso sejam pacotes a mais ou comecar a montar o novo
			// bloco caso seja diferente
			int differentEID = -1;
			if (pktPool[indexPool].eid != temp_packet[4])
				differentEID = indexPool;
			if(differentEID == -1){
				trace() << "    O bloco de fec que esta na memoria tenha o mesmo EID (" << pktPool[indexPool].eid << ") que o novo pacote";
				memmove(pktPool[indexPool].payload[pktPool[indexPool].count], temp_packet+pktPool[indexPool].fhs, pktPool[indexPool].fds);
				pktPool[indexPool].idx[pktPool[indexPool].count] = (int) temp_packet[3];
				//Ajustar os uid dos pacotes, pq senao posso ficar com id's diferentes no sd_file e rd_file
				// TODO: melhorar a explicacao da necessidade de ajuste do uid
				int adjusted_uid = (rcvPackets->getIdFrame()-(int)temp_packet[3])+pktPool[indexPool].count;
				pktPool[indexPool].packet_uid.push_back(adjusted_uid);
				pktPool[indexPool].count++;
				trace() << "    packet pool data -- " << "fhs: "	<< pktPool[indexPool].fhs << " k: " << pktPool[indexPool].k << " n: " << pktPool[indexPool].n << " eid: " << pktPool[indexPool].eid << " count: " << pktPool[indexPool].count << " uid: " << adjusted_uid;
				trace() << "end";
			} else if(differentEID != -1){
				trace() << "    ** Different Evalvid ID before packet pool clean, received eid: " << (int) temp_packet[4] << " packet pool eid: " << pktPool[indexPool].eid << " k: " << pktPool[indexPool].k << " n: " << pktPool[indexPool].n << " count: " << pktPool[indexPool].count << " nodeId: " << pktPool[indexPool].nodeId << " videoId: " << pktPool[indexPool].videoId;
				rebuildFECBlock(pktPool[indexPool].nodeId, pktPool[indexPool].videoId, differentEID);
				discardEidPkts[indexEidPkts].discard_eid_packets = 0;
				trace() << "    discard_eid_packets " << discardEidPkts[indexEidPkts].discard_eid_packets;
				FillPacketPool(rcvPackets->getIdFrame(), rcvPackets->getByteLength()-temp_packet[0], temp_packet, rcvPackets->getInfo().nodeId, rcvPackets->getIdVideo());
				trace() << "end";
			}
		} else if (discardEidPkts[indexEidPkts].discard_eid_packets != temp_packet[4]){
			trace() << "    recebi um numero suficiente de pacotes com FEC, descartar os mais com o mesmo EvalvidID";
			discardEidPkts[indexEidPkts].discard_eid_packets = 0;
			trace() << "    discard_eid_packets " << discardEidPkts[indexEidPkts].discard_eid_packets;
			FillPacketPool(rcvPackets->getIdFrame(), rcvPackets->getByteLength()-temp_packet[0], temp_packet, rcvPackets->getInfo().nodeId, rcvPackets->getIdVideo());
			trace() << "end";
		} else{
			trace() << "    drop " << pktPool.size();
		}

		int included = -1;
		for(int i =0; i<pktPool.size(); i++){
			if(pktPool[i].nodeId == rcvPackets->getInfo().nodeId){
				included = i;
				break;
			}
		}
		if (enoughPacketsReceived(included, indexEidPkts)){
			trace() << "    ++ criando novo bloco para receber pacotes (2) eid: " << pktPool[included].eid << " k: " << pktPool[included].k << " n: " << pktPool[included].n;
			discardEidPkts[indexEidPkts].discard_eid_packets = pktPool[included].eid;
			rebuildFECBlock(tParam.nodeId, tParam.seqNum, included);
		}
		free(temp_packet);
	}
}

void MobileApp::AddReceivedTrace(double time, bool generateTrace, TraceInfo tParam) {
	if (generateTrace){
		if(set.empty()){
			writeRdTrace(tParam.nodeId,tParam.seqNum);
			//trace() << "Frame " << tParam.id << " added";
		}
		for (crtlIterator = set.begin(); crtlIterator != set.end(); crtlIterator++){
			video info = *crtlIterator;
			/*
		 	 * Test if the output file is already created
		 	 * If the packtes is for the current seq number save the frame on the output file
		 	 * else close the file and create the new output file with the new seq number
			*/
			if (tParam.nodeId == info.nodeId && tParam.seqNum == info.sn){
				fprintf(info.pFile, "%f             id %d            udp %d\n",time, tParam.idFrame, tParam.byteLength);
				WiseApplicationPacket *pktTrace = new WiseApplicationPacket("received video frame", APPLICATION_STATISTICS_MESSAGE);
				pktTrace->setIdVideo(info.sn);
				pktTrace->setIdNode(info.nodeId);
				pktTrace->setIdFrame(0);
				toNetworkLayer(pktTrace, BROADCAST_NETWORK_ADDRESS);
				//trace() << "Frame " << tParam.id << " added";
			} else if (tParam.nodeId != info.nodeId || tParam.seqNum != info.sn){
		   		fclose(info.pFile);
				crtlIterator = set.erase(crtlIterator);
				writeRdTrace(tParam.nodeId, tParam.seqNum);
			}
		}
	}
}

bool MobileApp::FillPacketPool(int packet_uid, int fec_data_size, const u_char *tmp_packet, int nodeId, int videoId) {
	// carregando os campos
	fec_frames temp;
	temp.fhs	= (int) tmp_packet[0];	// fec header size 
	temp.k		= (int) tmp_packet[1];	// number of packets without redundancy
	temp.n		= (int) tmp_packet[2];	// number of redundant pkts
	temp.eid	= (int) tmp_packet[4];	// evalvid id
	temp.fds	= fec_data_size; 	// fec data size
	temp.nodeId	= nodeId;		// source node Id
	temp.videoId	= videoId;		// video id
	temp.count	= 0;			// TODO: verificar

	// preciso ajustar os uid dos pacotes, pq senao posso ficar com id's diferentes no sd_file e rd_file
	// TODO: melhorar a explicacao da necessidade de ajuste do uid
	int adjusted_uid = (packet_uid-tmp_packet[3])+temp.count;
	temp.packet_uid.push_back(adjusted_uid);

	// reservando espaco para o array dinamico (utilizado array pq o fec decode nao aceita vector)
	temp.idx = new int[temp.k];
	temp.idx[temp.count] = (int)tmp_packet[3];
	
	// reservando espaco para o buffer
	temp.payload = (u_char**)malloc(temp.k * sizeof(void *));
	for (int i = 0 ; i < temp.k ; i++ ) 
		temp.payload[i] = (u_char*)malloc(temp.fds);

	memmove(temp.payload[temp.count], tmp_packet+temp.fhs, temp.fds);

	temp.count++;
	pktPool.push_back(temp);
	trace() << "    pktPool size " << pktPool.size();
	return true;
}

bool MobileApp::rebuildFECBlock(int nodeId, int seqNum, int index){
	if (pktPool[index].count>0) {
		trace() << "    rebuildFECBlock::received " << pktPool[index].count << " of " << pktPool[index].n << " k: " << pktPool[index].k << " eid: " << pktPool[index].eid << " for node id " << pktPool[index].nodeId;
		int k(pktPool[index].k);
		int n(pktPool[index].n);
		int sz(pktPool[index].fds);
		
		ReedSolomonFec neoFec;
		void *newCode = neoFec.create(k,n);

		if (neoFec.decode((fec_parms *)newCode, (void**)pktPool[index].payload, pktPool[index].idx, sz)) {
			//trace() << "    rebuildFECBlock::detected singular matrix ..." << " n " << n;
		} 
		
		u_char **d_original = (u_char**)neoFec.safeMalloc(k * sizeof(void *), "d_original ptr");
		for (int i = 0 ; i < k ; i++ ) {
			d_original[i] = (u_char*)neoFec.safeMalloc(sz, "d_original data");
		}
		
		neoFec.BuildSampleData(d_original, k, sz);

		int errors(0);
		for (int i=0; i<k; i++){
			if (bcmp(d_original[i], pktPool[index].payload[i], sz )) {
				errors++;
				trace() << "    " << errors << " - rebuildFECBlock::error reconstructing block " << i << " k " << k << " n " << n;
			}
		}
		int included = 0;
		// adiciono o log separadamente da validacao pq algumas vezes dava erro em relacao aos pacotes
		// recuperados ... ex recebia 1,2,4 de 4 ... mas o fec recuperava 2,3,4 ocasionando pois nao
		// havia o ID do pacote 3 (que foi perdido)
		// entao desconsidero quais foram recuperados e adiciono os que possuem ID valido
		for (int i=0; i<k-errors; i++) {
			TraceInfo tParam;
			tParam.idFrame = pktPool[index].packet_uid[i];
			tParam.byteLength = sz + 5;
			tParam.nodeId = nodeId;
			tParam.seqNum = seqNum;
			AddReceivedTrace(SIMTIME_DBL(simTime()), true, tParam);
			included ++;
			trace() << "    whith FEC included => node id " << tParam.nodeId << " seq number " << tParam.seqNum << " frame id " << tParam.idFrame << "\n";
			if (included == pktPool[index].k){
					WiseApplicationPacket *pktTrace = new WiseApplicationPacket("received video frame", APPLICATION_STATISTICS_MESSAGE);
					pktTrace->setIdVideo(tParam.seqNum);
					pktTrace->setIdNode(tParam.nodeId);
					pktTrace->setIdFrame(1);
					toNetworkLayer(pktTrace, BROADCAST_NETWORK_ADDRESS);
			}
		}
		
		neoFec.destroy((fec_parms*)newCode);
		if (d_original != NULL) {
			for (int i = 0 ; i < k ; i++ ){ 
				free(d_original[i]);
				d_original[i] = NULL;
			}
		}
		free(d_original);
		d_original = NULL ;
		ClearPacketPool(index);
		trace() << "    pktPool size " << pktPool.size();
		return true;
	}
	return false;
}

bool MobileApp::ClearPacketPool(int index) {
	pktPool.erase(pktPool.begin()+index);
	return true;
}

bool MobileApp::enoughPacketsReceived(int index, int indexEidPkts) {
	trace() << "    enoughPacketsReceived";
	trace() << "    discard_eid_packets: " << discardEidPkts[indexEidPkts].discard_eid_packets << " count: " << pktPool[index].count << " k-1: " << pktPool[index].k-1;
	if ( (discardEidPkts[indexEidPkts].discard_eid_packets == 0) && (pktPool[index].count > (pktPool[index].k-1)) ) {
		trace() << "    true";
		return true;
	}
	trace() << "    false\n";
	return false;
}

void MobileApp::writeRdTrace(int nodeId,int seqNum){
	char fileTrace[50];
	sprintf(fileTrace,"rd_sn_%i_nodeId_%i", seqNum,nodeId);
	video temp;
	temp.nodeId = nodeId;
	temp.sn = seqNum;
	temp.pFile = fopen(fileTrace, "a+");
	set.push_back(temp);
}
