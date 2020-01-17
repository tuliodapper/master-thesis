# -*- coding: utf-8 -*-
import numpy as np
import copy
import math
import sys
from time import localtime, mktime
from datetime import timedelta

def progressbar(it, prefix="", size=50, file=sys.stdout):
	init_time = mktime(localtime())
	count = len(it)
	def sec2str(seconds):
		return "{:0>8}".format(str(timedelta(seconds=int(seconds))))
	def show(j):
		run_time 	= mktime(localtime()) - init_time
		srun_time 	= sec2str(seconds=run_time)
		if j==0:
			sremain_time = "---"
		else:
			sremain_time = sec2str(seconds=(run_time/j)*count-run_time)
		x = int(size*j/count)
		p = str(100*j/count) + "%"
		file.write("%s [%s%s] [%03d/%03d] [%s] [%s] [%s]\r" % (prefix, "#"*x, "."*(size-x), j, count, p, srun_time, sremain_time))
		file.flush()
	print("")
	show(0)
	for i, item in enumerate(it):
		yield item
		show(i+1)
	file.write("\n")
	file.flush()

def computeDistance(location1, location2):
	dx	= abs(location2[0] - location1[0])
	dy	= abs(location2[1] - location1[1])
	#dz	= abs(location2[2] - location1[2])
	distance = math.sqrt((dx * dx) + (dy * dy))
	return distance

def concatenateNames(nameNodeC, namesNodesMU, namesNodesRU):
	if nameNodeC is not None:
		names 	= [nameNodeC]
	else:
		names 	= []
	if (namesNodesMU is not None) and (len(namesNodesMU) > 0):
		names 	= names + namesNodesMU
	if (namesNodesRU is not None) and (len(namesNodesRU) > 0):
		names 	= names + namesNodesRU
	return names

def concatenatePos(posNodeC, posNodesMU, posNodesRU):
	pos = None
	if posNodeC is not None:
		pos = np.array([posNodeC])
	if (posNodesMU is not None) and (len(posNodesMU) > 0):
		if pos is not None:
			pos = np.concatenate((pos, posNodesMU), axis=0)
		else:
			pos = posNodesMU
	if (posNodesRU is not None) and (len(posNodesRU) > 0):
		if pos is not None:
			pos	= np.concatenate((pos, posNodesRU), axis=0)
		else:
			pos = posNodesRU
	return pos

def repeatPos(pos, n):
	return np.repeat(np.array([pos]), n, axis=0)

def computeName(posNodesRU, suf = "r"):
	Nr = []
	if len(posNodesRU) > 0:
		Nr = np.arange(len(posNodesRU))
		Nr = [str(c) for c in Nr]
		for iNr in range(0, len(Nr)):
			Nr[iNr] = suf + Nr[iNr]
	return Nr

def computeClean(	nameNodeC, posNodeC,
					namesNodesMU, posNodesMU,
					namesNodesRU, posNodesRU,
					routes, delete = False):
	if not delete:
		posNodesRU2 = np.copy(posNodesRU)
		namesNodesRU2 = copy.copy(namesNodesRU)
	else:
		posNodesRU2 = None
		namesNodesRU2 = []

	for i in range(0,len(namesNodesRU)):
		found = False
		for route in routes:
			if (len(route) > 2):
				isValid = False
				try:
					j = namesNodesMU.index(route[0])
					isValid = True
				except Exception as e:
					pass
				try:
					j = namesNodesMU.index(route[-1])
					isValid = True
				except Exception as e:
					pass
				if isValid:
					try:
						j = route.index(namesNodesRU[i])
						found = True
						break
					except Exception as e:
						pass
		if not found:
			if not delete:
				posNodesRU2[i] = posNodeC
		else:
			if delete:
				if posNodesRU2 is None:
					posNodesRU2 = np.copy([posNodesRU[i]])
				else:
					posNodesRU2 = np.concatenate((posNodesRU2, np.array([posNodesRU[i]])), axis=0)
				namesNodesRU2.append(namesNodesRU[i])
	if posNodesRU2 is None:
		posNodesRU2 = np.array([])
		namesNodesRU2 = []

	return namesNodesRU2, posNodesRU2

