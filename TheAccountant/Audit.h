#ifndef TheAccountant_Audit_H
#define TheAccountant_Audit_H

#include <EventLoop/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

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

// c++ includes
#include <string>

namespace RF = RestFrames;

class Audit : public EL::Algorithm
{

public:
  bool m_minMassJigsaw      = true,
       m_rapidityJigsaw     = true,
       m_contraBoostJigsaw  = true,
       m_hemiJigsaw         = true,
       m_drawDecayTreePlots = false,
       m_debug              = false;

  bool m_passPreSel         = true;

  std::string m_eventInfo       = "EventInfo",
              m_inputJets       = "AntiKt10LCTopoJets",
              m_inputBJets      = "",
              m_inputMET        = "MET_RefFinal",
              m_inputElectrons  = "",
              m_inputMuons      = "",
              m_inputTauJets    = "",
              m_inputPhotons    = "";

private:
  xAOD::TEvent *m_event; //!
  xAOD::TStore *m_store; //!

  RF::RLabFrame         LAB; //!
  RF::RDecayFrame       SS;  //!
  RF::RDecayFrame       S1;  //!
  RF::RDecayFrame       S2;  //!
  RF::RVisibleFrame     V1;  //!
  RF::RVisibleFrame     V2;  //!
  RF::RInvisibleFrame   I1;  //!
  RF::RInvisibleFrame   I2;  //!

  RF::CombinatoricGroup VIS;    //!
  RF::InvisibleGroup    INV;    //!

  RF::InvisibleMassJigsaw MinMassJigsaw; //!
  RF::InvisibleRapidityJigsaw RapidityJigsaw; //!
  RF::ContraBoostInvariantJigsaw ContraBoostJigsaw; //!
  RF::MinimizeMassesCombinatoricJigsaw HemiJigsaw; //!

public:
  // this is a standard constructor
  Audit ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(Audit, 1);
};

#endif
