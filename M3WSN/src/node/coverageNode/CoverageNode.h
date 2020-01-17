//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef COVERAGENODE_H
#define COVERAGENODE_H

#include <omnetpp.h>

#include "CritSensor.h"

/*
 * Records network coverage.
 *
 * This module periodically generates random points
 * and records the percentage of points that are
 * covered by a sensor.
 */
class CoverageNode : public cSimpleModule {
public:
    CoverageNode();
    virtual ~CoverageNode();
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    virtual void finish();

private:
    cMessage* recordCoverage;
    std::vector<CritSensor*> sensors;
    int numNodes;
    double fieldX, fieldY;

    simsignal_t coverageSignal;
    double measurementInterval;
    double numSamples;
};

#endif /* COVERAGENODE_H_ */
