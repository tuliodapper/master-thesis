# -*- coding: utf-8 -*-
#! /usr/bin/python
debug = True
# native lib
import sys
import os
import copy
import numpy as np
from matplotlib import pyplot 	as plt
from drawnow 	import drawnow, figure
# project lib
import construction
import adjustment
import routing
# own lib
import utils
import pltWSN

class Coordination(object):

	def __init__(self, config):
		
		self.config = config
		self.started 			= False
		self.time 				= 0
		self.nameNodeC 			= "0"
		self.posNodeC 			= None
		self.namesNodesMU 		= []
		self.posNodesMU			= None
		self.namesNodesRU 		= []
		self.posNodesRU			= None
		self.routes_last 		= []
		self.routes_target 		= []
		self.posNodesRU_target 	= []

		self.filepath = os.path.dirname(os.path.abspath(__file__))
		os.system("rm -rf " + self.config["simpath"] + self.config["filedata"])
		with open(self.config["simpath"] + self.config["filedata"], "w") as f:
			f.write("")
		f.close()

		self.construction 	= construction.Construction(config=self.config)

	# compute topology
	def computeTopology(self, time):

		self.time = time

		nRU = len(self.namesNodesRU)

		if debug:
			self.printAndSave("########################################################")
			self.printAndSave("------- IN")
			self.printAndSave("time:")
			self.printAndSave(time)
			self.printAndSave("self.posNodeC:")
			self.printAndSave(self.posNodeC)
			self.printAndSave("self.namesNodesRU:")
			self.printAndSave(self.namesNodesRU)
			self.printAndSave("self.posNodesRU:")
			self.printAndSave(self.posNodesRU)
			self.printAndSave("self.namesNodesMU:")
			self.printAndSave(self.namesNodesMU)
			self.printAndSave("self.posNodesMU:")
			self.printAndSave(self.posNodesMU)
			self.printAndSave("self.posNodesRU_target:")
			self.printAndSave(self.posNodesRU_target)

		posNodesRU2 = None
		routes2 	= None

		# intialization
		if (not self.started):

			self.config["namesUN"] = [str(c) for c in self.config["namesUN"]] 

			# self.posNodeC_construction, self.posNodesRU_construction = self.construction.computeConstruction(self.config["namesUN"], self.config["posUN"], nRU)
			
			self.posNodeC_construction 		= np.array(	[ 46.2617099,  113.20808974])
			self.posNodesRU_construction 	= np.array([[ 90.35116433, 199.        ],
														[ 72.91296309, 175.50019078],
														[  1.        , 117.39078419],
														[ 56.65032102,  47.84968012],
														[ 10.39410301,  26.29979387],
														[ 43.42193519, 183.60391224],
														[ 51.63839084, 153.16699831],
														[  1.72563541,  77.9262763 ],
														[ 25.52431382, 138.07060779],
														[ 89.04541352, 135.23920759],
														[  9.90122651, 166.52361893],
														[ 77.58225945,  92.67418484],
														[  8.59743802, 197.90954483],
														[ 29.43756246,  61.694819  ]])

			self.posNodesRU_construction = np.concatenate((np.array([self.posNodeC_construction]), self.posNodesRU_construction), axis=0)

			posNodesRU2 = self.posNodesRU_construction

			self.started = True

		routes2 = routing.computeRouting(	self.namesNodesRU[0], self.posNodesRU[0], 
											self.namesNodesMU, self.posNodesMU, 
											self.namesNodesRU[1:len(self.namesNodesRU)], 
											self.posNodesRU[1:len(self.posNodesRU)],
											self.config["dcm"])

		if not routes2 is None:
			self.routes_last 		= copy.copy(self.routes_target)
			self.routes_target 		= copy.copy(routes2)
		if not posNodesRU2 is None:
			self.posNodesRU_target 	= np.copy(posNodesRU2)

		if debug:
			self.printAndSave("OUT self.posNodesRU2:")
			self.printAndSave(posNodesRU2)

	def registerUser(self,name):
		name = str(name)
		if self.namesNodesMU is None:
			self.namesNodesMU = np.array([])
		try:
			index = self.namesNodesMU.index(name)
		except Exception as e:
			self.namesNodesMU.append(name)
			self.posNodesMU = utils.concatenatePos(None, self.posNodesMU, np.array([[0,0]]))
			index = -1
		else:
			pass
		finally:
			pass 
		return (index == -1)

	def registerRelay(self,name):
		name = str(name)
		if self.namesNodesRU is None:
			self.namesNodesRU = np.array([])
		self.namesNodesRU.append(name)
		self.posNodesRU = utils.concatenatePos(None, self.posNodesRU, np.array([[0,0]]))

	def notifyStatusUser(self,name, pos):
		name = str(name)
		index = self.namesNodesMU.index()
		self.posNodesMU[index] = pos

	def notifyStatusRelay(self,name, pos):
		name = str(name)
		index = self.namesNodesRU.index(name)
		self.posNodesRU[index] = pos

	def notifyStatusController(self, pos):
		self.posNodeC = pos

	def printAndSave(self, line):
		if debug:
			print(str(line))
			# self.save(line)

	def save(self, line):
		pass
		# f_d = open(self.config["simpath"] + self.config["filedata"], "a") 
		# f_d.write(str(self.time_current) + " - " + str(line) + "\n")
		# f_d.close()

def Test():
	dim = 100
	dcm = 20
	nRU = 5

	cordination = Coordination()

	cordination.nameNodeC 		= 'c'
	cordination.posNodeC		= np.array([10,10])
	cordination.namesNodesMU	= ['m0','m1','m2']
	cordination.posNodesMU		= np.array([[10,90],[90,10],[90,90]])

	# Generate RUs
	cordination.posNodesRU = []
	cordination.namesNodesRU = np.arange(nRU)
	cordination.namesNodesRU = [str(c) for c in cordination.namesNodesRU]
	for i in range(0, nRU):
		cordination.namesNodesRU[i] = "r" + cordination.namesNodesRU[i]
		cordination.posNodesRU = cordination.posNodesRU + [cordination.posNodeC]
	cordination.posNodesRU 	= np.array(cordination.posNodesRU)

	namesNodesRU2, posNodesRU2 = cordination.computeTopology(0);

	my_dpi=96
	fig1 = plt.figure(figsize=(480/my_dpi, 480/my_dpi), dpi=my_dpi)
	pltWSN.plotWSN(	nameNodeC=cordination.nameNodeC, posNodeC=cordination.posNodeC,\
					namesNodesMU=cordination.namesNodesMU, posNodesMU=cordination.posNodesMU, \
					namesNodesRU=namesNodesRU2, posNodesRU=posNodesRU2, \
					dcm_max = dcm, dcm = dcm, dcm_th = dcm,\
					routes=[], filename = None, xlim = [0, dim], ylim = [0, dim])
	plt.show()

if __name__ == '__main__': Test()
