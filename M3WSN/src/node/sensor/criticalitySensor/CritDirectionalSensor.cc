//*******************************************************************************
//*	Copyright (c) 2013. Federal University of Para (UFPA), Brazil and 	*
//*			    University of Bern (UBern), Switzerland		*
//*	Developed by Research Group on Computer Network and Multimedia		*
//*	Communication (GERCOM) of UFPA in collaboration to Communication and 	*
//*	Distributed Systems (CDS) research group of UBern.			*
//*	All rights reserved							*
//*										*
//*	Permission to use, copy, modify, and distribute this protocol and its	*
//*	documentation for any purpose, without fee, and without written		*
//*	agreement is hereby granted, provided that the above copyright notice,	*
//*	and the author appear in all copies of this protocol.			*
//*										*
//*  	Module: receiver trace creation to QoE-aware FEC mechanism		*
//*										*
//*	Ref.:	Z. Zhao, T. Braun, D. Ros´ario, E. Cerqueira, R. Immich, and 	*
//*		M. Curado, “QoE-aware FEC mechanism for intrusion detection in 	*
//*		multi-tier wireless multimedia sensor networks,” 		*
//*		in Proceedings of the 1st International Workshop on Wireless 	*
//*		Multimedia Sensor Networks (WiMob’12 WSWMSN), Barcelona, Spain,	*
//*		oct. 2012, pp. 697–704.						*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*		 Zhongliang Zhao <zhao@iam.unibe.ch>				*
//*										*
//******************************************************************************/ 

#include <map>
#include <algorithm>

#include "CritDirectionalSensor.h"
#include "Segment.h"
#include "WiseBaseApplication.h"
#include "WiseMovingTargetMessage_m.h"

Define_Module(CritDirectionalSensor);

CritDirectionalSensor::CritDirectionalSensor() {
    // TODO Auto-generated constructor stub
}

CritDirectionalSensor::~CritDirectionalSensor() {
    // TODO Auto-generated destructor stub
}

/**
 * Compute sets of nodes that cover this sensors FOV.
 */
void CritDirectionalSensor::computeCoverSets() {
    std::vector<Node> neighbors = app->getNeighbors();
    std::set<unsigned int> A, B, C, G, GP, GV, GB, GC, AG, BG, CG;

    // Build set A, B, C, G to compute cover-sets
    std::vector<Node>::const_iterator it = neighbors.begin();
    for ( ; it != neighbors.end(); ++it ) {
        if ( it->fov.contains(p) ) {
            A.insert(it->addr);
        }

        if ( it->fov.contains(b) ) {
            B.insert(it->addr);
        }

        if ( it->fov.contains(c) ) {
            C.insert(it->addr);
        }

        if ( it->fov.contains(g) ) {
            G.insert(it->addr);
        }
    }

    AG = computeIntersection(A, G);
    BG = computeIntersection(B, G);
    CG = computeIntersection(C, G);

    // Construct cover-sets by computing AG x BG x CG
    std::map<std::string, std::set<unsigned int> > AGxBG;
    std::set<unsigned int>::iterator ag, bg;

    availableCoverSets.clear();

    // Compute AG x BG
    for (ag = AG.begin(); ag != AG.end(); ++ag) {
        for (bg = BG.begin(); bg != BG.end(); ++bg) {
            std::set<unsigned int> pair;
            pair.insert(*ag);
            pair.insert(*bg);
            std::string key = keyFromSet(pair);
            if (AGxBG.count(key) == 0) {
                AGxBG[key] = pair;
            }
        }
    }

    std::map<std::string, std::set<unsigned int> > AGxBGxCG;
    std::map<std::string, std::set<unsigned int> >::iterator agxbg;
    std::set<unsigned int>::iterator cg;

    // Compute (AG x BG) X CG
    for (agxbg = AGxBG.begin(); agxbg != AGxBG.end(); ++agxbg) {
        for (cg = CG.begin(); cg != CG.end(); ++cg) {
            agxbg->second.insert(*cg);
            std::string key = keyFromSet(agxbg->second);
            if (AGxBGxCG.count(key) == 0) {
                AGxBGxCG[key] = agxbg->second;
                availableCoverSets.insert(agxbg->second);
            }
        }
    }

    numCoverSets = availableCoverSets.size();

    // print cover sets
    std::set<std::set<unsigned int> >::iterator it1 = availableCoverSets.begin();
    EV << "Available cover sets:" << std::endl;

    for ( ; it1 != availableCoverSets.end(); ++it1 ) {
        std::set<unsigned int>::iterator it2 = it1->begin();
        for ( ; it2 != it1->end(); ++it2 ) {
            EV << *it2;
        }
        EV << std::endl;
    }
}

