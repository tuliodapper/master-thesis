import numpy as np

import utils
import pltWSN

from matplotlib import pyplot 	as plt

from sklearn.cluster import AffinityPropagation
from sklearn.cluster import MiniBatchKMeans
from sklearn.cluster import AgglomerativeClustering
import cluster
from scipy.cluster.hierarchy import ward, fcluster
from scipy.spatial.distance import pdist

def computeCluster(posNodesRU, dcm, suf = "r"):
	Z = ward(pdist(posNodesRU))
	c = fcluster(Z, t=dcm, criterion='distance')
	pos = []
	for g in np.unique(c):
		i = np.where(c == g)
		pos.append(utils.computeCenter(posNodesRU[i,:][0]))
	posNodesRU 	= np.array(pos)
	namesNodesRU = utils.computeName(posNodesRU, suf)
	return namesNodesRU, posNodesRU

if __name__ == '__main__':
	dim = 2000
	dcm = 300
	namesUN = ["u1","u2","u3","u4","u5","u6","u7","u8","u9","u10","u11","u12","u13","u14","u15"]
	posUN	= np.array([[210,20],[1005,115],[1400,455],[785,1005],[675,1950],
						[200,10],[1000,100],[1500,450],[780,1000],[670,1900],
						[67,450],[350,1100],[1100,1780],[1560,140],[100,1000]])

	namesRN, posRN = computeCluster(posUN, dcm = dcm);

	my_dpi=96

	fig1 = plt.figure(figsize=(480/my_dpi, 480/my_dpi), dpi=my_dpi)
	pltWSN.plotWSN(	nameNodeC=None, posNodeC=None,\
					namesNodesMU=namesUN, posNodesMU=posUN, \
					namesNodesRU=namesRN, posNodesRU=posRN, \
					dcm_max = dcm, dcm = dcm, dcm_th = dcm,\
					routes=[], filename = None, xlim = [0, dim], ylim = [0, dim])
	plt.show()