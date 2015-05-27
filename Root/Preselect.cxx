#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TheAccountant/Preselect.h>

// Infrastructure includes
#include "xAODRootAccess/Init.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"
//#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"
#include "xAODBTagging/BTagging.h"

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"

namespace HF = HelperFunctions;

// this is needed to distribute the algorithm to the workers
ClassImp(Preselect)
Preselect :: Preselect () {}

EL::StatusCode Preselect :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("Preselect").ignore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Preselect :: histInitialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Preselect :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Preselect :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode Preselect :: initialize ()
{
  // assign m_event and m_store
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Preselect :: execute ()
{
  const xAOD::EventInfo*                eventInfo     (nullptr);
  const xAOD::JetContainer*             in_jetsLargeR (nullptr);
  const xAOD::JetContainer*             in_jets       (nullptr);
  const xAOD::MissingETContainer*       in_missinget  (nullptr);
  const xAOD::ElectronContainer*        in_electrons  (nullptr);
  const xAOD::MuonContainer*            in_muons      (nullptr);
  const xAOD::TauJetContainer*          in_taus       (nullptr);
  const xAOD::PhotonContainer*          in_photons    (nullptr);
  //const xAOD::TruthParticleContainer*   in_truth    (nullptr);

  // start grabbing all the containers that we can
  RETURN_CHECK("Preselect::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  RETURN_CHECK("Preselect::execute()", HF::retrieve(in_jetsLargeR,      m_inputLargeRJets,        m_event, m_store, m_debug), "Could not get the inputLargeRJets container.");
  RETURN_CHECK("Preselect::execute()", HF::retrieve(in_jets,     m_inputJets,       m_event, m_store, m_debug), "Could not get the inputJets container.");
  if(!m_inputMET.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_missinget, m_inputMET,         m_event, m_store, m_debug), "Could not get the inputMET container.");
  if(!m_inputElectrons.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_electrons, m_inputElectrons,   m_event, m_store, m_debug), "Could not get the inputElectrons container.");
  if(!m_inputMuons.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_muons,     m_inputMuons,       m_event, m_store, m_debug), "Could not get the inputMuons container.");
  if(!m_inputTauJets.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_taus,      m_inputTauJets,     m_event, m_store, m_debug), "Could not get the inputTauJets container.");
  if(!m_inputPhotons.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_photons,   m_inputPhotons,     m_event, m_store, m_debug), "Could not get the inputPhotons container.");

  // retrieve CalibMET_RefFinal for METContainer
  xAOD::MissingETContainer::const_iterator met_final = in_missinget->find("Final");
  if (met_final == in_missinget->end()) {
    Error("execute()", "No RefFinal inside MET container" );
    return EL::StatusCode::FAILURE;
  }

  static SG::AuxElement::Decorator< int > pass_preSel("pass_preSel");
  pass_preSel(*eventInfo) = 0;

  int num_passJetsLargeR = 0;
  for(const auto jet: *in_jetsLargeR){
    if(jet->pt()/1000.  < m_jetLargeR_minPt)  continue;
    if(jet->pt()/1000.  > m_jetLargeR_maxPt)  continue;
    if(jet->m()/1000.   < m_jetLargeR_minMass) continue;
    if(jet->m()/1000.   > m_jetLargeR_maxMass) continue;
    if(jet->eta()       < m_jetLargeR_minEta)  continue;
    if(jet->eta()       > m_jetLargeR_maxEta)  continue;
    if(jet->phi()       < m_jetLargeR_minPhi)  continue;
    if(jet->phi()       > m_jetLargeR_maxPhi)  continue;
    num_passJetsLargeR++;
  }

  // only select event if:
  //    m_jet_minNum <= num_passJets <= m_jet_maxNum
  if(num_passJetsLargeR < m_jetLargeR_minNum) return EL::StatusCode::SUCCESS;
  if(num_passJetsLargeR > m_jetLargeR_maxNum) return EL::StatusCode::SUCCESS;

  // for small-R jets, count number of jets that pass standard cuts
  int num_passJets = 0;
  //    but also count how many of those that pass the cuts are also b-tagged
  int num_passBJets = 0;
  for(const auto jet: *in_jets){
    if(jet->pt()/1000. < m_jet_minPt)  continue;
    if(jet->pt()/1000. > m_jet_maxPt)  continue;
    if(jet->m()/1000.  < m_jet_minMass) continue;
    if(jet->m()/1000.  > m_jet_maxMass) continue;
    if(jet->eta()      < m_jet_minEta)  continue;
    if(jet->eta()      > m_jet_maxEta)  continue;
    if(jet->phi()      < m_jet_minPhi)  continue;
    if(jet->phi()      > m_jet_maxPhi)  continue;
    num_passJets++;
    if(jet->btagging()->MV1_discriminant() < m_jet_MV1) continue;
    num_passBJets++;
  }

  // only select event if:
  //    m_jet_minNum <= num_passJets <= m_jet_maxNum
  if(num_passJets < m_jet_minNum) return EL::StatusCode::SUCCESS;
  if(num_passJets > m_jet_maxNum) return EL::StatusCode::SUCCESS;
  // and if:
  //    m_bjet_minNum <= num_passBJets <= m_bjet_maxNum
  if(num_passBJets < m_bjet_minNum) return EL::StatusCode::SUCCESS;
  if(num_passBJets > m_bjet_maxNum) return EL::StatusCode::SUCCESS;

  pass_preSel(*eventInfo) = 1;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Preselect :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Preselect :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Preselect :: histFinalize () { return EL::StatusCode::SUCCESS; }
