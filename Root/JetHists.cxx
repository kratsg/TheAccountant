#include <TheAccountant/JetHists.h>


TheAccountant::JetHists::JetHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::JetHists::~JetHists () {}

EL::StatusCode TheAccountant::JetHists::initialize() {
  m_massOverPt  = book(m_name, "MOverPt", "#frac{m^{jet}}{p_{T}}", 100, 0, 5);

  //topology
  m_dEta        = book(m_name, "j1j2/dEta", "|#Delta#eta(j^{1}, j^{2})|", 80, 0, 8);
  m_dPhi        = book(m_name, "j1j2/dPhi", "|#Delta#phi(j^{1}, j^{2})|", 120, 0, 2*TMath::Pi() );
  m_asymmetry   = book(m_name, "j1j2/asymmetry", "#frac{p_{T}^{1} - p_{T}^{2}}{p_{T}^{1} + p_{T}^{2}}", 100, 0, 1);
  m_ptRatio     = book(m_name, "j1j3/ptRatio", "#frac{p_{T}^{3}}{p_{T}^{1}}", 100, 0, 1);
  m_totalMass   = book(m_name, "sumJetM", "#Sigma m^{jet}", 120, 0, 3000.);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TheAccountant::JetHists::execute( const xAOD::JetContainer* jets, float eventWeight ) {

  float totalMass(0);
  for(const auto jet: *jets){
    totalMass += jet->m();
    this->execute(jet, eventWeight);
  }

  const xAOD::Jet* j1(nullptr);
  const xAOD::Jet* j2(nullptr);
  const xAOD::Jet* j3(nullptr);

  if(jets->size() > 0){
    j1 = jets->at(0);
    if(jets->size() > 1){
      j2 = jets->at(1);
      m_dEta->      Fill( fabs(j1->eta() - j2->eta()),                  eventWeight );
      m_dPhi->      Fill( fabs(j1->p4().DeltaPhi( j2->p4() )),          eventWeight );
      m_asymmetry-> Fill( (j1->pt() - j2->pt())/(j1->pt() + j2->pt()),  eventWeight );
      if(jets->size() > 2){
        j3 = jets->at(2);
        m_ptRatio->  Fill( j3->pt()/j1->pt() );
      }
    }
  }
  m_totalMass->     Fill( totalMass/1000., eventWeight );

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TheAccountant::JetHists::execute( const xAOD::Jet* jet, float eventWeight ){
  m_massOverPt->Fill( jet->m()/jet->pt(), eventWeight );

  return EL::StatusCode::SUCCESS;
}
