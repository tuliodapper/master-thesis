import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/../../application/coordination/")
import pickle
import utils
import numpy as np
import read
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pso
import replay

CONST_DATA_FILE	= "results.pickle"
CONST_CFG_FILE	= "config.pickle"

def getResultsFromFolder(path, outpath):

	lstLoadPath = []
	paths = os.listdir(path)
	for file in paths:
		if not (file == CONST_CFG_FILE):
			loadFile = path + file
			lstLoadPath.append(loadFile)

	results = {}

	for i in utils.progressbar(range(len(lstLoadPath)), "Collecting"):

		loadFile = lstLoadPath[i]

		if loadFile == outpath:
			continue

		simulation = read.ProcessedData(loadFile)

		if not simulation.nRU in results:
			results[simulation.nRU] = {}
		if not simulation.connected in results[simulation.nRU]:
			 results[simulation.nRU] = {}
			 results[simulation.nRU]["w"] = []
			 results[simulation.nRU]["c1"] = []
			 results[simulation.nRU]["c2"] = []
			 results[simulation.nRU]["nMU"] = []
			 results[simulation.nRU]["area"] = []
			 results[simulation.nRU]["cost"] = []
			 results[simulation.nRU]["routes"] = []
			 results[simulation.nRU]["connected"] = []
			 results[simulation.nRU]["connectedporc"] = []

		results[simulation.nRU]["w"].append(simulation.w)
		results[simulation.nRU]["c1"].append(simulation.c1)
		results[simulation.nRU]["c2"].append(simulation.c2)
		results[simulation.nRU]["nMU"].append(simulation.nMU)
		results[simulation.nRU]["area"].append(simulation.area)
		results[simulation.nRU]["cost"].append(simulation.cost)
		results[simulation.nRU]["routes"].append(simulation.routes)
		results[simulation.nRU]["connected"].append(simulation.connected)
		results[simulation.nRU]["connectedporc"].append(simulation.connectedporc)

		print("\n" + 	"connected = " + str(simulation.connected) + " | " + 
						"connectedporc = " + str(simulation.connectedporc) + " | " +
						"nMU = " + str(simulation.nMU) + " | " + 
						"nRU = " + str(simulation.nRU) + " | " + 
						"w = " + str(simulation.w) + " | " + 
						"c1 = " + str(simulation.c1) + " | " + 
						"c2 = " + str(simulation.c2) + "\n")

	if os.path.exists(outpath):
		os.system("rm -rf " + outpath)
	os.mkdir(outpath)

	with open(outpath + "/" + CONST_DATA_FILE, "wb") as f:
		pickle.dump(results, f)

	return results

def getResultsFromFile(path):

	with open(path, "rb") as f:
		results = pickle.load(f)

	return results

def Plot(results, outpath):
	for nRU in results:

		# plot scatter
		def Scatter(connected):
			my_dpi=96
			fig = 	plt.figure()
			ax = 	fig.add_subplot(111, projection='3d')
			c = np.array(results[nRU][connected]["cost"])
			c_thr = 0.05*(max(c) - min(c))+min(c)
			i = np.where(c<c_thr)[0]
			x = results[nRU][connected]["w"]
			y = results[nRU][connected]["c1"]
			z = results[nRU][connected]["c2"]
			x_ = [x[j] for j in i]
			y_ = [y[j] for j in i]
			z_ = [z[j] for j in i]
			ax.scatter(x_,y_,z_)
			ax.set_xlabel("w")
			ax.set_ylabel("c1")
			ax.set_zlabel('c2')
			plt.show()

		def Plot3D(connected, par1, par2):
			my_dpi=96
			fig = 	plt.figure()
			ax = 	fig.add_subplot(111, projection='3d')
			x = results[nRU][connected][par1]
			y = results[nRU][connected][par2]
			z = results[nRU][connected]["cost"]
			ax.scatter(x,y,z)
			ax.set_xlabel(par1)
			ax.set_ylabel(par2)
			ax.set_zlabel('cost')
			plt.show()

		params = ["w", "c1", "c2"]
		connected = max(results[nRU].keys())
		c = results[nRU][connected]["cost"]
		cmin = min(c)
		i_cmin = [c.index(cmin)]
		
		print("Connected => " + str(connected))
		print("Min Cost => " + str(cmin))

		Scatter(connected)
		for i in range(len(params)):
			for j in range(i+1, len(params)):
				Plot3D(connected, params[i], params[j])

			param = params[i]
			p = results[nRU][connected][param]
			my_dpi=96
			fig = plt.figure(figsize=(480/my_dpi, 480/my_dpi), dpi=my_dpi)
			plt.title(param)
			x = results[nRU][connected]["cost"]
			val = np.array([c,p]).transpose()
			val = val[val[:,0].argsort()]
			plt.plot(val[:,0], val[:,1], label = str(connected))
			plt.legend()
			plt.savefig(outpath + "/" + "nRU-" + str(nRU) + "_Par-" + param + ".png", dpi=96)
			plt.close()

			print(param + " => Min cost at: " + str([p[j_cmin] for j_cmin in i_cmin]) + " | Mean: " + str(np.mean(p)))

	print("Partially done!")

