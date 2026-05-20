
// system include files
#include <memory>
#include <utility>
#include <cmath>
#include <map>
#include <string>
#include <list>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "DataFormats/ParticleFlowReco/interface/PFBlockFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementTrack.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElementCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TProfile2D.h"

class RECOAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit RECOAnalyzer(const edm::ParameterSet&);
  ~RECOAnalyzer() override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override {}
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override {}

  edm::Service<TFileService> fs_;

  std::map<std::string, TH1F*>       m_mapH1D;
  std::map<std::string, TH2F*>       m_mapH2D;
  std::map<std::string, TH3F*>       m_mapH3D;
  std::map<std::string, TProfile*>   m_mapProf;
  std::map<std::string, TProfile2D*> m_mapProf2D;

  template <typename T, typename... Args>
  T* book(Args&&... args) const {
    return fs_->make<T>(std::forward<Args>(args)...);
  }

  template <typename... Args>
  void bookTH1F(const std::string& hName, const std::string& hTitle, Args&&... args) {
    m_mapH1D[hName] = book<TH1F>(hName.c_str(), hTitle.c_str(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  void bookTH2F(const std::string& hName, const std::string& hTitle, Args&&... args) {
    m_mapH2D[hName] = book<TH2F>(hName.c_str(), hTitle.c_str(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  void bookTH3F(const std::string& hName, const std::string& hTitle, Args&&... args) {
    m_mapH3D[hName] = book<TH3F>(hName.c_str(), hTitle.c_str(), std::forward<Args>(args)...);
  }

  edm::EDGetTokenT<std::vector<reco::PFBlock>> pfBlocksToken_;
  edm::EDGetTokenT<reco::VertexCollection> primaryVerticesToken_;
  edm::EDGetTokenT<reco::GenParticleCollection> genParticlesToken_;
};

RECOAnalyzer::RECOAnalyzer(const edm::ParameterSet& iConfig)
    : pfBlocksToken_(consumes<std::vector<reco::PFBlock>>(iConfig.getParameter<edm::InputTag>("pfBlock"))),
      primaryVerticesToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("primaryVertices"))),
      genParticlesToken_(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticles"))) {

  usesResource(TFileService::kSharedResource);

  bookTH1F("h_nPV", ";nPV", 80, 0, 80);
  bookTH1F("h_nPVGood", ";nPVGood", 80, 0, 80);

  bookTH1F("h_pftrack_pt", ";PF track pT [GeV];tracks", 100, 0., 100.);
  bookTH1F("h_pftrack_eta", ";PF track #eta;tracks", 100, -5., 5.);
  bookTH2F("h2_pftrack_eta_vs_pt", ";PF track #eta;PF track pT [GeV]", 100, -5., 5., 100, 0., 100.);

  bookTH1F("h_ep_hcal", ";HCAL E/p;tracks", 100, 0., 5.);
  bookTH1F("h_ep_total", ";(ECAL+HCAL)/p;tracks", 100, 0., 5.);

  bookTH1F("h_ep_hcal_barrel", ";HCAL E/p, |#eta| < 1.3;tracks", 100, 0., 5.);
  bookTH1F("h_ep_total_barrel", ";(ECAL+HCAL)/p, |#eta| < 1.3;tracks", 100, 0., 5.);

  bookTH1F("h_ep_hcal_endcap", ";HCAL E/p, 1.3 #leq |#eta| < 2.5;tracks", 100, 0., 5.);
  bookTH1F("h_ep_total_endcap", ";(ECAL+HCAL)/p, 1.3 #leq |#eta| < 2.5;tracks", 100, 0., 5.);

  bookTH2F("h2_ep_total_vs_p_barrel",
           ";track p [GeV];(ECAL+HCAL)/p, |#eta| < 1.3",
           100, 0., 100., 100, 0., 5.);

  bookTH2F("h2_ep_total_vs_eta_barrel",
           ";track #eta;(ECAL+HCAL)/p, |#eta| < 1.3",
           52, -1.3, 1.3, 100, 0., 5.);

  bookTH2F("h2_ep_total_vs_p_endcap",
           ";track p [GeV];(ECAL+HCAL)/p, 1.3 #leq |#eta| < 2.5",
           100, 0., 100., 100, 0., 5.);

  bookTH2F("h2_ep_total_vs_eta_endcap",
           ";track #eta;(ECAL+HCAL)/p, 1.3 #leq |#eta| < 2.5",
           100, -2.5, 2.5, 100, 0., 5.);

  bookTH2F("h2_ep_total_vs_genE_barrel",
           ";gen pion energy [GeV];(ECAL+HCAL)/p, |#eta| < 1.3",
           100, 0., 200., 100, 0., 5.);

  bookTH2F("h2_ep_total_vs_genE_endcap",
           ";gen pion energy [GeV];(ECAL+HCAL)/p, 1.3 #leq |#eta| < 2.5",
           100, 0., 200., 100, 0., 5.);

  bookTH3F("h3_ep_total_p_eta_barrel",
           ";track p [GeV];track #eta;(ECAL+HCAL)/p",
           100, 0., 100.,
           52, -1.3, 1.3,
           100, 0., 5.);

  bookTH3F("h3_ep_total_genE_eta_barrel",
           ";gen pion energy [GeV];track #eta;(ECAL+HCAL)/p",
           100, 0., 200.,
           52, -1.3, 1.3,
           100, 0., 5.);

  // Gen denominators
  bookTH1F("h_gen_pt_den", ";gen pion pT [GeV];gen pions", 100, 0., 100.);
  bookTH1F("h_gen_eta_den", ";gen pion #eta;gen pions", 100, -5., 5.);
  bookTH1F("h_gen_energy_den", ";gen pion energy [GeV];gen pions", 100, 0., 200.);

  bookTH1F("h_gen_pt_den_trackeff", ";gen pion pT [GeV];gen pions", 100, 0., 100.);
  bookTH1F("h_gen_eta_den_trackeff", ";gen pion #eta;gen pions", 100, -2.5, 2.5);
  bookTH1F("h_gen_energy_den_trackeff", ";gen pion energy [GeV];gen pions", 100, 0., 200.);

  bookTH1F("h_gen_pt_den_trackeff_barrel", ";gen pion pT [GeV], |#eta| < 1.3;gen pions", 100, 0., 100.);
  bookTH1F("h_gen_energy_den_trackeff_barrel", ";gen pion energy [GeV], |#eta| < 1.3;gen pions", 100, 0., 200.);

  // Gen numerators: reco track
  bookTH1F("h_gen_pt_num_trackeff", ";gen pion pT [GeV];gen pions with matched reco track", 100, 0., 100.);
  bookTH1F("h_gen_eta_num_trackeff", ";gen pion #eta;gen pions with matched reco track", 100, -2.5, 2.5);
  bookTH1F("h_gen_energy_num_trackeff", ";gen pion energy [GeV];gen pions with matched reco track", 100, 0., 200.);

  bookTH1F("h_gen_pt_num_trackeff_barrel", ";gen pion pT [GeV], |#eta| < 1.3;matched reco track", 100, 0., 100.);
  bookTH1F("h_gen_energy_num_trackeff_barrel", ";gen pion energy [GeV], |#eta| < 1.3;matched reco track", 100, 0., 200.);

  // Gen numerators: matched track in HCAL PFBlock
  bookTH1F("h_gen_pt_num_track_hcalblock", ";gen pion pT [GeV];matched track in PFBlock with HCAL", 100, 0., 100.);
  bookTH1F("h_gen_eta_num_track_hcalblock", ";gen pion #eta;matched track in PFBlock with HCAL", 100, -2.5, 2.5);
  bookTH1F("h_gen_energy_num_track_hcalblock", ";gen pion energy [GeV];matched track in PFBlock with HCAL", 100, 0., 200.);

  bookTH1F("h_gen_pt_num_track_hcalblock_barrel", ";gen pion pT [GeV], |#eta| < 1.3;matched track in PFBlock with HCAL", 100, 0., 100.);
  bookTH1F("h_gen_energy_num_track_hcalblock_barrel", ";gen pion energy [GeV], |#eta| < 1.3;matched track in PFBlock with HCAL", 100, 0., 200.);

  // Old deltaR matching diagnostics
  bookTH1F("h_gen_pt_num_dr02", ";gen pion pT [GeV];matched events, #DeltaR < 0.2", 100, 0., 100.);
  bookTH1F("h_gen_pt_num_dr03", ";gen pion pT [GeV];matched events, #DeltaR < 0.3", 100, 0., 100.);
  bookTH1F("h_gen_pt_num_dr04", ";gen pion pT [GeV];matched events, #DeltaR < 0.4", 100, 0., 100.);
  bookTH1F("h_gen_pt_num_dr05", ";gen pion pT [GeV];matched events, #DeltaR < 0.5", 100, 0., 100.);

  bookTH1F("h_gen_eta_num_dr02", ";gen pion #eta;matched events, #DeltaR < 0.2", 100, -5., 5.);
  bookTH1F("h_gen_eta_num_dr03", ";gen pion #eta;matched events, #DeltaR < 0.3", 100, -5., 5.);
  bookTH1F("h_gen_eta_num_dr04", ";gen pion #eta;matched events, #DeltaR < 0.4", 100, -5., 5.);
  bookTH1F("h_gen_eta_num_dr05", ";gen pion #eta;matched events, #DeltaR < 0.5", 100, -5., 5.);

  bookTH1F("h_min_dr_gen_pftrack", ";minimum #DeltaR(gen pion, PF track);events", 100, 0., 1.0);

  bookTH1F("h_pftrack_pt_all", ";PF track pT [GeV];tracks", 100, 0., 100.);
  bookTH1F("h_pftrack_eta_all", ";PF track #eta;tracks", 100, -5., 5.);

  bookTH1F("h_pftrack_pt_matched_dr02", ";PF track pT [GeV];matched tracks, #DeltaR < 0.2", 100, 0., 100.);
  bookTH1F("h_pftrack_pt_matched_dr03", ";PF track pT [GeV];matched tracks, #DeltaR < 0.3", 100, 0., 100.);
  bookTH1F("h_pftrack_pt_matched_dr04", ";PF track pT [GeV];matched tracks, #DeltaR < 0.4", 100, 0., 100.);
  bookTH1F("h_pftrack_pt_matched_dr05", ";PF track pT [GeV];matched tracks, #DeltaR < 0.5", 100, 0., 100.);

  bookTH1F("h_pftrack_eta_matched_dr02", ";PF track #eta;matched tracks, #DeltaR < 0.2", 100, -5., 5.);
  bookTH1F("h_pftrack_eta_matched_dr03", ";PF track #eta;matched tracks, #DeltaR < 0.3", 100, -5., 5.);
  bookTH1F("h_pftrack_eta_matched_dr04", ";PF track #eta;matched tracks, #DeltaR < 0.4", 100, -5., 5.);
  bookTH1F("h_pftrack_eta_matched_dr05", ";PF track #eta;matched tracks, #DeltaR < 0.5", 100, -5., 5.);

  bookTH1F("h_ep_total_matched_dr03", ";(ECAL+HCAL)/p, matched #DeltaR < 0.3;tracks", 100, 0., 5.);
  bookTH1F("h_ep_total_unmatched_dr03", ";(ECAL+HCAL)/p, unmatched #DeltaR #geq 0.3;tracks", 100, 0., 5.);
}

void RECOAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  edm::Handle<reco::VertexCollection> primaryVerticesHandle;
  iEvent.getByToken(primaryVerticesToken_, primaryVerticesHandle);

  int nPV = 0;
  int nPVGood = 0;

  if (primaryVerticesHandle.isValid()) {
    for (unsigned int ind = 0; ind < primaryVerticesHandle->size(); ++ind) {
      const reco::Vertex& pv = (*primaryVerticesHandle)[ind];
      nPV++;

      bool pass = (!pv.isFake()) && (pv.ndof() > 4) &&
                  (std::abs(pv.z()) <= 24.) &&
                  (pv.position().Rho() <= 2.);
      if (pass) nPVGood++;
    }
  }

  m_mapH1D["h_nPV"]->Fill(nPV);
  m_mapH1D["h_nPVGood"]->Fill(nPVGood);

  edm::Handle<reco::GenParticleCollection> genParticlesHandle;
  iEvent.getByToken(genParticlesToken_, genParticlesHandle);

  const reco::GenParticle* genPion = nullptr;

  if (genParticlesHandle.isValid()) {
    for (const auto& gen : *genParticlesHandle) {
      if (std::abs(gen.pdgId()) != 211) continue;
      if (gen.status() != 1) continue;
      if (gen.pt() < 1.0 || gen.pt() > 100.0) continue;

      genPion = &gen;
      break;
    }
  }

  if (!genPion) return;

  const float gen_pt     = genPion->pt();
  const float gen_eta    = genPion->eta();
  const float gen_phi    = genPion->phi();
  const float gen_p      = genPion->p();
  const float gen_energy = genPion->energy();

  const bool genIsBarrel = std::abs(gen_eta) < 1.3;

  m_mapH1D["h_gen_pt_den"]->Fill(gen_pt);
  m_mapH1D["h_gen_eta_den"]->Fill(gen_eta);
  m_mapH1D["h_gen_energy_den"]->Fill(gen_energy);

  m_mapH1D["h_gen_pt_den_trackeff"]->Fill(gen_pt);
  m_mapH1D["h_gen_eta_den_trackeff"]->Fill(gen_eta);
  m_mapH1D["h_gen_energy_den_trackeff"]->Fill(gen_energy);

  if (genIsBarrel) {
    m_mapH1D["h_gen_pt_den_trackeff_barrel"]->Fill(gen_pt);
    m_mapH1D["h_gen_energy_den_trackeff_barrel"]->Fill(gen_energy);
  }

  edm::Handle<std::vector<reco::PFBlock>> pfBlocksHandle;
  iEvent.getByToken(pfBlocksToken_, pfBlocksHandle);
  if (!pfBlocksHandle.isValid()) return;

  const auto& pfBlocks = *pfBlocksHandle;

  bool hasMatchedRecoTrack = false;

  for (unsigned iBlock = 0; iBlock < pfBlocks.size(); ++iBlock) {
    const reco::PFBlock& block = pfBlocks[iBlock];
    const auto& elements = block.elements();

    for (const auto& elem : elements) {
      if (elem.type() != reco::PFBlockElement::TRACK) continue;

      reco::TrackRef trk = elem.trackRef();
      if (trk.isNull()) continue;
      if (trk->p() <= 0.) continue;

      const float dr = reco::deltaR(gen_eta, gen_phi, trk->eta(), trk->phi());
      const float relDp = std::abs(trk->p() - gen_p) / gen_p;

      if (dr < 0.05 && relDp < 0.05) {
        hasMatchedRecoTrack = true;
        break;
      }
    }

    if (hasMatchedRecoTrack) break;
  }

  if (hasMatchedRecoTrack) {
    m_mapH1D["h_gen_pt_num_trackeff"]->Fill(gen_pt);
    m_mapH1D["h_gen_eta_num_trackeff"]->Fill(gen_eta);
    m_mapH1D["h_gen_energy_num_trackeff"]->Fill(gen_energy);

    if (genIsBarrel) {
      m_mapH1D["h_gen_pt_num_trackeff_barrel"]->Fill(gen_pt);
      m_mapH1D["h_gen_energy_num_trackeff_barrel"]->Fill(gen_energy);
    }
  }

  std::list<reco::PFBlockRef> singleTrackWithHCALBlockRefs;

  for (unsigned iBlock = 0; iBlock < pfBlocks.size(); ++iBlock) {
    reco::PFBlockRef blockref(pfBlocksHandle, iBlock);
    const reco::PFBlock& block = *blockref;
    const edm::OwnVector<reco::PFBlockElement>& elements = block.elements();

    int nTrack = 0;
    int nHCAL = 0;

    for (unsigned iElement = 0; iElement < elements.size(); ++iElement) {
      reco::PFBlockElement::Type type = elements[iElement].type();

      if (type == reco::PFBlockElement::TRACK) {
        nTrack++;
      } else if (type == reco::PFBlockElement::HCAL) {
        nHCAL++;
      }
    }

    if (nTrack == 1 && nHCAL >= 1) {
      singleTrackWithHCALBlockRefs.push_back(blockref);
    }
  }

  bool hasMatchedTrackInHCALBlock = false;

  for (const reco::PFBlockRef& blockRef : singleTrackWithHCALBlockRefs) {
    if (blockRef.isNull()) continue;

    const reco::PFBlock& block = *blockRef;
    const auto& elements = block.elements();

    reco::TrackRef trk;

    for (const auto& elem : elements) {
      if (elem.type() == reco::PFBlockElement::TRACK) {
        trk = elem.trackRef();
        break;
      }
    }

    if (trk.isNull()) continue;
    if (trk->p() <= 0.) continue;

    const float dr = reco::deltaR(gen_eta, gen_phi, trk->eta(), trk->phi());
    const float relDp = std::abs(trk->p() - gen_p) / gen_p;

    if (dr < 0.05 && relDp < 0.05) {
      hasMatchedTrackInHCALBlock = true;
      break;
    }
  }

  if (hasMatchedTrackInHCALBlock) {
    m_mapH1D["h_gen_pt_num_track_hcalblock"]->Fill(gen_pt);
    m_mapH1D["h_gen_eta_num_track_hcalblock"]->Fill(gen_eta);
    m_mapH1D["h_gen_energy_num_track_hcalblock"]->Fill(gen_energy);

    if (genIsBarrel) {
      m_mapH1D["h_gen_pt_num_track_hcalblock_barrel"]->Fill(gen_pt);
      m_mapH1D["h_gen_energy_num_track_hcalblock_barrel"]->Fill(gen_energy);
    }
  }

  bool genMatched_dr02 = false;
  bool genMatched_dr03 = false;
  bool genMatched_dr04 = false;
  bool genMatched_dr05 = false;

  float minDR = 999.0;

  for (const reco::PFBlockRef& blockRef : singleTrackWithHCALBlockRefs) {
    if (blockRef.isNull()) continue;

    const reco::PFBlock& block = *blockRef;
    const auto& elements = block.elements();

    reco::TrackRef trk;
    float hcalEnergy = 0.0;
    float ecalEnergy = 0.0;

    for (unsigned i = 0; i < elements.size(); ++i) {
      if (elements[i].type() == reco::PFBlockElement::TRACK) {
        trk = elements[i].trackRef();
      } else if (elements[i].type() == reco::PFBlockElement::HCAL) {
        reco::PFClusterRef cluster = elements[i].clusterRef();
        if (cluster.isNonnull()) hcalEnergy += cluster->energy();
      } else if (elements[i].type() == reco::PFBlockElement::ECAL) {
        reco::PFClusterRef cluster = elements[i].clusterRef();
        if (cluster.isNonnull()) ecalEnergy += cluster->energy();
      }
    }

    if (trk.isNull()) continue;

    const float trk_pt  = trk->pt();
    const float trk_eta = trk->eta();
    const float trk_phi = trk->phi();
    const float trk_p   = trk->p();

    if (trk_pt < 3.) continue;
    if (trk_p <= 0.) continue;

    const float dr = reco::deltaR(gen_eta, gen_phi, trk_eta, trk_phi);
    if (dr < minDR) minDR = dr;

    const bool matched_dr02 = dr < 0.2;
    const bool matched_dr03 = dr < 0.3;
    const bool matched_dr04 = dr < 0.4;
    const bool matched_dr05 = dr < 0.5;

    if (matched_dr02) genMatched_dr02 = true;
    if (matched_dr03) genMatched_dr03 = true;
    if (matched_dr04) genMatched_dr04 = true;
    if (matched_dr05) genMatched_dr05 = true;

    m_mapH1D["h_pftrack_pt"]->Fill(trk_pt);
    m_mapH1D["h_pftrack_eta"]->Fill(trk_eta);
    m_mapH2D["h2_pftrack_eta_vs_pt"]->Fill(trk_eta, trk_pt);

    m_mapH1D["h_pftrack_pt_all"]->Fill(trk_pt);
    m_mapH1D["h_pftrack_eta_all"]->Fill(trk_eta);

    if (matched_dr02) {
      m_mapH1D["h_pftrack_pt_matched_dr02"]->Fill(trk_pt);
      m_mapH1D["h_pftrack_eta_matched_dr02"]->Fill(trk_eta);
    }
    if (matched_dr03) {
      m_mapH1D["h_pftrack_pt_matched_dr03"]->Fill(trk_pt);
      m_mapH1D["h_pftrack_eta_matched_dr03"]->Fill(trk_eta);
    }
    if (matched_dr04) {
      m_mapH1D["h_pftrack_pt_matched_dr04"]->Fill(trk_pt);
      m_mapH1D["h_pftrack_eta_matched_dr04"]->Fill(trk_eta);
    }
    if (matched_dr05) {
      m_mapH1D["h_pftrack_pt_matched_dr05"]->Fill(trk_pt);
      m_mapH1D["h_pftrack_eta_matched_dr05"]->Fill(trk_eta);
    }

    const float ep_hcal = hcalEnergy / trk_p;
    const float ep_total = (ecalEnergy + hcalEnergy) / trk_p;

    const bool isBarrel = std::abs(trk_eta) < 1.3;
    const bool isEndcap = std::abs(trk_eta) >= 1.3 && std::abs(trk_eta) < 2.5;

    if (matched_dr03) {
      m_mapH1D["h_ep_total_matched_dr03"]->Fill(ep_total);
    } else {
      m_mapH1D["h_ep_total_unmatched_dr03"]->Fill(ep_total);
    }

    m_mapH1D["h_ep_hcal"]->Fill(ep_hcal);
    m_mapH1D["h_ep_total"]->Fill(ep_total);

    if (isBarrel) {
      m_mapH1D["h_ep_hcal_barrel"]->Fill(ep_hcal);
      m_mapH1D["h_ep_total_barrel"]->Fill(ep_total);

      m_mapH2D["h2_ep_total_vs_p_barrel"]->Fill(trk_p, ep_total);
      m_mapH2D["h2_ep_total_vs_eta_barrel"]->Fill(trk_eta, ep_total);
      m_mapH2D["h2_ep_total_vs_genE_barrel"]->Fill(gen_energy, ep_total);

      m_mapH3D["h3_ep_total_p_eta_barrel"]->Fill(trk_p, trk_eta, ep_total);
      m_mapH3D["h3_ep_total_genE_eta_barrel"]->Fill(gen_energy, trk_eta, ep_total);
    }

    if (isEndcap) {
      m_mapH1D["h_ep_hcal_endcap"]->Fill(ep_hcal);
      m_mapH1D["h_ep_total_endcap"]->Fill(ep_total);

      m_mapH2D["h2_ep_total_vs_p_endcap"]->Fill(trk_p, ep_total);
      m_mapH2D["h2_ep_total_vs_eta_endcap"]->Fill(trk_eta, ep_total);
      m_mapH2D["h2_ep_total_vs_genE_endcap"]->Fill(gen_energy, ep_total);
    }
  }

  if (minDR < 999.0) {
    m_mapH1D["h_min_dr_gen_pftrack"]->Fill(minDR);
  }

  if (genMatched_dr02) {
    m_mapH1D["h_gen_pt_num_dr02"]->Fill(gen_pt);
    m_mapH1D["h_gen_eta_num_dr02"]->Fill(gen_eta);
  }
  if (genMatched_dr03) {
    m_mapH1D["h_gen_pt_num_dr03"]->Fill(gen_pt);
    m_mapH1D["h_gen_eta_num_dr03"]->Fill(gen_eta);
  }
  if (genMatched_dr04) {
    m_mapH1D["h_gen_pt_num_dr04"]->Fill(gen_pt);
    m_mapH1D["h_gen_eta_num_dr04"]->Fill(gen_eta);
  }
  if (genMatched_dr05) {
    m_mapH1D["h_gen_pt_num_dr05"]->Fill(gen_pt);
    m_mapH1D["h_gen_eta_num_dr05"]->Fill(gen_eta);
  }
}

void RECOAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(RECOAnalyzer);