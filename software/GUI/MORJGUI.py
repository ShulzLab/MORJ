# -*- coding: utf-8 -*-
"""
Created on Wed Oct 23 17:34:07 2019

@author: master
"""


#import datetime


import sys

import json, copy
import threading

from PyQt5.QtGui import QColor, QTextCursor #, QIcon
from PyQt5.QtCore import Qt, QTimer, QCoreApplication
from PyQt5.QtWidgets import (QMainWindow, QApplication, QCheckBox, QGridLayout, QHBoxLayout,
                             QLabel, QLineEdit, QTextEdit, QWidget)


from Ressources.widgets.DetachableTabs import DetachableTabWidget
from Ressources.widgets.SocketPackets import SendSizedPacket
from Ressources.Acquisitions import AcquisitionController

from Ressources.widgets import AccelControl, AccelGraph, CommandField, ConnexionHeader

global_recieve_lock = threading.Lock()

_discards = ["",]#put the incoming data keys you want to silence here
        
class OutboundMessageManager():
    
    modules = ["","imu","coupling","motor","bnc_in_2","bnc_in_1","records","lock","shutter"]       
    
    def __init__(self,parent=None):
        self.parent = parent
    
    def ask_motor_pos_regularly(self):
        self.TIMER = QTimer()
        self.TIMER.timeout.connect(lambda : self.send({"motor":{"get_pos":1}}))
    
    def ask_lock(self):
        self.send({"lock":{"set_status":1}})
        
    def ask_unlock(self):
        self.send({"lock":{"set_status":0}})
        
    def ask_shutter_on(self):
        self.send({"shutter":{"set_status":1}})
        
    def ask_shutter_off(self):
        self.send({"shutter":{"set_status":0}})
        
    def ask_decoupling(self):
        self.send({"coupling":{"set_status":0}})
        
    def ask_coupling(self):
        self.send({"coupling":{"set_status":1}})
        
    def ask_CCW_turn(self):
        self.send({"coupling":{"add_turn":-1}})
        
    def ask_CW_turn(self):
        self.send({"coupling":{"add_turn":1}})
        
    def ask_setting_IMU_zero(self):
        self.send({"coupling":{"set_zero":1}})
        
    def ask_setting_OREF(self):
        self.send({"motor":{"zero":1}})
        
    def ask_acquire(self):
        self.send({"records":{"set_status":1}})
        
    def ask_stopacquire(self):
        self.send({"records":{"set_status":0}})
            
    def send(self,command_dict=None): #definition of JsonCommand for the connexionGui class
        if command_dict is None :
            Command = copy.copy(self.parent.command_manager.send_box.lineEdit().text())
            self.parent.command_manager.send_box.lineEdit().clear()
        else :
            Command = json.dumps(command_dict)
        
        self.parent.UpdateServerConnexionInfo('dinfo', "Sending command :" + Command)
        SendSizedPacket(self.parent.connexion_manager.server, Command)
        
class InboundMessageManager():
    
    def __init__(self,parent=None):
        self.parent = parent
    
    @staticmethod
    def asFloat(data):
        if data is not None :
            return float(data)
        return None
        
    @staticmethod
    def asInt(data):
        if data is not None :
            return int(data)
        return None
            
    def use_aknowledge(self, aknowledge ):
    
        if "t" in aknowledge.keys():
            ak_type = aknowledge["t"]
        else :
            ak_type = None
            
        if "e" in aknowledge.keys():
            ak_error = aknowledge["e"]
        else :
            ak_error = None
            
        if ak_type == "v":
            self.parent.UpdateServerConnexionInfo('info',"Valid message recieved")
        else :
            if ak_type == "t":
                conflict = "Message timeout error. Probably you miss a terminal }"
            if ak_type == "d": 
                conflict = "Message deserialization error. You must write valid relaxed Json (exaclty like json, with quotes on keys being optionnal)"
            if ak_type == "l": 
                conflict = "Message length error. Too long"
            self.UpdateServerConnexionInfo('error',"Error : {} - {}".format(conflict,ak_error))
        
    def use_q(self, quats):
        pass
    
    def use_error(self, error):
        
        print("value error present") 
        if error == 0:
            cnx_manager = self.parent.connexion_manager
            cnx_manager.IpField.setDisabled(False)
            cnx_manager.ConnectIP.setDisabled(False)
            cnx_manager.RecieveThread.Exit = 1
            if not cnx_manager.RecieveThread.isFinished() :
                cnx_manager.RecieveThread.terminate()
