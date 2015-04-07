#ifndef TheAccountant_ClassifyEvent_H
#define TheAccountant_ClassifyEvent_H

#include <EventLoop/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

class ClassifyEvent : public EL::Algorithm
{

private:
  xAOD::TEvent *m_event; //!
  xAOD::TStore *m_store; //!

public:
  // this is a standard constructor
  ClassifyEvent ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(ClassifyEvent, 1);
};

#endif
