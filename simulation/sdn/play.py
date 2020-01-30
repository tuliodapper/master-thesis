 # -*- coding: utf-8 -*-
import os
import random
import pickle
import subprocess
import numpy as np
import multiprocessing
from time import localtime, strftime, sleep
import connection

simfile = "sim-config.ini"
cfgfile = "sim-config.pickle"
mobfile = "sim-mob.ini"
msgfile = "sim-msg.ini"
topfile = "sim-top.ini"
dbgfile = "M3WSN-Debug.txt"
ou1file = "M3WSN-Output.txt"
ou2file = "output.txt"
runfile = "run.sh"

def ExecuteAndWait(command):
	# start omnet simulation
	process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
	# check end omnet simulation
	for line in process.stdout:
		print line

def ExecuteFromConfig(config):

	savepath 	= config["savepath"]
	loadpath 	= config["loadpath"]
	simpath 	= config["simpath"]
	rootpath	= config["rootpath"]
	simfilestd	= config["simfilestd"]

	# save config at cfgfile file
	with open(simpath+cfgfile, "wb") as f:
		pickle.dump(config, f)

	# read standard simulation ini file
	with open(simfilestd, "U") as f:
		newText=f.read()

	# replace key values
	for key in config:
		newText=newText.replace("$" + key, str(config[key]))

	# set random speeds to user nodes
	#for i in range(config["firstUserNodeId"], config["lastUserNodeId"]+1):
	#	speed 	= random.uniform(config["speedUserNodeMin"], config["speedUserNodeMax"])
	#	newText = newText + "\nSN.node[" + str(i) + "].MobilityManager.speed = " + str(speed)
	
	# set initial user nodes positions
	for name, position in 	zip(config["namesNodes"], list(config["posNodes"])):
		
		locationX = position[0]
		locationY = position[1]
		#newText = newText + "\nSN.node[" + str(name) + "].MobilityManager.xCoorDestination = " + str(locationX)
		#newText = newText + "\nSN.node[" + str(name) + "].MobilityManager.yCoorDestination = " + str(locationY)
		newText = newText + "\nSN.node[" + str(name) + "].xCoor = " + str(locationX)
		newText = newText + "\nSN.node[" + str(name) + "].yCoor = " + str(locationY)

	# write output simulation ini file 
	with open(simpath+simfile, "w+") as f:
		f.write(newText)

	# create mobility information
	# print("create mobility information")
	if config["loadpath"] == "":
		with open(simpath + mobfile, "w+") as f:
			j = 0
			t = 0
			d = 10
			while (t<config["simTimeLimit"]):
				for i in range(config["firstUserNodeId"], config["lastUserNodeId"]+1):
					x = random.uniform(0, config["fieldX"])
					y = random.uniform(0, config["fieldY"])
					f.write(str(j) + ";" + str(i) + ";" + str(d) + ";" + str(x) + ";" + str(y) + "\n")
				d = random.uniform(config["mobilityIntervalMin"], config["mobilityIntervalMax"])
				t=t+d
				j=j+1 
	else:
		os.system("cp " + loadpath + mobfile + " " + simpath + mobfile)

	# create message information
	# print("create message information")
	if config["loadpath"] == "":
		with open(simpath + msgfile, "w+") as f:
			t = 20
			for i in range(config["firstUserNodeId"], config["lastUserNodeId"]+1, 2):
				if (t>config["simTimeLimit"]):
					break
				source = i
				destination = i+1
				f.write("0;" + str(source) + ";" + str(t) + ";" + str(destination) + ";1024;1;1\n")
				t=t+60
	elif os.path.isfile(loadpath + msgfile):
		os.system("cp " + loadpath + msgfile + " " + simpath + msgfile)
	
	storepath = Execute(simpath, rootpath, savepath, config)

	return storepath

def Execute(simpath, rootpath, savepath, config):

	# create savepath
	if not os.path.exists(savepath):
		os.mkdir(savepath)
		print("Result folder created at " + savepath)

	# copy and replace variables run.sh
	with open(runfile, "U") as f:
		newText=f.read()

	newText=newText.replace("$root", 	rootpath)
	newText=newText.replace("$sim", 	simpath)

	with open(simpath + runfile, "w+") as f:
		f.write(newText)

	# remove run files
	ExecuteAndWait("cd " + simpath + " && sh clean.sh")

	config2 = {}
	config2["config"] = config

	if config["name"] == "SDN":
		# start controller code in python
		process2 = multiprocessing.Process(target=connection.ListenMultiple, args=(config,))
		process2.start()

	# remove run files
	ExecuteAndWait("sh " + simpath + "run.sh")

	if config["name"] == "SDN":
		# terminate controller process
		process2.terminate()

	# create storepath
	storepath = strftime("%Y-%m-%d-%H-%M-%S", localtime())
	storepath = savepath + storepath + "/"
	videopath = storepath + "videoTrace/"
	if not os.path.exists(storepath):
		os.mkdir(storepath)
	if not os.path.exists(videopath):
		os.mkdir(videopath)

	# copy files to storepath
	os.system("cp " + simpath + cfgfile + " " + storepath + cfgfile)
	os.system("cp " + simpath + simfile + " " + storepath + simfile)
	os.system("cp " + simpath + "rd_* " + videopath)
	os.system("cp " + simpath + "sd_* " + videopath)
	if os.path.isfile(simpath + mobfile):
		os.system("cp " + simpath + mobfile + " " + storepath + mobfile)
	if os.path.isfile(simpath + msgfile):
		os.system("cp " + simpath + msgfile + " " + storepath + msgfile)
	if os.path.isfile(simpath + topfile):
		os.system("cp " + simpath + topfile + " " + storepath + topfile)
	if os.path.isfile(simpath + dbgfile):
		os.system("cp " + simpath + dbgfile + " " + storepath + dbgfile)
	if os.path.isfile(simpath + ou1file):
		os.system("cp " + simpath + ou1file + " " + storepath + ou1file)
	if os.path.isfile(simpath + ou2file):
		os.system("cp " + simpath + ou2file + " " + storepath + ou2file)

	# done
	print("Simulation completed & stored at " + storepath + ".")

	return storepath