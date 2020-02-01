import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/../../application/coordination/")
import pso
import utils
import numpy as np
import math
import pickle
import interface

CONST_CONFIG_FILE	= "config.pickle"

simpath = "/home/tulio/Desktop/"

if len(sys.argv)>2:
	cfgpath = sys.argv[2]
else:
	cfgpath = ""

if not os.path.exists(simpath):
	os.mkdir(simpath)


if cfgpath == "":
	# User definitions #

	config 				= {}
	config["N"] 		= 1
	config["nUN"] 		= 20
	config["dim"] 		= 200
	config["dcm"] 		= 55
	config["density"] 	= [0.7]
	config["nRU"] 		= [15]
	config["w"] 		= [[0.9, 0.4]] # np.arange(0.5, 0.9, 0.1)
	config["c1"] 		= [1.4962] # np.arange(1.3, 1.7, 0.1)
	config["c2"] 		= [1.4962] #np.arange(1.3, 1.7, 0.1)
	config["i"] 		= 0

	# ################# #

	# config["posUN"] 	= []
	# for _density in config["density"]:
	# 	radius 	= _density*0.5*config["dim"] + np.random.normal(0, 75, config["nUN"])
	# 	angle 	= np.random.uniform(0,2*math.pi,config["nUN"])
	# 	_posUN 	= np.multiply(	np.array([radius,radius]).transpose(), 
	# 							np.array([np.cos(angle), np.sin(angle)]).transpose()) + config["dim"]*0.5
	# 	_posUN[_posUN > (config["dim"]-1)] = config["dim"]-1
	# 	_posUN[_posUN < 1] = 1
	# 	config["posUN"].append(_posUN)

	config["posUN"]	= 	[np.array(	[[0,	0	], # 16
									 [0,	75	], # 17
									 [0,	180	], # 18
									 [95,   170 ], # 19
									 [85, 	0	], # 20
									 [160, 	0	], # 21
									 [180, 	195	], # 22
									 [180, 	95	], # 23
									 ])]
	config["nUN"] 		= len(config["posUN"])

	# ################# #
else:

	with open(cfgpath, "rb") as f:
		config = pickle.load(f)

	config["N"] 		= 3
	config["nRU"] 		= [10]
	config["w"] 		= [[0.9, 0.4], 0.729] # np.arange(0.5, 0.9, 0.1)
	config["c1"] 		= [1.4962] # np.arange(1.3, 1.7, 0.1)
	config["c2"] 		= [1.4962] #np.arange(1.3, 1.7, 0c.1)
	config["posUN"] 	= [config["posUN"][0]]
	config["density"] 	= [config["density"][0]]

if len(sys.argv)>3:
	if sys.argv[3] == "-r":
		config["i"] = 5

list_config = []

for iPosUN in range(len(config["posUN"])):
	for _nRU in config["nRU"]:
		for _w in config["w"]:
			for _c1 in config["c1"]:
				for _c2 in config["c2"]:
					for _n in range(config["N"]):
						__filename = []
						__filename.append("n" + str(_n))
						__filename.append("d" + '{:05.2f}'.format(config["density"][iPosUN]))
						__filename.append("nRU"+str(_nRU))
						if not type(_w) is list:
							__filename.append("w"+'{:05.2f}'.format(_w))
						elif len(_w) == 1:
							__filename.append("w"+'{:05.2f}'.format(_w[0]))
						else:
							__filename.append("w"+'{:05.2f}'.format(_w[0])+ "-" + '{:05.2f}'.format(_w[1]))
						__filename.append("c1"+'{:05.2f}'.format(_c1))
						__filename.append("c2"+'{:05.2f}'.format(_c2))

						_filename = ""
						for ___filename in __filename:
							_filename = _filename + "_" + ___filename

						_filename = _filename.replace(".","")
						_filename = simpath + "/" + _filename + ".pickle"

						_config = config
						_config["n"] 		= _n
						_config["posUN"] 	= config["posUN"][iPosUN]
						_config["density"] 	= config["density"][iPosUN]
						_config["nRU"] 		= _nRU
						_config["w"] 		= _w
						_config["c1"] 		= _c1
						_config["c2"] 		= _c2
						_config["filename"] = _filename
									
						_config = utils.fixConfig(_config)

						list_config.append(_config)

if not os.path.exists(simpath):
	os.mkdir(simpath)

config["note"] = ""

for i in utils.progressbar(range(config["i"], len(list_config)), "Running PSO"):

	config["i"] = i

	with open(simpath + "/" + CONST_CONFIG_FILE, "wb") as f:
		pickle.dump(config, f)

	interface.run(	namesUN			= utils.computeName(list_config[i]["posUN"], "u"),  
					posUN 			= list_config[i]["posUN"], 
					nRU 			= list_config[i]["nRU"], 
					w 				= list_config[i]["w"], 
					c1 				= list_config[i]["c1"], 
					c2 				= list_config[i]["c2"], 
					savePICAt 		= list_config[i]["filename"],
					config 			= list_config[i])

	# pso.computePSO(	namesNodesMU	= utils.computeName(list_config[i]["posUN"], "u"),  
	# 				posNodesMU 		= list_config[i]["posUN"], 
	# 				nRU 			= list_config[i]["nRU"], 
	# 				w 				= list_config[i]["w"], 
	# 				c1 				= list_config[i]["c1"], 
	# 				c2 				= list_config[i]["c2"], 
	# 				savePICAt 		= list_config[i]["filename"],
	# 				paramConfig 	= list_config[i])

print("Done!")