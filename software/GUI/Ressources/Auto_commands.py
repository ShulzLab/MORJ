

from PyQt5.QtWidgets import QCompleter
from PyQt5.QtCore import QRegExp, QStringListModel

import json

class CommandCompleter :
    
    def __init__(self,path):
        self.path = path
        
    def load_commands(self):
        with open(self.path,'r') as f :
            all_commands = f.readlines()
        commands_str = ''
        all_commands = [commands_str += cmd for cmd in all_commands ]
        all_commands = json.loads(all_commands)
    
        

