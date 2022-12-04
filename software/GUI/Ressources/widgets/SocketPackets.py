# -*- coding: utf-8 -*-
"""
Created on Sat Feb 15 05:05:27 2020

@author: master
"""

#import socket
import struct
import pickle
import time





def unpack_helper(fmt, data):
    size = struct.calcsize(fmt)
    return struct.unpack(fmt, data[:size]), data[size:]

def ComputeHeader_Payload(InputString):
    PAYLOAD = InputString.encode('utf-8')
    HEADER = struct.pack("I", len(PAYLOAD))
    return HEADER, PAYLOAD

def SendSizedPacket(server,message):
    HEADER, PAYLOAD = ComputeHeader_Payload(message)
    server.sendall(HEADER)
    server.sendall(PAYLOAD)
    
def RecieveSizedPacket(server):
    HEADER = server.recv(4)
    BYTES = struct.unpack("I",HEADER)[0]
    PAYLOAD = server.recv(BYTES)
    Packet = PAYLOAD.decode('utf-8')
    return Packet

####################
    

header_packer = struct.Struct('I')
identifier_packer = struct.Struct('B')

def Send_AutoPacket(server, bin_payload, **kwargs):
    
    if kwargs.get("pickling",False) :
        bin_payload = pickle.dumps(bin_payload,protocol = 4)
        idd = 0 #INFO TO NOTIFY DATA IS PICKLED 
    else :
        idd = kwargs.get("identifier")
        #INFO TO NOTIFY DATA IS NOT PICKLED AND IT'S ID IS THIS VALUE
        #( BETWEEN 1 and 254 - 0x01 and 0xFE)
        if idd is None :
            idd = 255
            #INFO TO NOTIFY DATA IS NOT PICKLED AND THERE IS NO NEED FOR IDS
    identifier = identifier_packer.pack(idd)            
    size_header = header_packer.pack(len(bin_payload))#LENGTH BYTES OF DATA
    #SEND THE PACKET
    server.sendall(size_header)
    server.sendall(identifier)
    server.sendall(bin_payload)
    
def Recv_AutoPacket(server):#, const_unpacker = None):
    size_header = header_packer.unpack(recvall(server,4))[0]#LENGTH BYTES OF DATA
    identifier = identifier_packer.unpack(recvall(server,1))[0]#LENGTH BYTES OF DATA
    bin_payload = recvall(server,size_header)
    if identifier == 0 :
        return identifier, pickle.loads(bin_payload)
#    if identifier == 255 and const_unpacker is not None :
#        try :
#            return identifier, const_unpacker.unpack(bin_payload)
#        except struct.error :
#            return identifier, None
    return identifier, bin_payload
    
def RecievePack(server,decode,unpacktype):
    #HEADER = server.recv(4)
    #print(HEADER)
    #BYTES = struct.unpack("I",HEADER)[0]
    #PAYLOAD = server.recv(BYTES)
    PAYLOAD = recvall(server,344960)
    
    if decode :
        Packet = PAYLOAD.decode('utf-8')
        return Packet
    else:
        return PAYLOAD
    
def SendPack(server,PAYLOAD,encode):
    
    if encode :
        PAYLOAD = PAYLOAD.encode('utf-8')
    
    #HEADER = struct.pack("I", len(PAYLOAD))
    #server.sendall(HEADER)
    server.sendall(PAYLOAD)
    
def recvallBROKEN(sock, n):
    # Helper function to recv n bytes or return None if EOF is hit
    data = bytes(344960)
    start = 0
    while len(data) < n:
        packet = sock.recv(n - len(data))
        data[start:start+len(packet)] = packet
        start = len(packet)
    return data

def recvall(sock, n):
    # Helper function to recv n bytes or return None if EOF is hit
    data = bytearray()
    while len(data) < n:
        packet = sock.recv(n - len(data))
#       if not packet:
#            return None
        data.extend(packet)
    return data
    