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
Created on Fri Nov 25 09:00:09 2022
@author: Timothe
</div>
"""

from .widgets.JsonGrabber import JsonGrabber
from PyQt5.QtCore import QThread
import os, datetime, queue

class AcquisitionController(QThread):
    
    def __init__(self):
        
        super().__init__()
        
        self.main_output_path = r"O:\Timothe\DATA\IMU\CURRENT_RECORDS"
        if not os.path.exists(self.main_output_path):
            self.main_output_path = r"C:\Users\Timothe\Desktop\IMU_save_tests"
        
        self.current_output_path = self.main_output_path
        
        self.queue = queue.Queue()
        
        self.session_grabber = None
        self.trial_grabber = None
        
        self.Exit = False
    
    def open_session_grabber(self):
        
        try : 
            self.session_grabber.close()
        except Exception :
            pass
        
        self.current_output_path = os.path.join(self.main_output_path, datetime.datetime.now().strftime("%y%m%d_%H.%M.%S"))    
        
        self.session_grabber = JsonGrabber(self.current_output_path)
        self.session_grabber.open()
        
    def close_session_grabber(self):
        try : 
            self.session_grabber.close()
            self.session_grabber.prettify()
        except Exception :
            pass
        
        try : 
            self.trial_grabber.close()
            self.trial_grabber.prettify()
        except Exception :
            pass
        
        self.session_grabber = None
        self.trial_grabber = None
        
    def open_trial_grabber(self,trialno):
        try : 
            self.trial_grabber.close()
        except Exception :
            pass
        
        self.trial_grabber = JsonGrabber(self.current_output_path)
        self.trial_grabber.open(trialno)
        
    def close_trial_grabber(self):
        try : 
            self.trial_grabber.close()
            self.trial_grabber.prettify()
        except Exception :
            pass
        
        self.trial_grabber = None
        
    def write_to_files(self,data):
        
        try : 
            self.session_grabber.write(data)
        except Exception :
            pass
        
        try : 
            self.trial_grabber.write(data)
        except Exception :
            pass
    
    def run(self):
        
        while self.Exit == False :
            dataline = self.queue.get()
            try:
                data_dict = dict(dataline)
            except Exception:
                continue
            
            if "sstart" in data_dict.keys():
                self.open_session_grabber()
                data_dict.pop("sstart")
                
            if "sstop" in data_dict.keys():
                self.close_session_grabber()
                data_dict.pop("sstop")
                
            if "tstart" in data_dict.keys():
                trial_no = int(data_dict.pop("tstart"))
                self.open_trial_grabber(trial_no)
                
            if "tstop" in data_dict.keys():
                self.close_trial_grabber()
                data_dict.pop("tstop")
                
            self.write_to_files(data_dict)
        
        
        
        
        
        