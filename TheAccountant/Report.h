#ifndef TheAccountant_Report_H
#define TheAccountant_Report_H

#include <EventLoop/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODEventInfo/EventInfo.h"

// plots to include
#include <TheAccountant/IParticleKinematicHists.h>
#include <TheAccountant/JetHists.h>
#include <TheAccountant/JetMETHists.h>
#include <TheAccountant/METHists.h>
#include <TheAccountant/JetTagHists.h>

#include <TheAccountant/RazorVariableHists.h>
class Report : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  bool m_passPreSel = false;
  int m_numLeadingJets = 0;

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
  float m_jet_minPtView,
        m_jetLargeR_minPtView;

private:
  xAOD::TEvent *m_event;  //!
  xAOD::TStore *m_store;  //!

  std::map< std::string, TheAccountant::IParticleKinematicHists* > m_jetKinematicPlots; //!
  std::map< std::string, TheAccountant::JetHists* > m_jetPlots; //!
  std::map< std::string, TheAccountant::JetMETHists* > m_jetMETPlots; //!
  std::map< std::string, TheAccountant::JetTagHists* > m_jetTagPlots; //!
  std::map< std::string, TheAccountant::METHists* > m_METPlots; //!
  std::map< std::string, TheAccountant::RazorVariableHists* > m_RazorPlots; //!

  std::vector<std::string> m_topTagDecorationNames; //!

public:
  // this is a standard constructor
  Report ();
  Report (std::string name, std::string configName);

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
  ClassDef(Report, 1);
};

#endif
