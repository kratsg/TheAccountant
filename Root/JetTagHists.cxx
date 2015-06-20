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
  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::JetTagHists::execute( const xAOD::JetContainer* jets, float eventWeight ) {
  for(const auto jet: *jets){
    if(!this->execute(jet, eventWeight).isSuccess()) return StatusCode::FAILURE;
  }
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
