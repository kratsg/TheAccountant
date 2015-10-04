#ifndef TheAccountant_Preselect_H
#define TheAccountant_Preselect_H

#include <TheAccountant/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// trigger
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

class Preselect : public TA::Algorithm
{
public:
  int m_jetLargeR_minNum      = 0,
      m_jetLargeR_maxNum      = 100,
      m_toptag_minNum 	      = 0,
      m_toptag_maxNum 	      = 100;
  float m_jetLargeR_minPt     = 0.0,
        m_jetLargeR_maxPt     = 1.e6,
        m_jetLargeR_minMass   = 0.0,
        m_jetLargeR_maxMass   = 1.e6,
        m_jetLargeR_minEta    = -10.0,
        m_jetLargeR_maxEta    = 10.0,
        m_jetLargeR_minPhi    = -4.0,
        m_jetLargeR_maxPhi    = 4.0;
  std::string m_topTag_wp  = "VeryLoose";

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
  std::string m_baselineLeptonSelection = "",
              m_signalLeptonSelection = "";
  bool m_badJetVeto = false;
  std::string m_truthMETFilter = "";

  std::string m_triggerSelection = ""; // empty is none

private:
  TrigConf::xAODConfigTool* m_trigConf; //!
  Trig::TrigDecisionTool* m_TDT; //!

  // a map holding (cutflow, (unweighted, weighted))
  std::map<std::string, std::pair<float, float>> m_cutflow; //!

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
