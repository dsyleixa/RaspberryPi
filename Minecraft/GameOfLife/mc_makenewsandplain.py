import mcpi.minecraft as minecraft
import mcpi.block as block
import time

def findEdge(xz,inc):
    v=0
    id=0
    while True:
        if xz == 'x':
            id = mc.getBlock(v,0,0)
        else:
            id = mc.getBlock(0,0,v)
        #print(x,id)
        if id==95 or (inc == -1 and v<-256) or (inc == 1 and v>256):
            break
        v+=inc
    
    if inc == 1:
        v-=1
    else:
        v+=1
        
    return v

print('begin')

mc = minecraft.Minecraft.create()

mc.postToChat('starting')
time.sleep(0.5)

minx = findEdge('x',-1)
minz = findEdge('z',-1)
maxx = findEdge('x',1)
maxz = findEdge('z',1)

print('min x',minx)
print('min z',minz)
print('max x',maxx)
print('max z',maxz)

print(maxx-minx+1,maxz-minz+1)

# fill sky
mc.setBlocks(minx,0,minz,maxx,64,maxz,0)

# fill ground
mc.setBlocks(minx,-20,minz,maxx,0,maxz,block.SANDSTONE.id)

mc.postToChat('finished')

print('end')