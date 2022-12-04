import os
import json, datetime, queue

# fields_desc = {"tnb": ,
#                "tt":,
#                "t":,
#                "y":,
#                "p":,
#                "r":,
#                "ys":,
#                "a":,
#                "g":,
#                "m":,
#                "cy": "last command issued to motor. In steps (integer)"  ,
#                "c":  "last command issued to motor. In steps (integer)"    ,     
#                "lsc":  "last summed command. In steps (integer)"    ,     
#                "csc":  "current summed command. In steps (integer)"         
#                }

class JsonGrabber():
    
    def __init__(self,path):
        self.path = path
        self.layers_opened = queue.LifoQueue()
        self.initrial = False
        self.in_trial = True
    
    def open(self,sup = None):
        if sup is None :
            sup = 'session'
            section = 'main_session_'
        else : 
            section = "trial" + str(sup) + '_'
            sup = 'trial' + str(sup) + '_'
            
        self.filename = datetime.datetime.now().strftime("IMU_data_{0}%y%m%d_%H.%M.%S.json".format(sup))
        if not os.path.isdir(self.path):
            os.makedirs(self.path)
        
        self.filehandle = open(os.path.join(self.path,self.filename),"w")
        
        self._wline("{")
        
        self.section_count = [0]
        self.section_depth_index = 0
        
        self.layers_opened.put("o")#opening a new object (outermost object = file)
        
        self.open_sub("o","header")
        self.write( section[:-1] ,"context") #writing the "header"
        self.write( datetime.datetime.now().__str__() ,"start_time")#writing the "header"
        self.write( "TimJson_V1" ,"version" )#writing the "header"
        # self.open_sub("o","fields")
        # self.write( "TimJson_V1" ,"version" )#writing the "header"
        # self.close_sub()
        self.close_sub()
        
        self._separator()
        
        #self.open_sub("o","content")#opening a new object "content"
        self.open_sub("a","content")#opening a new array to write the data lines
        
    def write(self,data,key = None):
        if self.section_count[self.section_depth_index] == 0 :
            self.section_count[self.section_depth_index] = 1
        else :
            self._separator()
        if key is None :
            dump = json.dumps(data,sort_keys=True)
        else :
            dump = r'"' + key + r'"' + ": "+ json.dumps(data,sort_keys=True)
        self.filehandle.write(dump + "\n")
        
    
    def open_sub(self,_type, key = None ):
        if key is not None :
            self._wline(r'"' + key + r'": ')
        if _type == "a" :
            self._wline("[")
        else :
            self._wline("{")
        self.layers_opened.put(_type)
        self.section_count.append(0)
        self.section_depth_index = self.section_depth_index + 1
        
    def close_sub(self):
        _type = self.layers_opened.get()
        if _type == "a" :
            self._wline("]")
        else :
            self._wline("}")
        self.section_count.pop(self.section_depth_index)
        self.section_depth_index = self.section_depth_index - 1
    
    def close(self):
        while not self.layers_opened.empty():
            self.close_sub()
        self.filehandle.close()
    
    def _separator(self):
        self.filehandle.write(",")
        
    def _wline(self,line):
        self.filehandle.write(line + "\n")
        
    def prettify(self):
        
        with open(os.path.join(self.path,self.filename), 'r') as handle:
            parsed = json.load(handle)
        with open(os.path.join(self.path,self.filename), 'w') as handle:
            json.dump(parsed, handle, indent=1, sort_keys=False)