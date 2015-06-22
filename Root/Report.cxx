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

#include <TheAccountant/VariableDefinitions.h>

namespace HF = HelperFunctions;
namespace VD = VariableDefinitions;

// this is needed to distribute the algorithm to the workers
ClassImp(Report)

Report :: Report () :
  m_topTagDecorationNames({"TopTagLoose", "TopTagTight", "SmoothTopTagLoose", "SmoothTopTagTight"})
{}

EL::StatusCode Report :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("Report").ignore();

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: histInitialize () {
  // initialize all histograms here

  if(!m_inputJets.empty()){
    m_jetKinematicPlots["all/jets"] = new TheAccountant::IParticleKinematicHists( "all/jets/" );
    m_jetPlots["all/jets"]          = new TheAccountant::JetHists( "all/jets/" );
    m_jetMETPlots["all/jets"]       = new TheAccountant::JetMETHists( "all/jets/" );

    // tagged jets
    //all/jets/bTag
    if(!m_decor_jetTags_b.empty()){
      m_jetKinematicPlots["all/jets/bTag"]          = new TheAccountant::IParticleKinematicHists("all/jets/bTag/");
      m_jetPlots["all/jets/bTag"]       = new TheAccountant::JetHists("all/jets/bTag/");
      if(!m_inputMET.empty())
        m_jetMETPlots["all/jets/bTag"]       = new TheAccountant::JetMETHists("all/jets/bTag/");
    }
  }

  if(!m_inputLargeRJets.empty()){
    m_jetKinematicPlots["all/jetsLargeR"]          = new TheAccountant::IParticleKinematicHists( "all/jetsLargeR/" );
    m_jetPlots["all/jetsLargeR"]       = new TheAccountant::JetHists( "all/jetsLargeR/" );
    m_jetMETPlots["all/jetsLargeR"]       = new TheAccountant::JetMETHists( "all/jetsLargeR/" );

    // for top tag counts
    for(auto decorationName: m_topTagDecorationNames){
      m_jetTagPlots["all/jetsLargeR" + decorationName] = new TheAccountant::JetTagHists( "all/jetsLargeR/" );
      m_jetTagPlots["all/jetsLargeR" + decorationName]->m_decorationName = decorationName;
    }

    //all/jetsLargeR/topTag
    if(!m_decor_jetTags_top.empty()){
      m_jetKinematicPlots["all/jetsLargeR/topTag"]         = new TheAccountant::IParticleKinematicHists("all/jetsLargeR/topTag/");
      m_jetPlots["all/jetsLargeR/topTag"]      = new TheAccountant::JetHists("all/jetsLargeR/topTag/");
      if(!m_inputMET.empty())
        m_jetMETPlots["all/jetsLargeR/topTag"]      = new TheAccountant::JetMETHists("all/jetsLargeR/topTag/");
    }

    //all/jetsLargeR/wTag
    if(!m_decor_jetTags_w.empty()){
      m_jetKinematicPlots["all/jetsLargeR/wTag"]           = new TheAccountant::IParticleKinematicHists("all/jetsLargeR/wTag/");
      m_jetPlots["all/jetsLargeR/wTag"]        = new TheAccountant::JetHists("all/jetsLargeR/wTag/");
      if(!m_inputMET.empty())
        m_jetMETPlots["all/jetsLargeR/wTag"]        = new TheAccountant::JetMETHists("all/jetsLargeR/wTag/");
    }
  }

  if(!m_inputMET.empty())
    m_METPlots["all/MET"]               = new TheAccountant::METHists( "all/MET/" );

  // enable jet counting for jet plots above, set type to jet
  for(auto jetKinematicPlot: m_jetKinematicPlots){
    jetKinematicPlot.second->m_countParticles = true;
    jetKinematicPlot.second->m_particleType   = "jet";
  }

  for(auto jetPlot: m_jetPlots){
    // do topology for all
    jetPlot.second->m_doTopology = true;
    if(jetPlot.first == "all/jets") continue;
    if(jetPlot.first == "all/jets/bTag") continue;
    // only do substructure for the largeR jets
    // !!! Do not have valid constituents !!!
    //jetPlot.second->m_doSubstructure = true;
  }

  // set the numLeadingJets for the JetMET histograms
  for(auto jetMETPlot: m_jetMETPlots) jetMETPlot.second->m_numLeadingJets = m_numLeadingJets;

  // NLeadingJets
  for(int i=1; i <= m_numLeadingJets; ++i){
    if(!m_inputJets.empty()){
      //all/jetX
      m_jetKinematicPlots["all/jet"+std::to_string(i)] = new TheAccountant::IParticleKinematicHists( "all/jet"+std::to_string(i)+"/" );

      // tagged jets
      //all/jetX_bTag
      if(!m_decor_jetTags_b.empty()){
        m_jetKinematicPlots["all/jet"+std::to_string(i)+"/bTag"] = new TheAccountant::IParticleKinematicHists("all/jet"+std::to_string(i)+"/bTag/");
      }
    }

    if(!m_inputLargeRJets.empty()){
      //all/jetLargeRX
      m_jetKinematicPlots["all/jetLargeR"+std::to_string(i)] = new TheAccountant::IParticleKinematicHists( "all/jetLargeR"+std::to_string(i)+"/" );

      for(auto decorationName: m_topTagDecorationNames){
        m_jetTagPlots["all/jetLargeR" + decorationName + std::to_string(i)] = new TheAccountant::JetTagHists( "all/jetLargeR"+std::to_string(i)+"/" );
        m_jetTagPlots["all/jetLargeR" + decorationName + std::to_string(i)]->m_decorationName = decorationName;
      }

      //all/jetLargeRX_topTag
      if(!m_decor_jetTags_top.empty()){
        m_jetKinematicPlots["all/jetLargeR"+std::to_string(i)+"/topTag"] = new TheAccountant::IParticleKinematicHists("all/jetLargeR"+std::to_string(i)+"/topTag/");
      }

      //all/jetLargeRX_wTag
      if(!m_decor_jetTags_w.empty()){
        m_jetKinematicPlots["all/jetLargeR"+std::to_string(i)+"/wTag"] = new TheAccountant::IParticleKinematicHists("all/jetLargeR"+std::to_string(i)+"/wTag/");
      }
    }
  }

  for(auto jetKinematicPlot: m_jetKinematicPlots){
    RETURN_CHECK("Report::initialize()", jetKinematicPlot.second->initialize(), "");
    jetKinematicPlot.second->record( wk() );
  }

  for(auto jetPlot: m_jetPlots){
    RETURN_CHECK("Report::initialize()", jetPlot.second->initialize(), "");
    jetPlot.second->record( wk() );
  }

  for(auto jetMETPlot: m_jetMETPlots){
    RETURN_CHECK("Report::initialize()", jetMETPlot.second->initialize(), "");
    jetMETPlot.second->record( wk() );
  }

  for(auto METPlot: m_METPlots){
    RETURN_CHECK("Report::initialize()", METPlot.second->initialize(), "");
    METPlot.second->record( wk() );
  }

  for(auto jetTagPlot: m_jetTagPlots){
    RETURN_CHECK("Report::initialize()", jetTagPlot.second->initialize(), "");
    jetTagPlot.second->record( wk() );
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
  if(m_debug) Info("execute()", "Calling execute...");
  const xAOD::EventInfo*                eventInfo     (nullptr);
  const xAOD::JetContainer*             in_jetsLargeR (nullptr);
  const xAOD::JetContainer*             in_jets       (nullptr);
  const xAOD::MissingETContainer*       in_missinget  (nullptr);
  const xAOD::ElectronContainer*        in_electrons  (nullptr);
  const xAOD::MuonContainer*            in_muons      (nullptr);
  const xAOD::TauJetContainer*          in_taus       (nullptr);
  const xAOD::PhotonContainer*          in_photons    (nullptr);
  //const xAOD::TruthParticleContainer*   in_truth    (nullptr);

  // start grabbing all the containers that we can
  RETURN_CHECK("Report::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  if(!m_inputJets.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_jets,     m_inputJets,       m_event, m_store, m_debug), "Could not get the inputJets container.");
  if(!m_inputLargeRJets.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_jetsLargeR,      m_inputLargeRJets,        m_event, m_store, m_debug), "Could not get the inputLargeRJets container.");
  if(!m_inputMET.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_missinget, m_inputMET,         m_event, m_store, m_debug), "Could not get the inputMET container.");
  if(!m_inputElectrons.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_electrons, m_inputElectrons,   m_event, m_store, m_debug), "Could not get the inputElectrons container.");
  if(!m_inputMuons.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_muons,     m_inputMuons,       m_event, m_store, m_debug), "Could not get the inputMuons container.");
  if(!m_inputTauJets.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_taus,      m_inputTauJets,     m_event, m_store, m_debug), "Could not get the inputTauJets container.");
  if(!m_inputPhotons.empty())
    RETURN_CHECK("Report::execute()", HF::retrieve(in_photons,   m_inputPhotons,     m_event, m_store, m_debug), "Could not get the inputPhotons container.");

  // prepare the jets by creating a view container to look at them
  ConstDataVector<xAOD::JetContainer> in_jetsCDV(SG::VIEW_ELEMENTS);
  if(!m_inputJets.empty()){
    for(auto jet: *in_jets){
      if(jet->pt()/1.e3 < m_jet_minPtView) continue;
      in_jetsCDV.push_back(jet);
    }
    // make in_jets point to a view instead
    in_jets = in_jetsCDV.asDataVector();
  }

  ConstDataVector<xAOD::JetContainer> in_jetsLargeRCDV(SG::VIEW_ELEMENTS);
  if(!m_inputLargeRJets.empty()){
    for(auto jet: *in_jetsLargeR){
      if(jet->pt()/1.e3 < m_jetLargeR_minPtView) continue;
      in_jetsLargeRCDV.push_back(jet);
    }
    // make in_jetsLargeR point to a view instead
    in_jetsLargeR = in_jetsLargeRCDV.asDataVector();
  }


  const xAOD::MissingET* in_met(nullptr);
  if(!m_inputMET.empty()){
    // retrieve CalibMET_RefFinal for METContainer
    xAOD::MissingETContainer::const_iterator met_id = in_missinget->find(m_inputMETName);
    if (met_id == in_missinget->end()) {
      Error("execute()", "No %s inside MET container", m_inputMETName.c_str());
      return EL::StatusCode::FAILURE;
    }
    // dereference the iterator since it's just a single object
    in_met = *met_id;
  }

  float eventWeight = VD::eventWeight(eventInfo, wk()->metaData());

  if(!m_inputJets.empty()){
    RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jets"]->execute(in_jets, eventWeight), "");
    RETURN_CHECK("Report::execute()", m_jetPlots["all/jets"]->execute(in_jets, eventWeight), "");
    if(!m_inputMET.empty())
      RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jets"]->execute(in_jets, in_met, eventWeight), "");
  }

  if(!m_inputLargeRJets.empty()){
    RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jetsLargeR"]->execute(in_jetsLargeR, eventWeight), "");
    RETURN_CHECK("Report::execute()", m_jetPlots["all/jetsLargeR"]->execute(in_jetsLargeR, eventWeight), "");
    if(!m_inputMET.empty())
      RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jetsLargeR"]->execute(in_jetsLargeR, in_met, eventWeight), "");
    for(auto decorationName: m_topTagDecorationNames)
      RETURN_CHECK("Report::execute()", m_jetTagPlots["all/jetsLargeR" + decorationName]->execute(in_jetsLargeR, in_met, eventWeight), "");
  }

  if(!m_inputMET.empty())
    RETURN_CHECK("Report::execute()", m_METPlots["all/MET"]->execute(in_met, eventWeight), "");

  //build up the tagged containers
  ConstDataVector<xAOD::JetContainer> jets_bTagged(SG::VIEW_ELEMENTS);
  ConstDataVector<xAOD::JetContainer> jetsLargeR_topTagged(SG::VIEW_ELEMENTS);
  ConstDataVector<xAOD::JetContainer> jetsLargeR_wTagged(SG::VIEW_ELEMENTS);

  if(!m_decor_jetTags_b.empty() || !m_decor_jetTags_top.empty() || !m_decor_jetTags_w.empty()){
    static SG::AuxElement::Accessor< int > decor_jetTags_b(m_decor_jetTags_b);
    static SG::AuxElement::Accessor< int > decor_jetTags_top(m_decor_jetTags_top);
    static SG::AuxElement::Accessor< int > decor_jetTags_w(m_decor_jetTags_w);

    if(!m_inputJets.empty()){
      for(const auto jet: *in_jets){
        if(!m_decor_jetTags_b.empty()){
          if(decor_jetTags_b.isAvailable(*jet)){
            if(decor_jetTags_b(*jet) == 1) jets_bTagged.push_back(jet);
          } else {
            Error("Report::execute()", "m_decor_jetTags_b is set but the decoration is missing on this jet.");
            return EL::StatusCode::FAILURE;
          }
        }
      }
    }

    if(!m_inputLargeRJets.empty()){
      for(const auto jet: *in_jetsLargeR){
        if(!m_decor_jetTags_top.empty()){
          if(decor_jetTags_top.isAvailable(*jet)){
            if(decor_jetTags_top(*jet) == 1) jetsLargeR_topTagged.push_back(jet);
          } else {
            Error("Report::execute()", "m_decor_jetTags_top is set but the decoration is missing on this jet.");
            return EL::StatusCode::FAILURE;
          }
        }

        if(!m_decor_jetTags_w.empty()){
          if(decor_jetTags_w.isAvailable(*jet)){
            if(decor_jetTags_w(*jet) == 1) jetsLargeR_wTagged.push_back(jet);
          } else {
            Error("Report::execute()", "m_decor_jetTags_w is set but the decoration is missing on this jet.");
            return EL::StatusCode::FAILURE;
          }
        }
      }
    }

    if(!m_inputJets.empty()){
      //all/jets/bTag
      if(!m_decor_jetTags_b.empty()){
        RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jets/bTag"]->execute(jets_bTagged.asDataVector(), eventWeight), "");
        RETURN_CHECK("Report::execute()", m_jetPlots["all/jets/bTag"]->execute(jets_bTagged.asDataVector(), eventWeight), "");
        if(!m_inputMET.empty())
          RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jets/bTag"]->execute(jets_bTagged.asDataVector(), in_met, eventWeight), "");
      }
    }

    if(!m_inputLargeRJets.empty()){
      //all/jetsLargeR/topTag
      if(!m_decor_jetTags_top.empty()){
        RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jetsLargeR/topTag"]->execute(jetsLargeR_topTagged.asDataVector(), eventWeight), "");
        RETURN_CHECK("Report::execute()", m_jetPlots["all/jetsLargeR/topTag"]->execute(jetsLargeR_topTagged.asDataVector(), eventWeight), "");
        if(!m_inputMET.empty())
          RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jetsLargeR/topTag"]->execute(jetsLargeR_topTagged.asDataVector(), in_met, eventWeight), "");
      }

      //all/jetsLargeR/wTag
      if(!m_decor_jetTags_w.empty()){
        RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jetsLargeR/wTag"]->execute(jetsLargeR_wTagged.asDataVector(), eventWeight), "");
        RETURN_CHECK("Report::execute()", m_jetPlots["all/jetsLargeR/wTag"]->execute(jetsLargeR_wTagged.asDataVector(), eventWeight), "");
        if(!m_inputMET.empty())
          RETURN_CHECK("Report::execute()", m_jetMETPlots["all/jetsLargeR/wTag"]->execute(jetsLargeR_wTagged.asDataVector(), in_met, eventWeight), "");
      }
    }
  }

  if(!m_inputJets.empty()){
    //all/jetX
    for(int i=1; i <= std::min<int>( m_numLeadingJets, in_jets->size() ); ++i ){
      RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jet"+std::to_string(i)]->execute(in_jets->at(i-1), eventWeight), "");
    }

    //all/jetX_bTag
    if(!m_decor_jetTags_b.empty()){
      for(int i=1; i <= std::min<int>( m_numLeadingJets, jets_bTagged.size() ); ++i){
        RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jet"+std::to_string(i)+"/bTag"]->execute(jets_bTagged.at(i-1), eventWeight), "");
      }
    }
  }

  if(!m_inputLargeRJets.empty()){
    //all/jetLargeRX
    for(int i=1; i <= std::min<int>( m_numLeadingJets, in_jetsLargeR->size() ); ++i ){
      RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jetLargeR"+std::to_string(i)]->execute(in_jetsLargeR->at(i-1), eventWeight), "");
      for(auto decorationName: m_topTagDecorationNames)
        RETURN_CHECK("Report::execute()", m_jetTagPlots["all/jetLargeR" + decorationName + std::to_string(i)]->execute(in_jetsLargeR->at(i-1), eventWeight), "");
    }

    //all/jetLargeRX_topTag
    if(!m_decor_jetTags_top.empty()){
      for(int i=1; i <= std::min<int>( m_numLeadingJets, jetsLargeR_topTagged.size() ); ++i){
        RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jetLargeR"+std::to_string(i)+"/topTag"]->execute(jetsLargeR_topTagged.at(i-1), eventWeight), "");
      }
    }

    //all/jetLargeRX_wTag
    if(!m_decor_jetTags_w.empty()){
      for(int i=1; i <= std::min<int>( m_numLeadingJets, jetsLargeR_wTagged.size() ); ++i){
        RETURN_CHECK("Report::execute()", m_jetKinematicPlots["all/jetLargeR"+std::to_string(i)+"/wTag"]->execute(jetsLargeR_wTagged.at(i-1), eventWeight), "");
      }
    }
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: postExecute () { return EL::StatusCode::SUCCESS; }

EL::StatusCode Report :: finalize () {
  for( auto jetKinematicPlot : m_jetKinematicPlots )
    if(jetKinematicPlot.second)
      delete jetKinematicPlot.second;

  for( auto jetPlot : m_jetPlots )
    if(jetPlot.second)
      delete jetPlot.second;

  for(auto jetMETPlot: m_jetMETPlots)
    if(jetMETPlot.second)
      delete jetMETPlot.second;

  for(auto METPlot: m_METPlots)
    if(METPlot.second)
      delete METPlot.second;

  for(auto jetTagPlot: m_jetTagPlots)
    if(jetTagPlot.second)
      delete jetTagPlot.second;

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Report :: histFinalize () { return EL::StatusCode::SUCCESS; }
