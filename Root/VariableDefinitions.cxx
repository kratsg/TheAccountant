#include <TheAccountant/VariableDefinitions.h>

// P4Helpers
#include <FourMomUtils/xAODP4Helpers.h>

// for static casting
#include <xAODBase/IParticleContainer.h>

// c++ includes
#include <math.h>
#include <algorithm>
#include <string>

// substructure
#include <JetSubStructureUtils/Nsubjettiness.h>
#include <JetSubStructureUtils/KtSplittingScale.h>

// btagging
#include <xAODBTagging/BTagging.h>

// for event re-weighting
#include <SampleHandler/MetaFields.h>

namespace VD = VariableDefinitions;

std::string VD::wp2str(VD::WP wp){
  switch(wp){
    case VD::WP::VeryLoose:
      return "VeryLoose";
    break;
    case VD::WP::Loose:
      return "Loose";
    break;
    case VD::WP::Medium:
      return "Medium";
    break;
    case VD::WP::Tight:
      return "Tight";
    break;
  case VD::WP::SmoothLoose:
    return "SmoothTight";
    break;
  case VD::WP::SmoothTight:
    return "SmoothTight";
    break;
  }

  // should never reach here, we should be synced with the enum class
  return "BadWorkingPoint";
}

VD::WP VD::str2wp(std::string str){
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);
  if(str == "veryloose") return VD::WP::VeryLoose;
  if(str == "loose")     return VD::WP::Loose;
  if(str == "medium")    return VD::WP::Medium;
  if(str == "tight")     return VD::WP::Tight;
  if(str == "smoothloose") return VD::WP::SmoothLoose;
  if(str == "smoothtight") return VD::WP::SmoothTight;
  return VD::WP::None;
}


float VD::Meff(const xAOD::MissingET* met, const xAOD::JetContainer* jets, int numJets, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els){
  float meff(0.0);

  for(int i=0; i< std::min<int>(jets->size(), numJets); i++)
    meff += jets->at(i)->pt();

  if(muons)
    for(const auto &muon: *muons)
      meff += muon->pt();

  if(els)
    for(const auto &el: *els)
      meff += el->pt();

  meff += met->met();

  return meff;
}

float VD::HT(const xAOD::JetContainer* jets, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els){
  float ht(0.0);

  for(const auto &jet: *jets)
    ht += jet->pt();

  if(muons)
    for(const auto &muon: *muons)
      ht += muon->pt();

  if(els)
    for(const auto &el: *els)
      ht += el->pt();

  return ht;
}

float VD::mT(const xAOD::MissingET* met, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els){
  float mt(0.0);

  bool els_exist = els && els->size() > 0;
  bool muons_exist = muons && muons->size() > 0;

  // get leading lepton
  const xAOD::IParticle* leadingLepton(nullptr);

  // if no leptons passed, return 0.0
  if     (!muons_exist && !els_exist)
    return mt;
  else if(muons_exist && !els_exist)
    leadingLepton = static_cast<const xAOD::IParticle*>(muons->at(0));
  else if(!muons_exist && els_exist)
    leadingLepton = static_cast<const xAOD::IParticle*>(els->at(0));
  else {
    // if muon pt > electron pt, then leading lepton is muon
    if(muons->at(0)->pt() > els->at(0)->pt())
      leadingLepton = static_cast<const xAOD::IParticle*>(muons->at(0));
    // otherwise it is electron
    else
      leadingLepton = static_cast<const xAOD::IParticle*>(els->at(0));
  }

  mt = 2*leadingLepton->pt()*met->met()*(1-cos(xAOD::P4Helpers::deltaPhi(leadingLepton, met)));
  return std::sqrt(std::fabs(mt));
}

float VD::mTb(const xAOD::MissingET* met, const xAOD::JetContainer* bjets){
  // hold the sorted subset
  unsigned int numParticles(std::min<unsigned int>(bjets->size(), 3));
  if(numParticles == 0) return -99;
  std::vector<const xAOD::Jet*> subset_bjets(numParticles);
  // copy and sort
  std::partial_sort_copy(bjets->begin(), bjets->begin()+numParticles,
                         subset_bjets.begin(), subset_bjets.end(),
                         [](const xAOD::IParticle* lhs, const xAOD::IParticle* rhs)
                         -> bool {
                           return (lhs->pt() > rhs->pt());
                         });

  std::vector<float> result(numParticles);
  std::transform(subset_bjets.begin(), subset_bjets.end(),
                 result.begin(),
                 [met](const xAOD::Jet* bjet)
                 -> float {
                   return std::fabs(
                            pow(met->met() + bjet->pt(), 2)
                            - pow(met->mpx() + bjet->px(), 2)
                            - pow(met->mpy() + bjet->py(), 2)
                          );
                 });

  return std::sqrt(*std::min_element(result.begin(), result.end()));
}

