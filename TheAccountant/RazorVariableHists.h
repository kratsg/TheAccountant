#ifndef TheAccountant_RazorVariablesHists_H
#define TheAccountant_RazorVariablesHists_H

#include "xAODAnaHelpers/HistogramManager.h"

// RestFrames includes                                                          
#include "RestFrames/RestFrame.hh"
#include "RestFrames/RFrame.hh"
#include "RestFrames/RLabFrame.hh"
#include "RestFrames/RDecayFrame.hh"
#include "RestFrames/RVisibleFrame.hh"
#include "RestFrames/RInvisibleFrame.hh"
#include "RestFrames/RSelfAssemblingFrame.hh"
#include "RestFrames/InvisibleMassJigsaw.hh"
#include "RestFrames/InvisibleRapidityJigsaw.hh"
#include "RestFrames/ContraBoostInvariantJigsaw.hh"
#include "RestFrames/MinimizeMassesCombinatoricJigsaw.hh"
#include "RestFrames/InvisibleGroup.hh"
#include "RestFrames/CombinatoricGroup.hh"

//namespace RF = RestFrames;

#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODJet/JetContainer.h"
//includes for muons and electrons
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"


namespace TheAccountant
{
  class RazorVariableHists : public HistogramManager
  {
  public:
    RazorVariableHists(std::string name);
    virtual ~RazorVariableHists() ; 

    StatusCode initialize();
    StatusCode execute( const xAOD::EventInfo* eventInfo, const xAOD::MissingET* met, const xAOD::JetContainer* jets, const xAOD::MuonContainer* in_muons, const xAOD::ElectronContainer* in_electrons, float eventWeight );

    using HistogramManager::book;
    using HistogramManager::execute;

  private:
    TH1F* jet_multiplicity;
    TH1F* ss_mass;
    TH1F* ss_invgamma;
    TH1F* ss_dphivis;
    TH1F* ss_costheta;
    TH1F* ss_dphidecayangle;
    TH1F* ss_visshape;
    TH1F* ss_mdeltaR;

    TH2F* ss_mass_vs_leadJetPt;
    TH2F* ss_mass_vs_leadJetEta;
    TH2F* ss_mass_vs_leadJetPhi;
    TH2F* ss_mass_vs_leadJetMass;
    TH2F* ss_mass_vs_leadJetEnergy;
    TH2F* ss_mass_vs_leadJetRapidity;
    TH2F* ss_mass_vs_jetMultiplicity;

    TH2F* ss_mass_vs_subleadJetPt;
    TH2F* ss_mass_vs_subleadJetEta;
    TH2F* ss_mass_vs_subleadJetPhi;
    TH2F* ss_mass_vs_subleadJetMass;
    TH2F* ss_mass_vs_subleadJetEnergy;
    TH2F* ss_mass_vs_subleadJetRapidity;

    
    TH2F* ss_invgamma_vs_MET;

    TH2F* ss_mdeltaR_vs_leadJetPt;
    TH2F* ss_mdeltaR_vs_leadJetEta;
    TH2F* ss_mdeltaR_vs_leadJetPhi;
    TH2F* ss_mdeltaR_vs_leadJetMass;
    TH2F* ss_mdeltaR_vs_leadJetEnergy;
    TH2F* ss_mdeltaR_vs_leadJetRapidity;
    TH2F* ss_mdeltaR_vs_jetMultiplicity;

    TH2F* ss_mdeltaR_vs_subleadJetPt;
    TH2F* ss_mdeltaR_vs_subleadJetEta;
    TH2F* ss_mdeltaR_vs_subleadJetPhi;
    TH2F* ss_mdeltaR_vs_subleadJetMass;
    TH2F* ss_mdeltaR_vs_subleadJetEnergy;
    TH2F* ss_mdeltaR_vs_subleadJetRapidity;


    TH2F* ss_mass_invgamma;
    TH2F* ss_mdeltaR_invgamma;

    TH2F* ss_mass_HT;
    TH2F* ss_mass_Meff;
    TH2F* ss_invgamma_Meff;

    TH2F* ss_dphidecayangle_vs_MET;

    TH2F* ss_gamma_Meff;
    TH2F* ss_gamma_MET;
    TH2F* ss_costheta_leadJetEta;
    TH2F* ss_costheta_leadJetPhi;
    TH2F* ss_costheta_leadJetRapidity;


    TH1F* s1_mass;
    TH1F* s1_costheta;

    TH1F* s2_mass;
    TH1F* s2_costheta;

    TH1F* i1_depth;    
    TH1F* i2_depth;

    TH1F* v1_nelements;
    TH1F* v2_nelements;

  };
}
#endif
