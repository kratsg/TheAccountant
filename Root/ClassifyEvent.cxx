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



// this is needed to distribute the algorithm to the workers
ClassImp(ClassifyEvent)



ClassifyEvent :: ClassifyEvent ()
{
}



EL::StatusCode ClassifyEvent :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("ClassifyEvent").ignore();
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ClassifyEvent :: histInitialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: changeInput (bool firstFile) { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: initialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: execute ()
{
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode ClassifyEvent :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ClassifyEvent :: histFinalize () { return EL::StatusCode::SUCCESS; }
