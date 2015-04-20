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

      int m_numLeadingJets = 0; // how many leading jets to add
      std::string m_decor_jetTags_b   = "", // isB,
                  m_decor_jetTags_top = "", // isTop,
                  m_decor_jetTags_w   = ""; // isW;

    private:
      //basic
      TH1F* m_jetPt;                        //!
      TH1F* m_jetEta;                       //!
      TH1F* m_jetPhi;                       //!
      TH1F* m_jetM;                         //!
      TH1F* m_jetE;                         //!
      TH1F* m_jetRapidity;                  //!

      //NLeadingJets
      std::vector< TH1F* > m_NjetsPt;       //!
      std::vector< TH1F* > m_NjetsEta;      //!
      std::vector< TH1F* > m_NjetsPhi;      //!
      std::vector< TH1F* > m_NjetsM;        //!
      std::vector< TH1F* > m_NjetsE;        //!
      std::vector< TH1F* > m_NjetsRapidity; //!

      // counting jets per event
      TH1F* m_countJets_all;                //!
      TH1F* m_countJets_bTags;              //!
      TH1F* m_countJets_topTags;            //!
      TH1F* m_countJets_wTags;              //!

  };
}
#endif
