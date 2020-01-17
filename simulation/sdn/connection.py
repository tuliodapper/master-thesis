#! /usr/bin/python
import sys
sys.path.append('../../application/')

import socket
import threading
from thread import *
import application
import time
import pickle

CONST_MAX_BUFFER = 8192

def RunServer(con, nodeId, config):
    my_app = application.Application(nodeId, config)
    dataSend = my_app.initialize()
    con.send(dataSend)
    while my_app.active:
        # print("Waiting...")
        dataRecv = ""
        while (len(dataRecv) == 0):
            dataRecv = con.recv(CONST_MAX_BUFFER)
        # print("Received size of: " + str(len(dataRecv)))
        dataSend = my_app.request(dataRecv)
        time.sleep(1)
        sizeSentData = con.send(dataSend)
        # print("Sent size: " + str(sizeSentData) + "/" + str(len(dataSend)))
    con.close()

def ListenMultiple(config):
    host = ""
    port = 8000
    ncli = 1
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((host, port))
    sock.listen(ncli)
    print("Socket is listening...")
    while True:
        con, add = sock.accept()
        nodeId = con.recv(CONST_MAX_BUFFER)
        start_new_thread(RunServer, (con, nodeId, config))
    sock.close()

def ListenSingle(config):
    host = ""
    port = 8000
    ncli = 1
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((host, port))
    sock.listen(ncli)
    print("Socket is listening...")
    con, add = sock.accept()
    print("Connected!")
    typ = con.recv(CONST_MAX_BUFFER)
    time.sleep(2)
    con.send("ok")
    RunServer(con,typ,config)
    sock.close()


if __name__ == '__main__': 

    simfile = sys.argv[1]

    with open(simfile, "rb") as f:
        config = pickle.load(f)

    ListenMultiple(config)