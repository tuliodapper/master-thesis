# -*- coding: utf-8 -*-
debug = False

# own libraries
import os
import sys
filepath = os.path.dirname(os.path.abspath(__file__))
filedata = "/sim-top.ini"

import numpy as np
from dijkstar import Graph, find_path
import utils
import copy

def save(line):
	global filepath
	global filedata
	if debug:
		line = str(line)
		f_d = open(filepath + filedata, "a") 
		f_d.write(line + "\n")
		f_d.close()

def printAndSave(line):
	if debug:
		print(line)
		save(line)

def computeRouting(	nameNodeC, posNodeC,
					namesNodesMU, posNodesMU,
					namesNodesRU, posNodesRU,
					dcm):

	routesData 		= computeRoutingData(	namesNodesMU, posNodesMU, \
											namesNodesRU, posNodesRU, dcm)

	routesControl 	= computeRoutingControl(nameNodeC, posNodeC, \
											namesNodesRU, posNodesRU, dcm)

	routes = routesData + routesControl

	return routes

def computeRoutingData(	namesNodesMU, posNodesMU,
						namesNodesRU, posNodesRU,
						dcm):

	names 		= utils.concatenateNames(None, namesNodesMU, namesNodesRU)
	location 	= utils.concatenatePos(None, posNodesMU, posNodesRU)

	cost_func = lambda u, v, e, prev_e: e['cost']
	graph 	= Graph()
	routes 	= []

	for i1 in namesNodesMU:
		id1 = namesNodesMU.index(i1)
		min_distance = None
		for i2 in namesNodesRU:
			id2 = namesNodesRU.index(i2)
			distance = utils.computeDistance(posNodesMU[id1,:], posNodesRU[id2,:])
			if ((min_distance is None) or (distance < min_distance)):
				min_distance = distance
				id2_min = id2
		graph.add_edge(i1, namesNodesRU[id2_min], {'cost': distance})
		graph.add_edge(namesNodesRU[id2_min], i1, {'cost': distance})

	for i1 in namesNodesRU:
		id1 = namesNodesRU.index(i1)
		for i2 in namesNodesRU:
			if (i1 != i2):
				id2 = namesNodesRU.index(i2)
				distance = utils.computeDistance(posNodesRU[id1,:], posNodesRU[id2,:])
				if ((dcm is not None) and (distance > dcm)):
					distance = distance**2
				graph.add_edge(i1, i2, {'cost': distance})
				graph.add_edge(i2, i1, {'cost': distance})

	nMU = len(namesNodesMU)
	for i1 in range(0, nMU):
		for i2 in range(i1, nMU):
			if (i1 != i2):
				try:
					path = find_path(graph, namesNodesMU[i1], namesNodesMU[i2], cost_func=cost_func)
					if (len(path.nodes) > 0) and (len(set(path.nodes[1:(len(path.nodes)-1)]).intersection(namesNodesMU)) == 0):
						routes.append(path.nodes)
				except Exception as e:
					pass

	return routes

def computeRoutingControl(	nameNodeC, posNodeC,
							namesNodesRU, posNodesRU,
							dcm):

	names 		= utils.concatenateNames(nameNodeC, None, namesNodesRU)
	location 	= utils.concatenatePos(posNodeC, None, posNodesRU)

	cost_func = lambda u, v, e, prev_e: e['cost']
	graph  = Graph()
	routes = [] 

	for i1 in names:
		for i2 in names:
			if (i1 != i2):
				id1 = names.index(i1)
				id2 = names.index(i2)
				distance = utils.computeDistance(location[id1,:], location[id2,:])
				if ((dcm is not None) and (distance > dcm)):
					distance = distance**2
				graph.add_edge(i1, i2, {'cost': distance})

	for i1 in names:
		if (i1 != nameNodeC):
			try:
				path = find_path(graph, nameNodeC, i1, cost_func=cost_func)
				if len(path.nodes) > 0:
					routes.append(path.nodes)
			except Exception as e:
				pass

	return routes

def cleanRoutes2(	nameNodeC, posNodeC,
					namesNodesMU, posNodesMU, 
					namesNodesRU, posNodesRU, 
					routes, dcm):

	namesNodesRU 	= utils.concatenateNames(nameNodeC, namesNodesRU, None)
	posNodesRU 		= utils.concatenatePos(posNodeC, posNodesRU, None)

	return cleanRoutes(namesNodesMU, posNodesMU, namesNodesRU, posNodesRU, routes, dcm)

