#ifndef TheAccountant_Audit_H
#define TheAccountant_Audit_H

#include <TheAccountant/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// RestFrames includes
#include "RestFrames/RestFrames.hh"

// c++ includes
#include <string>

namespace RF = RestFrames;

class Audit : public TA::Algorithm
{

public:
  bool m_minMassJigsaw      = true,
       m_rapidityJigsaw     = true,
       m_contraBoostJigsaw  = true,
       m_hemiJigsaw         = true,
       m_drawDecayTreePlots = false;
  bool m_passPreSel         = true;

private:
  RF::LabRecoFrame         LAB; //!
  RF::DecayRecoFrame       SS;  //!
  RF::DecayRecoFrame       S1;  //!
  RF::DecayRecoFrame       S2;  //!
  RF::VisibleRecoFrame     V1;  //!
  RF::VisibleRecoFrame     V2;  //!
  RF::InvisibleRecoFrame   I1;  //!
  RF::InvisibleRecoFrame   I2;  //!

  RF::CombinatoricGroup VIS;    //!
  RF::InvisibleGroup    INV;    //!

  RF::SetMassInvJigsaw MinMassJigsaw; //!
  RF::SetRapidityInvJigsaw RapidityJigsaw; //!
  RF::ContraBoostInvJigsaw ContraBoostJigsaw; //!
  RF::MinMassesCombJigsaw HemiJigsaw; //!

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
