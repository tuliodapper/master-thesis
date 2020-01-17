 # -*- coding: utf-8 -*-
import os
import sys
sys.path.append('../../application/coordination/')
import pltWSN
import utils
import pickle
import numpy as np
from matplotlib import pyplot 	as plt
from drawnow 	import drawnow, figure
import time
import readchar

clear = lambda : os.system('clear')

class Replay(object):
	"""docstring for Graphs"""
	def __init__(self, nameNodeC, namesNodesMU, posNodesMU, Nr, Nk, config):

		self.cost = []

		self.config = utils.fixConfig(config)

		my_dpi = 96
		self.figParticle = plt.figure(figsize=(480/my_dpi, 480/my_dpi), dpi=my_dpi)
		
		self.figCost, self.ax1  = plt.subplots()

		self.ax1.set_xlabel('iterations')

		color = 'tab:red'
		self.ax1.set_ylabel('cost', color=color)
		self.plt1 = [ self.ax1.plot([0], [0], color=color, label='Total Cost')[0] ]
						# self.ax1.plot([0], [0], color='black', label='Distance')[0],
						# self.ax1.plot([0], [0], color='green', label='Range Penalty')[0],
						# self.ax1.plot([0], [0], color='purple', label='Distance Penalty')[0]]
		self.ax1.tick_params(axis='y', labelcolor=color)
		self.ax1.legend()

		self.ax2 = self.ax1.twinx()  # instantiate a second axes that shares the same x-axis

		color = 'tab:blue'
		self.ax2.set_ylabel('connectivity', color=color)  # we already handled the x-label with ax1
		self.plt2 = self.ax2.plot([0], [0], color=color)[0]
		self.ax2.tick_params(axis='y', labelcolor=color)

		self.figCost.tight_layout()  # otherwise the right y-label is slightly clipped

		self.nameNodeC 		= nameNodeC
		self.namesNodesMU 	= namesNodesMU
		self.posNodesMU 	= posNodesMU
		self.Nr 			= Nr
		self.Nk 			= Nk

	def PlotParticle(self, P, iNk, filename = None):

		plt.figure(self.figParticle.number)
		
		plt.title("k = " + str(iNk)) 

		if filename is None:
			drawnow(pltWSN.plotWSN,	nameNodeC 		= self.nameNodeC,
							 		posNodeC		= P.Xc,
							 		namesNodesMU 	= self.namesNodesMU,
							 		posNodesMU 		= self.posNodesMU,
							 		namesNodesRU 	= self.Nr,
							 		posNodesRU		= P.Xr,
									routes 			= P.R,
									dcm_max 		= self.config["dcm"], 
									dcm 			= self.config["dcm"], 
									dcm_th 			= self.config["dcm"],
							 		filename		= filename,
							 		xlim 			= [0, self.config["dim"]], 
							 		ylim 			= [0, self.config["dim"]])
		else:
			pltWSN.plotWSN (nameNodeC 		= self.nameNodeC,
					 		posNodeC		= P.Xc,
					 		namesNodesMU 	= self.namesNodesMU,
					 		posNodesMU 		= self.posNodesMU,
					 		namesNodesRU 	= self.Nr,
					 		posNodesRU		= P.Xr,
							routes 			= P.R,
							dcm_max 		= self.config["dcm"], 
							dcm 			= self.config["dcm"], 
							dcm_th 			= self.config["dcm"],
					 		filename		= filename,
					 		xlim 			= [0, self.config["dim"]], 
					 		ylim 			= [0, self.config["dim"]])

	def PlotCost(self, cost, filename = None):
		
		# print("Connectivity: " + str(aux_cost[-1][1]))
		# print("Cost: " + str(aux_cost[-1][2]))

		aux_cost = np.array(cost)

		self.plt2.set_data(aux_cost[:,0], aux_cost[:,1])
		self.ax2.set_ylim([0, max(aux_cost[:,1])])

		self.plt1[0].set_data(aux_cost[:,0], aux_cost[:,2])
		# self.plt1[1].set_data(aux_cost[:,0], aux_cost[:,3])
		# self.plt1[2].set_data(aux_cost[:,0], aux_cost[:,4])
		# self.plt1[3].set_data(aux_cost[:,0], aux_cost[:,5])
		self.ax1.set_ylim([0, 3*10**7])

		self.ax1.set_xlim([0, len(cost)])

		if filename is None:
			plt.draw()
		else:
			plt.figure(self.figCost.number)
			plt.savefig(filename, dpi=96)
			plt.close()

def PlotCost4(plt1, plt2, cost, Nk):

	aux_cost = np.array(cost)

	plt1.set_data(aux_cost[:,0], aux_cost[:,1])
	plt2.set_data(aux_cost[:,0], aux_cost[:,2])

	plt.xlim([0, Nk])

def PlotCost3(ax, cost, k, Nk):

	color = 'tab:red'
	ax.set_xlabel('iterations')
	ax.set_ylabel('cost', color=color)
	ax.plot(k, cost, color=color)
	ax.tick_params(axis='y', labelcolor=color)