/**
 *
 */
void CritDirectionalSensor::initialize() {
    CritSensor::initialize();
    isSink = par("isSink");
    fileIn = par("video");
    max = par("max_fragmented_size");
    draw_node();
    setupMsg = new cMessage("setup", 10);

    contSd = 0;
    nLin = 0;
    nLT = 0;
	sensorlastSampleTime.clear();
	sensorLastValue.clear();
	double theBias;
	totalSensors = 1;
	for (int i = 0; i < totalSensors; i++) {
		sensorLastValue.push_back(-1);
		sensorlastSampleTime.push_back(-100000.0);
		theBias = normal(0, sensorBiasSigma[i]);	// using rng generator --> "0" 
		sensorBias.push_back(theBias);
	}
}


/**
 *
 */
void CritDirectionalSensor::finish() {
    CritSensor::finish();
    cancelAndDelete(setupMsg);
}

/**
 *
 */
std::set<unsigned int> CritDirectionalSensor::computeIntersection(const std::set<unsigned int>& A, const std::set<unsigned int>& B) const {
    std::vector<int> vectorA(A.begin(), A.end());
    std::vector<int> vectorG(B.begin(), B.end());
    std::vector<int> intersection;

    std::set_intersection(vectorA.begin(), vectorA.end(), vectorG.begin(), vectorG.end(),
            std::back_inserter(intersection));

    return std::set<unsigned int>(intersection.begin(), intersection.end());
}

/**
 * Handle sample from physical process
 *
 * TODO extend/modify for WiseVideFile
 */
void CritDirectionalSensor::handleSample(cMessage* msg) {
    int msgKind = msg->getKind();
    switch (msgKind) {
	case PHYSICAL_PROCESS_SCALAR_DATA:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    if (reqMsg->getSource() == self){
		intruder.x = reqMsg->getX();
		intruder.y = reqMsg->getY();
	    	increaseCriticality();
		if (reqMsg->getAngle() == 1)
		    change_angle();
	    } else
		dinamicIncreaseOfCriticality();
	    break;
	}

	/*
	 * received a msg from App layer to start send multimedia content
	*/
	case SENSOR_READING_MESSAGE:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    dataTracer.nodeId = reqMsg->getIdNode();
	    dataTracer.seqNum = reqMsg->getIdVideo();
	    dataTracer.idTransmission = reqMsg->getIdTransmission();
	    trace() << "Node " << dataTracer.nodeId << " should start send multimedia with seq number " << dataTracer.seqNum;
	    scheduleAt(simTime() + 0, setupMsg);
	    break;
	}
	case MULTIMEDIA_REQUEST_MESSAGE:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    dataTracer.nodeId = reqMsg->getIdNode();
	    dataTracer.seqNum = reqMsg->getIdVideo();
	    dataTracer.idTransmission = reqMsg->getIdTransmission();
	    gopId = 0;
	    trace() << "Node " << dataTracer.nodeId << " should start send multimedia with seq number " << dataTracer.seqNum;
	    scheduleAt(simTime() + 0, setupMsg);
	    break;
	}

	case MOBILE_MESSAGE:{
	    WiseApplicationPacket *reqMsg  = check_and_cast<WiseApplicationPacket *>(msg);
	    if (reqMsg->getSource() == self){
		newLocation.x = reqMsg->getX();
		newLocation.y = reqMsg->getY();
	    	changeAngle = true;
		update_location();
	    }
	    break;
	}
	case PHYSICAL_PROCESS_SAMPLING:{
            /*WiseMovingTargetMessage* m = check_and_cast<WiseMovingTargetMessage*>(msg);
	    WiseSensorMessage *reportMsg = new WiseSensorMessage("sensor reading msg", SENSOR_READING);
	    reportMsg->setSensorType(sensorTypes[0].c_str());
	    reportMsg->setSensedValue(m->getValue());
	    reportMsg->setSensorIndex(0);
	    reportMsg->setX(p.x);
	    reportMsg->setY(p.y);
	    //reportMsg->setSource(self);
	    send(reportMsg, "toApplicationModule");	//send the sensor reading to the Application module*/


	    PhysicalProcessMessage *phyReply = check_and_cast<PhysicalProcessMessage*>(msg);
	    int sensorIndex = phyReply->getSensorIndex();
	    double theValue = phyReply->getValue();

	    // add the sensor's Bias and the random noise 
	    theValue += sensorBias[sensorIndex];
	    /*theValue += normal(0, sensorNoiseSigma[sensorIndex], 1);

	    // process the limitations of the sensing device (sensitivity, resoultion and saturation)
	    /*if (theValue < sensorSensitivity[sensorIndex])
		theValue = sensorSensitivity[sensorIndex];
	    if (theValue > sensorSaturation[sensorIndex])
	    theValue = sensorSaturation[sensorIndex];

	    theValue = sensorResolution[sensorIndex] * lrint(theValue / sensorResolution[sensorIndex]);
	    sensorLastValue[sensorIndex] = theValue;*/

	    WiseSensorMessage *reportMsg = new WiseSensorMessage("sensor reading msg", SENSOR_READING);
	    reportMsg->setSensorType(sensorTypes[sensorIndex].c_str());
	    reportMsg->setSensedValue(theValue);
	    reportMsg->setSensorIndex(sensorIndex);

	    send(reportMsg, "toApplicationModule");	//send the sensor reading to the Application module

		//trace() << sensorBias.size();
		//trace() << "DLR - type " << reportMsg->getSensorType();
		//trace() << "DLR - value " << reportMsg->getSensedValue();
		//trace() << "DLR - index " << reportMsg->getSensorIndex();

	    break;
	}
    }
    delete msg;
}

