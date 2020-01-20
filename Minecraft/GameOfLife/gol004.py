#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

# ver 004

mc = minecraft.Minecraft.create()

GPIO.setmode(GPIO.BCM)
p1 = 8
GPIO.setup(p1, GPIO.IN, GPIO.PUD_DOWN)


bl_1 = block.GRASS.id
bl_0 = block.SANDSTONE.id

msize = 20   # size of game map                         
Tmp = [[0]*(msize+2) for _ in range(msize+2)] # plus 2 for border frame


     

def putField_Glider(x,y,z):  
    mc.setBlock(x+0, y, z+0, bl_0)
    mc.setBlock(x+1, y, z+0, bl_0)
    mc.setBlock(x+2, y, z+0, bl_1)
                
    mc.setBlock(x+0, y, z+1, bl_1)
    mc.setBlock(x+1, y, z+1, bl_0)
    mc.setBlock(x+2, y, z+1, bl_1)
                
    mc.setBlock(x+0, y, z+2, bl_0)
    mc.setBlock(x+1, y, z+2, bl_1)
    mc.setBlock(x+2, y, z+2, bl_1)

    
#####################################
    

def clearField_n(x,y,z, n):
  for i in range(0,n):
    for j in range(0,n):
      mc.setBlock(x+i, y, z+j, bl_0)
      
    
def clearField_radius(x,y,z, size):
  for i in range(-size,size):
    for j in range(-size,size):
      mc.setBlock(x+i, y, z+j, bl_0)

      
#######################################
      

def countNeighbours(x,y,z):
  count = 0
  for i in range(-1,2):
       for j in range (-1,2):
            if(( mc.getBlock(x+i,y,z+j) != bl_0) and (i!=0 or j!=0)):
                 count=count+1
  return count

  
#######################################


def calcGeneration():
    
  aliveNeighbours = 0
  y = 0
  
  for x in range(1,msize):
    #debug
    print("   line ", x, "/",msize-1)  
    for z in range(1,msize):
        
      aliveNeighbours=countNeighbours(x,y,z)
      #debug
      #print(x,z, " alvNbs=",aliveNeighbours)

      if aliveNeighbours < 2:  # cell dies
        Tmp[x][z] = bl_0

      if aliveNeighbours>=2 and aliveNeighbours<=3:  # cell lives
        Tmp[x][z] = mc.getBlock(x,y,z)  

      if aliveNeighbours==3 :  # dead cell resurrects
        if mc.getBlock(x,y,z) == bl_0:
          Tmp[x][z] = bl_1 
          
      if aliveNeighbours > 3:  # cell dies
        Tmp[x][z] = bl_0
          
          
  y=0
  for x in range(0,msize+1):                     # not tested yet
    for z in range(0,msize+1):
      if x==0 or z==0 or x==msize or z==msize:
        Tmp[x][z] = bl_0  
      mc.setBlock(x,y,z, Tmp[x][z])   
  
  
#######################################






try:
  clearField_n(1,0,1, msize)
  mc.setBlock(1,1,1, block.TORCH.id)

  putField_Glider(1,0,1)

 
  
  ######################
  # test, 
   
  for g in range (1,6):
    print()  
    print("Generation= ", g)  
    calcGeneration();


    
    
except KeyboardInterrupt:
  GPIO.cleanup() 
  
