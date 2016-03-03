#include <TheAccountant/RazorVariableHists.h>
#include "xAODEventInfo/EventInfo.h"
#include "TheAccountant/VariableDefinitions.h"

namespace RF = RestFrames;
namespace VD = VariableDefinitions;

TheAccountant::RazorVariableHists::RazorVariableHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::RazorVariableHists::~RazorVariableHists () {}

StatusCode TheAccountant::RazorVariableHists::initialize() {

  //simple plots of variables
  gg_mass = book(m_name,"gg_mass", "SS mass [GeV]", 50, 1000, 13000.);
  gg_invgamma = book(m_name, "gg_invgamma", "SS InvGamma [units?]", 20, 0, 1);
  gg_dphivis = book(m_name, "gg_dphivis", "SS dPhiIvs [units?]", 32, 0, 3.2);
  gg_costheta = book(m_name, "gg_costheta", "SS CosTheta", 100, -1., 1.);
  gg_abs_costheta = book(m_name, "gg_abs_costheta","|cos(#theta)|",25,0,1);
  ga_abs_costheta = book(m_name, "ga_abs_costheta","|cos(#theta)|",25,0,1);
  gb_abs_costheta = book(m_name, "gb_abs_costheta","|cos(#theta)|",25,0,1);


  gg_dphidecayangle = book(m_name, "gg_dphidecayangle", "SS dPhiDecayAngle [radians?]",32, 0, 3.2);
  gg_visshape = book(m_name, "gg_visshape", "SS VisShape [units?]", 100, 0, 1.);
  gg_mdeltaR = book(m_name, "gg_mdeltaR", "SS MDeltaR [GeV?]", 1300, 0, 13000);
  ga_mass = book(m_name, "ga_mass", "Ga Mass [GeV]", 200, 0, 6500);
  ga_costheta = book(m_name, "ga_costheta", "Ga CosTheta", 100, -1., 1.);
  gb_mass = book(m_name, "gb_mass", "Gb Mass [GeV]", 200, 0, 6500.);
  gb_costheta = book(m_name, "gb_costheta", "Gb CosTheta", 100,-1., 1.);
  ia1_depth = book(m_name, "ia1_depth", "I1 Depth [units?]", 8, 0, 8);
  ib1_depth = book(m_name, "ib1_depth", "I2 Depth [units?]", 8, 0, 8);
  va1_n = book(m_name, "va1_n", "Va1 n", 20, 0, 20);
  vb1_n = book(m_name, "vb1_n", "Vb1 n", 20, 0, 20);

  gg_mdeltaR_ptless250 = book(m_name,"gg_mdeltaR_ptless250","SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_pt250to350 = book(m_name,"gg_mdeltaR_pt250to350","SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_ptgreat350 = book(m_name,"gg_mdeltaR_ptgreat350","SS M_{#Delta}^{R} (GeV)",1300,0,13000);

  jets_magg_largeR = book(m_name,"jets_magg_largeR","Mass Jets (GeV)",1300,0,13000);

  jet_multiplicity = book(m_name,"jet_multiplicity","Number of Jets",35,0,35);

  magg_jets = book(m_name,"magg_jets","Mass of Jets (GeV)",1300,0,13000);

  //comparison plots for gg_mass
  gg_magg_vs_leadJetPt = book(m_name,"gg_magg_vs_leadJetPt","lead Jet Pt (GeV)",400,0,13000,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_leadJetEta= book(m_name,"gg_magg_vs_leadJetEta","lead Jet Eta (radians)",34,-3.4,3.4,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_leadJetPhi= book(m_name,"gg_magg_vs_leadJetPhi","lead Jet Phi (radians)",34,-3.4,3.4,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_leadJetMass= book(m_name,"gg_magg_vs_leadJetMass","lead Jet Mass (GeV)",1300,0,13000,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_leadJetEnergy= book(m_name,"gg_magg_vs_leadJetEnergy","lead Jet E (GeV)",1300,0,13000,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_leadJetRapidity= book(m_name,"gg_magg_vs_leadJetRapidity","lead Jet Rapidity",25,-2.5,2.5,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_2ndJetPt = book(m_name,"gg_magg_vs_2ndJetPt","2nd Jet Pt (GeV)",1300,0,13000,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_2ndJetEta= book(m_name,"gg_magg_vs_2ndJetEta","2nd Jet Eta (radians)",34,-3.4,3.4,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_2ndJetPhi= book(m_name,"gg_magg_vs_2ndJetPhi","2nd Jet Phi (radians)",34,-3.4,3.4,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_2ndJetMass= book(m_name,"gg_magg_vs_2ndJetMass","2nd Jet Mass (GeV)",1300,0,13000,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_2ndJetEnergy= book(m_name,"gg_magg_vs_2ndJetEnergy","2nd Jet E (GeV)",400,0,6500,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_2ndJetRapidity= book(m_name,"gg_magg_vs_2ndJetRapidity","2nd Jet Rapidity",25,-2.5,2.5,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_jetMultiplicity= book(m_name,"gg_magg_vs_JetMultiplicity","Jet Multiplicity",12,4,15,"SS mass (GeV)",400,1000,13000);
  gg_magg_vs_HT = book(m_name,"gg_magg_vs_HT","HT (GeV)",1300,400,13000,"SS mass (GeV)",1300,1000,13000);
  gg_magg_vs_Meff = book(m_name,"gg_magg_vs_Meff","M_{eff} (GeV)", 1300,500,13000,"SS mass (GeV)",400,1000,13000);

  //comparison plots gg_invgamma
  gg_invgamma_vs_MET = book(m_name,"gg_invgamma_vs_MET","MET (GeV)",1300,50,13000,"Inv Gamma",20,0,1);
  gg_invgamma_vs_Meff = book(m_name,"gg_invgamma_vs_Meff","M_{eff} (GeV)",1300,500,13000,"Inv Gamma", 20,0,1);

  //comparison plots gg_gamma
  gg_gamma_vs_Meff = book(m_name,"gg_gamma_Meff","M_{eff} (GeV)",1300,0,13000,"SS Gamma",20,1,4);
  gg_gamma_vs_MET = book(m_name,"gg_gamma_MET","MET (GeV)",1300,50,13000,"SS Gamma",20,1,4);

  //comparison plots for gg_mdeltaR
  gg_mdeltaR_vs_leadJetPt = book(m_name,"gg_mdeltaR_vs_leadJetPt","lead Jet Pt (GeV)",1300,0,13000,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_leadJetEta= book(m_name,"gg_mdeltaR_vs_leadJetEta","lead Jet Eta (radians)",34,-3.4,3.4,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_leadJetPhi= book(m_name,"gg_mdeltaR_vs_leadJetPhi","lead Jet Phi (radians)",34,-3.4,3.4,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_leadJetMass= book(m_name,"gg_mdeltaR_vs_leadJetMass","lead Jet Mass (GeV)",1300,0,13000,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_leadJetEnergy= book(m_name,"gg_mdeltaR_vs_leadJetEnergy","lead Jet E (GeV)",1300,0,13000,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_leadJetRapidity= book(m_name,"gg_mdeltaR_vs_leadJetRapidity","lead Jet Rapidity",25,-2.5,2.5,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_2ndJetPt = book(m_name,"gg_mdeltaR_vs_2ndJetPt","2nd Jet Pt (GeV)",1300,0,13000,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_2ndJetEta= book(m_name,"gg_mdeltaR_vs_2ndJetEta","2nd Jet Eta (radians)",34,-3.4,3.4,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_2ndJetPhi= book(m_name,"gg_mdeltaR_vs_2ndJetPhi","2nd Jet Phi (radians)",34,-3.4,3.4,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_2ndJetMass= book(m_name,"gg_mdeltaR_vs_2ndJetMass","2nd Jet Mass (GeV)",1300,0,13000,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_2ndJetEnergy= book(m_name,"gg_mdeltaR_vs_2ndJetEnergy","2nd Jet E (GeV)",1300,0,13000,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_2ndJetRapidity= book(m_name,"gg_mdeltaR_vs_2ndJetRapidity","2nd Jet Rapidity",25,-2.5,2.5,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_jetMultiplicity= book(m_name,"gg_mdeltaR_vs_JetMultiplicity","Jet Multiplicity",12,4,15,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_HT = book(m_name,"gg_mdeltaR_vs_HT","HT (GeV)",1300,50,13000,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);
  gg_mdeltaR_vs_Meff = book(m_name,"gg_mdeltaR_vs_Meff","M_{eff} (GeV)",1300,500,13000,"SS M_{#Delta}^{R} (GeV)",1300,0,13000);

  //comparison plots for gg_costheta
  gg_costheta_vs_leadJetEta = book(m_name,"gg_costheta_leadJetEta","lead Jet Eta",34,-3.4,3.4,"SS cos(#theta)",50,-1,1);
  gg_costheta_vs_leadJetPhi = book(m_name,"gg_costheta_leadJetPhi","lead Jet Phi",34,-3.4,3.4,"SS cos(#theta)",50,-1,1);
  gg_costheta_vs_leadJetRapidity = book(m_name,"gg_costheta_leadJetRapidity","lead Jet Rapidity",25,-2.5,2.5,"SS cos(#theta)",50,-1,1);
  gg_costheta_vs_2ndJetEta = book(m_name,"gg_costheta_2ndJetEta","2nd Jet Eta",34,-3.4,3.4,"SS cos(#theta)",50,-1,1);
  gg_costheta_vs_2ndJetPhi = book(m_name,"gg_costheta_2ndJetPhi","2nd Jet Phi",34,-3.4,3.4,"SS cos(#theta)",50,-1,1);
  gg_costheta_vs_2ndJetRapidity = book(m_name,"gg_costheta_2ndJetRapidity","2nd Jet Rapidity",25,-2.5,2.5,"SS cos(#theta)",50,-1,1);

  //comparison plots for gg_dphivis
  gg_dphivis_vs_MET = book(m_name,"MET_vs_gg_dphivis","#Delta #phi_{vis} (rads.)",50,0,3.2,"MET (GeV)",1300,0,13000);

  //comparison plots for gg_dphidecayangle
  gg_dphidecayangle_vs_MET = book(m_name,"MET_vs_gg_dphidecayangle","#Delta #phi_{decay} (rads.)",50,0,3.2,"MET (GeV)",50,0,1000);
  gg_dphivis_vs_METphi = book(m_name,"gg_dphivis_vs_METphi","MET #phi",50,0,3.2,"SS #Delta#phi_{vis} (radians)",50,0,3.2);
  gg_dphidecayangle_vs_METphi = book(m_name,"gg_dphidecayangle_vs_METphi","MET #phi",50,0,3.2,"SS #Delta#phi_{decay} (radians)",50,0,3.2);

  gg_mdeltaR_vs_METsig = book(m_name,"gg_mdeltaR_vs_METsig","M_{#Delta}^{R} (GeV)",1300,0,13000,"MET sig",50,0,20);

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::RazorVariableHists::execute(const xAOD::EventInfo* eventInfo, const xAOD::MissingET* met, const xAOD::JetContainer* jets, const xAOD::JetContainer* jets_largeR, const xAOD::MuonContainer* in_muons, const xAOD::ElectronContainer* in_electrons, float eventWeight){

  static VD::accessor_t<float> GG_magg_acc("GG_mass");
  static VD::accessor_t<float> GG_invgamma_acc("GG_invgamma");
  static VD::accessor_t<float> GG_dphivis_acc("GG_dphivis");
  static VD::accessor_t<float> GG_costheta_acc("GG_costheta");
  static VD::accessor_t<float> GG_dphidecayangle_acc("GG_dphidecayangle");
  static VD::accessor_t<float> GG_mdeltaR_acc("GG_mdeltaR");
  static VD::accessor_t<float> Ga_magg_acc("Ga_mass");
  static VD::accessor_t<float> Gb_magg_acc("Gb_mass");
  static VD::accessor_t<float> Ga_costheta_acc("Ga_costheta");
  static VD::accessor_t<float> Gb_costheta_acc("Gb_costheta");
  static VD::accessor_t<float> Ia1_depth_acc("Ia1_depth");
  static VD::accessor_t<float> Ib1_depth_acc("Ib1_depth");
  static VD::accessor_t<float> Va1_n_acc("Va1_n");
  static VD::accessor_t<float> Vb1_n_acc("Vb1_n");

  gg_mass->           Fill( GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_invgamma->       Fill( GG_invgamma_acc(*eventInfo),eventWeight);
  gg_dphivis->        Fill( GG_dphivis_acc(*eventInfo), eventWeight);
  gg_costheta->       Fill( GG_costheta_acc(*eventInfo), eventWeight);
  gg_dphidecayangle-> Fill( GG_dphidecayangle_acc(*eventInfo), eventWeight);
  //gg_visshape->       Fill( GG_decor(*eventInfo).GetVisibleShape(), eventWeight);
  gg_mdeltaR->        Fill( GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ga_mass->           Fill( Ga_magg_acc(*eventInfo)/1000., eventWeight);
  ga_costheta->       Fill( Ga_costheta_acc(*eventInfo), eventWeight);
  gb_mass->           Fill( Gb_magg_acc(*eventInfo)/1000., eventWeight);
  gb_costheta->       Fill( Gb_costheta_acc(*eventInfo), eventWeight);
  ia1_depth->          Fill( Ia1_depth_acc(*eventInfo), eventWeight);
  ib1_depth->          Fill( Ib1_depth_acc(*eventInfo), eventWeight);
  va1_n->      Fill( Va1_n_acc(*eventInfo), eventWeight);
  vb1_n->      Fill( Vb1_n_acc(*eventInfo), eventWeight);

  gg_abs_costheta->   Fill( std::fabs(GG_costheta_acc(*eventInfo)), eventWeight);
  ga_abs_costheta->   Fill( std::fabs(Ga_costheta_acc(*eventInfo)), eventWeight);
  gb_abs_costheta->   Fill( std::fabs(Gb_costheta_acc(*eventInfo)), eventWeight);


  if((GG_mdeltaR_acc(*eventInfo)/1000.)<250)
    gg_mdeltaR_ptless250->Fill(GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  else if ((GG_mdeltaR_acc(*eventInfo)/1000.)<350)
    gg_mdeltaR_pt250to350->Fill(GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  else
    gg_mdeltaR_ptgreat350->Fill(GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);

  float leadingJetPt(0),
        leadingJetEta(0),
        leadingJetPhi(0),
        leadingJetMass(0),
        leadingJetE(0),
        leadingJetRapidity(0),
        subleadingJetPt(0),
        subleadingJetEta(0),
        subleadingJetPhi(0),
        subleadingJetMass(0),
        subleadingJetE(0),
        subleadingJetRapidity(0);
  float jets_mass = 0;
  //jets work
  int i = 0;
  for ( const auto &jet : *jets ) {
    i++;
    jets_mass += jet->m()/1.e3;
    if (i==1) {        // leading jet
      leadingJetPt       = jet->pt()/1.e3;
      leadingJetEta      = jet->eta();
      leadingJetPhi      = jet->phi();
      leadingJetMass     = jet->m()/1.e3;
      leadingJetE        = jet->e()/1.e3;
      leadingJetRapidity = jet->rapidity();
    }
    if (i==2) {
      subleadingJetPt       = jet->pt()/1.e3;
      subleadingJetEta      = jet->eta();
      subleadingJetPhi      = jet->phi();
      subleadingJetMass     = jet->m()/1.e3;
      subleadingJetE        = jet->e()/1.e3;
      subleadingJetRapidity = jet->rapidity();
    }
  }
  magg_jets->Fill(jets_mass);

  float magg_largeR_jets =0;
  //jets_magg_largeR
  for (const auto &largeRjet : *jets_largeR)
      magg_largeR_jets += largeRjet->m()/1.e3;

  float multiplicity = jets->size();
  float m_eff = VD::Meff_inclusive(met, jets, in_muons, in_electrons);
  float m_ht = VD::HT(jets, in_muons, in_electrons);

  jets_magg_largeR->                Fill(magg_largeR_jets);
  jet_multiplicity->                Fill(multiplicity, eventWeight);

  //GG_mass 2D plots
  gg_magg_vs_leadJetPt->            Fill(leadingJetPt,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_leadJetEta->           Fill(leadingJetPhi,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_leadJetPhi->           Fill(leadingJetEta,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_leadJetMass->          Fill(leadingJetMass,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_leadJetEnergy->        Fill(leadingJetE,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_leadJetRapidity->      Fill(leadingJetRapidity,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_2ndJetPt->             Fill(subleadingJetPt,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_2ndJetEta->            Fill(subleadingJetPhi,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_2ndJetPhi->            Fill(subleadingJetEta,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_2ndJetMass->           Fill(subleadingJetMass,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_2ndJetEnergy->         Fill(subleadingJetE,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_2ndJetRapidity->       Fill(subleadingJetRapidity,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_jetMultiplicity->      Fill(multiplicity,GG_magg_acc(*eventInfo)/1000.,eventWeight);
  gg_magg_vs_HT->                   Fill(m_ht/1000.,GG_magg_acc(*eventInfo)/1000., eventWeight);
  gg_magg_vs_Meff->                 Fill(m_eff/1000.,GG_magg_acc(*eventInfo)/1000., eventWeight);

  //GG_invgamma 2D plots
  gg_invgamma_vs_MET->              Fill(met->met()/1.e3, GG_invgamma_acc(*eventInfo), eventWeight);
  gg_invgamma_vs_Meff->             Fill(m_eff/1.e3,GG_invgamma_acc(*eventInfo),eventWeight);

  //GG_gamma 2D plots
  gg_gamma_vs_MET->                 Fill(met->met()/1.e3,1./GG_invgamma_acc(*eventInfo), eventWeight);
  gg_gamma_vs_Meff->                Fill(m_eff/1.e3,1./GG_invgamma_acc(*eventInfo),eventWeight);

  float met_significance(0.0);
  float ht(0.0);

  for(int i=0; i < std::min<int>(multiplicity, jets->size()); i++){
    const auto &jet = jets->at(i);
    if(jet->pt()/1.e3 < 30.) continue;
    ht += jet->pt();
  }
  if(ht > 0) met_significance = (met->met()/1.e3)/sqrt(ht/1.e3);

  gg_mdeltaR_vs_METsig->Fill(GG_mdeltaR_acc(*eventInfo)/1000., met_significance, eventWeight);

  //GG_mdeltaR 2D plots
  gg_mdeltaR_vs_leadJetPt->         Fill(leadingJetPt,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_leadJetEta->        Fill(leadingJetPhi,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_leadJetPhi->        Fill(leadingJetEta,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_leadJetMass->       Fill(leadingJetMass,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_leadJetEnergy->     Fill(leadingJetE,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_leadJetRapidity->   Fill(leadingJetRapidity,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_2ndJetPt->          Fill(subleadingJetPt,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_2ndJetEta->         Fill(subleadingJetPhi,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_2ndJetPhi->         Fill(subleadingJetEta,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_2ndJetMass->        Fill(subleadingJetMass,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_2ndJetEnergy->      Fill(subleadingJetE,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_2ndJetRapidity->    Fill(subleadingJetRapidity,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_jetMultiplicity->   Fill(multiplicity,GG_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  gg_mdeltaR_vs_HT->                Fill(m_ht/1000.,GG_mdeltaR_acc(*eventInfo)/1000.,eventWeight);
  gg_mdeltaR_vs_Meff->              Fill(m_eff/1.e3,GG_mdeltaR_acc(*eventInfo)/1000.,eventWeight);

  //GG_Costheta 2D plots
  gg_costheta_vs_leadJetEta->       Fill(leadingJetEta,GG_costheta_acc(*eventInfo),eventWeight);
  gg_costheta_vs_leadJetPhi->       Fill(leadingJetPhi,GG_costheta_acc(*eventInfo),eventWeight);
  gg_costheta_vs_leadJetRapidity->  Fill(leadingJetRapidity,GG_costheta_acc(*eventInfo),eventWeight);
  gg_costheta_vs_2ndJetEta->        Fill(subleadingJetEta,GG_costheta_acc(*eventInfo),eventWeight);
  gg_costheta_vs_2ndJetPhi->        Fill(subleadingJetPhi,GG_costheta_acc(*eventInfo),eventWeight);
  gg_costheta_vs_2ndJetRapidity->   Fill(subleadingJetRapidity,GG_costheta_acc(*eventInfo),eventWeight);

  //GG_dphivis 2D plots
  gg_dphivis_vs_MET->               Fill(GG_dphivis_acc(*eventInfo),met->met()/1.e3, eventWeight);
  gg_dphivis_vs_METphi->            Fill(met->phi(),GG_dphivis_acc(*eventInfo),eventWeight);

  //GG_dphidecayangle 2D plots
  gg_dphidecayangle_vs_MET->        Fill(GG_dphidecayangle_acc(*eventInfo),met->met()/1.e3, eventWeight);
  gg_dphidecayangle_vs_METphi->     Fill(met->phi(),GG_dphidecayangle_acc(*eventInfo),eventWeight);


  return StatusCode::SUCCESS;
}

