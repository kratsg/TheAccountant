#ifndef TheAccountant_RazorVariablesHists_H
#define TheAccountant_RazorVariablesHists_H

#include "xAODAnaHelpers/HistogramManager.h"

// RestFrames includes
#include "RestFrames/RestFrames.hh"

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
    StatusCode execute( const xAOD::EventInfo* eventInfo, const xAOD::MissingET* met, const xAOD::JetContainer* jets, const xAOD::JetContainer* jets_largeR, const xAOD::MuonContainer* in_muons, const xAOD::ElectronContainer* in_electrons, float eventWeight );

    using HistogramManager::book;
    using HistogramManager::execute;

  private:
    TH1F* jet_multiplicity;
    TH1F* gg_mass;
    TH1F* gg_invgamma;
    TH1F* gg_dphivis;
    TH1F* gg_costheta;
    TH1F* gg_dphidecayangle;
    TH1F* gg_visshape;
    TH1F* gg_mdeltaR;

    TH1F* gg_mdeltaR_ptless250;
    TH1F* gg_mdeltaR_pt250to350;
    TH1F* gg_mdeltaR_ptgreat350;

    TH1F* magg_jets;
    TH1F* jets_magg_largeR;

    TH2F* gg_magg_vs_leadJetPt;
    TH2F* gg_magg_vs_leadJetEta;
    TH2F* gg_magg_vs_leadJetPhi;
    TH2F* gg_magg_vs_leadJetMass;
    TH2F* gg_magg_vs_leadJetEnergy;
    TH2F* gg_magg_vs_leadJetRapidity;
    TH2F* gg_magg_vs_jetMultiplicity;
    TH2F* gg_magg_vs_2ndJetPt;
    TH2F* gg_magg_vs_2ndJetEta;
    TH2F* gg_magg_vs_2ndJetPhi;
    TH2F* gg_magg_vs_2ndJetMass;
    TH2F* gg_magg_vs_2ndJetEnergy;
    TH2F* gg_magg_vs_2ndJetRapidity;
    TH2F* gg_magg_vs_HT;
    TH2F* gg_magg_vs_Meff;

    TH2F* gg_invgamma_vs_MET;
    TH2F* gg_invgamma_vs_Meff;

    TH2F* gg_gamma_vs_Meff;
    TH2F* gg_gamma_vs_MET;

    TH2F* gg_mdeltaR_vs_leadJetPt;
    TH2F* gg_mdeltaR_vs_leadJetEta;
    TH2F* gg_mdeltaR_vs_leadJetPhi;
    TH2F* gg_mdeltaR_vs_leadJetMass;
    TH2F* gg_mdeltaR_vs_leadJetEnergy;
    TH2F* gg_mdeltaR_vs_leadJetRapidity;
    TH2F* gg_mdeltaR_vs_jetMultiplicity;
    TH2F* gg_mdeltaR_vs_2ndJetPt;
    TH2F* gg_mdeltaR_vs_2ndJetEta;
    TH2F* gg_mdeltaR_vs_2ndJetPhi;
    TH2F* gg_mdeltaR_vs_2ndJetMass;
    TH2F* gg_mdeltaR_vs_2ndJetEnergy;
    TH2F* gg_mdeltaR_vs_2ndJetRapidity;
    TH2F* gg_mdeltaR_vs_HT;
    TH2F* gg_mdeltaR_vs_Meff;

    TH2F* gg_costheta_vs_leadJetEta;
    TH2F* gg_costheta_vs_leadJetPhi;
    TH2F* gg_costheta_vs_leadJetRapidity;
    TH2F* gg_costheta_vs_2ndJetEta;
    TH2F* gg_costheta_vs_2ndJetPhi;
    TH2F* gg_costheta_vs_2ndJetRapidity;

    TH1F* gg_abs_costheta;
    TH1F* ga_abs_costheta;
    TH1F* gb_abs_costheta;

    TH2F* gg_dphivis_vs_MET;

    TH2F* gg_dphidecayangle_vs_MET;

    TH2F* gg_dphidecayangle_vs_METphi;
    TH2F* gg_dphivis_vs_METphi;

    TH1F* ga_mass;
    TH1F* ga_costheta;

    TH1F* gb_mass;
    TH1F* gb_costheta;

    TH1F* ia1_depth;
    TH1F* ib1_depth;

    TH1F* va1_n;
    TH1F* vb1_n;

    TH2F* gg_mdeltaR_vs_METsig;

  };
}
#endif
