#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TheAccountant/Audit.h>

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

// root includes
#include <TCanvas.h>
#include <TVector3.h>

// c++ includes
#include <set>

namespace RF = RestFrames;

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"
#include <TheAccountant/VariableDefinitions.h>
namespace HF = HelperFunctions;
namespace VD = VariableDefinitions;

// this is needed to distribute the algorithm to the workers
ClassImp(Audit)
Audit :: Audit () :
  LAB("LAB", "lab"),
  GG("GG", "GG"),
  Ga("Ga", "#tilde{G}^{a}"),
  Gb("Gb", "#tilde{G}^{b}"),
  Ca1("Ca1", "C^{a}_{1}"),
  Cb1("Cb1", "C^{b}_{1}"),
  Va1("Va1", "V^{a}_{1}"),
  Va2("Va2", "V^{a}_{2}"),
  Vb1("Vb1", "V^{b}_{1}"),
  Vb2("Vb2", "V^{b}_{2}"),
  Ia1("Ia1", "I^{a}_{1}"),
  Ib1("Ib1", "I^{b}_{1}"),
  VIS("VIS", "Visible Object Jigsaws"),
  INV("INV", "Invisible Frame Jigsaws"),
  MinMassJigsaw("MINMAGG_JIGSAW", "Invisible system mass Jigsaw"),
  RapidityJigsaw("RAPIDITY_JIGSAW", "Invisible system rapidity Jigsaw"),
  ContraBoostJigsaw("CB_JIGSAW", "Contraboost invariant Jigsaw"),
  HemiJigsaw("HEM_JIGSAW", "Minimize m _{V_{a,b}} Jigsaw"),
  HemiJigsaw_Ca("HEM_JIGSAW_Ca", "Minimize m _{C_{a}} Jigsaw"),
  HemiJigsaw_Cb("HEM_JIGSAW_Cb", "Minimize m _{C_{b}} Jigsaw"),
  // Anti-QCD vars
  LAB_bkg("LAB_bkg", "lab"),
  S_bkg("CM", "CM"),
  V_bkg("V_bkg", "Vis"),
  I_bkg("I_bkg", "Inv"),
  VIS_bkg("VIS_bkg", "Visible Object Jigsaws"),
  INV_bkg("INV_bkg", "Invisible State Jigsaws"),
  MinMassJigsaw_bkg("InvMass_bkg", "Invisible system mass Jigsaw"),
  RapidityJigsaw_bkg("InvRapidity_bkg", "Invisible system rapidity Jigsaw")
{}

