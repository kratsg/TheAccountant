#include <TheAccountant/JetMETHists.h>
#include <TLorentzVector.h>

#include <TheAccountant/VariableDefinitions.h>
namespace VD = VariableDefinitions;

TheAccountant::JetMETHists::JetMETHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::JetMETHists::~JetMETHists () {}

StatusCode TheAccountant::JetMETHists::initialize() {
  m_effectiveMass   = book(m_name, "Meff",  "M_{eff} [GeV]", 120, 0, 3000.);
  m_dPhiMin         = book(m_name, "dPhiMin", "#Delta#phi_{min}(jet, #slash{E}_{T})", 120, 0, 2*TMath::Pi());
  m_METSignificance = book(m_name, "METSig", "E_{T}^{miss}/#sqrt{H_{T}^{jet}} GeV^{1/2}", 40, 0., 20.);

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::JetMETHists::execute( const xAOD::JetContainer* jets, const xAOD::MissingET* met, float eventWeight ) {
  float sum_jetPt(0);
  // sum of leading N jets in event
  for(int i=0; i < std::min<int>( m_numLeadingJets, jets->size() ); ++i) sum_jetPt += jets->at(i)->pt();
  m_effectiveMass->Fill( (sum_jetPt + met->met())/1.e3, eventWeight );

  // compute dPhiMin
  m_dPhiMin->Fill( VD::dPhiMETMin(met, jets), eventWeight);

  // compute significance
  m_METSignificance->Fill( VD::METSignificance(met, jets, m_numLeadingJets), eventWeight );

  return StatusCode::SUCCESS;
}
