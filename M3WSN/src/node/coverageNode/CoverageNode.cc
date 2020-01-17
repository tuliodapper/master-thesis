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

#include "CoverageNode.h"

Define_Module(CoverageNode);

CoverageNode::CoverageNode() :
        recordCoverage(0), numNodes(0), fieldX(0), fieldY(0),
        coverageSignal(0), measurementInterval(0), numSamples(0) {

}

CoverageNode::~CoverageNode() {
}

/**
 *
 */
void CoverageNode::initialize() {
    numNodes = par("numNodes");
    fieldX = par("fieldX");
    fieldY = par("fieldY");

    numSamples = par("numSamples");
    measurementInterval = par("measurementInterval");

    recordCoverage = new cMessage("record coverage");
    scheduleAt(simTime() + measurementInterval, recordCoverage);

    coverageSignal = registerSignal("coverage");

    cModule* network = getParentModule();
    cModule* sensor = 0;

    // Find sensor modules in network
    for (int i = 0; i < numNodes; i++) {
        sensor = network->getSubmodule("node", i)->getSubmodule("SensorManager");
        if ( !sensor )
            continue;

        CritSensor* c = check_and_cast<CritSensor*>(sensor);
        sensors.push_back(c);
    }
}

/**
 *
 */
void CoverageNode::handleMessage(cMessage *msg) {
    if (msg == recordCoverage) {
        std::vector<CritSensor*>::iterator it = sensors.begin();

        double coveredPoints = 0;

        for (int i = 0; i <= numSamples; i++) {
            double x = uniform(0, fieldX);
            double y = uniform(0, fieldY);

            // iterate over all sensors in network
            for (it = sensors.begin(); it != sensors.end(); ++it) {
                if ( (*it)->covers(x, y) && !((*it)->isDisabled()) ) {
                    coveredPoints++;
                    break;
                }
            }
        }

        EV << "Coverage: " << (coveredPoints / numSamples) * 100 << "%" << std::endl;
        emit(coverageSignal, (coveredPoints / numSamples) * 100);

        scheduleAt(simTime() + measurementInterval, recordCoverage);
    } else {
        delete msg;
    }
}

/**
 *
 */
void CoverageNode::finish() {
    cancelAndDelete(recordCoverage);
}

