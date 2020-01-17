import pso
import cluster
import utils
import pltWSN
import sys
import numpy as np
import matplotlib.pyplot as plt

class Construction(object):

	def __init__(self, config):
		self.config = config

	def computeConstruction(self, namesUN, posUN, nRU):

		# namesRN1, posRN1 = cluster.computeCluster(posUN, self.config["dcm"])

		posCN, posRN = pso.computePSO(namesUN, posUN, nRU, config = self.config)

		# posRN 	= utils.concatenate(None, posRN1, posRN2)

		return posCN, posRN

def TestConstruction():

	namesUN = ["u1","u2","u3","u4","u5","u6","u7","u8","u9","u10","u11","u12","u13","u14","u15"]

	posUN	= np.array([[210,20],[1005,115],[1400,455],[785,1005],[675,1950],
						[200,10],[1000,100],[1500,450],[780,1000],[670,1900],
						[67,450],[350,1100],[1100,1780],[1560,140],[100,1000]])

	posCN, posRN = computeConstruction(namesUN, posUN, 30)

	namesRN = utils.computeName(posRN)

	routes 	= routing.computeRouting(	nameC, posCN,
										namesUN, posUN,
										namesRN, posRN)

	my_dpi=96
	fig1 = plt.figure(figsize=(480/my_dpi, 480/my_dpi), dpi=my_dpi)
	pltWSN.plotWSN(	nameNodeC=nameC, posNodeC=posCN,\
					namesNodesMU=namesUN, posNodesMU=posUN, \
					namesNodesRU=namesRN, posNodesRU=posRN, \
					dcm_max = config.dcm, dcm = config.dcm, dcm_th = config.dcm,\
					routes=[], filename = None, xlim = [0, config.dim], ylim = [0, config.dim])
	plt.show()

if __name__ == '__main__': TestConstruction()