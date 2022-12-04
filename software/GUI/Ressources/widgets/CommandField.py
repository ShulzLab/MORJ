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
Created on Thu Nov 24 11:48:07 2022
@author: Timothe
</div>
"""

from PyQt5.QtWidgets import QWidget, QComboBox, QHBoxLayout

#from PyQt5.QtWidgets import QCompleter
#from PyQt5.QtCore import QRegExp, QStringListModel
#from import QRegExpValidator

class CommandFieldEditor(QWidget):
        
    def __init__(self,parent = None):
        
        super().__init__()
        self.parent = parent
        
    def createlayout(self):
        
        layout = QHBoxLayout()
        self.send_box = QComboBox()
        
        #for i in range(len(default_commands_list)):
        #   default_commands_list[i] = "{" + default_commands_list[i] + ":  }"
            
            
        #self.send_box.addItems(default_commands_list)
        self.send_box.setEditable(True)
        #self.TextSendBox.returnPressed.connect(lambda : self.SendCommand())
        self.send_box.lineEdit().returnPressed.connect(self.parent.outbound.send)
        
        layout.addWidget(self.send_box)      
        
        self.setLayout(layout)