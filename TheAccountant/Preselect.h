#ifndef TheAccountant_Preselect_H
#define TheAccountant_Preselect_H

#include <EventLoop/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

class Preselect : public EL::Algorithm
{
public:
  bool m_debug              = false;

  std::string m_eventInfo       = "EventInfo",
              m_inputJets       = "AntiKt10LCTopoJets",
              m_inputBJets      = "",
              m_inputMET        = "MET_RefFinal",
              m_inputElectrons  = "",
              m_inputMuons      = "",
              m_inputTauJets    = "",
              m_inputPhotons    = "";

  int m_jet_minNum      = 0,
      m_jet_maxNum      = 100;
  float m_jet_minPt     = 0.0,
        m_jet_maxPt     = 1.e6,
        m_jet_minMass   = 0.0,
        m_jet_maxMass   = 1.e6,
        m_jet_minEta    = -10.0,
        m_jet_maxEta    = 10.0,
        m_jet_minPhi    = -4.0,
        m_jet_maxPhi    = 4.0;

  int m_bjet_minNum      = 0,
      m_bjet_maxNum      = 100;
  float m_bjet_minPt    = 0.0,
        m_bjet_maxPt    = 1.e6,
        m_bjet_minMass  = 0.0,
        m_bjet_maxMass  = 1.e6,
        m_bjet_minEta   = -10.0,
        m_bjet_maxEta   = 10.0,
        m_bjet_minPhi   = -4.0,
        m_bjet_maxPhi   = 4.0,
        m_bjet_MV1      = 0.0;

private:
  xAOD::TEvent *m_event; //!
  xAOD::TStore *m_store; //!

public:
  // this is a standard constructor
  Preselect ();

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
  ClassDef(Preselect, 1);
};

#endif
