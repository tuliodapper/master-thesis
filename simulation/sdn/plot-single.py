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

path = "/opt/omnetpp-4.6/project/M3WSN/Simulations/SDN/"

###
# Constants
###

file_result		= "result.pickle"
list_color 		= ['b','g','r','k','c','m','y','w','b','g','r','k','c','m','y','w','b','g','r','k','c','m','y','w']
list_linestyle 	= ['-','--','-.',':','-','--','-.',':','-','--','-.',':','-','--','-.',':','-','--','-.',':','-','--','-.',':']

####
# Generate .pickle simulation files
####

result = read.Read(path)

if result is not None:

	with open(path+file_result, "wb") as f:
		pickle.dump(result, f)

	####
	# Plotting results
	####

	list_RN  	= [1,2,13] #range(1, 4)
	list_graph 	= ["lenQueueMac", "lenQueueNet", "packetDropMac", "packetDropNet"]

	for graph in list_graph:

		print("Plotting: " + graph)
			
		plt.figure()

		for index_RN, RN in enumerate(list_RN):
			if RN in result["byCN"]["byRN"]:
				plt.plot(	result["byCN"]["byRN"][RN]["telemetry"]["timeSeries"], 
							result["byCN"]["byRN"][RN]["telemetry"][graph], 
							label=str(RN) + " - byCN", 
							zorder=index_RN,
							linestyle = list_linestyle[0],
							color = list_color[index_RN])
			if RN in result["byRN"]:
				plt.plot(	result["byRN"][RN]["telemetry"]["timeSeries"], 
							result["byRN"][RN]["telemetry"][graph], 
							label=str(RN) + " - byRN", 
							zorder=index_RN,
							linestyle = list_linestyle[1],
							color = list_color[index_RN])

		# plt.xlim([0, 100])
		# plt.ylim([0, 50])
		plt.legend()
		plt.title(graph)

	# graph = "Latency"

	# plt.figure()
	# for index_userId, userId in enumerate(result["byUserId"]):
	# 	for index_id_transmission, id_transmission in enumerate(result["byUserId"][userId]["transmissions"]):
	# 		print("Plotting: " + graph)
	# 		plt.plot(	result["byUserId"][userId]["transmissions"][id_transmission]["latency"]["time"], 
	# 					result["byUserId"][userId]["transmissions"][id_transmission]["latency"]["value"], 
	# 					label = "UN: " + str(userId) + " - IdTrans: " + str(id_transmission), 
	# 					linestyle = list_linestyle[index_id_transmission],
	# 					color = list_color[index_userId])
	# plt.legend()
	# plt.title(graph)

	# graph = "PacketLoss"

	# plt.figure()
	# for index_userId, userId in enumerate(result["byUserId"]):
	# 	for index_id_transmission, id_transmission in enumerate(result["byUserId"][userId]["transmissions"]):
	# 		print("Plotting: " + graph)
	# 		receivedPackets = np.array(result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["receivedPackets"])
	# 		sentPackets 	= np.array(result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["sentPackets"])
	# 		plt.plot(	result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["time"], 
	# 					100.0*(1.0-1.0*receivedPackets/sentPackets),
	# 					label = "UN: " + str(userId) + " - IdTrans: " + str(id_transmission) + " - Rate", 
	# 					linestyle = list_linestyle[2*index_id_transmission],
	# 					color = list_color[index_userId])
	# 		plt.plot(	result["byUserId"][userId]["transmissions"][id_transmission]["packetloss"]["time"], 
	# 					np.ones(len(sentPackets))*100.0*(1.0-1.0*np.sum(receivedPackets)/np.sum(sentPackets)),
	# 					label = "UN: " + str(userId) + " - IdTrans: " + str(id_transmission) + " - Total", 
	# 					linestyle = list_linestyle[2*index_id_transmission+1],
	# 					color = list_color[index_userId])
	# plt.legend()
	# plt.title(graph)
	plt.show()