#                    self.server.shutdown(socket.SocketShutdown.Both)
            cnx_manager.server.close()   
    
    def use_joystick_info(self, info):
        if not info :
            add = " not "
            mode = "error"
        else :
            add = " "
            mode = "info"
        self.parent.UpdateServerConnexionInfo(mode, "Joystick is" +add+ "connected".format())    
            
    def use_imu_status(self, status):
        self.parent.UpdateServerConnexionInfo('info',"Inertial Measurement Unit status : {}".format(status))
    
    def use_yaw(self, Yaw):
        
        try : 
            Yaw = float(Yaw)
        except TypeError :
            Yaw = None
                        
        if self.parent.AcquisitionStatus == 0 :
            self.parent.AcquisitionStatus = 1
            self.parent.accel_graph.Polarplot(Yaw,50)
        else :
            self.parent.accel_graph.UpdatePolarplot(Yaw, self.parent.accel_graph.summed_steps_to_yaw(self.parent.accel_box.SummedCommands))
    
    def use_data(self, data):

        self.parent.acquisition_manager.queue.put(copy.copy(data))

        if "cy" in data.keys() or "y" in data.keys():
            cy = data.pop("cy",None)
            y = data.pop("y",None)
            
            # if cy is not None :
            #     self.use_yaw(cy)
            #     self.parent.accel_box.CurrentPos = cy
            #     self.parent.accel_box.update()
            # elif y is not None :
            #     self.use_yaw(y)
                
                
        if "ys" in  data.keys() :
            ys = data.pop("ys")
      
            try : 
                ys = float(ys)
            except TypeError :
                ys = None
            if self.parent.AcquisitionStatus == 0 :
                self.parent.AcquisitionStatus = 1
                self.parent.accel_graph.Polarplot( self.parent.accel_graph.summed_yaw_to_yaw(ys) ,50)
            else :
                self.parent.accel_graph.UpdatePolarplot( self.parent.accel_graph.summed_yaw_to_yaw(ys) , self.parent.accel_graph.summed_steps_to_yaw(self.parent.accel_box.SummedCommands))
               
        if "csc" in data.keys():
            command = data.pop("csc")
            try : 
                self.parent.accel_box.SummedCommands = int(command)
            except TypeError :
                self.parent.accel_box.SummedCommands = None
            self.parent.accel_box.update()     
            
        if "lsc" in data.keys():
            command = data.pop("lsc")
            # try : 
            #     self.parent.accel_box.SummedCommands = int(command)
            # except TypeError :
            #     self.parent.accel_box.SummedCommands = None
            # self.parent.accel_box.update()     
                
        if "p" in data.keys():
            self.parent.accel_box.pitch = data.pop("p")
            self.parent.accel_box.update()
     
        if "r" in data.keys():
            self.parent.accel_box.roll = data.pop("r")
            self.parent.accel_box.update()
            
        if "tu" in data.keys():
            self.parent.accel_box.Turns = data.pop("tu")
            self.parent.accel_box.update()
    
    def check_keys(self, message):

        if "ak" in message.keys(): #message recieved aknowledgement
            self.use_aknowledge(message.pop("ak"))
            
        if "server_info" in message.keys():
            self.parent.UpdateServerConnexionInfo('dinfo',message.pop("server_info"))
        
        if "Error" in message.keys(): 
            self.use_error(message.pop("Error"))
            
        if "print" in message.keys():  
            print(message.pop("print")) 
        
        if "MORJ_Controller" in message.keys():  
            self.parent.UpdateServerConnexionInfo('info',"MORJ_Controller initializing status : {}".format(message.pop("MORJ_Controller")))

        if "joystick_connected" in message.keys():
            self.use_joystick_info(message.pop("joystick_connected"))

        if "IMU_status" in message.keys():
            self.use_imu_status(message.pop("IMU_status"))
            
        if "d" in message.keys():
            self.use_data(message.pop("d"))
        
        if "go" in message.keys():#now, go s sent through a nested object, usually with input_copy as outer key
            self.parent.accel_box.TommandTotal += message.pop("go")
            self.parent.accel_box.update()   
                
        if "accept_handshake" in message.keys():
            message.pop("accept_handshake")
            self.parent.UpdateServerConnexionInfo('info',"Connected with MORJ controller")
            
        if "locked" in message.keys():
            self.accel_box.LockStatus = message.pop("locked")
            self.parent.UpdateServerConnexionInfo('dinfo',"Lock status : {}".format(self.accel_box.LockStatus))
            self.accel_box.update()
            
            
            
        poplist = []
        for key in message.keys():
            if key in _discards :
                poplist.append(key)

        [message.pop(key) for key in poplist]        

        if len(message.keys()):

            self.parent.UpdateServerConnexionInfo('error',"Unhandled messages : {}".format(message.keys()))
            print(message.keys())
            

    def handle_packet(self):
        
        RawLine = self.parent.connexion_manager.RecieveThread.Line
        try :
            RawLine = RawLine.replace('\r', '').replace('\n', '')
            ParsedLine = json.loads(RawLine)
        except Exception :#as e: 
            self.parent.UpdateServerConnexionInfo('error',"{}".format(RawLine))
            global_recieve_lock.release() 
            return
            
        try :
            self.check_keys(ParsedLine)
        except Exception as e:
            self.parent.UpdateServerConnexionInfo('error',"{}".format(e))
            
        global_recieve_lock.release()   
    
    
