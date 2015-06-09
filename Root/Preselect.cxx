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

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"

#include <TheAccountant/VariableDefinitions.h>

namespace HF = HelperFunctions;
namespace VD = VariableDefinitions;

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

  int num_passJetsLargeR = 0;
  for(const auto jet: *in_jetsLargeR){
    pass_preSel(*jet) = 0;
    if(jet->pt()/1000.  < m_jetLargeR_minPt)  continue;
    if(jet->pt()/1000.  > m_jetLargeR_maxPt)  continue;
    if(jet->m()/1000.   < m_jetLargeR_minMass) continue;
    if(jet->m()/1000.   > m_jetLargeR_maxMass) continue;
    if(jet->eta()       < m_jetLargeR_minEta)  continue;
    if(jet->eta()       > m_jetLargeR_maxEta)  continue;
    if(jet->phi()       < m_jetLargeR_minPhi)  continue;
    if(jet->phi()       > m_jetLargeR_maxPhi)  continue;
    num_passJetsLargeR++;
    pass_preSel(*jet) = 1;
  }

  // only select event if:
  //    m_jet_minNum <= num_passJets <= m_jet_maxNum
  if(!( (m_jetLargeR_minNum <= num_passJetsLargeR)&&(num_passJetsLargeR <= m_jetLargeR_maxNum) )){
    wk()->skipEvent();
    return EL::StatusCode::SUCCESS;
  }

  // get the working point
  static VD::WP bTag_wp = VD::str2wp(m_bTag_wp);

  // for small-R jets, count number of jets that pass standard cuts
  int num_passJets = 0;
  int num_passBJets = 0;
  for(const auto jet: *in_jets){
    pass_preSel(*jet) = 0;
    if(jet->pt()/1000. < m_jet_minPt)  continue;
    if(jet->pt()/1000. > m_jet_maxPt)  continue;
    if(jet->m()/1000.  < m_jet_minMass) continue;
    if(jet->m()/1000.  > m_jet_maxMass) continue;
    if(jet->eta()      < m_jet_minEta)  continue;
    if(jet->eta()      > m_jet_maxEta)  continue;
    if(jet->phi()      < m_jet_minPhi)  continue;
    if(jet->phi()      > m_jet_maxPhi)  continue;
    num_passJets++;
    num_passBJets += static_cast<int>(VD::bTag(jet, bTag_wp));
    pass_preSel(*jet) = 1;
  }

  // only select event if:
  //    m_jet_minNum <= num_passJets <= m_jet_maxNum
  if(!( (m_jet_minNum <= num_passJets)&&(num_passJets <= m_jet_maxNum) )){
    wk()->skipEvent();
    return EL::StatusCode::SUCCESS;
  }

  // only select event if:
  //    m_bjet_minNum <= num_passBJets <= m_bjet_maxNum
  if(!( (m_bjet_minNum <= num_passBJets)&&(num_passBJets <= m_bjet_maxNum) )){
    wk()->skipEvent();
    return EL::StatusCode::SUCCESS;
  }

  static SG::AuxElement::Decorator< int > pass_preSel_jets("pass_preSel_jets");
  static SG::AuxElement::Decorator< int > pass_preSel_jetsLargeR("pass_preSel_jetsLargeR");
  static SG::AuxElement::Decorator< int > pass_preSel_bjets("pass_preSel_bjets");
  pass_preSel_jets(*eventInfo) = num_passJets;
  pass_preSel_jetsLargeR(*eventInfo) = num_passJetsLargeR;
  pass_preSel_bjets(*eventInfo) = num_passBJets;

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Preselect :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Preselect :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Preselect :: histFinalize () { return EL::StatusCode::SUCCESS; }
