#ifndef TheAccountant_JetHists_H
#define TheAccountant_JetHists_H

#include "xAODAnaHelpers/HistogramManager.h"
//#include "xAODAnaHelpers/HelperClasses.h"
#include <xAODJet/JetContainer.h>

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

      // enable if you don't use the execute(JetContainer, float) call since it fills m_jetMultiplicity
      bool m_countJets = false; // whether to count the jets in event or not

    private:
      //basic
      TH1F* m_jetPt;                        //!
      TH1F* m_jetEta;                       //!
      TH1F* m_jetPhi;                       //!
      TH1F* m_jetM;                         //!
      TH1F* m_jetE;                         //!
      TH1F* m_jetRapidity;                  //!

      // counting jets per event
      TH1F* m_jetMultiplicity;              //!

  };
}
#endif
