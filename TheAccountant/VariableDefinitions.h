#ifndef TheAccountant_VariableDefinitions_H
#define TheAccountant_VariableDefinitions_H

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMissingET/MissingETContainer.h"

// we want to use the metadata if we can
#include "SampleHandler/MetaObject.h"

/* Caveats: input containers are assumed sorted */
namespace VariableDefinitions {
  // definitions for baseline, or, signal
  static SG::AuxElement::ConstAccessor<char> isBaseline("baseline");
  static SG::AuxElement::ConstAccessor<char> passOverlap("passOR");
  static SG::AuxElement::ConstAccessor<char> isSignal("signal");

  // for tagging primarily, but an enum for working points
  //  - an enum class enforces strong typing
  enum class WP {
    VeryLoose,
    Loose,
    Medium,
    Tight,
    None
  };
  // helper to convert to actual string for the tagging
  std::string wp2str(WP wp);
  WP str2wp(std::string str);

  // Effective Mass calculated using exclusive or inclusive definition depending on args
  //    Exclusive: Meff(met, jets, 4, 0, 0)
  //    Inclusive: Meff(met, jets, jets->size(), muons, els)
  float Meff(const xAOD::MissingET* met, const xAOD::JetContainer* jets, int numJets, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els);

  // HT calculated using all jets, will include muons and/or electrons if specified
  float HT(const xAOD::JetContainer* jets, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els);

  // Transverse Mass calculated using MET and leading lepton
  float mT(const xAOD::MissingET* met, const xAOD::MuonContainer* muons, const xAOD::ElectronContainer* els);

  // Minimum transverse mass of leading 3 b-jets and the met with a mass set at 0 (default)
  float mTb(const xAOD::MissingET* met, const xAOD::JetContainer* bjets);

  // Calculates the minimum dPhi between the met and the container of particles passed in
  float dPhiMETMin(const xAOD::MissingET* met, const xAOD::IParticleContainer* particles, unsigned int numLeadingParticles=4);

  // Calculates the MET significance as MET/sqrt(HT)
  float METSignificance(const xAOD::MissingET* met, const xAOD::JetContainer* jets, int njets);

  // Get the event weight consistently
  float eventWeight(const xAOD::EventInfo*, const SH::MetaObject* metaData = nullptr);

  // top tagging on jets, set the eventInfo with "nTops_<WP>" int decoration
  //    - static SG::AuxElement::Accessor< int > nTops_wp("nTops_<WP>");
  //        * string of WP is equivalent to how you type it out in enum class
  int topTag(const xAOD::EventInfo* eventInfo, const xAOD::JetContainer* jets, WP wp);
  // top tagging on jet, set "isTop_<WP>" int decoration
  bool topTag(const xAOD::Jet* jet, WP wp);

  // b-tagging on jets, set the eventInfo with "nBJets_<WP>" int decoration
  //    - static SG::AuxElement::Accessor< int > nBJets_wp("nBJets_<WP>");
  //        * string of WP is equivalent to how you type it out in enum class
  int bTag(const xAOD::EventInfo* eventInfo, const xAOD::JetContainer* jets, WP wp);
  // b-tagging on jet, set "isB_<WP>" int decoration, for extra sanity -- make sure we cut at 2.5
  bool bTag(const xAOD::Jet* jet, WP wp, float maxAbsEta=2.5);

  // returning subjettiness ratios for a given jet
  //    - use Tau21 if set
  //    - else use Tau2/Tau1 if both set
  //    - else recompute using Nsubjettiness
  float Tau21(const xAOD::Jet* jet);
  float Tau32(const xAOD::Jet* jet);
  void Nsubjettiness(const xAOD::Jet* jet, float alpha = 1.0);

  // returning splitting scale calculations for a given jet
  //    - use Split12 if set
  //    - else recompute using KtSplittingScale
  float Split12(const xAOD::Jet* jet);
  float Split23(const xAOD::Jet* jet);
  float Split34(const xAOD::Jet* jet);
  void KtSplittingScale(const xAOD::Jet* jet);

  // build lepton veto using overlaps on baseline/signal leptons
  template <typename T>
  ConstDataVector<T> leptonVeto(const T* leptons, bool requireSignal = false){
    ConstDataVector<T> VetoLeptons(SG::VIEW_ELEMENTS);
    for(auto l: *leptons){
      if(isBaseline(*l) == 0) continue;
      if(requireSignal && isSignal(*l) == 0) continue;
      if(passOverlap(*l) == 0) continue;
      VetoLeptons.push_back(l);
    }
    return VetoLeptons;
  }

}

#endif
