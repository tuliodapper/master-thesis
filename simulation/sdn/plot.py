import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/../../application/coordination/")
import pickle
import utils
import numpy as np
import read
import matplotlib.pyplot as plt
import pandas as pd

# CONST_DATA_FILE	= "results.pickle"
# CONST_CFG_FILE	= "config.pickle"

# path = sys.argv[1]
# path = "/home/tulio/Dropbox/tools/omnetpp-4.6/ppgee-ppgc/simulation/sdn/results/2019-11-26-20-40-11/"
# path = "/home/tulio/Dropbox/tools/omnetpp-4.6/ppgee-ppgc/M3WSN/Simulations/SDN/"
# path = "/home/tulio/Dropbox/tools/omnetpp-4.6/ppgee-ppgc/M3WSN/Simulations/AODV/"

path_results	= "/opt/omnetpp-4.6/project/M3WSN/Simulations/"
overwrite 		= True

###
# Constants
###

file_result		= "result.pickle"
list_color 		= ['b','g','r','k','c','m','y','w']
list_linestyle 	= ['-','--','-.',':','.',',']

###
# Gather list of path','
###

paths = os.walk(path_results)
list_path = []
for path, j, y in os.walk(path_results) :
	 list_path.append(path + "/")

####
# Generate .pickle simulation files
####

for i in utils.progressbar(range(len(list_path)), "Reading OMNet Files"):

	path = list_path[i]

	if (not os.path.isfile(path+file_result)) or overwrite:

		result = read.Read(path)

		if result is not None:

			with open(path+file_result, "wb") as f:
				pickle.dump(result, f)

####
# Collect results
####

collection = {} # categorized by name, nUN and graph

for i in utils.progressbar(range(len(list_path)), "Collecting computed files"):

	path = list_path[i]

	if os.path.isfile(path+file_result):

		with open(path+file_result, "rb") as f:
			result = pickle.load(f)

		name	= result["name"]
		nUN 	= result["nUN"]

		####

		graph = "NumberOfPackets"

		PACKETS = {}

		PACKETS["SDN"] = {}
		PACKETS["SDN"]["DATA"] 		= ["SDN_DATA"]
		PACKETS["SDN"]["CONTROL"] 	= ["SDN_PACKET_IN", "SDN_UPDATE_ROUTING_TABLE"]
		 
		PACKETS["AODV"] = {}
		PACKETS["AODV"]["DATA"] 	= ["AODV_DATA_PACKET"]
		PACKETS["AODV"]["CONTROL"] 	= ["AODV_RREQ_PACKET","AODV_RREP_PACKET"]

		PACKTES_KIND = PACKETS[name]["DATA"] + PACKETS[name]["CONTROL"]

		if not graph in collection:
			collection[graph] = {}
		if not nUN in collection[graph]:
			collection[graph][nUN] = {}
		if not name in collection[graph][nUN]:
			collection[graph][nUN][name] = {}

		if len(collection[graph][nUN][name]) == 0:
			collection[graph][nUN][name]["time"] = result["timeSeries"]["time"]
			collection[graph][nUN][name]["packets"] = {}
			for packet_kind in result["timeSeries"]["numberOfPacketsByPacket"]:
				if packet_kind in PACKTES_KIND and len(result["timeSeries"]["numberOfPacketsByPacket"][packet_kind]) > 0:
					collection[graph][nUN][name]["packets"][packet_kind] = result["timeSeries"]["numberOfPacketsByPacket"][packet_kind]

		####

		graph = "DeliveredRate"

		if not graph in collection:
			collection[graph] = {}
		if not name in collection[graph]:
			collection[graph][name] = {}
		if not nUN in collection[graph][name]:
			collection[graph][name][nUN] = {}

		if len(collection[graph][name][nUN]) == 0:
			numberOfSentPackets = np.array(result["timeSeries"]["numberOfSentPackets"])
			numberOfReceivedPackets = np.array(result["timeSeries"]["numberOfReceivedPackets"])
			numberOfSentPackets[numberOfSentPackets == 0] = 1
			collection[graph][name][nUN]["time"] = result["timeSeries"]["time"]
			collection[graph][name][nUN]["rate"] = 100*numberOfReceivedPackets/numberOfSentPackets

		####

		graph = "LatencyByHops"

		NUMBER_HOPS = [1, 2, 3, 4, 5, 6]

		if not graph in collection:
			collection[graph] = {}
		if not nUN in collection[graph]:
			collection[graph][nUN] = {}
		if not name in collection[graph][nUN]:
			collection[graph][nUN][name] = {}
		for nHops in NUMBER_HOPS:
			if not nHops in collection[graph][nUN][name]:
				collection[graph][nUN][name][nHops] = []
			if nHops in result["latencyByHops"]:
				collection[graph][nUN][name][nHops] = collection[graph][nUN][name][nHops] + result["latencyByHops"][nHops]

		####

		graph = "NumberOfDrops"
		if not graph in collection:
			collection[graph] = {}
		if not nUN in collection[graph]:
			collection[graph][nUN] = {}
		if not name in collection[graph][nUN]:
			collection[graph][nUN][name] = {}

		if len(collection[graph][nUN][name]) == 0:
			collection[graph][nUN][name]["time"] = result["timeSeries"]["time"]
			collection[graph][nUN][name]["drops"] = result["timeSeries"]["numberOfDrops"]
			collection[graph][nUN][name]["dropsNet"] = result["timeSeries"]["numberOfDropsNetwork"]
			collection[graph][nUN][name]["dropsMac"] = result["timeSeries"]["numberOfDropsMac"]

