import os
import networkx as nx
import matplotlib.pyplot as plt
import numpy as np
import utils
from collections import Counter
from networkx.algorithms.approximation import clique

cores = np.arange(100) 
cores = [str(c) for c in cores]
cores = ["RED", "YELLOW", "GREEN", "BLUE", "GRAY", "PINK"]

def verificaCor(estado, cor, Grafo):
    for vizinho in Grafo.neighbors(estado):
        corVizinho = Grafo.node[vizinho]['cor']
        if corVizinho == cor:
            return False
    return True


def pegaCor(estado, Grafo):
    for cor in cores:
        if verificaCor(estado, cor, Grafo):
            return cor

def generateGraph(names, positions, dcm):
    numVertices = len(names)
    estados={}
    for i1 in range(numVertices):
        name1       = names[i1]
        position1   = positions[i1]
        estados[name1] = []
        for i2 in range(numVertices):
            name2       = names[i2]
            position2   = positions[i2]
            if  (i1 != i2) and \
                (utils.computeDistance(position1, position2) < dcm):
                estados[name1].append(name2)
    Grafo = nx.Graph()
    DiGrafo = nx.DiGraph()
    for estado in estados:
        Grafo.add_node(estado, cor=' ')
        DiGrafo.add_node(estado, cor=' ')
        for f in range(0,len(estados[estado])):
            Grafo.add_edge(estado,estados[estado][f])
            DiGrafo.add_edge(estado,estados[estado][f])
    return Grafo, DiGrafo

class Graph:        
    def __init__(self, names, positions, dcm):
        self.Graph, self.DiGraph = generateGraph(names, positions, dcm)
        
    def computeColoring(self):
        Grafo = self.Graph
        for estado in Grafo.node:
            Grafo.node[estado]['cor']=pegaCor(estado, Grafo)
        cores=[]
        for estado in Grafo.node:
            cores.append(Grafo.node[estado]['cor'])
        self.cores = cores
        return len(Counter(cores).keys())

    def computeNodeConnectivity(self):
        return nx.node_connectivity(self.Graph)

    def computeMaxClique(self):
        return len(list(clique.max_clique(self.DiGraph)))

    def computeNumberConnectedComponents(self):
        return nx.number_connected_components(self.Graph)

if __name__ == "__main__":
    
    forma = '2'
    estados={}

    estados['Alagoas'] = ['Bahia', 'Pernambuco',  'Sergipe' ]
    estados['Bahia'] = ['Alagoas', 'Pernambuco', 'Piaui', 'Sergipe' ]
    estados['Ceara'] = ['Maranhao', 'Piaui', 'Paraiba', 'Pernambuco','Rio Grande do Norte']
    estados['Maranhao'] = ['Piaui', 'Ceara']
    estados['Paraiba'] = ['Ceara', 'Pernambuco','Rio Grande do Norte']
    estados['Pernambuco'] = ['Rio Grande do Norte', 'Alagoas','Bahia','Ceara', 'Paraiba']
    estados['Piaui'] = ['Ceara','Maranhao', 'Bahia']
    estados['Rio Grande do Norte'] = ['Pernambuco', 'Ceara', 'Paraiba']
    estados['Sergipe'] = ['Alagoas', 'Bahia' ]

    Grafo = Graph([], np.array([]), 55)

    for estado in estados:
        Grafo.Graph.add_node(estado, cor=' ')
        Grafo.DiGraph.add_node(estado, cor=' ')
        for f in range(0,len(estados[estado])):
            Grafo.Graph.add_edge(estado,estados[estado][f])
            Grafo.DiGraph.add_edge(estado,estados[estado][f])

    print("computeColoring: " + str(Grafo.computeColoring()))
    print("computeNodeConnectivity: " + str(Grafo.computeNodeConnectivity()))
    print("computeMaxClique: " + str(Grafo.computeMaxClique()))
    print("computeNumberConnectedComponents: " + str(Grafo.computeNumberConnectedComponents()))

    if forma=='1':
        nx.draw_spectral(Grafo.Graph, node_color=Grafo.cores)
        aux= nx.spectral_layout(Grafo.Graph)
    else:
        nx.draw_shell(Grafo.Graph, node_color=Grafo.cores)
        aux= nx.shell_layout(Grafo.Graph)

    nnode=0
    labels = {}
    pos={}
    for estado in Grafo.Graph.node:
        labels[nnode]=estado
        pos[nnode]=aux[estado]
        pos[nnode][1]=pos[nnode][1]+0.05
        nnode+=1

    nx.draw_networkx_labels(Grafo.Graph, pos,labels)
    plt.show()