import sys, DDG4
from SystemOfUnits import *

class CLICSid:
  def __init__(self):
    self.kernel = DDG4.Kernel()
    self.lcdd   = self.kernel.lcdd()
    self.geant4 = DDG4.Geant4(self.kernel)
    self.kernel.UI = ""
    self.noPhysics()
 
  def loadGeometry(self):
    import os
    install_dir = os.environ['DD4hepINSTALL']
    self.kernel.loadGeometry("file:"+install_dir+"/DDDetectors/compact/SiD.xml")
    return self

  # Example to show how to configure G4 magnetic field tracking
  def setupField(self, quiet=True):
    field = self.geant4.addConfig('Geant4FieldTrackingSetupAction/MagFieldTrackingSetup')
    field.stepper            = "HelixGeant4Runge"
    field.equation           = "Mag_UsualEqRhs"
    field.eps_min            = 5e-05 * mm
    field.eps_max            = 0.001 * mm
    field.min_chord_step     = 0.01 * mm
    field.delta_chord        = 0.25 * mm
    field.delta_intersection = 1e-05 * mm
    field.delta_one_step     = 0.001 * mm
    if not quiet:
      print '+++++> ',field.name,'-> stepper  = ',field.stepper
      print '+++++> ',field.name,'-> equation = ',field.equation
      print '+++++> ',field.name,'-> eps_min  = ',field.eps_min
      print '+++++> ',field.name,'-> eps_max  = ',field.eps_max
      print '+++++> ',field.name,'-> delta_one_step = ',field.delta_one_step
    return field

  # Example to show how to setup random generator
  def setupRandom(self, name, type=None, seed=None, quiet=True):
    rndm = DDG4.Action(self.kernel,'Geant4Random/'+name)
    if seed: rndm.Seed = seed
    if type: rndm.Type = type
    rndm.initialize()
    if not quiet: rndm.showStatus()
    return rndm

  # Example to show how to configure the Geant4 physics list
  def setupPhysics(self, model):
    phys = self.geant4.setupPhysics(model)
    ph = DDG4.PhysicsList(self.kernel,'Geant4PhysicsList/Myphysics')
    # Add bosons to the model (redundant if already implemented by the model)
    ph.addParticleConstructor('G4BosonConstructor')
    # Add leptons to the model (redundant if already implemented by the model)
    ph.addParticleConstructor('G4LeptonConstructor')
    # Add multiple scattering in the material
    ph.addParticleProcess('e[+-]','G4eMultipleScattering',-1,1,1)
    # Add optical physics (RICH dets etc)
    ph.addPhysicsConstructor('G4OpticalPhysics')
    # Interactivity
    ph.enableUI()
    phys.adopt(ph)
    phys.dump()
    return phys

  # No physics list wanted for tests ? See how:
  def noPhysics(self):
    self.geant4.setupPhysics('')
    return self


  # Test runner
  def test_run(self, have_geo=True, have_physics=False):
    self.kernel.configure()
    if have_geo:
      self.kernel.initialize()
      self.kernel.NumEvents = 0
      self.kernel.run()
    self.kernel.terminate()
    sys.exit(0)
