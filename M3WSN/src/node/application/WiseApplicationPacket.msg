// *****************************************************************************
//  Copyright (C): Christian Nastasi, 2011                                    
//  Author(s): Christian Nastasi                                              
//  Developed at the:                                                         
//  - Retis LAB, TeCIP Institute, Scuola Superiore Sant'Anna (Pisa)           
//  - School of Elec Eng and Computer Science, Queen Mary University (London) 
//  This file is distributed under the terms in the attached LICENSE_2 file.  
//  If you do not find this file, copies can be found by writing to:          
//  - c.nastasi@sssup.it                                                      
//  - nastasichr@gmail.com                                                    
//  - andrea.cavallaro@eecs.qmul.ac.uk                                        
// *****************************************************************************

cplusplus {{
        #include "ApplicationPacket_m.h"
        #include "Node.h"
}};

class ApplicationPacket;
struct Node;
struct TraceInfo;

enum WiseApplicationPacketType {
	WISE_APP_NORMAL = 0;
	WISE_APP_DIRECT_COMMUNICATION = 10;
};

packet WiseApplicationPacket extends ApplicationPacket {
	Node src;
	unsigned char payload[];
	unsigned int type = WISE_APP_NORMAL;
	double sensedValue;
	int angle;
	int fecPkt [];

	//Destino do nó
	int destinationX;
	int destinationY;
	
	double speed; //Velocidade
	double speed_final;
	double time_final;
	double time_final_final;
	double car_id; //Identificador do carro
	bool in_coligate; //Verificar se o veiculo está fazendo alguma coligação
	
	double uf; //Função de utilidade
	double beta_s; //Menor valor esperado para a uf
	double gama_s; //Maior valor esperado para a uf
	int destino_final;
	int src_final;
	bool head;
	int src_head;
	double XOrig;
	double YOrig;
}
