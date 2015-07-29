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
#include "xAODBase/IParticleContainer.h"
#include "AthLinks/ElementLink.h"

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"

#include "TheAccountant/VariableDefinitions.h"

// root includes
#include <TFile.h>

// c++ includes
#include <set>
#include <stdlib.h>

namespace HF = HelperFunctions;
namespace VD = VariableDefinitions;

#define ARRAY_INIT {-99, -99, -99, -99}
#define MULTI_ARRAY_INIT {ARRAY_INIT, ARRAY_INIT, ARRAY_INIT}

// this is needed to distribute the algorithm to the workers
ClassImp(OptimizationDump)
OptimizationDump :: OptimizationDump () :
  m_tree(new TTree("oTree", "optimization tree")),
  m_eventWeight(0.0),
  m_effectiveMass(-999.0),
  m_totalTransverseMomentum(-999.0),
  m_totalTransverseMass(-999.0),
  m_dPhiMETMin(-999.0),
  m_mTb(-999.0),
  m_met(-999.0),
  m_met_mpx(-999.0),
  m_met_mpy(-999.0),
  m_numJets(-99),
  m_numJetsLargeR(-99),
  m_n_topTag_SmoothLoose(0),
  m_n_topTag_SmoothTight(0),
  m_n_topTag_Loose(0),
  m_n_topTag_Tight(0),
  m_jetReclusteringTools{{nullptr, nullptr, nullptr}},
  m_rc_pt{MULTI_ARRAY_INIT},
  m_rc_m{MULTI_ARRAY_INIT},
  m_rc_split12{MULTI_ARRAY_INIT},
  m_rc_split23{MULTI_ARRAY_INIT},
  m_rc_nsj{MULTI_ARRAY_INIT}
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

  m_tree->Branch ("event_weight",              &m_eventWeight, "event_weight/F");
  if(!m_inputMET.empty()){
    m_tree->Branch ("m_transverse",              &m_totalTransverseMass, "m_transverse/F");
    m_tree->Branch ("met",                       &m_met, "met/F");
    m_tree->Branch ("met_px",                    &m_met_mpx, "met_mpx/F");
    m_tree->Branch ("met_py",                    &m_met_mpy, "met_mpy/F");
  }
  if(!m_inputMET.empty() && !m_inputJets.empty()){
    m_tree->Branch ("m_effective",               &m_effectiveMass, "m_effective/F");
    m_tree->Branch ("dPhiMETMin",                &m_dPhiMETMin, "dPhiMETMin/F");
    m_tree->Branch ("mTb",                       &m_mTb, "mTb/F");
  }
  if(!m_inputJets.empty()){
    m_tree->Branch ("pt_total",                  &m_totalTransverseMomentum, "pt_total/F");
    m_tree->Branch ("multiplicity_jet",          &m_numJets, "multiplicity_jet/I");
    m_tree->Branch ("multiplicity_jet_b",        &m_numBJets, "multiplicity_jet_b/I");

    // initialize branches for reclustered jets
    for(int i=0; i<4; i++){
      for(int r=0; r<3; r++){
        int radius = r*2 + 8;
        std::string commonDenominator = "jet_rc"+std::to_string(radius)+"_"+std::to_string(i);
        std::string branchName;

        branchName = "pt_"+commonDenominator;
        m_tree->Branch(branchName.c_str(), &(m_rc_pt[r][i]), (branchName+"/F").c_str());

        branchName = "m_"+commonDenominator;
        m_tree->Branch(branchName.c_str(), &(m_rc_m[r][i]), (branchName+"/F").c_str());

        branchName = "split12_"+commonDenominator;
        m_tree->Branch(branchName.c_str(), &(m_rc_split12[r][i]), (branchName+"/F").c_str());

        branchName = "split23_"+commonDenominator;
        m_tree->Branch(branchName.c_str(), &(m_rc_split23[r][i]), (branchName+"/F").c_str());

        branchName = "nsj_"+commonDenominator;
        m_tree->Branch(branchName.c_str(), &(m_rc_nsj[r][i]), (branchName+"/I").c_str());
      }
    }

    for(int i=0; i<3; i++){
      char outputContainer[8];
      float radius = 0.8 + (0.2*i); // 0.8, 1.0, 1.2
      sprintf(outputContainer, "RC%02.0fJets", radius*10);
      m_jetReclusteringTools[i] = new JetReclusteringTool(outputContainer+std::to_string(std::rand()));
      RETURN_CHECK("initialize()", m_jetReclusteringTools[i]->setProperty("InputJetContainer",  m_inputJets), "");
      RETURN_CHECK("initialize()", m_jetReclusteringTools[i]->setProperty("OutputJetContainer", outputContainer), "");
      RETURN_CHECK("initialize()", m_jetReclusteringTools[i]->setProperty("ReclusterRadius",    radius), "");
      RETURN_CHECK("initialize()", m_jetReclusteringTools[i]->initialize(), "");
    }
  }

  if(!m_inputLargeRJets.empty()){
    m_tree->Branch ("multiplicity_jet_largeR",   &m_numJetsLargeR, "multiplicity_jet_largeR/I");
    m_tree->Branch ("multiplicity_topTag_smoothloose", &m_n_topTag_SmoothLoose, "multiplicity_topTag_smoothloose/I");
    m_tree->Branch ("multiplicity_topTag_smoothtight", &m_n_topTag_SmoothTight, "multiplicity_topTag_smoothtight/I");
    m_tree->Branch ("multiplicity_topTag_loose", &m_n_topTag_Loose, "multiplicity_topTag_loose/I");
    m_tree->Branch ("multiplicity_topTag_tight", &m_n_topTag_Tight, "multiplicity_topTag_tight/I");
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: execute ()
{
  if(m_debug) Info("execute()", "Calling execute...");
  const xAOD::EventInfo*                eventInfo     (nullptr);
  const xAOD::JetContainer*             in_jetsLargeR (nullptr);
  const xAOD::JetContainer*             in_jets       (nullptr);
  const xAOD::MissingETContainer*       in_missinget  (nullptr);
  const xAOD::ElectronContainer*        in_electrons  (nullptr);
  const xAOD::MuonContainer*            in_muons      (nullptr);
  const xAOD::TauJetContainer*          in_taus       (nullptr);
  const xAOD::PhotonContainer*          in_photons    (nullptr);

  // start grabbing all the containers that we can
  RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  if(!m_inputJets.empty())
    RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(in_jets,     m_inputJets,       m_event, m_store, m_debug), "Could not get the inputJets container.");
  if(!m_inputLargeRJets.empty())
    RETURN_CHECK("Audit::execute()", HF::retrieve(in_jetsLargeR,      m_inputLargeRJets,        m_event, m_store, m_debug), "Could not get the inputLargeRJets container.");
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

  // compute variables for optimization
  m_eventWeight = VD::eventWeight(eventInfo, wk()->metaData());

  const xAOD::MissingET* in_met(nullptr);
  if(!m_inputMET.empty()){
    // retrieve CalibMET_RefFinal for METContainer
    xAOD::MissingETContainer::const_iterator met_id = in_missinget->find(m_inputMETName);
    if (met_id == in_missinget->end()) {
      Error("execute()", "No %s inside MET container", m_inputMETName.c_str());
      return EL::StatusCode::FAILURE;
    }
    // dereference the iterator since it's just a single object
    in_met = *met_id;
    m_totalTransverseMass = VD::mT(in_met, in_muons, in_electrons);
    m_met     = in_met->met();
    m_met_mpx = in_met->mpx();
    m_met_mpy = in_met->mpy();
  }

  if(!m_inputMET.empty() && !m_inputJets.empty()){
    m_effectiveMass = VD::Meff(in_met, in_jets, in_jets->size(), in_muons, in_electrons);
    m_dPhiMETMin = VD::dPhiMETMin(in_met, in_jets);

    static SG::AuxElement::Decorator< int > isB("isB");
    ConstDataVector<xAOD::JetContainer> bjets(SG::VIEW_ELEMENTS);
    for(const auto jet: *in_jets){
      if(isB(*jet) != 1) continue;
      bjets.push_back(jet);
    }
    m_mTb = VD::mTb(in_met, bjets.asDataVector());
  }

  static SG::AuxElement::ConstAccessor< int > pass_preSel("pass_preSel");

  if(!m_inputJets.empty()){
    m_totalTransverseMomentum = VD::HT(in_jets, in_muons, in_electrons);

    // counts of stuff
    static SG::AuxElement::ConstAccessor< int > pass_preSel_jets("pass_preSel_jets");
    static SG::AuxElement::ConstAccessor< int > pass_preSel_bjets("pass_preSel_bjets");
    m_numJets = (pass_preSel_jets.isAvailable(*eventInfo))?pass_preSel_jets(*eventInfo):-99;
    m_numBJets = (pass_preSel_bjets.isAvailable(*eventInfo))?pass_preSel_bjets(*eventInfo):-99;

    // build the reclustered, trimmed jets
    for(auto tool: m_jetReclusteringTools)
      tool->execute();

    for(int r=0; r<3; r++){
      char rcJetContainer[8];
      float radius = 0.8 + (0.2*r); // 0.8, 1.0, 1.2
      sprintf(rcJetContainer, "RC%02.0fJets", radius*10);
      const xAOD::JetContainer* rcJets(nullptr);
      RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(rcJets, rcJetContainer, m_event, m_store, m_debug), ("Could not retrieve the reclustered jet container "+std::string(rcJetContainer)).c_str());
      for(unsigned int i=0; i<4; i++){
        m_rc_pt[r][i] = -99.0;
        m_rc_m[r][i]  = -99.0;
        m_rc_split12[r][i] = -99.0;
        m_rc_split23[r][i] = -99.0;
        m_rc_nsj[r][i] = -99;
        // if there are less than 4 jets, then...
        if(i < rcJets->size()){
          auto rcJet = rcJets->at(i);
          m_rc_pt[r][i] = rcJet->pt();
          m_rc_m[r][i] = rcJet->m();
          // retrieve attributes from jet -- if it fails, it'll be set to -99
          //    this way, we don't error out when we do jobs
          std::vector< ElementLink< xAOD::IParticleContainer > > constitLinks;
          rcJet->getAttribute("Split12", m_rc_split12[r][i]);
          rcJet->getAttribute("Split23", m_rc_split23[r][i]);
          if(rcJet->getAttribute("constituentLinks", constitLinks)) m_rc_nsj[r][i] = constitLinks.size();
        }
      }
    }
  }

  if(!m_inputLargeRJets.empty()){
    static SG::AuxElement::ConstAccessor< int > pass_preSel_jetsLargeR("pass_preSel_jetsLargeR");
    m_numJetsLargeR = (pass_preSel_jetsLargeR.isAvailable(*eventInfo))?pass_preSel_jetsLargeR(*eventInfo):-99;

    // THERE MUST BE A BETTER WAY
    // tagging variables
    m_n_topTag_SmoothLoose = 0;
    m_n_topTag_SmoothTight = 0;
    m_n_topTag_Loose = 0;
    m_n_topTag_Tight = 0;
    for(auto jet: *in_jetsLargeR){
      int topTag_SmoothLoose(-1), topTag_SmoothTight(-1),
          topTag_Loose(-1), topTag_Tight(-1);
      jet->getAttribute("LooseSmoothTopTag", topTag_SmoothLoose);
      jet->getAttribute("TightSmoothTopTag", topTag_SmoothTight);
      jet->getAttribute("LooseTopTag", topTag_Loose);
      jet->getAttribute("TightTopTag", topTag_Tight);
      if(topTag_SmoothLoose == 1) m_n_topTag_SmoothLoose++;
      if(topTag_SmoothTight == 1) m_n_topTag_SmoothTight++;
      if(topTag_Loose == 1) m_n_topTag_Loose++;
      if(topTag_Tight == 1) m_n_topTag_Tight++;

      /*
      if(topTag_SmoothLoose == 1 || topTag_SmoothTight == 1 || topTag_Loose == 1 || topTag_Tight == 1){
        std::cout << topTag_SmoothLoose << topTag_SmoothTight << topTag_Loose << topTag_Tight << "|run#" <<eventInfo->auxdata<uint>("runNumber") << "|event#"  << eventInfo->auxdata<unsigned long long>("eventNumber") << "|lumi#" << eventInfo->auxdata<uint>("lumiBlock") << std::endl;
      }
      */
    }
  }

  // fill in all variables
  m_tree->Fill();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode OptimizationDump :: finalize () {
  if(!m_inputJets.empty())
    for(auto tool: m_jetReclusteringTools)
      if(tool) delete tool;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: histFinalize () { return EL::StatusCode::SUCCESS; }