void CritDirectionalSensor::update_location(){
	if (changeAngle ){
    		int intersections = 0;
	    	lineOfSight = 0;
	    	Segment direction[2] = {
		 Segment(newLocation.x, newLocation.y, p.x, p.y),
	         Segment(p.x, p.y, newLocation.x, newLocation.y)
		};
	    	// Find intersections between the sensor area and the line of sight of the camera
	    	for (int i=0; i<32; i++) {
	    		lineOfSight = lineOfSight + 0.25;
			temp_v.x = dov * cos(lineOfSight) + p.x;
			temp_v.y = dov * sin(lineOfSight) + p.y;
	
			Segment line[1] = {
		          Segment(p.x, p.y, temp_v.x, temp_v.y)
			};
	
		    	for ( int i = 0; i < 1; i++ ) {
		            	for ( int l = 0; l < 8; l++ ) {
					if ( line[i].intersects(direction[l]) )
		                   		intersections++;
		            	}
		        }
		
			if (intersections > 30){
				changeAngle = false;
				break;
			}
		}
	}

        //remove the moving object and the sensor node fov
	terrain->clean_mobile();

    	NodeLocation location = mobilityModule->getLocation();
    	p.x = location.x;
    	p.y = location.y;

    	// find point v, b, c
    	v.x = dov * cos(lineOfSight) + p.x;
    	v.y = dov * sin(lineOfSight) + p.y;

    	double h = dov * sin(aov);
    	double d = dov * cos(aov);

    	b.x = v.x + h * (v.y - p.y) / d;
    	b.y = v.y - h * (v.x - p.x) / d;

    	c.x = v.x - h * (v.y - p.y) / d;
    	c.y = v.y + h * (v.x - p.x) / d;

    	// point g (center of mass)
    	g.x = (p.x + b.x + c.x) / 3;
    	g.y = (p.y + b.y + c.y) / 3;

    	// fill in the Triangle structure for geometric manipulation
    	myFoV = Triangle(p, b, c);

    	// draw node and camera FOV in opencv GUI
    	WiseCameraInfo::fov_triangle_t fov;
    	fov.a_x = myFoV.v1.x;
    	fov.a_y = myFoV.v1.y;
    	fov.b_x = myFoV.v2.x;
    	fov.b_y = myFoV.v2.y;
    	fov.c_x = myFoV.v3.x;
    	fov.c_y = myFoV.v3.y;

    	//trace() << "Node " << self << " has to change the angle" << "\n";
    	//trace() << "Sensor - Node " << self << " change location to  (" << newLocation.x << "," << newLocation.y << ")";

    	if(!isSink){
    		caminfo.set_triangle_fov(fov);
	    	caminfo.set_position(location.x, location.y, 0);
    		terrain->place_mobile(caminfo);
	} else {
    		caminfo.set_position(location.x, location.y, 0);
	    	terrain->place_BS(caminfo);
    	}
    	//trace() << "updated the node location\n";
}

