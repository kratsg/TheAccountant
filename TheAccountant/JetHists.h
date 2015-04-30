#ifndef TheAccountant_JetHists_H
#define TheAccountant_JetHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include "xAODJet/JetContainer.h"

namespace TheAccountant
{
  class JetHists : public HistogramManager
  {
    public:
      JetHists(std::string name);
      virtual ~JetHists() ;
      EL::StatusCode initialize();
      EL::StatusCode execute( const xAOD::JetContainer* jets, float eventWeight);
      EL::StatusCode execute( const xAOD::Jet* jet, float eventWeight);
      using HistogramManager::book; // make other overloaded version of book() to show up in subclass
      using HistogramManager::execute; // overload

    private:
      TH1F* m_massOverPt;//!

      //topology
      TH1F* m_dEta;      //!
      TH1F* m_dPhi;      //!
      TH1F* m_asymmetry; //!
      TH1F* m_ptRatio;   //!
      TH1F* m_totalMass; //!

  };
}
#endif
