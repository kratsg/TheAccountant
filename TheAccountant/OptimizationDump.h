#ifndef TheAccountant_OptimizationDump_H
#define TheAccountant_OptimizationDump_H

#include <TheAccountant/Algorithm.h>

// Infrastructure include(s):
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

// reclustering
#include <xAODJetReclustering/JetReclusteringTool.h>

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

  float m_dPhiMETMin; //!
  float m_mTb; //!

  float m_met; //!
  float m_met_mpx; //!
  float m_met_mpy; //!

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

  // three sets of reclustered jets for various radii
  std::array<JetReclusteringTool*, 3> m_jetReclusteringTools; //!
  // two sets of variable R reclustered jets
  std::array<JetReclusteringTool*, 2> m_varRjetReclusteringTools; //!

  /* reclustered jet details */
  std::array<std::array<float, 4>, 3 > m_rc_pt; //!
  std::array<std::array<float, 4>, 3 > m_rc_m; //!
  std::array<std::array<float, 4>, 3 > m_rc_split12; //!
  std::array<std::array<float, 4>, 3 > m_rc_split23; //!
  std::array<std::array<int, 4>  , 3 > m_rc_nsj; //!

  /* varR jet properties */
  std::array<float, 4> m_varR_top_m; //!
  std::array<float, 4> m_varR_top_pt; //!
  std::array<int, 4> m_varR_top_nsj; //!
  std::array<float, 4> m_varR_W_m; //!
  std::array<float, 4> m_varR_W_pt; //!
  std::array<int, 4> m_varR_W_nsj; //!

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
  double m_razor_GG_mass; //!
  double m_razor_Ga_mass; //!
  double m_razor_Gb_mass; //!
  double m_razor_Ca1_mass; //!
  double m_razor_Cb1_mass; //!
  double m_razor_Va1_mass; //!
  double m_razor_Vb1_mass; //!
  double m_razor_Va2_mass; //!
  double m_razor_Vb2_mass; //!
  /*
  double m_razor_pT_GG_Ga; //!
  double m_razor_pT_Va1_GG; //!
  double m_razor_pT_Va2_GG; //!
  double m_razor_pT_GG_Gb; //!
  double m_razor_pT_Vb1_GG; //!
  double m_razor_pT_Vb2_GG; //!
  double m_razor_pT_Ia1_GG; //!
  double m_razor_pT_Ib1_GG; //!
  double m_razor_p_Ga_GG; //!
  double m_razor_p_Va1_GG; //!
  double m_razor_p_Va2_GG; //!
  double m_razor_p_Gb_GG; //!
  double m_razor_p_Vb1_GG; //!
  double m_razor_p_Vb2_GG; //!
  double m_razor_p_Ia1_GG; //!
  double m_razor_p_Ib1_GG; //!
  double m_razor_GG_invGamma; //!
  */
  double m_razor_GG_invGamma; //!
  double m_razor_GG_visShape; //!
  double m_razor_GG_mDeltaR; //!
  double m_razor_Va1_n; //!
  double m_razor_Vb1_n; //!
  double m_razor_Ga_n; //!
  double m_razor_Va2_n; //!
  double m_razor_Vb2_n; //!
  double m_razor_Gb_n; //!
  /*
  double m_razor_Ia1_depth; //!
  double m_razor_Ib1_depth; //!
  */
  double m_razor_GG_cosTheta; //!
  double m_razor_Ga_cosIa1; //!
  double m_razor_Gb_cosIb1; //!
  double m_razor_Va1_cosTheta; //!
  double m_razor_Vb1_cosTheta; //!
  double m_razor_Va2_cosTheta; //!
  double m_razor_Vb2_cosTheta; //!
  double m_razor_GG_dPhiVis; //!
  double m_razor_GG_dPhiBetaR; //!
  double m_razor_GG_dPhiDecay; //!
  double m_razor_dPhi_Ga_Va1; //!
  double m_razor_dPhi_Ga_Ca1; //!
  double m_razor_dPhi_Gb_Vb1; //!
  double m_razor_dPhi_Gb_Cb1; //!
  double m_razor_dPhi_Ca1_Va2; //!
  double m_razor_dPhi_Cb1_Vb2; //!
  double m_razor_pT_GG; //!
  double m_razor_pZ_GG; //!
  double m_razor_H11GG; //!
  double m_razor_H21GG; //!
  double m_razor_H22GG; //!
  double m_razor_H41GG; //!
  double m_razor_H42GG; //!
  double m_razor_H11Ga; //!
  double m_razor_H11Gb; //!
  double m_razor_H21Ga; //!
  double m_razor_H21Gb; //!
  double m_razor_H11Ca1; //!
  double m_razor_H11Cb1; //!
  double m_razor_HT21GG; //!
  double m_razor_HT22GG; //!
  double m_razor_HT41GG; //!
  double m_razor_HT42GG; //!
  double m_razor_d_dPhiG; //!
  double m_razor_s_dPhiG; //!
  double m_razor_s_angle; //!
  double m_razor_d_angle; //!
  double m_razor_ratio_pZGG_HT21GG; //!
  double m_razor_ratio_pZGG_HT41GG; //!
  double m_razor_ratio_pTGG_HT21GG; //!
  double m_razor_ratio_pTGG_HT41GG; //!
  double m_razor_ratio_H11GG_H21GG; //!
  double m_razor_ratio_HT41GG_H41GG; //!
  double m_razor_ratio_H11GG_H41GG; //!
  double m_razor_maxRatio_H10PP_H11PP; //!


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
