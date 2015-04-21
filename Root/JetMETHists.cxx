#include <TheAccountant/JetMETHists.h>


TheAccountant::JetMETHists::JetMETHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::JetMETHists::~JetMETHists () {}

EL::StatusCode TheAccountant::JetMETHists::initialize() {
  m_effectiveMass   = book(m_name, "Meff",  "M_{eff} [GeV]", 120, 0, 3000.);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TheAccountant::JetMETHists::execute( const xAOD::JetContainer* jets, const xAOD::MissingET* met, float eventWeight ) {
  float sum_jetPt(0);
  // sum of leading N jets in event
  for(int i=0; i < std::min<int>( m_numLeadingJets, jets->size() ); ++i) sum_jetPt += jets->at(i)->pt();

  m_effectiveMass->Fill( (sum_jetPt + met->met())/1.e3, eventWeight );

  return EL::StatusCode::SUCCESS;
}
