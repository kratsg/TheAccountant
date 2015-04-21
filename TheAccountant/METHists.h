#ifndef TheAccountant_METHists_H
#define TheAccountant_METHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include <xAODMissingET/MissingETContainer.h>

namespace TheAccountant
{
  class METHists : public HistogramManager
  {
    public:
      METHists(std::string name);
      virtual ~METHists() ;
      EL::StatusCode initialize();
      EL::StatusCode execute( const xAOD::MissingET*, float eventWeight);
      using HistogramManager::book; // make other overloaded version of book() to show up in subclass
      using HistogramManager::execute; // overload

    private:
      TH1F* m_mpx;                //!
      TH1F* m_mpy;                //!
      TH1F* m_MET;                //!
      TH1F* m_phi;                //!
      TH1F* m_sumet;              //!
  };
}
#endif