class MORJConnector(QMainWindow):
    
    def __init__(self, parent=None):
        
        super(MORJConnector, self).__init__(parent, Qt.WindowStaysOnTopHint | Qt.X11BypassWindowManagerHint)
        self.setWindowTitle("Princess Maze Network Connector")
        self.CentralWidgetMaze = ConnexionGui(parent=self)
        self.setCentralWidget(self.CentralWidgetMaze)
        
    def closeEvent(self, arg):
        print('Close button pressed')
        
        #close database
        try :
            self.CentralWidgetMaze.connexion_manager.cnx.close()
        except AttributeError :
            pass
        #close arduino server to GUI thread
        try : 
            self.CentralWidgetMaze.connexion_manager.RecieveThread.Exit = 1
            if not self.CentralWidgetMaze.connexion_manager.RecieveThread.isFinished() :
                self.CentralWidgetMaze.connexion_manager.RecieveThread.terminate()            
        except AttributeError :
            pass
        #close arduino server to GUI socket
        try : 
            self.CentralWidgetMaze.connexion_manager.server.close()   
        except AttributeError:
            pass
        #close acquisition manager thread
        try :
            self.CentralWidgetMaze.acquisition_manager.Exit = True
            if not self.CentralWidgetMaze.acquisition_manager.isFinished() :
                self.CentralWidgetMaze.acquisition_manager.terminate()
        except AttributeError:
            pass
        
        sys.exit(0)
        
