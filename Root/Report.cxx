#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TheAccountant/Report.h>

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
ClassImp(Report)

Report :: Report () {}

EL::StatusCode Report :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("Report").ignore();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: histInitialize () {
  // initialize all histograms here

  m_jetPlots["all_jets"]        = new TheAccountant::JetHists( "all_jets/" );
  m_jetPlots["all_bjets"]       = new TheAccountant::JetHists( "all_bjets/" );

  if(m_passPreSel){
    m_jetPlots["presel_jets"]   = new TheAccountant::JetHists( "presel_jets/" );
    m_jetPlots["presel_bjets"]  = new TheAccountant::JetHists( "presel_bjets/" );
  }

  for(auto jetPlot: m_jetPlots){
    jetPlot.second->m_numLeadingJets = m_numLeadingJets;
    jetPlot.second->initialize();
    jetPlot.second->record( wk() );
  }

  return EL::StatusCode::SUCCESS;
}
EL::StatusCode Report :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Report :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode Report :: initialize ()
{
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: execute ()
{
  const xAOD::EventInfo*                eventInfo   (nullptr);
  const xAOD::JetContainer*             in_jets     (nullptr);
  const xAOD::JetContainer*             in_bjets    (nullptr);
  const xAOD::MissingETContainer*       in_missinget(nullptr);
  const xAOD::ElectronContainer*        in_electrons(nullptr);
  const xAOD::MuonContainer*            in_muons    (nullptr);
  const xAOD::TauJetContainer*          in_taus     (nullptr);
  const xAOD::PhotonContainer*          in_photons  (nullptr);
  //const xAOD::TruthParticleContainer*   in_truth    (nullptr);

  // start grabbing all the containers that we can
  RETURN_CHECK("Preselect::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  RETURN_CHECK("Preselect::execute()", HF::retrieve(in_jets,      m_inputJets,        m_event, m_store, m_debug), "Could not get the inputJets container.");
  RETURN_CHECK("Preselect::execute()", HF::retrieve(in_bjets,     m_inputBJets,       m_event, m_store, m_debug), "Could not get the inputBJets container.");
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

  static SG::AuxElement::Accessor< int > pass_preSel("pass_preSel");
  static SG::AuxElement::Accessor< float > decor_eventWeight("eventWeight");

  float eventWeight(1);
  if( decor_eventWeight.isAvailable(*eventInfo) ) eventWeight = decor_eventWeight(*eventInfo);

  m_jetPlots["all_jets"]->execute(in_jets, eventWeight);
  m_jetPlots["all_bjets"]->execute(in_bjets, eventWeight);

  if(m_passPreSel && pass_preSel.isAvailable(*eventInfo) && pass_preSel(*eventInfo) == 1){
    m_jetPlots["presel_jets"]->execute(in_jets, eventWeight);
    m_jetPlots["presel_bjets"]->execute(in_bjets, eventWeight);
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode Report :: finalize () {
  for( auto jetPlot : m_jetPlots ) {
    if(jetPlot.second) delete jetPlot.second;
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: histFinalize () { return EL::StatusCode::SUCCESS; }
