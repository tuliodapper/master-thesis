# -*- coding: utf-8 -*-
debug = False
# native lib
import sys
import os
import numpy as np
import utils

class Adjustment(object):

	def __init__(self, config):
		self.config = config

	def computeAdjustment(namesNodesMU, posNodesMU, namesNodesRU, posNodesRU, routes):

		names 		= np.concatenate((namesNodesMU, namesNodesRU), axis=0)
		pos 		= np.concatenate((posNodesMU, posNodesRU), axis=0)
		posNodesRU2 = np.copy(posNodesRU)

		# constants
		alpha 	= config["alpha"]
		gama 	= config["gama"]
		gama_r 	= config["gama_r"]

		# run through all nodes
		for i_node in range(len(namesNodesRU)):
			# initiate gradient
			gradiente = [0,0]
			# for each route
			for route in routes:
				try:
					# check if this node is in the route
					i_route = route.index(namesNodesRU[i_node])
					# if yes:
					Nr = len(route)
					# if it is not the first node
					if (i_route > 0):
						# busca pelo vizinho anterior
						i_name_neigh = np.where(names==route[i_route-1])[0][0]
						# soma calcula gradiente
						gradiente = gradiente + computeGradient(alpha, posNodesRU[i_node], pos[i_name_neigh])
					# if it is not the last node
					if (i_route < (Nr-1)):
						# busca pelo vizinho posterior
						i_name_neigh = np.where(names==route[i_route+1])[0][0]
						# soma calcula gradiente
						gradiente = gradiente + computeGradient(alpha, posNodesRU[i_node], pos[i_name_neigh])
				except Exception as e:
					pass
				else:
					pass
				finally:
					pass
			# computa abs
			s = ""
			distance 	= utils.computeDistance([0,0],gradiente)
			add 		= [0,0]
			if distance != 0:
				# multiply by a factor
				if (distance <= gama_r):
					add = gama*gradiente
					s 	= "gama*gradiente"
				else:
					add = gama_r*gradiente/distance
					s 	= "gama_r*gradiente/distance"
			posNodesRU2[i_node] = posNodesRU2[i_node] - add

			printAndSave("computeAdjustment -> distance: " + str(distance) + " -> add: " + str(utils.computeDistance([0,0],add)) + " -> " + s)

		return posNodesRU2

	def computeGradient(alpha, posR, posN):
		distance = utils.computeDistance(posR, posN)
		gradiente = alpha*(distance**(alpha-2))*(posR-posN)
		return gradiente

def printAndSave(line):
	if debug:
		print(line)
		# save(line)