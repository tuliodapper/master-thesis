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
//*  	Module: Sensor application to detect intruder				*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/ 

#ifndef INTRUDERSENSOR_H
#define INTRUDERSENSOR_H
#include <set>
#include "CritSensor.h"
#define MAX_COVERSET_TABLESIZE 10

class IntruderSensor: public CritSensor {
public:
    IntruderSensor();
    virtual ~IntruderSensor();

    virtual bool covers(double x, double y);
    Triangle getFov() { return myFoV; };

protected:
    void initialize();
    void finish();
    void draw_node();

private:
    virtual void computeCoverSets();
    virtual void handleSample(cMessage* msg);
    virtual void handleSensorMessage(cMessage *);

    std::set<unsigned int> computeIntersection(const std::set<unsigned int>& A,
            const std::set<unsigned int>& B) const;

    std::string keyFromSet(const std::set<unsigned int>& set) const;

    // physical position properties
    ///////////////////////////////
    Point p;

    // coverage properties
    ///////////////////////////////
    Triangle myFoV;
};

#endif /* CRITICALITYSENSOR_H_ */