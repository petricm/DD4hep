
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ILDExRunAction_h
#define ILDExRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Run;

class ILDExRunAction : public G4UserRunAction {
public:
  ILDExRunAction();
  virtual ~ILDExRunAction();

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  void fillPerEvent(G4double ESupport, G4double ESensitive, G4double LSupport, G4double LSensitive, G4double AngleSupport,
                    G4double AngleSensitive);

private:
  G4double sumESupport, sum2ESupport;
  G4double sumESensitive, sum2ESensitive;

  G4double sumLSupport, sum2LSupport;
  G4double sumLSensitive, sum2LSensitive;

  G4double sumAngleSupport, sum2AngleSupport;
  G4double sumAngleSensitive, sum2AngleSensitive;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