void CritDirectionalSensor::handleSensorMessage(cMessage* msg) {
	if (msg->getKind() == 10){
		trace() << "SETUP_TRACE timmer";
		setup();
		getNextFrame(ndx, trec_);
		id = 0;
		cancelEvent(processTrace);
		scheduleAt(simTime() + 0, processTrace);
	} else if (msg->getKind() == 11){//PROCESS_TRACE timmer
		trace() << "node " << dataTracer.nodeId << " sending multimedia with pkts number " << dataTracer.idFrame +1;
		double energy = energyPerSample;// + uniform(0, 1) * (energyPerSample / 100);
		powerDrawn(energy);
		trace() << "consume energy " << energy;
		processStreamData();
	}
}

void CritDirectionalSensor::setup(){
    trace() << "setup";
    unsigned long id, size, prio;
    double time;
    char type;
    tracerec* t;
    ndx = 0;
    nrec = 0;
    id = 0;
    a_ = 0;
    contFrm = 0;
    contSd = 0;
    nLT = 0;
    pos_frame = 0;
    uid = 0;


    char frdTrace[55];
    sprintf(frdTrace,"rd_sn_%i_nodeId_%i",dataTracer.seqNum, dataTracer.nodeId);
    rdTrace.open(frdTrace, ios::out);
    rdTrace.close();
    rdTrace.clear();

    char fTrace[55];
    sprintf(fTrace,"sd_sn_%i_nodeId_%i",dataTracer.seqNum, dataTracer.nodeId);
    sdTrace.open(fTrace, ios::out);
    if (sdTrace.bad()) {
    	trace() << "Error while opening " << fTrace;
    	error("Error while opening input file\n");
	return;
    }
    sdTrace.close();
    sdTrace.clear();
    sdTrace.open(fTrace, ios::out);

    ifstream traceFile(fileIn, ios::in);
    if (traceFile.bad()) {
        trace() << "Error while opening " << fileIn;
        error("Error while opening input file\n");
        return;
    }

    while (traceFile >> id >> type >> size >> prio >> time) {
        nrec++;
    }
		
    contLTrace(nrec);
    traceFile.clear();
    traceFile.seekg(0);
    tracer = new struct tracerec[nrec];
    t = tracer;
    last_rand_id = 0;

    while (traceFile >> id >> type >> size >> prio >> time) {
        t->trec_id = id;
	t->trec_type = type;
        t->trec_size = size;
        t->trec_prio = prio;
        t->trec_time = time;
        t++;
    }
}

void CritDirectionalSensor::getNextFrame(unsigned int& ndx, struct tracerec& t) {
    if (uid != 0)
    	uid = dataTracer.idFrame + 1;
    t.trec_time = tracer[ndx].trec_time;
    t.trec_size = tracer[ndx].trec_size;
    t.trec_type = tracer[ndx].trec_type;
    t.trec_prio = tracer[ndx].trec_prio;
    t.trec_id   = tracer[ndx].trec_id;

    trace() << "getNextFrame " << t.trec_id;

    if (ndx++ == nrec) {
	ndx = 0;
        a_ = 1;
    }
}

