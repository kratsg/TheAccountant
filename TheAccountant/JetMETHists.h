#ifndef TheAccountant_JetMETHists_H
#define TheAccountant_JetMETHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include <xAODJet/JetContainer.h>
#include <xAODMissingET/MissingETContainer.h>

namespace TheAccountant
{
  class JetMETHists : public HistogramManager
  {
    public:
      JetMETHists(std::string name);
      virtual ~JetMETHists() ;
      StatusCode initialize();
      StatusCode execute( const xAOD::JetContainer* jets, const xAOD::MissingET*, float eventWeight);
      using HistogramManager::book; // make other overloaded version of book() to show up in subclass
      using HistogramManager::execute; // overload

      // effective mass is the sum of the leading N jets in event together with MET
      int m_numLeadingJets;
    private:
      TH1F* m_effectiveMass;                //!
      TH1F* m_dPhiMin;                      //!
  };
}
#endif
