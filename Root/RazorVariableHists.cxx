#include <TheAccountant/RazorVariableHists.h>
#include "xAODEventInfo/EventInfo.h"

namespace RF = RestFrames;

TheAccountant::RazorVariableHists::RazorVariableHists (std::string name) :
  HistogramManager(name, "")
{
}

TheAccountant::RazorVariableHists::~RazorVariableHists () {}

StatusCode TheAccountant::RazorVariableHists::initialize() {
  ss_mass = book(m_name,"ss_mass", "SS mass [GeV]", 120, 0, 3000.);
  ss_invgamma = book(m_name, "ss_invgamma", "SS InvGamma [units?]", 100, 0, 1.); 
  ss_dphivis = book(m_name, "ss_dphivis", "SS dPhiIvs [units?]", 100, 0, 1.);
  ss_costheta = book(m_name, "ss_costheta", "SS CosTheta", 100, -1., 1.);
  ss_dphidecayangle = book(m_name, "ss_dphidecayangle", "SS dPhiDecayAngle [radians?]",100, -7, 7);
  ss_visshape = book(m_name, "ss_visshape", "SS VisShape [units?]", 100, 0, 1.);
  ss_mdeltaR = book(m_name, "ss_mdeltaR", "SS MDeltaR [GeV?]", 100, 0, 1.);
  s1_mass = book(m_name, "s1_mass", "S1 Mass [GeV]", 100, 0, 1.);
  s1_costheta = book(m_name, "s1_costheta", "S1 CosTheta", 100, -1., 1.);
  s2_mass = book(m_name, "s2_mass", "S2 Mass [GeV]", 100, 0, 1.);
  s2_costheta = book(m_name, "s2_costheta", "S2 CosTheta", 100,-1., 1.);
  i1_depth = book(m_name, "i1_depth", "I1 Depth [units?]", 100, -1., 1.);
  i2_depth = book(m_name, "i2_depth", "I2 Depth [units?]", 100, -1., 1.);
  v1_nelements = book(m_name, "v1_nelements", "V1 NElements", 5, 0, 5);
  v2_nelements = book(m_name, "v2_nelements", "V2 NElements", 5, 0, 5);

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::RazorVariableHists::execute(const xAOD::EventInfo* eventInfo, float eventWeight){

  SG::AuxElement::ConstAccessor< RF::RDecayFrame > SS_decor("SS");
  SG::AuxElement::ConstAccessor< RF::RDecayFrame > S1_decor("S1");
  SG::AuxElement::ConstAccessor< RF::RDecayFrame > S2_decor("S2");
  SG::AuxElement::ConstAccessor< RF::RVisibleFrame > V1_decor("V1");
  SG::AuxElement::ConstAccessor< RF::RVisibleFrame > V2_decor("V2");
  SG::AuxElement::ConstAccessor< RF::RInvisibleFrame > I1_decor("I1");
  SG::AuxElement::ConstAccessor< RF::RInvisibleFrame > I2_decor("I2");
  SG::AuxElement::ConstAccessor< RF::CombinatoricGroup > VIS_decor("VIS");
  SG::AuxElement::ConstAccessor< RF::InvisibleGroup > INV_decor("INV");

  RF::RDecayFrame       SS(SS_decor(*eventInfo));
  RF::RDecayFrame       S1(S1_decor(*eventInfo));
  RF::RDecayFrame       S2(S2_decor(*eventInfo));
  RF::RVisibleFrame     V1(V1_decor(*eventInfo));
  RF::RVisibleFrame     V2(V2_decor(*eventInfo));
  RF::RInvisibleFrame   I1(I1_decor(*eventInfo));
  RF::RInvisibleFrame   I2(I2_decor(*eventInfo));

  RF::CombinatoricGroup VIS(VIS_decor(*eventInfo));
  RF::InvisibleGroup    INV(INV_decor(*eventInfo));


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


  ss_mass->           Fill( SS.GetMass()/1000., eventWeight);
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

  return StatusCode::SUCCESS;
}

