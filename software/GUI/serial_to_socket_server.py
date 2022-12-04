# -*- coding: utf-8 -*-
"""
Created on Tue Oct 22 17:59:02 2019

@author: master
"""

import serial
import serial.tools.list_ports
import sys, os
import glob
import time
import socket
import threading, queue, copy
from threading import Event, Thread
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QVBoxLayout

sys.path.append(os.path.join(os.path.dirname(os.path.dirname(__file__)),"Maze_Python"))
from Ressources.widgets.SocketPackets import SendSizedPacket, RecieveSizedPacket

key_lock = threading.Lock()

       

class RecieveHandle(threading.Thread):

    def __init__(self, clientsocket, ready = None):

        threading.Thread.__init__(self)
        self.ready = ready
        self.Exit = 0
        self.Request = []
        self.clientsocket = clientsocket
        
    def run(self): 
        self.ready.set()
        while self.Exit == 0:  
            try:
                key_lock.acquire()
                self.Request = RecieveSizedPacket(self.clientsocket)
                
                #self.Request = clientsocket.recv(999)
            except:
                self.Exit = 1

class JsonMessageBuilder : 
    
    def __init__(self):
    
        self.layer = 0
        self.status = "idle"
        self.message = ""

    def add_byte(self,input_bit) :
        if self.status == "idle" and input_bit == bytes([0x7B]) : #we aren't capturing anything yet and a { arrives.
            self.layer = 1
            self.status = "capturing"
            try :
                self.message = input_bit.decode()
            except Exception as e :
                print("Error decoding a byte : {}".format(e))
            
        elif self.status == "capturing" :
            if input_bit == bytes([0x7B]) : # we go deeper in the nesting
                self.layer += 1
            
            elif input_bit == bytes([0x7D]): # we exit one layer out in the nesting
                self.layer -= 1
            
            try :
                self.message += input_bit.decode()
            except Exception as e :
                print("Error decoding a byte : {}".format(e))
            
        if self.status == "capturing" and self.layer <= 0 : #did we reach outermost nesting layer ? Then we show the massage is ready for collection
            self.status = "message_ready"

    def is_ready(self):
        return True if self.status == "message_ready" else False

    def get_message(self):
        copy_msg = copy.copy(self.message)
        self.clear()
        return copy_msg

    def clear(self):
        self.layer = 0
        self.status = "idle"
        self.message = ''

