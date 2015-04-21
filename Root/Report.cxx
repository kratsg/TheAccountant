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

  m_jetPlots["all/jets"]          = new TheAccountant::JetHists( "all/jets/" );
  m_jetPlots["all/bjets"]         = new TheAccountant::JetHists( "all/bjets/" );
  m_jetMETPlots["all/jets"]       = new TheAccountant::JetMETHists( "all/jets/" );
  m_jetMETPlots["all/bjets"]      = new TheAccountant::JetMETHists( "all/bjets/" );
  m_METPlots["all"]               = new TheAccountant::METHists( "all/" );

  if(m_passPreSel){
    m_jetPlots["presel/jets"]     = new TheAccountant::JetHists( "presel/jets/" );
    m_jetPlots["presel/bjets"]    = new TheAccountant::JetHists( "presel/bjets/" );

    m_jetMETPlots["presel/jets"]  = new TheAccountant::JetMETHists( "presel/jets/" );
    m_jetMETPlots["presel/bjets"] = new TheAccountant::JetMETHists( "presel/bjets/" );

    m_METPlots["presel"]          = new TheAccountant::METHists( "presel/" );
  }

  // tagged jets
  //all/bjets/bTag
  //presel/bjets/bTag
  if(!m_decor_jetTags_b.empty()){
    m_jetPlots["all/bjets/bTag"]          = new TheAccountant::JetHists("all/bjets/bTag/");
    m_jetMETPlots["all/bjets/bTag"]       = new TheAccountant::JetMETHists("all/bjets/bTag/");

    if(m_passPreSel){
      m_jetPlots["presel/bjets/bTag"]     = new TheAccountant::JetHists("presel/bjets/bTag/");
      m_jetMETPlots["presel/bjets/bTag"]  = new TheAccountant::JetMETHists("presel/bjets/bTag/");
    }
  }

  //all/jets/topTag
  //presel/jets/topTag
  if(!m_decor_jetTags_top.empty()){
    m_jetPlots["all/jets/topTag"]         = new TheAccountant::JetHists("all/jets/topTag/");
    m_jetMETPlots["all/jets/topTag"]      = new TheAccountant::JetMETHists("all/jets/topTag/");

    if(m_passPreSel){
      m_jetPlots["presel/jets/topTag"]    = new TheAccountant::JetHists("presel/jets/topTag/");
      m_jetMETPlots["presel/jets/topTag"] = new TheAccountant::JetMETHists("presel/jets/topTag/");
    }
  }

  //all/jets/wTag
  //presel/jets/wTag
  if(!m_decor_jetTags_w.empty()){
    m_jetPlots["all/jets/wTag"]           = new TheAccountant::JetHists("all/jets/wTag/");
    m_jetMETPlots["all/jets/wTag"]        = new TheAccountant::JetMETHists("all/jets/wTag/");

    if(m_passPreSel){
      m_jetPlots["presel/jets/wTag"]      = new TheAccountant::JetHists("presel/jets/wTag/");
      m_jetMETPlots["presel/jets/wTag"]   = new TheAccountant::JetMETHists("presel/jets/wTag/");
    }
  }

  // enable jet counting for jet plots above
  for(auto jetPlot: m_jetPlots) jetPlot.second->m_countJets = true;
  // set the numLeadingJets for the JetMET histograms
  for(auto jetMETPlot: m_jetMETPlots) jetMETPlot.second->m_numLeadingJets = m_numLeadingJets;

  // NLeadingJets
  for(int i=1; i <= m_numLeadingJets; ++i){
    //all/jetX
    //all bjetX
    m_jetPlots["all/jet"+std::to_string(i)] = new TheAccountant::JetHists( "all/jet"+std::to_string(i)+"/" );
    m_jetPlots["all/bjet"+std::to_string(i)] = new TheAccountant::JetHists( "all/bjet"+std::to_string(i)+"/" );

    //presel/jetX
    //presel/bjetX
    if(m_passPreSel){
      m_jetPlots["presel/jet"+std::to_string(i)] = new TheAccountant::JetHists( "presel/jet"+std::to_string(i)+"/" );
      m_jetPlots["presel/bjet"+std::to_string(i)] = new TheAccountant::JetHists( "presel/bjet"+std::to_string(i)+"/" );
    }

    // tagged jets
    //all/bjetX_bTag
    //presel/bjetX_bTag
    if(!m_decor_jetTags_b.empty()){
      m_jetPlots["all/bjet"+std::to_string(i)+"/bTag"] = new TheAccountant::JetHists("all/bjet"+std::to_string(i)+"/bTag/");
      if(m_passPreSel) m_jetPlots["presel/bjet"+std::to_string(i)+"/bTag"] = new TheAccountant::JetHists("presel/bjet"+std::to_string(i)+"/bTag/");
    }

    //all/jetX_topTag
    //presel/jetX_topTag
    if(!m_decor_jetTags_top.empty()){
      m_jetPlots["all/jet"+std::to_string(i)+"/topTag"] = new TheAccountant::JetHists("all/jet"+std::to_string(i)+"/topTag/");
      if(m_passPreSel) m_jetPlots["presel/jet"+std::to_string(i)+"/topTag"] = new TheAccountant::JetHists("presel/jet"+std::to_string(i)+"/topTag/");
    }

    //all/jetX_wTag
    //presel/jetX_wTag
    if(!m_decor_jetTags_w.empty()){
      m_jetPlots["all/jet"+std::to_string(i)+"/wTag"] = new TheAccountant::JetHists("all/jet"+std::to_string(i)+"/wTag/");
      if(m_passPreSel) m_jetPlots["presel/jet"+std::to_string(i)+"/wTag"] = new TheAccountant::JetHists("presel/jet"+std::to_string(i)+"/wTag/");
    }
  }

  for(auto jetPlot: m_jetPlots){
    jetPlot.second->initialize();
    jetPlot.second->record( wk() );
  }

  for(auto jetMETPlot: m_jetMETPlots){
    jetMETPlot.second->initialize();
    jetMETPlot.second->record( wk() );
  }

  for(auto METPlot: m_METPlots){
    METPlot.second->initialize();
    METPlot.second->record( wk() );
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
  // dereference the iterator since it's just a single object
  const xAOD::MissingET* in_met = *met_final;

  static SG::AuxElement::Accessor< int > pass_preSel("pass_preSel");
  static SG::AuxElement::Accessor< float > decor_eventWeight("eventWeight");

  float eventWeight(1);
  if( decor_eventWeight.isAvailable(*eventInfo) ) eventWeight = decor_eventWeight(*eventInfo);
  if( m_passPreSel && !pass_preSel.isAvailable(*eventInfo) ){
    Error("Preselect::execute()", "m_passPreSel is enabled but could not find the decoration on the EventInfo object.");
    return EL::StatusCode::FAILURE;
  }

  // standard all jets and all bjets
  if(m_jetPlots["all/jets"]->execute(in_jets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
  if(m_jetPlots["all/bjets"]->execute(in_bjets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
  if(m_jetMETPlots["all/jets"]->execute(in_jets, in_met, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
  if(m_jetMETPlots["all/bjets"]->execute(in_bjets, in_met, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
  if(m_METPlots["all"]->execute(in_met, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;

  // standard preselected jets and preselected bjets
  if(m_passPreSel && pass_preSel(*eventInfo) == 1){
    if(m_jetPlots["presel/jets"]->execute(in_jets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    if(m_jetPlots["presel/bjets"]->execute(in_bjets, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    if(m_jetMETPlots["presel/jets"]->execute(in_jets, in_met, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    if(m_jetMETPlots["presel/bjets"]->execute(in_bjets, in_met, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    if(m_METPlots["presel"]->execute(in_met, eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
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

    //all/bjets/bTag
    //presel/bjets/bTag
    if(!m_decor_jetTags_b.empty()){
      m_jetPlots["all/bjets/bTag"]->execute(bjets_bTagged.asDataVector(), eventWeight);
      m_jetMETPlots["all/bjets/bTag"]->execute(bjets_bTagged.asDataVector(), in_met, eventWeight);

      if(m_passPreSel && pass_preSel(*eventInfo) == 1){
        m_jetPlots["presel/bjets/bTag"]->execute(bjets_bTagged.asDataVector(), eventWeight);
        m_jetMETPlots["presel/bjets/bTag"]->execute(bjets_bTagged.asDataVector(), in_met, eventWeight);
      }
    }

    //all/jets/topTag
    //presel/jets/topTag
    if(!m_decor_jetTags_top.empty()){
      m_jetPlots["all/jets/topTag"]->execute(jets_topTagged.asDataVector(), eventWeight);
      m_jetMETPlots["all/jets/topTag"]->execute(jets_topTagged.asDataVector(), in_met, eventWeight);

      if(m_passPreSel && pass_preSel(*eventInfo) == 1){
        m_jetPlots["presel/jets/topTag"]->execute(jets_topTagged.asDataVector(), eventWeight);
        m_jetMETPlots["presel/jets/topTag"]->execute(jets_topTagged.asDataVector(), in_met, eventWeight);
      }
    }

    //all/jets/wTag
    //presel/jets/wTag
    if(!m_decor_jetTags_w.empty()){
      m_jetPlots["all/jets/wTag"]->execute(jets_wTagged.asDataVector(), eventWeight);
      m_jetMETPlots["all/jets/wTag"]->execute(jets_wTagged.asDataVector(), in_met, eventWeight);

      if(m_passPreSel && pass_preSel(*eventInfo) == 1){
        m_jetPlots["presel/jets/wTag"]->execute(jets_wTagged.asDataVector(), eventWeight);
        m_jetMETPlots["presel/jets/wTag"]->execute(jets_wTagged.asDataVector(), in_met, eventWeight);
      }
    }
  }

  //all/jetX
  //presel/jetX
  for(int i=1; i <= std::min<int>( m_numLeadingJets, in_jets->size() ); ++i ){
    if(m_jetPlots["all/jet"+std::to_string(i)]->execute(in_jets->at(i), eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    if(m_passPreSel && pass_preSel(*eventInfo) == 1){
      if(m_jetPlots["presel/jet"+std::to_string(i)]->execute(in_jets->at(i), eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    }
  }

  //all/bjetX
  //presel/bjetX
  for(int i=1; i <= std::min<int>( m_numLeadingJets, in_bjets->size() ); ++i ){
    if(m_jetPlots["all/bjet"+std::to_string(i)]->execute(in_jets->at(i), eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    if(m_passPreSel && pass_preSel(*eventInfo) == 1){
      if(m_jetPlots["presel/bjet"+std::to_string(i)]->execute(in_jets->at(i), eventWeight) != EL::StatusCode::SUCCESS) return EL::StatusCode::FAILURE;
    }
  }

  //all/bjetX_bTag
  //presel/bjetX_bTag
  if(!m_decor_jetTags_b.empty()){
    for(int i=1; i <= std::min<int>( m_numLeadingJets, bjets_bTagged.size() ); ++i){
      m_jetPlots["all/bjet"+std::to_string(i)+"/bTag"]->execute(bjets_bTagged.at(i), eventWeight);
      if(m_passPreSel && pass_preSel(*eventInfo) == 1){
        m_jetPlots["presel/bjet"+std::to_string(i)+"/bTag"]->execute(bjets_bTagged.at(i), eventWeight);
      }
    }
  }

  //all/jetX_topTag
  //presel/jetX_topTag
  if(!m_decor_jetTags_top.empty()){
    for(int i=1; i <= std::min<int>( m_numLeadingJets, jets_topTagged.size() ); ++i){
      m_jetPlots["all/jet"+std::to_string(i)+"/topTag"]->execute(jets_topTagged.at(i), eventWeight);
      if(m_passPreSel && pass_preSel(*eventInfo) == 1){
        m_jetPlots["presel/jet"+std::to_string(i)+"/topTag"]->execute(jets_topTagged.at(i), eventWeight);
      }
    }
  }

  //all/jetX_wTag
  //presel/jetX_wTag
  if(!m_decor_jetTags_w.empty()){
    for(int i=1; i <= std::min<int>( m_numLeadingJets, jets_wTagged.size() ); ++i){
      m_jetPlots["all/jet"+std::to_string(i)+"/wTag"]->execute(jets_wTagged.at(i), eventWeight);
      if(m_passPreSel && pass_preSel(*eventInfo) == 1){
        m_jetPlots["presel/jet"+std::to_string(i)+"/wTag"]->execute(jets_wTagged.at(i), eventWeight);
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
