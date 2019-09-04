"""

   Perform a material scan using Geant4 shotting geantinos

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import, unicode_literals
import os, sys, time, logging, DDG4
from DDG4 import OutputLevel as Output

def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry("file:"+install_dir+"/examples/OpticalSurfaces/compact/OpNovice.xml")

  logging.basicConfig(format='%(levelname)s: %(message)s')
  logger = logging.getLogger(__name__)
  logger.setLevel(logging.INFO)

if __name__ == "__main__":
  run()