float VD::dPhiMETMin(const xAOD::MissingET* met, const xAOD::IParticleContainer* particles,
                     unsigned int numLeadingParticles){
  // hold the sorted subset
  unsigned int numParticles(std::min<unsigned int>(particles->size(), numLeadingParticles));
  if(numParticles == 0) return -99;
  std::vector<const xAOD::IParticle*> subset_particles(numParticles);
  // copy and sort
  std::partial_sort_copy(particles->begin(), particles->begin()+numParticles,
                         subset_particles.begin(), subset_particles.end(),
                         [](const xAOD::IParticle* lhs, const xAOD::IParticle* rhs)
                         -> bool {
                           return (lhs->pt() > rhs->pt());
                         });
  // compute it
  std::vector<float> result(numParticles);
  std::transform(subset_particles.begin(), subset_particles.end(),
                 result.begin(),
                 [met](const xAOD::IParticle* particle)
                 -> float {
                   return std::fabs(xAOD::P4Helpers::deltaPhi(particle, met));
                 });

  return *std::min_element(result.begin(), result.end());
}

float VD::METSignificance(const xAOD::MissingET* met, const xAOD::JetContainer* jets, int njets){
  float met_significance(0.0);
  float ht(0.0);

  for(int i=0; i < std::min<int>(njets, jets->size()); i++){
    const auto jet = jets->at(i);
    if(jet->pt()/1.e3 < 30.) continue;
    ht += jet->pt();
  }
  if(ht > 0) met_significance = (met->met()/1.e3)/std::sqrt(ht/1.e3);
  return met_significance;
}

float VD::eventWeight(const xAOD::EventInfo* ei, const SH::MetaObject* metaData){
  // for now, we will return 1.0 to make it consistent and scale in optimization and plotting (fuck)
  static VD::accessor_t<uint32_t> eventType("eventTypeBitmask");
  if(!eventType.isAvailable(*ei)){
    std::cout << "Warning: eventType is not available. Returning 1." << std::endl;
    return 1.0;
  }

  //if(!ei->eventType( xAOD::EventInfo::IS_SIMULATION ))
  if( !(static_cast<uint32_t>(eventType(*ei)) & xAOD::EventInfo::IS_SIMULATION) )
    return 1.0;

  static VD::accessor_t<float> weight_mc("weight_mc");
  float weight(weight_mc.isAvailable(*ei)?weight_mc(*ei):1.0);
  //float weight(weight_mc.isAvailable(*ei)?weight_mc(*ei):ei->mcEventWeight());

  return weight;
  /*
  float crossSection(1),
        kFactor(1),
        filterEfficiency(1),
  //      xsUncertainty(1),
        numEvents(1);

  // if metadata is set, use it, otherwise pass the event weight, uncorrected
  if(metaData){
    crossSection = metaData->castDouble(SH::MetaFields::crossSection, crossSection);
    kFactor = metaData->castDouble(SH::MetaFields::kfactor, kFactor);
    filterEfficiency = metaData->castDouble(SH::MetaFields::filterEfficiency, filterEfficiency);
    //xsUncertainty = metaData->castDouble(SH::MetaFields::crossSectionRelUncertainty, xsUncertainty);
    numEvents = metaData->castDouble(SH::MetaFields::numEvents, numEvents);
  }

  return (weight*crossSection*kFactor*filterEfficiency/numEvents);
  */
}

int VD::ttbarHF(const xAOD::EventInfo* ei){
    static VD::accessor_t<int> ttbar_class("ttbar_class");
  int ttbarHF(ttbar_class.isAvailable(*ei)?ttbar_class(*ei):-888);
  
  return ttbarHF;  
}

int VD::ttbarHF_ext(const xAOD::EventInfo* ei){
 
 static VD::accessor_t<int> ttbar_class_ext("ttbar_class_ext");
  int ttbarHF_ext(ttbar_class_ext.isAvailable(*ei)?ttbar_class_ext(*ei):-888);

  return ttbarHF_ext;
}