void CritDirectionalSensor::processStreamData() {
	if ( trec_.trec_type == 'I' || trec_.trec_type == 'H')
		gopId++;
	if (fecType == 0){ //without FEC
		fullPkt = trec_.trec_size / max;
		restPkt = trec_.trec_size % max;
		int k(restPkt=0?fullPkt:fullPkt+1);
		trace() << "Send without FEC - Frame type " << trec_.trec_type << " number " << trec_.trec_id << " with size " << trec_.trec_size;
		if (fullPkt > 0) {
			for (i = 0; i < fullPkt; i++) {
				SendWithoutFEC(max, k);
				//trace() << "send pkt - id " << dataTracer.id << " size " << dataTracer.byteLength << "\n";
			}
		}
		if (restPkt != 0) {
			SendWithoutFEC(max, k);
			//trace() << "send pkt - id " << dataTracer.id << " size " << dataTracer.byteLength << "\n";
		}
	} else if (fecType == 1){ //standard FEC
		SendFrameWithFEC(trec_.trec_size / max, trec_.trec_size % max);
	} else if (fecType == 2){ //QoE-Aware FEC
		pos_frame++;
		if (pos_frame > gopSize)
			pos_frame = 1;
		fullPkt = trec_.trec_size / max;
		restPkt = trec_.trec_size % max;
		int k(restPkt=0?fullPkt:fullPkt+1);
		if ( trec_.trec_type == 'I' || trec_.trec_type == 'H' || (trec_.trec_type == 'P' && pos_frame <= (gopSize * pRedundancy))){
			SendFrameWithFEC(trec_.trec_size / max, trec_.trec_size % max);
		} else if ( trec_.trec_type == 'B' || (trec_.trec_type == 'P' && pos_frame > (gopSize * pRedundancy))){
			int k(restPkt=0?fullPkt:fullPkt+1);
			for(int i=0; i<k; i++)
				SendWithoutFEC(max+5, k);
		}
	}
	simtime_t interval = trec_.trec_time;       
	getNextFrame(ndx, trec_);	
	simtime_t time = trec_.trec_time - interval;

	if (a_ == 0) {
		scheduleAt(simTime() + time, processTrace);
	} else {
		cancelEvent(processTrace);
	}
}


void CritDirectionalSensor::SendWithoutFEC (int nbytes, int k){
	trace() << "Not using fec - Frame " << trec_.trec_id << " type " << trec_.trec_type << " number " << trec_.trec_id << " with size " << max;
	WiseSensorMessage *pkt = new WiseSensorMessage("VIDEO TRACES", MULTIMEDIA);    
	pkt->setByteLength(nbytes);
	dataTracer.idFrame = id++;
	dataTracer.byteLength = pkt->getByteLength();
	dataTracer.frameType = trec_.trec_type;
	dataTracer.k = k;
	dataTracer.gop = gopId;
	pkt->setIdFrame(dataTracer.idFrame);
	pkt->setInfo(dataTracer);
	pkt->setFrame(trec_.trec_id);
	pkt->setRelevance(getRelevance());
	double time = SIMTIME_DBL(simTime());
	//trace() << "send pkt - id " << dataTracer.id << " size " << dataTracer.byteLength << "\n";
	// creating sender tracer 
	sdTrace << time << setfill(' ') << setw(16)
            << " id " 	<< dataTracer.idFrame 	<< setfill(' ') << setw(16)
	    << " udp " 	<< pkt->getByteLength() << endl;
	send(pkt, "toApplicationModule"); //send the video tracer to the Application module
}

void CritDirectionalSensor::SendFrameWithFEC(unsigned int fullPkt, unsigned int restPkt) {
	// de pacotes cheios (fullPkt), caso contrario, preciso acicionar +1 pacote para armazenar o resto (restPkt)
	int k(restPkt=0?fullPkt:fullPkt+1);
	// k tem o numero de pacotes do frame e vou adicionar mais x% de redundancia
	int redundancy = k * fecRedundancy;
	// caso redundancy seja menor que 1, signal que x% não chegou a dar um pacote então coloco ao menos 1 de redundancia
	int n(k+(redundancy<1?1:redundancy));
	int sz(max);
	BuildFecBlockAndSend(k, n, sz);
	trace() << "Use fec - frame " << trec_.trec_id << " type " << trec_.trec_type << " with " << k << " original pkt and " << n << " redundant pkt";
	trace() << "frame posistion inside GOP " << pos_frame << "\n";
}

