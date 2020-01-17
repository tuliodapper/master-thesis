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
//*  	Module: Sensor application to camera node with node mobility		*
//*										*
//*  	Version: 1.0								*
//*  	Authors: Denis do Rosário <denis@ufpa.br>				*
//*										*
//******************************************************************************/ 

#ifndef CRITICALITYVIDEOSENSOR_H
#define CRITICALITYVIDEOSENSOR_H
#include <set>
#include "CritSensor.h"

#define MAX_COVERSET_TABLESIZE 10

class MobileDirectionalSensor: public CritSensor {
public:
    MobileDirectionalSensor();
    virtual ~MobileDirectionalSensor();

    virtual bool covers(double x, double y);
    Triangle getFov() { return myFoV; };
    virtual bool getIsSink(){return isSink;};

protected:
    void initialize();
    void finish();
    void draw_node();
    void change_angle();
    void update_location();
    bool isSink;
    
    cMessage* setupMsg;//, *processTrace;

private:
    virtual void computeCoverSets();
    virtual void handleSample(cMessage* msg);
    virtual void handleSensorMessage(cMessage *);

    std::set<unsigned int> computeIntersection(const std::set<unsigned int>& A,
            const std::set<unsigned int>& B) const;

    std::string keyFromSet(const std::set<unsigned int>& set) const;


    // physical position properties
    ///////////////////////////////
    Point p, v, temp_v, b, c, g, gp, gv, gb, gc, intruder, newLocation; // points that define the sensor's FoV
    double lineOfSight; // line of sight

    // coverage properties
    ///////////////////////////////
    double dov; // depth of view
    double aov; // angle of view
    Triangle myFoV;

    bool changeAngle;

    std::set<unsigned int> activeNeighbors;
    std::set<std::set<unsigned int> > availableCoverSets; // only stores the set the node's ID
    std::map<std::set<unsigned int>, double> coveragePerCoverSet;
    int activeCoverset;
};

#endif /* CRITICALITYSENSOR_H_ */