def PlotCost2(plt, cost, Nk):

	aux_cost = np.array(cost)

	fig, ax1  = plt.subplots()

	color = 'tab:red'
	ax1.set_xlabel('iterations')
	ax1.set_ylabel('cost', color=color)
	ax1.plot(aux_cost[:,0], aux_cost[:,2], color=color)
	ax1.tick_params(axis='y', labelcolor=color)

	ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis

	color = 'tab:blue'
	ax2.set_ylabel('connectivity', color=color)  # we already handled the x-label with ax1
	ax2.plot(aux_cost[:,0], aux_cost[:,1], color=color)
	ax2.tick_params(axis='y', labelcolor=color)

	fig.tight_layout()  # otherwise the right y-label is slightly clipped

def PlotCost(plt, cost, k, Nk):
	plt.plot(k, cost)
	plt.xlim([0, Nk])
	plt.title("k = " + str(k[-1]) + " | cost = " + str(cost[-1])) 

def PlotParticle(plt, nameNodeC, namesNodesMU, posNodesMU, namesNodesRU, P, k, i):
	pltWSN.plotWSN(	nameNodeC=nameNodeC, posNodeC=P.Xc,\
					namesNodesMU=namesNodesMU, posNodesMU=posNodesMU, \
					namesNodesRU=namesNodesRU, posNodesRU=P.Xr, \
					dcm_max = config["dcm"], dcm = config["dcm"], dcm_th = config["dcm"],\
					routes=P.R, filename = None, xlim = [0, config["dim"]], ylim = [0, config["dim"]])
	plt.title("k = " + str(k) + " | i = " + str(i)) 

def Plot(plt, nameNodeC, namesNodesMU, posNodesMU, namesNodesRU, P_Orig, P_Best, P_Glob, k, i):
	pltWSN.plotWSN(	nameNodeC=nameNodeC, posNodeC=P_Orig.Xc,\
					namesNodesMU=namesNodesMU, posNodesMU=posNodesMU, \
					namesNodesRU=namesNodesRU, posNodesRU=P_Orig.Xr, \
					dcm_max = config["dcm"], dcm = config["dcm"], dcm_th = config["dcm"],\
					routes=P_Orig.R, filename = None, xlim = [0, config["dim"]], ylim = [0, config["dim"]],
					marker = ".", linecolor = "b")
	pltWSN.plotWSN(	nameNodeC=nameNodeC, posNodeC=P_Best.Xc,\
					namesNodesMU=namesNodesMU, posNodesMU=posNodesMU, \
					namesNodesRU=namesNodesRU, posNodesRU=P_Best.Xr, \
					dcm_max = config["dcm"], dcm = config["dcm"], dcm_th = config["dcm"],\
					routes=P_Best.R, filename = None, xlim = [0, config["dim"]], ylim = [0, config["dim"]],
					marker = "+", linecolor = "k")
	pltWSN.plotWSN(	nameNodeC=nameNodeC, posNodeC=P_Glob.Xc,\
					namesNodesMU=namesNodesMU, posNodesMU=posNodesMU, \
					namesNodesRU=namesNodesRU, posNodesRU=P_Glob.Xr, \
					dcm_max = config["dcm"], dcm = config["dcm"], dcm_th = config["dcm"],\
					routes=P_Glob.R, filename = None, xlim = [0, config["dim"]], ylim = [0, config["dim"]],
					marker = "x", linecolor = "g")
	plt.title("k = " + str(k) + " | i = " + str(i)) 


def Save(nameNodeC, namesNodesMU, posNodesMU, namesNodesRU, P_Orig, P_Best, P_Glob, k, i):
	my_dpi = 96
	fig1 = plt.figure(figsize=(480/my_dpi, 480/my_dpi), dpi=my_dpi)
	storepath = simpath + str(i) + "/"
	if not os.path.exists(storepath):
		os.mkdir(storepath)
	Plot(plt, nameNodeC, namesNodesMU, posNodesMU, namesNodesRU, P_Orig, P_Best, P_Glob, k, i)
	plt.savefig(storepath + str(k) + ".png", dpi=96)
	plt.close()

def Read(dicSimulation):

	nameNodeC = dicSimulation["nameNodeC"]
	namesNodesMU = dicSimulation["namesNodesMU"]
	posNodesMU = dicSimulation["posNodesMU"]
	namesNodesRU = dicSimulation["namesNodesRU"]

	for k in dicSimulation["P"]:
		for i in dicSimulation["P"][k]:
			if type(i) is int:
				P_Orig = dicSimulation["P"][k][i]["P_Orig"]
				P_Best = dicSimulation["P"][k][i]["P_Best"]
				P_Glob = dicSimulation["P"][k][i]["P_Glob"]
				Save(nameNodeC, namesNodesMU, posNodesMU, namesNodesRU, P_Orig, P_Best, P_Glob, k, i)

