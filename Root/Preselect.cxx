#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TheAccountant/Preselect.h>

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

// root include for cutflow
#include <TH1F.h>

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"
#include <TheAccountant/VariableDefinitions.h>
namespace HF = HelperFunctions;
namespace VD = VariableDefinitions;

// this is needed to distribute the algorithm to the workers
ClassImp(Preselect)
Preselect :: Preselect () {}

EL::StatusCode Preselect :: setupJob (EL::Job& job)
{
  job.useXAOD();
  xAOD::Init("Preselect").ignore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Preselect :: histInitialize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Preselect :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Preselect :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode Preselect :: initialize ()
{
  // assign m_event and m_store
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  // only do this check in preselect
  if(m_rc_enable && m_inputJets.empty()){
    Error("initialize()", "Cannot do reclustering if the input jet container is empty");
    return EL::StatusCode::FAILURE;
  }

  // if doing rc jets, use rc jets container instead
  if(m_rc_enable) m_inputLargeRJets = m_RCJetsContainerName;

  if(!m_triggerSelection.empty()){
    Info("initialize()", "Initializing for trigger selections: %s", m_triggerSelection.c_str());
    // trigger configuration, needed for TDT
    m_trigConf = new TrigConf::xAODConfigTool("TA_xAODConfigTool");
    RETURN_CHECK("initialize()", m_trigConf->initialize(), "Could not initialize TrigConf::xAODConfigTool.");
    ToolHandle< TrigConf::ITrigConfigTool > configHandle( m_trigConf );

    // The decision tool
    m_TDT = new Trig::TrigDecisionTool("TA_TrigDecTool");
    RETURN_CHECK("initialize()", m_TDT->setProperty("ConfigTool", configHandle), "Could not set ConfigTool property");
    RETURN_CHECK("initialize()", m_TDT->setProperty("TrigDecisionKey", "xTrigDecision"), "Could not set TrigDecisionKey property");
    RETURN_CHECK("initialize()", m_TDT->initialize(), "Could not initialize Trig::TrigDecisionTool");
  }

  if(m_rc_enable){
    // reclustering jets
    RETURN_CHECK("initialize()", (m_reclusteringTool.make<IJetReclusteringTool>()), "Failed to create handle to JetReclusteringTool");
    RETURN_CHECK("initialize()", m_reclusteringTool.setProperty("InputJetContainer",  "InputJetsPassPresel"), "Could not set input jet container");
    RETURN_CHECK("initialize()", m_reclusteringTool.setProperty("OutputJetContainer", m_RCJetsContainerName), "Coult not set output jet container");
    RETURN_CHECK("initialize()", m_reclusteringTool.setProperty("ReclusterRadius",    m_rc_radius), "Could not set radius for RC jet");
    RETURN_CHECK("initialize()", m_reclusteringTool.setProperty("InputJetPtMin",      m_rc_inputPt), "Could not set input pt filter");
    RETURN_CHECK("initialize()", m_reclusteringTool.setProperty("RCJetPtFrac",        m_rc_trimPtFrac), "Could not set output pt trim");
    RETURN_CHECK("initialize()", m_reclusteringTool.initialize(), "Could not initialize reclustering tool");
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Preselect :: execute ()
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
  RETURN_CHECK("Preselect::execute()", HF::retrieve(eventInfo,    m_eventInfo,        m_event, m_store, m_debug), "Could not get the EventInfo container.");
  if(!m_inputJets.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_jets,     m_inputJets,       m_event, m_store, m_debug), "Could not get the inputJets container.");
  // special: only retrieve large-R jets after doing reclustering (if we have to)
  //if(!m_inputLargeRJets.empty())
  //  RETURN_CHECK("Preselect::execute()", HF::retrieve(in_jetsLargeR,      m_inputLargeRJets,        m_event, m_store, m_debug), "Could not get the inputLargeRJets container.");
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

  float eventWeight = VD::eventWeight(eventInfo, wk()->metaData());

  const xAOD::MissingET* in_met(nullptr);
  if(!m_inputMET.empty()){
    in_met = (*in_missinget)[m_inputMETName.c_str()];
    if (!in_met) {
      Error("execute()", "No %s inside MET container", m_inputMETName.c_str());
      return EL::StatusCode::FAILURE;
    }
  }

  // Get total cutflow
  m_cutflow["total"].first += 1;
  m_cutflow["total"].second += eventWeight;


  // handle trigger
  if(!m_triggerSelection.empty()){
    const Trig::ChainGroup* triggerChainGroup = m_TDT->getChainGroup(m_triggerSelection);
    if ( !triggerChainGroup->isPassed() ) {
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["trigger"].first += 1;
    m_cutflow["trigger"].second += eventWeight;
  }

  // truth met filter
  if(!m_truthMETFilter.empty()){
    std::string truthMETSelection_str = m_truthMETFilter.substr(0,2);
    unsigned int truthMETSelection_cut = std::stoul(m_truthMETFilter.substr(2));
    bool pass_truthMETSelection = false;

    float met_truth_filter = eventInfo->auxdata<float>("met_truth_filter");
    if(truthMETSelection_str == "==")
      pass_truthMETSelection = (met_truth_filter == truthMETSelection_cut);
    else if(truthMETSelection_str == ">=")
      pass_truthMETSelection = (met_truth_filter >= truthMETSelection_cut);
    else if(truthMETSelection_str == "<=")
      pass_truthMETSelection = (met_truth_filter <= truthMETSelection_cut);
    else if(truthMETSelection_str == " >")
      pass_truthMETSelection = (met_truth_filter > truthMETSelection_cut);
    else if(truthMETSelection_str == " <")
      pass_truthMETSelection = (met_truth_filter < truthMETSelection_cut);
    else if(truthMETSelection_str == "!=")
      pass_truthMETSelection = (met_truth_filter != truthMETSelection_cut);
    else
      pass_truthMETSelection = false;

    if(!pass_truthMETSelection){
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["truthMET"].first += 1;
    m_cutflow["truthMET"].second += eventWeight;
  }

  // keep these outside of the if() because we need scope for the cutflow-after-preselection further beloe
  int num_passJets(0);
  int num_passBJets(0);
  if(!m_inputJets.empty()){
    // for small-R jets, count number of jets that pass standard cuts
    //int num_passJets = 0;
    for(const auto &jet: *in_jets){
      VD::dec_pass_preSel(*jet) = 0;
      if(m_badJetVeto && VD::isBad(*jet)){ // veto on bad jet if enabled
        wk()->skipEvent();
        return EL::StatusCode::SUCCESS;
      }
      if(jet->pt()/1000. < m_jet_minPt)   continue;
      if(jet->pt()/1000. > m_jet_maxPt)   continue;
      if(jet->eta()      < m_jet_minEta)  continue;
      if(jet->eta()      > m_jet_maxEta)  continue;
      if(jet->phi()      < m_jet_minPhi)  continue;
      if(jet->phi()      > m_jet_maxPhi)  continue;
      if(!VD::isSignal(*jet))             continue;
      num_passJets++;
      VD::dec_pass_preSel(*jet) = 1;
    }

    // increment cutflow for passing bad jet requirements
    m_cutflow["bad_jets"].first += 1;
    m_cutflow["bad_jets"].second += eventWeight;

    // only select event if:
    //    m_jet_minNum <= num_passJets <= m_jet_maxNum
    if(!( (m_jet_minNum <= num_passJets)&&(num_passJets <= m_jet_maxNum) )){
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["jets"].first += 1;
    m_cutflow["jets"].second += eventWeight;

    //int num_passBJets = 0;
    for(const auto &jet: *in_jets){
      VD::decor_tag_b(*jet) = 0;
      // assume it does not pass presel
      VD::dec_pass_preSel_b(*jet) = 0;
      if(m_badJetVeto && VD::isBad(*jet)){ // veto on bad jet if enabled
        wk()->skipEvent();
        return EL::StatusCode::SUCCESS;
      }
      if(jet->pt()/1000. < m_bjet_minPt)   continue;
      if(jet->pt()/1000. > m_bjet_maxPt)   continue;
      if(jet->eta()      < m_bjet_minEta)  continue;
      if(jet->eta()      > m_bjet_maxEta)  continue;
      if(jet->phi()      < m_bjet_minPhi)  continue;
      if(jet->phi()      > m_bjet_maxPhi)  continue;
      if(!VD::isSignal(*jet))              continue;
      // bTag the jet -- bjets are only btagged if passing preselection
      if(!VD::bTag(jet, m_bTag_wp)) continue;
      VD::decor_tag_b(*jet) = 1;
      // guess it passed!
      num_passBJets++;
      VD::dec_pass_preSel_b(*jet) = 1;
    }

    m_cutflow["bad_bjets"].first += 1;
    m_cutflow["bad_bjets"].second += eventWeight;

    // only select event if:
    //    m_bjet_minNum <= num_passBJets <= m_bjet_maxNum
    if(!( (m_bjet_minNum <= num_passBJets)&&(num_passBJets <= m_bjet_maxNum) )){
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["bjets"].first += 1;
    m_cutflow["bjets"].second += eventWeight;

    static VD::decor_t< int > pass_preSel_jets("pass_preSel_jets");
    static VD::decor_t< int > pass_preSel_bjets("pass_preSel_bjets");
    pass_preSel_jets(*eventInfo) = num_passJets;
    pass_preSel_bjets(*eventInfo) = num_passBJets;
  }

  // if rc_enable is true, then we will use the input jets that pass preselection to make reclustered jets
  //    if there are no input jets, the code will crash (eg: because the user didn't specify an input jet container)
  if(m_rc_enable){
    //auto preselJets = VD::subset_using_decor(in_jets, VD::acc_pass_preSel, 1);
    ConstDataVector<xAOD::JetContainer>* preselJets(new ConstDataVector<xAOD::JetContainer>(VD::subset_using_decor(in_jets, VD::acc_pass_preSel, 1)));

    RETURN_CHECK("Preselect::execute()", m_store->record(preselJets, "InputJetsPassPresel"), "Could not record presel jets into Store for reclustering");
    m_reclusteringTool->execute();
  }

  // special: only retrieve large-R jets after we get here, because if we do reclustering, they won't exist until after we process small-R jets)
  if(!m_inputLargeRJets.empty())
    RETURN_CHECK("Preselect::execute()", HF::retrieve(in_jetsLargeR,      m_inputLargeRJets,        m_event, m_store, m_debug), "Could not get the inputLargeRJets container.");

  // keep it outside scope for cut-flow selections
  int num_passJetsLargeR(0);
  int num_passTopTags(0);
  if(!m_inputLargeRJets.empty()){
    for(const auto &jet: *in_jetsLargeR){
      VD::dec_pass_preSel(*jet) = 0;
      if(jet->pt()/1000.  < m_jetLargeR_minPt)   continue;
      if(jet->pt()/1000.  > m_jetLargeR_maxPt)   continue;
      if(jet->m()/1000.   < m_jetLargeR_minMass) continue;
      if(jet->m()/1000.   > m_jetLargeR_maxMass) continue;
      if(jet->eta()       < m_jetLargeR_minEta)  continue;
      if(jet->eta()       > m_jetLargeR_maxEta)  continue;
      if(jet->phi()       < m_jetLargeR_minPhi)  continue;
      if(jet->phi()       > m_jetLargeR_maxPhi)  continue;
      num_passJetsLargeR++;
      VD::dec_pass_preSel(*jet) = 1;
    }

    // only select event if:
    //    m_jet_minNum <= num_passJets <= m_jet_maxNum
    if(!( (m_jetLargeR_minNum <= num_passJetsLargeR)&&(num_passJetsLargeR <= m_jetLargeR_maxNum) )){
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["jets_largeR"].first += 1;
    m_cutflow["jets_largeR"].second += eventWeight;

    for(const auto &jet: *in_jetsLargeR){
      VD::decor_tag_top(*jet) = 0;
      VD::dec_pass_preSel_top(*jet) = 0;
      if(jet->pt()/1000.  < m_topTag_minPt)   continue;
      if(jet->pt()/1000.  > m_topTag_maxPt)   continue;
      if(jet->m()/1000.   < m_topTag_minMass) continue;
      if(jet->m()/1000.   > m_topTag_maxMass) continue;
      if(jet->eta()       < m_topTag_minEta)  continue;
      if(jet->eta()       > m_topTag_maxEta)  continue;
      if(jet->phi()       < m_topTag_minPhi)  continue;
      if(jet->phi()       > m_topTag_maxPhi)  continue;
      // top tag the jet -- only largeR jets passing top tag preselection are considered for top tags
      if(!VD::topTag(jet, m_topTag_wp)) continue;
      VD::decor_tag_top(*jet) = 1;
      VD::dec_pass_preSel_top(*jet) = 1;
      num_passTopTags++;
    }

    // only select event if:
    //	m_topTag_minNum <= num_passTopTags <= m_topTag_maxNum
    if(!( (m_topTag_minNum <= num_passTopTags)&&(num_passTopTags <= m_topTag_maxNum) )){
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["toptags"].first += 1;
    m_cutflow["toptags"].second += eventWeight;

    static VD::decor_t< int > pass_preSel_jetsLargeR("pass_preSel_jetsLargeR");
    static VD::decor_t< int > pass_preSel_toptags("pass_preSel_toptags");
    pass_preSel_jetsLargeR(*eventInfo) = num_passJetsLargeR;
    pass_preSel_toptags(*eventInfo) = num_passTopTags;
  }

  // the following is copied twice, need to DRY it
  if(!m_baselineLeptonSelection.empty()){
    unsigned int numLeptons(0);
    // lepton veto
    if(!m_inputElectrons.empty()){
      ConstDataVector<xAOD::ElectronContainer> BaselineElectrons(VD::filterLeptons(in_electrons));
      numLeptons += BaselineElectrons.size();
    }
    if(!m_inputMuons.empty()){
      ConstDataVector<xAOD::MuonContainer> BaselineMuons(VD::filterLeptons(in_muons, false, true));
      numLeptons += BaselineMuons.size();
    }

    std::string leptonSelection_str = m_baselineLeptonSelection.substr(0,2);
    unsigned int leptonSelection_cut = std::stoul(m_baselineLeptonSelection.substr(2));
    bool pass_leptonSelection = false;
    if(leptonSelection_str == "==")
      pass_leptonSelection = (numLeptons == leptonSelection_cut);
    else if(leptonSelection_str == ">=")
      pass_leptonSelection = (numLeptons >= leptonSelection_cut);
    else if(leptonSelection_str == "<=")
      pass_leptonSelection = (numLeptons <= leptonSelection_cut);
    else if(leptonSelection_str == " >")
      pass_leptonSelection = (numLeptons > leptonSelection_cut);
    else if(leptonSelection_str == " <")
      pass_leptonSelection = (numLeptons < leptonSelection_cut);
    else if(leptonSelection_str == "!=")
      pass_leptonSelection = (numLeptons != leptonSelection_cut);
    else
      pass_leptonSelection = false;

    if(!pass_leptonSelection){
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["leptons_baseline"].first += 1;
    m_cutflow["leptons_baseline"].second += eventWeight;
  }

  if(!m_signalLeptonSelection.empty()){
    unsigned int numLeptons(0);
    // lepton veto
    if(!m_inputElectrons.empty()){
      ConstDataVector<xAOD::ElectronContainer> SignalElectrons(VD::filterLeptons(in_electrons, true));
      numLeptons += SignalElectrons.size();
    }
    if(!m_inputMuons.empty()){
      ConstDataVector<xAOD::MuonContainer> SignalMuons(VD::filterLeptons(in_muons, true, true));
      numLeptons += SignalMuons.size();
    }

    std::string leptonSelection_str = m_signalLeptonSelection.substr(0,2);
    unsigned int leptonSelection_cut = std::stoul(m_signalLeptonSelection.substr(2));
    bool pass_leptonSelection = false;
    if(leptonSelection_str == "==")
      pass_leptonSelection = (numLeptons == leptonSelection_cut);
    else if(leptonSelection_str == ">=")
      pass_leptonSelection = (numLeptons >= leptonSelection_cut);
    else if(leptonSelection_str == "<=")
      pass_leptonSelection = (numLeptons <= leptonSelection_cut);
    else if(leptonSelection_str == " >")
      pass_leptonSelection = (numLeptons > leptonSelection_cut);
    else if(leptonSelection_str == " <")
      pass_leptonSelection = (numLeptons < leptonSelection_cut);
    else if(leptonSelection_str == "!=")
      pass_leptonSelection = (numLeptons != leptonSelection_cut);
    else
      pass_leptonSelection = false;

    if(!pass_leptonSelection){
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["leptons_signal"].first += 1;
    m_cutflow["leptons_signal"].second += eventWeight;
  }

  if(!m_inputJets.empty() && !m_inputMET.empty()){
    if(VD::dPhiMETMin(in_met, VD::subset_using_decor(in_jets, VD::decor_signal, 1).asDataVector()) < m_dPhiMin){
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["dPhiMETMin"].first += 1;
    m_cutflow["dPhiMETMin"].second += eventWeight;
  }

  if(!m_inputMET.empty()){
    if(in_met->met()/1.e3 < m_minMET){
      wk()->skipEvent();
      return EL::StatusCode::SUCCESS;
    }
    m_cutflow["missing_et"].first += 1;
    m_cutflow["missing_et"].second += eventWeight;
  }

  // below here, we do not skip events, this is for merely counting after the fact
  // do cutflows on top of pre-selection
  ConstDataVector<xAOD::ElectronContainer> signalElectrons;
  ConstDataVector<xAOD::ElectronContainer> baselineElectrons;
  if(!m_inputElectrons.empty()){
    signalElectrons = VD::filterLeptons(in_electrons, true);
    baselineElectrons = VD::filterLeptons(in_electrons, false);
  }

  ConstDataVector<xAOD::MuonContainer> signalMuons;
  ConstDataVector<xAOD::MuonContainer> baselineMuons;
  if(!m_inputMuons.empty()){
    signalMuons = VD::filterLeptons(in_muons, true, true);
    baselineMuons = VD::filterLeptons(in_muons, false, true);
  }

  if(signalElectrons.size() >= 1){
    m_cutflow["geq1SigElectron"].first += 1;
    m_cutflow["geq1SigElectron"].second += eventWeight;
  }

  if(signalMuons.size() >= 1){
    m_cutflow["geq1SigMuon"].first += 1;
    m_cutflow["geq1SigMuon"].second += eventWeight;
  }

  if((baselineMuons.size() + baselineElectrons.size()) == 0){
    m_cutflow["exactly0BaselineLeptons"].first += 1;
    m_cutflow["exactly0BaselineLeptons"].second += eventWeight;
  }

  if(!m_inputMET.empty()){
    if(in_met->met()/1.e3 >= 250.){
      m_cutflow["MET250"].first += 1;
      m_cutflow["MET250"].second += eventWeight;
    }
  }

  if(!m_inputJets.empty() && !m_inputMET.empty()){
    auto signalJets = VD::subset_using_decor(in_jets, VD::decor_signal, 1);
    if(VD::dPhiMETMin(in_met, signalJets.asDataVector()) >= 0.4){
      m_cutflow["DPhi04"].first += 1;
      m_cutflow["DPhi04"].second += eventWeight;
    }

    if(VD::Meff_inclusive(in_met, signalJets.asDataVector(), signalMuons.asDataVector(), signalElectrons.asDataVector())/1000. >= 1000){
      m_cutflow["Meff1000"].first += 1;
      m_cutflow["Meff1000"].second += eventWeight;
    }

    auto signalBJets = VD::subset_using_decor(signalJets.asDataVector(), VD::acc_tag_b, 1);
    if(VD::mTb(in_met, signalBJets.asDataVector())/1000. >= 160){
      m_cutflow["mTb160"].first += 1;
      m_cutflow["mTb160"].second += eventWeight;
    }
  }

  if(num_passBJets >= 3){
    m_cutflow["geq3Bjets"].first += 1;
    m_cutflow["geq3Bjets"].second += eventWeight;
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode Preselect :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode Preselect :: finalize () {
  if(!m_triggerSelection.empty()){
    if(m_trigConf) delete m_trigConf;
    if(m_TDT) delete m_TDT;
  }

  for(const auto &cutflow: m_cutflow){
    TH1F* hist = new TH1F(("cutflow/"+cutflow.first).c_str(), cutflow.first.c_str(), 2, 1, 3);
    hist->SetBinContent(1, cutflow.second.first);
    hist->SetBinContent(2, cutflow.second.second);
    wk()->addOutput(hist);
  }

  return EL::StatusCode::SUCCESS;
}
EL::StatusCode Preselect :: histFinalize () { return EL::StatusCode::SUCCESS; }
