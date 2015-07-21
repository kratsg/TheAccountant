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

  ss_mass->           Fill( SS_decor(*eventInfo).GetMass()/1000., eventWeight);
  ss_invgamma->       Fill( 1./eventInfo->SS.GetGammaInParentFrame(),eventWeight);
  ss_dphivis->        Fill( eventInfo->SS.GetDeltaPhiBoostVisible(), eventWeight);
  ss_costheta->       Fill( eventInfo->SS.GetCosDecayAngle(), eventWeight);
  ss_dphidecayangle-> Fill( eventInfo->SS.GetDeltaPhiDecayAngle(), eventWeight);
  ss_visshape->       Fill( eventInfo->SS.GetVisibleShape(), eventWeight);
  ss_mdeltaR->        Fill( eventInfo->SS.GetVisibleShape()*eventInfo->SS.GetMass(), eventWeight);
  s1_mass->           Fill( eventInfo->S1.GetMass()/1000., eventWeight);
  s1_costheta->       Fill( eventInfo->S1.GetCosDecayAngle(), eventWeight);
  s2_mass->           Fill( eventInfo->S2.GetMass()/1000., eventWeight);
  s2_costheta->       Fill( eventInfo->S2.GetCosDecayAngle(), eventWeight);
  i1_depth->          Fill( eventInfo->S1.GetFrameDepth(eventInfo->I1), eventWeight);
  i2_depth->          Fill( eventInfo->S2.GetFrameDepth(eventInfo->I2), eventWeight);
  v1_nelements->      Fill( eventInfo->VIS.GetNElementsInFrame(eventInfo->V1), eventWeight);
  v2_nelements->      Fill( eventInfo->VIS.GetNElementsInFrame(eventInfo->V2), eventWeight);

  return StatusCode::SUCCESS;
}
