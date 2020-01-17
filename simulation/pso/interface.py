import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/../../application/coordination/")
from Tkinter import *
import pso
import numpy as np
import pltWSN
from matplotlib import pyplot 	as plt
from drawnow 	import drawnow, figure
import replay

class Interface:		
	def __init__(self, master=None):

		self.run = False
		self.stop = False

		self.fontePadrao = ("Arial", "10")

		self.Container1 = Frame(master)
		self.Container1["pady"] = 10
		self.Container1.pack()
  
		self.Container2 = Frame(master)
		self.Container2["padx"] = 10
		self.Container2.pack()
  
		self.Container3 = Frame(master)
		self.Container3["padx"] = 10
		self.Container3.pack()
  
		self.Container4 = Frame(master)
		self.Container4["padx"] = 10
		self.Container4.pack()

		self.Container5 = Frame(master)
		self.Container5["pady"] = 10
		self.Container5.pack()
  
		self.Container6 = Frame(master)
		self.Container6.pack()
  
		self.titulo = Label(self.Container1, text="PSO Parameters")
		self.titulo["font"] = ("Arial", "12", "bold")
		self.titulo.pack()
  
		self.wLabel = Label(self.Container2, text="w: ", font=self.fontePadrao)
		self.wLabel.pack(side=LEFT)

		self.wCurrent = Label(self.Container2, text="---", font=self.fontePadrao)
		self.wCurrent.pack(side=LEFT)
  
		self.wNew = Entry(self.Container2)
		self.wNew["width"] = 30
		self.wNew["font"] = self.fontePadrao
		self.wNew.pack(side=LEFT)
  
		self.c1Label = Label(self.Container3, text="c1: ", font=self.fontePadrao)
		self.c1Label.pack(side=LEFT)

		self.c1Current = Label(self.Container3, text="---", font=self.fontePadrao)
		self.c1Current.pack(side=LEFT)
  
		self.c1New = Entry(self.Container3)
		self.c1New["width"] = 30
		self.c1New["font"] = self.fontePadrao
		self.c1New.pack(side=LEFT)
  
		self.c2Label = Label(self.Container4, text="c2: ", font=self.fontePadrao)
		self.c2Label.pack(side=LEFT)

		self.c2Current = Label(self.Container4, text="---", font=self.fontePadrao)
		self.c2Current.pack(side=LEFT)
  
		self.c2New = Entry(self.Container4)
		self.c2New["width"] = 30
		self.c2New["font"] = self.fontePadrao
		self.c2New.pack(side=LEFT)

		self.start = Button(self.Container5)
		self.start["text"] = "Start"
		self.start["font"] = ("Calibri", "8")
		self.start["width"] = 12
		self.start["command"] = self.Start
		self.start.pack()
  
		self.mensagem = Label(self.Container6, text="", font=self.fontePadrao)
		self.mensagem.pack()
 
	# Metodo verificar senha
	def Start(self):
		self.run = True
		self.start["text"] = "Pause"
		self.start["command"] = self.Pause

	def Pause(self):
		self.stop = True
		self.start["text"] = "Start"
		self.start["command"] = self.Start

def run(namesUN, posUN, nRU, w, c1, c2, savePICAt, config):

	_root = Tk()
	_interface = Interface(_root)

	running = False
	paused = False

	cost = []

	while True:

		_root.update_idletasks()
		_root.update()

		if not running and not paused and _interface.run:
			_interface.run = False

			_pso 	= pso.PSO(namesUN, posUN, nRU, config, savePICAt)
			_pso.setup()
			_pso.initialize()
			_replay = replay.Replay( nameNodeC = _pso.nameNodeC,
									 namesNodesMU = _pso.namesNodesMU,
									 posNodesMU = _pso.posNodesMU,
									 Nr = _pso.Nr,
									 Nk =  _pso.Nk,
									 config = _pso.config)


			running = True

		if running and not paused and not _interface.stop:
			_interface.mensagem["text"] = "Running..."
			_interface.wCurrent["text"] = str(_pso.w)
			_interface.c1Current["text"] = str(_pso.c1)
			_interface.c2Current["text"] = str(_pso.c2)

			running = not _pso.iterate()

			_replay.PlotParticle(_pso.Pg, _pso.iNk)

			cost.append([_pso.iNk, _pso.Pg.F.connectivity, _pso.Pg.F.Cost, _pso.Pg.F.distance, _pso.Pg.F.rangePenalty, _pso.Pg.F.safetyPenalty])

			_replay.PlotCost(cost)

			if not running:
				_interface.mensagem["text"] = "Done!"

		if running and not paused and _interface.stop:
			_interface.stop = False
			paused = True
			_interface.mensagem["text"] = "Paused."

		if running and paused and _interface.run:
			_interface.run = False
			paused = False