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

  // N leading jets
  for(int i=0; i < m_numLeadingJets; ++i){
    std::string jetNum = std::to_string(i);
    m_NjetsPt.push_back(      book(m_name, ("jetPt_jet"+jetNum),       "jet p_{T} [GeV]", 120, 0, 3000.) );
    m_NjetsEta.push_back(     book(m_name, ("jetEta_jet"+jetNum),      "jet #eta", 80, -4, 4) );
    m_NjetsPhi.push_back(     book(m_name, ("jetPhi_jet"+jetNum),      "jet #phi", 120, -TMath::Pi(), TMath::Pi() ) );
    m_NjetsM.push_back(       book(m_name, ("jetMass_jet"+jetNum),     "jet Mass [GeV]", 120, 0, 400) );
    m_NjetsE.push_back(       book(m_name, ("jetEnergy_jet"+jetNum),   "jet Energy [GeV]", 120, 0, 4000.) );
    m_NjetsRapidity.push_back(book(m_name, ("jetRapidity_jet"+jetNum), "jet Rapidity", 120, -10, 10) );
  }


  // event counting
  m_countJets_all = book(m_name, "multiplicity_jets", "number of jets/event", 10, -0.5, 8.5);
  if(!m_decor_jetTags_b.empty() || !m_decor_jetTags_top.empty() || !m_decor_jetTags_top.empty()){
    if(!m_decor_jetTags_b.empty())   m_countJets_bTags    = book(m_name, "multiplicity_bTag", "number of b-tags/event", 10, -0.5, 8.5);
    if(!m_decor_jetTags_top.empty()) m_countJets_topTags  = book(m_name, "multiplicity_topTag", "number of top-tags/event", 10, -0.5, 8.5);
    if(!m_decor_jetTags_w.empty())   m_countJets_wTags    = book(m_name, "multiplicity_wTag", "number of w-tags/event", 10, -0.5, 8.5);
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TheAccountant::JetHists::execute( const xAOD::JetContainer* jets, float eventWeight ) {
  for( auto jet_itr : *jets ) {
    this->execute( jet_itr, eventWeight );
  }

  for(int i=0; i < std::min<int>( m_numLeadingJets, jets->size() ); ++i){
    m_NjetsPt.at(i)->        Fill( jets->at(i)->pt()/1.e3,  eventWeight);
    m_NjetsEta.at(i)->       Fill( jets->at(i)->eta(),      eventWeight);
    m_NjetsPhi.at(i)->       Fill( jets->at(i)->phi(),      eventWeight);
    m_NjetsM.at(i)->         Fill( jets->at(i)->m()/1.e3,   eventWeight);
    m_NjetsE.at(i)->         Fill( jets->at(i)->e()/1.e3,   eventWeight);
    m_NjetsRapidity.at(i)->  Fill( jets->at(i)->rapidity(), eventWeight);
  }

  // event counting
  m_countJets_all->Fill( jets->size(), eventWeight);
  if(!m_decor_jetTags_b.empty() || !m_decor_jetTags_top.empty() || !m_decor_jetTags_w.empty()){
    static SG::AuxElement::Accessor< int > decor_jetTags_b(m_decor_jetTags_b);
    static SG::AuxElement::Accessor< int > decor_jetTags_top(m_decor_jetTags_top);
    static SG::AuxElement::Accessor< int > decor_jetTags_w(m_decor_jetTags_w);
    int num_jetTags_b(0),
        num_jetTags_top(0),
        num_jetTags_w(0);

    for(const auto jet: *jets){
      if(!m_decor_jetTags_b.empty()){
        if(decor_jetTags_b.isAvailable(*jet)){
          if(decor_jetTags_b(*jet) == 1)   num_jetTags_b++;
        } else {
          Error("JetHists::execute()", "m_decor_jetTags_b is set but the decoration is missing on this jet.");
          return EL::StatusCode::FAILURE;
        }
      }

      if(!m_decor_jetTags_top.empty()){
        if(decor_jetTags_top.isAvailable(*jet)){
          if(decor_jetTags_top(*jet) == 1) num_jetTags_top++;
        } else {
          Error("JetHists::execute()", "m_decor_jetTags_top is set but the decoration is missing on this jet.");
          return EL::StatusCode::FAILURE;
        }
      }

      if(!m_decor_jetTags_w.empty()){
        if(decor_jetTags_w.isAvailable(*jet)){
          if(decor_jetTags_w(*jet) == 1)   num_jetTags_w++;
        } else {
          Error("JetHists::execute()", "m_decor_jetTags_w is set but the decoration is missing on this jet.");
          return EL::StatusCode::FAILURE;
        }
      }
    }

    if(!m_decor_jetTags_b.empty())   m_countJets_bTags->   Fill(num_jetTags_b,   eventWeight);
    if(!m_decor_jetTags_top.empty()) m_countJets_topTags-> Fill(num_jetTags_top, eventWeight);
    if(!m_decor_jetTags_w.empty())   m_countJets_wTags->   Fill(num_jetTags_w,   eventWeight);
  }

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
