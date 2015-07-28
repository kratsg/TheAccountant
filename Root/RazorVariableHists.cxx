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
  ss_invgamma = book(m_name, "ss_invgamma", "SS InvGamma [units?]", 20, 0, 1); 
  ss_dphivis = book(m_name, "ss_dphivis", "SS dPhiIvs [units?]", 100, 0, 3.2);
  ss_costheta = book(m_name, "ss_costheta", "SS CosTheta", 100, -1., 1.);
  ss_dphidecayangle = book(m_name, "ss_dphidecayangle", "SS dPhiDecayAngle [radians?]",100, 0, 3.2);
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

  //comparison plots
  ss_mass_vs_leadJetPt = book(m_name,"ss_mass_vs_leadJetPt","lead Jet Pt (GeV)",50,0,1500,"SS_mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetEta= book(m_name,"ss_mass_vs_leadJetEta","lead Jet Eta (radians)",34,-3.4,3.4,"SS_mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetPhi= book(m_name,"ss_mass_vs_leadJetPhi","lead Jet Phi (radians)",34,-3.4,3.4,"SS_mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetMass= book(m_name,"ss_mass_vs_leadJetMass","lead Jet Mass (GeV)",50,0,200,"SS_mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetEnergy= book(m_name,"ss_mass_vs_leadJetEnergy","lead Jet E (GeV)",50,0,4000,"SS_mass (GeV)",50,1000,13000);
  ss_mass_vs_leadJetRapidity= book(m_name,"ss_mass_vs_leadJetRapidity","lead Jet Rapidity",25,-2.5,2.5,"SS_mass (GeV)",50,1000,13000);
  ss_mass_vs_jetMultiplicity= book(m_name,"ss_mass_vs_JetMultiplicity","Jet Multiplicity",12,4,15,"SS_mass (GeV)",50,1000,13000);


  ss_invgamma_vs_MET = book(m_name,"ss_invgamma_vs_MET","MET (GeV)",100,0,1000,"Inv Gamma",20,0,1);
  
  ss_mdeltaR_vs_leadJetPt = book(m_name,"ss_mdeltaR_vs_leadJetPt","lead Jet Pt (GeV)",30,0,1500,"SS_mdeltaR (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetEta= book(m_name,"ss_mdeltaR_vs_leadJetEta","lead Jet Eta (radians)",34,-3.4,3.4,"SS_mdeltaR (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetPhi= book(m_name,"ss_mdeltaR_vs_leadJetPhi","lead Jet Phi (radians)",34,-3.4,3.4,"SS_mdeltaR (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetMass= book(m_name,"ss_mdeltaR_vs_leadJetMass","lead Jet Mass (GeV)",50,0,200,"SS_mdeltaR (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetEnergy= book(m_name,"ss_mdeltaR_vs_leadJetEnergy","lead Jet E (GeV)",50,0,4000,"SS_mdeltaR (GeV)",50,0,800);
  ss_mdeltaR_vs_leadJetRapidity= book(m_name,"ss_mdeltaR_vs_leadJetRapidity","lead Jet Rapidity",25,-2.5,2.5,"SS_mdeltaR (GeV)",50,0,800);
  ss_mdeltaR_vs_jetMultiplicity= book(m_name,"ss_mdeltaR_vs_JetMultiplicity","Jet Multiplicity",12,4,15,"SS_mdeltaR (GeV)",50,0,800);

  jet_multiplicity = book(m_name,"jet_multiplicity","Number of Jets",15,0,15);

  ss_mass_HT = book(m_name,"ss_mass_HT","HT (GeV)",100,0,3000,"SS_mass (GeV)",100,1000,13000);
  ss_mass_Meff = book(m_name,"ss_mass_Meff","Meff (GeV)", 100,0,3000,"SS_mass (GeV)",100,1000,13000);
  ss_invgamma_Meff = book(m_name,"ss_invgamma_Meff","Meff (GeV)",100,0,3000,"Inv Gamma", 20,0,1);

  ss_gamma_Meff = book(m_name,"ss_gamma_Meff","Meff (GeV)",50,0,3000,"ss_gamma",20,0,4);
  ss_gamma_MET = book(m_name,"ss_gamma_MET","MET (GeV)",50,0,1000,"Gamma",20,0,4);
  ss_costheta_leadJetEta = book(m_name,"ss_costheta_leadJetEta","lead Jet Eta",34,-3.4,3.4,"ss_costheta",50,-1,1);
  ss_costheta_leadJetPhi = book(m_name,"ss_costheta_leadJetPhi","lead Jet Phi",34,-3.4,3.4,"ss_costheta",50,-1,1);
  ss_costheta_leadJetRapidity = book(m_name,"ss_costheta_leadJetRapidity","lead Jet Rapidity",25,-2.5,2.5,"ss_costheta",50,-1,1);


  //ss_mass_invgamma = book(m_name,"ss_mass_invgamma","Inv Gamma",20,0,1.,"SS Mass (GeV)",100,1000,13000);
  //ss_mdeltaR_invgamma = book(m_name,"ss_mdeltaR_invgamma","Inv Gamma",20,0,1.,"SS mDeltaR (GeV)",75,0,1500);

  ss_dphidecayangle_vs_MET = book(m_name,"ss_dphidecayangle_vs_MET","dphidecayangle (rads.)",100,0,3.2,"MET (GeV)",100,0,1000); 

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::RazorVariableHists::execute(const xAOD::EventInfo* eventInfo, const xAOD::MissingET* met, const xAOD::JetContainer* jets, const xAOD::MuonContainer* in_muons, const xAOD::ElectronContainer* in_electrons, float eventWeight){

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




  //RF::RDecayFrame       SS(SS_decor(*eventInfo));
  //RF::RDecayFrame       S1(S1_decor(*eventInfo));
  //RF::RDecayFrame       S2(S2_decor(*eventInfo));
  //RF::RVisibleFrame     V1(V1_decor(*eventInfo));
  //RF::RVisibleFrame     V2(V2_decor(*eventInfo));
  //RF::RInvisibleFrame   I1(I1_decor(*eventInfo));
  //RF::RInvisibleFrame   I2(I2_decor(*eventInfo));

  //RF::CombinatoricGroup VIS(VIS_decor(*eventInfo));
  //RF::InvisibleGroup    INV(INV_decor(*eventInfo));


  //  ss_mass->           Fill( SS_decor(*eventInfo).GetMass()/1000
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
  v2_nelements->      Fill( V2_nelements_acc(*eventInfo), eventWeight);      

  float leadingJetPt, leadingJetEta, leadingJetPhi, leadingJetMass, leadingJetE, leadingJetRapidity;

  //jets work
  int i = 0;
  for ( const auto jet : *jets ) {
    i++;
    if (i==1)
      {        // leading jet
	leadingJetPt = jet->pt()/1.e3;
	leadingJetEta = jet->eta();
	leadingJetPhi = jet->phi();
	leadingJetMass = jet->m()/1.e3;
	leadingJetE = jet->e()/1.e3;
	leadingJetRapidity = jet->rapidity();
      }
  }
  float multiplicity = jets->size();
  jet_multiplicity->Fill(multiplicity, eventWeight);

  ss_mass_vs_leadJetPt->Fill(leadingJetPt,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetEta->Fill(leadingJetPhi,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetPhi->Fill(leadingJetEta,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetMass->Fill(leadingJetMass,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetEnergy->Fill(leadingJetE,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_leadJetRapidity->Fill(leadingJetRapidity,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_vs_jetMultiplicity->Fill(multiplicity,SS_mass_acc(*eventInfo)/1000.,eventWeight);

  ss_invgamma_vs_MET->Fill(met->met()/1.e3, SS_invgamma_acc(*eventInfo), eventWeight);

  


  ss_mdeltaR_vs_leadJetPt->Fill(leadingJetPt,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetEta->Fill(leadingJetPhi,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetPhi->Fill(leadingJetEta,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetMass->Fill(leadingJetMass,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetEnergy->Fill(leadingJetE,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_leadJetRapidity->Fill(leadingJetRapidity,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);
  ss_mdeltaR_vs_jetMultiplicity->Fill(multiplicity,SS_mdeltaR_acc(*eventInfo)/1000., eventWeight);

  //ss_mass_invgamma->Fill(SS_invgamma_acc(*eventInfo),SS_mass_acc(*eventInfo)/1.e3, eventWeight);
  //ss_mdeltaR_invgamma->Fill(SS_invgamma_acc(*eventInfo),SS_mdeltaR_acc(*eventInfo)/1.e3, eventWeight);

  ss_dphidecayangle_vs_MET->Fill(SS_dphidecayangle_acc(*eventInfo),met->met()/1.e3, eventWeight);



  float m_eff = VD::Meff(met, jets, jets->size(), in_muons, in_electrons);
  float m_ht = VD::HT(jets, in_muons, in_electrons);

  ss_mass_HT->Fill(m_ht/1000.,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_mass_Meff->Fill(m_eff/1000.,SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_invgamma_Meff->Fill(m_eff/1000.,SS_invgamma_acc(*eventInfo), eventWeight);

  ss_gamma_Meff->Fill(m_eff/1000.,1./SS_invgamma_acc(*eventInfo),eventWeight);
  ss_gamma_MET->Fill(met->met()/1.e3,1./SS_invgamma_acc(*eventInfo),eventWeight);

  ss_costheta_leadJetEta->Fill(leadingJetEta,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_leadJetPhi->Fill(leadingJetPhi,SS_costheta_acc(*eventInfo),eventWeight);
  ss_costheta_leadJetRapidity->Fill(leadingJetRapidity,SS_costheta_acc(*eventInfo),eventWeight);

  return StatusCode::SUCCESS;
}

