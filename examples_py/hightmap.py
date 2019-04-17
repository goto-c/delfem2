from OpenGL.GL import *

import sys
sys.path.append("../module_py")
import dfm2

import numpy, math

def main():
  A0 = numpy.zeros((16,32))
  for iy in range(A0.shape[0]):
    for ix in range(A0.shape[1]):
      A0[iy,ix] = 5*math.sin(ix*0.4)*math.cos(iy*0.6) + 5

  msh = dfm2.mesh_grid((A0.shape[1],A0.shape[0]))
  msh.np_pos = numpy.hstack((msh.np_pos,A0.reshape((-1,1))))

  axis = dfm2.AxisXYZ(32)
  dfm2.winDraw3d([msh,axis],(400,400))


if __name__ == "__main__":
  main()