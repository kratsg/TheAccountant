#include <TheAccountant/Algorithm.h>

// ROOT includes
#include <TSystem.h>

// RCU include for throwing an exception+message
#include <RootCoreUtils/ThrowMsg.h>

//  for isMC()
#include "xAODEventInfo/EventInfo.h"

// xAH includes
#include "xAODAnaHelpers/HelperFunctions.h"
#include "xAODAnaHelpers/tools/ReturnCheck.h"
#include <TheAccountant/VariableDefinitions.h>

namespace HF = HelperFunctions;
namespace VD = VariableDefinitions;

// this is needed to distribute the algorithm to the workers
ClassImp(TA::Algorithm)

TA::Algorithm::Algorithm() :
  m_name(""),
  m_eventInfo("EventInfo"),
  m_inputLargeRJets(""),
  m_inputJets(""),
  m_inputMET("MET_RefFinal"),
  m_inputMETName("Final"),
  m_inputElectrons(""),
  m_inputMuons(""),
  m_inputTauJets(""),
  m_inputPhotons(""),
  m_decor_jetTags_b(""),
  m_decor_jetTags_top(""),
  m_decor_jetTags_w(""),
  m_debug(false),
  m_isMC(-1),
  m_event(nullptr),
  m_store(nullptr)
{}

int TA::Algorithm::isMC(){
  // first override if need to
  if(m_isMC == 0 || m_isMC == 1) return m_isMC;

  const xAOD::EventInfo* ei(nullptr);
  // couldn't retrieve it
  if(!HF::retrieve(ei, m_eventInfo, m_event, m_store).isSuccess()){
    if(m_debug) Warning("isMC()", "Could not retrieve eventInfo container: %s", m_eventInfo.c_str());
    return -1;
  }

  static SG::AuxElement::ConstAccessor<uint32_t> eventType("eventTypeBitmask");
  if(!eventType.isAvailable(*ei)){
    if(m_debug) Warning("isMC()", "eventType is not available.");
    return -1;
  }

  // reached here, return 0 or 1 since we have all we need
  return (static_cast<uint32_t>(eventType(*ei)) & xAOD::EventInfo::IS_SIMULATION);
}
