#ifndef TheAccountant_Preselect_H
#define TheAccountant_Preselect_H

#include <EventLoop/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

class Preselect : public EL::Algorithm
{
public:
  int m_jetLargeR_minNum      = 0,
      m_jetLargeR_maxNum      = 100;
  float m_jetLargeR_minPt     = 0.0,
        m_jetLargeR_maxPt     = 1.e6,
        m_jetLargeR_minMass   = 0.0,
        m_jetLargeR_maxMass   = 1.e6,
        m_jetLargeR_minEta    = -10.0,
        m_jetLargeR_maxEta    = 10.0,
        m_jetLargeR_minPhi    = -4.0,
        m_jetLargeR_maxPhi    = 4.0;

  int m_jet_minNum      = 0,
      m_jet_maxNum      = 100,
      m_bjet_minNum     = 0,
      m_bjet_maxNum     = 100;
  float m_jet_minPt    = 0.0,
        m_jet_maxPt    = 1.e6,
        m_jet_minMass  = 0.0,
        m_jet_maxMass  = 1.e6,
        m_jet_minEta   = -10.0,
        m_jet_maxEta   = 10.0,
        m_jet_minPhi   = -4.0,
        m_jet_maxPhi   = 4.0;
  std::string m_bTag_wp  = "Loose";
  float m_dPhiMin      = 0.4;
  float m_minMET       = 0.0;
  int m_numLeptons     = 0; // default is 0L channel

  // standard across all algorithms for configuration
  bool m_debug = false;
  std::string m_eventInfo       = "EventInfo",
              m_inputLargeRJets       = "",
              m_inputJets      = "",
              m_inputMET        = "MET_RefFinal",
              m_inputMETName    = "Final",
              m_inputElectrons  = "",
              m_inputMuons      = "",
              m_inputTauJets    = "",
              m_inputPhotons    = "";
  std::string m_decor_jetTags_b   = "", // isB,
              m_decor_jetTags_top = "", // isTop,
              m_decor_jetTags_w   = ""; // isW;

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
