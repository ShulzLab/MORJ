# -*- coding: utf-8 -*-
"""

Boilerplate:
A one line summary of the module or program, terminated by a period.

Rest of the description. Multiliner

<div id = "exclude_from_mkds">
Excluded doc
</div>

<div id = "content_index">

<div id = "contributors">
Created on Thu Nov 24 11:48:26 2022
@author: Timothe
</div>
"""

from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QWidget, QPushButton, QLineEdit, QCheckBox, QTextEdit, QComboBox, QGridLayout, QSlider, QGroupBox, QLabel
from .DetachableTabs import DetachableTabWidget

class AccelControlPannel(QWidget):
    
    def __init__(self,parent=None):
        
        super().__init__()
        self.parent = parent
        
    def createlayout(self):
                
        coupling_button = QPushButton("CopleIMU")
        coupling_button.setMaximumWidth(75)
        coupling_button.clicked.connect(self.parent.outbound.ask_coupling)
        
        DeCouplingButton = QPushButton("DecopIMU")
        DeCouplingButton.setMaximumWidth(75)
        DeCouplingButton.clicked.connect(self.parent.outbound.ask_decoupling)
        
        
        LockButton = QPushButton("Lock")
        LockButton.setMaximumWidth(75)
        LockButton.clicked.connect(self.parent.outbound.ask_lock)
        
        UnLockButton = QPushButton("UnLock")
        UnLockButton.setMaximumWidth(75)
        UnLockButton.clicked.connect(self.parent.outbound.ask_unlock)
        
    
        MoveLeftButton = QPushButton("Turn CW")
        MoveLeftButton.setShortcut('Ctrl+Left')
        MoveLeftButton.setMaximumWidth(75)
        MoveLeftButton.clicked.connect(self.parent.outbound.ask_CW_turn)
        
        MoveRightButton = QPushButton("Turn CCW")
        MoveRightButton.setShortcut('Ctrl+Right')
        MoveRightButton.setMaximumWidth(75)
        MoveRightButton.clicked.connect(self.parent.outbound.ask_CCW_turn)

        
        ZeroMotorButton = QPushButton("set OREF")
        ZeroMotorButton.setMaximumWidth(75)
        ZeroMotorButton.clicked.connect(self.parent.outbound.ask_setting_OREF)      
        
        ZeroAccelButton = QPushButton("Zero IMU")
        ZeroAccelButton.setMaximumWidth(75)
        ZeroAccelButton.clicked.connect(self.parent.outbound.ask_setting_IMU_zero)  
        
        AcquireButton = QPushButton("Acquire")
        AcquireButton.clicked.connect(self.parent.outbound.ask_acquire)
        AcquireButton.setMaximumWidth(75)
        
        StopAcquireButton = QPushButton("Stop")
        StopAcquireButton.clicked.connect(self.parent.outbound.ask_stopacquire)
        StopAcquireButton.setMaximumWidth(75)
        

        layout = QGridLayout()
        layout.setContentsMargins(5, 5, 5, 5)
        layout.setSpacing(5)
        
        
        layout.addWidget(DeCouplingButton, 0, 0, 1, 1)
        layout.addWidget(coupling_button, 0, 1, 1, 1)
        
        layout.addWidget(LockButton, 1, 0, 1, 1)
        layout.addWidget(UnLockButton, 1, 1, 1, 1)

        layout.addWidget(ZeroAccelButton, 2, 0, 1, 1)
        layout.addWidget(ZeroMotorButton, 2, 1, 1, 1)
        
        layout.addWidget(AcquireButton, 3, 0, 1, 1)
        layout.addWidget(StopAcquireButton, 3, 1, 1, 1)
        
        layout.addWidget(MoveLeftButton, 4, 0, 1, 1)
        layout.addWidget(MoveRightButton, 4, 1, 1, 1)
        
        self.setLayout(layout)
        
class AccelInfoBox(QWidget):
    
    def __init__(self,parent=None):
        
        super().__init__()
        self.parent = parent
        
        
        self.LockStatus = 0

        self.TommandTotal = 0
        self.LockStatus = 0
        self.SummedCommands = 0
        self.Turns = 0
        self.CurrentPos = 0
        
        self.roll = 0
        self.pitch = 0
        
    
    def createlayout(self):
        
        self.LabelTab = QGroupBox()
        
        labelbox = QGridLayout()
        
        self.LockedIndicator = QLabel("-")
        self.MotPosIndicator = QLabel("-")
        self.TurnsIndicator = QLabel("-")
        self.CurrentPosIndicator = QLabel("-")
        self.ROLPITCH = QLabel("-")
        
        self.LockedIndicator.setStyleSheet('color: red;font-size : 14px ')
        
        self.MotPosIndicator.setStyleSheet('color: blue; font-size : 14px ')
        
        self.TurnsIndicator.setStyleSheet('color: blue; font-size : 14px ')
        
        self.CurrentPosIndicator.setStyleSheet('color: blue; font-size : 14px ')
        
        self.ROLPITCH.setStyleSheet('color: green; font-size : 14px ')
        
        labelbox.addWidget(QLabel("Lock Status:"), 0, 0, 1, 1)
        labelbox.addWidget(self.LockedIndicator, 0,  1, 1, 1)
        labelbox.addWidget(QLabel("Summed Motor Pos :"), 1, 0, 1, 1)        
        labelbox.addWidget(self.MotPosIndicator, 1, 1, 1, 1)   
        labelbox.addWidget(QLabel("Turns : "), 2, 0, 1, 1)
        labelbox.addWidget(self.TurnsIndicator, 2, 1, 1, 1)
        labelbox.addWidget(QLabel("Summed Sensor Pos : "), 3, 0, 1, 1)
        labelbox.addWidget(self.CurrentPosIndicator, 3, 1, 1, 1)
        labelbox.addWidget(self.ROLPITCH, 4, 0, 1, 1)
    
        self.setMinimumWidth(350)
        self.setLayout(labelbox)
        
        
    def update(self):
        
        self.LockedIndicator.setText(str(self.LockStatus))
        self.MotPosIndicator.setText(str(self.SummedCommands))
        self.TurnsIndicator.setText(str(self.Turns))
        self.CurrentPosIndicator.setText(str(self.CurrentPos))
        
        self.ROLPITCH.setText("Roll: " + str(self.roll) + " Pitch: "+ str(self.pitch))          
              
        