def computeConnections(	nameNodeC, posNodeC,
						namesNodesMU, posNodesMU, 
						namesNodesRU, posNodesRU, 
						routes, dcm):

	namesNodes 	= utils.concatenateNames(nameNodeC, namesNodesMU, namesNodesRU)
	posNodes 	= utils.concatenatePos(posNodeC, posNodesMU, posNodesRU)
	
	connections = 0
	
	for route in routes:
		if (route[0] in namesNodesMU) or (route[-1] in namesNodesMU):
			add = True
			for iNode in range(len(route)-1):
				i1 = namesNodes.index(str(route[iNode]))
				i2 = namesNodes.index(str(route[iNode+1]))
				posNode1 = posNodes[i1]
				posNode2 = posNodes[i2]
				nameNode1 = namesNodes[i1]
				nameNode2 = namesNodes[i2]
				distance = utils.computeDistance(posNode1, posNode2)
				if distance > dcm:
					add = False
					break
			if add:
				connections = connections + 1

	return connections

def cleanRoutes(namesNodesMU, posNodesMU, namesNodesRU, posNodesRU, routes, dcm):

	namesNodes 	= namesNodesMU + namesNodesRU
	posNodes 	= np.concatenate((posNodesMU, posNodesRU), axis=0)
	
	routes = copy.copy(routes)

	iRoute = 0
	
	printAndSave("cleanRoutes:")
	
	for route in range(len(routes)):
		bDel = False
		nameNodeFrom 	= str(routes[iRoute][0])
		nameNodeTo 		= str(routes[iRoute][len(routes[iRoute])-1])
		for iNode in range(len(routes[iRoute])-1):
			i1 = namesNodes.index(str(routes[iRoute][iNode]))
			i2 = namesNodes.index(str(routes[iRoute][iNode+1]))
			posNode1 = posNodes[i1]
			posNode2 = posNodes[i2]
			nameNode1 = namesNodes[i1]
			nameNode2 = namesNodes[i2]
			distance = utils.computeDistance(posNode1, posNode2)
			if distance > dcm:
				printAndSave("delete route =>  F: " + nameNodeFrom + " - T: " + nameNodeTo + " | D: " + str(distance) + " - F: " + nameNode1 + " - T: " + nameNode2)
				bDel = True
		if bDel:
			routes.pop(iRoute)
		else:
			iRoute=iRoute+1

	return routes
	
def printRoutes(namesNodesMU, posNodesMU, namesNodesRU, posNodesRU, routes):

	namesNodes 	= namesNodesMU + namesNodesRU
	posNodes 	= np.concatenate((posNodesMU, posNodesRU), axis=0)
	
	routes = copy.copy(routes)

	printAndSave("routes:")
	for iRoute in range(len(routes)):
		s 	= str(routes[iRoute][0])
		for iNode in range(len(routes[iRoute])-1):
			i1 = namesNodes.index(str(routes[iRoute][iNode]))
			i2 = namesNodes.index(str(routes[iRoute][iNode+1]))
			posNode1 = posNodes[i1]
			posNode2 = posNodes[i2]
			nameNode2 = namesNodes[i2]
			distance = utils.computeDistance(posNode1, posNode2)
			s = s + " - " + str(distance) + " - " + nameNode2
		printAndSave(s)

def convertRoute(routes):

	routes2 = []
	routes3 = []

	for route in routes:
		n = len(route)
		for iNode in range(n):
			if (iNode < (n-1)):
				routes2.append([route[iNode], route[iNode+1], route[(n-1)]])
			if (iNode > 0):
				routes2.append([route[iNode], route[iNode-1], route[0]])

	for route in routes2:
		if route not in routes3:
			routes3.append(route)

	return routes3

