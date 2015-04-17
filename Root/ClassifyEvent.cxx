#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TheAccountant/ClassifyEvent.h>

// RestFrames includes
#include "RestFrames/RestFrame.hh"
#include "RestFrames/RFrame.hh"
#include "RestFrames/RLabFrame.hh"
#include "RestFrames/RDecayFrame.hh"
#include "RestFrames/RVisibleFrame.hh"
#include "RestFrames/RInvisibleFrame.hh"
#include "RestFrames/RSelfAssemblingFrame.hh"
#include "RestFrames/InvisibleMassJigsaw.hh"
#include "RestFrames/InvisibleRapidityJigsaw.hh"
#include "RestFrames/ContraBoostInvariantJigsaw.hh"
#include "RestFrames/MinimizeMassesCombinatoricJigsaw.hh"
#include "RestFrames/InvisibleGroup.hh"
#include "RestFrames/CombinatoricGroup.hh"
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

namespace HF = HelperFunctions;
namespace RF = RestFrames;

// this is needed to distribute the algorithm to the workers
ClassImp(ClassifyEvent)
ClassifyEvent :: ClassifyEvent () {}

EL::StatusCode ClassifyEvent :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("ClassifyEvent").ignore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ClassifyEvent :: histInitialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: changeInput (bool firstFile) { return EL::StatusCode::SUCCESS; }

