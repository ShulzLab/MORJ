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
Created on Thu Nov 24 11:47:29 2022
@author: Timothe
</div>
"""

import math
import numpy as np
from PyQt5.QtWidgets import QWidget, QHBoxLayout

#from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph as pg

class AccelGraphPannel(QWidget):
    
    def __init__(self,parent=None):
        
        super().__init__()
        self.parent = parent
        
    def createlayout(self):
        
#        self.plot = pg.plot()
        self.plot = pg.PlotWidget()
        self.plot.setAspectLocked()
        
        layout = QHBoxLayout()
        
        layout.addWidget(self.plot)
                
        self.setLayout(layout)
        
        self.UpdateCorrection()
        self.SetInversionYaw()
        
    def init_plot(self):
        
        self.plot.addLine(x=0, pen=0.2)
        self.plot.addLine(y=0, pen=0.2)
        
        self.Polarplot(0,50)

    
    def convertYawTorad(self,Yaw):
        
        if Yaw is None :
            return None
        
        Yaw = Yaw - self.YawZero
        if Yaw >= 180 :
            Yaw = -180 + (Yaw -180)
        elif Yaw <= -180:
            Yaw = 180 + (Yaw + 180)
          
        if self.InvertYaw : 
            Yaw = - Yaw
        
        if Yaw < 0 :
            RadYaw = math.radians(Yaw+360)
        else : 
            RadYaw = math.radians(Yaw)
            
        return RadYaw
    
    
    def Polarplot(self,Yaw,InitSize):
        
        self.plot.clear()
        
        angle = 90
        incidence = 160
        
        self.curve = self.plot.plot()
        
        self.pitchcurve = self.plot.plot()
        self.rollcurve = self.plot.plot()
        self.motorposcurve = self.plot.plot()
        
        RadYaw = self.convertYawTorad(Yaw)
        radius = InitSize
        
        self.AccelArray = np.zeros((InitSize,2))
        self.AccelArray.fill(np.nan)
        
        self.RadMposArray = np.full_like(self.AccelArray,np.nan)
        self.RadMtarArray = np.full_like(self.AccelArray,np.nan)
        
        self.AccelArray[0,0] = RadYaw
        self.AccelArray[0,1] = radius
        
        self.RadMposArray[0,0] = 0
        self.RadMposArray[0,1] = radius
                
        for r in range(2, InitSize+10, int((InitSize+10)/5)):
            circle = pg.QtGui.QGraphicsEllipseItem(-r, -r, r * 2, r * 2)
            circle.setPen(pg.mkPen(color=(100, 100, 155)))
            self.plot.addItem(circle)
        
        self.plot.disableAutoRange()
        
        Centerline = pg.QtGui.QGraphicsLineItem(0, 0, 0 ,InitSize)
        Centerline.setPen(pg.mkPen(color=(155, 155, 155)))
        self.plot.addItem(Centerline)
        
        
        x1 = InitSize * np.cos(math.radians(angle-(1/2*incidence)))
        y1 = InitSize * np.sin(math.radians(angle-(1/2*incidence)))
        BoundLine1 = pg.QtGui.QGraphicsLineItem(0, 0, x1 ,y1)
        BoundLine1.setPen(pg.mkPen(color=(155, 100, 100)))
        self.plot.addItem(BoundLine1)
        
        x2 = InitSize * np.cos(math.radians(angle+(1/2*incidence)))
        y2 = InitSize * np.sin(math.radians(angle+(1/2*incidence)))
        BoundLine2 = pg.QtGui.QGraphicsLineItem(0, 0, x2 ,y2)
        BoundLine2.setPen(pg.mkPen(color=(155, 100, 100)))
        self.plot.addItem(BoundLine2)
            
#        x = radius * np.cos(theta)
#        y = radius * np.sin(theta)
        PolarAccelArray = np.zeros((self.AccelArray.shape[0],2))
        PolarAccelArray.fill(np.nan)
        
        PolarAccelArray[0,0] = self.AccelArray[0,1] * np.cos(self.AccelArray[0,0])
        PolarAccelArray[0,1] = self.AccelArray[0,1] * np.sin(self.AccelArray[0,0])
        
        PolarMotorArray = np.zeros((self.RadMposArray.shape[0],2))
        PolarMotorArray.fill(np.nan)
        
        PolarMotorArray[0,0] = self.RadMposArray[0,1] * np.cos(self.RadMposArray[0,0])
        PolarMotorArray[0,1] = self.RadMposArray[0,1] * np.sin(self.RadMposArray[0,0])
        
        
        self.plotText = pg.TextItem(text='Degrees', color=(200, 255, 200), html=None, anchor=(0, 0), border=None, fill=None, angle=0, rotateAxis=None)
        print(self.plotText.viewPos())
        self.plotText.setPos(-(InitSize+((0/8)*InitSize)), (InitSize+((0/8)*InitSize)))
        self.curve.setData(PolarAccelArray)
        self.curve.setPen(pg.mkPen(color=(50, 255, 50),width=4.5))
        
        self.motorposcurve.setData(PolarMotorArray)
        self.motorposcurve.setPen(pg.mkPen(color=(255, 0, 255),width=4.5))
        self.plot.addItem(self.plotText)
        
    def UpdatePolarplot(self,Yaw,Mpos = None):
        
        #Yaw = Yaw - 180
        RadYaw = self.convertYawTorad(Yaw)
        radius = self.AccelArray.shape[0]
        
        for Index in range(self.AccelArray.shape[0]):
            if not np.isnan(self.AccelArray[Index,1]):
                self.AccelArray[Index,1] = self.AccelArray[Index,1]-1
        
        self.AccelArray = np.roll(self.AccelArray, 1, axis = 0)
        
        self.AccelArray[0,0] = RadYaw
        self.AccelArray[0,1] = radius
        
        PolarAccelArray = np.zeros((self.AccelArray.shape[0],2))
        PolarAccelArray.fill(np.nan)
        
        for Index in range(self.AccelArray.shape[0]):
            PolarAccelArray[Index,0] = self.AccelArray[Index,1] * np.cos(self.AccelArray[Index,0])
            PolarAccelArray[Index,1] = self.AccelArray[Index,1] * np.sin(self.AccelArray[Index,0])

        self.curve.setData(PolarAccelArray)
        
        if RadYaw is None :
            self.plotText.setText('Current angle: \nNone\nLock :\n{}'.format(self.parent.accel_box.LockStatus))
        else :
            RadYaw = math.degrees(RadYaw)
            if (RadYaw > 180):
                RadYaw = RadYaw -360
            
            self.plotText.setText('Current angle: \n{:5.2f}\nZero:{:5.2f}\nLock :\n{}'.format(RadYaw, self.YawZero, self.parent.accel_box.LockStatus))
            
        if Mpos is not None :
            MotYaw = self.convertYawTorad(Mpos)
            
            for Index in range(self.RadMposArray.shape[0]):
                if not np.isnan(self.RadMposArray[Index,1]):
                    self.RadMposArray[Index,1] = self.RadMposArray[Index,1]-1
                    
            self.RadMposArray = np.roll(self.RadMposArray, 1, axis = 0)
            self.RadMposArray[0,0] = MotYaw
            self.RadMposArray[0,1] = radius
            
            PolarMotorArray = np.zeros((self.RadMposArray.shape[0],2))
            PolarMotorArray.fill(np.nan)   
            
            for Index in range(self.RadMposArray.shape[0]):
                PolarMotorArray[Index,0] = self.RadMposArray[Index,1] * np.cos(self.RadMposArray[Index,0])
                PolarMotorArray[Index,1] = self.RadMposArray[Index,1] * np.sin(self.RadMposArray[Index,0])
            
  
            self.motorposcurve.setData(PolarMotorArray)
        
    def summed_steps_to_yaw(self,summed_steps):
        return (((summed_steps%9600)/9600)*360)-180

    def summed_yaw_to_yaw(self,summed_yaw):
        return (summed_yaw%360)-180	

    def UpdateCorrection(self):
        
        try : 
            self.YawZero = float(self.parent.YawCorrection.text()) + 90
            self.parent.UpdateServerConnexionInfo('info', "Zero Updated")
        except Exception as e:
            self.parent.UpdateServerConnexionInfo('merror', "Error setting Zero : {}".format(e))

    def SetInversionYaw(self):
        
        if self.parent.YawInverseBox.isChecked() :
            self.InvertYaw = 1
            self.YawZero = -self.YawZero
            
        else :
            self.InvertYaw = 0
            self.YawZero = -self.YawZero
            