int VD::topTag(const xAOD::EventInfo* eventInfo, const xAOD::JetContainer* jets, VD::WP wp){

  static VD::decor_t< int > nTops_wp("nTops_"+VD::wp2str(wp));

  // set or reset to 0 top tags
  nTops_wp(*eventInfo) = 0;

  // loop over jets, tag, and count top tags
  int nTops(0);
  for(const auto &jet: *jets) nTops += static_cast<int>(VD::topTag(eventInfo, jet, wp));

  // tag the event itself with # of jets tagged
  nTops_wp(*eventInfo) = nTops;
  return nTops;
}

bool VD::topTag(const xAOD::EventInfo* eventInfo, const xAOD::Jet* jet, VD::WP wp){
  bool isTop_tagged = false;
  switch(wp){
    case VD::WP::VeryLoose:
    {
      isTop_tagged = (jet->m()/1.e3 > 100.);
    }
    break;
    case VD::WP::Loose:
    {
      if(eventInfo->isAvailable<char>("LooseTopTag"))
	  isTop_tagged = (bool)jet->auxdata<char>("LooseTopTag");
    }				     
    case VD::WP::Tight:
    {
      if(eventInfo->isAvailable<int>("TightTopTag"))
	isTop_tagged = (bool)jet->auxdata<int>("TightTopTag");
    }
    break;
  case VD::WP::SmoothLoose:
    {
      if(eventInfo->isAvailable<char>("LooseSmoothTopTag"))
	isTop_tagged = (bool)jet->auxdata<int>("LooseSmoothTopTag");
    }
    break;
  case VD::WP::SmoothTight:
    {
      if(eventInfo->isAvailable<int>("TightSmoothTopTag"))
	isTop_tagged = (bool)jet->auxdata<int>("TightSmoothTopTag");
    }
    break;
  default:
    {
      isTop_tagged = false;
    }
    break;
  }

  static VD::decor_t< int > isTop_wp("isTop_"+VD::wp2str(wp));
  // tag the jet
  isTop_wp(*jet) = static_cast<int>(isTop_tagged);

  return isTop_tagged;
}

int VD::bTag(const xAOD::EventInfo* eventInfo, const xAOD::JetContainer* jets, VD::WP wp){

  static VD::decor_t< int > nBJets_wp("nBJets_"+VD::wp2str(wp));

  // set or reset to 0 b tags
  nBJets_wp(*eventInfo) = 0;

  // loop over jets, tag, and count top tags
  int nBJets(0);
  for(const auto &jet: *jets) nBJets += static_cast<int>(VD::bTag(jet, wp));

  // tag the event itself with # of jets tagged
  nBJets_wp(*eventInfo) = nBJets;
  return nBJets;
}

bool VD::bTag(const xAOD::Jet* jet, VD::WP wp, float maxAbsEta){

  if(std::fabs(jet->eta()) > maxAbsEta) return false;

  // stupid btagging doesn't overload the fucking MVx_discriminant()
  // float btag_weight(-99.);
  double btag_weight(-99.);
  if(!jet->getAttribute("btag_MV2c20", btag_weight))
    if(!jet->btagging()->MVx_discriminant("MV2c20", btag_weight))
      return false;

  // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarks#MV2c20_tagger_AntiKt4EMTopoJets
  bool isB_tagged = false;
  switch(wp){
    case VD::WP::Loose: // 85%
    {
      isB_tagged = btag_weight > -0.7887;
    }
    break;
    case VD::WP::Medium: // 70%
    {
      isB_tagged = btag_weight > -0.0436;
    }
    break;
    case VD::WP::Tight: // 60%
    {
      isB_tagged = btag_weight > 0.4496;
    }
    break;
    default:
    {
      isB_tagged = false;
    }
    break;
  }

  static VD::decor_t< int > isB_wp("isB_"+VD::wp2str(wp));
  // tag the jet
  isB_wp(*jet) = static_cast<int>(isB_tagged);

  return isB_tagged;
}