// precisei adicionar um block_id para o cliente conseguir diferenciar os blocos de fec
// antes era somente utilizado o id do frame do video, agora eu realizo soma com o id do bloco fec
// TODO: acho q deve ser melhor utilizar um numero aleatorio para evitar problemas ... verificar isto
void CritDirectionalSensor::BuildFecBlockAndSend(unsigned int _k, unsigned int _n, unsigned int _sz) {
	// build and send
	ReedSolomonFec neoFec;
	void *code = neoFec.create(_k,_n);
	
	// alocando memória
	u_char **framePkts = (u_char**)neoFec.safeMalloc(_k * sizeof(void *), "d_original ptr");
	for (unsigned int i = 0 ; i < _k ; i++ ) 
		framePkts[i] = (u_char*)neoFec.safeMalloc(_sz, "d_original data");
	// bulding sample data
	neoFec.BuildSampleData(framePkts, _k, _sz);
	
	unsigned int fec_header_size(5);
	// 5*sizeof(uint_t) eh o tamanho do header que vai ser adicionado 	
	// com as informacoes necessarias
	// todos os campos do header sao uint8_t
	// +-------------------------+-----+-----+-------+--------------------+----------------+
	// |  fec header size - fhs  |  k  |  n  |  idx  |  evalvid id - EID  |  Payload (sz)  |
	// +-------------------------+-----+-----+-------+--------------------+----------------+
	uint8_t *onePkt = (uint8_t*)neoFec.safeMalloc(_sz+fec_header_size * sizeof(uint8_t), "d_src data");

	int rand_id = 1 + rand()%255;
	
	// pog para nao repetir o ultimo numero aleatorio
	// soh o ultimo eh importante, se repetir depois nao tem problema
	// o que nao pode acontecer eh repetir em sequencia
	while ( rand_id == last_rand_id) {
		rand_id = 1 + rand()%255;
	}
	last_rand_id = rand_id;
	
	//NS_LOG_DEBUG ( "rand group id: " << rand_id );
	
	for (unsigned int i = 0 ; i < _n; i++ ) {
		neoFec.encode(code, (void**)framePkts, onePkt, i, _sz);
   		WiseSensorMessage *pkt = new WiseSensorMessage("VIDEO TRACES", MULTIMEDIA);
    		pkt->setByteLength(_sz+fec_header_size*sizeof(uint8_t));
		// criando header
		// primeiro "abro espaco" para o header no inico do buffer com o memmove
		// depois carrego as informacoes
		memmove( onePkt+fec_header_size, onePkt, _sz );
		onePkt[0] = fec_header_size;
		onePkt[1] = _k;
		onePkt[2] = _n;
		onePkt[3] = i;
		onePkt[4] = rand_id;
		pkt->setFecPktArraySize(_sz+fec_header_size * sizeof(uint8_t));
		trace() << "_k " << _k;
		trace() << "FecPktArraySize " << pkt->getFecPktArraySize();
		trace() << "frame id " << trec_.trec_id;
    		for (int i = 0; i<_sz+fec_header_size * sizeof(uint8_t); i++){
    			pkt->setFecPkt(i, onePkt[i]);
		}
		// enquanto o numero de pacotes enviados for menor que k (numero de pacotes "originais)
		// continuo enviando estas informacoes ao trace do evalvid. na hora que comeco a enviar pacotes
		// "a mais", ou seja, os gerados pelo FEC nao gero trace disto, somente envio para o client
		Send(pkt, (i<_k?true:false), _k);
	}
	
	neoFec.destroy((fec_parms*)code);
	if (onePkt != NULL){
		free(onePkt);
		onePkt = NULL;
	}
	if (framePkts != NULL) {
		for (unsigned int i = 0 ; i < _k ; i++ ) {
			free(framePkts[i]);
			framePkts[i] = NULL;
		}
		free(framePkts);
		framePkts = NULL ;
	}
}

