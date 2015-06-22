#include <TheAccountant/JetTagHists.h>

// jet definition
#include <fastjet/JetDefinition.hh>

// subjet finding
#include "JetSubStructureUtils/SubjetFinder.h"

#include "TheAccountant/VariableDefinitions.h"

#include <FourMomUtils/xAODP4Helpers.h>

namespace VD = VariableDefinitions;

TheAccountant::JetTagHists::JetTagHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::JetTagHists::~JetTagHists () {}

StatusCode TheAccountant::JetTagHists::initialize() {
  m_decorationCount = book(m_name, m_decorationName, m_decorationName, 3, -1.5, 1.5);

  m_meff_decor      = book(m_name, m_decorationName+"_Meff", m_decorationName, 6, -1.5, 4.5,
                                 "M_{eff} [GeV]", 120, 0, 3000.);
  m_metSig_decor    = book(m_name, m_decorationName+"_METSig", m_decorationName, 6, -1.5, 4.5,
                                   "E_{T}^{miss}/#sqrt{H_{T}^{jet}} GeV^{1/2}", 40, 0., 20.);
  m_ht_decor        = book(m_name, m_decorationName+"_HT", m_decorationName, 6, -1.5, 4.5,
                                   "#sum p_{T}^{jet} [GeV]", 100, 0, 3000.);

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::JetTagHists::execute( const xAOD::JetContainer* jets, const xAOD::MissingET* met, float eventWeight ) {
  int numDecor(0);
  for(const auto jet: *jets){
    if(!this->execute(jet, eventWeight).isSuccess()) return StatusCode::FAILURE;
    int decorVal(-1);
    jet->getAttribute(m_decorationName, decorVal);
    numDecor += static_cast<int>(decorVal == 1);
  }

  float meff(VD::Meff(met, jets, m_numLeadingJets, nullptr, nullptr));
  float metsig(VD::METSignificance(met, jets, m_numLeadingJets));
  float ht(VD::HT(jets, nullptr, nullptr));

  m_meff_decor->Fill( numDecor, meff/1.e3, eventWeight );
  m_metSig_decor->Fill( numDecor, metsig, eventWeight );
  m_ht_decor->Fill( numDecor, ht/1.e3, eventWeight );

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::JetTagHists::execute( const xAOD::Jet* jet, float eventWeight ){
  // if -1, could not retrieve decoration
  // 0,1: actual decoration value
  int decorVal(-1);
  jet->getAttribute(m_decorationName, decorVal);
  m_decorationCount->Fill(decorVal, eventWeight);

  return StatusCode::SUCCESS;
}
