#ifndef TheAccountant_Algorithm_H
#define TheAccountant_Algorithm_H

// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// EL include(s):
#include <EventLoop/StatusCode.h>
#include <EventLoop/Algorithm.h>

#include <string>

namespace TA {

  class Algorithm : public EL::Algorithm {
      public:
        Algorithm();
        ClassDef(Algorithm, 1);

        // each algorithm should have a unique name for init, to differentiate them
        std::string m_name;

        // container names
        std::string m_eventInfo,
                    m_inputLargeRJets,
                    m_inputJets,
                    m_inputMET,
                    m_inputMETName,
                    m_inputElectrons,
                    m_inputMuons,
                    m_inputTauJets,
                    m_inputPhotons;
        std::string m_decor_jetTags_b,
                    m_decor_jetTags_top,
                    m_decor_jetTags_w;

        // enable verbosity, debugging or not
        bool m_debug;

        // override at algorithm level
        // default: -1, use eventInfo object to determine if data or mc
        // 0: this is data
        // 1: this is mc
        int m_isMC;

      protected:
        xAOD::TEvent* m_event; //!
        xAOD::TStore* m_store; //!

        // will try to determine if data or if MC
        // returns: -1=unknown (could not determine), 0=data, 1=mc
        int isMC();
  };

}
#endif