class SendAndRecieve(threading.Thread):

    def __init__(self, chosenport, clientsocket, ip):
        threading.Thread.__init__(self)
        self.clientsocket = clientsocket
        self.ExitThread = 0
        self.ip=ip
        print(chosenport)
        if chosenport == -1 or chosenport is None:
            self.WarningSendAndQuit=1
        else:
            self.chosenport=chosenport
            self.WarningSendAndQuit=0
            print("[+] New thread for %s %s" % (self.chosenport, self.ip))

    def run(self):
        
        
        if self.WarningSendAndQuit==1:
            Msg = '{"Error" : 0 , "print": "No connexion was opened between server and arduino. Quit app, plug-unplug arduino USB, check power and restart app"}'
            SendSizedPacket(self.clientsocket,Msg)
            return
        
        with serial.Serial(port=self.chosenport, baudrate=250000, timeout=0.1, writeTimeout=0.1) as self.port_serie:
            SendSizedPacket(self.clientsocket,'{"server_info" : "Serial and socket communications opened"}')
            time.sleep(0.1)
            self.port_serie.flushInput()
            self.SendSerial("{handshake:1}")

            ready = Event()
            recieve=RecieveHandle(self.clientsocket,ready)
            recieve.start()
            ready.wait()
            
            millis = int(round(time.time() * 1000 ))
            json_builder = JsonMessageBuilder()
            
            while self.ExitThread==0:
                
                #handling arduino inputs to the GUI
                if self.port_serie.in_waiting:
                    
                    json_builder.add_byte(self.port_serie.read())
                    if json_builder.is_ready():
                        Line = json_builder.get_message()
                        print(Line)
                        try : 
                            SendSizedPacket(self.clientsocket,Line)
                        except :
                            recieve.Exit=1
                            self.clean_exit_thread("Sending data failed")

                #pinging GUI every few seconds to check if closed
                if int(round(time.time() * 1000 )) > millis + 2000:
                    try:
                        SendSizedPacket(self.clientsocket,"")
                        millis = int(round(time.time() * 1000 ))
                    except:     
                        recieve.Exit=1
                        self.clean_exit_thread("GUI ping timeout")
                        
                    millis = int(round(time.time() * 1000 ))
                    
                #treating info from the GUI to this thread (exit requests) or the arduino
                if recieve.Request != []:
                    print("Treating recieve request : {}".format(recieve.Request))
                    if recieve.Request == 'exit':
                        recieve.Exit=1
                        self.clean_exit_thread("GUI requested exit")
                    else :
                        self.SendSerial(recieve.Request)
                            
                    recieve.Request=[]
                    key_lock.release()
                
                if recieve.Request == [] and key_lock.locked():
                    key_lock.release()
                    
        print("[-] Exiting thread from %s %s \nMORJ server. En écoute..."% (self.chosenport, self.ip))
                
    def clean_exit_thread(self,reason):
        self.port_serie.close()
        self.clientsocket.close()
        self.ExitThread = 1
        print("Connexion closed, shuting down arduino COM to make it available with TCP socket again - Reason : " + reason )


    def SendSerial(self,Arg):
        
        try :
            SendLine = Arg.decode()
        except Exception :# as e: 
            SendLine = Arg
            #print("Can't decode : {}".format(e))
        try :
            print("Sending to arduino : {}".format(SendLine))
            self.port_serie.write(((SendLine)+"\n").encode())            
#            self.clientsocket.send(Arg)
        except Exception as e: 
            print("Error {} - when sending via serial : {}".format(e,SendLine))

                
def serial_ports(*args):
    
    results = []
    if sys.platform.startswith('win'):
#        ports = ['COM%s' % (i + 1) for i in range(256)]
        
        # serial.tools.list_ports.grep
        ports = list(serial.tools.list_ports.comports())
        
        print("Detected COM ports open and available are:")
        for p in ports:
            print(p)
            print(p.hwid)
            if p.serial_number == "557363037313519081C0" or p.serial_number == "959313239313513112A1" or p.serial_number == "959313239313515181D0" : # "557363037313519081C0" : serial id of arduino mega MORJ
            #959313239313515181D0 id or Arduino Mega test board from timothe's house
                results.append(p[0])
#            if "Genuino" in p[1] or "Mega" in p[1] : #"Arduino" in p[1] or "Uno" in p[1]
                
                
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')
    
    if len(results) > 1 :
        if len(args) < 1:
            print("Cannot automatically resolve correct arduino port to select, several are available, choosing fist one by default, if necessary, enter the index of the port to select as argument to this function")
            result = results[0]
        else :
            result = results[args[0]]
    elif len(results) < 1 :
        print("No COM port suceptible to be a microcontroller has been found, make sure an arduino (or equivalent) is connected, and the arduino port is not used by another windows app")
        result = None
    else :
        result = results[0]
        
    print("Chosen Port is %s" % (result)) 
#            pass
    return result



tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcpsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
tcpsock.bind(("",1112))
COMportID = 1

while True:
    try :
        tcpsock.listen(10)
        print( "MORJ server. En écoute...")
        print("\n")
        (clientsocket, (ip, port)) = tcpsock.accept()
        print("Requested connextion detected from {}:{}".format(ip, port))
        ArduinoPort = serial_ports()
        send=SendAndRecieve(ArduinoPort, clientsocket, ip)
        send.start()
    except Exception as e:
        print("Error happened : {} . Trying recovery state".format(e))