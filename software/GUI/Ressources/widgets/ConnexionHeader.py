
import socket, mysql.connector
from PyQt5.QtGui import QRegExpValidator
from PyQt5.QtCore import QRegExp, QStringListModel

from PyQt5.QtWidgets import QWidget, QGridLayout, QLineEdit, QCompleter, QPushButton, QSizePolicy, QLabel, QTextEdit



from .RecieveThread import recieveTCP

class HeaderConnector(QWidget):
    
    def __init__(self,global_read_lock,parent=None,):
        
        super().__init__()
        self.parent = parent
        self.global_read_lock = global_read_lock
    
        self.serverco = False
        self.DbCxSucess = False
    
    def createlayout(self):
            
        self.IpField = QLineEdit("157.136.60.198")
        regex = QRegExp("[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}")
        validator = QRegExpValidator(regex)
        self.IpField.setValidator(validator)
        self.IpField.setMaximumWidth(145)
        
        model = QStringListModel()
        model.setStringList(["157.136.60.198", "127.0.0.1", "0.0.0.0"])
        completer = QCompleter()
        completer.setModel(model)
        self.IpField.setCompleter(completer)
        Iplabel = QLabel("Arduino Server IP")
        Iplabel.setBuddy(self.IpField)
        
        self.ConnectIP = QPushButton("ConnectMORJ")
        self.ConnectIP.setMaximumWidth(150)
        self.ConnectIP.setMaximumHeight(50)
        self.ConnectIP.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Expanding)
        self.ConnectIP.clicked.connect(self.connect_to_server)
        
        
        self.ConnectDB = QPushButton("ConnectMySQL")
        self.ConnectDB.setMaximumWidth(150)
        self.ConnectDB.setMaximumHeight(50)
        self.ConnectDB.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Expanding)
        self.ConnectDB.clicked.connect(self.connect_to_database)

#        self.ServerInfoBox = QLineEdit()
        self.ServerInfoBox = QTextEdit()
        self.ServerInfoBox.setMaximumHeight(35)
        self.ServerInfoBox.setReadOnly(True)
        servinflabel = QLabel("Connections Status")
        servinflabel.setBuddy(self.ServerInfoBox)
        
        layout = QGridLayout()
        
        layout.setSpacing(5)
        layout.setContentsMargins(5,5,5,5)
        
        layout.addWidget(Iplabel, 0, 0, 1, 1)
        layout.addWidget(self.IpField, 1, 0, 1, 1)
        layout.addWidget(self.ConnectIP, 0, 2, 2, 1)
        layout.addWidget(self.ConnectDB, 0, 3, 2, 1)
        layout.addWidget(servinflabel, 0, 4, 1, 4)
        layout.addWidget(self.ServerInfoBox, 1, 4, 1, 4)
        
#        layout.addStretch(1)   
        self.setLayout(layout)
        
    def connect_to_server(self):
        
        try:
            self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.IP = self.IpField.text()
            self.server.connect((self.IP, 1112))
            self.parent.UpdateServerConnexionInfo('',"Sucessfully reached TCP socket server, waiting serial ports opening.")
            
            
            
            
            

        except Exception as e: 
            print(e)
            self.parent.UpdateServerConnexionInfo('merror',"Connection not established")
            return
        
        self.serverco = True
        self.IpField.setDisabled(True)
        self.ConnectIP.setDisabled(True)
        
        self.RecieveThread = recieveTCP(self.server,self.global_read_lock)
        self.RecieveThread.PacketRecieved.connect(self.parent.inbound.handle_packet)
        self.RecieveThread.start() 
                
        if self.DbCxSucess and self.serverco :
            self.hide()
 
    def connect_to_database(self):
        
        try:
            self.cnx = mysql.connector.connect(host="157.136.60.198",user="RedCentral",passwd="21User*91!",db="maze",connection_timeout=3)
            self.DbCxSucess=True
        except:
            try:
                self.cnx = mysql.connector.connect(host="127.0.0.1",user="RedCentral",passwd="21User*91!",db="maze",connection_timeout=3)
                self.DbCxSucess=True
            except:
                self.DbCxSucess=False
                print("Couldn't connect to database")
                
        if self.DbCxSucess :
            self.ConnectDB.setDisabled(True)
        
        if self.DbCxSucess and self.serverco :
            self.hide()
                