// uso o parametro generate_trace para indicar se o pacote precisa ser adicionado
// ao trace do evalvid. eh necessario pois os pacotes "a mais" gerados pelo FEC
// não devem aparecer no trace que sera utilizado para remontar os frames posteriormente
// se nao for passado o parametro generate_trace assumo que deve ser gerado trace no evalvid
// OBS: por enquanto nao estou me preocupando com o tamanho dos pacotes pois o 
// evalvid nao usa esta informacao para remontar os frames
void CritDirectionalSensor::Send (WiseSensorMessage *pkt, bool generateTrace, int _k) {
	if (generateTrace){
		//trace() << "Add packet to Evalvid Tracer";
		dataTracer.idFrame = id++;
		double time = SIMTIME_DBL(simTime());
		// creating sender tracer 
    		sdTrace << time << setfill(' ') << setw(16)
            		<< " id " 	<< dataTracer.idFrame 	<< setfill(' ') << setw(16)
	    		<< " udp " 	<< pkt->getByteLength() << endl;
	} 

	dataTracer.byteLength = pkt->getByteLength();
	dataTracer.frameType = trec_.trec_type;
	dataTracer.k = _k;
	dataTracer.gop = gopId;
	pkt->setInfo(dataTracer);
	pkt->setIdFrame(uid);
	pkt->setFrame(trec_.trec_id);
	pkt->setRelevance(getRelevance());
	uid ++;
	//trace() << "send pkt - id " << dataTracer.id << " size " << dataTracer.byteLength;
	send(pkt, "toApplicationModule"); //send the video tracer to the Application module
}

int CritDirectionalSensor::getRelevance(){
	double Xmax = getParentModule()->getParentModule()->par("field_x");
	int relevance = 0;
	for (int k=0; k< areas.size(); k++){
		int intersections = 0;
		Segment target[4] = {
			Segment(areas[k].p1.x, areas[k].p1.y, areas[k].p2.x, areas[k].p2.y),
			Segment(areas[k].p2.x, areas[k].p2.y, areas[k].p3.x, areas[k].p3.y),
			Segment(areas[k].p3.x, areas[k].p3.y, areas[k].p4.x, areas[k].p4.y),
			Segment(areas[k].p4.x, areas[k].p4.y, areas[k].p1.x, areas[k].p1.y),
		};

		Segment sensorNode[1] = {
		    Segment(mobilityModule->getLocation().x, mobilityModule->getLocation().y, Xmax + 10, mobilityModule->getLocation().y),
		};

		for ( int i = 0; i < 4; i++ ) {
		     for ( int l = 0; l < 1; l++ ) {
			if ( target[i].intersects(sensorNode[l]) )
			    intersections++;
		    }
		}
		if (intersections % 2 != 0){
			relevance = areas[k].relevance;
			trace() << "Node is within the area with relevance = " << relevance;
			break;
		}
	}
	return relevance;
}

void CritDirectionalSensor::contLTrace(int cLin){
     nLin = cLin;
}

int CritDirectionalSensor::getContLTrace(){
    return nLin;
}

void CritDirectionalSensor::change_angle(){
    int intersections = 0;
    lineOfSight = 0;

    Segment intruderNode[8] = {
      Segment(intruder.x + fov_radius, intruder.y, intruder.x + fov_radius*0.75, intruder.y + fov_radius*0.75),
      Segment(intruder.x + fov_radius*0.75, intruder.y + fov_radius*0.75, intruder.x, intruder.y + fov_radius),
      Segment(intruder.x, intruder.y + fov_radius, intruder.x - fov_radius*0.75, intruder.y + fov_radius*0.75),
      Segment(intruder.x - fov_radius*0.75, intruder.y + fov_radius*0.75, intruder.x - fov_radius, intruder.y),
      Segment(intruder.x - fov_radius, intruder.y, intruder.x - fov_radius*0.75, intruder.y - fov_radius*0.75),
      Segment(intruder.x - fov_radius*0.75, intruder.y - fov_radius*0.75, intruder.x, intruder.y- fov_radius),
      Segment(intruder.x, intruder.y- fov_radius, intruder.x + fov_radius*0.75, intruder.y - fov_radius*0.75),
      Segment(intruder.x + fov_radius*0.75, intruder.y - fov_radius*0.75, intruder.x + fov_radius, intruder.y)
    };

    // Find intersections between the sensor area and the line of sight of the camera
    for (int i=0; i<32; i++) {
    	lineOfSight = lineOfSight + 0.25;
	temp_v.x = dov * cos(lineOfSight) + p.x;
	temp_v.y = dov * sin(lineOfSight) + p.y;

	Segment line[1] = {
          Segment(p.x, p.y, temp_v.x, temp_v.y)
	};

    	for ( int i = 0; i < 1; i++ ) {
            for ( int l = 0; l < 8; l++ ) {
		if ( line[i].intersects(intruderNode[l]) )
                    intersections++;
            }
        }

	if (intersections > 2)
	    break;
    }

    if (intersections > 2) {
        //remove the moving object and the sensor node fov
	terrain->clean_target();
	terrain->remove_camera(caminfo);

    	// find point v, b, c
	v.x = temp_v.x;
	v.y = temp_v.y;

    	double h = dov * sin(aov);
    	double d = dov * cos(aov);

    	b.x = v.x + h * (v.y - p.y) / d;
    	b.y = v.y - h * (v.x - p.x) / d;

    	c.x = v.x - h * (v.y - p.y) / d;
    	c.y = v.y + h * (v.x - p.x) / d;

    	// point g (center of mass)
    	g.x = (p.x + b.x + c.x) / 3;
    	g.y = (p.y + b.y + c.y) / 3;

    	// fill in the Triangle structure for geometric manipulation
    	myFoV = Triangle(p, b, c);

    	// draw node and camera FOV in opencv GUI
    	WiseCameraInfo::fov_triangle_t fov;
    	fov.a_x = myFoV.v1.x;
    	fov.a_y = myFoV.v1.y;
    	fov.b_x = myFoV.v2.x;
    	fov.b_y = myFoV.v2.y;
    	fov.c_x = myFoV.v3.x;
    	fov.c_y = myFoV.v3.y;

    	trace() << "Node " << self << " has to change the angle" << "\n";
    	//trace() << "Sensor - Node " << self << " received a vibration report  (" << intruder.x << "," << intruder.y << ")";

    	if(!isSink)
    	    caminfo.set_triangle_fov(fov);
    	caminfo.set_position(p.x, p.y, 0);
    	terrain->place_camera(caminfo);
    }
}

