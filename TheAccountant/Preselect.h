#ifndef TheAccountant_Preselect_H
#define TheAccountant_Preselect_H

#include <TheAccountant/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// trigger
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// for limits
#include <limits>

// reclustering forward declaration
class JetReclusteringTool;

class Preselect : public TA::Algorithm
{
public:
  // small-R jets
  int   m_jet_minNum    = 0,
        m_jet_maxNum    = 100;
  float m_jet_minPt     = -std::numeric_limits<float>::max(),
        m_jet_maxPt     = std::numeric_limits<float>::max(),
        m_jet_minEta    = -std::numeric_limits<float>::max(),
        m_jet_maxEta    = std::numeric_limits<float>::max(),
        m_jet_minPhi    = -std::numeric_limits<float>::max(),
        m_jet_maxPhi    = std::numeric_limits<float>::max();
  // small-R b-jets
  std::string m_bTag_wp  = "Loose";
  int   m_bjet_minNum    = 0,
        m_bjet_maxNum    = 100;
  float m_bjet_minPt     = -std::numeric_limits<float>::max(),
        m_bjet_maxPt     = std::numeric_limits<float>::max(),
        m_bjet_minEta    = -std::numeric_limits<float>::max(),
        m_bjet_maxEta    = std::numeric_limits<float>::max(),
        m_bjet_minPhi    = -std::numeric_limits<float>::max(),
        m_bjet_maxPhi    = std::numeric_limits<float>::max();
  // large-R jets
  int   m_jetLargeR_minNum    = 0,
        m_jetLargeR_maxNum    = 100;
  float m_jetLargeR_minPt     = -std::numeric_limits<float>::max(),
        m_jetLargeR_maxPt     = std::numeric_limits<float>::max(),
        m_jetLargeR_minMass   = -std::numeric_limits<float>::max(),
        m_jetLargeR_maxMass   = std::numeric_limits<float>::max(),
        m_jetLargeR_minEta    = -std::numeric_limits<float>::max(),
        m_jetLargeR_maxEta    = std::numeric_limits<float>::max(),
        m_jetLargeR_minPhi    = -std::numeric_limits<float>::max(),
        m_jetLargeR_maxPhi    = std::numeric_limits<float>::max();
  // top-tagged large-R jets
  std::string m_topTag_wp  = "VeryLoose";
  int   m_topTag_minNum    = 0,
        m_topTag_maxNum    = 100;
  float m_topTag_minPt     = -std::numeric_limits<float>::max(),
        m_topTag_maxPt     = std::numeric_limits<float>::max(),
        m_topTag_minMass   = -std::numeric_limits<float>::max(),
        m_topTag_maxMass   = std::numeric_limits<float>::max(),
        m_topTag_minEta    = -std::numeric_limits<float>::max(),
        m_topTag_maxEta    = std::numeric_limits<float>::max(),
        m_topTag_minPhi    = -std::numeric_limits<float>::max(),
        m_topTag_maxPhi    = std::numeric_limits<float>::max();
  // reclustering jet definitions -- used only if m_rc_enable is true
  float m_rc_radius = 1.0,
        m_rc_inputPt = 50.0,
        m_rc_trimPtFrac = 0.05;

  // various other preselections of more complicated objects
  float m_dPhiMin      = 0.0;
  float m_minMET       = 0.0;
  std::string m_baselineLeptonSelection = "",
              m_signalLeptonSelection = "";
  bool m_badJetVeto = false;
  std::string m_truthMETFilter = "";
  std::string m_triggerSelection = ""; // empty is none

private:
  TrigConf::xAODConfigTool* m_trigConf = nullptr; //!
  Trig::TrigDecisionTool* m_TDT = nullptr; //!
  JetReclusteringTool* m_reclusteringTool = nullptr; //!

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
