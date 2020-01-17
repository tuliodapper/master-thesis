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

package wise_mnet.node;

import castalia.node.mobilityManager.iMobilityManager;

// The sensor node module. Connects to the wireless channel in order to communicate 
// with other nodes. Connects to psysical processes so it can sample them.

module Node
{
    parameters:
        double xCoor = default(0);
        double yCoor = default(0);
        double zCoor = default(0);
        double phi = default(0);
        double theta = default(0);
        bool dummy_communication = default(false); 	//chris's add-up: see usage of this
        string dummy_connectivity_map = default("");	// chris's add-up: should be used by dummy wireless channel

        string SensorManagerName = default("SensorManager"); // Chris's add-up
        string ApplicationName;	//the name of the implemented Application Module
        string MobilityManagerName = default("NoMobilityManager");	//the name of the implemented Mobility Module

        @display("bgb=521,383");
    gates:
        output toWirelessChannel;
        output toPhysicalProcess[];
        input fromWirelessChannel;
        input fromPhysicalProcess[];
        // chris' add-up: direct (ideal) communication among nodes 
        //                for simplistic assumptions (neighbours discover, locations)
        output toNodeDirect[];
        input fromNodeDirect[];

    submodules:
        MobilityManager: <MobilityManagerName> like iMobilityManager {

            @display("p=391,124");
        }
        ResourceManager: castalia.node.resourceManager.ResourceManager {
            @display("p=74,299");
        }

        SensorManager: <SensorManagerName> like wise_mnet.node.sensorManager.WiseBaseSensorManager {
            @display("p=74,124");
            gates:
                fromNodeContainerModule[sizeof(toPhysicalProcess)];
                toNodeContainerModule[sizeof(toPhysicalProcess)];
        }

        Communication: castalia.node.communication.CommunicationModule {
            @display("p=247,299");
        }

        //DummyCommunication: wise.node.communication.WiseDummyCommunicationModule;
        Application: <ApplicationName> like wise_mnet.node.application.WiseBaseApplication {
            @display("p=247,124");
            gates:
                toApplicationDirect[sizeof(toNodeDirect)];
                fromApplicationDirect[sizeof(fromNodeDirect)];
        }

    connections allowunconnected:
        Communication.toNodeContainerModule --> toWirelessChannel if (!dummy_communication);
        fromWirelessChannel --> Communication.fromNodeContainerModule if (!dummy_communication);
        Application.toCommunicationModule --> Communication.fromApplicationModule if (!dummy_communication);
        Application.toSensorDeviceManager --> SensorManager.fromApplicationModule;
        Communication.toApplicationModule --> Application.fromCommunicationModule if (!dummy_communication);
        SensorManager.toApplicationModule --> Application.fromSensorDeviceManager;

        Application.toCommunicationModule --> toWirelessChannel if (dummy_communication);
        fromWirelessChannel --> Application.fromCommunicationModule if (dummy_communication);

        for i=0..sizeof(fromNodeDirect) - 1 {
            fromNodeDirect[i] --> Application.fromApplicationDirect[i];
        }
        for i=0..sizeof(toNodeDirect) - 1 {
            toNodeDirect[i] <-- Application.toApplicationDirect[i];
        }

        for i=0..sizeof(toPhysicalProcess) - 1 {
            fromPhysicalProcess[i] --> SensorManager.fromNodeContainerModule[i];
            SensorManager.toNodeContainerModule[i] --> toPhysicalProcess[i];
        }

        ResourceManager.toSensorDevManager --> SensorManager.fromResourceManager;
        ResourceManager.toApplication --> Application.fromResourceManager;
        ResourceManager.toNetwork --> Communication.fromResourceManager2Net if (!dummy_communication);
        ResourceManager.toMac --> Communication.fromResourceManager2Mac if (!dummy_communication);
        ResourceManager.toRadio --> Communication.fromResourceManager2Radio if (!dummy_communication);
        Application.toMobilityManager --> MobilityManager.fromApplicationModule;
        MobilityManager.toApplicationModule --> Application.fromMobilityManager;
}

