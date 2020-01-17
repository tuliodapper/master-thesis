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
//*  	Module: Reed-solomon coding to QoE-aware FEC mechanism			*
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

#ifndef REED_SOLOMON_FEC_H
#define REED_SOLOMON_FEC_H

#include <sys/types.h>
#include <set>

class ReedSolomonFec {
private:

protected:


public:
	ReedSolomonFec();
	virtual ~ReedSolomonFec ();
	
	struct fec_parms* create(int k, int n);
	void destroy(struct fec_parms* code);
	void encode(void *code, void *src[], void *dst, int index, int sz);
	int decode(void *code, void *pkt[], int index[], int sz);
	void *safeMalloc(int sz, const char *s);
	void BuildSampleData(u_char **frame, int k, int sz);


};



#endif /* REED_SOLOMON_FEC_H */
