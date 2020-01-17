package wise_mnet;
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

import castalia.wirelessChannel.WirelessChannel;
import wise_mnet.node.coverageNode.CoverageNode;

network SN
{
    parameters:
        int field_x = default(30);			// the length of the deployment field
        int field_y = default(30);			// the width of the deployment field
        int field_z = default(0);			// the height of the deployment field (2-D field by default)
        bool dummy_communication = default(false);

        int numNodes;						// the number of nodes

        string deployment = default("");

        int numPhysicalProcesses = default(1);
        string physicalProcessName = default("CustomizablePhysicalProcess");
        string TerrainName = default("WiseEmptyTerrain"); // Chris' add-up
        string debugInfoFileName = default("M3WSN-Debug.txt");
	string outputInfoFileName = default("M3WSN-Output.txt");
	string resultInfoFileName = default("M3WSN-Result.txt");

    submodules:
        coverageNode: CoverageNode {

        }

        wirelessChannel: WirelessChannel {
            gates:
                toNode[numNodes];
                fromNode[numNodes];
        }
        dummyChannel: wise_mnet.wirelessChannel.WiseDummyWirelessChannel {
            gates:
                toNode[numNodes];
                fromNode[numNodes];
        }

        wiseTerrain: <TerrainName> like wise_mnet.world.terrain.WiseBaseTerrain;
        physicalProcess[numPhysicalProcesses]: <physicalProcessName> like castalia.physicalProcess.iPhysicalProcess {
            gates:
                toNode[numNodes];
                fromNode[numNodes];
        }

        node[numNodes]: wise_mnet.node.Node {
            dummy_communication = dummy_communication;
            gates:
                toPhysicalProcess[numPhysicalProcesses];
                fromPhysicalProcess[numPhysicalProcesses];

                toNodeDirect[numNodes];
                fromNodeDirect[numNodes];
        }

    connections allowunconnected:
        for i=0..numNodes - 1 {
            node[i].toWirelessChannel --> wirelessChannel.fromNode[i] if (!dummy_communication);
            node[i].fromWirelessChannel <-- wirelessChannel.toNode[i] if (!dummy_communication);
        }
        for i=0..numNodes - 1 {
            node[i].toWirelessChannel --> dummyChannel.fromNode[i] if (dummy_communication);
            node[i].fromWirelessChannel <-- dummyChannel.toNode[i] if (dummy_communication);
        }

        for i=0..numNodes - 1, for j=0..numPhysicalProcesses - 1 {
            node[i].toPhysicalProcess[j] --> physicalProcess[j].fromNode[i];
            node[i].fromPhysicalProcess[j] <-- physicalProcess[j].toNode[i];
        }

        for i=0..numNodes - 1, for j=0..numNodes - 1 {
            node[i].toNodeDirect[j] --> node[j].fromNodeDirect[i] if (i != j);
        }
}
