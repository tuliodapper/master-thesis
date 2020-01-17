package castalia;
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

import castalia.physicalProcess.iPhysicalProcess;


simple coordNode
{
    parameters:
        @class(Node);
        double fieldMaxX;
        double fieldMaxY;
        double nodesNb;
}

simple intrusionModule
{
    parameters:
        double activatedAt;
}

network SN
{
    parameters:
        int field_x = default(75);			// the length of the deployment field
        int field_y = default(75);			// the width of the deployment field
        int field_z = default(0);			// the height of the deployment field (2-D field by default)

        int numNodes;						// the number of nodes

        string deployment = default("");

        int numPhysicalProcesses = default(1);
        string physicalProcessName = default("CustomizablePhysicalProcess");
        string debugInfoFileName = default("M3WSN-Debug.txt");
	string outputInfoFileName = default("M3WSN-Output.txt");
	string resultInfoFileName = default("M3WSN-Result.txt");

    submodules:
        wirelessChannel: castalia.wirelessChannel.WirelessChannel {
            parameters:
                @display("i=device/antennatower_l,black;p=0,0");
            gates:
                toNode[numNodes];
                fromNode[numNodes];
        }

        physicalProcess[numPhysicalProcesses]: <physicalProcessName> like iPhysicalProcess {
            parameters:
                @display("i=misc/globe_l,blue;p=750,0");
            gates:
                toNode[numNodes];
                fromNode[numNodes];
        }

        node[numNodes]: castalia.node.Node {
            parameters:
                @display("i=block/circle_vs,cyan;");
            gates:
                toPhysicalProcess[numPhysicalProcesses];
                fromPhysicalProcess[numPhysicalProcesses];
        }

    connections:
        for i=0..numNodes - 1 {
            node[i].toWirelessChannel --> { @display("ls=black,0"); } --> wirelessChannel.fromNode[i];
            node[i].fromWirelessChannel <-- { @display("ls=black,0"); } <-- wirelessChannel.toNode[i];
        }

        for i=0..numNodes - 1, for j=0..numPhysicalProcesses - 1 {
            node[i].toPhysicalProcess[j] --> { @display("ls=black,0"); } --> physicalProcess[j].fromNode[i];
            node[i].fromPhysicalProcess[j] <-- { @display("ls=black,0"); } <-- physicalProcess[j].toNode[i];
        }
}
