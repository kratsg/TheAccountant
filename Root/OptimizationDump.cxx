#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "EventLoop/OutputStream.h"
#include <TheAccountant/OptimizationDump.h>

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

// Infrastructure includes
#include "xAODRootAccess/Init.h"

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"

// root includes
#include <TFile.h>

// c++ includes
#include <set>

namespace HF = HelperFunctions;

// this is needed to distribute the algorithm to the workers
ClassImp(OptimizationDump)
OptimizationDump :: OptimizationDump () :
  m_tree(new TTree("oTree", "optimization tree")),
  m_eventWeight(0.0),
  m_effectiveMass(-999.0),
  m_totalTransverseMomentum(-999.0),
  m_totalTransverseMass(-999.0),
  m_numBJets(-99),
  m_numJets(-99)
{}

EL::StatusCode OptimizationDump :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("OptimizationDump").ignore();

  /* https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/EventLoop#Creating_output_n_tuples */
  // output file for optimization tree
  EL::OutputStream out ("optimizationTree");
  job.outputAdd (out);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: histInitialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode OptimizationDump :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode OptimizationDump :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode OptimizationDump :: initialize () {
  // assign m_event and m_store
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  TFile *file = wk()->getOutputFile ("optimizationTree");
  m_tree->SetDirectory (file);

  m_tree->Branch( "w",    &m_eventWeight, "w/F");
  m_tree->Branch ("meff", &m_effectiveMass, "meff/F");
  m_tree->Branch ("ht",   &m_totalTransverseMomentum, "ht/F");
  m_tree->Branch ("mt",   &m_totalTransverseMass, "mt/F");
  m_tree->Branch ("n_b",  &m_numBJets, "n_b/I");
  m_tree->Branch ("n_j",  &m_numJets, "n_j/I");

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: execute ()
{
  const xAOD::EventInfo*                eventInfo   (nullptr);
  const xAOD::JetContainer*             in_jets     (nullptr);
  const xAOD::JetContainer*             in_bjets    (nullptr);
  const xAOD::MissingETContainer*       in_missinget(nullptr);
  const xAOD::ElectronContainer*        in_electrons(nullptr);
  const xAOD::MuonContainer*            in_muons    (nullptr);
  const xAOD::TauJetContainer*          in_taus     (nullptr);
  const xAOD::PhotonContainer*          in_photons  (nullptr);

  // start grabbing all the containers that we can
  RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");

  static SG::AuxElement::Accessor< int > pass_preSel("pass_preSel");
  static SG::AuxElement::Accessor< float > decor_eventWeight("eventWeight");
  if(pass_preSel.isAvailable(*eventInfo) && pass_preSel(*eventInfo) == 0) return EL::StatusCode::SUCCESS;

  RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(in_jets,      m_inputJets,        m_event, m_store, m_debug), "Could not get the inputJets container.");
  RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(in_bjets,     m_inputBJets,       m_event, m_store, m_debug), "Could not get the inputBJets container.");

  if(!m_inputMET.empty())
    RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(in_missinget, m_inputMET,         m_event, m_store, m_debug), "Could not get the inputMET container.");
  if(!m_inputElectrons.empty())
    RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(in_electrons, m_inputElectrons,   m_event, m_store, m_debug), "Could not get the inputElectrons container.");
  if(!m_inputMuons.empty())
    RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(in_muons,     m_inputMuons,       m_event, m_store, m_debug), "Could not get the inputMuons container.");
  if(!m_inputTauJets.empty())
    RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(in_taus,      m_inputTauJets,     m_event, m_store, m_debug), "Could not get the inputTauJets container.");
  if(!m_inputPhotons.empty())
    RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(in_photons,   m_inputPhotons,     m_event, m_store, m_debug), "Could not get the inputPhotons container.");

  // retrieve CalibMET_RefFinal for METContainer
  xAOD::MissingETContainer::const_iterator met_final = in_missinget->find("Final");
  if (met_final == in_missinget->end()) {
    Error("execute()", "No RefFinal inside MET container" );
    return EL::StatusCode::FAILURE;
  }
  // dereference the iterator since it's just a single object
  const xAOD::MissingET* in_met = *met_final;

  // compute variables for optimization
  m_eventWeight = 0.0;
  if(decor_eventWeight.isAvailable(*eventInfo)) m_eventWeight = decor_eventWeight(*eventInfo);
  m_numBJets = in_bjets->size();
  m_numJets = in_jets->size();
  m_totalTransverseMomentum = 0;
  m_totalTransverseMass = 0;
  for(auto jet: *in_jets){
    m_totalTransverseMomentum += jet->pt();
    m_totalTransverseMass += jet->m();
  }
  // effective mass: sum of jet pt + met
  m_effectiveMass = m_totalTransverseMomentum + in_met->met();
  // total transverse mass: sum of jet pt + sum of jet m
  m_totalTransverseMass += m_totalTransverseMomentum;

  // fill in all variables
  m_tree->Fill();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode OptimizationDump :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode OptimizationDump :: histFinalize () { return EL::StatusCode::SUCCESS; }