def savePlotsFromFolder(path, outpath):

	lstLoadPath = []
	paths = os.listdir(path)
	for file in paths:
		if not (file == CONST_CFG_FILE):
			loadFile = path + file
			lstLoadPath.append(loadFile)

	if os.path.exists(outpath):
		os.system("rm -rf " + outpath)
	os.mkdir(outpath)

	results = {}

	for i in utils.progressbar(range(len(lstLoadPath)), "Saving"):

		loadFile = lstLoadPath[i]

		try:
			with open(loadFile, "rb") as f:
				dicSimulation = pickle.load(f)
		except Exception as e:
			continue
		else:
			pass
		finally:
			pass

		_replay = replay.Replay(	nameNodeC = dicSimulation["nameNodeC"],
						 			namesNodesMU = dicSimulation["namesNodesMU"],
						 			posNodesMU = dicSimulation["posNodesMU"],
						 			Nr = dicSimulation["namesNodesRU"],
						 			Nk = 300)
		cost = []
		for k in dicSimulation["P"]:
			cost.append([k, dicSimulation["P"][k]["P_Glob"].F.Connectivity, dicSimulation["P"][k]["P_Glob"].F.Cost])

		k = len(cost)-1

		filename = 	"_nRU" + str(dicSimulation["config"]["nRU"]) + \
					"_n" + str(dicSimulation["config"]["n"])
					
		_replay.PlotCost(cost, filename = outpath + "/cost_" + filename)
												 
		_replay.PlotParticle(	P = dicSimulation["P"][k]["P_Glob"], iNk = k,\
								filename = outpath + "/part_" + filename)

CONST_PLOT_FROM_FOLDER 	= "-f"
CONST_PLOT_FROM_FILE 	= "-p" 
CONST_PLOT_FROM_FOLDERS = "-fs"
CONST_PLOT_FROM_FILES 	= "-ps" 
CONST_SAVE_FROM_FOLDER	= "-s" 

if len(sys.argv) > 0:

	param 	= sys.argv[1]
	path 	= sys.argv[2]

	if param == CONST_PLOT_FROM_FOLDER:
		outpath = path + "/final"
		results = getResultsFromFolder(path, outpath)
		Plot(results, outpath)
	elif param == CONST_PLOT_FROM_FILE:
		outpath = os.path.dirname(path)
		results = getResultsFromFile(path)
		Plot(results, outpath)
	elif param == CONST_PLOT_FROM_FOLDERS:
		lstLoadPath = []
		folders = os.listdir(path)
		for folder in folders:
			loadPath = path + folder + "/"
			outpath = loadPath + "final"
			results = getResultsFromFolder(loadPath, outpath)
			Plot(results, outpath)
	elif param == CONST_PLOT_FROM_FILES:
		lstLoadPath = []
		folders = os.listdir(path)
		for folder in folders:
			outpath = path + "/" + folder + "/final"
			loadPath = outpath + "/" + CONST_DATA_FILE
			results = getResultsFromFile(loadPath)
			Plot(results, outpath)
	if param == CONST_SAVE_FROM_FOLDER:
		outpath = path + "/final"
		savePlotsFromFolder(path, outpath)

print("Done!")
