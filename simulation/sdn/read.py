 # -*- coding: utf-8 -*-
import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/../../application/coordination/")
import pickle
import utils

outfile = "M3WSN-Output.txt"
cfgfile	= "sim-config.pickle"

TIME_PERIOD = 0.5

PACKETS = {}

PACKETS["SDN"] = {}
PACKETS["SDN"]["CONTROL"] 	= ["SDN_PACKET_IN", "SDN_TELEMETRY", "SDN_UPDATE_ROUTING_TABLE","SDN_ACK","SDN_HELLO","SDN_REGISTER","SDN_CONFIRMATION_REGISTRY"]
PACKETS["SDN"]["DATA"] 		= ["SDN_DATA"]
 
PACKETS["AODV"] = {}
PACKETS["AODV"]["CONTROL"] 	= ["AODV_RREQ_PACKET","AODV_RREP_PACKET","AODV_UNKNOWN_PACKET_TYPE","AODV_RERR_PACKET","AODV_RREP_ACK_PACKET","AODV_HELLO_MESSAGE_PACKET"]
PACKETS["AODV"]["DATA"] 	= ["AODV_DATA_PACKET"]

CONST_TIME 	= 0
CONST_NODE 	= 1
CONST_KIND 	= 2
CONST_KIND_PACKET 		= "pkt"
CONST_KIND_LOCATION 	= "loc"
CONST_KIND_DROP 		= "drp"
CONST_KIND_TELEMETRY 	= "tel"
CONST_KIND_DROP_NETWORK = "net"
CONST_KIND_DROP_MAC 	= "mac"

CONST_PACKET_ACTION 			= 3
CONST_PACKET_ACTION_SENT 		= "SENT"
CONST_PACKET_ACTION_RECEIVED 	= "RECV"
CONST_PACKET_ACTION_RECEIVED_TO_FOWARD = "RCFW"
CONST_PACKET_ACTION_FOWARD 		= "FWRD"
CONST_PACKET_KIND 				= 4
CONST_PACKET_TYPE 				= 5
CONST_PACKET_ID_TRANSMISSION	= 6
CONST_PACKET_IDENTIFIER 		= 7
CONST_PACKET_SOURCE 			= 8
CONST_PACKET_NEXT_HOP 			= 9
CONST_PACKET_DESTINATION 		= 10
CONST_PACKET_TIME_TO_LIVE 		= 11
CONST_PACKET_TRACE 				= 12

CONST_LOCATION_X 				= 3
CONST_LOCATION_Y 				= 4

CONST_DROP_LAYER 				= 3

CONST_TELEMETRY_CN_SOURCE 			= 3
CONST_TELEMETRY_CN_DESTINATION 		= 4
CONST_TELEMETRY_CN_MESSAGE_TYPE 	= 5
CONST_TELEMETRY_CN_SIZE 			= 6

CONST_TELEMETRY_RN_LEN_QUEUE_MAC 	= 3
CONST_TELEMETRY_RN_LEN_QUEUE_NET 	= 4
CONST_TELEMETRY_RN_PACKET_DROP_MAC 	= 5
CONST_TELEMETRY_RN_PACKET_DROP_NET 	= 6

CONST_TELEMETRYBYHOPY_CN_RELAYID 			= 1
CONST_TELEMETRYBYHOPY_CN_TIME_RECEIVED 		= 2
CONST_TELEMETRYBYHOPY_CN_LEN_QUEUE_MAC 		= 3
CONST_TELEMETRYBYHOPY_CN_LEN_QUEUE_NET 		= 4
CONST_TELEMETRYBYHOPY_CN_PACKET_DROP_MAC 	= 5
CONST_TELEMETRYBYHOPY_CN_PACKET_DROP_NET 	= 6
CONST_TELEMETRYBYHOPY_CN_RSSI 				= 7

CONST_TRACE_SOURCE 				= 0
CONST_TRACE_IDENTIFIED 			= 1
CONST_TRACE_DESTINATION 		= 2

