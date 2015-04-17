#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TheAccountant/ClassifyEvent.h>

// RestFrames includes
#include "RestFrames/FramePlot.hh"

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
#include <TCanvas.h>
#include <TVector3.h>

// c++ includes
#include <set>

namespace HF = HelperFunctions;
namespace RF = RestFrames;

// this is needed to distribute the algorithm to the workers
ClassImp(ClassifyEvent)
ClassifyEvent :: ClassifyEvent () :
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

EL::StatusCode ClassifyEvent :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("ClassifyEvent").ignore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ClassifyEvent :: histInitialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode ClassifyEvent :: initialize () {
  // assign m_event and m_store
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

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
    RapidityJigsaw.AddVisibleFrame((LAB.GetListVisibleFrames()));
  }

  if(m_contraBoostJigsaw){
    INV.AddJigsaw(ContraBoostJigsaw);
    ContraBoostJigsaw.AddVisibleFrame((S1.GetListVisibleFrames()), 0);
    ContraBoostJigsaw.AddVisibleFrame((S2.GetListVisibleFrames()), 1);
    ContraBoostJigsaw.AddInvisibleFrame((S1.GetListInvisibleFrames()), 0);
    ContraBoostJigsaw.AddInvisibleFrame((S2.GetListInvisibleFrames()), 1);
  }

  if(m_hemiJigsaw){
    VIS.AddJigsaw(HemiJigsaw);
    HemiJigsaw.AddFrame(V1,0);
    HemiJigsaw.AddFrame(V2,1);
  }

  if(LAB.InitializeAnalysis()){ Info("initialize()", "Our tree is ok for analysis."); }
  else                        { Error("initialize()", "Our tree is not ok for analysis."); return EL::StatusCode::FAILURE; }

  // only output this thing once, perhaps only during direct
  if(m_drawDecayTreePlots){
    RF::FramePlot* decayTree_plot = new RF::FramePlot("tree", "Decay Tree");
    // start with the lab frame
    decayTree_plot->AddFrameTree(LAB);
    // add the jigsaws
    if(m_minMassJigsaw)     decayTree_plot->AddJigsaw(MinMassJigsaw);
    if(m_rapidityJigsaw)    decayTree_plot->AddJigsaw(RapidityJigsaw);
    if(m_contraBoostJigsaw) decayTree_plot->AddJigsaw(ContraBoostJigsaw);
    if(m_hemiJigsaw)        decayTree_plot->AddJigsaw(HemiJigsaw);
    decayTree_plot->DrawFramePlot();
    TCanvas* plotCanvas = decayTree_plot->GetCanvas();
    plotCanvas->SetName("decayTree");
    wk()->addOutput(plotCanvas);

    RF::FramePlot* visGroup_plot = new RF::FramePlot("VIStree", "Visible Group");
    visGroup_plot->AddGroupTree(VIS);
    visGroup_plot->DrawFramePlot();
    TCanvas* visPlotCanvas = visGroup_plot->GetCanvas();
    visPlotCanvas->SetName("visTree");
    wk()->addOutput(visPlotCanvas);

    RF::FramePlot* invGroup_plot = new RF::FramePlot("INVtree", "Invisible Group");
    invGroup_plot->AddGroupTree(INV);
    invGroup_plot->DrawFramePlot();
    TCanvas* invPlotCanvas = invGroup_plot->GetCanvas();
    invPlotCanvas->SetName("invTree");
    wk()->addOutput(invPlotCanvas);
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ClassifyEvent :: execute ()
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
  RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  if(!m_inputJets.empty())
    RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(in_jets,      m_inputJets,        m_event, m_store, m_debug), "Could not get the inputJets container.");
  if(!m_inputBJets.empty())
    RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(in_bjets,     m_inputBJets,       m_event, m_store, m_debug), "Could not get the inputBJets container.");
  if(!m_inputMET.empty())
    RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(in_missinget, m_inputMET,         m_event, m_store, m_debug), "Could not get the inputMET container.");
  if(!m_inputElectrons.empty())
    RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(in_electrons, m_inputElectrons,   m_event, m_store, m_debug), "Could not get the inputElectrons container.");
  if(!m_inputMuons.empty())
    RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(in_muons,     m_inputMuons,       m_event, m_store, m_debug), "Could not get the inputMuons container.");
  if(!m_inputTauJets.empty())
    RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(in_taus,      m_inputTauJets,     m_event, m_store, m_debug), "Could not get the inputTauJets container.");
  if(!m_inputPhotons.empty())
    RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(in_photons,   m_inputPhotons,     m_event, m_store, m_debug), "Could not get the inputPhotons container.");

  // clear the event
  LAB.ClearEvent();

  // retrieve CalibMET_RefFinal for METContainer
  xAOD::MissingETContainer::const_iterator met_final = in_missinget->find("Final");
  if (met_final == in_missinget->end()) {
    Error("execute()", "No RefFinal inside MET container" );
    return EL::StatusCode::FAILURE;
  }

  // create a vector to hold the group element ids for when adding jets
  std::map<const RF::GroupElementID, const xAOD::Jet*> in_jets_IDs;
  for(const auto jet: *in_jets)
    in_jets_IDs[VIS.AddLabFrameFourVector( jet->p4() )] = jet;

  // no mpz, but why set it this way???
  INV.SetLabFrameThreeVector(  TVector3( (*met_final)->mpx(), (*met_final)->mpy(), 0 ) );

  // dump information about the jets and met at least
  if(m_debug){
    Info("execute()", "Details about input jets...");
    for(const auto jet: *in_jets)
        Info("execute()", "\tpT: %0.2f GeV\tm: %0.2f GeV\teta: %0.2f\tphi: %0.2f", jet->pt()/1000., jet->m()/1000., jet->eta(), jet->phi());
    Info("execute()", "Details about MET...");
    Info("execute()", "\tpx: %0.2f GeV\tpy: %0.2f GeV\tpz: %0.2f GeV", (*met_final)->mpx()/1000., (*met_final)->mpy()/1000., 0.0/1000.);
  }

  // analyze the event
  if(LAB.AnalyzeEvent()){ if(m_debug) Info("execute()", "Analyzed the event successfully."); }
  else                  { Error("execute()", "Run #%u, Event #%llu: Analyzed the event unsuccessfully.", eventInfo->runNumber(), eventInfo->eventNumber()); return EL::StatusCode::SUCCESS; }

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

  /* TODO
     QCD rejection stuff
  */
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ClassifyEvent :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: histFinalize () { return EL::StatusCode::SUCCESS; }
