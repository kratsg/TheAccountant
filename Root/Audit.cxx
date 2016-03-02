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
  HemiJigsaw_Cb("HEM_JIGSAW_Cb", "Minimize m _{C_{b}} Jigsaw")
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

  // define razor decorations
  static VD::decor_t<float> GG_mass_decor             ("GG_mass");
  static VD::decor_t<float> GG_invgamma_decor         ("GG_invgamma");
  static VD::decor_t<float> GG_dphivis_decor          ("GG_dphivis");
  static VD::decor_t<float> GG_costheta_decor         ("GG_costheta");
  static VD::decor_t<float> GG_dphidecayangle_decor   ("GG_dphidecayangle");
  static VD::decor_t<float> GG_mdeltaR_decor          ("GG_mdeltaR");
  static VD::decor_t<float> Ga_mass_decor             ("Ga_mass");
  static VD::decor_t<float> Gb_mass_decor             ("Gb_mass");
  static VD::decor_t<float> Ga_costheta_decor         ("Ga_costheta");
  static VD::decor_t<float> Gb_costheta_decor         ("Gb_costheta");
  static VD::decor_t<float> Ia1_depth_decor            ("Ia1_depth");
  static VD::decor_t<float> Ib1_depth_decor            ("Ib1_depth");
  static VD::decor_t<float> Va1_nelements_decor        ("Va1_nelements");
  static VD::decor_t<float> Va2_nelements_decor        ("Va2_nelements");
  static VD::decor_t<float> Vb1_nelements_decor        ("Vb1_nelements");
  static VD::decor_t<float> Vb2_nelements_decor        ("Vb2_nelements");
  // initialize to zero on event
  GG_mass_decor(*eventInfo)             = -99;
  GG_invgamma_decor(*eventInfo)         = -99;
  GG_dphivis_decor(*eventInfo)          = -99;
  GG_costheta_decor(*eventInfo)         = -99;
  GG_dphidecayangle_decor(*eventInfo)   = -99;
  GG_mdeltaR_decor(*eventInfo)          = -99;
  Ga_mass_decor(*eventInfo)             = -99;
  Gb_mass_decor(*eventInfo)             = -99;
  Ga_costheta_decor(*eventInfo)         = -99;
  Gb_costheta_decor(*eventInfo)         = -99;
  Ia1_depth_decor(*eventInfo)            = -99;
  Ib1_depth_decor(*eventInfo)            = -99;
  Va1_nelements_decor(*eventInfo)        = -99;
  Va2_nelements_decor(*eventInfo)        = -99;
  Vb1_nelements_decor(*eventInfo)        = -99;
  Vb2_nelements_decor(*eventInfo)        = -99;


  // clear the event
  LAB.ClearEvent();

  // create a vector to hold the group element ids for when adding jets
  std::map<const int, const xAOD::Jet*> in_jets_IDs;
  if(!m_inputJets.empty()){
    for(const auto &jet: *in_jets)
      in_jets_IDs[VIS.AddLabFrameFourVector( jet->p4() ).GetKey()] = jet;
  }

  if(!m_inputMET.empty()){
    // no mpz, but why set it this way???
    INV.SetLabFrameThreeVector(  TVector3( in_met->mpx(), in_met->mpy(), 0 ) );
  }

  // dump information about the jets and met at least
  if(m_debug){
    if(!m_inputJets.empty()){
      Info("execute()", "Details about input jets...");
      for(const auto &jet: *in_jets)
          Info("execute()", "\tpT: %0.2f GeV\tm: %0.2f GeV\teta: %0.2f\tphi: %0.2f", jet->pt()/1000., jet->m()/1000., jet->eta(), jet->phi());
    }

    if(!m_inputMET.empty()){
      Info("execute()", "Details about MET...");
      Info("execute()", "\tpx: %0.2f GeV\tpy: %0.2f GeV\tpz: %0.2f GeV", in_met->mpx()/1000., in_met->mpy()/1000., 0.0/1000.);
    }
  }

  // analyze the event
  if(LAB.AnalyzeEvent()){ if(m_debug) Info("execute()", "Analyzed the event successfully."); }
  else                  { Error("execute()", "Run #%u, Event #%llu: Analyzed the event unsuccessfully.", eventInfo->runNumber(), eventInfo->eventNumber()); return EL::StatusCode::SUCCESS; }

  // Signal Variables
  // https://github.com/lawrenceleejr/ZeroLeptonRun2/blob/67042394b0bca205081175f002ef3fb44fd46b98/Root/PhysObjProxyUtils.cxx#L471
  typedef TLorentzVector TLV;

  // momentum (P) vectors
  std::map<std::string, TLV> vP;
  vP["Ga"] = Ga.GetVisibleFourVector(Ga);
  vP["Gb"] = Gb.GetVisibleFourVector(Gb);
  vP["Va1_GG"] = Va1.GetFourVector(GG);
  vP["Va2_GG"] = Va2.GetFourVector(GG);
  vP["Vb1_GG"] = Vb1.GetFourVector(GG);
  vP["Vb2_GG"] = Vb2.GetFourVector(GG);
  vP["Ia1_GG"] = Ia1.GetFourVector(GG);
  vP["Ib1_GG"] = Ib1.GetFourVector(GG);
  vP["Va1_Ga"] = Va1.GetFourVector(Ga);
  vP["Va2_Ga"] = Va2.GetFourVector(Ga);
  vP["Ia1_Ga"] = Ia1.GetFourVector(Ga);
  vP["Vb1_Gb"] = Vb1.GetFourVector(Gb);
  vP["Vb2_Gb"] = Vb2.GetFourVector(Gb);
  vP["Ib1_Gb"] = Ib1.GetFourVector(Gb);

  // H-variables (H_{n,m}^{F} )
  /*
    H2PP = H1,1PP
    H3PP = H2,1PP
    H4PP = H2,2PP
    H5PP = H4,1PP
    H6PP = H4,2PP
  */
  std::map<std::string, double> Hvar; // format is "n,m;F"
  Hvar["1,1;GG"] = (vP["Va1_GG"] + vP["Va2_GG"] + vP["Vb1_GG"] + vP["Vb2_GG"]).P()   + (vP["Ia1_GG"] + vP["Ib1_GG"]).P();
  Hvar["2,1;GG"] = (vP["Va1_GG"] + vP["Va2_GG"]).P() + (vP["Vb1_GG"] + vP["Vb2_GG"]).P() + (vP["Ia1_GG"] + vP["Ib1_GG"]).P();
  Hvar["2,2;GG"] = (vP["Va1_GG"] + vP["Va2_GG"]).P() + (vP["Vb1_GG"] + vP["Vb2_GG"]).P() + vP["Ia1_GG"].P() + vP["Ib1_GG"].P();
  Hvar["4,1;GG"] = vP["Va1_GG"].P() + vP["Va2_GG"].P() + vP["Vb1_GG"].P() + vP["Vb2_GG"].P() + (vP["Ia1_GG"] + vP["Ib1_GG"]).P();
  Hvar["4,2;GG"] = vP["Va1_GG"].P() + vP["Va2_GG"].P() + vP["Vb1_GG"].P() + vP["Vb2_GG"].P() + vP["Ia1_GG"].P() + vP["Ib1_GG"].P();

  Hvar["1,1;Ga"] = (vP["Va1_Ga"] + vP["Va2_Ga"]).P() + vP["Ia1_Ga"].P();
  Hvar["1,1;Gb"] = (vP["Vb1_Gb"] + vP["Vb2_Gb"]).P() + vP["Ib1_Gb"].P();
  Hvar["2,1;Ga"] = vP["Va1_Ga"].P() + vP["Va2_Ga"].P() + vP["Ia1_Ga"].P();
  Hvar["2,1;Gb"] = vP["Vb1_Gb"].P() + vP["Vb2_Gb"].P() + vP["Ib1_Gb"].P();

  // Other variables
  std::map<std::string, float> RJRVars;
  RJRVars["GG_mass"]        = GG.GetMass();
  RJRVars["GG_invGamma"]    = 1./GG.GetGammaInParentFrame();
  RJRVars["GG_dphiVis"]     = GG.GetDeltaPhiBoostVisible();
  RJRVars["GG_cosTheta"]    = GG.GetCosDecayAngle();
  RJRVars["GG_VisShape"]    = GG.GetVisibleShape();
  RJRVars["GG_MDR"]         = GG.GetVisibleShape() * GG.GetMass();

  RJRVars["Ga_mass"]        = Ga.GetMass();
  RJRVars["Ga_cosTheta"]    = Ga.GetCosDecayAngle();
  RJRVars["Gb_mass"]        = Gb.GetMass();
  RJRVars["Gb_cosTheta"]    = Gb.GetCosDecayAngle();
  RJRVars["Ia1_depth"]       = Ga.GetFrameDepth(Ia1);
  RJRVars["Ib1_depth"]       = Gb.GetFrameDepth(Ib1);
  RJRVars["Va1_n"]           = VIS.GetNElementsInFrame(Va1);
  RJRVars["Va2_n"]           = VIS.GetNElementsInFrame(Va2);
  RJRVars["Vb1_n"]           = VIS.GetNElementsInFrame(Vb1);
  RJRVars["Vb2_n"]           = VIS.GetNElementsInFrame(Vb2);

  RJRVars["cosP"]           = Ga.GetCosDecayAngle(Ia1);
  RJRVars["dPhiVP"]         = (GG.GetDeltaPhiDecayVisible()-std::acos(-1.)/2.)/(std::acos(-1.)/2.);
  RJRVars["s_angle"]        = std::fabs(GG.GetDeltaPhiDecayVisible() + 2.*RJRVars["cosP"])/3.;
  RJRVars["d_angle"]        = (2.*GG.GetDeltaPhiDecayVisible() - RJRVars["cosP"])/3.;

  RJRVars["pT_Va1_GG"]      = Va1.GetTransverseMomentum(GG);
  RJRVars["pT_Va2_GG"]      = Va2.GetTransverseMomentum(GG);
  RJRVars["pT_Vb1_GG"]      = Vb1.GetTransverseMomentum(GG);
  RJRVars["pT_Vb2_GG"]      = Vb2.GetTransverseMomentum(GG);
  RJRVars["pT_Ia1_GG"]      = Ia1.GetTransverseMomentum(GG);
  RJRVars["pT_Ib1_GG"]      = Ib1.GetTransverseMomentum(GG);

  RJRVars["p_Va1_GG"]       = Va1.GetMomentum(GG);
  RJRVars["p_Va2_GG"]       = Va2.GetMomentum(GG);
  RJRVars["p_Vb1_GG"]       = Vb1.GetMomentum(GG);
  RJRVars["p_Vb2_GG"]       = Vb2.GetMomentum(GG);
  RJRVars["p_Ia1_GG"]       = Ia1.GetMomentum(GG);
  RJRVars["p_Ib1_GG"]       = Ib1.GetMomentum(GG);

  if(m_debug){
    Info("execute()", "Details about RestFrames analysis...");
    Info("execute()", "\tRJRVars");
    for(const auto& item: RJRVars)
      Info("execute()", "\t\t%10s: %0.4f", item.first.c_str(), item.second);

    Info("execute()", "\tMomentum Vectors");
    for(const auto& item: vP)
      Info("execute()", "\t\t%10s: (pT, eta, phi, m) = (%0.4f, %0.4f, %0.4f, %0.4f)", item.first.c_str(), item.second.Pt(), item.second.Eta(), item.second.Phi(), item.second.M());

    Info("execute()", "\tH-variables");
    for(const auto& item: Hvar)
      Info("execute()", "\t\t%10s: %0.4f", item.first.c_str(), item.second);
  }

  GG_mass_decor(*eventInfo)             = GG.GetMass();
  GG_invgamma_decor(*eventInfo)         = 1/GG.GetGammaInParentFrame();
  GG_dphivis_decor(*eventInfo)          = GG.GetDeltaPhiBoostVisible();
  GG_costheta_decor(*eventInfo)         = GG.GetCosDecayAngle();
  GG_dphidecayangle_decor(*eventInfo)   = GG.GetDeltaPhiDecayAngle();
  GG_mdeltaR_decor(*eventInfo)          = GG.GetVisibleShape()*GG.GetMass();
  Ga_mass_decor(*eventInfo)             = Ga.GetMass();
  Gb_mass_decor(*eventInfo)             = Gb.GetMass();
  Ga_costheta_decor(*eventInfo)         = Ga.GetCosDecayAngle();
  Gb_costheta_decor(*eventInfo)         = Gb.GetCosDecayAngle();
  Ia1_depth_decor(*eventInfo)           = Ga.GetFrameDepth(Ia1);
  Ib1_depth_decor(*eventInfo)           = Gb.GetFrameDepth(Ib1);
  Va1_nelements_decor(*eventInfo)       = VIS.GetNElementsInFrame(Va1);
  Va2_nelements_decor(*eventInfo)       = VIS.GetNElementsInFrame(Va2);
  Vb1_nelements_decor(*eventInfo)       = VIS.GetNElementsInFrame(Vb1);
  Vb2_nelements_decor(*eventInfo)       = VIS.GetNElementsInFrame(Vb2);

  /* TODO
     QCD rejection stuff
  */
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Audit :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Audit :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Audit :: histFinalize () { return EL::StatusCode::SUCCESS; }
