#! /usr/bin/python
import copy
import numpy as np
import contract_in_pb2
import contract_out_pb2
from coordination import topology
from coordination import pltWSN
from coordination import utils
from coordination import routing
from matplotlib import pyplot 	as plt
from drawnow 	import drawnow, figure
	
class Application(object):
	"""docstring for ClassName"""
	def __init__(self, nodeId, config):

		self.nodeId = str(nodeId)

		self.config = config

		self.active = (self.nodeId == config["nameCN"])

		routes_target = routing.processRoute(self.config["routes"])
		for route_target in routes_target:
			if (route_target["nodeId"] == str(5)):
				print(route_target)

		if (self.nodeId == config["nameCN"]):

			self.coordination = topology.Coordination(config)

			# my_dpi = 96
			# self.figPlot = plt.figure(figsize=(480/my_dpi, 480/my_dpi), dpi=my_dpi)
		
	def initialize(self):

		contractOut = contract_out_pb2.ContractOut()

		contractOut.isACK = True

		routes_target = routing.processRoute(self.config["routes"])

		for route_target in routes_target:

			if (self.nodeId == route_target["nodeId"]):

				if 	(((self.nodeId in self.config["namesRN"]) 	and (route_target["destination"] == self.config["nameCN"])) or
					 ((self.nodeId in self.config["namesRN"]) 	and (route_target["destination"] in self.config["namesUN"])) or
					 ((self.nodeId == self.config["nameCN"]) 	and (route_target["destination"] in self.config["namesRN"]))):
	
					new = contractOut.updateRoutingTable.add()
					new.relayId = int(route_target["nodeId"])
					new.route.nextHop = int(route_target["nextHop"])
					new.route.destination = int(route_target["destination"])
					new.route.drop = False
					new.action = contract_out_pb2.ContractOut.Action.ADD

				if 	(	(self.nodeId in self.config["namesRN"]) 	and 
						(route_target["destination"] in self.config["namesUN"]) and 
						(route_target["nextHop"] in self.config["namesUN"]) and 
						(route_target["nodeId"] == self.nodeId)):

					new = contractOut.updateUser.add()
					new.relayId = int(route_target["nodeId"])
					new.userId 	= int(route_target["destination"])
					new.action 	= contract_out_pb2.ContractOut.Action.ADD
					new.event 	= contract_out_pb2.ContractOut.Event.REQUIRED


		# print("size of updateRoutingTable: " + str(len(contractOut.updateRoutingTable)))
		# for reg in contractOut.updateRoutingTable:
		# 	print(	str(reg.relayId) 		+ " - " + 
		# 			str(reg.route.destination) 	+ " - " + 
		# 			str(reg.route.nextHop)	+ " - " + 
		# 			str(reg.route.drop)	+ " - " + 
		# 			str(reg.action))

		return contractOut.SerializeToString()

	def request(self, data):
		# print("Receiving  ContractIn...")
		contractIn 	= contract_in_pb2.ContractIn()
		# print("Read from data: " + data)
		contractIn.ParseFromString(data)
		# print("ContractIn parsed!")

		# print("relayId: " + str(contractIn.relayId))
		# print("userId: " + str(contractIn.userId))

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
			# print('_CONTRACTIN_PACKET_IN')
			self.processPacketIn(contractOut.updateRoutingTable, contractIn.relayId, contractIn.userId)

		# print("size of updateUser: " + str(len(contractOut.updateUser)))
		# for reg in contractOut.updateUser:
		# 	print(	str(reg.relayId) 		+ " - " + 
		# 			str(reg.userId))

		# print("size of updateRoutingTable: " + str(len(contractOut.updateRoutingTable)))
		# for reg in contractOut.updateRoutingTable:
		# 	print(	str(reg.relayId) 		+ " - " + 
		# 			str(reg.route.destination) 	+ " - " + 
		# 			str(reg.route.nextHop)	+ " - " + 
		# 			str(reg.route.drop)	+ " - " + 
		# 			str(reg.action))

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
			if (route_target["nodeId"] == str(relayId) and 
				route_target["nextHop"] == str(userId) and 
				route_target["destination"] == str(userId)):
				register = True
				break

		if register:
			# print('User registered!')
			new 		= updateUser.add()
			new.relayId = relayId
			new.userId 	= userId
			new.action 	= contract_out_pb2.ContractOut.Action.ADD
			new.event 	= contract_out_pb2.ContractOut.Event.REQUIRED

	def processPacketIn(self, updateRoutingTable, relayId, userId):

		routes_target = routing.processRoute(self.config["routes"])

		for route_target in routes_target:
			if (route_target["nodeId"] == str(relayId) and route_target["destination"] == str(userId)):
				new = updateRoutingTable.add()
				new.relayId = int(relayId)
				new.route.destination = int(userId)
				new.route.nextHop = int(route_target["nextHop"])
				new.route.drop = False
				new.action = contract_out_pb2.ContractOut.Action.ADD
				break

	def processUpdateRoutingTable(self, updateRoutingTable):

		routes_target 		= routing.processRoute(self.coordination.routes_target)

		routes_active_new 	= copy.copy(self.routes_active)

		for route_active in self.routes_active:

			indices = utils.findIndices(routes_target, lambda r: (	(r[0] == route_active[0]) and \
																	(r[-1] == route_active[-1])))
			if (len(indices) > 0):
				for index in indices:
					checked[index] = 1
					if not (routes_target[index][1] == route_active[1]):
						new = updateRoutingTable.add()
						new.relayId = int(routes_target[index][0])
						new.route.destination = int(routes_target[index][-1])
						new.route.nextHop = int(routes_target[index][1])
						new.route.drop = False
						new.action = contract_out_pb2.ContractOut.Action.UPDATE
			else:
				new = updateRoutingTable.add()
				new.relayId = int(route_active[0])
				new.route.destination = int(route_active[-1])
				new.route.nextHop = int(route_active[1])
				new.route.drop = False
				new.action = contract_out_pb2.ContractOut.Action.DELETE
				routes_active_new.remove(route_active)

		self.routes_active = routes_active_new

	def processUpdateArea(self, updateArea):
		names = copy.copy(self.coordination.namesNodesRU)
		positions = copy.copy(self.coordination.posNodesRU_target)
		for i in range(len(names)):
			new 				= updateArea.add()
			new.relayId 		= int(names[i])
			new.area.radius 	= str(0)
			new.area.center.x 	= str(positions[i][0])
			new.area.center.y 	= str(positions[i][1])

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