float VD::Tau21(const xAOD::Jet* jet){
  float tau21(0.0), tau1(0.0), tau2(0.0);

  if(jet->getAttribute("Tau21_wta", tau21)){
    return tau21;
  } else if(jet->getAttribute("Tau1_wta", tau1) && jet->getAttribute("Tau2_wta", tau2)){
    static VD::decor_t< float > Tau21("Tau21_wta");
    Tau21(*jet) = tau2/tau1;
    return tau2/tau1;
  } else {
    // this should never be called, but we include it
    //std::cout << "Warning <VariableDefinition>: Calculating subjettiness." << std::endl;
    //VD::Nsubjettiness(jet);
    //return VD::Tau21(jet);
    std::cout << "Warning <VariableDefinition>: Cannot find subjettiness Tau21." << std::endl;
    return -99;
  }
}

float VD::Tau32(const xAOD::Jet* jet){
  float tau32(0.0), tau2(0.0), tau3(0.0);

  if(jet->getAttribute("Tau32_wta", tau32)){
    return tau32;
  } else if(jet->getAttribute("Tau2_wta", tau2) && jet->getAttribute("Tau3_wta", tau3)){
    static VD::decor_t< float > Tau32("Tau32_wta");
    Tau32(*jet) = tau3/tau2;
    return tau3/tau2;
  } else {
    // this should never be called, but we include it
    //std::cout << "Warning <VariableDefinition>: Calculating subjettiness." << std::endl;
    //VD::Nsubjettiness(jet);
    //return VD::Tau32(jet);
    std::cout << "Warning <VariableDefinition>: Cannot find subjettiness Tau32." << std::endl;
    return -99;
  }
}

void VD::Nsubjettiness(const xAOD::Jet* jet, float alpha){
  fastjet::contrib::NormalizedCutoffMeasure normalized_measure(alpha, jet->getSizeParameter(), 1000000);
  fastjet::contrib::KT_Axes kt_axes;

  JetSubStructureUtils::Nsubjettiness tau1_cal(1, kt_axes, normalized_measure);
  JetSubStructureUtils::Nsubjettiness tau2_cal(2, kt_axes, normalized_measure);
  JetSubStructureUtils::Nsubjettiness tau3_cal(3, kt_axes, normalized_measure);

  // compute the subjettiness
  float tau1 = tau1_cal.result(*jet);
  float tau2 = tau2_cal.result(*jet);
  float tau3 = tau3_cal.result(*jet);

  // set up decorators for setting the properties
  static VD::decor_t< float > Tau1("Tau1");
  static VD::decor_t< float > Tau2("Tau2");
  static VD::decor_t< float > Tau3("Tau3");
  static VD::decor_t< float > Tau21("Tau21");
  static VD::decor_t< float > Tau32("Tau32");

  // start decorating our jets
  Tau1(*jet) = tau1;
  Tau2(*jet) = tau2;
  Tau3(*jet) = tau3;

  if(std::fabs(tau1) > 1e-8) // Prevent div-0
    Tau21(*jet) = tau2/tau1;
  else
    Tau21(*jet) = -999.0;
  if(std::fabs(tau2) > 1e-8) // Prevent div-0
    Tau32(*jet) = tau3/tau2;
  else
    Tau32(*jet) = -999.0;

  return;
}

float VD::Split12(const xAOD::Jet* jet){
  float split12(0.0);
  if(jet->getAttribute("Split12", split12)){
      return split12;
  } else {
    VD::KtSplittingScale(jet);
    return VD::Split12(jet);
  }
}

float VD::Split23(const xAOD::Jet* jet){
  float split23(0.0);
  if(jet->getAttribute("Split23", split23)){
      return split23;
  } else {
    VD::KtSplittingScale(jet);
    return VD::Split23(jet);
  }
}

float VD::Split34(const xAOD::Jet* jet){
  float split34(0.0);
  if(jet->getAttribute("Split34", split34)){
      return split34;
  } else {
    VD::KtSplittingScale(jet);
    return VD::Split34(jet);
  }
}

void VD::KtSplittingScale(const xAOD::Jet* jet){
  static VD::decor_t< float > Split12("Split12");
  static VD::decor_t< float > Split23("Split23");
  static VD::decor_t< float > Split34("Split34");

  JetSubStructureUtils::KtSplittingScale split12_cal(1);
  JetSubStructureUtils::KtSplittingScale split23_cal(2);
  JetSubStructureUtils::KtSplittingScale split34_cal(3);

  Split12(*jet) = split12_cal.result(*jet);
  Split23(*jet) = split23_cal.result(*jet);
  Split34(*jet) = split34_cal.result(*jet);

  return;
}