def PlotPGlobal(dicSimulation):

	nameNodeC = dicSimulation["nameNodeC"]
	namesNodesMU = dicSimulation["namesNodesMU"]
	posNodesMU = dicSimulation["posNodesMU"]
	namesNodesRU = dicSimulation["namesNodesRU"]

	for k in dicSimulation["P"]:

		P_Glob = dicSimulation["P"][k]["P_Glob"]

		drawnow(pltWSN.plotWSN, nameNodeC=nameNodeC, posNodeC=P_Glob.Xc,\
								namesNodesMU=namesNodesMU, posNodesMU=posNodesMU, \
								namesNodesRU=namesNodesRU, posNodesRU=P_Glob.Xr, \
								dcm_max = config["dcm"], dcm = config["dcm"], dcm_th = config["dcm"],\
								routes=P_Glob.R, filename = None, xlim = [0, config["dim"]], ylim = [0, config["dim"]])

		time.sleep(0.1)

def PlotP(dicSimulation, i):

	_replay = Replay(nameNodeC = dicSimulation["nameNodeC"],
					 namesNodesMU = dicSimulation["namesNodesMU"],
					 posNodesMU = dicSimulation["posNodesMU"],
					 Nr = dicSimulation["namesNodesRU"],
					 Nk = 300)


	for k in dicSimulation["P"]:
		_replay.Plot(	PLocal = dicSimulation["P"][k][i]["P_Orig"], 
						PGlob = dicSimulation["P"][k]["P_Glob"], 
						iNk = k)

def PlotPGlobFinal(dicSimulation):

	my_dpi = 96
	fig1 = plt.figure(figsize=(480/my_dpi, 480/my_dpi), dpi=my_dpi)

	nameNodeC 		= dicSimulation["nameNodeC"]
	namesNodesMU 	= dicSimulation["namesNodesMU"]
	posNodesMU 		= dicSimulation["posNodesMU"]
	namesNodesRU 	= dicSimulation["namesNodesRU"]
	P_Glob			= dicSimulation["P_Glob"]

	pltWSN.plotWSN(	nameNodeC=nameNodeC, posNodeC=P_Glob.Xc,\
					namesNodesMU=namesNodesMU, posNodesMU=posNodesMU, \
					namesNodesRU=namesNodesRU, posNodesRU=P_Glob.Xr, \
					dcm_max = config["dcm"], dcm = config["dcm"], dcm_th = config["dcm"],\
					routes=P_Glob.R, filename = None, xlim = [0, config["dim"]], ylim = [0, config["dim"]],
					marker = ".", linecolor = "b")

	plt.show()

dicSimulation = None

try:
	simfile = sys.argv[1]
	with open(simfile, "rb") as f:
		dicSimulation = pickle.load(f)
except Exception as e:
	pass
else:
	pass
finally:
	pass

if not dicSimulation is None:
	# for key in dicSimulation:
	# 	field_type = type(dicSimulation[key])
	# 	if field_type is not list and field_type is not dict:
	# 		print(key + " => " + str(dicSimulation[key]))

	_replay = Replay(nameNodeC = dicSimulation["nameNodeC"],
					 namesNodesMU = dicSimulation["namesNodesMU"],
					 posNodesMU = dicSimulation["posNodesMU"],
					 Nr = dicSimulation["namesNodesRU"],
					 Nk = 300,
					 config = dicSimulation["config"])
	cost = []
	for k in dicSimulation["P"]:
		cost.append([k, dicSimulation["P"][k]["P_Glob"].F.Connectivity, dicSimulation["P"][k]["P_Glob"].F.Cost])

	k = len(cost)-1

	_replay.PlotCost(cost)
	_replay.PlotParticle(	P = dicSimulation["P"][k]["P_Glob"], 
							iNk = k)
	while True:
		print("Simulation:")
		print("file => " + str(simfile))
		print("itearation => " + str(k))
		print("cost function => " + str(cost[k]))
		print("")
		print("Commands:")
		print("a => increase k.")
		print("d => descrease k.")
		print("s => set custom k.")
		print("p => print.")
		print("e => exit.")
		c = readchar.readchar()
		if c == "a" and k > 0:
			k = k - 1
			clear()
		elif c == "d" and k < (300-1):
			k = k + 1
			clear()
		elif c == "s":
			k1 = input("Enter Iteration k: ")
			if type(k) is int:
				k = k1
			clear()
		elif c == "p":
			print("-> posCN:")
			print(dicSimulation["P"][k]["P_Glob"].Xc)
			print("\n-> posRN:")
			print(dicSimulation["P"][k]["P_Glob"].Xr)
			print("\n-> posUN:")
			print(dicSimulation["posNodesMU"])
			print("\n-> routes:")
			print(dicSimulation["P"][k]["P_Glob"].R)
			print("")
		elif c == "e":
			break
		else:
			continue
		_replay.PlotParticle(	P = dicSimulation["P"][k]["P_Glob"], 
								iNk = k)