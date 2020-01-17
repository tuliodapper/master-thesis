/*******************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                            *
 *  Developed at the ATP lab, Networked Systems research theme                 *
 *  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis, Yuriy Tselishchev *
 *  This file is distributed under the terms in the attached LICENSE file.     *
 *  If you do not find this file, copies can be found by writing to:           *
 *                                                                             *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia                *
 *      Attention:  License Inquiry.                                           *
 *                                                                             *  
 *******************************************************************************/

#include "ResourceManager.h"

#include <cassert>

Define_Module(ResourceManager);

ResourceManager::ResourceManager() : energyMsg(0){
}

ResourceManager::~ResourceManager(){
    cancelAndDelete(energyMsg);
    energyMsg = 0;
}

void ResourceManager::initialize(){
	sigmaCPUClockDrift = par("sigmaCPUClockDrift");
	//using the "0" rng generator of the ResourceManager module
	cpuClockDrift = normal(0, sigmaCPUClockDrift);
	/* Crop any values beyond +/- 3 sigmas. Some protocols (e.g., MAC) rely on
	 * bounded cpuClockDrift. Although the bounds are conservative (usually 3sigmas),
	 * if you instantiate thousands of nodes (in multiple runs) we will get a
	 * couple of nodes that will be beyond this bound. Limiting/Croping the drift
	 * is actually realistic, since usually there is some kind of quality
	 * control on quartz crystals or the boards that use them (sensor node)
	 */
	if (cpuClockDrift > 3 * sigmaCPUClockDrift)
		cpuClockDrift = 3 * sigmaCPUClockDrift;
	if (cpuClockDrift < -3 * sigmaCPUClockDrift)
		cpuClockDrift = -3 * sigmaCPUClockDrift;

	initialEnergy = par("initialEnergy");
	ramSize = par("ramSize");
	baselineNodePower = par("baselineNodePower");
	periodicEnergyCalculationInterval = (double)par("periodicEnergyCalculationInterval") / 1000;

	if (baselineNodePower < 0 || periodicEnergyCalculationInterval < 0)
		opp_error("Illegal values for baselineNodePower and/or periodicEnergyCalculationInterval in resource manager module");

	currentNodePower = baselineNodePower;
	remainingEnergy = initialEnergy;
	totalRamData = 0;
	disabled = true;
	declareOutput("Remaing Energy");
}

void ResourceManager::calculateEnergySpent(){
	self = getParentModule()->getIndex();
	if (remainingEnergy > 0 && (self!=0)) {
		simtime_t timePassed = simTime() - timeOfLastCalculation;
		//EV << "energy consumed in the last " << timePassed <<
		//	"s is " <<(timePassed * currentNodePower);
		consumeEnergy(SIMTIME_DBL(timePassed * currentNodePower / 1000.0));
		trace() << "consume " << SIMTIME_DBL(timePassed * currentNodePower / 1000.0) << " (J)";
		timeOfLastCalculation = simTime();
		if ( energyMsg ){
		    cancelEvent(energyMsg);
		    scheduleAt(simTime() + periodicEnergyCalculationInterval, energyMsg);
		}
	} 
}

/* The ResourceManager module has only one "unconnected" port where it can receive messages that
 * update the power drawn by a module, or a NODE_STARTUP message. If disabled we still process
 * messages because we want to have the latest power drawn from any module.
 */
