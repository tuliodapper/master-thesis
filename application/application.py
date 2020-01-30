#! /usr/bin/python
import copy
import numpy as np
import contract_in_pb2
import contract_out_pb2
from coordination import topology
from coordination import pltWSN
from coordination import utils
from coordination import routing2 as routing
from matplotlib import pyplot 	as plt
from drawnow 	import drawnow, figure

def sortBySource(e):
	return e['source']

def sortByIdVideo(e):
	return e['idVideo']

class Application(object):
	"""docstring for ClassName"""
	def __init__(self, nodeId, config):

		self.nodeId = str(nodeId)

		self.config = config

		self.active = (self.nodeId == config["nameCN"])

		self.routes_active = []

		if (self.nodeId == config["nameCN"]):
		 	self.coordination 	= topology.Coordination(config)
		 	self.routing 		= routing.Routing(config)

		self.transmissions = []

	def initialize(self):

		contractOut = contract_out_pb2.ContractOut()

		contractOut.isACK = True

		routes_target = copy.copy(routing.processRoute(self.config["routes"]))

		for route_target in routes_target:

			if ((	(str(route_target["nodeId"]) in self.config["namesRN"]) and \
					(str(route_target["destination"]) == self.config["nameCN"])) or
				(	(str(route_target["nodeId"]) in self.config["namesRN"]) and \
					(str(route_target["destination"]) in self.config["namesRN"])) or
				(	(str(route_target["nodeId"]) in self.config["namesRN"]) and \
				  	(str(route_target["destination"]) in self.config["namesUN"]) and \
				  	(str(route_target["nextHop"]) in self.config["namesUN"])) or
				(	(str(route_target["nodeId"]) == self.config["nameCN"]) and \
				 	(str(route_target["destination"]) in self.config["namesRN"]))):

				if 	(int(self.nodeId) == int(self.config["nameCN"])):

					self.updateRoutesActive(nodeId = route_target["nodeId"],
											nextHop = route_target["nextHop"],
											destination = route_target["destination"])

				if 	(int(self.nodeId) == int(route_target["nodeId"])):

					new = contractOut.updateRoutingTable.add()
					new.relayId = int(route_target["nodeId"])
					new.route.nextHop = int(route_target["nextHop"])
					new.route.destination = int(route_target["destination"])
					new.route.drop = False
					new.action = contract_out_pb2.ContractOut.Action.ADD

			if(	(int(self.nodeId) == int(route_target["nodeId"])) and
				(str(self.nodeId) in self.config["namesRN"]) 	and 
				(str(route_target["destination"]) in self.config["namesUN"]) and 
				(str(route_target["nextHop"]) in self.config["namesUN"]) and 
				(int(route_target["nodeId"]) == int(self.nodeId))):

				new = contractOut.updateUser.add()
				new.relayId = int(route_target["nodeId"])
				new.userId 	= int(route_target["destination"])
				new.action 	= contract_out_pb2.ContractOut.Action.ADD
				new.event 	= contract_out_pb2.ContractOut.Event.REQUIRED

		self.printUpdateRoutingTable(contractOut.updateRoutingTable)
		self.printUpdateUser(contractOut.updateUser)

		return contractOut.SerializeToString()

	def request(self, data):
		# print("Receiving  ContractIn...")
		contractIn 	= contract_in_pb2.ContractIn()
		# print("Read from data: " + data)
		contractIn.ParseFromString(data)

		time = float(contractIn.time)
		# print("ContractIn parsed!")

		# print("relayId: " + str(contractIn.relayId))
		# print("userId: " + str(contractIn.userId))
		print(" > TIME: " + str(time))

		contractOut = contract_out_pb2.ContractOut()
		contractOut.isACK = True

		if (contractIn.contractInPacket == contract_in_pb2.ContractIn.ContractInPacket.NOTIFY_STATUS_USER):
			# print('_CONTRACTIN_NOTIFYSTATUS_USER')
			pos = np.array([float(contractIn.msgnotifystatususer.status.location.x),
							float(contractIn.msgnotifystatususer.status.location.y)])
			self.coordination.notifyStatusUser(contractIn.userId, pos)

		elif (contractIn.contractInPacket == contract_in_pb2.ContractIn.ContractInPacket.REGISTER_USER):
			# print('_CONTRACTIN_REGISTER_USER')
			self.processRegisterUser(contractOut.updateUser, contractIn.relayId, contractIn.userId)

		elif (contractIn.contractInPacket == contract_in_pb2.ContractIn.ContractInPacket.UNREGISTER_USER):
			pass
			# print('_CONTRACTIN_UNREGISTER_USER')

		if (contractIn.contractInPacket == contract_in_pb2.ContractIn.ContractInPacket.NOTIFY_STATUS_RELAY):
			# print('_CONTRACTIN_NOTIFYSTATUS_RELAY')
			pos = np.array([float(contractIn.msgnotifystatusrelay.status.location.x),
							float(contractIn.msgnotifystatusrelay.status.location.y)])
			self.coordination.notifyStatusRelay(contractIn.relayId, pos)

		elif (contractIn.contractInPacket == contract_in_pb2.ContractIn.ContractInPacket.REGISTER_RELAY):
			# print('_CONTRACTIN_REGISTER_RELAY')
			self.coordination.registerRelay(contractIn.relayId)
			# self.routes_active.append([contractIn.relayId, '0', '0'])
			# self.routes_active.append(['0', contractIn.relayId, contractIn.relayId])

		if (contractIn.contractInPacket == contract_in_pb2.ContractIn.ContractInPacket.NOTIFY_STATUS_CONTROLLER):
			# print('_CONTRACTIN_NOTIFYSTATUS_CONTROLLER')
			pos = np.array([float(contractIn.msgNotifyStatusController.status.location.x), 
							float(contractIn.msgNotifyStatusController.status.location.y)])
			self.coordination.notifyStatusController(pos)

		elif (contractIn.contractInPacket == contract_in_pb2.ContractIn.ContractInPacket.REQUEST):
			pass
			# print('_CONTRACTIN_REQUEST')
			# self.coordination.computeTopology(float(contractIn.time))
			# self.processUpdateRoutingTable(contractOut.updateRoutingTable)
			# self.processUpdateArea(contractOut.updateArea)
			# self.plot()

		elif (contractIn.contractInPacket == contract_in_pb2.ContractIn.ContractInPacket.PACKET_IN):
			print('_CONTRACTIN_PACKET_IN')
			self.processPacketIn(contractOut.updateRoutingTable, contractIn.relayId, contractIn.userId)
			
		elif (contractIn.contractInPacket == contract_in_pb2.ContractIn.ContractInPacket.TELEMETRY):

			print('_CONTRACTIN_TELEMETRY')

			source 			= contractIn.msgTelemetry.source - 1
			destination 	= contractIn.msgTelemetry.destination - 1
			messageType 	= contractIn.msgTelemetry.messageType - 1
			size 			= contractIn.msgTelemetry.size - 1
			idVideo			= contractIn.msgTelemetry.idVideo - 1
			timeStarted 	= float(contractIn.msgTelemetry.msgTelemetryByHop[0].timeReceived)

			for index, transmission in enumerate(self.transmissions):
				if 	(transmission["source"] 		== source) and \
					(transmission["destination"] 	== destination) and \
					(transmission["messageType"] 	== messageType):
					del self.transmissions[index]
					break

			hops = []
			for msgTelemetryByHop in contractIn.msgTelemetry.msgTelemetryByHop:
				relayId 		= msgTelemetryByHop.relayId - 1
				timeReceived 	= float(msgTelemetryByHop.timeReceived)
				lenQueueMac 	= msgTelemetryByHop.lenQueueMac - 1
				lenQueueNet 	= msgTelemetryByHop.lenQueueNet - 1
				packetDropMac 	= msgTelemetryByHop.packetDropMac - 1
				packetDropNet 	= msgTelemetryByHop.packetDropNet - 1
				rssi 			= float(msgTelemetryByHop.rssi)

				hops.append({	"relayId": 		relayId,
								"timeReceived": timeReceived})

			self.transmissions.append({	"time": 			timeStarted,
										"timeStarted": 		timeStarted,
										"source": 			source,
										"destination": 		destination,
										"messageType": 		messageType,
										"idVideo": 			idVideo,
										"hops": 			hops})


			transmissions = [transmission for transmission in self.transmissions if (transmission["time"] > (time - 2.0))]
			self.transmissions = copy.copy(transmissions)
			self.transmissions.sort(key=sortByIdVideo)

			print("-> transmissions (BEGIN)")
			for transmission in self.transmissions:
				print(	"--> timeStarted: " 	+ str(transmission["timeStarted"]) + \
						" - idVideo: " 			+ str(transmission["idVideo"]) + \
						" - source: " 			+ str(transmission["source"]) + \
						" - destination: " 		+ str(transmission["destination"]))
				for hop in transmission["hops"]:
					print(	"---> relayId: " 	+ str(hop["relayId"]) + \
							" - timeReceived: " + str(hop["timeReceived"]))
			print("-> transmissions (END)")

			# transmissionsNodesRU = np.zeros(self.config["numRelayNodes"])
			# for index, nameNodeRU in enumerate(self.config["namesRN"]):
			# 	relayId = int(nameNodeRU)
			# 	if relayId in self.transmissions:
			# 		transmissionsNodesRU[index] = len(self.transmissions[relayId]["transmissions"])
			# print("transmissionsNodesRU -> " + str(transmissionsNodesRU))

			if (self.config["manageRouting"] == "true"):

				routes_target = []
				self.routing.initialize()

				for transmission in self.transmissions:
					routes 	= self.routing.computeRoutes(transmission["source"], transmission["destination"])
					n = len(routes)
					for i in range(1, n):
						routes_target.append(routes[i])
						self.routing.addTransmission(routes[i]["nodeId"])

				# routes = routing.computeRouting(self.config["nameCN"], 	self.config["posCN"],
				# 								self.config["namesUN"], self.config["posUN"],
				# 								self.config["namesRN"], self.config["posRN"],
				# 								self.config["dcm"], transmissionsNodesRU)
				# self.routes_target = routing.processRoute(routes)

				self.processUpdateRoutingTable(contractOut.updateRoutingTable, routes_target)

					# new = contractOut.updateRoutingTable.add()
					# new.relayId = 3
					# new.route.destination = 21
					# new.route.nextHop = 7
					# new.route.drop = False
					# new.action = contract_out_pb2.ContractOut.Action.UPDATE

					# new = contractOut.updateRoutingTable.add()
					# new.relayId = 7
					# new.route.destination = 21
					# new.route.nextHop = 6
					# new.route.drop = False
					# new.action = contract_out_pb2.ContractOut.Action.UPDATE

					# self.transmissions[relayId]["lastTimeSent"] = time

		# 	self.coordination = topology.Coordination(config)self.processPacketIn(contractOut.updateRoutingTable, contractIn.relayId, contractIn.userId)

		self.printUpdateRoutingTable(contractOut.updateRoutingTable)

		self.printUpdateUser(contractOut.updateUser)

		# print("size of updateArea: " + str(len(contractOut.updateArea)))
		# for reg in contractOut.updateArea:
		# 	print(	str(reg.relayId) 		+ " - " + 
		# 			str(reg.area.center.x) 	+ " - " + 
		# 			str(reg.area.center.y))

		return contractOut.SerializeToString()

	def processRegisterUser(self, updateUser, relayId, userId):

		routes_target = routing.processRoute(self.config["routes"])

		register = False

		for route_target in routes_target:
			if (int(route_target["nodeId"]) == int(relayId) and 
				int(route_target["nextHop"]) == int(userId) and 
				int(route_target["destination"]) == int(userId)):
				register = True
				break

		if register:
			# print('User registered!')
			new 		= updateUser.add()
			new.relayId = int(relayId)
			new.userId 	= int(userId)
			new.action 	= contract_out_pb2.ContractOut.Action.ADD
			new.event 	= contract_out_pb2.ContractOut.Event.REQUIRED

	def processPacketIn(self, updateRoutingTable, relayId, userId):
		route = self.routing.computeRoute(relayId, userId)
		self.deleteRoutesActive(route["nodeId"], route["nextHop"], route["destination"])
		self.processUpdateRoutingTable(updateRoutingTable, [route])

		# for route_target in self.routes_target:
		# 	if (int(route_target["nodeId"]) == int(relayId) and \
		# 		int(route_target["destination"]) == int(userId)):

		# 		print("add route")
		# 		new = updateRoutingTable.add()
		# 		new.relayId = int(relayId)
		# 		new.route.destination = int(userId)
		# 		new.route.nextHop = int(route_target["nextHop"])
		# 		new.route.drop = False
		# 		new.action = contract_out_pb2.ContractOut.Action.ADD
		# 		self.updateRoutesActive(nodeId = new.relayId,
		# 								nextHop = new.route.nextHop,
		# 								destination = new.route.destination)
		# 		break

	def processUpdateRoutingTable(self, updateRoutingTable, routes_target):

		for route_target in routes_target:
			indices = utils.findIndices(self.routes_active, lambda r: (	(int(r["nodeId"]) == int(route_target["nodeId"])) and \
																	(int(r["nextHop"]) == int(route_target["nextHop"])) and \
																	(int(r["destination"]) == int(route_target["destination"]))))
			if not (len(indices) > 0):	
				print("update route")
				new = updateRoutingTable.add()
				new.relayId = int(route_target["nodeId"])
				new.route.destination = int(route_target["destination"])
				new.route.nextHop = int(route_target["nextHop"])
				new.route.drop = False
				new.action = contract_out_pb2.ContractOut.Action.UPDATE
				self.updateRoutesActive(nodeId = new.relayId,
										nextHop = new.route.nextHop,
										destination = new.route.destination)
			# else:
			# 	print("delete route")
			# 	new = updateRoutingTable.add()
			# 	new.relayId = int(route_active["nodeId"])
			# 	new.route.destination = int(route_active["destination"])
			# 	new.route.nextHop = int(route_active["nextHop"])
			# 	new.route.drop = False
			# 	new.action = contract_out_pb2.ContractOut.Action.DELETE
			# 	self.deleteRoutesActive(nodeId = new.relayId,
			# 							nextHop = new.route.nextHop,
			# 							destination = new.route.destination)

	def processUpdateArea(self, updateArea):
		names = copy.copy(self.coordination.namesNodesRU)
		positions = copy.copy(self.coordination.posNodesRU_target)
		for i in range(len(names)):
			new 				= updateArea.add()
			new.relayId 		= int(names[i])
			new.area.radius 	= str(0)
			new.area.center.x 	= str(positions[i][0])
			new.area.center.y 	= str(positions[i][1])

	def updateRoutesActive(self, nodeId, nextHop, destination):
		found = False
		for index, route in enumerate(self.routes_active):
			if 	(int(route["nodeId"]) == int(nodeId)) and \
				(int(route["destination"]) == int(destination)):
				self.routes_active[index]["nextHop"] = str(nextHop)
				found = True
		if not found:
			self.routes_active.append({	"nodeId": 		str(nodeId),
										"nextHop": 		str(nextHop),
										"destination": 	str(destination)})

	def deleteRoutesActive(self, nodeId, nextHop, destination):
		found = False
		for index, route in enumerate(self.routes_active):
			if 	(int(route["nodeId"]) == int(nodeId)) and \
				(int(route["nextHop"]) == int(nextHop)) and \
				(int(route["destination"]) == int(destination)):
				del self.routes_active[index]

	# def updateRoutesTarget(self, route_new):
	# 	for index, route in enumerate(self.routes_target):
	# 		if 	(int(route["source"]) == int(route_new["source"])) and \
	# 			(int(route["destination"]) == int(route_new["destination"])):
	# 			del self.routes_target[index]
	# 			break
	# 	self.routes_target.append(route_new)

	def printUpdateRoutingTable(self, updateRoutingTable):
		if (len(updateRoutingTable) > 0):
			print("-> updateRoutingTable (BEGIN)")
			print("size: " + str(len(updateRoutingTable)))
			for reg in updateRoutingTable:
				print(	"relayId: " 	+ str(reg.relayId) 				+ " - " + 
						"destination: " + str(reg.route.destination) 	+ " - " + 
						"nextHop: " 	+ str(reg.route.nextHop)		+ " - " + 
						"drop: " 		+ str(reg.route.drop)			+ " - " + 
						"action: " 		+ str(reg.action))
			print("-> updateRoutingTable (END)")

	def printUpdateUser(self, updateUser):
		if (len(updateUser) > 0):
			print("-> updateUser (BEGIN)")
			print("size of updateUser: " + str(len(updateUser)))
			for reg in updateUser:
				print(	"relayId: " 	+ str(reg.relayId) + " - " + 
						"userId: " 		+ str(reg.userId))
			print("-> updateUser (END)")

	def plot(self):

		plt.figure(self.figPlot.number)
		
		plt.title("time: " + str(self.coordination.time)) 

		drawnow(pltWSN.plotWSN,	nameNodeC 		= self.coordination.nameNodeC,
						 		posNodeC		= self.coordination.posNodeC,
						 		namesNodesMU 	= self.coordination.namesNodesMU,
						 		posNodesMU 		= self.coordination.posNodesMU,
						 		namesNodesRU 	= self.coordination.namesNodesRU,
						 		posNodesRU		= self.coordination.posNodesRU,
								routes 			= [],
								dcm_max 		= self.config["dcm"], 
								dcm 			= self.config["dcm"], 
								dcm_th 			= self.config["dcm"],
						 		filename		= None,
						 		xlim 			= [0, self.config["dim"]], 
						 		ylim 			= [0, self.config["dim"]])
