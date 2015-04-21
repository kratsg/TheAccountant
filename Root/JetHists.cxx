#include <TheAccountant/JetHists.h>


TheAccountant::JetHists::JetHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::JetHists::~JetHists () {}

EL::StatusCode TheAccountant::JetHists::initialize() {
  m_jetPt           = book(m_name, "jetPt",  "jet p_{T} [GeV]", 120, 0, 3000.);
  m_jetEta          = book(m_name, "jetEta", "jet #eta",         80, -4, 4);
  m_jetPhi          = book(m_name, "jetPhi", "jet Phi",120, -TMath::Pi(), TMath::Pi() );
  m_jetM            = book(m_name, "jetMass", "jet Mass [GeV]",120, 0, 400);
  m_jetE            = book(m_name, "jetEnergy", "jet Energy [GeV]",120, 0, 4000.);
  m_jetRapidity     = book(m_name, "jetRapidity", "jet Rapidity",120, -10, 10);

  if(m_countJets) m_jetMultiplicity = book(m_name, "multiplicity_jets", "number of jets/event", 10, -0.5, 8.5);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TheAccountant::JetHists::execute( const xAOD::JetContainer* jets, float eventWeight ) {
  for( auto jet : *jets ) {
    this->execute( jet, eventWeight );
  }

  if(m_countJets) m_jetMultiplicity->Fill( jets->size(), eventWeight);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TheAccountant::JetHists::execute( const xAOD::Jet* jet, float eventWeight ) {
  //basic
  m_jetPt ->      Fill( jet->pt()/1e3,    eventWeight );
  m_jetEta->      Fill( jet->eta(),       eventWeight );
  m_jetPhi->      Fill( jet->phi(),       eventWeight );
  m_jetM->        Fill( jet->m()/1e3,     eventWeight );
  m_jetE->        Fill( jet->e()/1e3,     eventWeight );
  m_jetRapidity-> Fill( jet->rapidity(),  eventWeight );

  return EL::StatusCode::SUCCESS;
}
