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

#include <TCanvas.h>

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

  Info("execute()", "Do we have consistent tree topology? %s", (LAB.InitializeTree() ? "yes" : "no"));

  static bool saved;
  if(!saved){
    saved = true;
    RF::FramePlot* decayTree_plot = new RF::FramePlot("tree", "Decay Tree");
    decayTree_plot->AddFrameTree(LAB);
    decayTree_plot->DrawFramePlot();
    wk()->addOutput(decayTree_plot->GetCanvas());
  }


  /* TODO
  //////////////////////////////////////////////////////////////
  // now we define 'jigsaw rules' that tell the tree
  // how to define the objects in our groups
  //////////////////////////////////////////////////////////////
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

  //////////////////////////////////////////////////////////////
  // check to make sure that all the jigsaws etc. are correctly connected
  //////////////////////////////////////////////////////////////
  std::cout << "Is consistent analysis tree? : " << LAB.InitializeAnalysis() << std::endl;

  LAB.ClearEvent();


  //////////////////////////////////////////////////////////////
  // Now, we make a 'background'-like, transverse momentum only, tree
  //////////////////////////////////////////////////////////////
  RF::RLabFrame LAB_alt("LAB","lab");
  RF::RSelfAssemblingFrame S_alt("CM","CM");
  RF::RVisibleFrame V_alt("V_alt","Vis");
  RF::RInvisibleFrame I_alt("I_alt","Iinv");

  RF::InvisibleGroup INV_alt("INV_alt","Invisible State Jigsaws");
  INV_alt.AddFrame(I_alt);

  RF::CombinatoricGroup VIS_alt("VIS_alt","Visible Object Jigsaws");
  VIS_alt.AddFrame(V_alt);
  VIS_alt.SetNElementsForFrame(V_alt,1,false);

  LAB_alt.SetChildFrame(S_alt);
  S_alt.AddChildFrame(V_alt);
  S_alt.AddChildFrame(I_alt);

  LAB_alt.InitializeTree();

  // Will just set invisible mass to zero
  RF::InvisibleMassJigsaw MinMass_alt("MINMASS_JIGSAW_ALT", "Invisible system mass Jigsaw");
  INV_alt.AddJigsaw(MinMass_alt);

  // will set rapidity to zero
  RF::InvisibleRapidityJigsaw Rapidity_alt("RAPIDITY_JIGSAW_ALT", "Invisible system rapidity Jigsaw");
  INV_alt.AddJigsaw(Rapidity_alt);
  Rapidity_alt.AddVisibleFrame((LAB_alt.GetListVisibleFrames()));

  LAB_alt.InitializeAnalysis();

  TLorentzVector jet;

  jet_itr = (jets_copy)->begin();
  for( ; jet_itr != jet_end; ++jet_itr ) {

    if( (*jet_itr)->auxdata< char >("baseline")==1  &&
      (*jet_itr)->auxdata< char >("passOR")==1  &&
      (*jet_itr)->pt() > 30000.  && ( fabs( (*jet_itr)->eta()) < 2.8) ) {
      VIS.AddLabFrameFourVector( (*jet_itr)->p4()  );

      jet.SetPtEtaPhiM( (*jet_itr)->pt(), 0., (*jet_itr)->phi(), (*jet_itr)->m()  );
      VIS_alt.AddLabFrameFourVector(jet);
    }

  }


  // Get MET Collection to hand to Rest Frames////////////////////////////////////////////////////

  xAOD::MissingETContainer* MET = new xAOD::MissingETContainer;
  CHECK( m_store->retrieve( MET, "CalibMET_RefFinal" ) );

  TVector3 MET_TV3;

    xAOD::MissingETContainer::const_iterator met_it = MET->find("Final");
  if (met_it == MET->end()) {
    Error( APP_NAME, "No RefFinal inside MET container" );
  } else {
    INV.SetLabFrameThreeVector(  TVector3( (*met_it)->mpx(), (*met_it)->mpy(), 0 ) );
    MET_TV3.SetZ(0.);
    MET_TV3.SetX((*met_it)->mpx() );
    MET_TV3.SetY((*met_it)->mpy() );
  }

  LAB.AnalyzeEvent();


  INV_alt.SetLabFrameThreeVector(MET_TV3);
  LAB_alt.AnalyzeEvent();

  //std::cout << "RestFrames shatR is: " << SS.GetMass() << std::endl;

  eventInfo->auxdecor<float>("SS_Mass"           ) = SS.GetMass();
  eventInfo->auxdecor<float>("SS_InvGamma"       ) = 1./SS.GetGammaInParentFrame();
  eventInfo->auxdecor<float>("SS_dPhiBetaR"      ) = SS.GetDeltaPhiBoostVisible();
  eventInfo->auxdecor<float>("SS_dPhiVis"        ) = SS.GetDeltaPhiVisible();
  eventInfo->auxdecor<float>("SS_CosTheta"       ) = SS.GetCosDecayAngle();
  eventInfo->auxdecor<float>("SS_dPhiDecayAngle" ) = SS.GetDeltaPhiDecayAngle();
  eventInfo->auxdecor<float>("SS_VisShape"       ) = SS.GetVisibleShape();
  eventInfo->auxdecor<float>("SS_MDeltaR"        ) = SS.GetVisibleShape() * SS.GetMass() ;
  eventInfo->auxdecor<float>("S1_Mass"           ) = S1.GetMass();
  eventInfo->auxdecor<float>("S1_CosTheta"       ) = S1.GetCosDecayAngle();
  eventInfo->auxdecor<float>("S2_Mass"           ) = S2.GetMass();
  eventInfo->auxdecor<float>("S2_CosTheta"       ) = S2.GetCosDecayAngle();
  eventInfo->auxdecor<float>("I1_Depth"          ) = S1.GetFrameDepth(I1);
  eventInfo->auxdecor<float>("I2_Depth"          ) = S2.GetFrameDepth(I2);
  eventInfo->auxdecor<float>("V1_N"              ) = VIS.GetNElementsInFrame(V1);
  eventInfo->auxdecor<float>("V2_N"              ) = VIS.GetNElementsInFrame(V2);

  // dphiR and Rptshat (formerly cosPT)
  // for QCD rejection
  double dphiR = SS.GetDeltaPhiBoostVisible();
  double PTCM = SS.GetFourVector(LAB).Pt();
  double Rptshat = PTCM / (PTCM + SS.GetMass()/4.);

  // QCD rejection using the 'background tree'
  // MET 'sibling' in background tree auxillary calculations
  TLorentzVector Psib = I_alt.GetSiblingFrame()->GetFourVector(LAB_alt);
  TLorentzVector Pmet = I_alt.GetFourVector(LAB_alt);
  double Psib_dot_METhat = max(0., Psib.Vect().Dot(MET_TV3.Unit()));
  double Mpar2 = Psib.E()*Psib.E()-Psib.Vect().Dot(MET_TV3.Unit())*Psib.Vect().Dot(MET_TV3.Unit());
  double Msib2 = Psib.M2();
  double MB2 = 2.*(Pmet.E()*Psib.E()-MET_TV3.Dot(Psib.Vect()));
  TVector3 boostPsibM = (Pmet+Psib).BoostVector();


  // QCD rejection variables from 'background tree'
  double DepthBKG = S_alt.GetFrameDepth(I_alt);
  int Nsib = I_alt.GetSiblingFrame()->GetNDescendants();
  double cosBKG = I_alt.GetParentFrame()->GetCosDecayAngle();
  double dphiMsib = fabs(MET_TV3.DeltaPhi(Psib.Vect()));
  double RpsibM = Psib_dot_METhat / (Psib_dot_METhat + MET_TV3.Mag());
  double RmsibM = 1. / ( MB2/(Mpar2-Msib2) + 1.);
  Psib.Boost(-boostPsibM);
  double cosPsibM = -1.*Psib.Vect().Unit().Dot(boostPsibM.Unit());
  cosPsibM = (1.-cosPsibM)/2.;
  double DeltaQCD1 = (cosPsibM-RpsibM)/(cosPsibM+RpsibM);
  double DeltaQCD2 = (cosPsibM-RmsibM)/(cosPsibM+RmsibM);

  eventInfo->auxdecor<float>("QCD_dPhiR")   = dphiR;
  eventInfo->auxdecor<float>("QCD_Rpt")     = Rptshat;
  eventInfo->auxdecor<float>("QCD_Rmsib")   = RmsibM;
  eventInfo->auxdecor<float>("QCD_Delta2")  = DeltaQCD2;
  eventInfo->auxdecor<float>("QCD_Rpsib")   = RpsibM;
  eventInfo->auxdecor<float>("QCD_Delta1")  = DeltaQCD1;
  */
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ClassifyEvent :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: histFinalize () { return EL::StatusCode::SUCCESS; }
