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
  SS("SS", "SS"),
  S1("S1", "#tilde{S}_{a}"),
  S2("S2", "#tilde{S}_{b}"),
  V1("V1", "V_{a}"),
  V2("V2", "V_{b}"),
  I1("I1", "I_{a}"),
  I2("I2", "I_{b}"),
  VIS("VIS", "Visible Object Jigsaws"),
  INV("INV", "Invisible Frame Jigsaws"),
  MinMassJigsaw("MINMASS_JIGSAW", "Invisible system mass Jigsaw"),
  RapidityJigsaw("RAPIDITY_JIGSAW", "Invisible system rapidity Jigsaw"),
  ContraBoostJigsaw("CB_JIGSAW", "Contraboost invariant Jigsaw"),
  HemiJigsaw("HEM_JIGSAW", "Minimize m _{V_{a,b}} Jigsaw")
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
  LAB.SetChildFrame(SS);
  SS.AddChildFrame(S1);
  SS.AddChildFrame(S2);
  S1.AddChildFrame(V1);
  S1.AddChildFrame(I1);
  S2.AddChildFrame(V2);
  S2.AddChildFrame(I2);

  // The invisible group is all of the weakly interacting particles
  INV.AddFrame(I1);
  INV.AddFrame(I2);

  // the combinatoric group is the list of visible objects
  // that go into our hemispheres
  VIS.AddFrame(V1);
  VIS.AddFrame(V2);
  // require at least one element in each hemisphere
  VIS.SetNElementsForFrame(V1,1,false);
  VIS.SetNElementsForFrame(V2,1,false);

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
    ContraBoostJigsaw.AddVisibleFrames((S1.GetListVisibleFrames()), 0);
    ContraBoostJigsaw.AddVisibleFrames((S2.GetListVisibleFrames()), 1);
    ContraBoostJigsaw.AddInvisibleFrames((S1.GetListInvisibleFrames()), 0);
    ContraBoostJigsaw.AddInvisibleFrames((S2.GetListInvisibleFrames()), 1);
  }

  if(m_hemiJigsaw){
    VIS.AddJigsaw(HemiJigsaw);
    HemiJigsaw.AddFrame(V1,0);
    HemiJigsaw.AddFrame(V2,1);
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
  static VD::decor_t<float> SS_mass_decor             ("SS_mass");
  static VD::decor_t<float> SS_invgamma_decor         ("SS_invgamma");
  static VD::decor_t<float> SS_dphivis_decor          ("SS_dphivis");
  static VD::decor_t<float> SS_costheta_decor         ("SS_costheta");
  static VD::decor_t<float> SS_dphidecayangle_decor   ("SS_dphidecayangle");
  static VD::decor_t<float> SS_mdeltaR_decor          ("SS_mdeltaR");
  static VD::decor_t<float> S1_mass_decor             ("S1_mass");
  static VD::decor_t<float> S2_mass_decor             ("S2_mass");
  static VD::decor_t<float> S1_costheta_decor         ("S1_costheta");
  static VD::decor_t<float> S2_costheta_decor         ("S2_costheta");
  static VD::decor_t<float> I1_depth_decor            ("I1_depth");
  static VD::decor_t<float> I2_depth_decor            ("I2_depth");
  static VD::decor_t<float> V1_nelements_decor        ("V1_nelements");
  static VD::decor_t<float> V2_nelements_decor        ("V2_nelements");
  // initialize to zero on event
  SS_mass_decor(*eventInfo)             = -99;
  SS_invgamma_decor(*eventInfo)         = -99;
  SS_dphivis_decor(*eventInfo)          = -99;
  SS_costheta_decor(*eventInfo)         = -99;
  SS_dphidecayangle_decor(*eventInfo)   = -99;
  SS_mdeltaR_decor(*eventInfo)          = -99;
  S1_mass_decor(*eventInfo)             = -99;
  S2_mass_decor(*eventInfo)             = -99;
  S1_costheta_decor(*eventInfo)         = -99;
  S2_costheta_decor(*eventInfo)         = -99;
  I1_depth_decor(*eventInfo)            = -99;
  I2_depth_decor(*eventInfo)            = -99;
  V1_nelements_decor(*eventInfo)        = -99;
  V2_nelements_decor(*eventInfo)        = -99;


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
  // PP = SS
  // Pa = S1, Pb = S2
  // V1a = V1, V1b = V2
  // Ia = I1, Ib = I2
  typedef TLorentzVector TLV;

  // momentum (P) vectors
  std::map<std::string, TLV> vP;
  vP["S1"] = S1.GetVisibleFourVector(S1);
  vP["S2"] = S2.GetVisibleFourVector(S2);
  vP["V1_SS"] = V1.GetFourVector(SS);
  vP["V2_SS"] = V2.GetFourVector(SS);
  vP["I1_SS"] = I1.GetFourVector(SS);
  vP["I2_SS"] = I2.GetFourVector(SS);
  vP["V1_S1"] = V1.GetFourVector(S1);
  vP["I1_S1"] = I1.GetFourVector(S1);
  vP["V2_S2"] = V2.GetFourVector(S2);
  vP["I2_S2"] = I2.GetFourVector(S2);

  // H-variables (H_{n,m}^{F} )
  /*
    H2PP = H1,1PP
    H3PP = H2,1PP
    H4PP = H2,2PP
    H5PP = H4,1PP
    H6PP = H4,2PP
  */
  std::map<std::string, double> Hvar; // format is "n,m;F"
  Hvar["1,1;SS"] = (vP["V1_SS"] + vP["V2_SS"]).P()   + (vP["I1_SS"] + vP["I2_SS"]).P();
  Hvar["2,1;SS"] = vP["V1_SS"].P() + vP["V2_SS"].P() + (vP["I1_SS"] + vP["I2_SS"]).P();
  Hvar["2,2;SS"] = vP["V1_SS"].P() + vP["V2_SS"].P() + vP["I1_SS"].P() + vP["I2_SS"].P();
  //Hvar["4,1;SS"] = Hvar["2,1;SS"];
  //Hvar["4,2;SS"] = Hvar["2,2;SS"];

  Hvar["1,1;S1"] = vP["V1_S1"].P() + vP["I1_S1"].P();
  Hvar["1,1;S2"] = vP["V2_S2"].P() + vP["I2_S2"].P();
  //Hvar["2,1;S1"] = Hvar["1,1;S1"];
  //Hvar["2,1;S2"] = Hvar["1,1;S2"];

  if(m_debug){
    Info("execute()", "Details about RestFrames analysis...");
    Info("execute()", "\tSS...");
    Info("execute()", "\t\tMass:          %0.2f GeV", SS.GetMass()/1000.);
    Info("execute()", "\t\tInvGamma:      %0.2f", 1./SS.GetGammaInParentFrame());
    Info("execute()", "\t\tdPhiVis:       %0.2f", SS.GetDeltaPhiBoostVisible());
    Info("execute()", "\t\tCosTheta:      %0.2f", SS.GetCosDecayAngle());
    Info("execute()", "\t\tdPhiDecayAngle:%0.2f", SS.GetDeltaPhiDecayAngle());
    Info("execute()", "\t\tVisShape:      %0.2f", SS.GetVisibleShape());
    Info("execute()", "\t\tMDeltaR:       %0.2f", SS.GetVisibleShape()*SS.GetMass());
    Info("execute()", "\tS1...");
    Info("execute()", "\t\tMass:          %0.2f GeV", S1.GetMass()/1000.);
    Info("execute()", "\t\tCosTheta:      %0.2f", S1.GetCosDecayAngle());
    Info("execute()", "\tS2...");
    Info("execute()", "\t\tMass:          %0.2f GeV", S2.GetMass()/1000.);
    Info("execute()", "\t\tCosTheta:      %0.2f", S2.GetCosDecayAngle());
    Info("execute()", "\tI1...");
    Info("execute()", "\t\tDepth:         %d",    S1.GetFrameDepth(I1));
    Info("execute()", "\tI2...");
    Info("execute()", "\t\tDepth:         %d",    S2.GetFrameDepth(I2));
    Info("execute()", "\tV1...");
    Info("execute()", "\t\tNElements:     %d",    VIS.GetNElementsInFrame(V1));
    Info("execute()", "\tV2...");
    Info("execute()", "\t\tNElements:     %d",    VIS.GetNElementsInFrame(V2));
  }

  SS_mass_decor(*eventInfo)             = SS.GetMass();
  SS_invgamma_decor(*eventInfo)         = 1/SS.GetGammaInParentFrame();
  SS_dphivis_decor(*eventInfo)          = SS.GetDeltaPhiBoostVisible();
  SS_costheta_decor(*eventInfo)         = SS.GetCosDecayAngle();
  SS_dphidecayangle_decor(*eventInfo)   = SS.GetDeltaPhiDecayAngle();
  SS_mdeltaR_decor(*eventInfo)          = SS.GetVisibleShape()*SS.GetMass();
  S1_mass_decor(*eventInfo)             = S1.GetMass();
  S2_mass_decor(*eventInfo)             = S2.GetMass();
  S1_costheta_decor(*eventInfo)         = S1.GetCosDecayAngle();
  S2_costheta_decor(*eventInfo)         = S2.GetCosDecayAngle();
  I1_depth_decor(*eventInfo)            = S1.GetFrameDepth(I1);
  I2_depth_decor(*eventInfo)            = S2.GetFrameDepth(I2);
  V1_nelements_decor(*eventInfo)        = VIS.GetNElementsInFrame(V1);
  V2_nelements_decor(*eventInfo)        = VIS.GetNElementsInFrame(V2);

  /* TODO
     QCD rejection stuff
  */
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Audit :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Audit :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Audit :: histFinalize () { return EL::StatusCode::SUCCESS; }
