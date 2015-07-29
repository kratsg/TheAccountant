#include <TheAccountant/JetHists.h>

// jet definition
#include <fastjet/JetDefinition.hh>

// subjet finding
#include "JetSubStructureUtils/SubjetFinder.h"

#include "TheAccountant/VariableDefinitions.h"

#include <FourMomUtils/xAODP4Helpers.h>

namespace VD = VariableDefinitions;

TheAccountant::JetHists::JetHists (std::string name) :
  HistogramManager(name, ""),
  m_doTopology(false),
  m_doSubstructure(false)
{
}

TheAccountant::JetHists::~JetHists () {}

StatusCode TheAccountant::JetHists::initialize() {
  m_ht          = book(m_name, "HT", "#sum p_{T}^{jet} [GeV]", 100, 0, 3000.);
  m_massOverPt  = book(m_name, "MOverPt", "#frac{m^{jet}}{p_{T}}", 100, 0, 5);
  m_jvt         = book(m_name, "Jvt", "JVT", 110, -0.1, 1);

  if(m_doTopology){
    //topology
    m_dEta        = book(m_name, "j1j2/dEta", "|#Delta#eta(j^{1}, j^{2})|", 80, 0, 8);
    m_dPhi        = book(m_name, "j1j2/dPhi", "|#Delta#phi(j^{1}, j^{2})|", 120, 0, 2*TMath::Pi() );
    m_asymmetry   = book(m_name, "j1j2/asymmetry", "#frac{p_{T}^{1} - p_{T}^{2}}{p_{T}^{1} + p_{T}^{2}}", 100, 0, 1);
    m_ptRatio     = book(m_name, "j1j3/ptRatio", "#frac{p_{T}^{3}}{p_{T}^{1}}", 100, 0, 1);
    m_totalMass   = book(m_name, "sumJetM", "#Sigma m^{jet}", 120, 0, 3000.);
  }

  if(m_doSubstructure){
    //substructure
      //subjettiness
    m_tau21                     = book(m_name, "tau21", "#Tau_{21}", 120, 0, 1.2);
    m_tau32                     = book(m_name, "tau32", "#Tau_{32}", 120, 0, 1.2);
      //subjet info
    m_subjet_multiplicity       = book(m_name, "subjets/multiplicity", "N_{subjet}", 21, -0.5, 20.5);
    m_subjet_ptFrac             = book(m_name, "subjets/ptFrac", "p_{T}^{subjet}/p_{T}^{jet}", 100, 0, 1);
      //constituent info
    m_constituents_multiplicity = book(m_name, "constituents/multiplicity", "N_{constituents}", 61, -0.5, 60.5);
    m_constituents_width        = book(m_name, "width", "#frac{1}{p_{T}^{jet}} #Sigma_{i}^{constit} p_{T,i}^{constit} #cdot #Delta R(constit, jet)", 100, 0, 2);
  }
  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::JetHists::execute( const xAOD::JetContainer* jets, float eventWeight ) {

  float totalMass(0);
  for(const auto jet: *jets){
    totalMass += jet->m();
    if(!this->execute(jet, eventWeight).isSuccess()) return StatusCode::FAILURE;
  }

  m_ht->Fill( VD::HT(jets, nullptr, nullptr)/1.e3, eventWeight );

  if(m_doTopology){
    const xAOD::Jet* j1(nullptr);
    const xAOD::Jet* j2(nullptr);
    const xAOD::Jet* j3(nullptr);

    if(jets->size() > 0){
      j1 = jets->at(0);
      if(jets->size() > 1){
        j2 = jets->at(1);
        m_dEta->      Fill( fabs(xAOD::P4Helpers::deltaEta(j1, j2)),      eventWeight );
        m_dPhi->      Fill( fabs(xAOD::P4Helpers::deltaPhi(j1, j2)),      eventWeight );
        m_asymmetry-> Fill( (j1->pt() - j2->pt())/(j1->pt() + j2->pt()),  eventWeight );
        if(jets->size() > 2){
          j3 = jets->at(2);
          m_ptRatio->  Fill( j3->pt()/j1->pt() );
        }
      }
    }
    m_totalMass->     Fill( totalMass/1000., eventWeight );
  }

  return StatusCode::SUCCESS;
}

StatusCode TheAccountant::JetHists::execute( const xAOD::Jet* jet, float eventWeight ){

  m_massOverPt->Fill( jet->m()/jet->pt(), eventWeight );
  float jvt(-99.0);
  jet->getAttribute("Jvt", jvt);
  m_jvt->Fill(jvt, eventWeight);

  if(m_doSubstructure){
    static SG::AuxElement::ConstAccessor<float> Width("Width");

    // subjettiness
    m_tau21->Fill( VD::Tau21(jet), eventWeight);
    m_tau32->Fill( VD::Tau32(jet), eventWeight);

    /*
       xAOD::TEvent::getInputObject:0: RuntimeWarning: Key 0x39b44ab4 unknown
       xAOD::TVirtualEvent::retrieve:0: RuntimeWarning: Couldn't retrieve 10DataVectorIN4xAOD9IParticleEN16DataModel_detail6NoBaseEE/0x39b44ab4
       terminate called after throwing an instance of 'std::runtime_error'
         what():  ElementLink::operator*() Element not available
         Aborted
    */
    /*

    // default to using the kt_algorithm and 0.2 radius
    fastjet::JetAlgorithm subjet_clustering(fastjet::kt_algorithm);
    float subjet_radius(0.2);
    // if it is a transformed jet, deal with that
    static SG::AuxElement::ConstAccessor<int> TransformType("TransformType");
    if(TransformType.isAvailable(*jet)){
      switch(TransformType(*jet)){
        static SG::AuxElement::ConstAccessor<float> RClus("RClus");
        case xAOD::JetTransform::Trim:
          subjet_clustering = fastjet::kt_algorithm;
          subjet_radius     = (RClus.isAvailable(*jet))?RClus(*jet):0.2;
        break;
        case xAOD::JetTransform::MassDrop:
          subjet_clustering = fastjet::cambridge_algorithm;
          subjet_radius     = (RClus.isAvailable(*jet))?RClus(*jet):0.2;
        break;
        default:
          //case xAOD::JetTransform::Prune:
          //use defaults
        break;
      }
    }
    // else, use defaults

    JetSubStructureUtils::SubjetFinder subjetFinder(subjet_clustering, subjet_radius, 0.0);
    std::vector<fastjet::PseudoJet> subjets = subjetFinder.result(*jet);
    for(auto subjet: subjets) m_subjet_ptFrac->Fill( subjet.pt()/jet->pt(), eventWeight);
    m_subjet_multiplicity->Fill( subjets.size(), eventWeight);
    */

    m_constituents_multiplicity->Fill( jet->numConstituents(), eventWeight);
    if(Width.isAvailable(*jet)) m_constituents_width->Fill( Width(*jet), eventWeight);
  }

  return StatusCode::SUCCESS;
}