void CritDirectionalSensor::draw_node(){
    NodeLocation location = mobilityModule->getLocation();
    p.x = location.x;
    p.y = location.y;

    //alpha = PI / 10; 
    aov = par("aov"); // angle of view
    dov = par("dov"); // depth of view

    // find coordinate of point v of vector pv: set a direction for the camera
    // node that can rotate their camera will update their lineOfSight during the simulation
    lineOfSight = par("lineOfSight").longValue() == -1 ? uniform(0, 1) * 2 * PI : par("lineOfSight");

    // find point v, b, c for each sensor node
    v.x = dov * cos(lineOfSight) + p.x;
    v.y = dov * sin(lineOfSight) + p.y;

    double h = dov * sin(aov);
    double d = dov * cos(aov);

    b.x = v.x + h * (v.y - p.y) / d;
    b.y = v.y - h * (v.x - p.x) / d;

    c.x = v.x - h * (v.y - p.y) / d;
    c.y = v.y + h * (v.x - p.x) / d;

    // point g (center of mass)
    g.x = (p.x + b.x + c.x) / 3;
    g.y = (p.y + b.y + c.y) / 3;

    // fill in the Triangle structure for geometric manipulation
    myFoV = Triangle(p, b, c);

    // draw node and camera FOV in opencv GUI
    WiseCameraInfo::fov_triangle_t fov;
    fov.a_x = myFoV.v1.x;
    fov.a_y = myFoV.v1.y;
    fov.b_x = myFoV.v2.x;
    fov.b_y = myFoV.v2.y;
    fov.c_x = myFoV.v3.x;
    fov.c_y = myFoV.v3.y;

    if(!isSink){
    	caminfo.set_triangle_fov(fov);
    	caminfo.set_position(location.x, location.y, 0);
    	terrain->place_camera(caminfo);
    } else {
    	caminfo.set_position(location.x, location.y, 0);
    	terrain->place_BS(caminfo);
    }
}

/**
 * Output set into a std::string.
 *
 * Since std::sets are ordered, the returned string might be used for testing
 * set equality without iterating over all elements.
 */
std::string CritDirectionalSensor::keyFromSet(const std::set<unsigned int>& set) const {
    std::stringstream ss;
    for (std::set<unsigned int>::const_iterator it = set.begin(); it != set.end(); ++it) {
        ss << *it;
    }
    return ss.str();
}

/**
 *
 */
bool CritDirectionalSensor::covers(double x, double y) {
    return myFoV.contains(Point(x, y));
}
