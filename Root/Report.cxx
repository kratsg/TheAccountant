#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TheAccountant/Report.h>

// Infrastructure includes
#include "xAODRootAccess/Init.h"

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

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"

namespace HF = HelperFunctions;

// this is needed to distribute the algorithm to the workers
ClassImp(Report)

Report :: Report () {}

EL::StatusCode Report :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("Report").ignore();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: histInitialize () {
  // initialize all histograms here

  m_jetPlots["all_jets"]        = new TheAccountant::JetHists( "all_jets/" );
  m_jetPlots["all_bjets"]       = new TheAccountant::JetHists( "all_bjets/" );

  if(m_passPreSel){
    m_jetPlots["presel_jets"]   = new TheAccountant::JetHists( "presel_jets/" );
    m_jetPlots["presel_bjets"]  = new TheAccountant::JetHists( "presel_bjets/" );
  }

  // tagged jets
  //all_bjets_bTag
  //presel_bjets_bTag
  if(!m_decor_jetTags_b.empty()){
    m_jetPlots["all_bjets_bTag"] = new TheAccountant::JetHists("all_bjets_bTag/");
    if(m_passPreSel) m_jetPlots["presel_bjets_bTag"] = new TheAccountant::JetHists("presel_bjets_bTag/");
  }

  //all_jets_topTag
  //presel_jets_topTag
  if(!m_decor_jetTags_top.empty()){
    m_jetPlots["all_jets_topTag"] = new TheAccountant::JetHists("all_jets_topTag/");
    if(m_passPreSel) m_jetPlots["presel_jets_topTag"] = new TheAccountant::JetHists("presel_jets_topTag/");
  }

  //all_jets_wTag
  //presel_jets_wTag
  if(!m_decor_jetTags_w.empty()){
    m_jetPlots["all_jets_wTag"] = new TheAccountant::JetHists("all_jets_wTag/");
    if(m_passPreSel) m_jetPlots["presel_jets_wTag"] = new TheAccountant::JetHists("presel_jets_wTag/");
  }

  // enable jet counting for everything above
  for(auto jetPlot: m_jetPlots) jetPlot.second->m_countJets = true;

  // NLeadingJets
  for(int i=1; i <= m_numLeadingJets; ++i){
    //all_jetX
    //all bjetX
    m_jetPlots["all_jet"+std::to_string(i)] = new TheAccountant::JetHists( "all_jet"+std::to_string(i)+"/" );
    m_jetPlots["all_bjet"+std::to_string(i)] = new TheAccountant::JetHists( "all_bjet"+std::to_string(i)+"/" );

    //presel_jetX
    //presel_bjetX
    if(m_passPreSel){
      m_jetPlots["presel_jet"+std::to_string(i)] = new TheAccountant::JetHists( "presel_jet"+std::to_string(i)+"/" );
      m_jetPlots["presel_bjet"+std::to_string(i)] = new TheAccountant::JetHists( "presel_bjet"+std::to_string(i)+"/" );
    }

    // tagged jets
    //all_bjetX_bTag
    //presel_bjetX_bTag
    if(!m_decor_jetTags_b.empty()){
      m_jetPlots["all_bjet"+std::to_string(i)+"_bTag"] = new TheAccountant::JetHists("all_bjet"+std::to_string(i)+"_bTag/");
      if(m_passPreSel) m_jetPlots["presel_bjet"+std::to_string(i)+"_bTag"] = new TheAccountant::JetHists("presel_bjet"+std::to_string(i)+"_bTag/");
    }

    //all_jetX_topTag
    //presel_jetX_topTag
    if(!m_decor_jetTags_top.empty()){
      m_jetPlots["all_jet"+std::to_string(i)+"_topTag"] = new TheAccountant::JetHists("all_jet"+std::to_string(i)+"_topTag/");
      if(m_passPreSel) m_jetPlots["presel_jet"+std::to_string(i)+"_topTag"] = new TheAccountant::JetHists("presel_jet"+std::to_string(i)+"_topTag/");
    }

    //all_jetX_wTag
    //presel_jetX_wTag
    if(!m_decor_jetTags_w.empty()){
      m_jetPlots["all_jet"+std::to_string(i)+"_wTag"] = new TheAccountant::JetHists("all_jet"+std::to_string(i)+"_wTag/");
      if(m_passPreSel) m_jetPlots["presel_jet"+std::to_string(i)+"_wTag"] = new TheAccountant::JetHists("presel_jet"+std::to_string(i)+"_wTag/");
    }
  }

  for(auto jetPlot: m_jetPlots){
    jetPlot.second->initialize();
    jetPlot.second->record( wk() );
  }

  return EL::StatusCode::SUCCESS;
}
EL::StatusCode Report :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Report :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode Report :: initialize ()
{
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: execute ()
{
  const xAOD::EventInfo*                eventInfo   (nullptr);
  const xAOD::JetContainer*             in_jets     (nullptr);
  const xAOD::JetContainer*             in_bjets    (nullptr);
  const xAOD::MissingETContainer*       in_missinget(nullptr);
  const xAOD::ElectronContainer*        in_electrons(nullptr);
  const xAOD::MuonContainer*            in_muons    (nullptr);
  const xAOD::TauJetContainer*          in_taus     (nullptr);
  const xAOD::PhotonContainer*          in_photons  (nullptr);
  //const xAOD::TruthParticleContainer*   in_truth    (nullptr);

  // start grabbing all the containers that we can
  RETURN_CHECK("Preselect::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  RETURN_CHECK("Preselect::execute()", HF::retrieve(in_jets,      m_inputJets,        m_event, m_store, m_debug), "Could not get the inputJets container.");
  RETURN_CHECK("Preselect::execute()", HF::retrieve(in_bjets,     m_inputBJets,       m_event, m_store, m_debug), "Could not get the inputBJets container.");
  if(!m_inputMET.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_missinget, m_inputMET,         m_event, m_store, m_debug), "Could not get the inputMET container.");
  if(!m_inputElectrons.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_electrons, m_inputElectrons,   m_event, m_store, m_debug), "Could not get the inputElectrons container.");
  if(!m_inputMuons.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_muons,     m_inputMuons,       m_event, m_store, m_debug), "Could not get the inputMuons container.");
  if(!m_inputTauJets.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_taus,      m_inputTauJets,     m_event, m_store, m_debug), "Could not get the inputTauJets container.");
  if(!m_inputPhotons.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_photons,   m_inputPhotons,     m_event, m_store, m_debug), "Could not get the inputPhotons container.");

  // retrieve CalibMET_RefFinal for METContainer
  xAOD::MissingETContainer::const_iterator met_final = in_missinget->find("Final");
  if (met_final == in_missinget->end()) {
    Error("execute()", "No RefFinal inside MET container" );
    return EL::StatusCode::FAILURE;
  }

  static SG::AuxElement::Accessor< int > pass_preSel("pass_preSel");
  static SG::AuxElement::Accessor< float > decor_eventWeight("eventWeight");

  float eventWeight(1);
  if( decor_eventWeight.isAvailable(*eventInfo) ) eventWeight = decor_eventWeight(*eventInfo);
  if( m_passPreSel && !pass_preSel.isAvailable(*eventInfo) ){
    Error("Preselect::execute()", "m_passPreSel is enabled but could not find the decoration on the EventInfo object.");
    return EL::StatusCode::FAILURE;
  }

  // standard all jets and all bjets
  if(m_jetPlots["all_jets"]->execute(in_jets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
  if(m_jetPlots["all_bjets"]->execute(in_bjets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;

  // standard preselected jets and preselected bjets
  if(m_passPreSel && pass_preSel(*eventInfo) == 1){
    if(m_jetPlots["presel_jets"]->execute(in_jets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    if(m_jetPlots["presel_bjets"]->execute(in_bjets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
  }

  //build up the tagged containers
  ConstDataVector<xAOD::JetContainer> bjets_bTagged(SG::VIEW_ELEMENTS);
  ConstDataVector<xAOD::JetContainer> jets_topTagged(SG::VIEW_ELEMENTS);
  ConstDataVector<xAOD::JetContainer> jets_wTagged(SG::VIEW_ELEMENTS);

  if(!m_decor_jetTags_b.empty() || !m_decor_jetTags_top.empty() || !m_decor_jetTags_w.empty()){
    static SG::AuxElement::Accessor< int > decor_jetTags_b(m_decor_jetTags_b);
    static SG::AuxElement::Accessor< int > decor_jetTags_top(m_decor_jetTags_top);
    static SG::AuxElement::Accessor< int > decor_jetTags_w(m_decor_jetTags_w);

    for(const auto jet: *in_bjets){
      if(!m_decor_jetTags_b.empty()){
        if(decor_jetTags_b.isAvailable(*jet)){
          if(decor_jetTags_b(*jet) == 1) bjets_bTagged.push_back(jet);
        } else {
          Error("Preselect::execute()", "m_decor_jetTags_b is set but the decoration is missing on this bjet.");
          return EL::StatusCode::FAILURE;
        }
      }
    }

    for(const auto jet: *in_jets){
      if(!m_decor_jetTags_top.empty()){
        if(decor_jetTags_top.isAvailable(*jet)){
          if(decor_jetTags_top(*jet) == 1) jets_topTagged.push_back(jet);
        } else {
          Error("Preselect::execute()", "m_decor_jetTags_top is set but the decoration is missing on this jet.");
          return EL::StatusCode::FAILURE;
        }
      }

      if(!m_decor_jetTags_w.empty()){
        if(decor_jetTags_w.isAvailable(*jet)){
          if(decor_jetTags_w(*jet) == 1) jets_wTagged.push_back(jet);
        } else {
          Error("Preselect::execute()", "m_decor_jetTags_w is set but the decoration is missing on this jet.");
          return EL::StatusCode::FAILURE;
        }
      }
    }

    //all_bjets_bTag
    //presel_bjets_bTag
    if(!m_decor_jetTags_b.empty()){
      m_jetPlots["all_bjets_bTag"]->execute(bjets_bTagged.asDataVector(), eventWeight);
      if(m_passPreSel && pass_preSel(*eventInfo) == 1) m_jetPlots["presel_bjets_bTag"]->execute(bjets_bTagged.asDataVector(), eventWeight);
    }

    //all_jets_topTag
    //presel_jets_topTag
    if(!m_decor_jetTags_top.empty()){
      m_jetPlots["all_jets_topTag"]->execute(jets_topTagged.asDataVector(), eventWeight);
      if(m_passPreSel && pass_preSel(*eventInfo) == 1) m_jetPlots["presel_jets_topTag"]->execute(jets_topTagged.asDataVector(), eventWeight);
    }

    //all_jets_wTag
    //presel_jets_wTag
    if(!m_decor_jetTags_w.empty()){
      m_jetPlots["all_jets_wTag"]->execute(jets_wTagged.asDataVector(), eventWeight);
      if(m_passPreSel && pass_preSel(*eventInfo) == 1) m_jetPlots["presel_jets_wTag"]->execute(jets_wTagged.asDataVector(), eventWeight);
    }
  }

  //all_jetX
  //presel_jetX
  for(int i=1; i <= std::min<int>( m_numLeadingJets, in_jets->size() ); ++i ){
    if(m_jetPlots["all_jet"+std::to_string(i)]->execute(in_jets->at(i), eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    if(m_passPreSel && pass_preSel(*eventInfo) == 1){
      if(m_jetPlots["presel_jet"+std::to_string(i)]->execute(in_jets->at(i), eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    }
  }

  //all_bjetX
  //presel_bjetX
  for(int i=1; i <= std::min<int>( m_numLeadingJets, in_bjets->size() ); ++i ){
    if(m_jetPlots["all_bjet"+std::to_string(i)]->execute(in_jets->at(i), eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    if(m_passPreSel && pass_preSel(*eventInfo) == 1){
      if(m_jetPlots["presel_bjet"+std::to_string(i)]->execute(in_jets->at(i), eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    }
  }

  //all_bjetX_bTag
  //presel_bjetX_bTag
  if(!m_decor_jetTags_b.empty()){
    for(int i=1; i <= std::min<int>( m_numLeadingJets, bjets_bTagged.size() ); ++i){
      m_jetPlots["all_bjet"+std::to_string(i)+"_bTag"]->execute(bjets_bTagged.at(i), eventWeight);
      if(m_passPreSel && pass_preSel(*eventInfo) == 1){
        m_jetPlots["presel_bjet"+std::to_string(i)+"_bTag"]->execute(bjets_bTagged.at(i), eventWeight);
      }
    }
  }

  //all_jetX_topTag
  //presel_jetX_topTag
  if(!m_decor_jetTags_top.empty()){
    for(int i=1; i <= std::min<int>( m_numLeadingJets, jets_topTagged.size() ); ++i){
      m_jetPlots["all_jet"+std::to_string(i)+"_topTag"]->execute(jets_topTagged.at(i), eventWeight);
      if(m_passPreSel && pass_preSel(*eventInfo) == 1){
        m_jetPlots["presel_jet"+std::to_string(i)+"_topTag"]->execute(jets_topTagged.at(i), eventWeight);
      }
    }
  }

  //all_jetX_wTag
  //presel_jetX_wTag
  if(!m_decor_jetTags_w.empty()){
    for(int i=1; i <= std::min<int>( m_numLeadingJets, jets_wTagged.size() ); ++i){
      m_jetPlots["all_jet"+std::to_string(i)+"_wTag"]->execute(jets_wTagged.at(i), eventWeight);
      if(m_passPreSel && pass_preSel(*eventInfo) == 1){
        m_jetPlots["presel_jet"+std::to_string(i)+"_wTag"]->execute(jets_wTagged.at(i), eventWeight);
      }
    }
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode Report :: finalize () {
  for( auto jetPlot : m_jetPlots ) {
    if(jetPlot.second) delete jetPlot.second;
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: histFinalize () { return EL::StatusCode::SUCCESS; }
