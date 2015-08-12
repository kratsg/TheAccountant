#include <TheAccountant/RazorVariableHists.h>
#include "xAODEventInfo/EventInfo.h"
#include "TheAccountant/VariableDefinitions.h"

using namespace std;

namespace RF = RestFrames;
namespace VD = VariableDefinitions;

TheAccountant::RazorVariableHists::RazorVariableHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::RazorVariableHists::~RazorVariableHists () {}

StatusCode TheAccountant::RazorVariableHists::initialize() {

  //simple plots of variables
  ss_mass = book(m_name,"ss_mass", "SS mass [GeV]", 50, 1000, 13000.);
<<<<<<< HEAD
  ss_invgamma = book(m_name, "ss_invgamma", "SS InvGamma [units?]", 20, 0, 1);
=======
  ss_invgamma = book(m_name, "ss_invgamma", "SS InvGamma [units?]", 20, 0, 1); 
>>>>>>> 9565015e8273730a6527ea185604feec23cd162f
  ss_dphivis = book(m_name, "ss_dphivis", "SS dPhiIvs [units?]", 32, 0, 3.2);
  ss_costheta = book(m_name, "ss_costheta", "SS CosTheta", 100, -1., 1.);
  ss_abs_costheta = book(m_name, "ss_abs_costheta","|cos(#theta)|",25,0,1);
  s1_abs_costheta = book(m_name, "s1_abs_costheta","|cos(#theta)|",25,0,1);
  s2_abs_costheta = book(m_name, "s2_abs_costheta","|cos(#theta)|",25,0,1);


  ss_dphidecayangle = book(m_name, "ss_dphidecayangle", "SS dPhiDecayAngle [radians?]",32, 0, 3.2);
  ss_visshape = book(m_name, "ss_visshape", "SS VisShape [units?]", 100, 0, 1.);
  ss_mdeltaR = book(m_name, "ss_mdeltaR", "SS MDeltaR [GeV?]", 50, 0, 1500);
  s1_mass = book(m_name, "s1_mass", "S1 Mass [GeV]", 50, 0, 6500);
  s1_costheta = book(m_name, "s1_costheta", "S1 CosTheta", 100, -1., 1.);
  s2_mass = book(m_name, "s2_mass", "S2 Mass [GeV]", 50, 0, 6500.);
  s2_costheta = book(m_name, "s2_costheta", "S2 CosTheta", 100,-1., 1.);
  i1_depth = book(m_name, "i1_depth", "I1 Depth [units?]", 4, 0, 4);
  i2_depth = book(m_name, "i2_depth", "I2 Depth [units?]", 4, 0, 4);
  v1_nelements = book(m_name, "v1_nelements", "V1 NElements", 15, 0, 15);
  v2_nelements = book(m_name, "v2_nelements", "V2 NElements", 15, 0, 15);

  ss_mdeltaR_ptless250 = book(m_name,"ss_mdeltaR_ptless250","SS M_{#Delta}^{R} (GeV)",50,0,1500);
  ss_mdeltaR_pt250to350 = book(m_name,"ss_mdeltaR_pt250to350","SS M_{#Delta}^{R} (GeV)",50,0,1500);
  ss_mdeltaR_ptgreat350 = book(m_name,"ss_mdeltaR_ptgreat350","SS M_{#Delta}^{R} (GeV)",50,0,1500);

  jets_mass_largeR = book(m_name,"jets_mass_largeR","Mass Jets (GeV)",50,0,1500);

  jet_multiplicity = book(m_name,"jet_multiplicity","Number of Jets",16,0,16);

  mass_jets = book(m_name,"mass_jets","Mass of Jets (GeV)",50,0,1500);

  //comparison plots for ss_mass
  ss_mass_vs_leadJetPt = book(m_name,"ss_mass_vs_leadJetPt","lead Jet Pt (GeV)",50,0,1500,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetEta= book(m_name,"ss_mass_vs_leadJetEta","lead Jet Eta (radians)",34,-3.4,3.4,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetPhi= book(m_name,"ss_mass_vs_leadJetPhi","lead Jet Phi (radians)",34,-3.4,3.4,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetMass= book(m_name,"ss_mass_vs_leadJetMass","lead Jet Mass (GeV)",50,0,200,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetEnergy= book(m_name,"ss_mass_vs_leadJetEnergy","lead Jet E (GeV)",50,0,4000,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetRapidity= book(m_name,"ss_mass_vs_leadJetRapidity","lead Jet Rapidity",25,-2.5,2.5,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_2ndJetPt = book(m_name,"ss_mass_vs_2ndJetPt","2nd Jet Pt (GeV)",50,0,1500,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_2ndJetEta= book(m_name,"ss_mass_vs_2ndJetEta","2nd Jet Eta (radians)",34,-3.4,3.4,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_2ndJetPhi= book(m_name,"ss_mass_vs_2ndJetPhi","2nd Jet Phi (radians)",34,-3.4,3.4,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_2ndJetMass= book(m_name,"ss_mass_vs_2ndJetMass","2nd Jet Mass (GeV)",50,0,200,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_2ndJetEnergy= book(m_name,"ss_mass_vs_2ndJetEnergy","2nd Jet E (GeV)",50,0,4000,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_2ndJetRapidity= book(m_name,"ss_mass_vs_2ndJetRapidity","2nd Jet Rapidity",25,-2.5,2.5,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_jetMultiplicity= book(m_name,"ss_mass_vs_JetMultiplicity","Jet Multiplicity",12,4,15,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_HT = book(m_name,"ss_mass_vs_HT","HT (GeV)",50,400,3000,"SS mass (GeV)",50,1000,13000);
  ss_mass_vs_Meff = book(m_name,"ss_mass_vs_Meff","M_{eff} (GeV)", 50,500,3000,"SS mass (GeV)",50,1000,13000);

  //comparison plots ss_invgamma
  ss_invgamma_vs_MET = book(m_name,"ss_invgamma_vs_MET","MET (GeV)",50,40,300,"Inv Gamma",20,0,1);
  ss_invgamma_vs_Meff = book(m_name,"ss_invgamma_vs_Meff","M_{eff} (GeV)",50,500,3000,"Inv Gamma", 20,0,1);

  //comparison plots ss_gamma
  ss_gamma_vs_Meff = book(m_name,"ss_gamma_Meff","M_{eff} (GeV)",50,0,3000,"SS Gamma",20,1,4);
  ss_gamma_vs_MET = book(m_name,"ss_gamma_MET","MET (GeV)",50,40,300,"SS Gamma",20,1,4);

  //comparison plots for ss_mdeltaR
  ss_mdeltaR_vs_leadJetPt = book(m_name,"ss_mdeltaR_vs_leadJetPt","lead Jet Pt (GeV)",30,0,1500,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetEta= book(m_name,"ss_mdeltaR_vs_leadJetEta","lead Jet Eta (radians)",34,-3.4,3.4,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetPhi= book(m_name,"ss_mdeltaR_vs_leadJetPhi","lead Jet Phi (radians)",34,-3.4,3.4,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetMass= book(m_name,"ss_mdeltaR_vs_leadJetMass","lead Jet Mass (GeV)",50,0,200,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetEnergy= book(m_name,"ss_mdeltaR_vs_leadJetEnergy","lead Jet E (GeV)",50,0,4000,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetRapidity= book(m_name,"ss_mdeltaR_vs_leadJetRapidity","lead Jet Rapidity",25,-2.5,2.5,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_2ndJetPt = book(m_name,"ss_mdeltaR_vs_2ndJetPt","2nd Jet Pt (GeV)",30,0,1500,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_2ndJetEta= book(m_name,"ss_mdeltaR_vs_2ndJetEta","2nd Jet Eta (radians)",34,-3.4,3.4,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_2ndJetPhi= book(m_name,"ss_mdeltaR_vs_2ndJetPhi","2nd Jet Phi (radians)",34,-3.4,3.4,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_2ndJetMass= book(m_name,"ss_mdeltaR_vs_2ndJetMass","2nd Jet Mass (GeV)",50,0,200,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_2ndJetEnergy= book(m_name,"ss_mdeltaR_vs_2ndJetEnergy","2nd Jet E (GeV)",50,0,4000,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_2ndJetRapidity= book(m_name,"ss_mdeltaR_vs_2ndJetRapidity","2nd Jet Rapidity",25,-2.5,2.5,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_jetMultiplicity= book(m_name,"ss_mdeltaR_vs_JetMultiplicity","Jet Multiplicity",12,4,15,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_HT = book(m_name,"ss_mdeltaR_vs_HT","HT (GeV)",50,40,3000,"SS M_{#Delta}^{R} (GeV)",50,0,800);
  ss_mdeltaR_vs_Meff = book(m_name,"ss_mdeltaR_vs_Meff","M_{eff} (GeV)",50,500,3000,"SS M_{#Delta}^{R} (GeV)",50,0,800);

  //comparison plots for ss_costheta
  ss_costheta_vs_leadJetEta = book(m_name,"ss_costheta_leadJetEta","lead Jet Eta",34,-3.4,3.4,"SS cos(#theta)",50,-1,1);
  ss_costheta_vs_leadJetPhi = book(m_name,"ss_costheta_leadJetPhi","lead Jet Phi",34,-3.4,3.4,"SS cos(#theta)",50,-1,1);
  ss_costheta_vs_leadJetRapidity = book(m_name,"ss_costheta_leadJetRapidity","lead Jet Rapidity",25,-2.5,2.5,"SS cos(#theta)",50,-1,1);
  ss_costheta_vs_2ndJetEta = book(m_name,"ss_costheta_2ndJetEta","2nd Jet Eta",34,-3.4,3.4,"SS cos(#theta)",50,-1,1);
  ss_costheta_vs_2ndJetPhi = book(m_name,"ss_costheta_2ndJetPhi","2nd Jet Phi",34,-3.4,3.4,"SS cos(#theta)",50,-1,1);
  ss_costheta_vs_2ndJetRapidity = book(m_name,"ss_costheta_2ndJetRapidity","2nd Jet Rapidity",25,-2.5,2.5,"SS cos(#theta)",50,-1,1);

  //comparison plots for ss_dphivis
  ss_dphivis_vs_MET = book(m_name,"MET_vs_ss_dphivis","#Delta #phi_{vis} (rads.)",50,0,3.2,"MET (GeV)",50,0,1000);

  //comparison plots for ss_dphidecayangle
<<<<<<< HEAD
  ss_dphidecayangle_vs_MET = book(m_name,"MET_vs_ss_dphidecayangle","#Delta #phi_{decay} (rads.)",50,0,3.2,"MET (GeV)",50,0,1000);
=======
  ss_dphidecayangle_vs_MET = book(m_name,"MET_vs_ss_dphidecayangle","#Delta #phi_{decay} (rads.)",50,0,3.2,"MET (GeV)",50,0,1000); 
>>>>>>> 9565015e8273730a6527ea185604feec23cd162f

  ss_dphivis_vs_METphi = book(m_name,"ss_dphivis_vs_METphi","MET #phi",50,0,3.2,"SS #Delta#phi_{vis} (radians)",50,0,3.2);
  ss_dphidecayangle_vs_METphi = book(m_name,"ss_dphidecayangle_vs_METphi","MET #phi",50,0,3.2,"SS #Delta#phi_{decay} (radians)",50,0,3.2);

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::RazorVariableHists::execute(const xAOD::EventInfo* eventInfo, const xAOD::MissingET* met, const xAOD::JetContainer* jets, const xAOD::JetContainer* jets_largeR, const xAOD::MuonContainer* in_muons, const xAOD::ElectronContainer* in_electrons, float eventWeight){

<<<<<<< HEAD
  static SG::AuxElement::ConstAccessor<float> SS_mass_acc("SS_mass");
  static SG::AuxElement::ConstAccessor<float> SS_invgamma_acc("SS_invgamma");
  static SG::AuxElement::ConstAccessor<float> SS_dphivis_acc("SS_dphivis");
  static SG::AuxElement::ConstAccessor<float> SS_costheta_acc("SS_costheta");
  static SG::AuxElement::ConstAccessor<float> SS_dphidecayangle_acc("SS_dphidecayangle");
  static SG::AuxElement::ConstAccessor<float> SS_mdeltaR_acc("SS_mdeltaR");
  static SG::AuxElement::ConstAccessor<float> S1_mass_acc("S1_mass");
  static SG::AuxElement::ConstAccessor<float> S2_mass_acc("S2_mass");
  static SG::AuxElement::ConstAccessor<float> S1_costheta_acc("S1_costheta");
  static SG::AuxElement::ConstAccessor<float> S2_costheta_acc("S2_costheta");
  static SG::AuxElement::ConstAccessor<float> I1_depth_acc("I1_depth");
  static SG::AuxElement::ConstAccessor<float> I2_depth_acc("I2_depth");
  static SG::AuxElement::ConstAccessor<float> V1_nelements_acc("V1_nelements");
  static SG::AuxElement::ConstAccessor<float> V2_nelements_acc("V2_nelements");
=======
  SG::AuxElement::ConstAccessor<float> SS_mass_acc("SS_mass");
  SG::AuxElement::ConstAccessor<float> SS_invgamma_acc("SS_invgamma");
  SG::AuxElement::ConstAccessor<float> SS_dphivis_acc("SS_dphivis");
  SG::AuxElement::ConstAccessor<float> SS_costheta_acc("SS_costheta");
  SG::AuxElement::ConstAccessor<float> SS_dphidecayangle_acc("SS_dphidecayangle");
  SG::AuxElement::ConstAccessor<float> SS_mdeltaR_acc("SS_mdeltaR");
  SG::AuxElement::ConstAccessor<float> S1_mass_acc("S1_mass");
  SG::AuxElement::ConstAccessor<float> S2_mass_acc("S2_mass");
  SG::AuxElement::ConstAccessor<float> S1_costheta_acc("S1_costheta");
  SG::AuxElement::ConstAccessor<float> S2_costheta_acc("S2_costheta");
  SG::AuxElement::ConstAccessor<float> I1_depth_acc("I1_depth");
  SG::AuxElement::ConstAccessor<float> I2_depth_acc("I2_depth");
  SG::AuxElement::ConstAccessor<float> V1_nelements_acc("V1_nelements");
  SG::AuxElement::ConstAccessor<float> V2_nelements_acc("V2_nelements");
>>>>>>> 9565015e8273730a6527ea185604feec23cd162f

  ss_mass->           Fill( SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_invgamma->       Fill( SS_invgamma_acc(*eventInfo),eventWeight);
  ss_dphivis->        Fill( SS_dphivis_acc(*eventInfo), eventWeight);
  ss_costheta->       Fill( SS_costheta_acc(*eventInfo), eventWeight);
  ss_dphidecayangle-> Fill( SS_dphidecayangle_acc(*eventInfo), eventWeight);
  //ss_visshape->       Fill( SS_decor(*eventInfo).GetVisibleShape(), eventWeight);
  ss_mdeltaR->        Fill( SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  s1_mass->           Fill( S1_mass_acc(*eventInfo)/1000., eventWeight);
  s1_costheta->       Fill( S1_costheta_acc(*eventInfo), eventWeight);
  s2_mass->           Fill( S2_mass_acc(*eventInfo)/1000., eventWeight);
  s2_costheta->       Fill( S2_costheta_acc(*eventInfo), eventWeight);
  i1_depth->          Fill( I1_depth_acc(*eventInfo), eventWeight);
  i2_depth->          Fill( I2_depth_acc(*eventInfo), eventWeight);
  v1_nelements->      Fill( V1_nelements_acc(*eventInfo), eventWeight);
<<<<<<< HEAD
  v2_nelements->      Fill( V2_nelements_acc(*eventInfo), eventWeight);

  ss_abs_costheta->   Fill( std::fabs(SS_costheta_acc(*eventInfo)), eventWeight);
  s1_abs_costheta->   Fill( std::fabs(S1_costheta_acc(*eventInfo)), eventWeight);
  s2_abs_costheta->   Fill( std::fabs(S2_costheta_acc(*eventInfo)), eventWeight);


=======
  v2_nelements->      Fill( V2_nelements_acc(*eventInfo), eventWeight);      

  ss_abs_costheta->   Fill( abs(SS_costheta_acc(*eventInfo)), eventWeight);
  s1_abs_costheta->   Fill( abs(S1_costheta_acc(*eventInfo)), eventWeight);
  s2_abs_costheta->   Fill( abs(S2_costheta_acc(*eventInfo)), eventWeight);

  
>>>>>>> 9565015e8273730a6527ea185604feec23cd162f
  if((SS_mdeltaR_acc(*eventInfo)/1000.)<250)
    ss_mdeltaR_ptless250->Fill(SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  else if ((SS_mdeltaR_acc(*eventInfo)/1000.)<350)
    ss_mdeltaR_pt250to350->Fill(SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  else
    ss_mdeltaR_ptgreat350->Fill(SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);

<<<<<<< HEAD
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
=======
  float leadingJetPt, leadingJetEta, leadingJetPhi, leadingJetMass, leadingJetE, leadingJetRapidity, subleadingJetPt, subleadingJetEta, subleadingJetPhi, subleadingJetMass, subleadingJetE, subleadingJetRapidity;
>>>>>>> 9565015e8273730a6527ea185604feec23cd162f
  float jets_mass = 0;
  //jets work
  int i = 0;
  for ( const auto jet : *jets ) {
    i++;
    jets_mass += jet->m()/1.e3;
<<<<<<< HEAD
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
=======
    if (i==1)
      {        // leading jet
	leadingJetPt = jet->pt()/1.e3;
	leadingJetEta = jet->eta();
	leadingJetPhi = jet->phi();
	leadingJetMass = jet->m()/1.e3;
	leadingJetE = jet->e()/1.e3;
	leadingJetRapidity = jet->rapidity();
      }
    if (i==2)
      {
	subleadingJetPt = jet->pt()/1.e3;
	subleadingJetEta = jet->eta();
	subleadingJetPhi = jet->phi();
	subleadingJetMass = jet->m()/1.e3;
	subleadingJetE = jet->e()/1.e3;
	subleadingJetRapidity = jet->rapidity();
      }
>>>>>>> 9565015e8273730a6527ea185604feec23cd162f
  }
  mass_jets->Fill(jets_mass);

  float mass_largeR_jets =0;
<<<<<<< HEAD
  //jets_mass_largeR
  for (const auto largeRjet : *jets_largeR)
      mass_largeR_jets += largeRjet->m()/1.e3;

  float multiplicity = jets->size();
  float m_eff = VD::Meff(met, jets, jets->size(), in_muons, in_electrons);
  float m_ht = VD::HT(jets, in_muons, in_electrons);

  jets_mass_largeR->                Fill(mass_largeR_jets);
  jet_multiplicity->                Fill(multiplicity, eventWeight);

  //SS_mass 2D plots
  ss_mass_vs_leadJetPt->            Fill(leadingJetPt,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetEta->           Fill(leadingJetPhi,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetPhi->           Fill(leadingJetEta,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetMass->          Fill(leadingJetMass,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetEnergy->        Fill(leadingJetE,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetRapidity->      Fill(leadingJetRapidity,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetPt->             Fill(subleadingJetPt,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetEta->            Fill(subleadingJetPhi,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetPhi->            Fill(subleadingJetEta,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetMass->           Fill(subleadingJetMass,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetEnergy->         Fill(subleadingJetE,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetRapidity->       Fill(subleadingJetRapidity,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_jetMultiplicity->      Fill(multiplicity,SS_mass_acc(*eventInfo)/1000.,eventWeight);
  ss_mass_vs_HT->                   Fill(m_ht/1000.,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_Meff->                 Fill(m_eff/1000.,SS_mass_acc(*eventInfo)/1000., eventWeight);

  //SS_invgamma 2D plots
  ss_invgamma_vs_MET->              Fill(met->met()/1.e3, SS_invgamma_acc(*eventInfo), eventWeight);
  ss_invgamma_vs_Meff->             Fill(m_eff/1.e3,SS_invgamma_acc(*eventInfo),eventWeight);

  //SS_gamma 2D plots
  ss_gamma_vs_MET->                 Fill(met->met()/1.e3,1./SS_invgamma_acc(*eventInfo), eventWeight);
  ss_gamma_vs_Meff->                Fill(m_eff/1.e3,1./SS_invgamma_acc(*eventInfo),eventWeight);


  //SS_mdeltaR 2D plots
  ss_mdeltaR_vs_leadJetPt->         Fill(leadingJetPt,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetEta->        Fill(leadingJetPhi,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetPhi->        Fill(leadingJetEta,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetMass->       Fill(leadingJetMass,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetEnergy->     Fill(leadingJetE,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetRapidity->   Fill(leadingJetRapidity,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetPt->          Fill(subleadingJetPt,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetEta->         Fill(subleadingJetPhi,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetPhi->         Fill(subleadingJetEta,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetMass->        Fill(subleadingJetMass,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetEnergy->      Fill(subleadingJetE,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetRapidity->    Fill(subleadingJetRapidity,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_jetMultiplicity->   Fill(multiplicity,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_HT->                Fill(m_ht/1000.,SS_mdeltaR_acc(*eventInfo)/1000.,eventWeight);
  ss_mdeltaR_vs_Meff->              Fill(m_eff/1.e3,SS_mdeltaR_acc(*eventInfo)/1000.,eventWeight);

  //SS_Costheta 2D plots
  ss_costheta_vs_leadJetEta->       Fill(leadingJetEta,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_leadJetPhi->       Fill(leadingJetPhi,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_leadJetRapidity->  Fill(leadingJetRapidity,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_2ndJetEta->        Fill(subleadingJetEta,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_2ndJetPhi->        Fill(subleadingJetPhi,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_2ndJetRapidity->   Fill(subleadingJetRapidity,SS_costheta_acc(*eventInfo),eventWeight);

  //SS_dphivis 2D plots
  ss_dphivis_vs_MET->               Fill(SS_dphivis_acc(*eventInfo),met->met()/1.e3, eventWeight);
  ss_dphivis_vs_METphi->            Fill(met->phi(),SS_dphivis_acc(*eventInfo),eventWeight);

  //SS_dphidecayangle 2D plots
  ss_dphidecayangle_vs_MET->        Fill(SS_dphidecayangle_acc(*eventInfo),met->met()/1.e3, eventWeight);
  ss_dphidecayangle_vs_METphi->     Fill(met->phi(),SS_dphidecayangle_acc(*eventInfo),eventWeight);
=======
  //jets_mass_largeR 
  for (const auto largeRjet : *jets_largeR)
    {
      mass_largeR_jets += largeRjet->m()/1.e3;
    }
  jets_mass_largeR->Fill(mass_largeR_jets);

  float multiplicity = jets->size();
  jet_multiplicity->Fill(multiplicity, eventWeight);

  float m_eff = VD::Meff(met, jets, jets->size(), in_muons, in_electrons);
  float m_ht = VD::HT(jets, in_muons, in_electrons);

  //SS_mass 2D plots
  ss_mass_vs_leadJetPt->Fill(leadingJetPt,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetEta->Fill(leadingJetPhi,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetPhi->Fill(leadingJetEta,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetMass->Fill(leadingJetMass,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetEnergy->Fill(leadingJetE,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetRapidity->Fill(leadingJetRapidity,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetPt->Fill(subleadingJetPt,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetEta->Fill(subleadingJetPhi,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetPhi->Fill(subleadingJetEta,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetMass->Fill(subleadingJetMass,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetEnergy->Fill(subleadingJetE,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_2ndJetRapidity->Fill(subleadingJetRapidity,SS_mass_acc(*eventInfo)/1000., eventWeight); 
  ss_mass_vs_jetMultiplicity->Fill(multiplicity,SS_mass_acc(*eventInfo)/1000.,eventWeight);
  ss_mass_vs_HT->Fill(m_ht/1000.,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_Meff->Fill(m_eff/1000.,SS_mass_acc(*eventInfo)/1000., eventWeight);

  //SS_invgamma 2D plots
  ss_invgamma_vs_MET->Fill(met->met()/1.e3, SS_invgamma_acc(*eventInfo), eventWeight);
  ss_invgamma_vs_Meff->Fill(m_eff/1.e3,SS_invgamma_acc(*eventInfo),eventWeight);

  //SS_gamma 2D plots
  ss_gamma_vs_MET->Fill(met->met()/1.e3,1./SS_invgamma_acc(*eventInfo), eventWeight);
  ss_gamma_vs_Meff->Fill(m_eff/1.e3,1./SS_invgamma_acc(*eventInfo),eventWeight);
  

  //SS_mdeltaR 2D plots
  ss_mdeltaR_vs_leadJetPt->Fill(leadingJetPt,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetEta->Fill(leadingJetPhi,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetPhi->Fill(leadingJetEta,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetMass->Fill(leadingJetMass,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetEnergy->Fill(leadingJetE,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetRapidity->Fill(leadingJetRapidity,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetPt->Fill(subleadingJetPt,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetEta->Fill(subleadingJetPhi,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetPhi->Fill(subleadingJetEta,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetMass->Fill(subleadingJetMass,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetEnergy->Fill(subleadingJetE,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_2ndJetRapidity->Fill(subleadingJetRapidity,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_jetMultiplicity->Fill(multiplicity,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_HT->Fill(m_ht/1000.,SS_mdeltaR_acc(*eventInfo)/1000.,eventWeight);
  ss_mdeltaR_vs_Meff->Fill(m_eff/1.e3,SS_mdeltaR_acc(*eventInfo)/1000.,eventWeight);

  //SS_Costheta 2D plots
  ss_costheta_vs_leadJetEta->Fill(leadingJetEta,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_leadJetPhi->Fill(leadingJetPhi,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_leadJetRapidity->Fill(leadingJetRapidity,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_2ndJetEta->Fill(subleadingJetEta,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_2ndJetPhi->Fill(subleadingJetPhi,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_vs_2ndJetRapidity->Fill(subleadingJetRapidity,SS_costheta_acc(*eventInfo),eventWeight);

  //SS_dphivis 2D plots
  ss_dphivis_vs_MET->Fill(SS_dphivis_acc(*eventInfo),met->met()/1.e3, eventWeight);
  ss_dphivis_vs_METphi->Fill(met->phi(),SS_dphivis_acc(*eventInfo),eventWeight);

  //SS_dphidecayangle 2D plots                                                                                                             
  ss_dphidecayangle_vs_MET->Fill(SS_dphidecayangle_acc(*eventInfo),met->met()/1.e3, eventWeight);
  ss_dphidecayangle_vs_METphi->Fill(met->phi(),SS_dphidecayangle_acc(*eventInfo),eventWeight);


>>>>>>> 9565015e8273730a6527ea185604feec23cd162f

  return StatusCode::SUCCESS;
}