def connectedMU(namesNodesMU, posNodesMU, namesNodesRU, posNodesRU, routes, dcm):

	cleanedRoutes = cleanRoutes(namesNodesMU, posNodesMU, namesNodesRU, posNodesRU, routes, dcm)

	connNamesNodesMU = []

	for route in routes:
		try:
			i_node = namesNodesMU.index(route[0])
			try:
				i = connNamesNodesMU.index(namesNodesMU[i_node])
			except Exception as e:
				connNamesNodesMU.append(namesNodesMU[i_node])
		except Exception as e:
			pass
		try:
			i_node = namesNodesMU.index(route[-1])
			try:
				i = connNamesNodesMU.index(namesNodesMU[i_node])
			except Exception as e:
				connNamesNodesMU.append(namesNodesMU[i_node])
		except Exception as e:
			pass
	return connNamesNodesMU

def getIdealConnection(nMU, nRU):
	ret = getIdealConnectionData(nMU)
	if nRU > 0:
		ret = ret + getIdealConnectionControl(nMU, nRU)
	return ret

def getIdealConnectionData(nMU):
	return (nMU*(nMU-1))/2

def getIdealConnectionControl(nMU, nRU):
	return (nMU+nRU-1)
	
def processRoute(routes):

	routes2 = []

	for route in routes:
		n = len(route)
		for iNode in range(n):
			if (iNode < (n-1)):
				routes2.append({"nodeId": 		route[iNode],
								"nextHop": 		route[iNode+1],
								"destination": 	route[-1]})
			if (iNode > 0):
				routes2.append({"nodeId": 		route[iNode],
								"nextHop": 		route[iNode-1],
								"destination": 	route[0]})
				
	return [dict(t) for t in {tuple(d.items()) for d in routes2}]

