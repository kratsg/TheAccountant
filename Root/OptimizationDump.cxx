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

// root includes
#include <TFile.h>

// c++ includes
#include <set>
#include <stdlib.h>

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"
#include <TheAccountant/VariableDefinitions.h>
namespace HF = HelperFunctions;
namespace VD = VariableDefinitions;

#define ARRAY_INIT {-99, -99, -99, -99}
#define MULTI_ARRAY_INIT {ARRAY_INIT, ARRAY_INIT, ARRAY_INIT}

// this is needed to distribute the algorithm to the workers
ClassImp(OptimizationDump)
OptimizationDump :: OptimizationDump () :
  m_tree(new TTree("oTree", "optimization tree")),
  m_eventWeight(0.0),
  m_ttbarHF(0),
  m_ttbarHF_ext(0),
  m_eventNumber(-999.0),
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
  m_numJetsVarR_top(-99),
  m_numJetsVarR_W(-99),
  m_n_topTag_SmoothLoose(0),
  m_n_topTag_SmoothTight(0),
  m_n_topTag_Loose(0),
  m_n_topTag_Tight(0),
  m_jetReclusteringTools{{nullptr, nullptr, nullptr}},
  m_varRjetReclusteringTools{{nullptr, nullptr}},
  m_rc_pt{MULTI_ARRAY_INIT},
  m_rc_m{MULTI_ARRAY_INIT},
  m_rc_split12{MULTI_ARRAY_INIT},
  m_rc_split23{MULTI_ARRAY_INIT},
  m_rc_nsj{MULTI_ARRAY_INIT},
  m_varR_top_m{ARRAY_INIT},
  m_varR_top_pt{ARRAY_INIT},
  m_varR_top_nsj{ARRAY_INIT},
  m_varR_W_m{ARRAY_INIT},
  m_varR_W_pt{ARRAY_INIT},
  m_varR_W_nsj{ARRAY_INIT},
  m_largeR_pt{ARRAY_INIT},
  m_largeR_m{ARRAY_INIT},
  m_largeR_split12{ARRAY_INIT},
  m_largeR_split23{ARRAY_INIT},
  m_largeR_nsj{ARRAY_INIT},
  m_largeR_topTag_loose{ARRAY_INIT},
  m_largeR_topTag_tight{ARRAY_INIT},
  m_largeR_topTag_smoothLoose{ARRAY_INIT},
  m_largeR_topTag_smoothTight{ARRAY_INIT}
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
  m_tree->Branch ("ttbarHF",  		       &m_ttbarHF, "ttbarHF/I");
  m_tree->Branch ("ttbarHF_ext",               &m_ttbarHF_ext, "ttbarHF_ext/I");
  m_tree->Branch ("event_number",              &m_eventNumber, "event_number/I");
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
      RETURN_CHECK("initialize()", m_jetReclusteringTools[i]->setProperty("RCJetPtFrac",    m_rcTrimFrac), "");
      RETURN_CHECK("initialize()", m_jetReclusteringTools[i]->setProperty("InputJetPtMin",    20.0), "");
      RETURN_CHECK("initialize()", m_jetReclusteringTools[i]->initialize(), "");
    }

    m_tree->Branch ("multiplicity_jet_varR_top", &m_numJetsVarR_top, "multiplicity_jet_varR_top/I");
    m_tree->Branch ("multiplicity_jet_varR_W", &m_numJetsVarR_W, "multiplicity_jet_varR_W/I");
    for(int i=0; i<4; i++){
      std::string topcommonDenominator = "variableR_top_jet_"+std::to_string(i);
      std::string WcommonDenominator = "variableR_W_jet_"+std::to_string(i);
      std::string branchName;

      branchName = "m_"+topcommonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_varR_top_m[i]), (branchName+"/F").c_str());
      branchName = "m_"+WcommonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_varR_W_m[i]), (branchName+"/F").c_str());

      branchName = "pt_"+topcommonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_varR_top_pt[i]), (branchName+"/F").c_str());
      branchName = "pt_"+WcommonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_varR_W_pt[i]), (branchName+"/F").c_str());

      branchName = "nsj_"+topcommonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_varR_top_nsj[i]), (branchName+"/I").c_str());
      branchName = "nsj_"+WcommonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_varR_W_nsj[i]), (branchName+"/I").c_str());
    }

    for(int i=0; i<2; i++){
      char outputContainer[15];
      if(i==0) sprintf(outputContainer,"VarR_top_Jets");
      else sprintf(outputContainer,"VarR_W_Jets");
      m_varRjetReclusteringTools[i] = new JetReclusteringTool(outputContainer+std::to_string(std::rand()));
      RETURN_CHECK("initialize()", m_varRjetReclusteringTools[i]->setProperty("OutputJetContainer", outputContainer), "");
      if(i==0) RETURN_CHECK("initialize()", m_varRjetReclusteringTools[i]->setProperty("VariableRMassScale",    2*173.34), "");
      else RETURN_CHECK("initialize()", m_varRjetReclusteringTools[i]->setProperty("VariableRMassScale",    2*80.385), "");
      RETURN_CHECK("initialize()", m_varRjetReclusteringTools[i]->setProperty("InputJetContainer",  m_inputJets), "");
      RETURN_CHECK("initialize()", m_varRjetReclusteringTools[i]->setProperty("ReclusterRadius",    1.5), "");
      RETURN_CHECK("initialize()", m_varRjetReclusteringTools[i]->setProperty("VariableRMinRadius",    0.4), "");
      RETURN_CHECK("initialize()", m_varRjetReclusteringTools[i]->setProperty("RCJetPtFrac",    m_rcTrimFrac), "");
      RETURN_CHECK("initialize()", m_varRjetReclusteringTools[i]->setProperty("InputJetPtMin",    20.0), "");
      RETURN_CHECK("initialize()", m_varRjetReclusteringTools[i]->initialize(), "");
    }
  }

  if(!m_inputLargeRJets.empty()){
    m_tree->Branch ("multiplicity_jet_largeR",   &m_numJetsLargeR, "multiplicity_jet_largeR/I");
    m_tree->Branch ("multiplicity_topTag_smoothloose", &m_n_topTag_SmoothLoose, "multiplicity_topTag_smoothloose/I");
    m_tree->Branch ("multiplicity_topTag_smoothtight", &m_n_topTag_SmoothTight, "multiplicity_topTag_smoothtight/I");
    m_tree->Branch ("multiplicity_topTag_loose", &m_n_topTag_Loose, "multiplicity_topTag_loose/I");
    m_tree->Branch ("multiplicity_topTag_tight", &m_n_topTag_Tight, "multiplicity_topTag_tight/I");

    // initialize branches for reclustered jets
    for(int i=0; i<4; i++){
      std::string commonDenominator = "jet_largeR_"+std::to_string(i);
      std::string branchName;

      branchName = "pt_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_pt[i]), (branchName+"/F").c_str());

      branchName = "m_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_m[i]), (branchName+"/F").c_str());

      branchName = "split12_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_split12[i]), (branchName+"/F").c_str());

      branchName = "split23_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_split23[i]), (branchName+"/F").c_str());

      branchName = "nsj_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_nsj[i]), (branchName+"/I").c_str());

      branchName = "topTag_Loose_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_topTag_loose[i]), (branchName+"/I").c_str());

      branchName = "topTag_Tight_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_topTag_tight[i]), (branchName+"/I").c_str());

      branchName = "topTag_SmoothLoose_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_topTag_smoothLoose[i]), (branchName+"/I").c_str());

      branchName = "topTag_SmoothTight_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_topTag_smoothTight[i]), (branchName+"/I").c_str());
    }
  }

  m_tree->Branch("razor_ss_mass",           &m_ss_mass          , "razor_ss_mass/F");
  m_tree->Branch("razor_ss_invgamma",       &m_ss_invgamma      , "razor_ss_invgamma/F");
  m_tree->Branch("razor_ss_dphivis",        &m_ss_dphivis       , "razor_ss_dphivis/F");
  m_tree->Branch("razor_ss_costheta",       &m_ss_costheta      , "razor_ss_costheta/F");
  m_tree->Branch("razor_ss_dphidecayangle", &m_ss_dphidecayangle, "razor_ss_dphidecayangle/F");
  m_tree->Branch("razor_ss_mdeltaR",        &m_ss_mdeltaR       , "razor_ss_mdeltaR/F");
  m_tree->Branch("razor_s1_mass",           &m_s1_mass          , "razor_s1_mass/F");
  m_tree->Branch("razor_s1_costheta",       &m_s1_costheta      , "razor_s1_costheta/F");
  m_tree->Branch("razor_s2_mass",           &m_s2_mass          , "razor_s2_mass/F");
  m_tree->Branch("razor_s2_costheta",       &m_s2_costheta      , "razor_s2_costheta/F");
  m_tree->Branch("razor_i1_depth",          &m_i1_depth         , "razor_i1_depth/I");
  m_tree->Branch("razor_i2_depth",          &m_i2_depth         , "razor_i2_depth/I");
  m_tree->Branch("razor_v1_nelements",      &m_v1_nelements     , "razor_v1_nelements/I");
  m_tree->Branch("razor_v2_nelements",      &m_v2_nelements     , "razor_v2_nelements/I");
  m_tree->Branch("razor_ss_abs_costheta",   &m_ss_abs_costheta  , "razor_ss_abs_costheta/F");
  m_tree->Branch("razor_s1_abs_costheta",   &m_s1_abs_costheta  , "razor_s1_abs_costheta/F");
  m_tree->Branch("razor_s2_abs_costheta",   &m_s2_abs_costheta  , "razor_s2_abs_costheta/F");

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

  m_ttbarHF = VD::ttbarHF(eventInfo);
  m_ttbarHF_ext = VD::ttbarHF_ext(eventInfo);

  m_eventNumber = eventInfo->eventNumber();

  // do all of the razor variables
  static VD::accessor_t<float> SS_mass_acc("SS_mass");
  static VD::accessor_t<float> SS_invgamma_acc("SS_invgamma");
  static VD::accessor_t<float> SS_dphivis_acc("SS_dphivis");
  static VD::accessor_t<float> SS_costheta_acc("SS_costheta");
  static VD::accessor_t<float> SS_dphidecayangle_acc("SS_dphidecayangle");
  static VD::accessor_t<float> SS_mdeltaR_acc("SS_mdeltaR");
  static VD::accessor_t<float> S1_mass_acc("S1_mass");
  static VD::accessor_t<float> S2_mass_acc("S2_mass");
  static VD::accessor_t<float> S1_costheta_acc("S1_costheta");
  static VD::accessor_t<float> S2_costheta_acc("S2_costheta");
  static VD::accessor_t<float> I1_depth_acc("I1_depth");
  static VD::accessor_t<float> I2_depth_acc("I2_depth");
  static VD::accessor_t<float> V1_nelements_acc("V1_nelements");
  static VD::accessor_t<float> V2_nelements_acc("V2_nelements");

  m_ss_mass           = (SS_mass_acc.isAvailable(*eventInfo))?SS_mass_acc(*eventInfo)/1000.:-99;
  m_ss_invgamma       = (SS_invgamma_acc.isAvailable(*eventInfo))?SS_invgamma_acc(*eventInfo):-99;
  m_ss_dphivis        = (SS_dphivis_acc.isAvailable(*eventInfo))?SS_dphivis_acc(*eventInfo):-99;
  m_ss_costheta       = (SS_costheta_acc.isAvailable(*eventInfo))?SS_costheta_acc(*eventInfo):-99;
  m_ss_dphidecayangle = (SS_dphidecayangle_acc.isAvailable(*eventInfo))?SS_dphidecayangle_acc(*eventInfo):-99;
  m_ss_mdeltaR        = (SS_mdeltaR_acc.isAvailable(*eventInfo))?SS_mdeltaR_acc(*eventInfo)/1000.:-99;
  m_s1_mass           = (S1_mass_acc.isAvailable(*eventInfo))?S1_mass_acc(*eventInfo)/1000.:-99;
  m_s1_costheta       = (S1_costheta_acc.isAvailable(*eventInfo))?S1_costheta_acc(*eventInfo):-99;
  m_s2_mass           = (S2_mass_acc.isAvailable(*eventInfo))?S2_mass_acc(*eventInfo)/1000.:-99;
  m_s2_costheta       = (S2_costheta_acc.isAvailable(*eventInfo))?S2_costheta_acc(*eventInfo):-99;
  m_i1_depth          = (I1_depth_acc.isAvailable(*eventInfo))?I1_depth_acc(*eventInfo):-99;
  m_i2_depth          = (I2_depth_acc.isAvailable(*eventInfo))?I2_depth_acc(*eventInfo):-99;
  m_v1_nelements      = (V1_nelements_acc.isAvailable(*eventInfo))?V1_nelements_acc(*eventInfo):-99;
  m_v2_nelements      = (V2_nelements_acc.isAvailable(*eventInfo))?V2_nelements_acc(*eventInfo):-99;
  m_ss_abs_costheta   = (SS_costheta_acc.isAvailable(*eventInfo))?std::fabs(SS_costheta_acc(*eventInfo)):-99;
  m_s1_abs_costheta   = (S1_costheta_acc.isAvailable(*eventInfo))?std::fabs(S1_costheta_acc(*eventInfo)):-99;
  m_s2_abs_costheta   = (S2_costheta_acc.isAvailable(*eventInfo))?std::fabs(S2_costheta_acc(*eventInfo)):-99;

  // build signal electrons
  ConstDataVector<xAOD::ElectronContainer> signalElectrons;
  if(!m_inputElectrons.empty()){
    signalElectrons = VD::filterLeptons(in_electrons, true);
    in_electrons = signalElectrons.asDataVector();
  }

  // build signal muons
  ConstDataVector<xAOD::MuonContainer> signalMuons;
  if(!m_inputMuons.empty()){
    signalMuons = VD::filterLeptons(in_muons, true, true);
    in_muons = signalMuons.asDataVector();
  }

  const xAOD::MissingET* in_met(nullptr);
  if(!m_inputMET.empty()){
    in_met = (*in_missinget)[m_inputMETName.c_str()];
    if (!in_met) {
      Error("execute()", "No %s inside MET container", m_inputMETName.c_str());
      return EL::StatusCode::FAILURE;
    }

    m_totalTransverseMass = VD::mT(in_met, in_muons, in_electrons)/1.e3;
    m_met     = in_met->met()/1.e3;
    m_met_mpx = in_met->mpx()/1.e3;
    m_met_mpy = in_met->mpy()/1.e3;
  }

  // in_jets will always contain the signal jets
  ConstDataVector<xAOD::JetContainer> signalJets;
  if(!m_inputJets.empty()){
    signalJets = VD::subset_using_decor(in_jets, VD::decor_signal, 1);
    in_jets = signalJets.asDataVector();
  }

  if(!m_inputMET.empty() && !m_inputJets.empty()){
    
    ConstDataVector<xAOD::JetContainer> presel_jets = VD::subset_using_decor(in_jets, VD::decor_vd_pass_Presel, 1);
    //std::cout << in_jets->size() << " " << presel_jets.size() << std::endl;
    m_effectiveMass = VD::Meff_inclusive(in_met, presel_jets.asDataVector(), in_muons, in_electrons)/1000.;
    m_dPhiMETMin = VD::dPhiMETMin(in_met, in_jets);


    static VD::accessor_t< int > isB("isB");
    ConstDataVector<xAOD::JetContainer> bjets = VD::subset_using_decor(in_jets, isB, 1);
    auto sortedBJets = HF::sort_container_pt(bjets.asDataVector());
    m_mTb = VD::mTb(in_met, &sortedBJets)/1000.;
  }

  static VD::accessor_t< int > pass_preSel("pass_preSel");

  if(!m_inputJets.empty()){
    m_totalTransverseMomentum = VD::HT(in_jets, in_muons, in_electrons)/1000.;

    // number of jets and bjets that pass preselection
    static VD::accessor_t< int > pass_preSel_jets("pass_preSel_jets");
    static VD::accessor_t< int > pass_preSel_bjets("pass_preSel_bjets");
    m_numJets = (pass_preSel_jets.isAvailable(*eventInfo))?pass_preSel_jets(*eventInfo):-99;
    m_numBJets = (pass_preSel_bjets.isAvailable(*eventInfo))?pass_preSel_bjets(*eventInfo):-99;

    // build the reclustered, trimmed jets
    for(const auto &tool: m_jetReclusteringTools)
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
        // if there are fewer than 4 jets, then...
        if(i < rcJets->size()){
          auto rcJet = rcJets->at(i);
          m_rc_pt[r][i] = rcJet->pt()/1000.;
          m_rc_m[r][i] = rcJet->m()/1000.;
          // retrieve attributes from jet -- if it fails, it'll be set to -99
          //    this way, we don't error out when we do jobs
          std::vector< ElementLink< xAOD::IParticleContainer > > constitLinks;
          rcJet->getAttribute("Split12", m_rc_split12[r][i]);
          rcJet->getAttribute("Split23", m_rc_split23[r][i]);
          if(rcJet->getAttribute("constituentLinks", constitLinks)) m_rc_nsj[r][i] = constitLinks.size();
        }
      }
    }

    for(const auto& tool: m_varRjetReclusteringTools)
      tool->execute();

    for(int i=0; i<2; i++){
      char varRJetContainer[15];
      if(i==0) sprintf(varRJetContainer,"VarR_top_Jets");
      else sprintf(varRJetContainer,"VarR_W_Jets");
      const xAOD::JetContainer* varRJets(nullptr);
      RETURN_CHECK("OptimizationDump::execute()", HF::retrieve(varRJets, varRJetContainer, m_event, m_store, m_debug), ("Could not retrieve the variable R jet container "+std::string(varRJetContainer)).c_str());
      if(i==0) m_numJetsVarR_top = varRJets->size();
      else m_numJetsVarR_W = varRJets->size();
      for(unsigned int j=0; j<4; j++){
        if(i==0){
          m_varR_top_m[j]  = -99.0;
          m_varR_top_pt[j]  = -99.0;
          m_varR_top_nsj[j]  = -99.0;
        }
        else{
          m_varR_W_m[j]  = -99.0;
          m_varR_W_pt[j]  = -99.0;
          m_varR_W_nsj[j]  = -99.0;
        }
        // if there are fewer than 4 jets, then...
        if(j < varRJets->size()){
          auto varRJet = varRJets->at(j);
          if(i==0){
            m_varR_top_m[j] = varRJet->m()/1000.;
            m_varR_top_pt[j] = varRJet->pt()/1000.;
            std::vector< ElementLink< xAOD::IParticleContainer > > constitLinks;
            if(varRJet->getAttribute("constituentLinks", constitLinks)) m_varR_top_nsj[j] = constitLinks.size();
          }
          else{
            m_varR_W_m[j] = varRJet->m()/1000.;
            m_varR_W_pt[j] = varRJet->pt()/1000.;
            std::vector< ElementLink< xAOD::IParticleContainer > > constitLinks;
            if(varRJet->getAttribute("constituentLinks", constitLinks)) m_varR_W_nsj[j] = constitLinks.size();
          }
        }
      }
    }
  }

  if(!m_inputLargeRJets.empty()){
    static VD::accessor_t< int > pass_preSel_jetsLargeR("pass_preSel_jetsLargeR");
    m_numJetsLargeR = (pass_preSel_jetsLargeR.isAvailable(*eventInfo))?pass_preSel_jetsLargeR(*eventInfo):-99;

    // initialize for leading 4 largeR jets that pass preselection
    for(unsigned int i=0; i<4; i++){
      m_largeR_pt[i] = -99.0;
      m_largeR_m[i]  = -99.0;
      m_largeR_split12[i] = -99.0;
      m_largeR_split23[i] = -99.0;
      m_largeR_nsj[i] = -99;
    }

    // THERE MUST BE A BETTER WAY
    // tagging variables
    m_n_topTag_SmoothLoose = 0;
    m_n_topTag_SmoothTight = 0;
    m_n_topTag_Loose = 0;
    m_n_topTag_Tight = 0;

    int jetIndex = 0;
    for(const auto &jet: *in_jetsLargeR){
      int topTag_SmoothLoose(-1), topTag_SmoothTight(-1),
          topTag_Loose(-1), topTag_Tight(-1);
      // don't count it if it doesn't pass preselection
      if(pass_preSel(*jet) == 0) continue;

      jet->getAttribute("LooseSmoothTopTag", topTag_SmoothLoose);
      jet->getAttribute("TightSmoothTopTag", topTag_SmoothTight);
      jet->getAttribute("LooseTopTag", topTag_Loose);
      jet->getAttribute("TightTopTag", topTag_Tight);
      if(topTag_SmoothLoose == 1) m_n_topTag_SmoothLoose++;
      if(topTag_SmoothTight == 1) m_n_topTag_SmoothTight++;
      if(topTag_Loose == 1) m_n_topTag_Loose++;
      if(topTag_Tight == 1) m_n_topTag_Tight++;

      if(jetIndex < 4){
        m_largeR_pt[jetIndex] = jet->pt()/1000.;
        m_largeR_m[jetIndex] = jet->m()/1000.;
        // retrieve attributes from jet -- if it fails, it'll be set to -99
        //    this way, we don't error out when we do jobs
        jet->getAttribute("Split12", m_largeR_split12[jetIndex]);
        jet->getAttribute("Split23", m_largeR_split23[jetIndex]);
        std::vector< ElementLink< xAOD::IParticleContainer > > constitLinks;
        if(jet->getAttribute("constituentLinks", constitLinks)) m_largeR_nsj[jetIndex] = constitLinks.size();
        // top tagging
        m_largeR_topTag_loose[jetIndex] = topTag_Loose;
        m_largeR_topTag_tight[jetIndex] = topTag_Tight;
        m_largeR_topTag_smoothLoose[jetIndex] = topTag_SmoothLoose;
        m_largeR_topTag_smoothTight[jetIndex] = topTag_SmoothTight;
      }
      jetIndex++;
    }

      /*
      if(topTag_SmoothLoose == 1 || topTag_SmoothTight == 1 || topTag_Loose == 1 || topTag_Tight == 1){
        std::cout << topTag_SmoothLoose << topTag_SmoothTight << topTag_Loose << topTag_Tight << "|run#" <<eventInfo->auxdata<uint>("runNumber") << "|event#"  << eventInfo->auxdata<unsigned long long>("eventNumber") << "|lumi#" << eventInfo->auxdata<uint>("lumiBlock") << std::endl;
      }
      */
  }

  // fill in all variables
  m_tree->Fill();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode OptimizationDump :: finalize () {
  if(!m_inputJets.empty()){
    for(const auto &tool: m_jetReclusteringTools)
      if(tool) delete tool;
    for(const auto& tool: m_varRjetReclusteringTools)
      if(tool) delete tool;
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: histFinalize () { return EL::StatusCode::SUCCESS; }
