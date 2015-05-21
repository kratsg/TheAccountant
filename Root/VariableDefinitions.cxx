#include <TheAccountant/VariableDefinitions.h>

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"

// P4Helpers
#include <FourMomUtils/xAODP4Helpers.h>

// for static casting
#include "xAODBase/IParticleContainer.h"

// c++ includes
#include <math.h>

namespace VD = VariableDefinitions;

float VD::Meff(const xAOD::MissingET* met, const xAOD::JetContainer* jets, int numJets, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els){
  float meff(0.0);

  for(int i=0; i<numJets; i++)
    meff += jets->at(i)->pt();

  if(muons)
    for(auto muon: *muons)
      meff += muon->pt();

  if(els)
    for(auto el: *els)
      meff += el->pt();

  meff += met->met();

  return meff;
}

float VD::HT(const xAOD::JetContainer* jets, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els){
  float ht(0.0);

  for(auto jet: *jets)
    ht += jet->pt();

  if(muons)
    for(auto muon: *muons)
      ht += muon->pt();

  if(els)
    for(auto el: *els)
      ht += el->pt();

  return ht;
}

float VD::mT(const xAOD::MissingET* met, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els){
  float mt(0.0);

  // if no leptons passed, return 0.0
  if(!muons && !els) return mt;

  // get leading lepton
  const xAOD::IParticle* leadingLepton(nullptr);
  if(muons && !els)
    leadingLepton = static_cast<const xAOD::IParticle*>(muons->at(0));
  else if(!muons && els)
    leadingLepton = static_cast<const xAOD::IParticle*>(els->at(0));
  else
    if(muons->at(0)->pt() > els->at(0)->pt())
      leadingLepton = static_cast<const xAOD::IParticle*>(muons->at(0));
    else
      leadingLepton = static_cast<const xAOD::IParticle*>(els->at(0));

  mt = 2*leadingLepton->pt()*met->met()*(1-cos(xAOD::P4Helpers::deltaPhi(leadingLepton, met)));
  return sqrt(fabs(mt));
}
