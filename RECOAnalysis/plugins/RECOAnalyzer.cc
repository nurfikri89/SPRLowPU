// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/Provenance/interface/StableProvenance.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementBrem.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementSuperCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementGsfTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"


class RECOAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit RECOAnalyzer(const edm::ParameterSet&);
  ~RECOAnalyzer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // ----------member data ---------------------------
   edm::EDGetTokenT<std::vector<reco::PFBlock>> pfBlocksToken_;
};


RECOAnalyzer::RECOAnalyzer(const edm::ParameterSet& iConfig):
  pfBlocksToken_(consumes<std::vector<reco::PFBlock>>(iConfig.getParameter<edm::InputTag>("pfBlock"))){
  //now do what ever initialization is needed
}

RECOAnalyzer::~RECOAnalyzer() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
}

// ------------ method called for each event  ------------
void RECOAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  //
  // Get PFBlocks
  //
  edm::Handle<std::vector<reco::PFBlock>> pfBlocksHandle;
  iEvent.getByToken(pfBlocksToken_, pfBlocksHandle);
  std::vector<reco::PFBlock> pfBlocks = *pfBlocksHandle;

  //
  // Loop over PFBlocks
  //
  std::list<reco::PFBlockRef> singleTrackBlockRefs;

  for (unsigned iBlock = 0; iBlock < pfBlocks.size(); ++iBlock) {
    reco::PFBlockRef blockref = reco::PFBlockRef(pfBlocksHandle, iBlock);
    const reco::PFBlock& block = *blockref;
    const edm::OwnVector<reco::PFBlockElement>& elements = block.elements();

    int nTrack = 0;
    int nPS1 = 0;
    int nPS2 = 0;
    int nECAL = 0;
    int nHCAL = 0;
    int nHFEM = 0;
    int nHFHAD = 0;
    int nHO = 0;
    //
    // Loop over elements in PFBlock
    //
    for (unsigned iElement = 0; iElement < elements.size(); ++iElement) {
      reco::PFBlockElement::Type type = elements[iElement].type();
      if (type == reco::PFBlockElement::TRACK) {
        nTrack++;
      }
      else if (type == reco::PFBlockElement::PS1) {
        nPS1++;
      }
      else if (type == reco::PFBlockElement::PS2) {
        nPS2++;
      }
      else if (type == reco::PFBlockElement::ECAL) {
        nECAL++;
      }
      else if (type == reco::PFBlockElement::HCAL) {
        nHCAL++;
      }
      else if (type == reco::PFBlockElement::HFEM) {
        nHFEM++;
      }
      else if (type == reco::PFBlockElement::HFHAD) {
        nHFHAD++;
      }
      else if (type == reco::PFBlockElement::HO) {
        nHO++;
      }
    }
    if(nTrack == 1){
      singleTrackBlockRefs.push_back(blockref);
    }
  }
}

// ------------ method called once each job just before starting event loop  ------------
void RECOAnalyzer::beginJob() {
  // please remove this method if not needed
}

// ------------ method called once each job just after ending the event loop  ------------
void RECOAnalyzer::endJob() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void RECOAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(RECOAnalyzer);
