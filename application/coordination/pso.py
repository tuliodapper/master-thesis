# -*- coding: utf-8 -*-
# native lib
import sys
import os
import numpy as np
import random
import copy
import time
import math
from matplotlib import pyplot 	as plt
from drawnow 	import drawnow, figure
import utils
import routing
import pickle
import pltWSN

P_Orig = 0	
P_Best = 1

class Particle:
	"""docstring for Particle"""
	def __init__(self, Xc, Xr, V, R, F):
		self.Xc = Xc
		self.Xr = Xr
		self.V = V
		self.R = R
		self.F = F

class Score:
	def __init__(self):
		self.connectivity 	= 0
		self.rangePenalty 	= 0
		self.safetyPenalty 	= 0
		self.distance 		= 0

		# old versions
		self.Cost 			= 0
		self.Connectivity 	= 0

class PSO(object):

	def __init__(self, namesNodesMU, posNodesMU, nRU, config, savePICAt = ""):

		self.namesNodesMU	= namesNodesMU
		self.posNodesMU 	= posNodesMU
		self.nRU			= nRU
		self.savePICAt 		= savePICAt
		self.config 		= config
		self.w 				= self.config["w"]
		self.c1 			= self.config["c1"]
		self.c2 			= self.config["c2"]
		self.nameNodeC 		= "c"
		self.Np 			= self.config["Np"]
		self.Nk 			= self.config["Nk"] 
		self.Vjmax_perc		= self.config["Vjmax_perc"]
		self.dcm 			= self.config["dcm"] 

		self.P  = []

	def setup(self):

		self.posMin 		= np.amin(self.posNodesMU,axis=0)
		self.posMax 		= np.amax(self.posNodesMU,axis=0)

		self.Nr = np.arange(self.nRU) 
		self.Nr = [str(c + 100) for c in self.Nr]

		self.Vjmax_x = self.Vjmax_perc*abs(self.posMax[0]-self.posMin[0])
		self.Vjmax_y = self.Vjmax_perc*abs(self.posMax[1]-self.posMin[1])

		if not type(self.w) is list:
			self.w_delta = 0
		else:
			self.w_delta = (self.w[0] - self.w[1])/self.Nk
			self.w = self.w[0]

		if self.savePICAt != "":
			self.dicSimulation = {}
			self.dicSimulation["version"] = 5
			self.dicSimulation["nameNodeC"] = self.nameNodeC
			self.dicSimulation["namesNodesMU"] = self.namesNodesMU
			self.dicSimulation["posNodesMU"] = self.posNodesMU
			self.dicSimulation["namesNodesRU"] = self.Nr
			self.dicSimulation["dcm"] = self.dcm
			self.dicSimulation["dim"] = self.config["dim"]
			self.dicSimulation["w"] = self.w
			self.dicSimulation["c1"] = self.c1
			self.dicSimulation["c2"] = self.c2
			self.dicSimulation["Nk"] = self.Nk
			self.dicSimulation["config"] = self.config
			self.dicSimulation["cost"] = []
			self.dicSimulation["P"] = {}

		self.iNk = 0

	def initialize(self):
		# LINE 2 -> 7
		j = 0
		# inicializacao de cada particula (Np particulas)
		for i in range(self.Np):
			# inicializacao de posicionamento do controlador
			Xc = np.random.uniform(self.posMin,self.posMax,2)
			# inicializacao de posicionamento de cada no (nRU nos)
			Xr = np.random.uniform(self.posMin,self.posMax,(self.nRU,2))
			# inicializacao da velocidade de atualizacao
			V = np.zeros((self.nRU+1,2))
			# roteamento para caso inicial
			R = routing.computeRouting(self.nameNodeC, Xc, self.namesNodesMU, self.posNodesMU, self.Nr, Xr, self.dcm)
			# min_fun
			F = self.computeScores(	self.nameNodeC, Xc,
									self.namesNodesMU, self.posNodesMU, 
									self.Nr, Xr,
									R)
			# adiciona particula (0:posição, 1:velocidade, 2:individual best-so-far, 3:roteamento, 4:funcao de minimizacao)
			Par = Particle(Xc,Xr,V,R,F)
			self.P.append([Par,copy.copy(Par)])
			if (i==0) or compareScore(self.Pg.F,self.P[i][P_Best].F):
				self.Pg = copy.copy(self.P[i][P_Best])

		self.P_Glob_ChangedAt = 0

	def iterate(self):

		if (self.iNk != self.Nk):
			if self.savePICAt != "":
				self.dicSimulation["P"][self.iNk] = {}

			j = 0
			for i in range(self.Np):
				u1 	= np.random.uniform(0,1,[self.nRU+1,2])
				u2 	= np.random.uniform(0,1,[self.nRU+1,2])

				X 		= np.concatenate((np.array([self.P[i][P_Orig].Xc]), self.P[i][P_Orig].Xr), axis=0)
				Xbest 	= np.concatenate((np.array([self.P[i][P_Best].Xc]), self.P[i][P_Best].Xr), axis=0)
				Xglob 	= np.concatenate((np.array([self.Pg.Xc]), self.Pg.Xr), axis=0)

				# Xbest	= utils.computeChoose(X, Xbest)
				# Xglob 	= utils.computeChoose(X, Xglob)

				V_ 	= self.w*self.P[i][P_Orig].V + np.multiply(self.c1*u1, Xbest-X) + np.multiply(self.c2*u2, Xglob-X)

				for iY in range(len(V_)):
					if abs(V_[iY][0]) > self.Vjmax_x:
						if V_[iY][0] > 0:
							V_[iY][0] = self.Vjmax_x
						else:
							V_[iY][0] = -self.Vjmax_x
					if abs(V_[iY][1]) > self.Vjmax_y:
						if V_[iY][1] > 0:
							V_[iY][1] = self.Vjmax_y
						else:	
							V_[iY][1] = -self.Vjmax_y

				X_ = X + V_

				for iX in range(len(X_)):
					if X_[iX][0] < self.posMin[0]:
						X_[iX][0] = self.posMin[0]
						V_[iX][0] = -V_[iX][0]
					elif X_[iX][0] > self.posMax[0]:
						X_[iX][0] = self.posMax[0]
						V_[iX][0] = -V_[iX][0]
					if X_[iX][1] < self.posMin[1]:
						X_[iX][1] = self.posMin[1]
						V_[iX][1] = -V_[iX][1]
					elif X_[iX][1] > self.posMax[1]:
						X_[iX][1] = self.posMax[1]
						V_[iX][1] = -V_[iX][1]

				self.P[i][P_Orig].Xc = np.copy(X_[0,:])
				self.P[i][P_Orig].Xr = np.copy(X_[1:len(X_),:])
				self.P[i][P_Orig].V = V_

				R = routing.computeRouting(	self.nameNodeC, self.P[i][P_Orig].Xc, 
											self.namesNodesMU, self.posNodesMU, 
											self.Nr, self.P[i][P_Orig].Xr, 
											self.dcm)
				self.P[i][P_Orig].R = R

				F = self.computeScores(	self.nameNodeC, self.P[i][P_Orig].Xc,
										self.namesNodesMU, self.posNodesMU,
										self.Nr, self.P[i][P_Orig].Xr,
										self.P[i][P_Orig].R)


				self.P[i][P_Orig].F = F

				P_Best_Changed = False
				if compareScore(self.P[i][P_Best].F, F):
					self.P[i][P_Best] = copy.copy(self.P[i][P_Orig])
					P_Best_Changed = True
				if i!=0:
					if compareScore(self.P[j][P_Best].F, self.P[i][P_Best].F):
						j=i

				if self.savePICAt != "":
					self.dicSimulation["P"][self.iNk][i] = {}
					self.dicSimulation["P"][self.iNk][i]["P_Glob"] 			= copy.copy(self.Pg)
					self.dicSimulation["P"][self.iNk][i]["P_Orig"] 			= copy.copy(self.P[i][P_Orig])
					self.dicSimulation["P"][self.iNk][i]["P_Best"] 			= copy.copy(self.P[i][P_Best])
					self.dicSimulation["P"][self.iNk][i]["P_Best_Changed"] 	= P_Best_Changed

			self.w = self.w - self.w_delta

			P_Glob_Changed = False
			P_Glob_ChangedBy = -1
			if compareScore(self.Pg.F, self.P[j][P_Best].F):
				self.Pg  = copy.copy(self.P[j][P_Best])
				P_Glob_Changed = True
				P_Glob_ChangedBy = j
				self.P_Glob_ChangedAt = self.iNk+1
			
			if self.savePICAt != "":
				self.dicSimulation["P"][self.iNk]["P_Glob"] 			= copy.copy(self.Pg)
				self.dicSimulation["P"][self.iNk]["P_Glob_Changed"] 	= P_Glob_Changed
				self.dicSimulation["P"][self.iNk]["P_Glob_ChangedBy"] 	= P_Glob_ChangedBy
				self.dicSimulation["cost"].append([self.Pg.F.Connectivity, self.Pg.F.Cost])

			self.iNk = self.iNk + 1

			if (self.iNk == self.Nk):
				if self.savePICAt != "":	
					self.dicSimulation["P_Glob"] = copy.copy(self.Pg)
					self.dicSimulation["P_Glob_ChangedAt"] = self.P_Glob_ChangedAt
					with open(self.savePICAt, "wb") as f:
						pickle.dump(self.dicSimulation, f)

		return (self.iNk == self.Nk)

	# compute score
	def computeScores(	self,
						nameNodeC, posNodeC,
						namesNodesMU, posNodesMU,
						namesNodesRU, posNodesRU,
						routes):

		namesNodes 		= utils.concatenateNames(nameNodeC, namesNodesMU, namesNodesRU)
		posNodes 		= utils.concatenatePos(posNodeC, posNodesMU, posNodesRU)

		# Distances
		dsf				= self.config["dsf"]
		dcm 			= self.config["dcm"]
		# Penality coefficients
		u_distance		= self.config["u_distance"]
		u_distance_pow	= self.config["u_distance_pow"]
		u_range			= self.config["u_range"]
		u_range_pow		= self.config["u_range_pow"]
		u_security 		= self.config["u_security"]

		score = Score()

		# route analysis
		for route in routes:
			nr = len(route)
			if nr > 0:
				if ((route[0] in namesNodesMU) or (route[-1] in namesNodesMU)):
					max_distance = 0
					for iNode in range(nr-1):
						posNode1 = posNodes[namesNodes.index(str(route[iNode]))]
						posNode2 = posNodes[namesNodes.index(str(route[iNode+1]))]
						distance = utils.computeDistance(posNode1, posNode2)
						score.distance = score.distance + u_distance*(distance**u_distance_pow)
						if distance > max_distance:
							max_distance = distance
					aux = max_distance - dcm
					if (aux > 0):
						score.rangePenalty 	= score.rangePenalty + u_range*(aux**u_range_pow)
					elif ((route[0] in namesNodesMU) and (route[-1] in namesNodesMU)):
						score.connectivity 	= score.connectivity + 1
		
		# node distances
		min_distance 	= None
		for nameNode1 in namesNodes:
			for nameNode2 in namesNodes:
				if (nameNode1 != nameNode2):
					posNode1 = posNodes[namesNodes.index(nameNode1)]
					posNode2 = posNodes[namesNodes.index(nameNode2)]
					distance = utils.computeDistance(posNode1, posNode2)
					if (min_distance is None) or (distance < min_distance):
						min_distance = distance
		aux = dsf - min_distance
		if aux > 0:
			score.safetyPenalty = u_security*(aux**2)

		# old versions
		score.Cost 			= score.distance + score.rangePenalty + score.safetyPenalty
		score.Connectivity 	= score.connectivity

		return score

def computePSO(	namesNodesMU, posNodesMU, nRU, 
				config, savePICAt = ""):

	_pso = PSO(	namesNodesMU = namesNodesMU, posNodesMU = posNodesMU, nRU = nRU,
				config = config, savePICAt = savePICAt)
	_pso.setup()
	_pso.initialize()
	while (not _pso.iterate()):
		pass
		
	return _pso.Pg.Xc, _pso.Pg.Xr

def computeScoresData(	namesNodesMU, posNodesMU,
						namesNodesRU, posNodesRU,
						routes, dcm):

	return computeScores(	None, None,
							namesNodesMU, posNodesMU,
							namesNodesRU, posNodesRU,
							routes, dcm)
# compare score
def compareScore(oldScore, newScore):
	# return (not (newScore.connectivity < oldScore.connectivity)) and (newScore.Cost < oldScore.Cost)
	return (newScore.Cost < oldScore.Cost)

if __name__ == '__main__':

	namesUN = ["u1","u2","u3","u4"]
	posUN	= np.array([[300, 300], [300, 1200], [1200, 300], [1200, 1200]])
	computePSO(namesUN, posUN, 30, utils.getStandardConfig(), savePICAt = "/home/tulio/Desktop/result.pickle")