####
# Plotting results
####

graph = "NumberOfPackets"

print("Plotting: " + graph)

for index_nUN, nUN in enumerate(collection[graph]):
	for name in collection[graph][nUN]:
		plt.figure()
		for index_packet_kind, packet_kind in enumerate(collection[graph][nUN][name]["packets"]):
			plt.plot(	collection[graph][nUN][name]["time"], 
						collection[graph][nUN][name]["packets"][packet_kind], 
						label=packet_kind, 
						zorder=index_packet_kind+1,
						linestyle = list_linestyle[0],
						color = list_color[index_packet_kind])
		if name == "AODV":
			plt.ylim([0, 1500])
		plt.xlim([0, 100])
		# plt.ylim([0, 50])
		plt.legend()
		plt.title("Transmitted Packets. " + name + " / " + str(nUN))

####

graph = "DeliveredRate"

print("Plotting: " + graph)

plt.figure()
for index_name, name in enumerate(collection[graph]):
	for index_nUN, nUN in enumerate(collection[graph][name]):
		plt.plot(	collection[graph][name][nUN]["time"], 
					collection[graph][name][nUN]["rate"], 
					label=name + " -> " + str(nUN),
					linestyle = list_linestyle[index_nUN],
					color = list_color[index_name])
plt.xlim([0, 100])
plt.ylim([0, 110])
plt.legend()
plt.title("Delivered Rate")

####

graph = "LatencyByHops"

print("Plotting: " + graph)

index = NUMBER_HOPS
values = {}
for index_nUN, nUN in enumerate(collection[graph]):
	for index_name, name in enumerate(collection[graph][nUN]):
		values[name] = []
		for nHops in NUMBER_HOPS:
			values[name].append(0)
			if nHops in collection[graph][nUN][name]:
				values[name][-1] = np.mean(collection[graph][nUN][name][nHops])
	df = pd.DataFrame(values, index=index)		
	df.plot.bar(rot=0, title="Latency By Hops -> " + str(nUN))

####

graph = "NumberOfDrops"

print("Plotting: " + graph)

for index_nUN, nUN in enumerate(collection[graph]):
	plt.figure()
	for index_name, name in enumerate(collection[graph][nUN]):
		plt.plot(	collection[graph][nUN][name]["time"], 
					collection[graph][nUN][name]["drops"], 
					label=name + " - Total",
					linestyle = list_linestyle[0],
					color = list_color[index_name])
		plt.plot(	collection[graph][nUN][name]["time"], 
					collection[graph][nUN][name]["dropsMac"], 
					label=name + " - Mac",
					linestyle = list_linestyle[1],
					color = list_color[index_name])
		plt.plot(	collection[graph][nUN][name]["time"], 
					collection[graph][nUN][name]["dropsNet"], 
					label=name + " - Net",
					linestyle = list_linestyle[2],
					color = list_color[index_name])
	plt.xlim([0, 100])
	plt.legend()
	plt.title("Number of Drops -> " + str(nUN))
	
plt.show()