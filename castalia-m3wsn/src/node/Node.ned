//********************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2010                             *
//*  Developed at the ATP lab, Networked Systems research theme                  *
//*  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis, Yuriy Tselishchev  *
//*  This file is distributed under the terms in the attached LICENSE file.      *
//*  If you do not find this file, copies can be found by writing to:            *
//*                                                                              *
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia                 *
//*      Attention:  License Inquiry.                                            *
//*                                                                              *
//*******************************************************************************/

package castalia.node;

module Node
{
    parameters:
        //node location is defined by five parameters below
        double xCoor = default(0);
        double yCoor = default(0);
        double zCoor = default(0);
        double phi = default(0);
        double theta = default(0);

        double startupOffset = default(0);							//node startup offset (i.e. delay), in seconds 
        double startupRandomization = default(0.05);				//node startup randomisation, in seconds
	// Node will become active startupOffset + random(startupRandomization) 
	// seconds after the start of simulation

        string ApplicationName;										//the name of the implemented Application Module
        string MobilityManagerName = default("NoMobilityManager");	//the name of the implemented Mobility Module

    gates:
        output toWirelessChannel;
        output toPhysicalProcess[];
        input fromWirelessChannel;
        input fromPhysicalProcess[];

    submodules:
        MobilityManager: <MobilityManagerName> like castalia.node.mobilityManager.iMobilityManager {
            @display("p=447,165");
        }
        ResourceManager: castalia.node.resourceManager.ResourceManager {
            @display("p=66,271");
        }
        SensorManager: castalia.node.sensorManager.SensorManager {
            @display("p=66,165");
            gates:
                fromNodeContainerModule[sizeof(toPhysicalProcess)];
                toNodeContainerModule[sizeof(toPhysicalProcess)];
        }
        Communication: castalia.node.communication.CommunicationModule {
            @display("p=261,271");
        }
        Application: <ApplicationName> like castalia.node.application.iApplication {
            @display("p=261,165");
        }
    connections:
        Communication.toNodeContainerModule --> toWirelessChannel;
        fromWirelessChannel --> Communication.fromNodeContainerModule;
        Application.toCommunicationModule --> Communication.fromApplicationModule;
        Application.toSensorDeviceManager --> SensorManager.fromApplicationModule;
        Application.toMobilityManager --> MobilityManager.fromApplicationModule;
        MobilityManager.toApplicationModule --> Application.fromMobilityManager;
        
        Communication.toApplicationModule --> Application.fromCommunicationModule;
        SensorManager.toApplicationModule --> Application.fromSensorDeviceManager;

        for i=0..sizeof(toPhysicalProcess) - 1 {
            fromPhysicalProcess[i] --> SensorManager.fromNodeContainerModule[i];
            SensorManager.toNodeContainerModule[i] --> toPhysicalProcess[i];
        }

        ResourceManager.toSensorDevManager --> SensorManager.fromResourceManager;
        ResourceManager.toApplication --> Application.fromResourceManager;
        ResourceManager.toNetwork --> Communication.fromResourceManager2Net;
        ResourceManager.toMac --> Communication.fromResourceManager2Mac;
        ResourceManager.toRadio --> Communication.fromResourceManager2Radio;
}

