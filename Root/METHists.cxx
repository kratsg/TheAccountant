#include <TheAccountant/METHists.h>


TheAccountant::METHists::METHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::METHists::~METHists () {}

StatusCode TheAccountant::METHists::initialize() {
  m_mpx   = book(m_name, "mpx",  "p_{x}^{miss} [GeV]", 120, 0, 3000.);
  m_mpy   = book(m_name, "mpy",  "p_{y}^{miss} [GeV]", 120, 0, 3000.);
  m_MET   = book(m_name, "met",  "E_{T}^{miss} [GeV]", 120, 0, 3000.);
  m_phi   = book(m_name, "phi",  "#phi", 120, -TMath::Pi(), TMath::Pi() );
  m_sumet = book(m_name, "sumEt", "#Sigma E_{T} [GeV]", 120, 0, 3000.);

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::METHists::execute( const xAOD::MissingET* met, float eventWeight ) {
  m_mpx->Fill( met->mpx()/1.e3, eventWeight);
  m_mpy->Fill( met->mpy()/1.e3, eventWeight);
  m_MET->Fill( met->met()/1.e3, eventWeight);
  m_phi->Fill( met->phi(), eventWeight);
  m_sumet->Fill( met->sumet()/1.e3, eventWeight);

  return StatusCode::SUCCESS;
}