void ResourceManager::handleMessage(cMessage * msg) {
	switch (msg->getKind()) {
		case NODE_STARTUP:{
			disabled = false;
			assert(remainingEnergy != 0);
			timeOfLastCalculation = simTime();
			energyMsg = new cMessage("Periodic energy calculation", TIMER_SERVICE);
        		scheduleAt(simTime() + periodicEnergyCalculationInterval, energyMsg);
			break;
		}
	
		case TIMER_SERVICE:{
			calculateEnergySpent();
			getRemainingEnergy();
			return;
		}

		case RESOURCE_MANAGER_DRAW_POWER:{
			ResourceManagerMessage *resMsg = check_and_cast<ResourceManagerMessage*>(msg);
			int id = resMsg->getSenderModuleId();
			double oldPower = storedPowerConsumptions[id];
			EV << "New power consumption, id = " << id << ", oldPower = " <<
					currentNodePower << ", newPower = " << 
					currentNodePower - oldPower + resMsg->getPowerConsumed();

			if ( !disabled ) {
			    trace() << "call calculateEnergySpent";
			    //consumeEnergy(resMsg->getPowerConsumed());
			    calculateEnergySpent();
			}
			currentNodePower = currentNodePower - oldPower + resMsg->getPowerConsumed();
			trace() << "powerConsumed() " << resMsg->getPowerConsumed();
			trace() << "currentNodePower " << currentNodePower;
			storedPowerConsumptions[id] = resMsg->getPowerConsumed();
			break;
		}

		default:
			throw cRuntimeError("ERROR: Unexpected message received by resource manager: %s", msg->getKind());
	}
	delete msg;
}

void ResourceManager::finishSpecific(){
	calculateEnergySpent();
	declareOutput("Consumed Energy");

	self = getParentModule()->getIndex();
	if (self != 0){
		collectOutput("Remaing Energy", "", getRemainingEnergy());
		collectOutput("Consumed Energy", "", getSpentEnergy());
	}
}

double ResourceManager::getSpentEnergy(){
	Enter_Method("getSpentEnergy()");
	return (initialEnergy - remainingEnergy);
}

double ResourceManager::getRemainingEnergy(void){
	Enter_Method("getRemainingEnergy()");	
	return (remainingEnergy);
}

// added by Pedro
// criada para setar um valor arbitrário para quantidade energia na bateria
// possivelmente será usado para solução de energy harvesting
void ResourceManager::setRemainingEnergy(double newEnergy){
	Enter_Method("setRemainingEnergy(double newEnergy)");
	remainingEnergy = newEnergy;
//	return (remainingEnergy);
}

// criada para resetar a quantidade de energia para a total inicial
void ResourceManager::resetRemainingEnergy(){
	Enter_Method("resetRemainingEnergy()");
	output() << "WELLFINAL0 - remainingEnergy: " << remainingEnergy;
	remainingEnergy = initialEnergy;
	output() << "WELLFINAL1 - remainingEnergy: " << remainingEnergy;
//	return (remainingEnergy);
}
// END added by Pedro

double ResourceManager::getInitialEnergy(void){
	Enter_Method("getInitialEnergy()");	
	return (initialEnergy);
}

double ResourceManager::getCPUClockDrift(){
	Enter_Method("getCPUClockDrift()");
	return (1.0f + cpuClockDrift);
}

void ResourceManager::consumeEnergy(double amount){
	Enter_Method("consumeEnergy(double amount)");
	trace() << "consumed Energy = " << amount;

	if (remainingEnergy <= amount) {
		remainingEnergy = 0;
		destroyNode();
	} else
		remainingEnergy -= amount;
}

void ResourceManager::destroyNode(){
	disabled = true;
	send(new cMessage("Destroy node message", DESTROY_NODE), "toSensorDevManager");
	send(new cMessage("Destroy node message", DESTROY_NODE), "toApplication");
	send(new cMessage("Destroy node message", DESTROY_NODE), "toNetwork");
	send(new cMessage("Destroy node message", DESTROY_NODE), "toMac");
	send(new cMessage("Destroy node message", DESTROY_NODE), "toRadio");
}

int ResourceManager::RamStore(int numBytes){
	Enter_Method("RamStore(int numBytes)");

	int ramHasSpace = ((totalRamData + numBytes) <= ramSize) ? 1 : 0;
	if (!ramHasSpace) {
		trace() << "\n[Resource Manager] t= " << simTime() <<
				": WARNING: Data not stored to Ram. Not enough space to store them.";
		return 0;
	} else
		totalRamData += numBytes;
	return 1;
}

void ResourceManager::RamFree(int numBytes){
	Enter_Method("RamFree(int numBytes)");
	totalRamData -= numBytes;
	totalRamData = (totalRamData < 0) ? 0 : totalRamData;
}

