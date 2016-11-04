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
#define ARRAY_INIT0 {0, 0, 0, 0}
#define MULTI_ARRAY_INIT {ARRAY_INIT, ARRAY_INIT, ARRAY_INIT}
#define MULTI_ARRAY_INIT0 {ARRAY_INIT0, ARRAY_INIT0, ARRAY_INIT0}

// this is needed to distribute the algorithm to the workers
ClassImp(OptimizationDump)
OptimizationDump :: OptimizationDump () :
  m_tree(new TTree("oTree", "optimization tree")),
  m_eventWeight(0.0),
  m_SF_pu(0.0),
  m_SF_btag(0.0),
  m_ttbarHF(0),
  m_ttbarHF_ext(0),
  m_eventNumber(-999.0),
  m_effectiveMass(-999.0),
  m_totalTransverseMomentum(-999.0),
  m_totalTransverseMass(-999.0),
  m_mjsum(-999.0),
  m_dPhiMETMin(-999.0),
  m_mTb(-999.0),
  m_met(-999.0),
  m_met_mpx(-999.0),
  m_met_mpy(-999.0),
  m_met_phi(-999.0),
  m_numJets(0),
  m_numBJets(0),
  m_numJetsLargeR(0),
  m_numJetsVarR_top(0),
  m_numJetsVarR_W(0),
  m_n_topTag_VeryLoose(0),
  m_n_topTag_SmoothLoose(0),
  m_n_topTag_SmoothTight(0),
  m_n_topTag_Loose(0),
  m_n_topTag_Tight(0),
  m_largeR_pt{ARRAY_INIT},
  m_largeR_m{ARRAY_INIT},
  m_largeR_split12{ARRAY_INIT},
  m_largeR_split23{ARRAY_INIT},
  m_largeR_nsj{ARRAY_INIT0},
  m_largeR_topTag_veryloose{ARRAY_INIT0},
  m_largeR_topTag_loose{ARRAY_INIT0},
  m_largeR_topTag_tight{ARRAY_INIT0},
  m_largeR_topTag_smoothLoose{ARRAY_INIT0},
  m_largeR_topTag_smoothTight{ARRAY_INIT0},
  m_inclVar()
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

  // if doing rc jets, use rc jets container instead
  if(m_rc_enable) m_inputLargeRJets = m_RCJetsContainerName;

  TFile *file = wk()->getOutputFile ("optimizationTree");
  m_tree->SetDirectory (file);

  m_tree->Branch ("event_weight",              &m_eventWeight, "event_weight/F");
  m_tree->Branch ("sf_pu",                     &m_SF_pu, "sf_pu/F");
  m_tree->Branch ("sf_btag",                   &m_SF_btag, "sf_btag/F");

  m_tree->Branch ("ttbarHF",  		           &m_ttbarHF, "ttbarHF/I");
  m_tree->Branch ("ttbarHF_ext",               &m_ttbarHF_ext, "ttbarHF_ext/I");
  m_tree->Branch ("event_number",              &m_eventNumber, "event_number/I");
  if(!m_inputMET.empty()){
    m_tree->Branch ("m_transverse",              &m_totalTransverseMass, "m_transverse/F");
    m_tree->Branch ("met",                       &m_met, "met/F");
    m_tree->Branch ("met_px",                    &m_met_mpx, "met_mpx/F");
    m_tree->Branch ("met_py",                    &m_met_mpy, "met_mpy/F");
    m_tree->Branch ("met_phi",                    &m_met_phi, "met_phi/F");
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
  }

  if(!m_inputLargeRJets.empty()){
    m_tree->Branch ("mjsum", &m_mjsum, "mjsum/F");
    m_tree->Branch ("multiplicity_jet_largeR",   &m_numJetsLargeR, "multiplicity_jet_largeR/I");
    m_tree->Branch ("multiplicity_topTag_veryloose", &m_n_topTag_VeryLoose, "multiplicity_topTag_veryloose/I");
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

      branchName = "topTag_VeryLoose_"+commonDenominator;
      m_tree->Branch(branchName.c_str(), &(m_largeR_topTag_veryloose[i]), (branchName+"/I").c_str());

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

  if(!m_inputJets.empty() && !m_inputMET.empty()){
    // empty initialization to get the branch working
    m_inclVar["GG_mass"] = -99.0;
    m_inclVar["Ga_mass"] = -99.0;
    m_inclVar["Gb_mass"] = -99.0;
    m_inclVar["Ca1_mass"] = -99.0;
    m_inclVar["Cb1_mass"] = -99.0;
    m_inclVar["Va1_mass"] = -99.0;
    m_inclVar["Vb1_mass"] = -99.0;
    m_inclVar["Va2_mass"] = -99.0;
    m_inclVar["Vb2_mass"] = -99.0;
    /*
    m_inclVar["pT_GG_Ga"] = -99.0;
    m_inclVar["pT_Va1_GG"] = -99.0;
    m_inclVar["pT_Va2_GG"] = -99.0;
    m_inclVar["pT_GG_Gb"] = -99.0;
    m_inclVar["pT_Vb1_GG"] = -99.0;
    m_inclVar["pT_Vb2_GG"] = -99.0;
    m_inclVar["pT_Ia1_GG"] = -99.0;
    m_inclVar["pT_Ib1_GG"] = -99.0;
    m_inclVar["p_Ga_GG"] = -99.0;
    m_inclVar["p_Va1_GG"] = -99.0;
    m_inclVar["p_Va2_GG"] = -99.0;
    m_inclVar["p_Gb_GG"] = -99.0;
    m_inclVar["p_Vb1_GG"] = -99.0;
    m_inclVar["p_Vb2_GG"] = -99.0;
    m_inclVar["p_Ia1_GG"] = -99.0;
    m_inclVar["p_Ib1_GG"] = -99.0;
    m_inclVar["GG_invGamma"] = -99.0;
    */
    m_inclVar["GG_invGamma"] = -99.0;
    m_inclVar["GG_visShape"] = -99.0;
    m_inclVar["GG_mDeltaR"] = -99.0;
    m_inclVar["Va1_n"] = -99.0;
    m_inclVar["Vb1_n"] = -99.0;
    m_inclVar["Ga_n"] = -99.0;
    m_inclVar["Va2_n"] = -99.0;
    m_inclVar["Vb2_n"] = -99.0;
    m_inclVar["Gb_n"] = -99.0;
    /*
    m_inclVar["Ia1_depth"] = -99.0;
    m_inclVar["Ib1_depth"] = -99.0;
    */
    m_inclVar["GG_cosTheta"] = -99.0;
    m_inclVar["Ga_cosIa1"] = -99.0;
    m_inclVar["Gb_cosIb1"] = -99.0;
    m_inclVar["Va1_cosTheta"] = -99.0;
    m_inclVar["Vb1_cosTheta"] = -99.0;
    m_inclVar["Va2_cosTheta"] = -99.0;
    m_inclVar["Vb2_cosTheta"] = -99.0;
    m_inclVar["GG_dPhiVis"] = -99.0;
    m_inclVar["GG_dPhiBetaR"] = -99.0;
    m_inclVar["GG_dPhiDecay"] = -99.0;
    m_inclVar["dPhi_Ga_Va1"] = -99.0;
    m_inclVar["dPhi_Ga_Ca1"] = -99.0;
    m_inclVar["dPhi_Gb_Vb1"] = -99.0;
    m_inclVar["dPhi_Gb_Cb1"] = -99.0;
    m_inclVar["dPhi_Ca1_Va2"] = -99.0;
    m_inclVar["dPhi_Cb1_Vb2"] = -99.0;
    m_inclVar["pT_GG"] = -99.0;
    m_inclVar["pZ_GG"] = -99.0;
    m_inclVar["H11GG"] = -99.0;
    m_inclVar["H21GG"] = -99.0;
    m_inclVar["H22GG"] = -99.0;
    m_inclVar["H41GG"] = -99.0;
    m_inclVar["H42GG"] = -99.0;
    m_inclVar["H11Ga"] = -99.0;
    m_inclVar["H11Gb"] = -99.0;
    m_inclVar["H21Ga"] = -99.0;
    m_inclVar["H21Gb"] = -99.0;
    m_inclVar["H11Ca1"] = -99.0;
    m_inclVar["H11Cb1"] = -99.0;
    m_inclVar["HT21GG"] = -99.0;
    m_inclVar["HT22GG"] = -99.0;
    m_inclVar["HT41GG"] = -99.0;
    m_inclVar["HT42GG"] = -99.0;
    m_inclVar["d_dPhiG"] = -99.0;
    m_inclVar["s_dPhiG"] = -99.0;
    m_inclVar["s_angle"] = -99.0;
    m_inclVar["d_angle"] = -99.0;
    m_inclVar["ratio_pZGG_HT21GG"] = -99.0;
    m_inclVar["ratio_pZGG_HT41GG"] = -99.0;
    m_inclVar["ratio_pTGG_HT21GG"] = -99.0;
    m_inclVar["ratio_pTGG_HT41GG"] = -99.0;
    m_inclVar["ratio_H11GG_H21GG"] = -99.0;
    m_inclVar["ratio_HT41GG_H41GG"] = -99.0;
    m_inclVar["ratio_H11GG_H41GG"] = -99.0;
    m_inclVar["maxRatio_H10PP_H11PP"] = -99.0;

    //QCD Vars
    m_inclVar["Rsib"] = -99.0;
    m_inclVar["cosQCD"] = -99.0;
    m_inclVar["deltaQCD"] = -99.0;
    m_inclVar["temp_Rsib"] = -99.0;
    m_inclVar["Pmet_pT"] = -99.0;
    m_inclVar["Psib_phi"] = -99.0;
    m_inclVar["Pmet_phi"] = -99.0;

    m_tree->Branch("razor_GG_mass",     &(m_inclVar.at("GG_mass")));
    m_tree->Branch("razor_Ga_mass",     &(m_inclVar.at("Ga_mass")));
    m_tree->Branch("razor_Gb_mass",     &(m_inclVar.at("Gb_mass")));
    m_tree->Branch("razor_Ca1_mass",    &(m_inclVar.at("Ca1_mass")));
    m_tree->Branch("razor_Cb1_mass",    &(m_inclVar.at("Cb1_mass")));
    m_tree->Branch("razor_Va1_mass",    &(m_inclVar.at("Va1_mass")));
    m_tree->Branch("razor_Vb1_mass",    &(m_inclVar.at("Vb1_mass")));
    m_tree->Branch("razor_Va2_mass",    &(m_inclVar.at("Va2_mass")));
    m_tree->Branch("razor_Vb2_mass",    &(m_inclVar.at("Vb2_mass")));

    /*
    m_tree->Branch("razor_pT_GG_Ga",    &(m_inclVar.at("pT_GG_Ga")));
    m_tree->Branch("razor_pT_Va1_GG",   &(m_inclVar.at("pT_Va1_GG")));
    m_tree->Branch("razor_pT_Va2_GG",   &(m_inclVar.at("pT_Va2_GG")));
    m_tree->Branch("razor_pT_GG_Gb",    &(m_inclVar.at("pT_GG_Gb")));
    m_tree->Branch("razor_pT_Vb1_GG",   &(m_inclVar.at("pT_Vb1_GG")));
    m_tree->Branch("razor_pT_Vb2_GG",   &(m_inclVar.at("pT_Vb2_GG")));
    m_tree->Branch("razor_pT_Ia1_GG",   &(m_inclVar.at("pT_Ia1_GG")));
    m_tree->Branch("razor_pT_Ib1_GG",   &(m_inclVar.at("pT_Ib1_GG")));
    */

    /*
    m_tree->Branch("razor_p_Ga_GG",     &(m_inclVar.at("p_Ga_GG")));
    m_tree->Branch("razor_p_Va1_GG",    &(m_inclVar.at("p_Va1_GG")));
    m_tree->Branch("razor_p_Va2_GG",    &(m_inclVar.at("p_Va2_GG")));
    m_tree->Branch("razor_p_Gb_GG",     &(m_inclVar.at("p_Gb_GG")));
    m_tree->Branch("razor_p_Vb1_GG",    &(m_inclVar.at("p_Vb1_GG")));
    m_tree->Branch("razor_p_Vb2_GG",    &(m_inclVar.at("p_Vb2_GG")));
    m_tree->Branch("razor_p_Ia1_GG",    &(m_inclVar.at("p_Ia1_GG")));
    m_tree->Branch("razor_p_Ib1_GG",    &(m_inclVar.at("p_Ib1_GG")));
    */

    /*
    m_tree->Branch("razor_GG_invGamma", &(m_inclVar.at("GG_invGamma")));
    */
    m_tree->Branch("razor_GG_invGamma", &(m_inclVar.at("GG_invGamma")));
    m_tree->Branch("razor_GG_visShape", &(m_inclVar.at("GG_visShape")));
    m_tree->Branch("razor_GG_mDeltaR",  &(m_inclVar.at("GG_mDeltaR")));

    // counting
    m_tree->Branch("razor_Va1_n",       &(m_inclVar.at("Va1_n")));
    m_tree->Branch("razor_Vb1_n",       &(m_inclVar.at("Vb1_n")));
    m_tree->Branch("razor_Ga_n",        &(m_inclVar.at("Ga_n")));
    m_tree->Branch("razor_Va2_n",       &(m_inclVar.at("Va2_n")));
    m_tree->Branch("razor_Vb2_n",       &(m_inclVar.at("Vb2_n")));
    m_tree->Branch("razor_Gb_n",        &(m_inclVar.at("Gb_n")));
    /*
    m_tree->Branch("razor_Ia1_depth",   &(m_inclVar.at("Ia1_depth")));
    m_tree->Branch("razor_Ib1_depth",   &(m_inclVar.at("Ib1_depth")));
    */

    m_tree->Branch("razor_GG_cosTheta", &(m_inclVar.at("GG_cosTheta")));
    m_tree->Branch("razor_Ga_cosIa1",   &(m_inclVar.at("Ga_cosIa1")));
    m_tree->Branch("razor_Gb_cosIb1",   &(m_inclVar.at("Gb_cosIb1")));
    m_tree->Branch("razor_Va1_cosTheta",&(m_inclVar.at("Va1_cosTheta")));
    m_tree->Branch("razor_Vb1_cosTheta",&(m_inclVar.at("Vb1_cosTheta")));
    m_tree->Branch("razor_Va2_cosTheta",&(m_inclVar.at("Va2_cosTheta")));
    m_tree->Branch("razor_Vb2_cosTheta",&(m_inclVar.at("Vb2_cosTheta")));

    // delta phi
    m_tree->Branch("razor_GG_dPhiVis",  &(m_inclVar.at("GG_dPhiVis")));
    m_tree->Branch("razor_GG_dPhiBetaR",&(m_inclVar.at("GG_dPhiBetaR")));
    m_tree->Branch("razor_GG_dPhiDecay",&(m_inclVar.at("GG_dPhiDecay")));
    m_tree->Branch("razor_dPhi_Ga_Va1", &(m_inclVar.at("dPhi_Ga_Va1")));
    m_tree->Branch("razor_dPhi_Ga_Ca1", &(m_inclVar.at("dPhi_Ga_Ca1")));
    m_tree->Branch("razor_dPhi_Gb_Vb1", &(m_inclVar.at("dPhi_Gb_Vb1")));
    m_tree->Branch("razor_dPhi_Gb_Cb1", &(m_inclVar.at("dPhi_Gb_Cb1")));
    m_tree->Branch("razor_dPhi_Ca1_Va2",&(m_inclVar.at("dPhi_Ca1_Va2")));
    m_tree->Branch("razor_dPhi_Cb1_Vb2",&(m_inclVar.at("dPhi_Cb1_Vb2")));

    // CM variables
    m_tree->Branch("razor_pT_GG",       &(m_inclVar.at("pT_GG")));
    m_tree->Branch("razor_pZ_GG",       &(m_inclVar.at("pZ_GG")));

    // H-variables (H_{n,m}^{F
    m_tree->Branch("razor_H11GG",    &(m_inclVar.at("H11GG")));
    m_tree->Branch("razor_H21GG",    &(m_inclVar.at("H21GG")));
    m_tree->Branch("razor_H22GG",    &(m_inclVar.at("H22GG")));
    m_tree->Branch("razor_H41GG",    &(m_inclVar.at("H41GG")));
    m_tree->Branch("razor_H42GG",    &(m_inclVar.at("H42GG")));

    m_tree->Branch("razor_H11Ga",    &(m_inclVar.at("H11Ga")));
    m_tree->Branch("razor_H11Gb",    &(m_inclVar.at("H11Gb")));
    m_tree->Branch("razor_H21Ga",    &(m_inclVar.at("H21Ga")));
    m_tree->Branch("razor_H21Gb",    &(m_inclVar.at("H21Gb")));

    m_tree->Branch("razor_H11Ca1",   &(m_inclVar.at("H11Ca1")));
    m_tree->Branch("razor_H11Cb1",   &(m_inclVar.at("H11Cb1")));

    m_tree->Branch("razor_HT21GG",   &(m_inclVar.at("HT21GG")));
    m_tree->Branch("razor_HT22GG",   &(m_inclVar.at("HT22GG")));
    m_tree->Branch("razor_HT41GG",   &(m_inclVar.at("HT41GG")));
    m_tree->Branch("razor_HT42GG",   &(m_inclVar.at("HT42GG")));

    // gluino hemishpere varia
    m_tree->Branch("razor_d_dPhiG",     &(m_inclVar.at("d_dPhiG")));
    m_tree->Branch("razor_s_dPhiG",     &(m_inclVar.at("s_dPhiG")));

    // sangle and dangle
    m_tree->Branch("razor_s_angle",     &(m_inclVar.at("s_angle")));
    m_tree->Branch("razor_d_angle",     &(m_inclVar.at("d_angle")));

    // ratios
    m_tree->Branch("razor_ratio_pZGG_HT21GG",       &(m_inclVar.at("ratio_pZGG_HT21GG")));
    m_tree->Branch("razor_ratio_pZGG_HT41GG",       &(m_inclVar.at("ratio_pZGG_HT41GG")));
    m_tree->Branch("razor_ratio_pTGG_HT21GG",       &(m_inclVar.at("ratio_pTGG_HT21GG")));
    m_tree->Branch("razor_ratio_pTGG_HT41GG",       &(m_inclVar.at("ratio_pTGG_HT41GG")));

    m_tree->Branch("razor_ratio_H11GG_H21GG",     &(m_inclVar.at("ratio_H11GG_H21GG")));
    m_tree->Branch("razor_ratio_HT41GG_H41GG",    &(m_inclVar.at("ratio_HT41GG_H41GG")));
    m_tree->Branch("razor_ratio_H11GG_H41GG",     &(m_inclVar.at("ratio_H11GG_H41GG")));
    m_tree->Branch("razor_maxRatio_H10PP_H11PP",  &(m_inclVar.at("maxRatio_H10PP_H11PP")));

    m_tree->Branch("razor_Rsib",     &(m_inclVar.at("Rsib")));
    m_tree->Branch("razor_cosQCD",   &(m_inclVar.at("cosQCD")));
    m_tree->Branch("razor_deltaQCD", &(m_inclVar.at("deltaQCD")));
    std::cout << "here" << std::endl;
    m_tree->Branch("razor_temp_Rsib", &(m_inclVar.at("temp_Rsib")));
    std::cout << "and here" << std::endl;
    m_tree->Branch("razor_Pmet_pT", &(m_inclVar.at("Pmet_pT")));
    m_tree->Branch("razor_Psib_phi", &(m_inclVar.at("Psib_phi")));
    m_tree->Branch("razor_Pmet_phi", &(m_inclVar.at("Pmet_phi")));
    std::cout << "and here too" << std::endl;
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
  // do all of the razor variables
  std::map<std::string, double>* in_inclVar(nullptr);
  RETURN_CHECK("Report::execute()", HF::retrieve(in_inclVar, "RJigsawInclusiveVariables", nullptr, m_store, m_debug), "Could not get the RJRVars");
  // fill in the original map with the values
  for(const auto& item: *in_inclVar) m_inclVar[item.first] = item.second;

  // compute variables for optimization
  m_eventWeight = VD::eventWeight(eventInfo, wk()->metaData());
  // scale factors
  static VD::accessor_t<float> sf_pu("weight_pu");
  static VD::accessor_t<float> sf_btag("weight_btag");
  m_SF_pu = sf_pu.isAvailable(*eventInfo)?sf_pu(*eventInfo):1.0;
  m_SF_btag = sf_btag.isAvailable(*eventInfo)?sf_btag(*eventInfo):1.0;

  // ttbar classification
  m_ttbarHF = VD::ttbarHF(eventInfo);
  m_ttbarHF_ext = VD::ttbarHF_ext(eventInfo);

  // event number
  m_eventNumber = eventInfo->eventNumber();

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
    m_met_phi = in_met->phi();
  }

  // in_jets will always contain the presel jets
  ConstDataVector<xAOD::JetContainer> presel_jets;
  ConstDataVector<xAOD::JetContainer> presel_bjets;
  if(!m_inputJets.empty()){
    presel_jets = VD::subset_using_decor(in_jets, VD::acc_pass_preSel, 1);
    presel_bjets = VD::subset_using_decor(in_jets, VD::acc_pass_preSel_b, 1);
  }

  if(!m_inputMET.empty() && !m_inputJets.empty()){
    m_effectiveMass = VD::Meff_inclusive(in_met, presel_jets.asDataVector(), in_muons, in_electrons)/1000.;
    m_dPhiMETMin = VD::dPhiMETMin(in_met, presel_jets.asDataVector());

    m_mTb = VD::mTb(in_met, presel_bjets.asDataVector())/1000.;
  }

  if(!m_inputJets.empty()){
    m_totalTransverseMomentum = VD::HT(presel_jets.asDataVector(), in_muons, in_electrons)/1000.;

    // number of jets and bjets that pass preselection
    m_numJets = presel_jets.size();
    m_numBJets = presel_bjets.size();
  }

  ConstDataVector<xAOD::JetContainer> presel_jetsLargeR;
  ConstDataVector<xAOD::JetContainer> presel_topTags;
  if(!m_inputLargeRJets.empty()){
    presel_jetsLargeR = VD::subset_using_decor(in_jetsLargeR, VD::acc_pass_preSel, 1);
    presel_topTags = VD::subset_using_decor(in_jetsLargeR, VD::acc_pass_preSel_top, 1);
    m_numJetsLargeR = presel_jetsLargeR.size();

    m_mjsum = VD::MJSum(presel_jetsLargeR.asDataVector());

    // initialize for leading 4 largeR jets that pass preselection
    for(unsigned int i=0; i<4; i++){
      m_largeR_pt[i] = -99.0;
      m_largeR_m[i]  = -99.0;
      m_largeR_split12[i] = -99.0;
      m_largeR_split23[i] = -99.0;
      m_largeR_nsj[i] = 0;
    }

    // THERE MUST BE A BETTER WAY
    // tagging variables
    m_n_topTag_VeryLoose = 0;
    m_n_topTag_SmoothLoose = 0;
    m_n_topTag_SmoothTight = 0;
    m_n_topTag_Loose = 0;
    m_n_topTag_Tight = 0;

    int jetIndex = 0;
    for(const auto &jet: presel_jetsLargeR){
      m_n_topTag_VeryLoose += VD::topTag(jet, "VeryLoose", 2.0, 300);
      m_n_topTag_Loose += VD::topTag(jet, "Loose", 2.0, 300);
      m_n_topTag_SmoothLoose += VD::topTag(jet, "SmoothLoose", 2.0, 300);
      m_n_topTag_Tight += VD::topTag(jet, "Tight", 2.0, 300);
      m_n_topTag_SmoothTight += VD::topTag(jet, "SmoothTight", 2.0, 300);

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
        m_largeR_topTag_veryloose[jetIndex] = VD::topTag(jet, "VeryLoose", 2.0, 300);
        m_largeR_topTag_loose[jetIndex] = VD::topTag(jet, "Loose", 2.0, 300);
        m_largeR_topTag_tight[jetIndex] = VD::topTag(jet, "Tight", 2.0, 300);
        m_largeR_topTag_smoothLoose[jetIndex] = VD::topTag(jet, "SmoothLoose", 2.0, 300);
        m_largeR_topTag_smoothTight[jetIndex] = VD::topTag(jet, "SmoothTight", 2.0, 300);
      }
      jetIndex++;
    }
  }

  // fill in all variables
  m_tree->Fill();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode OptimizationDump :: finalize () {
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode OptimizationDump :: histFinalize () { return EL::StatusCode::SUCCESS; }