def computeChoose(posNodesRU, posNodesRU_new):

	nRU = len(posNodesRU)

	waypoints = np.zeros((nRU,2))

	waypoints[0] = posNodesRU_new[0]

	n = nRU - 1

	s_old = [False]*n
	s_new = [False]*n

	d = np.zeros((n,n))

	for i in range(0,n):
		for j in range(0,n):
			d[i,j] = computeDistance(posNodesRU[i+1], posNodesRU_new[j+1])

	count = 0
	dim = 5000

	while(count<n):
		m = np.unravel_index(d.argmin(), d.shape)
		i_old = m[0]
		i_new = m[1]
		if not s_old[i_old] and not s_new[i_new]:
			waypoints[i_old+1]	= posNodesRU_new[i_new+1]
			s_old[i_old]		= True
			s_new[i_new]		= True
			count	 			+= 1
		d[i_old, :] = dim
		d[:, i_new] = dim

	return waypoints

def findIndices(lst, condition):
	return [i for i, elem in enumerate(lst) if condition(elem)]

def computeCenter(arr):
	# print(arr)
	length = arr.shape[0]
	# print(length)
	sum_x = np.sum(arr[:, 0])
	# print(sum_x)
	sum_y = np.sum(arr[:, 1])
	# print(sum_y)
	return sum_x/length, sum_y/length

def computeDistribution(posNode1, posNode2):
	posNodes = []
	d = utils.computeDistance(posNode1, posNode2)
	l = int(math.ceil(d/(config.lam*config.r_max)))
	a = (posNode2 - posNode1[i])/(l+1)
	for k in range(0,l):
		b = posNode1[i] + (k+1)*a
		posNodes = posNodes + [b]
	return posNodes

def fixConfig(config):

	standardConfig = getStandardConfig()

	for key in standardConfig:
		if not key in config:
			config[key] = standardConfig[key]

	return config

def getStandardConfig():

	config = {}

	config["pc"] = "pclascar"

	config["rootpath"] = "/home/tulio/Dropbox/tools/omnetpp-4.6/projects/"
	config["simpath"]	 = config["rootpath"] + "M3WSN/Simulations/SDFANET-" + config["pc"] + "/"
	config["filedata"] = "sim-top.ini"

	config["dim"] = 2000

	# maximum distance between nodes for reconfiguring communication
	config["dcm_min"] 	= 15 #
	# maximum distance between nodes for communication
	config["dcm"] 		= 300 #
	# maximum distance between nodes for reconfiguring communication
	config["dcm_th"] 		= 45 #

	config["r_min"] 		= 15
	config["r_max"] 		= 45

	# maximum distance between nodes for reconfiguring communication
	config["dcm_max"] 		= 55 #
	# minimum distance between nodes
	config["dsf"]				= 30  # article: 30
	# number of particles in a swarm
	config["Np"] 				= 30 # article: 30
	# number of iterations
	config["Nk"] 				= 300 # article: 300
	# inertia weight (regulates the drastic change of the velocity)
	config["w"] 				= 0.729
	# local/cognitive parameter (exploitation)
	config["c1"] 				= 1.4962 # article: 1.4962
	# global/social parameter (exploration
	config["c2"] 				= 1.4962 # article: 1.4962
	# threshould percentage for velocity clamping
	config["Vjmax_perc"] 		= 0.1 # article: 0.1
	# number of tries in order to exit
	config["n_tries"] 		= 20
	# coeficiente de penalidade para restrição do range
	config["u_distance"] 		= 1
	# config["coeficiente"] da potência da distância
	config["u_distance_pow"]	= 2
	# coeficiente de penalidade para restrição do range
	config["u_range"] 		= 50
	# coeficiente da potência da distância
	config["u_range_pow"] 	= 2
	# coeficiente de penalidade para restrição de segurança
	config["u_security"] 		= 50
	# coeficiente de penalidade para preferência ao menor número de hops possível
	config["u_hops"] 			= 5
	# coeficiente de penalidade por deixar MU desconectada
	config["u_connect"] 		= 100

	# adjustment variables
	config["alpha"] 		= 5
	config["gama"] 		= 0.05
	config["gama_r"] 		= 2

	# nodes
	config["speed"] 		= 4

	config["prodtime"] 	= 1.2
	config["prodr"]		= 1.2

	# n values
	config["n_routing"] = 4
	config["n_construction"] = 4

	return config