PACKET_ACTION = [CONST_PACKET_ACTION_SENT, CONST_PACKET_ACTION_RECEIVED, CONST_PACKET_ACTION_RECEIVED_TO_FOWARD, CONST_PACKET_ACTION_FOWARD]

def Read(simpath):

	if os.path.isfile(simpath+outfile):
		with open(simpath+outfile, "r") as f:
			outlines = f.read().splitlines()
			f.close()
	else:
		return None

	if os.path.isfile(simpath+cfgfile):
		with open(simpath+cfgfile, "rb") as f:
			config = pickle.load(f)
	else:
		return None

	result = {}

	name 				= config["name"]
	result["name"] 		= name
	result["nUN"] 		= config["nUN"]
	result["byUserId"] 	= {}

	PACKETS_KIND = PACKETS[name]["DATA"] + PACKETS[name]["CONTROL"]

	result["timeSeries"] = {}
	result["timeSeries"]["time"] = []
	result["timeSeries"]["numberOfPacketsByPacket"] = {}
	result["timeSeries"]["numberOfPacketsByActionAndPacket"] = {}
	for packet in PACKETS_KIND:
		result["timeSeries"]["numberOfPacketsByPacket"][packet] = []
	for action in PACKET_ACTION:
		result["timeSeries"]["numberOfPacketsByActionAndPacket"][action] = {}
		for packet in PACKETS_KIND:
			result["timeSeries"]["numberOfPacketsByActionAndPacket"][action][packet] = []
	result["timeSeries"]["numberOfPackets"] = []
	result["timeSeries"]["numberOfDataPackets"] = []
	result["timeSeries"]["numberOfControlPackets"] = []
	result["timeSeries"]["numberOfSentPackets"] = []
	result["timeSeries"]["numberOfReceivedPackets"] = []
	result["timeSeries"]["numberOfDrops"] = []
	result["timeSeries"]["numberOfDropsMac"] = []
	result["timeSeries"]["numberOfDropsNetwork"] = []

	result["latencyByHops"] = {}

	result["byRN"] 	= {}

	result["byCN"] 	= {}
	result["byCN"]["byRN"] 	= {}

	max_time = 0

	for line in outlines:

		if len(line) > 0:

			line = line.split(";")
			time = float(line[CONST_TIME])
			node = int(line[CONST_NODE])
			kind = line[CONST_KIND]

			while (time > max_time):
				max_time = max_time + TIME_PERIOD
				result["timeSeries"]["time"].append(max_time)
				for packet in PACKETS_KIND:
					result["timeSeries"]["numberOfPacketsByPacket"][packet].append(0)
				for action in PACKET_ACTION:
					for packet in PACKETS_KIND:
						result["timeSeries"]["numberOfPacketsByActionAndPacket"][action][packet].append(0)
				result["timeSeries"]["numberOfPackets"].append(0)
				result["timeSeries"]["numberOfDataPackets"].append(0)
				result["timeSeries"]["numberOfControlPackets"].append(0)
				result["timeSeries"]["numberOfSentPackets"].append(0)
				result["timeSeries"]["numberOfReceivedPackets"].append(0)
				result["timeSeries"]["numberOfDrops"].append(0)
				result["timeSeries"]["numberOfDropsMac"].append(0)
				result["timeSeries"]["numberOfDropsNetwork"].append(0)

			if (kind == CONST_KIND_PACKET):

				packet_action 			= line[CONST_PACKET_ACTION]
				packet_kind 			= line[CONST_PACKET_KIND]
				packet_type 			= int(line[CONST_PACKET_TYPE])
				packet_id_transmission 	= int(line[CONST_PACKET_ID_TRANSMISSION])
				packet_identifier 		= int(line[CONST_PACKET_IDENTIFIER])
				packet_source 			= int(line[CONST_PACKET_SOURCE])
				packet_next_hop 		= int(line[CONST_PACKET_NEXT_HOP])
				packet_destination 		= int(line[CONST_PACKET_DESTINATION])
				packet_time_to_live 	= int(line[CONST_PACKET_TIME_TO_LIVE])
				packet_trace 			= line[CONST_PACKET_TRACE]

				if packet_kind in PACKETS[name]["DATA"]:

					if (str(node) in config["namesUN"]):

						if (packet_action == CONST_PACKET_ACTION_SENT):

							result["timeSeries"]["numberOfSentPackets"][-1] = result["timeSeries"]["numberOfSentPackets"][-1] + 1

					 		if not node in result["byUserId"]:
					 			result["byUserId"][node] = {}
			 					result["byUserId"][node]["transmissions"] = {}
					 		if not packet_id_transmission in result["byUserId"][node]["transmissions"]:
			 					result["byUserId"][node]["transmissions"][packet_id_transmission] = {}
				 				result["byUserId"][node]["transmissions"][packet_id_transmission]["packet_type"] = packet_type
				 				result["byUserId"][node]["transmissions"][packet_id_transmission]["packet_destination"] = packet_destination
				 				result["byUserId"][node]["transmissions"][packet_id_transmission]["packet_identifier"] = []
					 		if not packet_destination in result["byUserId"][node]:
					 			result["byUserId"][node][packet_destination] = {}

			 				#result["byUserId"][node]["transmissions"][packet_id_transmission]["packetloss"]["sentPackets"][-1] = result["byUserId"][node]["transmissions"][packet_id_transmission]["packetloss"]["sentPackets"][-1] + 1
			 				#result["byUserId"][node]["transmissions"][packet_id_transmission]["packetloss"]["indexTime"] = len(result["byUserId"][node]["transmissions"][packet_id_transmission]["packetloss"]["time"])-1
			 				result["byUserId"][node]["transmissions"][packet_id_transmission]["packet_identifier"].append(packet_identifier)

					 		result["byUserId"][node][packet_destination][packet_identifier] = {}
							result["byUserId"][node][packet_destination][packet_identifier]["sentTime"] 		= time
							result["byUserId"][node][packet_destination][packet_identifier]["indexTimeSeries"] 	= len(result["timeSeries"]["time"])-1
					 		result["byUserId"][node][packet_destination][packet_identifier]["hasReceived"] 		= False
					 		result["byUserId"][node][packet_destination][packet_identifier]["receivedTime"] 	= []
							
						elif (packet_action == CONST_PACKET_ACTION_RECEIVED):

					 		if not result["byUserId"][packet_source][node][packet_identifier]["hasReceived"]:
								receivedTime = time
								result["byUserId"][packet_source][node][packet_identifier]["receivedTime"].append(receivedTime)
					 			result["byUserId"][packet_source][node][packet_identifier]["hasReceived"] = True
								index = result["byUserId"][packet_source][node][packet_identifier]["indexTimeSeries"]
								result["timeSeries"]["numberOfReceivedPackets"][index] = result["timeSeries"]["numberOfReceivedPackets"][index] + 1
								nHops = len(result["byUserId"][packet_source][node][packet_identifier]["receivedTime"])
								if time > 50:
									if not nHops in result["latencyByHops"]:
										result["latencyByHops"][nHops] = []
									sentTime = result["byUserId"][packet_source][node][packet_identifier]["sentTime"]
									result["latencyByHops"][nHops].append(receivedTime-sentTime)

								# indexTime = result["byUserId"][packet_source]["transmissions"][packet_id_transmission]["indexTime"]
			 					# result["byUserId"][packet_source]["transmissions"][packet_id_transmission]["packetloss"]["receivedPackets"][indexTime] = result["byUserId"][packet_source]["transmissions"][packet_id_transmission]["packetloss"]["receivedPackets"][indexTime] + 1
			 					#result["byUserId"][packet_source]["transmissions"][packet_id_transmission]["latency"]["time"].append(time)
			 					#result["byUserId"][packet_source]["transmissions"][packet_id_transmission]["latency"]["value"].append(receivedTime-sentTime)
					
					elif (packet_action == CONST_PACKET_ACTION_RECEIVED_TO_FOWARD):
						result["byUserId"][packet_source][packet_destination][packet_identifier]["receivedTime"].append(time)

				if (packet_action == CONST_PACKET_ACTION_SENT) or (packet_action == CONST_PACKET_ACTION_FOWARD):

					result["timeSeries"]["numberOfPacketsByPacket"][packet_kind][-1] = result["timeSeries"]["numberOfPacketsByPacket"][packet_kind][-1] + 1
					result["timeSeries"]["numberOfPacketsByActionAndPacket"][packet_action][packet_kind][-1] = result["timeSeries"]["numberOfPacketsByActionAndPacket"][packet_action][packet_kind][-1] + 1

					if packet_trace != "":

						result["timeSeries"]["numberOfPackets"][-1] = result["timeSeries"]["numberOfPackets"][-1] + 1
						
						if packet_kind in PACKETS[name]["DATA"]:
							result["timeSeries"]["numberOfDataPackets"][-1] = result["timeSeries"]["numberOfDataPackets"][-1] + 1
						elif packet_kind in PACKETS[name]["CONTROL"]:
							result["timeSeries"]["numberOfControlPackets"][-1] = result["timeSeries"]["numberOfControlPackets"][-1] + 1

			elif (kind == CONST_KIND_DROP):
				drop_layer = line[CONST_DROP_LAYER]
				result["timeSeries"]["numberOfDrops"][-1] = result["timeSeries"]["numberOfDrops"][-1] + 1
				if drop_layer == CONST_KIND_DROP_NETWORK:
					result["timeSeries"]["numberOfDropsNetwork"][-1] = result["timeSeries"]["numberOfDropsNetwork"][-1] + 1
				elif drop_layer == CONST_KIND_DROP_MAC:
					result["timeSeries"]["numberOfDropsMac"][-1] = result["timeSeries"]["numberOfDropsMac"][-1] + 1

			elif (kind == CONST_KIND_TELEMETRY):
				
				if (str(node) == config["nameCN"]):

					index_byhop = utils.findIndices(line, lambda e: e == "byhop")

					for index in index_byhop:

						relayId 		= int(line[index 	+ CONST_TELEMETRYBYHOPY_CN_RELAYID])
						timeReceived 	= float(line[index 	+ CONST_TELEMETRYBYHOPY_CN_TIME_RECEIVED])
						lenQueueMac 	= int(line[index 	+ CONST_TELEMETRYBYHOPY_CN_LEN_QUEUE_MAC])
						lenQueueNet 	= int(line[index 	+ CONST_TELEMETRYBYHOPY_CN_LEN_QUEUE_NET])
						packetDropMac 	= int(line[index 	+ CONST_TELEMETRYBYHOPY_CN_PACKET_DROP_MAC])
						packetDropNet 	= int(line[index 	+ CONST_TELEMETRYBYHOPY_CN_PACKET_DROP_NET])
						rssi 			= float(line[index 	+ CONST_TELEMETRYBYHOPY_CN_RSSI]) 

				 		if not relayId in result["byCN"]["byRN"]:
				 			result["byCN"]["byRN"][relayId] = {}
				 			result["byCN"]["byRN"][relayId]["telemetry"] = {}
				 			result["byCN"]["byRN"][relayId]["telemetry"]["timeSeries"] 		= []
				 			result["byCN"]["byRN"][relayId]["telemetry"]["lenQueueMac"] 	= []
				 			result["byCN"]["byRN"][relayId]["telemetry"]["lenQueueNet"] 	= []
				 			result["byCN"]["byRN"][relayId]["telemetry"]["packetDropMac"] 	= []
				 			result["byCN"]["byRN"][relayId]["telemetry"]["packetDropNet"] 	= []

			 			result["byCN"]["byRN"][relayId]["telemetry"]["timeSeries"].append(timeReceived);
			 			result["byCN"]["byRN"][relayId]["telemetry"]["lenQueueMac"].append(lenQueueMac);
			 			result["byCN"]["byRN"][relayId]["telemetry"]["lenQueueNet"].append(lenQueueNet);
			 			result["byCN"]["byRN"][relayId]["telemetry"]["packetDropMac"].append(packetDropMac);
			 			result["byCN"]["byRN"][relayId]["telemetry"]["packetDropNet"].append(packetDropNet);


				elif (str(node) in config["namesRN"]):

					lenQueueMac 	= int(line[CONST_TELEMETRY_RN_LEN_QUEUE_MAC])
					lenQueueNet 	= int(line[CONST_TELEMETRY_RN_LEN_QUEUE_NET])
					packetDropMac 	= int(line[CONST_TELEMETRY_RN_PACKET_DROP_MAC])
					packetDropNet 	= int(line[CONST_TELEMETRY_RN_PACKET_DROP_NET])

			 		if not node in result["byRN"]:
			 			result["byRN"][node] = {}
			 			result["byRN"][node]["telemetry"] = {}
			 			result["byRN"][node]["telemetry"]["timeSeries"] 	= []
			 			result["byRN"][node]["telemetry"]["lenQueueMac"] 	= []
			 			result["byRN"][node]["telemetry"]["lenQueueNet"] 	= []
			 			result["byRN"][node]["telemetry"]["packetDropMac"] 	= []
			 			result["byRN"][node]["telemetry"]["packetDropNet"] 	= []

		 			result["byRN"][node]["telemetry"]["timeSeries"].append(time)
		 			result["byRN"][node]["telemetry"]["lenQueueMac"].append(lenQueueMac)
		 			result["byRN"][node]["telemetry"]["lenQueueNet"].append(lenQueueNet)
		 			result["byRN"][node]["telemetry"]["packetDropMac"].append(packetDropMac)
		 			result["byRN"][node]["telemetry"]["packetDropNet"].append(packetDropNet)

	for userId in result["byUserId"]:
		for id_transmission in result["byUserId"][userId]["transmissions"]:
			print(id_transmission)

			result["byUserId"][userId]["transmissions"][id_transmission]["latency"] = {}
			result["byUserId"][userId]["transmissions"][id_transmission]["latency"]["time"] = []
			result["byUserId"][userId]["transmissions"][id_transmission]["latency"]["value"] = []

			result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"] = {}
			result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["time"] = []
			result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["sentPackets"] = []
			result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["receivedPackets"] = []

			packet_destination = result["byUserId"][userId]["transmissions"][id_transmission]["packet_destination"]

			max_time = 0
			for packet_identifier in result["byUserId"][userId]["transmissions"][id_transmission]["packet_identifier"]:

				sentTime = result["byUserId"][userId][packet_destination][packet_identifier]["sentTime"]

				while (sentTime > max_time):
					max_time = max_time + TIME_PERIOD
					result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["time"].append(max_time)
					result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["sentPackets"].append(0)
					result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["receivedPackets"].append(0)

				result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["sentPackets"][-1] = result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["sentPackets"][-1]+1
				if result["byUserId"][userId][packet_destination][packet_identifier]["hasReceived"]:
					print("hasReceived: " + str(packet_identifier))
					result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["receivedPackets"][-1] = result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["receivedPackets"][-1]+1
					receivedTime = result["byUserId"][userId][packet_destination][packet_identifier]["receivedTime"][-1]
					result["byUserId"][userId]["transmissions"][id_transmission]["latency"]["time"].append(sentTime)
					result["byUserId"][userId]["transmissions"][id_transmission]["latency"]["value"].append(receivedTime-sentTime)

	return result