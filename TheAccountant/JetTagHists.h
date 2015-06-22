#ifndef TheAccountant_JetTagHists_H
#define TheAccountant_JetTagHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include "xAODJet/JetContainer.h"
#include <xAODMissingET/MissingETContainer.h>

namespace TheAccountant
{
  class JetTagHists : public HistogramManager
  {
    public:
      JetTagHists(std::string name);
      virtual ~JetTagHists() ;
      StatusCode initialize();
      StatusCode execute( const xAOD::JetContainer* jets, const xAOD::MissingET*, float eventWeight);
      StatusCode execute( const xAOD::Jet* jet, float eventWeight);
      using HistogramManager::book; // make other overloaded version of book() to show up in subclass
      using HistogramManager::execute; // overload

      std::string m_decorationName;
      // effective mass is the sum of the leading N jets in event together with MET
      int m_numLeadingJets;

    private:
      TH1F* m_decorationCount;          //!
      TH2F* m_meff_decor;               //!
      TH2F* m_metSig_decor;             //!
      TH2F* m_ht_decor;                 //!
  };
}
#endif
