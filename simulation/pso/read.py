 # -*- coding: utf-8 -*-
import pickle
import sys
sys.path.append('/home/tulio/Dropbox/tools/omnetpp-4.6/ppgee-ppgc/application/coordination')
import pso
import matplotlib.pyplot as plt
import numpy as np
import routing
import pltWSN

def readDict(simpath):

	with open(simpath, "rb") as f:
		dicSimulation = pickle.load(f)

	return dicSimulation

def getArea(posNodesMU):
	return 0

def processFunctionCost(dicSimulation):
	version = dicSimulation["version"]
	if version == 5:
		routes 		= dicSimulation["P_Glob"].R
		connected 	= dicSimulation["P_Glob"].F.Connectivity
		cost 		= dicSimulation["P_Glob"].F.Cost
	elif version == 4:
		routes = routing.computeRoutingData(dicSimulation["namesNodesMU"], dicSimulation["posNodesMU"],
											dicSimulation["namesNodesRU"], dicSimulation["P_Glob"].Xr,
											dicSimulation["dcm"])
		connected = len(routes)
		cost = dicSimulation["P_Glob"].F.Cost
	elif version == 3:
		k = max(dicSimulation["P"], key=int)
		connected = dicSimulation["P"][k]["P_Glob"].F.Connectivity
		cost = dicSimulation["P"][k]["P_Glob"].F.Cost
		routes = []
	else:
		del dicSimulation[""]
		connected = 0
		cost = 0
		routes = []
	return connected, cost, routes

class ProcessedData():

	def __init__(self, simpath):

		dicSimulation = readDict(simpath)

		self.area 				= getArea(dicSimulation["posNodesMU"])
		self.w 					= dicSimulation["w"]
		self.c1 				= dicSimulation["c1"]
		self.c2 				= dicSimulation["c2"]
		self.nRU 				= len(dicSimulation["namesNodesRU"])
		self.nMU 				= len(dicSimulation["namesNodesMU"])
		self.connected, self.cost, self.routes = processFunctionCost(dicSimulation)
		self.localChanges 		= []
		self.globalChanges		= []

		j = -1
		c = 0
		for k in dicSimulation["P"]:
			# for i in dicSimulation["P"][k]:
			# 	if type(i) is int and dicSimulation["P"][k][i]["P_Best_Changed"]:
			# 		self.localChanges.append([i, k])
			if dicSimulation["P"][k]["P_Glob_Changed"]:
				self.globalChanges.append([dicSimulation["P"][k]["P_Glob_ChangedBy"], k])
				if (j==-1) and (dicSimulation["P"][k]["P_Glob"].F.Connectivity == routing.getIdealConnectionData(self.nMU)):
					j = k
				elif (j>-1) and (dicSimulation["P"][k]["P_Glob"].F.Connectivity != routing.getIdealConnectionData(self.nMU)):
					j = -1
					c = c + 1
		self.P_Glob_ConnectivityAchievedAt 	= j
		self.P_Glob_Disconnections 			= c

		if dicSimulation["version"] == 5:
			self.connectedporc 		= 100*len(self.connected)/routing.getIdealConnectionData(self.nMU)
		else:
			self.connectedporc 		= 100*len(self.routes)/routing.getIdealConnectionData(self.nMU)
		
		self.P_Glob_ChangedAt = dicSimulation["P_Glob_ChangedAt"]

def Test():

	processedData = ProcessedData("/home/tulio/Dropbox/tools/omnetpp-4.6/ppgee-ppgc/simulation/pso/results/2019_09_11/n0_d0070_nRU18_w0040_c10120_c20120.pickle")
	my_dpi=96
	fig = plt.figure(figsize=(480/my_dpi, 480/my_dpi), dpi=my_dpi)
	localChanges = np.array(processedData.globalChanges)
	plt.scatter(localChanges[:,0], localChanges[:,1])
	plt.show()

if __name__ == '__main__': Test()