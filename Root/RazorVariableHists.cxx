#include <TheAccountant/RazorVariableHists.h>
#include "xAODEventInfo/EventInfo.h"

using namespace std;

namespace RF = RestFrames;

TheAccountant::RazorVariableHists::RazorVariableHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::RazorVariableHists::~RazorVariableHists () {}

StatusCode TheAccountant::RazorVariableHists::initialize() {
  ss_mass = book(m_name,"ss_mass", "SS mass [GeV]", 50, 0, 5000.);
  ss_invgamma = book(m_name, "ss_invgamma", "SS InvGamma [units?]", 40, 0, 4); 
  ss_dphivis = book(m_name, "ss_dphivis", "SS dPhiIvs [units?]", 100, 0, 3.2);
  ss_costheta = book(m_name, "ss_costheta", "SS CosTheta", 100, -1., 1.);
  ss_dphidecayangle = book(m_name, "ss_dphidecayangle", "SS dPhiDecayAngle [radians?]",100, 0, 3.2);
  ss_visshape = book(m_name, "ss_visshape", "SS VisShape [units?]", 100, 0, 1.);
  ss_mdeltaR = book(m_name, "ss_mdeltaR", "SS MDeltaR [GeV?]", 50, 0, 2500);
  s1_mass = book(m_name, "s1_mass", "S1 Mass [GeV]", 50, 0, 1500);
  s1_costheta = book(m_name, "s1_costheta", "S1 CosTheta", 100, -1., 1.);
  s2_mass = book(m_name, "s2_mass", "S2 Mass [GeV]", 50, 0, 1500.);
  s2_costheta = book(m_name, "s2_costheta", "S2 CosTheta", 100,-1., 1.);
  i1_depth = book(m_name, "i1_depth", "I1 Depth [units?]", 4, 0, 4);
  i2_depth = book(m_name, "i2_depth", "I2 Depth [units?]", 4, 0, 4);
  v1_nelements = book(m_name, "v1_nelements", "V1 NElements", 10, 0, 10);
  v2_nelements = book(m_name, "v2_nelements", "V2 NElements", 10, 0, 10);

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::RazorVariableHists::execute(const xAOD::EventInfo* eventInfo, float eventWeight){

  //  SG::AuxElement::ConstAccessor< RF::RDecayFrame > SS_decor("SS");
  //SG::AuxElement::ConstAccessor< RF::RDecayFrame > S1_decor("S1");
  //SG::AuxElement::ConstAccessor< RF::RDecayFrame > S2_decor("S2");
  //SG::AuxElement::ConstAccessor< RF::RVisibleFrame > V1_decor("V1");
  //SG::AuxElement::ConstAccessor< RF::RVisibleFrame > V2_decor("V2");
  //SG::AuxElement::ConstAccessor< RF::RInvisibleFrame > I1_decor("I1");
  //SG::AuxElement::ConstAccessor< RF::RInvisibleFrame > I2_decor("I2");
  //SG::AuxElement::ConstAccessor< RF::CombinatoricGroup > VIS_decor("VIS");
  //SG::AuxElement::ConstAccessor< RF::InvisibleGroup > INV_decor("INV");

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


  //  ss_mass->           Fill( SS_decor(*eventInfo).GetMass()/1000., eventWeight);
  //ss_invgamma->       Fill( 1./SS_decor(*eventInfo).GetGammaInParentFrame(),eventWeight);
  //ss_dphivis->        Fill( SS_decor(*eventInfo).GetDeltaPhiBoostVisible(), eventWeight);
  //ss_costheta->       Fill( SS_decor(*eventInfo).GetCosDecayAngle(), eventWeight);
  //ss_dphidecayangle-> Fill( SS_decor(*eventInfo).GetDeltaPhiDecayAngle(), eventWeight);
  //ss_visshape->       Fill( SS_decor(*eventInfo).GetVisibleShape(), eventWeight);
  //ss_mdeltaR->        Fill( SS_decor(*eventInfo).GetVisibleShape()*SS_decor(*eventInfo).GetMass(), eventWeight);
  //s1_mass->           Fill( S1_decor(*eventInfo).GetMass()/1000., eventWeight);
  //s1_costheta->       Fill( S1_decor(*eventInfo).GetCosDecayAngle(), eventWeight);
  //s2_mass->           Fill( S2_decor(*eventInfo).GetMass()/1000., eventWeight);
  //s2_costheta->       Fill( S2_decor(*eventInfo).GetCosDecayAngle(), eventWeight);
  //i1_depth->          Fill( S1_decor(*eventInfo).GetFrameDepth(I1_decor(*eventInfo)), eventWeight);
  //i2_depth->          Fill( S2_decor(*eventInfo).GetFrameDepth(I2_decor(*eventInfo)), eventWeight);
  //v1_nelements->      Fill( VIS_decor(*eventInfo).GetNElementsInFrame(V1_decor(*eventInfo)), eventWeight);
  //v2_nelements->      Fill( VIS_decor(*eventInfo).GetNElementsInFrame(V2_decor(*eventInfo)), eventWeight);
  ///  float test = SS_mass_acc(*eventInfo);
  
  //std::cout << "Test: " << test << std::endl; // getting filled and has value
  // std::cout << "Event Weight: " << eventWeight << std::endl;
  //ss_mass->Fill(0); // breaks here! 
  //std::cout << "test" << std::endl;
  ss_mass->           Fill( SS_mass_acc(*eventInfo)/1000., eventWeight);
  ss_invgamma->       Fill( 1./SS_invgamma_acc(*eventInfo),eventWeight);
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

  return StatusCode::SUCCESS;
}

