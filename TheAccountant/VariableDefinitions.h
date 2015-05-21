#ifndef TheAccountant_VariableDefinitions_H
#define TheAccountant_VariableDefinitions_H

// EDM includes
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMissingET/MissingETContainer.h"

/* Caveats: input containers are assumed sorted */
namespace VariableDefinitions {
  // Effective Mass calculated using exclusive or inclusive definition depending on args
  //    Exclusive: Meff(met, jets, 4, 0, 0)
  //    Inclusive: Meff(met, jets, jets->size(), muons, els)
  float Meff(const xAOD::MissingET* met, const xAOD::JetContainer* jets, int numJets, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els);

  // HT calculated using all jets, will include muons and/or electrons if specified
  float HT(const xAOD::JetContainer* jets, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els);

  // Transverse Mass calculated using MET and leading lepton
  float mT(const xAOD::MissingET* met, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els);
}

#endif
