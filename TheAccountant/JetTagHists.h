#ifndef TheAccountant_JetTagHists_H
#define TheAccountant_JetTagHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include "xAODJet/JetContainer.h"

namespace TheAccountant
{
  class JetTagHists : public HistogramManager
  {
    public:
      JetTagHists(std::string name);
      virtual ~JetTagHists() ;
      StatusCode initialize();
      StatusCode execute( const xAOD::JetContainer* jets, float eventWeight);
      StatusCode execute( const xAOD::Jet* jet, float eventWeight);
      using HistogramManager::book; // make other overloaded version of book() to show up in subclass
      using HistogramManager::execute; // overload

      std::string m_decorationName;

    private:
      TH1F* m_decorationCount;          //!
  };
}
#endif
