#ifndef TheAccountant_OptimizationDump_H
#define TheAccountant_OptimizationDump_H

#include <TheAccountant/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// root includes
#include <TTree.h>

// c++ includes
#include <string>
#include <array>

class OptimizationDump : public TA::Algorithm
{

public:
  float m_rcTrimFrac = 0.0;

private:
  TTree* m_tree; //!

  // everything below here is filled in the ttree
  float m_eventWeight; //!
  float m_SF_pu; //!
  float m_SF_btag; //!
  int	m_ttbarHF; //!
  int	m_ttbarHF_ext; //!
  int   m_eventNumber; //!
  float m_effectiveMass; //!
  float m_totalTransverseMomentum; //!
  float m_totalTransverseMass; //!
  float m_mjsum; //!

  float m_dPhiMETMin; //!
  float m_mTb; //!

  float m_met; //!
  float m_met_mpx; //!
  float m_met_mpy; //!
  float m_met_phi; //!

  int m_numJets; //!
  int m_numBJets; //!
  int m_numJetsLargeR; //!
  int m_numJetsVarR_top; //!
  int m_numJetsVarR_W; //!

  /* tagging */
  int m_n_topTag_VeryLoose; //!
  int m_n_topTag_SmoothLoose; //!
  int m_n_topTag_SmoothTight; //!
  int m_n_topTag_Loose; //!
  int m_n_topTag_Tight; //!

  /* large-R jet details */
  std::array<float, 4> m_largeR_pt; //!
  std::array<float, 4> m_largeR_m; //!
  std::array<float, 4> m_largeR_split12; //!
  std::array<float, 4> m_largeR_split23; //!
  std::array<int  , 4> m_largeR_nsj; //!
  std::array<int  , 4> m_largeR_topTag_veryloose; //!
  std::array<int  , 4> m_largeR_topTag_loose; //!
  std::array<int  , 4> m_largeR_topTag_tight; //!
  std::array<int  , 4> m_largeR_topTag_smoothLoose; //!
  std::array<int  , 4> m_largeR_topTag_smoothTight; //!

  /* razor information */
  std::map<std::string, double> m_inclVar; //!

public:
  // this is a standard constructor
  OptimizationDump ();

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
  ClassDef(OptimizationDump, 1);
};

#endif
