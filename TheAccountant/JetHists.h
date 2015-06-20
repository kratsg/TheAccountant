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
      StatusCode initialize();
      StatusCode execute( const xAOD::JetContainer* jets, float eventWeight);
      StatusCode execute( const xAOD::Jet* jet, float eventWeight);
      using HistogramManager::book; // make other overloaded version of book() to show up in subclass
      using HistogramManager::execute; // overload

      bool m_doTopology,
           m_doSubstructure;

    private:
      TH1F* m_massOverPt;//!

      //topology
      TH1F* m_dEta;                     //!
      TH1F* m_dPhi;                     //!
      TH1F* m_asymmetry;                //!
      TH1F* m_ptRatio;                  //!
      TH1F* m_totalMass;                //!

      //substructure
      TH1F* m_tau21;                    //!
      TH1F* m_tau32;                    //!
      TH1F* m_subjet_multiplicity;      //!
      TH1F* m_subjet_ptFrac;            //!
      TH1F* m_constituents_multiplicity;//!
      TH1F* m_constituents_width;       //!
  };
}
#endif