if __name__ == '__main__':
	
	scenario = 8

	dcm = 55
	dim = 200

	if scenario == 1:

		nameNodeC		= "0"
		posNodeC 		= np.array([106, 120])

		namesNodesRU	= [str(c) for c in range(1, 12)]
		posNodesRU		= np.array(	[[130,	145	],
									 [85,	60	],
									 [110,	81	],
									 [160,	150	],
									 [125,	50	],
									 [50,	150	],
									 [84,	133	],
									 [120,	175	],
									 [ 58,	32	],
									 [128,	110	],
									 [ 28,	170	]])

		namesNodesMU	= [str(c) for c in range(12, 27)]
		posNodesMU		= np.array(	[[ 74,	152	],
									 [ 	2,	158	],
									 [109,	62	],
									 [ 85,	121	],
									 [132,	95	],
									 [ 38,	195	],
									 [101,	95	],
									 [175,	135	],
									 [170,	180	],
									 [ 74,	25	],
									 [109, 	10 	],
									 [140,	15 	],
									 [ 25,	5	],
									 [ 91,	189	],
									 [160,	50	]])
	elif scenario == 2:

		nameNodeC		= "0"
		posNodeC 		= np.array([106, 120])

		namesNodesRU	= [str(c) for c in range(1, 12)]
		posNodesRU		= np.array(	[[130,	145	],
									 [85,	60	],
									 [110,	81	],
									 [160,	150	],
									 [125,	50	],
									 [50,	150	],
									 [84,	133	],
									 [120,	175	],
									 [ 58,	32	],
									 [128,	110	],
									 [ 28,	170	]])

		namesNodesMU	= [str(c) for c in range(12, 42)]
		posNodesMU		= np.array(	[[ 74,	152	],
									 [ 	2,	158	],
									 [109,	62	],
									 [ 85,	121	],
									 [132,	95	],
									 [ 38,	195	],
									 [101,	95	],
									 [175,	135	],
									 [170,	180	],
									 [ 74,	25	],
									 [109, 	10 	],
									 [140,	15 	],
									 [ 25,	5	],
									 [ 91,	189	],
									 [72,	131	],
									 [25,	40	],
									 [56,	50	],
									 [49,	21	],
									 [117,	192],
									[131,	177],
									[182,	160],
									[110,	168],
									[127,34],
									[12,177],
									[27,160],
									[20,190],
									[157,164],
									[158,141],
									[107,180],
									[132,192]])
	elif scenario == 3:

		nameNodeC		= "0"
		posNodeC 		= np.array([106, 120])

		namesNodesRU	= [str(c) for c in range(1, 12)]
		posNodesRU		= np.array(	[[130,	145	],
									 [85,	60	],
									 [110,	81	],
									 [160,	150	],
									 [125,	50	],
									 [50,	150	],
									 [84,	133	],
									 [120,	175	],
									 [ 58,	32	],
									 [128,	110	],
									 [ 28,	170	]])
		
		namesNodesMU	= [str(c) for c in range(12, 57)]
		posNodesMU		= np.array(	[[ 74,	152	],
									 [ 	2,	158	],
									 [109,	62	],
									 [ 85,	121	],
									 [132,	95	],
									 [ 38,	195	],
									 [101,	95	],
									 [175,	135	],
									 [170,	180	],
									 [ 74,	25	],
									 [109, 	10 	],
									 [140,	15 	],
									 [ 25,	5	],
									 [ 91,	189	],
									 [72,	131	],
									 [25,	40	],
									 [56,	50	],
									 [49,	21	],
									 [117,	192],
									[131,	177],
									[182,	160],
									[110,	168],
									[127,	34],
									[12,	177],
									[27,	160],
									[20,	190],
									[157,	164],
									[158,	141],
									[107,	180],
									[132,	192],
									[94,	148],
									[97,	138],
									[93,	158],
									[120,	151],
									[132,	129],
									[143,	123],
									[118,	102],
									[97,	80],
									[79,	49],
									[134,	67],
									[51,	170],
									[55, 	140],
									[63, 	170],
									[149,	109],
									[40, 	150]])
	elif scenario == 6:

		nameNodeC		= "0"
		posNodeC 		= np.array([95,  160])
		
		posNodesRU		= np.array(	[[30,	40], # 1
									 [30,	80], # 2
									 [70,	100], # 3
									 [100,	130], # 4
									 [145,	110], # 5
									 [190,	95], # 6
									 #[190,	70], # 7
									 ])

		nRU = len(posNodesRU)

		namesNodesRU	= [str(c) for c in range(1, nRU+1)]
		
		posNodesMU		= np.array(	[[0,	10	],
									 [60,	0	],
									 [0,	100	],
									 #[95,  160 ],
									 [95, 	60	],
									 [160, 	150	],
									 [200, 	60	],
									 ])



		nMU = len(posNodesMU)

		namesNodesMU	= [str(c) for c in range(nRU+1, nRU+nMU+1)]

	elif scenario == 7:

		nameNodeC		= "0"
		posNodeC 		= np.array([95,  160])
		
		posNodesRU		= np.array(	[[30,	30 ], # 1
									 [30,	70 ], # 2
									 [60,	85], # 3
									 [75,	110], # 4
									 [115,	110], # 5
									 [150,	105 ], # 6
									 #[190,	70 ], # 7
									 ])

		nRU = len(posNodesRU)

		namesNodesRU	= [str(c) for c in range(1, nRU+1)]
		
		posNodesMU		= np.array(	[[10,	10	],
									 [70,	0	],
									 [0,	100	],
									 #[95,  160 ],
									 #[85, 	60	],
									 [160, 	150	],
									 [180, 	95	],
									 ])

		nMU = len(posNodesMU)

		namesNodesMU	= [str(c) for c in range(nRU+1, nRU+nMU+1)]

	elif scenario == 8:

		nameNodeC		= "0"
		posNodeC 		= np.array([100,  100])
		
		posNodesRU		= np.array(	[[20,	15 ], # 1
									 [30,	55 ], # 2
									 [75,	60 ], # 3
									 [85,	20 ], # 4
									 [155,	10 ], # 5
									 [150,	50 ], # 6
									 [110,	70 ], # 7
									 [170,	80 ], # 8
									 [160,	110], # 9
									 [170,	155], # 10
									 [115,	110], # 11
									 [90,	130], # 12
									 [20,	90 ], # 13
									 [20,	135], # 14
									 [55,	150], # 15
									 ])

		nRU = len(posNodesRU)

		namesNodesRU	= [str(c) for c in range(1, nRU+1)]
		
		posNodesMU		= np.array(	[[0,	0	], # 16
									 [0,	75	], # 17
									 [0,	180	], # 18
									 [95,   170 ], # 19
									 [85, 	0	], # 20
									 [160, 	0	], # 21
									 [180, 	195	], # 22
									 [180, 	95	], # 23
									 ])

		nMU = len(posNodesMU)

		namesNodesMU	= [str(c) for c in range(nRU+1, nRU+nMU+1)]

	routes = computeRouting(nameNodeC, posNodeC,
							namesNodesMU, posNodesMU,
							namesNodesRU, posNodesRU,
							dcm)

	print(routes)
