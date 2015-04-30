#include <TheAccountant/JetMETHists.h>
#include <TLorentzVector.h>

TheAccountant::JetMETHists::JetMETHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::JetMETHists::~JetMETHists () {}

EL::StatusCode TheAccountant::JetMETHists::initialize() {
  m_effectiveMass   = book(m_name, "Meff",  "M_{eff} [GeV]", 120, 0, 3000.);
  m_dPhiMin         = book(m_name, "dPhiMin", "#Delta#phi_{min}(jet, #slash{E}_{T})", 120, 0, 2*TMath::Pi());

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TheAccountant::JetMETHists::execute( const xAOD::JetContainer* jets, const xAOD::MissingET* met, float eventWeight ) {
  float sum_jetPt(0);
  // sum of leading N jets in event
  for(int i=0; i < std::min<int>( m_numLeadingJets, jets->size() ); ++i) sum_jetPt += jets->at(i)->pt();
  m_effectiveMass->Fill( (sum_jetPt + met->met())/1.e3, eventWeight );

  float dPhiMin(2*TMath::Pi());
  // create a basic TLV
  TLorentzVector metp4;
  metp4.SetPhi( met->phi() );
  // compute dPhiMin
  for(const auto jet: *jets) dPhiMin = std::min<float>( dPhiMin, fabs(jet->p4().DeltaPhi(metp4)) );
  m_dPhiMin->Fill( dPhiMin, eventWeight);

  return EL::StatusCode::SUCCESS;
}
