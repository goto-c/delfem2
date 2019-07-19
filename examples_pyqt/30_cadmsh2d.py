####################################################################
# Copyright (c) 2019 Nobuyuki Umetani                              #
#                                                                  #
# This source code is licensed under the MIT license found in the  #
# LICENSE file in the root directory of this source tree.          #
####################################################################

import sys

from PySide2.QtWidgets import (QApplication, QVBoxLayout,
                             QWidget, QPushButton)

import PyDelFEM2 as dfm2
import PyDelFEM2.gl
import PyDelFEM2.qt


class Window(dfm2.qt.QW_CadMshFem):
  def __init__(self):
    super(Window, self).__init__()
    self.setWindowTitle("CAD_Mesh")

    self.cadmsh = dfm2.CadMesh2D(edge_length=0.05)
    self.cadmsh.add_polygon([-1, -1, +1, -1, +1, +1, -1, +1])

    super().init_UI()


if __name__ == '__main__':
  app = QApplication(sys.argv)
  window = Window()
  window.show()
  sys.exit(app.exec_())
