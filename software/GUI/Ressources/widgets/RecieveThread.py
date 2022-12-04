from PyQt5.QtCore import QThread, pyqtSignal
from .SocketPackets import RecieveSizedPacket

class recieveTCP(QThread):
    
    PacketRecieved = pyqtSignal()  
    
    def __init__(self, server, lock):
        super().__init__()
        self.server = server
        self.Line=[]
        self.Exit = 0
        self.global_lock = lock        
             
    def run(self):
        while self.Exit == 0 :      
            
            try :
                buffer = RecieveSizedPacket(self.server)
                if buffer != "":
                    self.global_lock.acquire()
                    self.Line = buffer
                    self.PacketRecieved.emit()
            except Exception as e:
                print(e)
                self.Line = []