EL::StatusCode ClassifyEvent :: initialize () {
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ClassifyEvent :: execute ()
{
  const xAOD::EventInfo*                eventInfo   (nullptr);
  const xAOD::JetContainer*             in_jets     (nullptr);
  const xAOD::ElectronContainer*        in_electrons(nullptr);
  const xAOD::MuonContainer*            in_muons    (nullptr);
  const xAOD::TauJetContainer*          in_taus     (nullptr);
  const xAOD::PhotonContainer*          in_photons  (nullptr);
  const xAOD::MissingETContainer*       in_missinget(nullptr);
  const xAOD::TruthParticleContainer*   in_truth    (nullptr);

  RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(in_jets, "AntiKt10LCTopoJets", m_event, m_store, true), "");
  RETURN_CHECK("ClassifyEvent::execute()", HF::retrieve(in_missinget, "MET_RefFinal", m_event, m_store, true), "");
  // retrieve CalibMET_RefFinal for METContainer
  if (in_missinget->find("Final") == in_missinget->end()) {
    Error("execute()", "No RefFinal inside MET container" );
    return EL::StatusCode::FAILURE;
  }

  // build up the RestFrames code, indented for visualization
  RF::RLabFrame LAB("LAB","lab");
    RF::RDecayFrame SS("SS","SS"); // di-sparticle
    LAB.SetChildFrame(SS);
      RF::RDecayFrame S1("S1","#tilde{S}_{a}"); // sparticle 1
      SS.AddChildFrame(S1);
        RF::RVisibleFrame V1("V1","V_{a}");
        S1.AddChildFrame(V1);
        RF::RInvisibleFrame I1("I1","I_{a}");
        S1.AddChildFrame(I1);
      RF::RDecayFrame S2("S2","#tilde{S}_{b}"); // sparticle 2
      SS.AddChildFrame(S2);
        RF::RVisibleFrame V2("V2","V_{b}");
        S2.AddChildFrame(V2);
        RF::RInvisibleFrame I2("I2","I_{b}");
        S2.AddChildFrame(I2);

  // The invisible group is all of the weakly interacting particles
  RF::InvisibleGroup INV("INV","Invisible Frame Jigsaws");
  INV.AddFrame(I1);
  INV.AddFrame(I2);

  // the combinatoric group is the list of visible objects
  // that go into our hemispheres
  RF::CombinatoricGroup VIS("VIS","Visible Object Jigsaws");
  VIS.AddFrame(V1);
  VIS.AddFrame(V2);

  // require at least one element in each hemisphere
  VIS.SetNElementsForFrame(V1,1,false);
  VIS.SetNElementsForFrame(V2,1,false);

  bool topologyOk = LAB.InitializeTree();
  if(topologyOk){ Info("execute()", "We do have consistent tree topology."); }
  else          { Warning("execute()", "We do not have consistent tree topology."); return EL::StatusCode::FAILURE; }


  // now we define 'jigsaw rules' that tell the tree how to define the objects
  // in our groups, indented to cleanly see the Jigsaws being defined
  RF::InvisibleMassJigsaw MinMassJigsaw("MINMASS_JIGSAW", "Invisible system mass Jigsaw");
    INV.AddJigsaw(MinMassJigsaw);

  RF::InvisibleRapidityJigsaw RapidityJigsaw("RAPIDITY_JIGSAW", "Invisible system rapidity Jigsaw");
    INV.AddJigsaw(RapidityJigsaw);
    RapidityJigsaw.AddVisibleFrame((LAB.GetListVisibleFrames()));

  RF::ContraBoostInvariantJigsaw ContraBoostJigsaw("CB_JIGSAW","Contraboost invariant Jigsaw");
    INV.AddJigsaw(ContraBoostJigsaw);
    ContraBoostJigsaw.AddVisibleFrame((S1.GetListVisibleFrames()), 0);
    ContraBoostJigsaw.AddVisibleFrame((S2.GetListVisibleFrames()), 1);
    ContraBoostJigsaw.AddInvisibleFrame((S1.GetListInvisibleFrames()), 0);
    ContraBoostJigsaw.AddInvisibleFrame((S2.GetListInvisibleFrames()), 1);

  RF::MinimizeMassesCombinatoricJigsaw HemiJigsaw("HEM_JIGSAW","Minimize m _{V_{a,b}} Jigsaw");
    VIS.AddJigsaw(HemiJigsaw);
    HemiJigsaw.AddFrame(V1,0);
    HemiJigsaw.AddFrame(V2,1);

  bool analysisOk = LAB.InitializeAnalysis();
  if(analysisOk){ Info("execute()", "Our tree is ok for analysis."); }
  else          { Warning("execute()", "Our tree is not ok for analysis."); return EL::StatusCode::FAILURE; }


  // only output this thing once, hence the static
  static bool saved;
  if(!saved){
    saved = true;
    RF::FramePlot* decayTree_plot = new RF::FramePlot("tree", "Decay Tree");
    // start with the lab frame
    decayTree_plot->AddFrameTree(LAB);
    // add the jigsaws
    decayTree_plot->AddJigsaw(MinMassJigsaw);
    decayTree_plot->AddJigsaw(RapidityJigsaw);
    decayTree_plot->AddJigsaw(ContraBoostJigsaw);
    decayTree_plot->AddJigsaw(HemiJigsaw);
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

  // clear the event
  LAB.ClearEvent();

  // create a vector to hold the group element ids for when adding jets
  std::vector<RF::GroupElementID> in_jets_IDs;
  for(const auto jet: *in_jets){
    RF::GroupElementID jet_id = VIS.AddLabFrameFourVector( jet->p4() );
    in_jets_IDs.push_back(jet_id);
  }

  xAOD::MissingETContainer::const_iterator met_it = in_missinget->find("Final");
  // no mpz, but why set it this way???
  INV.SetLabFrameThreeVector(  TVector3( (*met_it)->mpx(), (*met_it)->mpy(), 0 ) );

  bool analyzedOk = LAB.AnalyzeEvent();
  if(analyzedOk){ Info("execute()", "Analyzed the event successfully."); }
  else          { Warning("execute()", "Analyzed the event unsuccessfully."); return EL::StatusCode::FAILURE; }

  Info("execute()", "Details about input jets...");
  for(const auto jet: *in_jets){
    Info("execute()", "\tpT: %0.2f GeV\tm: %0.2f GeV\teta: %0.2f\tphi: %0.2f", jet->pt()/1000., jet->m()/1000., jet->eta(), jet->phi());
  }

  Info("execute()", "Details about MET...");
  Info("execute()", "\tmpx: %0.2f GeV\tmpy: %0.2f GeV\tmpz: %0.2f GeV", (*met_it)->mpx()/1000., (*met_it)->mpy()/1000., 0.0/1000.);

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

  /* TODO
     QCD rejection stuff
  */
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ClassifyEvent :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: histFinalize () { return EL::StatusCode::SUCCESS; }