EL::StatusCode Audit :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("Audit").ignore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Audit :: histInitialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Audit :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Audit :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode Audit :: initialize () {
  // assign m_event and m_store
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  // if doing rc jets, use rc jets container instead
  if(m_rc_enable) m_inputLargeRJets = m_RCJetsContainerName;

  // create the decay structure
  LAB.SetChildFrame(GG);
    GG.AddChildFrame(Ga);
      Ga.AddChildFrame(Va1);
      Ga.AddChildFrame(Ca1);
      Ca1.AddChildFrame(Va2);
      Ca1.AddChildFrame(Ia1);
    GG.AddChildFrame(Gb);
      Gb.AddChildFrame(Vb1);
      Gb.AddChildFrame(Cb1);
      Cb1.AddChildFrame(Vb2);
      Cb1.AddChildFrame(Ib1);

  // The invisible group is all of the weakly interacting particles
  INV.AddFrame(Ia1);
  INV.AddFrame(Ib1);

  // the combinatoric group is the list of visible objects
  // that go into our hemispheres
  VIS.AddFrame(Va1);
  VIS.AddFrame(Va2);
  VIS.AddFrame(Vb1);
  VIS.AddFrame(Vb2);

  // require at least two elements in each hemisphere
  VIS.SetNElementsForFrame(Va1,1,false);
  VIS.SetNElementsForFrame(Va2,1,false);
  VIS.SetNElementsForFrame(Vb1,1,false);
  VIS.SetNElementsForFrame(Vb2,1,false);

  if(LAB.InitializeTree()){ Info("initialize()", "We do have consistent tree topology."); }
  else                    { Error("initialize()", "We do not have consistent tree topology."); return EL::StatusCode::FAILURE; }

  // now we define 'jigsaw rules' that tell the tree how to define the objects
  // in our groups, indented to cleanly see the Jigsaws being defined
  if(m_minMassJigsaw){
    INV.AddJigsaw(MinMassJigsaw);
  }

  if(m_rapidityJigsaw){
    INV.AddJigsaw(RapidityJigsaw);
    RapidityJigsaw.AddVisibleFrames((LAB.GetListVisibleFrames()));
  }

  if(m_contraBoostJigsaw){
    INV.AddJigsaw(ContraBoostJigsaw);
    ContraBoostJigsaw.AddVisibleFrames((Ga.GetListVisibleFrames()), 0);
    ContraBoostJigsaw.AddVisibleFrames((Gb.GetListVisibleFrames()), 1);
    ContraBoostJigsaw.AddInvisibleFrames((Ga.GetListInvisibleFrames()), 0);
    ContraBoostJigsaw.AddInvisibleFrames((Gb.GetListInvisibleFrames()), 1);
  }

  if(m_hemiJigsaw){
    VIS.AddJigsaw(HemiJigsaw);
    HemiJigsaw.AddFrame(Va1,0);
    HemiJigsaw.AddFrame(Va2,0);
    HemiJigsaw.AddFrame(Vb1,1);
    HemiJigsaw.AddFrame(Vb2,1);

    VIS.AddJigsaw(HemiJigsaw_Ca);
    HemiJigsaw_Ca.AddFrame(Va1,0);
    HemiJigsaw_Ca.AddFrame(Va2,1);
    HemiJigsaw_Ca.AddFrame(Ia1,1);

    VIS.AddJigsaw(HemiJigsaw_Cb);
    HemiJigsaw_Cb.AddFrame(Vb1,0);
    HemiJigsaw_Cb.AddFrame(Vb2,1);
    HemiJigsaw_Cb.AddFrame(Ib1,1);
  }

  if(LAB.InitializeAnalysis()){ Info("initialize()", "Our tree is ok for analysis."); }
  else                        { Error("initialize()", "Our tree is not ok for analysis."); return EL::StatusCode::FAILURE; }

  // Anti-QCD tree
  LAB_bkg.SetChildFrame(S_bkg);
  S_bkg.AddChildFrame(V_bkg);
  S_bkg.AddChildFrame(I_bkg);

  if(LAB_bkg.InitializeTree()){ Info("initialize()", "We do have consistent bkg tree topology."); }
  else                        { Error("initialize()", "We do not have consistent bkg tree topology."); return EL::StatusCode::FAILURE;}

  VIS_bkg.AddFrame(V_bkg);
  VIS_bkg.SetNElementsForFrame(V_bkg, 1, false);

  INV_bkg.AddFrame(I_bkg);
  INV_bkg.AddJigsaw(MinMassJigsaw_bkg);
  INV_bkg.AddJigsaw(RapidityJigsaw_bkg);
  RapidityJigsaw_bkg.AddVisibleFrames(LAB_bkg.GetListVisibleFrames());

  if(LAB_bkg.InitializeAnalysis()){ Info("initialize()", "Our bkg tree is ok for analysis."); }
  else                            { Error("initialize()", "Our bkg tree is not ok for analysis."); return EL::StatusCode::FAILURE; }

  /* BROKEN NEED TO FIX
  // only output this thing once, perhaps only during direct
  if(m_drawDecayTreePlots){
    RF::TreePlot* decayTree_plot = new RF::TreePlot("tree", "Decay Tree");
    // start with the lab frame
    decayTree_plot->SetFrameTree(LAB);
    // add the jigsaws
    if(m_minMassJigsaw)     decayTree_plot->AddJigsaw(MinMassJigsaw);
    if(m_rapidityJigsaw)    decayTree_plot->AddJigsaw(RapidityJigsaw);
    if(m_contraBoostJigsaw) decayTree_plot->AddJigsaw(ContraBoostJigsaw);
    if(m_hemiJigsaw)        decayTree_plot->AddJigsaw(HemiJigsaw);
    decayTree_plot->Draw();
    TCanvas* plotCanvas = decayTree_plot->GetNewCanvas("decayTree", "decayTree");
    wk()->addOutput(plotCanvas);

    RF::TreePlot* visGroup_plot = new RF::TreePlot("VIStree", "Visible Group");
    visGroup_plot->SetGroupTree(VIS);
    visGroup_plot->Draw();
    TCanvas* visPlotCanvas = visGroup_plot->GetNewCanvas("visTree", "visTree");
    wk()->addOutput(visPlotCanvas);

    RF::TreePlot* invGroup_plot = new RF::TreePlot("INVtree", "Invisible Group");
    invGroup_plot->SetGroupTree(INV);
    invGroup_plot->Draw();
    TCanvas* invPlotCanvas = invGroup_plot->GetNewCanvas("invTree", "invTree");
    wk()->addOutput(invPlotCanvas);
  }
  */

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Audit :: execute ()
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
  //const xAOD::TruthParticleContainer*   in_truth    (nullptr);

  // start grabbing all the containers that we can
  RETURN_CHECK("Audit::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  if(!m_inputLargeRJets.empty())
    RETURN_CHECK("Audit::execute()", HF::retrieve(in_jetsLargeR,      m_inputLargeRJets,        m_event, m_store, m_debug), "Could not get the inputLargeRJets container.");
  if(!m_inputJets.empty())
    RETURN_CHECK("Audit::execute()", HF::retrieve(in_jets,     m_inputJets,       m_event, m_store, m_debug), "Could not get the inputJets container.");
  if(!m_inputMET.empty())
    RETURN_CHECK("Audit::execute()", HF::retrieve(in_missinget, m_inputMET,         m_event, m_store, m_debug), "Could not get the inputMET container.");
  if(!m_inputElectrons.empty())
    RETURN_CHECK("Audit::execute()", HF::retrieve(in_electrons, m_inputElectrons,   m_event, m_store, m_debug), "Could not get the inputElectrons container.");
  if(!m_inputMuons.empty())
    RETURN_CHECK("Audit::execute()", HF::retrieve(in_muons,     m_inputMuons,       m_event, m_store, m_debug), "Could not get the inputMuons container.");
  if(!m_inputTauJets.empty())
    RETURN_CHECK("Audit::execute()", HF::retrieve(in_taus,      m_inputTauJets,     m_event, m_store, m_debug), "Could not get the inputTauJets container.");
  if(!m_inputPhotons.empty())
    RETURN_CHECK("Audit::execute()", HF::retrieve(in_photons,   m_inputPhotons,     m_event, m_store, m_debug), "Could not get the inputPhotons container.");

  const xAOD::MissingET* in_met(nullptr);
  if(!m_inputMET.empty()){
    in_met = (*in_missinget)[m_inputMETName.c_str()];
    if (!in_met) {
      Error("execute()", "No %s inside MET container", m_inputMETName.c_str());
      return EL::StatusCode::FAILURE;
    }
  }

  // clear the event
  LAB.ClearEvent();

  // only look at signal jets
  ConstDataVector<xAOD::JetContainer> signal_jets(SG::VIEW_ELEMENTS);
  if(!m_inputJets.empty()){
    signal_jets = VD::subset_using_decor(in_jets, VD::decor_signal, 1);
  }

  // create a vector to hold the group element ids for when adding jets
  std::map<const int, const xAOD::Jet*> in_jets_IDs;
  if(!m_inputJets.empty()){
    for(const auto &jet: signal_jets)
      in_jets_IDs[VIS.AddLabFrameFourVector( jet->p4() ).GetKey()] = jet;
  }

  if(!m_inputMET.empty()){
    // no mpz, but why set it this way???
    INV.SetLabFrameThreeVector(  TVector3( in_met->mpx(), in_met->mpy(), 0 ) );
  }

  // dump information about the jets and met at least
  if(m_debug){
    if(!m_inputJets.empty()){
      Info("execute()", "Details about signal jets...");
      for(const auto &jet: signal_jets)
          Info("execute()", "\tpT: %0.2f GeV\tm: %0.2f GeV\teta: %0.2f\tphi: %0.2f", jet->pt()/1000., jet->m()/1000., jet->eta(), jet->phi());
    }

    if(!m_inputMET.empty()){
      Info("execute()", "Details about MET...");
      Info("execute()", "\tpx: %0.2f GeV\tpy: %0.2f GeV\tpz: %0.2f GeV", in_met->mpx()/1000., in_met->mpy()/1000., 0.0/1000.);
    }
  }

  // analyze the event
  if(LAB.AnalyzeEvent()){ if(m_debug) Info("execute()", "Run #%u, Event #%llu: Analyzed the event successfully.", eventInfo->runNumber(), eventInfo->eventNumber()); }
  else                  { Error("execute()", "Run #%u, Event #%llu: Analyzed the event unsuccessfully.", eventInfo->runNumber(), eventInfo->eventNumber()); return EL::StatusCode::SUCCESS; }


  LAB_bkg.ClearEvent();
  double HT(0.0);
  std::map<const int, const xAOD::Jet*> in_jets_bkg_IDs;
  if(!m_inputJets.empty()){
    for(const auto &jet: signal_jets){
      TLorentzVector jet_tlv;
      jet_tlv.SetPtEtaPhiM(jet->pt(), 0.0, jet->phi(), jet->m());
      in_jets_bkg_IDs[VIS_bkg.AddLabFrameFourVector(jet_tlv).GetKey()] = jet;
      HT += jet->pt();
    }
  }

  if(!m_inputMET.empty()){
    // no mpz, but why set it this way???
    INV_bkg.SetLabFrameThreeVector(  TVector3( in_met->mpx(), in_met->mpy(), 0 ) );
  }

  if(LAB_bkg.AnalyzeEvent()){ if(m_debug) Info("execute()", "Run #%u, Event #%llu: Analyzed the bkg event successfully.", eventInfo->runNumber(), eventInfo->eventNumber()); }
  else                      { Error("execute()", "Run #%u, Event #%llu: Analyzed the bkg event unsuccessfully.", eventInfo->runNumber(), eventInfo->eventNumber()); return EL::StatusCode::SUCCESS; }

  // Signal Variables
  // https://github.com/lawrenceleejr/ZeroLeptonRun2/blob/67042394b0bca205081175f002ef3fb44fd46b98/Root/PhysObjProxyUtils.cxx#L471
  // inclusive variable definitions
  std::map<std::string, double>* inclVar_ptr(new std::map<std::string, double>);
  RETURN_CHECK("Audit::execute()", m_store->record(inclVar_ptr, "RJigsawInclusiveVariables"), "Could not record RJR Inclusive Variables.");
  auto& inclVar = *inclVar_ptr;

  inclVar["GG_mass"]        = GG.GetMass()/1.e3;
  inclVar["Ga_mass"]        = Ga.GetMass()/1.e3;
  inclVar["Gb_mass"]        = Gb.GetMass()/1.e3;
  inclVar["Ca1_mass"]       = Ca1.GetMass()/1.e3;
  inclVar["Cb1_mass"]       = Cb1.GetMass()/1.e3;
  inclVar["Va1_mass"]       = Va1.GetMass()/1.e3;
  inclVar["Vb1_mass"]       = Vb1.GetMass()/1.e3;
  inclVar["Va2_mass"]       = Va2.GetMass()/1.e3;
  inclVar["Vb2_mass"]       = Vb2.GetMass()/1.e3;

  // used for HT
  inclVar["pT_GG_Ga"]       = GG.GetTransverseMomentum(Va1.GetFourVector() + Va2.GetFourVector())/1.e3;
  inclVar["pT_Va1_GG"]      = Va1.GetTransverseMomentum(GG)/1.e3;
  inclVar["pT_Va2_GG"]      = Va2.GetTransverseMomentum(GG)/1.e3;
  inclVar["pT_GG_Gb"]       = GG.GetTransverseMomentum(Vb1.GetFourVector() + Vb2.GetFourVector())/1.e3;
  inclVar["pT_Vb1_GG"]      = Vb1.GetTransverseMomentum(GG)/1.e3;
  inclVar["pT_Vb2_GG"]      = Vb2.GetTransverseMomentum(GG)/1.e3;
  inclVar["pT_Ia1_GG"]      = Ia1.GetTransverseMomentum(GG)/1.e3;
  inclVar["pT_Ib1_GG"]      = Ib1.GetTransverseMomentum(GG)/1.e3;

  // used for gluino ratios
  inclVar["p_Ga_GG"]        = (Va1.GetFourVector(GG) + Va2.GetFourVector(GG)).P()/1.e3;
  inclVar["p_Va1_GG"]       = Va1.GetMomentum(GG)/1.e3;
  inclVar["p_Va2_GG"]       = Va2.GetMomentum(GG)/1.e3;
  inclVar["p_Gb_GG"]        = (Vb1.GetFourVector(GG) + Vb2.GetFourVector(GG)).P()/1.e3;
  inclVar["p_Vb1_GG"]       = Vb1.GetMomentum(GG)/1.e3;
  inclVar["p_Vb2_GG"]       = Vb2.GetMomentum(GG)/1.e3;
  inclVar["p_Ia1_GG"]       = Ia1.GetMomentum(GG)/1.e3;
  inclVar["p_Ib1_GG"]       = Ib1.GetMomentum(GG)/1.e3;

  //inclVar["GG_invGamma"]    = 1./GG.GetGammaInParentFrame();
  inclVar["GG_invGamma"]    = GG.GetVisibleShape();
  inclVar["GG_visShape"]    = GG.GetVisibleShape();
  inclVar["GG_mDeltaR"]     = GG.GetVisibleShape() * GG.GetMass()/1.e3;

  // counting
  inclVar["Va1_n"]          = VIS.GetNElementsInFrame(Va1);
  inclVar["Vb1_n"]          = VIS.GetNElementsInFrame(Vb1);
  inclVar["Ga_n"]           = inclVar["Va1_n"] + inclVar["Va2_n"];
  inclVar["Va2_n"]          = VIS.GetNElementsInFrame(Va2);
  inclVar["Vb2_n"]          = VIS.GetNElementsInFrame(Vb2);
  inclVar["Gb_n"]           = inclVar["Vb1_n"] + inclVar["Vb2_n"];
  inclVar["Ia1_depth"]      = Ga.GetFrameDepth(Ia1);
  inclVar["Ib1_depth"]      = Gb.GetFrameDepth(Ib1);

  inclVar["GG_cosTheta"]    = GG.GetCosDecayAngle();
  inclVar["Ga_cosIa1"]    = Ga.GetCosDecayAngle(Ia1);
  inclVar["Gb_cosIb1"]    = Gb.GetCosDecayAngle(Ib1);
  inclVar["Va1_cosTheta"]   = Ga.GetCosDecayAngle();
  inclVar["Vb1_cosTheta"]   = Gb.GetCosDecayAngle();
  inclVar["Va2_cosTheta"]   = Ca1.GetCosDecayAngle();
  inclVar["Vb2_cosTheta"]   = Cb1.GetCosDecayAngle();

  // delta phi
  inclVar["GG_dPhiVis"]     = GG.GetDeltaPhiVisible();
  inclVar["GG_dPhiBetaR"]   = GG.GetDeltaPhiBoostVisible();
  inclVar["GG_dPhiDecay"]   = GG.GetDeltaPhiDecayVisible();
  inclVar["dPhi_Ga_Va1"]    = Ga.GetDeltaPhiDecayPlanes(Va1);
  inclVar["dPhi_Ga_Ca1"]    = Ga.GetDeltaPhiDecayPlanes(Ca1);
  inclVar["dPhi_Gb_Vb1"]    = Gb.GetDeltaPhiDecayPlanes(Vb1);
  inclVar["dPhi_Gb_Cb1"]    = Gb.GetDeltaPhiDecayPlanes(Cb1);
  inclVar["dPhi_Ca1_Va2"]   = Ca1.GetDeltaPhiDecayPlanes(Va2);
  inclVar["dPhi_Cb1_Vb2"]   = Cb1.GetDeltaPhiDecayPlanes(Vb2);

  // momentum (P) vectors
  std::map<std::string, TLorentzVector>* vP_ptr(new std::map<std::string, TLorentzVector>);
  RETURN_CHECK("Audit::execute()", m_store->record(vP_ptr, "RJigsawFourVectors"), "Could not record RJR 4-Vectors.");
  auto& vP = *vP_ptr;

  vP["GG"]                  = GG.GetFourVector(LAB);
  vP["Ga"]                  = Ga.GetVisibleFourVector(Ga);
  vP["Gb"]                  = Gb.GetVisibleFourVector(Gb);
  vP["Va1_GG"]              = Va1.GetFourVector(GG);
  vP["Va2_GG"]              = Va2.GetFourVector(GG);
  vP["Vb1_GG"]              = Vb1.GetFourVector(GG);
  vP["Vb2_GG"]              = Vb2.GetFourVector(GG);
  vP["Ia1_GG"]              = Ia1.GetFourVector(GG);
  vP["Ib1_GG"]              = Ib1.GetFourVector(GG);
  vP["Va1_Ga"]              = Va1.GetFourVector(Ga);
  vP["Va2_Ga"]              = Va2.GetFourVector(Ga);
  vP["Ia1_Ga"]              = Ia1.GetFourVector(Ga);
  vP["Vb1_Gb"]              = Vb1.GetFourVector(Gb);
  vP["Vb2_Gb"]              = Vb2.GetFourVector(Gb);
  vP["Ib1_Gb"]              = Ib1.GetFourVector(Gb);
  vP["Va2_Ca1"]             = Va2.GetFourVector(Ca1);
  vP["Ia1_Ca1"]             = Ia1.GetFourVector(Ca1);
  vP["Vb2_Cb1"]             = Vb2.GetFourVector(Cb1);
  vP["Ib1_Cb1"]             = Ib1.GetFourVector(Cb1);

  // CM variables
  inclVar["pT_GG"] = vP["GG"].Pt()/1.e3;
  inclVar["pZ_GG"] = std::fabs(vP["GG"].Pz())/1.e3;

  // H-variables (H_{n,m}^{F} )
  inclVar["H11GG"]       = (vP["Va1_GG"] + vP["Va2_GG"] + vP["Vb1_GG"] + vP["Vb2_GG"]).P()/1.e3   + (vP["Ia1_GG"] + vP["Ib1_GG"]).P()/1.e3;
  inclVar["H21GG"]       = (vP["Va1_GG"] + vP["Va2_GG"]).P()/1.e3 + (vP["Vb1_GG"] + vP["Vb2_GG"]).P()/1.e3 + (vP["Ia1_GG"] + vP["Ib1_GG"]).P()/1.e3;
  inclVar["H22GG"]       = (vP["Va1_GG"] + vP["Va2_GG"]).P()/1.e3 + (vP["Vb1_GG"] + vP["Vb2_GG"]).P()/1.e3 + vP["Ia1_GG"].P()/1.e3 + vP["Ib1_GG"].P()/1.e3;
  inclVar["H41GG"]       = vP["Va1_GG"].P()/1.e3 + vP["Va2_GG"].P()/1.e3 + vP["Vb1_GG"].P()/1.e3 + vP["Vb2_GG"].P()/1.e3 + (vP["Ia1_GG"] + vP["Ib1_GG"]).P()/1.e3;
  inclVar["H42GG"]       = vP["Va1_GG"].P()/1.e3 + vP["Va2_GG"].P()/1.e3 + vP["Vb1_GG"].P()/1.e3 + vP["Vb2_GG"].P()/1.e3 + vP["Ia1_GG"].P()/1.e3 + vP["Ib1_GG"].P()/1.e3;

  inclVar["H11Ga"]       = (vP["Va1_Ga"] + vP["Va2_Ga"]).P()/1.e3 + vP["Ia1_Ga"].P()/1.e3;
  inclVar["H11Gb"]       = (vP["Vb1_Gb"] + vP["Vb2_Gb"]).P()/1.e3 + vP["Ib1_Gb"].P()/1.e3;
  inclVar["H21Ga"]       = vP["Va1_Ga"].P()/1.e3 + vP["Va2_Ga"].P()/1.e3 + vP["Ia1_Ga"].P()/1.e3;
  inclVar["H21Gb"]       = vP["Vb1_Gb"].P()/1.e3 + vP["Vb2_Gb"].P()/1.e3 + vP["Ib1_Gb"].P()/1.e3;

  inclVar["H11Ca1"]      = vP["Va2_Ca1"].P()/1.e3 + vP["Ia1_Ca1"].P()/1.e3;
  inclVar["H11Cb1"]      = vP["Vb2_Cb1"].P()/1.e3 + vP["Ib1_Cb1"].P()/1.e3;

  inclVar["HT21GG"]      = inclVar["pT_GG_Ga"] + inclVar["pT_GG_Gb"] + inclVar["H11GG"]/2.;
  inclVar["HT22GG"]      = inclVar["pT_GG_Ga"] + inclVar["pT_GG_Gb"] + inclVar["pT_Ia1_GG"] + inclVar["pT_Ib1_GG"];
  inclVar["HT41GG"]      = inclVar["pT_Va1_GG"] + inclVar["pT_Va2_GG"] + inclVar["pT_Vb1_GG"] + inclVar["pT_Vb2_GG"] + inclVar["H11GG"]/2.;
  inclVar["HT42GG"]      = inclVar["pT_Va1_GG"] + inclVar["pT_Va2_GG"] + inclVar["pT_Vb1_GG"] + inclVar["pT_Vb2_GG"] + inclVar["pT_Ia1_GG"] + inclVar["pT_Ib1_GG"];

  // gluino hemishpere variables
  double ddphiGa = inclVar["dPhi_Ga_Ca1"];
  double ddphiGb = inclVar["dPhi_Gb_Cb1"];
  if(ddphiGa > std::acos(-1.)) ddphiGa = 2.*std::acos(-1.) - ddphiGa;
  if(ddphiGb > std::acos(-1.)) ddphiGb = 2.*std::acos(-1.) - ddphiGb;
  inclVar["d_dPhiG"] = (ddphiGa - ddphiGb)/std::acos(-1.);
  inclVar["s_dPhiG"] = std::fabs(ddphiGa + ddphiGb)/2./std::acos(-1.);

  // sangle and dangle
  inclVar["s_angle"]        = std::fabs(inclVar["GG_dPhiDecay"] + 2.*inclVar["Ga_cosIa1"])/3.;
  inclVar["d_angle"]        = (2.*inclVar["GG_dPhiDecay"] - inclVar["Ga_cosIa1"])/3.;

  // Other variables -- what do we do with them???
  //inclVar["dPhiVP"]         = (GG.GetDeltaPhiDecayVisible()-std::acos(-1.)/2.)/(std::acos(-1.)/2.);

  // ratios
  inclVar["ratio_pZGG_HT21GG"] = inclVar["pZ_GG"]/(inclVar["pZ_GG"] + inclVar["HT21GG"]);
  inclVar["ratio_pZGG_HT41GG"] = inclVar["pZ_GG"]/(inclVar["pZ_GG"] + inclVar["HT41GG"]);
  inclVar["ratio_pTGG_HT21GG"] = inclVar["pT_GG"]/(inclVar["pT_GG"] + inclVar["HT21GG"]);
  inclVar["ratio_pTGG_HT41GG"] = inclVar["pT_GG"]/(inclVar["pT_GG"] + inclVar["HT41GG"]);

  inclVar["ratio_H11GG_H21GG"]    = inclVar["H11GG"]/inclVar["H21GG"];
  inclVar["ratio_HT41GG_H41GG"]   = inclVar["HT41GG"]/inclVar["H41GG"];
  inclVar["ratio_H11GG_H41GG"]    = inclVar["H11GG"]/inclVar["H41GG"];
  inclVar["maxRatio_H10PP_H11PP"] = std::max(
                                            inclVar["p_Ga_GG"]/(inclVar["p_Va1_GG"] + inclVar["p_Va2_GG"]),
                                            inclVar["p_Gb_GG"]/(inclVar["p_Vb1_GG"] + inclVar["p_Vb2_GG"])
                                          );
  // TODO: ratios not included
  //inclVar["m_RPZ_HT9PP"]
  //inclVar["m_RPT_HT9PP"]
  //inclVar["R_pTj2_HT3PP"]
  //inclVar["minR_pTj2i_HT3PPi"]
  //inclVar["R_HT9PP_H9PP"]
  //inclVar["R_H2PP_H9PP"]

  // QCD Variables
  TLorentzVector Psib = I_bkg.GetSiblingFrame().GetFourVector(LAB_bkg);
  TLorentzVector Pmet = I_bkg.GetFourVector(LAB_bkg);
  inclVar["temp_Rsib"] = std::max(0.0, Psib.Vect().Dot(Pmet.Vect().Unit()));
  inclVar["Rsib"] = inclVar["temp_Rsib"] / (Pmet.Pt() + inclVar["temp_Rsib"]);
  inclVar["Pmet_pT"] = Pmet.Pt();
  inclVar["Psib_phi"] = Psib.Phi();
  inclVar["Pmet_phi"] = Pmet.Phi();
  TVector3 boostQCD = (Pmet + Psib).BoostVector();
  Psib.Boost(-boostQCD);
  inclVar["cosQCD"] = (1. + Psib.Vect().Unit().Dot(boostQCD.Unit()))/2.;
  inclVar["deltaQCD"] = (inclVar["cosQCD"] - inclVar["Rsib"])/(inclVar["cosQCD"] + inclVar["Rsib"]);

  if(m_debug){
    Info("execute()", "Details about RestFrames analysis...");
    Info("execute()", "\tinclVar");
    for(const auto& item: inclVar)
      Info("execute()", "\t\t%30s: %0.4f", item.first.c_str(), item.second);

    Info("execute()", "\tMomentum Vectors");
    for(const auto& item: vP)
      Info("execute()", "\t\t%30s: (pT, eta, phi, m) = (%0.4f, %0.4f, %0.4f, %0.4f)", item.first.c_str(), item.second.Pt()/1.e3, item.second.Eta(), item.second.Phi(), item.second.M()/1.e3);
  }

  /* TODO
     QCD rejection stuff
  */
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Audit :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Audit :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Audit :: histFinalize () { return EL::StatusCode::SUCCESS; }