class ConnexionGui(QWidget):
    
    def __init__(self, parent=None):
        
        super(ConnexionGui, self).__init__()
        
        self.logging_info_viewer = QTextEdit()
        self.logging_info_viewer.setReadOnly(True)
        
        self.outbound = OutboundMessageManager(self)
        self.inbound = InboundMessageManager(self)
        
        self.accel_pannel = AccelControl.AccelControlPannel(self)
        self.accel_pannel.createlayout()
        
        self.accel_graph = AccelGraph.AccelGraphPannel(self)
        self.make_yaw_box()
        self.accel_graph.createlayout()
        
        self.accel_box = AccelControl.AccelInfoBox(self) 
        self.accel_box.createlayout()
        
        self.command_manager = CommandField.CommandFieldEditor(self)
        self.command_manager.createlayout()
        self.connexion_manager = ConnexionHeader.HeaderConnector(global_recieve_lock,self)
        self.connexion_manager.createlayout()
        
        multi_tab_widget = DetachableTabWidget() #QTabWidget()
        multi_tab_widget.addTab(self.accel_pannel, "MORJ control")
        multi_tab_widget.addTab(self.accel_graph, "Yaw graph")
        

                
        self.mainLayout = QGridLayout()
        self.mainLayout.addWidget(self.connexion_manager, 0, 0 , 1 , 2)
        self.mainLayout.addWidget(multi_tab_widget, 1, 0 , 2 , 1)
        self.mainLayout.addWidget(self.logging_info_viewer, 1, 1 , 3 , 1)
        self.mainLayout.addWidget(self.accel_box, 3, 0, 1, 1)
        self.mainLayout.addWidget(self.YawBox, 4, 0, 1, 2)
        self.mainLayout.addWidget(self.command_manager, 5, 0, 1, 2)
        self.setLayout(self.mainLayout)
        
        ############
        
        self.acquisition_manager = AcquisitionController()
        self.acquisition_manager.start()
        
        self.AcquisitionStatus = 0
        self.accel_graph.init_plot()
        

    def make_yaw_box(self):
        
        self.YawBox = QWidget()
        
        layout = QHBoxLayout()
        
        self.YawInverseBox = QCheckBox("Inverse Yaw")
        self.YawInverseBox.setChecked(True)
        self.YawInverseBox.stateChanged.connect(self.accel_graph.SetInversionYaw)
        
        yawcorlabel = QLabel("Yaw offset correction (display only)")
        self.YawCorrection = QLineEdit("0")
        self.YawCorrection.returnPressed.connect(self.accel_graph.UpdateCorrection)
        self.YawCorrection.setMaximumWidth(75)
        
        layout.addWidget(self.YawInverseBox)
        layout.addWidget(yawcorlabel)
        layout.addWidget(self.YawCorrection)
        
        self.YawBox.setLayout(layout)
        
    def UpdateServerConnexionInfo(self,Type,message):
        try :
            if Type == '':
                self.logging_info_viewer.setTextColor(QColor(0, 0, 0))
                self.logging_info_viewer.setFontWeight(75)
                self.logging_info_viewer.append(message)
                
                self.connexion_manager.ServerInfoBox.setTextColor(QColor(0, 0, 0))
                self.connexion_manager.ServerInfoBox.setFontWeight(75)
                self.connexion_manager.ServerInfoBox.setText(message)
                
            elif Type == 'merror':
                self.logging_info_viewer.setTextColor(QColor(232, 58, 0))
                self.logging_info_viewer.setFontWeight(75)
                self.logging_info_viewer.append(message)
                
                self.connexion_manager.ServerInfoBox.setTextColor(QColor(232, 58, 0))
                self.connexion_manager.ServerInfoBox.setTextBackgroundColor(QColor(255, 255, 0))       
                self.connexion_manager.ServerInfoBox.setFontWeight(75)
                self.connexion_manager.ServerInfoBox.setText(message)
                self.connexion_manager.ServerInfoBox.setTextBackgroundColor(QColor(255, 255, 255))
                
            else :
                
                if Type == 'smsg':
                    self.logging_info_viewer.setFontWeight(1)
                    self.logging_info_viewer.setTextColor(QColor(116, 130, 98))
                elif Type == 'data':
                    self.logging_info_viewer.setFontWeight(1)
                    self.logging_info_viewer.setTextColor(QColor(125, 155, 26))
                elif Type == 'error':
                    self.logging_info_viewer.setFontWeight(75)
                    self.logging_info_viewer.setTextColor(QColor(232, 58, 0))
                elif Type == 'info':
                    self.logging_info_viewer.setFontWeight(75)
                    self.logging_info_viewer.setTextColor(QColor(0, 232, 181))
                elif Type == 'dinfo':
                    self.logging_info_viewer.setFontWeight(1)
                    self.logging_info_viewer.setTextColor(QColor(0, 102, 165))
                else:
                    print("fromat error in Type, UpdateServerConnexionInfo")
                    
                self.logging_info_viewer.append(message)
            self.logging_info_viewer.ensureCursorVisible()
            cursor = QTextCursor(self.logging_info_viewer.textCursor())
            cursor.movePosition(cursor.Down, cursor.KeepAnchor,  10)
        except Exception as e:
            print("error showing non string data : {}".format(e))
        
if __name__ == '__main__':
    app = QCoreApplication.instance()
    if app is None:
        app = QApplication(sys.argv)
        app.setStyleSheet("QGroupBox{padding-top:15px; margin-top:-17px}")
    
    # creating main window
    mw = MORJConnector()
    mw.show()
    getattr(mw, "raise")()
    mw.activateWindow()
    # mw.showMaximized()
    sys.exit(app.exec_())