#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CondFormats/DataRecord/interface/PFCalibrationRcd.h"
#include "CondFormats/PhysicsToolsObjects/interface/PerformancePayloadFromTFormula.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/ESInputTag.h"
#include "RecoParticleFlow/PFClusterTools/interface/PFEnergyCalibration.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"

#include <cmath>
#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>

class SPRLowPUDataAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources, edm::one::WatchRuns> {
public:
  explicit SPRLowPUDataAnalyzer(edm::ParameterSet const& config);
  ~SPRLowPUDataAnalyzer() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  struct MuonMatchResult {
    unsigned int multiplicity = 0;
    bool passesSoft = false;
    bool passesLoose = false;
    bool passesMedium = false;
    bool passesTight = false;

    bool hasExactMatch() const { return multiplicity > 0; }
    bool isMuonTagged() const { return passesSoft || passesLoose; }
  };

  void beginRun(edm::Run const& run, edm::EventSetup const& setup) override;
  void endRun(edm::Run const& run, edm::EventSetup const& setup) override {}
  void analyze(edm::Event const& event, edm::EventSetup const& setup) override;
  bool isGoodVertex(reco::Vertex const& vertex) const;
  bool isIsolationNeighbor(reco::Track const& track) const;
  std::pair<double, double> chargedIsolation(std::size_t candidateIndex,
                                             reco::TrackCollection const& tracks,
                                             reco::Vertex const& leadingVertex) const;
  MuonMatchResult matchMuon(reco::TrackRef const& trackRef,
                            reco::MuonCollection const& muons,
                            reco::Vertex const& leadingVertex) const;
  void fillPFLinkingDiagnostics(reco::TrackRef const& trackRef,
                                reco::Track const& track,
                                reco::PFBlockCollection const& pfBlocks,
                                unsigned int numberOfGoodVertices,
                                MuonMatchResult const& muonMatch);

  edm::EDGetTokenT<reco::TrackCollection> tracksToken_;
  edm::EDGetTokenT<reco::VertexCollection> verticesToken_;
  edm::EDGetTokenT<reco::PFBlockCollection> pfBlocksToken_;
  edm::EDGetTokenT<reco::MuonCollection> muonsToken_;
  edm::ESGetToken<PerformancePayload, PFCalibrationRcd> pfCalibrationToken_;

  PFEnergyCalibration pfEnergyCalibration_;

  double minVertexNdof_;
  double maxVertexAbsZ_;
  double maxVertexRho_;
  reco::TrackBase::TrackQuality trackQuality_;
  double minTrackPt_;
  double minTrackP_;
  double maxTrackAbsEta_;
  double maxTrackNormalizedChi2_;
  double maxTrackRelativePError_;
  unsigned int minTrackerLayers_;
  unsigned int minPixelLayers_;
  double maxTrackAbsDxy_;
  double maxTrackAbsDz_;
  double isolationCone_;
  double minIsolationTrackPt_;
  double maxIsolationTrackAbsEta_;
  double maxRelativeIsolation_;

  TH1D* eventCount_;
  TH1D* eventCutflow_;
  TH1D* trackCutflow_;
  TH1D* numberOfVertices_;
  TH1D* numberOfGoodVertices_;
  TH1D* numberOfTracks_;
  TH1D* numberOfPFBlocks_;
  TH1D* numberOfSelectedTracks_;
  TH1D* leadingVertexZ_;
  TH1D* leadingVertexRho_;
  TH1D* leadingVertexNdof_;
  TH1D* selectedTrackP_;
  TH1D* selectedTrackPt_;
  TH1D* selectedTrackEta_;
  TH1D* selectedTrackPhi_;
  TH1D* selectedTrackNormalizedChi2_;
  TH1D* selectedTrackRelativePError_;
  TH1D* selectedTrackTrackerLayers_;
  TH1D* selectedTrackPixelLayers_;
  TH1D* selectedTrackDxy_;
  TH1D* selectedTrackDz_;
  TH1D* selectedTrackSamePVRelativeIsolation_;
  TH1D* selectedTrackAllVertexRelativeIsolation_;
  TH1D* muonMatchMultiplicity_;
  TH1D* muonIDSummary_;
  TH1D* selectedTrackMuonCategory_;
  TH1D* exactMuonMatchedTrackP_;
  TH1D* muonTaggedTrackP_;
  TH1D* nonMuonTrackP_;
  TH1D* pfLinkingCutflow_;
  TH1D* pfBlockMatchMultiplicity_;
  TH1D* matchedPFBlockNumberOfElements_;
  TH1D* matchedPFBlockNumberOfTrackElements_;
  TH1D* matchedPFBlockNumberOfECALElements_;
  TH1D* matchedPFBlockNumberOfHCALElements_;
  TH1D* matchedPFBlockNumberOfHOElements_;
  TH1D* matchedPFBlockNumberOfOtherElements_;
  TH1D* linkedECALClusterMultiplicity_;
  TH1D* linkedHCALClusterMultiplicity_;
  TH1D* linkedHOClusterMultiplicity_;
  TH1D* trackECALLinkDistance_;
  TH1D* trackHCALLinkDistance_;
  TH1D* trackHOLinkDistance_;
  TH1D* noPFBlockMatchTrackP_;
  TH1D* multiplePFBlockMatchesTrackP_;
  TH1D* noLinkedECALTrackP_;
  TH1D* noLinkedHCALTrackP_;
  TH1D* linkedECALEnergy_;
  TH1D* linkedHCALEnergy_;
  TH1D* linkedHOEnergy_;
  TH1D* linkedECALPlusHCALEnergy_;
  TH1D* linkedECALResponse_;
  TH1D* linkedHCALResponse_;
  TH1D* linkedECALPlusHCALResponse_;
  TH1D* linkedECALPlusHCALPlusHOResponse_;
  TH1D* linkedECALEnergyFraction_;
  TH1D* closestTrackECALPlusHCALResponse_;
  TH1D* pfHadronCalibratedLinkedECALEnergy_;
  TH1D* pfHadronCalibratedLinkedHCALEnergy_;
  TH1D* pfHadronCalibratedLinkedECALPlusHCALEnergy_;
  TH1D* pfHadronCalibratedLinkedECALResponse_;
  TH1D* pfHadronCalibratedLinkedHCALResponse_;
  TH1D* pfHadronCalibratedLinkedECALPlusHCALResponse_;
  TH1D* pfHadronCalibrationFactor_;
  TH1D* pfHadronCalibrationResponseChange_;
  TH1D* muonTaggedLinkedECALPlusHCALResponse_;
  TH1D* nonMuonLinkedECALPlusHCALResponse_;
  TH2D* linkedECALEnergyVsLinkedHCALEnergy_;
  TH2D* linkedECALPlusHCALResponseVsP_;
  TH2D* linkedECALPlusHCALResponseVsEta_;
  TH2D* linkedECALPlusHCALResponseVsGoodVertices_;
  TH2D* linkedECALPlusHCALResponseVsPFBlockTracks_;
  TH2D* closestTrackECALPlusHCALResponseVsP_;
  TH2D* pfHadronCalibratedLinkedECALPlusHCALResponseVsP_;
  TH2D* pfHadronCalibratedLinkedECALPlusHCALResponseVsEta_;
  TH2D* pfHadronCalibratedLinkedECALPlusHCALResponseVsGoodVertices_;
  TH2D* pfHadronCalibratedLinkedECALPlusHCALResponseVsPFBlockTracks_;
  TH2D* rawVsPFHadronCalibratedLinkedECALPlusHCALResponse_;
  TH2D* muonTaggedLinkedECALPlusHCALResponseVsP_;
  TH2D* nonMuonLinkedECALPlusHCALResponseVsP_;
  TProfile* meanTrackPtVsEta_;
  TProfile* meanSelectedTracksVsGoodVertices_;
  TProfile* exactMuonMatchEfficiencyVsP_;
  TProfile* exactMuonMatchEfficiencyVsEta_;
  TProfile* muonTagEfficiencyVsP_;
  TProfile* muonTagEfficiencyVsEta_;
  TProfile* pfBlockMatchEfficiencyVsP_;
  TProfile* pfBlockMatchEfficiencyVsEta_;
  TProfile* pfBlockMatchEfficiencyVsGoodVertices_;
  TProfile* uniquePFBlockMatchEfficiencyVsP_;
  TProfile* uniquePFBlockMatchEfficiencyVsEta_;
  TProfile* uniquePFBlockMatchEfficiencyVsGoodVertices_;
  TProfile* linkedECALEfficiencyVsP_;
  TProfile* linkedECALEfficiencyVsEta_;
  TProfile* linkedECALEfficiencyVsGoodVertices_;
  TProfile* linkedHCALEfficiencyVsP_;
  TProfile* linkedHCALEfficiencyVsEta_;
  TProfile* linkedHCALEfficiencyVsGoodVertices_;
  TProfile* meanLinkedECALPlusHCALResponseVsP_;
  TProfile* meanLinkedECALPlusHCALResponseVsEta_;
  TProfile* meanLinkedECALPlusHCALResponseVsGoodVertices_;
  TProfile* meanLinkedECALPlusHCALResponseVsPFBlockTracks_;
  TProfile* meanPFHadronCalibratedLinkedECALPlusHCALResponseVsP_;
  TProfile* meanPFHadronCalibratedLinkedECALPlusHCALResponseVsEta_;
  TProfile* meanPFHadronCalibratedLinkedECALPlusHCALResponseVsGoodVertices_;
  TProfile* meanPFHadronCalibratedLinkedECALPlusHCALResponseVsPFBlockTracks_;
  TProfile* meanMuonTaggedLinkedECALPlusHCALResponseVsP_;
  TProfile* meanNonMuonLinkedECALPlusHCALResponseVsP_;
};

SPRLowPUDataAnalyzer::SPRLowPUDataAnalyzer(edm::ParameterSet const& config)
    : tracksToken_(consumes<reco::TrackCollection>(config.getParameter<edm::InputTag>("tracks"))),
      verticesToken_(consumes<reco::VertexCollection>(config.getParameter<edm::InputTag>("vertices"))),
      pfBlocksToken_(consumes<reco::PFBlockCollection>(config.getParameter<edm::InputTag>("pfBlocks"))),
      muonsToken_(consumes<reco::MuonCollection>(config.getParameter<edm::InputTag>("muons"))),
      pfCalibrationToken_(esConsumes<edm::Transition::BeginRun>(edm::ESInputTag("", ""))),
      minVertexNdof_(config.getParameter<double>("minVertexNdof")),
      maxVertexAbsZ_(config.getParameter<double>("maxVertexAbsZ")),
      maxVertexRho_(config.getParameter<double>("maxVertexRho")),
      trackQuality_(reco::TrackBase::qualityByName(config.getParameter<std::string>("trackQuality"))),
      minTrackPt_(config.getParameter<double>("minTrackPt")),
      minTrackP_(config.getParameter<double>("minTrackP")),
      maxTrackAbsEta_(config.getParameter<double>("maxTrackAbsEta")),
      maxTrackNormalizedChi2_(config.getParameter<double>("maxTrackNormalizedChi2")),
      maxTrackRelativePError_(config.getParameter<double>("maxTrackRelativePError")),
      minTrackerLayers_(config.getParameter<unsigned int>("minTrackerLayers")),
      minPixelLayers_(config.getParameter<unsigned int>("minPixelLayers")),
      maxTrackAbsDxy_(config.getParameter<double>("maxTrackAbsDxy")),
      maxTrackAbsDz_(config.getParameter<double>("maxTrackAbsDz")),
      isolationCone_(config.getParameter<double>("isolationCone")),
      minIsolationTrackPt_(config.getParameter<double>("minIsolationTrackPt")),
      maxIsolationTrackAbsEta_(config.getParameter<double>("maxIsolationTrackAbsEta")),
      maxRelativeIsolation_(config.getParameter<double>("maxRelativeIsolation")) {
  usesResource(TFileService::kSharedResource);

  edm::Service<TFileService> fileService;
  eventCount_ = fileService->make<TH1D>("eventCount", ";;events", 1, 0.0, 1.0);
  eventCutflow_ = fileService->make<TH1D>("eventCutflow", ";event selection;events", 4, 0.5, 4.5);
  eventCutflow_->GetXaxis()->SetBinLabel(1, "All events");
  eventCutflow_->GetXaxis()->SetBinLabel(2, "Has vertex");
  eventCutflow_->GetXaxis()->SetBinLabel(3, "Good leading vertex");
  eventCutflow_->GetXaxis()->SetBinLabel(4, "Has selected track");

  trackCutflow_ = fileService->make<TH1D>("trackCutflow", ";track selection;tracks", 13, 0.5, 13.5);
  trackCutflow_->GetXaxis()->SetBinLabel(1, "All tracks");
  trackCutflow_->GetXaxis()->SetBinLabel(2, "High purity");
  trackCutflow_->GetXaxis()->SetBinLabel(3, "pT");
  trackCutflow_->GetXaxis()->SetBinLabel(4, "p");
  trackCutflow_->GetXaxis()->SetBinLabel(5, "eta");
  trackCutflow_->GetXaxis()->SetBinLabel(6, "normalized chi2");
  trackCutflow_->GetXaxis()->SetBinLabel(7, "relative p error");
  trackCutflow_->GetXaxis()->SetBinLabel(8, "tracker layers");
  trackCutflow_->GetXaxis()->SetBinLabel(9, "pixel layers");
  trackCutflow_->GetXaxis()->SetBinLabel(10, "leading-PV dxy");
  trackCutflow_->GetXaxis()->SetBinLabel(11, "leading-PV dz");
  trackCutflow_->GetXaxis()->SetBinLabel(12, "isolation");
  trackCutflow_->GetXaxis()->SetBinLabel(13, "Selected tracks");

  numberOfVertices_ =
      fileService->make<TH1D>("numberOfVertices", ";number of primary vertices;events", 100, 0.0, 100.0);
  numberOfGoodVertices_ =
      fileService->make<TH1D>("numberOfGoodVertices", ";number of good primary vertices;events", 100, 0.0, 100.0);
  numberOfTracks_ = fileService->make<TH1D>("numberOfTracks", ";number of tracks;events", 500, 0.0, 5000.0);
  numberOfPFBlocks_ = fileService->make<TH1D>("numberOfPFBlocks", ";number of PF blocks;events", 500, 0.0, 5000.0);
  numberOfSelectedTracks_ =
      fileService->make<TH1D>("numberOfSelectedTracks", ";number of selected tracks;events", 100, 0.0, 100.0);
  leadingVertexZ_ =
      fileService->make<TH1D>("leadingVertexZ", ";leading primary vertex z [cm];events", 120, -30.0, 30.0);
  leadingVertexRho_ =
      fileService->make<TH1D>("leadingVertexRho", ";leading primary vertex #rho [cm];events", 100, 0.0, 2.0);
  leadingVertexNdof_ =
      fileService->make<TH1D>("leadingVertexNdof", ";leading primary vertex ndof;events", 100, 0.0, 200.0);

  selectedTrackP_ = fileService->make<TH1D>("selectedTrackP", ";selected-track p [GeV];tracks", 100, 0.0, 100.0);
  selectedTrackPt_ = fileService->make<TH1D>("selectedTrackPt", ";selected-track p_{T} [GeV];tracks", 100, 0.0, 100.0);
  selectedTrackEta_ = fileService->make<TH1D>("selectedTrackEta", ";selected-track #eta;tracks", 60, -3.0, 3.0);
  selectedTrackPhi_ = fileService->make<TH1D>("selectedTrackPhi", ";selected-track #phi;tracks", 64, -3.2, 3.2);
  selectedTrackNormalizedChi2_ = fileService->make<TH1D>(
      "selectedTrackNormalizedChi2", ";selected-track normalized #chi^{2};tracks", 100, 0.0, 10.0);
  selectedTrackRelativePError_ =
      fileService->make<TH1D>("selectedTrackRelativePError", ";selected-track #sigma_{p}/p;tracks", 100, 0.0, 0.5);
  selectedTrackTrackerLayers_ = fileService->make<TH1D>(
      "selectedTrackTrackerLayers", ";selected-track tracker layers with measurements;tracks", 30, -0.5, 29.5);
  selectedTrackPixelLayers_ = fileService->make<TH1D>(
      "selectedTrackPixelLayers", ";selected-track pixel layers with measurements;tracks", 10, -0.5, 9.5);
  selectedTrackDxy_ =
      fileService->make<TH1D>("selectedTrackDxy", ";selected-track d_{xy}(leading PV) [cm];tracks", 100, -0.1, 0.1);
  selectedTrackDz_ =
      fileService->make<TH1D>("selectedTrackDz", ";selected-track d_{z}(leading PV) [cm];tracks", 100, -0.2, 0.2);
  selectedTrackSamePVRelativeIsolation_ = fileService->make<TH1D>(
      "selectedTrackSamePVRelativeIsolation", ";selected-track same-PV relative isolation;tracks", 100, 0.0, 2.0);
  selectedTrackAllVertexRelativeIsolation_ = fileService->make<TH1D>(
      "selectedTrackAllVertexRelativeIsolation", ";selected-track all-vertex relative isolation;tracks", 100, 0.0, 2.0);

  muonMatchMultiplicity_ = fileService->make<TH1D>(
      "muonMatchMultiplicity", ";exact reco::Muon inner-track matches;selected tracks", 6, -0.5, 5.5);
  muonIDSummary_ = fileService->make<TH1D>("muonIDSummary", ";muon matching and ID;selected tracks", 7, 0.5, 7.5);
  muonIDSummary_->GetXaxis()->SetBinLabel(1, "Selected tracks");
  muonIDSummary_->GetXaxis()->SetBinLabel(2, "Exact match");
  muonIDSummary_->GetXaxis()->SetBinLabel(3, "Soft");
  muonIDSummary_->GetXaxis()->SetBinLabel(4, "Loose");
  muonIDSummary_->GetXaxis()->SetBinLabel(5, "Soft or Loose");
  muonIDSummary_->GetXaxis()->SetBinLabel(6, "Medium");
  muonIDSummary_->GetXaxis()->SetBinLabel(7, "Tight");
  selectedTrackMuonCategory_ =
      fileService->make<TH1D>("selectedTrackMuonCategory", ";muon category;selected tracks", 5, 0.5, 5.5);
  selectedTrackMuonCategory_->GetXaxis()->SetBinLabel(1, "No exact match");
  selectedTrackMuonCategory_->GetXaxis()->SetBinLabel(2, "Exact, fail Soft/Loose");
  selectedTrackMuonCategory_->GetXaxis()->SetBinLabel(3, "Soft only");
  selectedTrackMuonCategory_->GetXaxis()->SetBinLabel(4, "Loose only");
  selectedTrackMuonCategory_->GetXaxis()->SetBinLabel(5, "Soft and Loose");
  exactMuonMatchedTrackP_ =
      fileService->make<TH1D>("exactMuonMatchedTrackP", ";selected-track p [GeV];exact muon matches", 100, 0.0, 100.0);
  muonTaggedTrackP_ =
      fileService->make<TH1D>("muonTaggedTrackP", ";selected-track p [GeV];muon-tagged tracks", 100, 0.0, 100.0);
  nonMuonTrackP_ = fileService->make<TH1D>("nonMuonTrackP", ";selected-track p [GeV];non-muon tracks", 100, 0.0, 100.0);

  pfLinkingCutflow_ = fileService->make<TH1D>("pfLinkingCutflow", ";PF linking;selected tracks", 7, 0.5, 7.5);
  pfLinkingCutflow_->GetXaxis()->SetBinLabel(1, "Selected tracks");
  pfLinkingCutflow_->GetXaxis()->SetBinLabel(2, "Found PF block");
  pfLinkingCutflow_->GetXaxis()->SetBinLabel(3, "Exactly one PF block");
  pfLinkingCutflow_->GetXaxis()->SetBinLabel(4, "Linked ECAL");
  pfLinkingCutflow_->GetXaxis()->SetBinLabel(5, "Linked HCAL");
  pfLinkingCutflow_->GetXaxis()->SetBinLabel(6, "Linked ECAL or HCAL");
  pfLinkingCutflow_->GetXaxis()->SetBinLabel(7, "Linked ECAL and HCAL");

  pfBlockMatchMultiplicity_ = fileService->make<TH1D>(
      "pfBlockMatchMultiplicity", ";number of PF blocks containing selected track;selected tracks", 6, -0.5, 5.5);
  matchedPFBlockNumberOfElements_ =
      fileService->make<TH1D>("matchedPFBlockNumberOfElements",
                              ";number of elements in matched PF block;uniquely matched selected tracks",
                              100,
                              -0.5,
                              99.5);
  matchedPFBlockNumberOfTrackElements_ =
      fileService->make<TH1D>("matchedPFBlockNumberOfTrackElements",
                              ";TRACK elements in matched PF block;uniquely matched selected tracks",
                              50,
                              -0.5,
                              49.5);
  matchedPFBlockNumberOfECALElements_ =
      fileService->make<TH1D>("matchedPFBlockNumberOfECALElements",
                              ";ECAL elements in matched PF block;uniquely matched selected tracks",
                              50,
                              -0.5,
                              49.5);
  matchedPFBlockNumberOfHCALElements_ =
      fileService->make<TH1D>("matchedPFBlockNumberOfHCALElements",
                              ";HCAL elements in matched PF block;uniquely matched selected tracks",
                              50,
                              -0.5,
                              49.5);
  matchedPFBlockNumberOfHOElements_ =
      fileService->make<TH1D>("matchedPFBlockNumberOfHOElements",
                              ";HO elements in matched PF block;uniquely matched selected tracks",
                              20,
                              -0.5,
                              19.5);
  matchedPFBlockNumberOfOtherElements_ =
      fileService->make<TH1D>("matchedPFBlockNumberOfOtherElements",
                              ";other elements in matched PF block;uniquely matched selected tracks",
                              50,
                              -0.5,
                              49.5);
  linkedECALClusterMultiplicity_ =
      fileService->make<TH1D>("linkedECALClusterMultiplicity",
                              ";directly linked ECAL PF clusters;uniquely matched selected tracks",
                              20,
                              -0.5,
                              19.5);
  linkedHCALClusterMultiplicity_ =
      fileService->make<TH1D>("linkedHCALClusterMultiplicity",
                              ";directly linked HCAL PF clusters;uniquely matched selected tracks",
                              20,
                              -0.5,
                              19.5);
  linkedHOClusterMultiplicity_ = fileService->make<TH1D>(
      "linkedHOClusterMultiplicity", ";directly linked HO PF clusters;uniquely matched selected tracks", 20, -0.5, 19.5);
  trackECALLinkDistance_ =
      fileService->make<TH1D>("trackECALLinkDistance", ";stored track-ECAL PF link distance;links", 200, 0.0, 2.0);
  trackHCALLinkDistance_ =
      fileService->make<TH1D>("trackHCALLinkDistance", ";stored track-HCAL PF link distance;links", 200, 0.0, 2.0);
  trackHOLinkDistance_ =
      fileService->make<TH1D>("trackHOLinkDistance", ";stored track-HO PF link distance;links", 200, 0.0, 2.0);
  noPFBlockMatchTrackP_ = fileService->make<TH1D>(
      "noPFBlockMatchTrackP", ";p [GeV];selected tracks without a PF-block match", 100, 0.0, 100.0);
  multiplePFBlockMatchesTrackP_ = fileService->make<TH1D>(
      "multiplePFBlockMatchesTrackP", ";p [GeV];selected tracks with multiple PF-block matches", 100, 0.0, 100.0);
  noLinkedECALTrackP_ = fileService->make<TH1D>(
      "noLinkedECALTrackP", ";p [GeV];uniquely matched selected tracks without a linked ECAL cluster", 100, 0.0, 100.0);
  noLinkedHCALTrackP_ = fileService->make<TH1D>(
      "noLinkedHCALTrackP", ";p [GeV];uniquely matched selected tracks without a linked HCAL cluster", 100, 0.0, 100.0);

  linkedECALEnergy_ =
      fileService->make<TH1D>("linkedECALEnergy",
                              ";directly linked ECAL PF-cluster energy [GeV];uniquely matched selected tracks",
                              200,
                              0.0,
                              200.0);
  linkedHCALEnergy_ =
      fileService->make<TH1D>("linkedHCALEnergy",
                              ";directly linked HCAL PF-cluster energy [GeV];uniquely matched selected tracks",
                              200,
                              0.0,
                              200.0);
  linkedHOEnergy_ = fileService->make<TH1D>(
      "linkedHOEnergy", ";directly linked HO PF-cluster energy [GeV];uniquely matched selected tracks", 100, 0.0, 50.0);
  linkedECALPlusHCALEnergy_ =
      fileService->make<TH1D>("linkedECALPlusHCALEnergy",
                              ";directly linked ECAL+HCAL PF-cluster energy [GeV];uniquely matched selected tracks",
                              200,
                              0.0,
                              200.0);
  linkedECALResponse_ =
      fileService->make<TH1D>("linkedECALResponse",
                              ";directly linked ECAL PF-cluster energy / p;uniquely matched selected tracks",
                              200,
                              0.0,
                              5.0);
  linkedHCALResponse_ =
      fileService->make<TH1D>("linkedHCALResponse",
                              ";directly linked HCAL PF-cluster energy / p;uniquely matched selected tracks",
                              200,
                              0.0,
                              5.0);
  linkedECALPlusHCALResponse_ =
      fileService->make<TH1D>("linkedECALPlusHCALResponse",
                              ";directly linked ECAL+HCAL PF-cluster energy / p;uniquely matched selected tracks",
                              200,
                              0.0,
                              5.0);
  linkedECALPlusHCALPlusHOResponse_ =
      fileService->make<TH1D>("linkedECALPlusHCALPlusHOResponse",
                              ";directly linked ECAL+HCAL+HO PF-cluster energy / p;uniquely matched selected tracks",
                              200,
                              0.0,
                              5.0);
  linkedECALEnergyFraction_ =
      fileService->make<TH1D>("linkedECALEnergyFraction",
                              ";ECAL / (ECAL+HCAL) directly linked PF-cluster energy;tracks with ECAL+HCAL energy > 0",
                              100,
                              0.0,
                              1.0);
  closestTrackECALPlusHCALResponse_ =
      fileService->make<TH1D>("closestTrackECALPlusHCALResponse",
                              ";closest-track ECAL+HCAL PF-cluster energy / p;uniquely matched selected tracks",
                              200,
                              0.0,
                              5.0);
  pfHadronCalibratedLinkedECALEnergy_ =
      fileService->make<TH1D>("pfHadronCalibratedLinkedECALEnergy",
                              ";PF hadron-calibrated linked ECAL energy [GeV];uniquely matched selected tracks",
                              200,
                              0.0,
                              200.0);
  pfHadronCalibratedLinkedHCALEnergy_ =
      fileService->make<TH1D>("pfHadronCalibratedLinkedHCALEnergy",
                              ";PF hadron-calibrated linked HCAL energy [GeV];uniquely matched selected tracks",
                              200,
                              0.0,
                              200.0);
  pfHadronCalibratedLinkedECALPlusHCALEnergy_ =
      fileService->make<TH1D>("pfHadronCalibratedLinkedECALPlusHCALEnergy",
                              ";PF hadron-calibrated linked ECAL+HCAL energy [GeV];uniquely matched selected tracks",
                              200,
                              0.0,
                              200.0);
  pfHadronCalibratedLinkedECALResponse_ =
      fileService->make<TH1D>("pfHadronCalibratedLinkedECALResponse",
                              ";PF hadron-calibrated linked ECAL energy / p;uniquely matched selected tracks",
                              200,
                              0.0,
                              5.0);
  pfHadronCalibratedLinkedHCALResponse_ =
      fileService->make<TH1D>("pfHadronCalibratedLinkedHCALResponse",
                              ";PF hadron-calibrated linked HCAL energy / p;uniquely matched selected tracks",
                              200,
                              0.0,
                              5.0);
  pfHadronCalibratedLinkedECALPlusHCALResponse_ =
      fileService->make<TH1D>("pfHadronCalibratedLinkedECALPlusHCALResponse",
                              ";PF hadron-calibrated linked ECAL+HCAL energy / p;uniquely matched selected tracks",
                              200,
                              0.0,
                              5.0);
  pfHadronCalibrationFactor_ =
      fileService->make<TH1D>("pfHadronCalibrationFactor",
                              ";PF hadron-calibrated energy / raw energy;tracks with raw energy > 0",
                              1000,
                              0.0,
                              50.0);
  pfHadronCalibrationResponseChange_ =
      fileService->make<TH1D>("pfHadronCalibrationResponseChange",
                              ";PF hadron-calibrated response - raw response;uniquely matched selected tracks",
                              400,
                              -5.0,
                              5.0);
  muonTaggedLinkedECALPlusHCALResponse_ =
      fileService->make<TH1D>("muonTaggedLinkedECALPlusHCALResponse",
                              ";directly linked ECAL+HCAL PF-cluster energy / p;muon-tagged tracks",
                              200,
                              0.0,
                              5.0);
  nonMuonLinkedECALPlusHCALResponse_ =
      fileService->make<TH1D>("nonMuonLinkedECALPlusHCALResponse",
                              ";directly linked ECAL+HCAL PF-cluster energy / p;non-muon tracks",
                              200,
                              0.0,
                              5.0);

  linkedECALEnergyVsLinkedHCALEnergy_ = fileService->make<TH2D>(
      "linkedECALEnergyVsLinkedHCALEnergy",
      ";directly linked ECAL PF-cluster energy [GeV];directly linked HCAL PF-cluster energy [GeV]",
      200,
      0.0,
      200.0,
      200,
      0.0,
      200.0);
  linkedECALPlusHCALResponseVsP_ =
      fileService->make<TH2D>("linkedECALPlusHCALResponseVsP",
                              ";selected-track p [GeV];directly linked ECAL+HCAL PF-cluster energy / p",
                              100,
                              0.0,
                              100.0,
                              200,
                              0.0,
                              5.0);
  linkedECALPlusHCALResponseVsEta_ =
      fileService->make<TH2D>("linkedECALPlusHCALResponseVsEta",
                              ";selected-track #eta;directly linked ECAL+HCAL PF-cluster energy / p",
                              40,
                              -maxTrackAbsEta_,
                              maxTrackAbsEta_,
                              200,
                              0.0,
                              5.0);
  linkedECALPlusHCALResponseVsGoodVertices_ =
      fileService->make<TH2D>("linkedECALPlusHCALResponseVsGoodVertices",
                              ";number of good primary vertices;directly linked ECAL+HCAL PF-cluster energy / p",
                              20,
                              -0.5,
                              19.5,
                              200,
                              0.0,
                              5.0);
  linkedECALPlusHCALResponseVsPFBlockTracks_ =
      fileService->make<TH2D>("linkedECALPlusHCALResponseVsPFBlockTracks",
                              ";TRACK elements in matched PF block;directly linked ECAL+HCAL PF-cluster energy / p",
                              20,
                              -0.5,
                              19.5,
                              200,
                              0.0,
                              5.0);
  closestTrackECALPlusHCALResponseVsP_ =
      fileService->make<TH2D>("closestTrackECALPlusHCALResponseVsP",
                              ";selected-track p [GeV];closest-track ECAL+HCAL PF-cluster energy / p",
                              100,
                              0.0,
                              100.0,
                              200,
                              0.0,
                              5.0);
  pfHadronCalibratedLinkedECALPlusHCALResponseVsP_ =
      fileService->make<TH2D>("pfHadronCalibratedLinkedECALPlusHCALResponseVsP",
                              ";selected-track p [GeV];PF hadron-calibrated linked ECAL+HCAL energy / p",
                              100,
                              0.0,
                              100.0,
                              200,
                              0.0,
                              5.0);
  pfHadronCalibratedLinkedECALPlusHCALResponseVsEta_ =
      fileService->make<TH2D>("pfHadronCalibratedLinkedECALPlusHCALResponseVsEta",
                              ";selected-track #eta;PF hadron-calibrated linked ECAL+HCAL energy / p",
                              40,
                              -maxTrackAbsEta_,
                              maxTrackAbsEta_,
                              200,
                              0.0,
                              5.0);
  pfHadronCalibratedLinkedECALPlusHCALResponseVsGoodVertices_ =
      fileService->make<TH2D>("pfHadronCalibratedLinkedECALPlusHCALResponseVsGoodVertices",
                              ";number of good primary vertices;PF hadron-calibrated linked ECAL+HCAL energy / p",
                              20,
                              -0.5,
                              19.5,
                              200,
                              0.0,
                              5.0);
  pfHadronCalibratedLinkedECALPlusHCALResponseVsPFBlockTracks_ =
      fileService->make<TH2D>("pfHadronCalibratedLinkedECALPlusHCALResponseVsPFBlockTracks",
                              ";TRACK elements in matched PF block;PF hadron-calibrated linked ECAL+HCAL energy / p",
                              20,
                              -0.5,
                              19.5,
                              200,
                              0.0,
                              5.0);
  rawVsPFHadronCalibratedLinkedECALPlusHCALResponse_ =
      fileService->make<TH2D>("rawVsPFHadronCalibratedLinkedECALPlusHCALResponse",
                              ";raw linked ECAL+HCAL energy / p;PF hadron-calibrated linked ECAL+HCAL energy / p",
                              200,
                              0.0,
                              5.0,
                              200,
                              0.0,
                              5.0);
  muonTaggedLinkedECALPlusHCALResponseVsP_ =
      fileService->make<TH2D>("muonTaggedLinkedECALPlusHCALResponseVsP",
                              ";selected-track p [GeV];directly linked ECAL+HCAL PF-cluster energy / p",
                              100,
                              0.0,
                              100.0,
                              200,
                              0.0,
                              5.0);
  nonMuonLinkedECALPlusHCALResponseVsP_ =
      fileService->make<TH2D>("nonMuonLinkedECALPlusHCALResponseVsP",
                              ";selected-track p [GeV];directly linked ECAL+HCAL PF-cluster energy / p",
                              100,
                              0.0,
                              100.0,
                              200,
                              0.0,
                              5.0);

  meanTrackPtVsEta_ =
      fileService->make<TProfile>("meanTrackPtVsEta", ";track #eta;mean track p_{T} [GeV]", 60, -3.0, 3.0);
  meanTrackPtVsEta_->SetMinimum(0.0);
  meanTrackPtVsEta_->SetMaximum(100.0);
  meanSelectedTracksVsGoodVertices_ = fileService->make<TProfile>(
      "meanSelectedTracksVsGoodVertices", ";number of good primary vertices;mean selected tracks", 20, -0.5, 19.5);

  exactMuonMatchEfficiencyVsP_ = fileService->make<TProfile>(
      "exactMuonMatchEfficiencyVsP", ";selected-track p [GeV];fraction with an exact muon match", 100, 0.0, 100.0);
  exactMuonMatchEfficiencyVsEta_ = fileService->make<TProfile>("exactMuonMatchEfficiencyVsEta",
                                                               ";selected-track #eta;fraction with an exact muon match",
                                                               40,
                                                               -maxTrackAbsEta_,
                                                               maxTrackAbsEta_);
  muonTagEfficiencyVsP_ = fileService->make<TProfile>(
      "muonTagEfficiencyVsP", ";selected-track p [GeV];fraction tagged as Soft or Loose muon", 100, 0.0, 100.0);
  muonTagEfficiencyVsEta_ = fileService->make<TProfile>("muonTagEfficiencyVsEta",
                                                        ";selected-track #eta;fraction tagged as Soft or Loose muon",
                                                        40,
                                                        -maxTrackAbsEta_,
                                                        maxTrackAbsEta_);

  pfBlockMatchEfficiencyVsP_ = fileService->make<TProfile>(
      "pfBlockMatchEfficiencyVsP", ";selected-track p [GeV];fraction with a PF-block match", 100, 0.0, 100.0);
  pfBlockMatchEfficiencyVsEta_ = fileService->make<TProfile>("pfBlockMatchEfficiencyVsEta",
                                                             ";selected-track #eta;fraction with a PF-block match",
                                                             40,
                                                             -maxTrackAbsEta_,
                                                             maxTrackAbsEta_);
  pfBlockMatchEfficiencyVsGoodVertices_ =
      fileService->make<TProfile>("pfBlockMatchEfficiencyVsGoodVertices",
                                  ";number of good primary vertices;fraction with a PF-block match",
                                  20,
                                  -0.5,
                                  19.5);
  uniquePFBlockMatchEfficiencyVsP_ =
      fileService->make<TProfile>("uniquePFBlockMatchEfficiencyVsP",
                                  ";selected-track p [GeV];fraction with exactly one PF-block match",
                                  100,
                                  0.0,
                                  100.0);
  uniquePFBlockMatchEfficiencyVsEta_ =
      fileService->make<TProfile>("uniquePFBlockMatchEfficiencyVsEta",
                                  ";selected-track #eta;fraction with exactly one PF-block match",
                                  40,
                                  -maxTrackAbsEta_,
                                  maxTrackAbsEta_);
  uniquePFBlockMatchEfficiencyVsGoodVertices_ =
      fileService->make<TProfile>("uniquePFBlockMatchEfficiencyVsGoodVertices",
                                  ";number of good primary vertices;fraction with exactly one PF-block match",
                                  20,
                                  -0.5,
                                  19.5);
  linkedECALEfficiencyVsP_ = fileService->make<TProfile>(
      "linkedECALEfficiencyVsP", ";selected-track p [GeV];fraction with a direct ECAL PF link", 100, 0.0, 100.0);
  linkedECALEfficiencyVsEta_ = fileService->make<TProfile>("linkedECALEfficiencyVsEta",
                                                           ";selected-track #eta;fraction with a direct ECAL PF link",
                                                           40,
                                                           -maxTrackAbsEta_,
                                                           maxTrackAbsEta_);
  linkedECALEfficiencyVsGoodVertices_ =
      fileService->make<TProfile>("linkedECALEfficiencyVsGoodVertices",
                                  ";number of good primary vertices;fraction with a direct ECAL PF link",
                                  20,
                                  -0.5,
                                  19.5);
  linkedHCALEfficiencyVsP_ = fileService->make<TProfile>(
      "linkedHCALEfficiencyVsP", ";selected-track p [GeV];fraction with a direct HCAL PF link", 100, 0.0, 100.0);
  linkedHCALEfficiencyVsEta_ = fileService->make<TProfile>("linkedHCALEfficiencyVsEta",
                                                           ";selected-track #eta;fraction with a direct HCAL PF link",
                                                           40,
                                                           -maxTrackAbsEta_,
                                                           maxTrackAbsEta_);
  linkedHCALEfficiencyVsGoodVertices_ =
      fileService->make<TProfile>("linkedHCALEfficiencyVsGoodVertices",
                                  ";number of good primary vertices;fraction with a direct HCAL PF link",
                                  20,
                                  -0.5,
                                  19.5);
  meanLinkedECALPlusHCALResponseVsP_ =
      fileService->make<TProfile>("meanLinkedECALPlusHCALResponseVsP",
                                  ";selected-track p [GeV];mean directly linked ECAL+HCAL PF-cluster energy / p",
                                  100,
                                  0.0,
                                  100.0);
  meanLinkedECALPlusHCALResponseVsEta_ =
      fileService->make<TProfile>("meanLinkedECALPlusHCALResponseVsEta",
                                  ";selected-track #eta;mean directly linked ECAL+HCAL PF-cluster energy / p",
                                  40,
                                  -maxTrackAbsEta_,
                                  maxTrackAbsEta_);
  meanLinkedECALPlusHCALResponseVsGoodVertices_ = fileService->make<TProfile>(
      "meanLinkedECALPlusHCALResponseVsGoodVertices",
      ";number of good primary vertices;mean directly linked ECAL+HCAL PF-cluster energy / p",
      20,
      -0.5,
      19.5);
  meanLinkedECALPlusHCALResponseVsPFBlockTracks_ = fileService->make<TProfile>(
      "meanLinkedECALPlusHCALResponseVsPFBlockTracks",
      ";TRACK elements in matched PF block;mean directly linked ECAL+HCAL PF-cluster energy / p",
      20,
      -0.5,
      19.5);
  meanPFHadronCalibratedLinkedECALPlusHCALResponseVsP_ =
      fileService->make<TProfile>("meanPFHadronCalibratedLinkedECALPlusHCALResponseVsP",
                                  ";selected-track p [GeV];mean PF hadron-calibrated linked ECAL+HCAL energy / p",
                                  100,
                                  0.0,
                                  100.0);
  meanPFHadronCalibratedLinkedECALPlusHCALResponseVsEta_ =
      fileService->make<TProfile>("meanPFHadronCalibratedLinkedECALPlusHCALResponseVsEta",
                                  ";selected-track #eta;mean PF hadron-calibrated linked ECAL+HCAL energy / p",
                                  40,
                                  -maxTrackAbsEta_,
                                  maxTrackAbsEta_);
  meanPFHadronCalibratedLinkedECALPlusHCALResponseVsGoodVertices_ = fileService->make<TProfile>(
      "meanPFHadronCalibratedLinkedECALPlusHCALResponseVsGoodVertices",
      ";number of good primary vertices;mean PF hadron-calibrated linked ECAL+HCAL energy / p",
      20,
      -0.5,
      19.5);
  meanPFHadronCalibratedLinkedECALPlusHCALResponseVsPFBlockTracks_ = fileService->make<TProfile>(
      "meanPFHadronCalibratedLinkedECALPlusHCALResponseVsPFBlockTracks",
      ";TRACK elements in matched PF block;mean PF hadron-calibrated linked ECAL+HCAL energy / p",
      20,
      -0.5,
      19.5);
  meanMuonTaggedLinkedECALPlusHCALResponseVsP_ = fileService->make<TProfile>(
      "meanMuonTaggedLinkedECALPlusHCALResponseVsP",
      ";selected-track p [GeV];mean directly linked ECAL+HCAL PF-cluster energy / p for muon-tagged tracks",
      100,
      0.0,
      100.0);
  meanNonMuonLinkedECALPlusHCALResponseVsP_ = fileService->make<TProfile>(
      "meanNonMuonLinkedECALPlusHCALResponseVsP",
      ";selected-track p [GeV];mean directly linked ECAL+HCAL PF-cluster energy / p for non-muon tracks",
      100,
      0.0,
      100.0);
}

void SPRLowPUDataAnalyzer::beginRun(edm::Run const&, edm::EventSetup const& setup) {
  auto const payloadHandle = setup.getHandle(pfCalibrationToken_);
  auto const* calibrationFunctions = static_cast<PerformancePayloadFromTFormula const*>(payloadHandle.product());
  pfEnergyCalibration_.setCalibrationFunctions(calibrationFunctions);
}

void SPRLowPUDataAnalyzer::analyze(edm::Event const& event, edm::EventSetup const&) {
  auto const tracksHandle = event.getHandle(tracksToken_);
  auto const& tracks = *tracksHandle;
  auto const& vertices = event.get(verticesToken_);
  auto const& pfBlocks = event.get(pfBlocksToken_);
  auto const& muons = event.get(muonsToken_);

  eventCount_->Fill(0.5);
  numberOfVertices_->Fill(vertices.size());
  numberOfTracks_->Fill(tracks.size());
  numberOfPFBlocks_->Fill(pfBlocks.size());

  for (auto const& track : tracks) {
    meanTrackPtVsEta_->Fill(track.eta(), track.pt());
  }

  eventCutflow_->Fill(1.0);

  unsigned int numberOfGoodVertices = 0;
  for (auto const& vertex : vertices) {
    if (isGoodVertex(vertex)) {
      ++numberOfGoodVertices;
    }
  }
  numberOfGoodVertices_->Fill(numberOfGoodVertices);

  if (vertices.empty()) {
    return;
  }
  eventCutflow_->Fill(2.0);

  reco::Vertex const& leadingVertex = vertices.front();
  leadingVertexZ_->Fill(leadingVertex.z());
  leadingVertexRho_->Fill(leadingVertex.position().Rho());
  leadingVertexNdof_->Fill(leadingVertex.ndof());

  if (!isGoodVertex(leadingVertex)) {
    return;
  }
  eventCutflow_->Fill(3.0);

  unsigned int numberOfSelectedTracks = 0;
  for (std::size_t trackIndex = 0; trackIndex < tracks.size(); ++trackIndex) {
    reco::Track const& track = tracks[trackIndex];
    trackCutflow_->Fill(1.0);

    if (!track.quality(trackQuality_)) {
      continue;
    }
    trackCutflow_->Fill(2.0);

    if (minTrackPt_ >= 0.0 && track.pt() < minTrackPt_) {
      continue;
    }
    trackCutflow_->Fill(3.0);

    if (track.p() < minTrackP_) {
      continue;
    }
    trackCutflow_->Fill(4.0);

    if (std::abs(track.eta()) > maxTrackAbsEta_) {
      continue;
    }
    trackCutflow_->Fill(5.0);

    if (track.normalizedChi2() > maxTrackNormalizedChi2_) {
      continue;
    }
    trackCutflow_->Fill(6.0);

    double const relativePError = track.qoverpError() / std::abs(track.qoverp());
    if (relativePError > maxTrackRelativePError_) {
      continue;
    }
    trackCutflow_->Fill(7.0);

    unsigned int const trackerLayers = track.hitPattern().trackerLayersWithMeasurement();
    if (trackerLayers < minTrackerLayers_) {
      continue;
    }
    trackCutflow_->Fill(8.0);

    unsigned int const pixelLayers = track.hitPattern().pixelLayersWithMeasurement();
    if (pixelLayers < minPixelLayers_) {
      continue;
    }
    trackCutflow_->Fill(9.0);

    double const dxy = track.dxy(leadingVertex.position());
    if (std::abs(dxy) > maxTrackAbsDxy_) {
      continue;
    }
    trackCutflow_->Fill(10.0);

    double const dz = track.dz(leadingVertex.position());
    if (std::abs(dz) > maxTrackAbsDz_) {
      continue;
    }
    trackCutflow_->Fill(11.0);

    auto const [samePVIsolation, allVertexIsolation] = chargedIsolation(trackIndex, tracks, leadingVertex);
    double const samePVRelativeIsolation = samePVIsolation / track.pt();
    double const allVertexRelativeIsolation = allVertexIsolation / track.pt();
    if (allVertexRelativeIsolation >= maxRelativeIsolation_) {
      continue;
    }
    trackCutflow_->Fill(12.0);
    trackCutflow_->Fill(13.0);

    ++numberOfSelectedTracks;
    selectedTrackP_->Fill(track.p());
    selectedTrackPt_->Fill(track.pt());
    selectedTrackEta_->Fill(track.eta());
    selectedTrackPhi_->Fill(track.phi());
    selectedTrackNormalizedChi2_->Fill(track.normalizedChi2());
    selectedTrackRelativePError_->Fill(relativePError);
    selectedTrackTrackerLayers_->Fill(trackerLayers);
    selectedTrackPixelLayers_->Fill(pixelLayers);
    selectedTrackDxy_->Fill(dxy);
    selectedTrackDz_->Fill(dz);
    selectedTrackSamePVRelativeIsolation_->Fill(samePVRelativeIsolation);
    selectedTrackAllVertexRelativeIsolation_->Fill(allVertexRelativeIsolation);

    reco::TrackRef const trackRef(tracksHandle, trackIndex);
    MuonMatchResult const muonMatch = matchMuon(trackRef, muons, leadingVertex);
    muonMatchMultiplicity_->Fill(muonMatch.multiplicity);
    muonIDSummary_->Fill(1.0);
    if (muonMatch.hasExactMatch()) {
      muonIDSummary_->Fill(2.0);
      exactMuonMatchedTrackP_->Fill(track.p());
    }
    if (muonMatch.passesSoft) {
      muonIDSummary_->Fill(3.0);
    }
    if (muonMatch.passesLoose) {
      muonIDSummary_->Fill(4.0);
    }
    if (muonMatch.isMuonTagged()) {
      muonIDSummary_->Fill(5.0);
      muonTaggedTrackP_->Fill(track.p());
    } else {
      nonMuonTrackP_->Fill(track.p());
    }
    if (muonMatch.passesMedium) {
      muonIDSummary_->Fill(6.0);
    }
    if (muonMatch.passesTight) {
      muonIDSummary_->Fill(7.0);
    }

    if (!muonMatch.hasExactMatch()) {
      selectedTrackMuonCategory_->Fill(1.0);
    } else if (!muonMatch.isMuonTagged()) {
      selectedTrackMuonCategory_->Fill(2.0);
    } else if (muonMatch.passesSoft && !muonMatch.passesLoose) {
      selectedTrackMuonCategory_->Fill(3.0);
    } else if (!muonMatch.passesSoft && muonMatch.passesLoose) {
      selectedTrackMuonCategory_->Fill(4.0);
    } else {
      selectedTrackMuonCategory_->Fill(5.0);
    }

    exactMuonMatchEfficiencyVsP_->Fill(track.p(), muonMatch.hasExactMatch());
    exactMuonMatchEfficiencyVsEta_->Fill(track.eta(), muonMatch.hasExactMatch());
    muonTagEfficiencyVsP_->Fill(track.p(), muonMatch.isMuonTagged());
    muonTagEfficiencyVsEta_->Fill(track.eta(), muonMatch.isMuonTagged());

    fillPFLinkingDiagnostics(trackRef, track, pfBlocks, numberOfGoodVertices, muonMatch);
  }

  numberOfSelectedTracks_->Fill(numberOfSelectedTracks);
  meanSelectedTracksVsGoodVertices_->Fill(numberOfGoodVertices, numberOfSelectedTracks);
  if (numberOfSelectedTracks > 0) {
    eventCutflow_->Fill(4.0);
  }
}

bool SPRLowPUDataAnalyzer::isGoodVertex(reco::Vertex const& vertex) const {
  return !vertex.isFake() && vertex.ndof() > minVertexNdof_ && std::abs(vertex.z()) < maxVertexAbsZ_ &&
         vertex.position().Rho() < maxVertexRho_;
}

bool SPRLowPUDataAnalyzer::isIsolationNeighbor(reco::Track const& track) const {
  return track.quality(trackQuality_) && track.pt() > minIsolationTrackPt_ &&
         std::abs(track.eta()) < maxIsolationTrackAbsEta_;
}

std::pair<double, double> SPRLowPUDataAnalyzer::chargedIsolation(std::size_t candidateIndex,
                                                                 reco::TrackCollection const& tracks,
                                                                 reco::Vertex const& leadingVertex) const {
  double samePVIsolation = 0.0;
  double allVertexIsolation = 0.0;
  double const isolationCone2 = isolationCone_ * isolationCone_;
  reco::Track const& candidate = tracks[candidateIndex];

  for (std::size_t neighborIndex = 0; neighborIndex < tracks.size(); ++neighborIndex) {
    if (neighborIndex == candidateIndex) {
      continue;
    }

    reco::Track const& neighbor = tracks[neighborIndex];
    if (!isIsolationNeighbor(neighbor)) {
      continue;
    }
    if (reco::deltaR2(candidate.eta(), candidate.phi(), neighbor.eta(), neighbor.phi()) >= isolationCone2) {
      continue;
    }

    allVertexIsolation += neighbor.pt();
    if (std::abs(neighbor.dxy(leadingVertex.position())) < maxTrackAbsDxy_ &&
        std::abs(neighbor.dz(leadingVertex.position())) < maxTrackAbsDz_) {
      samePVIsolation += neighbor.pt();
    }
  }

  return {samePVIsolation, allVertexIsolation};
}

SPRLowPUDataAnalyzer::MuonMatchResult SPRLowPUDataAnalyzer::matchMuon(reco::TrackRef const& trackRef,
                                                                      reco::MuonCollection const& muons,
                                                                      reco::Vertex const& leadingVertex) const {
  MuonMatchResult result;
  for (auto const& muonObject : muons) {
    reco::TrackRef const& innerTrack = muonObject.innerTrack();
    if (innerTrack.isNull() || !innerTrack.isAvailable() || innerTrack != trackRef) {
      continue;
    }

    ++result.multiplicity;
    result.passesSoft = result.passesSoft || muon::isSoftMuon(muonObject, leadingVertex);
    result.passesLoose = result.passesLoose || muon::isLooseMuon(muonObject);
    result.passesMedium = result.passesMedium || muon::isMediumMuon(muonObject);
    result.passesTight = result.passesTight || muon::isTightMuon(muonObject, leadingVertex);
  }
  return result;
}

void SPRLowPUDataAnalyzer::fillPFLinkingDiagnostics(reco::TrackRef const& trackRef,
                                                    reco::Track const& track,
                                                    reco::PFBlockCollection const& pfBlocks,
                                                    unsigned int numberOfGoodVertices,
                                                    MuonMatchResult const& muonMatch) {
  struct PFBlockMatch {
    reco::PFBlock const* block;
    unsigned int trackElementIndex;
  };


  //
  // Check how many PFBlocks does this track matched to
  //
  std::vector<PFBlockMatch> matches;
  for (auto const& block : pfBlocks) {
    auto const& elements = block.elements();
    for (unsigned int elementIndex = 0; elementIndex < elements.size(); ++elementIndex) {
      reco::PFBlockElement const& element = elements[elementIndex];
      if (element.type() == reco::PFBlockElement::TRACK && element.trackRef() == trackRef) {
        matches.push_back({&block, elementIndex});
        break;
      }
    }
  }

  pfLinkingCutflow_->Fill(1.0);
  pfBlockMatchMultiplicity_->Fill(matches.size());

  bool const hasPFBlockMatch = !matches.empty();
  bool const hasUniquePFBlockMatch = matches.size() == 1;
  if (hasPFBlockMatch) {
    pfLinkingCutflow_->Fill(2.0);
  } else {
    noPFBlockMatchTrackP_->Fill(track.p());
  }


  if (hasUniquePFBlockMatch) {
    pfLinkingCutflow_->Fill(3.0);
  } else if (matches.size() > 1) {
    multiplePFBlockMatchesTrackP_->Fill(track.p());
  }

  bool hasLinkedECAL = false;
  bool hasLinkedHCAL = false;
  if (hasUniquePFBlockMatch) {
    reco::PFBlock const& block = *matches.front().block;
    unsigned int const trackElementIndex = matches.front().trackElementIndex;
    auto const& elements = block.elements();

    unsigned int numberOfTrackElements = 0;
    unsigned int numberOfECALElements = 0;
    unsigned int numberOfHCALElements = 0;
    unsigned int numberOfHOElements = 0;
    unsigned int numberOfOtherElements = 0;
    for (auto const& element : elements) {
      switch (element.type()) {
        case reco::PFBlockElement::TRACK:
          ++numberOfTrackElements;
          break;
        case reco::PFBlockElement::ECAL:
          ++numberOfECALElements;
          break;
        case reco::PFBlockElement::HCAL:
          ++numberOfHCALElements;
          break;
        case reco::PFBlockElement::HO:
          ++numberOfHOElements;
          break;
        default:
          ++numberOfOtherElements;
          break;
      }
    }

    matchedPFBlockNumberOfElements_->Fill(elements.size());
    matchedPFBlockNumberOfTrackElements_->Fill(numberOfTrackElements);
    matchedPFBlockNumberOfECALElements_->Fill(numberOfECALElements);
    matchedPFBlockNumberOfHCALElements_->Fill(numberOfHCALElements);
    matchedPFBlockNumberOfHOElements_->Fill(numberOfHOElements);
    matchedPFBlockNumberOfOtherElements_->Fill(numberOfOtherElements);

    //
    // In the block, get the calorimeter cluster PFBlockElements that are linked to this track
    //
    std::multimap<double, unsigned int> linkedECAL;
    std::multimap<double, unsigned int> linkedHCAL;
    std::multimap<double, unsigned int> linkedHO;
    block.associatedElements(trackElementIndex, block.linkData(), linkedECAL, reco::PFBlockElement::ECAL, reco::PFBlock::LINKTEST_RECHIT);
    block.associatedElements(trackElementIndex, block.linkData(), linkedHCAL, reco::PFBlockElement::HCAL, reco::PFBlock::LINKTEST_RECHIT);
    block.associatedElements(trackElementIndex, block.linkData(), linkedHO,   reco::PFBlockElement::HO,   reco::PFBlock::LINKTEST_RECHIT);

    //====================================
    //
    //
    //
    //====================================
    auto const clusterIsClosestToSelectedTrack = [&](unsigned int clusterElementIndex) {
      std::multimap<double, unsigned int> linkedTracks;
      block.associatedElements(clusterElementIndex, block.linkData(), linkedTracks, reco::PFBlockElement::TRACK, reco::PFBlock::LINKTEST_ALL);
      return !linkedTracks.empty() && linkedTracks.begin()->second == trackElementIndex;
    };

    //
    // ECAL
    //
    unsigned int numberOfLinkedECALClusters = 0;
    double linkedECALEnergy = 0.0;
    double closestTrackECALEnergy = 0.0;

    for (auto const& [distance, elementIndex] : linkedECAL) {
      reco::PFClusterRef const& clusterRef = elements[elementIndex].clusterRef();

      if (clusterRef.isNonnull() && clusterRef.isAvailable()) {
        ++numberOfLinkedECALClusters;
        trackECALLinkDistance_->Fill(distance);
        linkedECALEnergy += clusterRef->energy();
        if (clusterIsClosestToSelectedTrack(elementIndex)) {
          closestTrackECALEnergy += clusterRef->energy();
        }
      }
    }

    //
    // HCAL
    //
    unsigned int numberOfLinkedHCALClusters = 0;
    double linkedHCALEnergy = 0.0;
    double closestTrackHCALEnergy = 0.0;

    for (auto const& [distance, elementIndex] : linkedHCAL) {
      reco::PFClusterRef const& clusterRef = elements[elementIndex].clusterRef();

      if (clusterRef.isNonnull() && clusterRef.isAvailable()) {
        ++numberOfLinkedHCALClusters;
        trackHCALLinkDistance_->Fill(distance);
        linkedHCALEnergy += clusterRef->energy();

        if (clusterIsClosestToSelectedTrack(elementIndex)) {
          closestTrackHCALEnergy += clusterRef->energy();
        }
      }
    }

    //
    // HO
    //
    unsigned int numberOfLinkedHOClusters = 0;
    double linkedHOEnergy = 0.0;

    for (auto const& [distance, elementIndex] : linkedHO) {
      reco::PFClusterRef const& clusterRef = elements[elementIndex].clusterRef();

      if (clusterRef.isNonnull() && clusterRef.isAvailable()) {
        ++numberOfLinkedHOClusters;
        trackHOLinkDistance_->Fill(distance);
        linkedHOEnergy += clusterRef->energy();
      }
    }

    linkedECALClusterMultiplicity_->Fill(numberOfLinkedECALClusters);
    linkedHCALClusterMultiplicity_->Fill(numberOfLinkedHCALClusters);
    linkedHOClusterMultiplicity_->Fill(numberOfLinkedHOClusters);

    hasLinkedECAL = numberOfLinkedECALClusters > 0;
    hasLinkedHCAL = numberOfLinkedHCALClusters > 0;

    double const linkedECALPlusHCALEnergy = linkedECALEnergy + linkedHCALEnergy;
    double const linkedECALResponse = linkedECALEnergy / track.p();
    double const linkedHCALResponse = linkedHCALEnergy / track.p();
    double const linkedECALPlusHCALResponse = linkedECALPlusHCALEnergy / track.p();
    double const linkedECALPlusHCALPlusHOResponse = (linkedECALPlusHCALEnergy + linkedHOEnergy) / track.p();
    double const closestTrackECALPlusHCALResponse = (closestTrackECALEnergy + closestTrackHCALEnergy) / track.p();

    //====================================
    //
    // PF hadron calibration
    //
    //====================================
    double pfHadronCalibratedLinkedECALEnergy = linkedECALEnergy;
    double pfHadronCalibratedLinkedHCALEnergy = linkedHCALEnergy;
    pfEnergyCalibration_.energyEmHad(track.p(), pfHadronCalibratedLinkedECALEnergy, pfHadronCalibratedLinkedHCALEnergy, track.eta(), track.phi());

    double const pfHadronCalibratedLinkedECALPlusHCALEnergy =
        pfHadronCalibratedLinkedECALEnergy + pfHadronCalibratedLinkedHCALEnergy;
    double const pfHadronCalibratedLinkedECALResponse = pfHadronCalibratedLinkedECALEnergy / track.p();
    double const pfHadronCalibratedLinkedHCALResponse = pfHadronCalibratedLinkedHCALEnergy / track.p();
    double const pfHadronCalibratedLinkedECALPlusHCALResponse = pfHadronCalibratedLinkedECALPlusHCALEnergy / track.p();

    linkedECALEnergy_->Fill(linkedECALEnergy);
    linkedHCALEnergy_->Fill(linkedHCALEnergy);
    linkedHOEnergy_->Fill(linkedHOEnergy);
    linkedECALPlusHCALEnergy_->Fill(linkedECALPlusHCALEnergy);
    linkedECALResponse_->Fill(linkedECALResponse);
    linkedHCALResponse_->Fill(linkedHCALResponse);
    linkedECALPlusHCALResponse_->Fill(linkedECALPlusHCALResponse);
    linkedECALPlusHCALPlusHOResponse_->Fill(linkedECALPlusHCALPlusHOResponse);
    closestTrackECALPlusHCALResponse_->Fill(closestTrackECALPlusHCALResponse);

    pfHadronCalibratedLinkedECALEnergy_->Fill(pfHadronCalibratedLinkedECALEnergy);
    pfHadronCalibratedLinkedHCALEnergy_->Fill(pfHadronCalibratedLinkedHCALEnergy);
    pfHadronCalibratedLinkedECALPlusHCALEnergy_->Fill(pfHadronCalibratedLinkedECALPlusHCALEnergy);
    pfHadronCalibratedLinkedECALResponse_->Fill(pfHadronCalibratedLinkedECALResponse);
    pfHadronCalibratedLinkedHCALResponse_->Fill(pfHadronCalibratedLinkedHCALResponse);
    pfHadronCalibratedLinkedECALPlusHCALResponse_->Fill(pfHadronCalibratedLinkedECALPlusHCALResponse);
    pfHadronCalibrationResponseChange_->Fill(pfHadronCalibratedLinkedECALPlusHCALResponse - linkedECALPlusHCALResponse);
    if (linkedECALPlusHCALEnergy > 0.0) {
      linkedECALEnergyFraction_->Fill(linkedECALEnergy / linkedECALPlusHCALEnergy);
      pfHadronCalibrationFactor_->Fill(pfHadronCalibratedLinkedECALPlusHCALEnergy / linkedECALPlusHCALEnergy);
    }

    linkedECALEnergyVsLinkedHCALEnergy_->Fill(linkedECALEnergy, linkedHCALEnergy);
    linkedECALPlusHCALResponseVsP_->Fill(track.p(), linkedECALPlusHCALResponse);
    linkedECALPlusHCALResponseVsEta_->Fill(track.eta(), linkedECALPlusHCALResponse);
    linkedECALPlusHCALResponseVsGoodVertices_->Fill(numberOfGoodVertices, linkedECALPlusHCALResponse);
    linkedECALPlusHCALResponseVsPFBlockTracks_->Fill(numberOfTrackElements, linkedECALPlusHCALResponse);
    closestTrackECALPlusHCALResponseVsP_->Fill(track.p(), closestTrackECALPlusHCALResponse);
    meanLinkedECALPlusHCALResponseVsP_->Fill(track.p(), linkedECALPlusHCALResponse);
    meanLinkedECALPlusHCALResponseVsEta_->Fill(track.eta(), linkedECALPlusHCALResponse);
    meanLinkedECALPlusHCALResponseVsGoodVertices_->Fill(numberOfGoodVertices, linkedECALPlusHCALResponse);
    meanLinkedECALPlusHCALResponseVsPFBlockTracks_->Fill(numberOfTrackElements, linkedECALPlusHCALResponse);
    pfHadronCalibratedLinkedECALPlusHCALResponseVsP_->Fill(track.p(), pfHadronCalibratedLinkedECALPlusHCALResponse);
    pfHadronCalibratedLinkedECALPlusHCALResponseVsEta_->Fill(track.eta(), pfHadronCalibratedLinkedECALPlusHCALResponse);
    pfHadronCalibratedLinkedECALPlusHCALResponseVsGoodVertices_->Fill(numberOfGoodVertices,
                                                                      pfHadronCalibratedLinkedECALPlusHCALResponse);
    pfHadronCalibratedLinkedECALPlusHCALResponseVsPFBlockTracks_->Fill(numberOfTrackElements,
                                                                       pfHadronCalibratedLinkedECALPlusHCALResponse);
    rawVsPFHadronCalibratedLinkedECALPlusHCALResponse_->Fill(linkedECALPlusHCALResponse,
                                                             pfHadronCalibratedLinkedECALPlusHCALResponse);
    meanPFHadronCalibratedLinkedECALPlusHCALResponseVsP_->Fill(track.p(), pfHadronCalibratedLinkedECALPlusHCALResponse);
    meanPFHadronCalibratedLinkedECALPlusHCALResponseVsEta_->Fill(track.eta(),
                                                                 pfHadronCalibratedLinkedECALPlusHCALResponse);
    meanPFHadronCalibratedLinkedECALPlusHCALResponseVsGoodVertices_->Fill(numberOfGoodVertices,
                                                                          pfHadronCalibratedLinkedECALPlusHCALResponse);
    meanPFHadronCalibratedLinkedECALPlusHCALResponseVsPFBlockTracks_->Fill(
        numberOfTrackElements, pfHadronCalibratedLinkedECALPlusHCALResponse);

    if (muonMatch.isMuonTagged()) {
      muonTaggedLinkedECALPlusHCALResponse_->Fill(linkedECALPlusHCALResponse);
      muonTaggedLinkedECALPlusHCALResponseVsP_->Fill(track.p(), linkedECALPlusHCALResponse);
      meanMuonTaggedLinkedECALPlusHCALResponseVsP_->Fill(track.p(), linkedECALPlusHCALResponse);
    } else {
      nonMuonLinkedECALPlusHCALResponse_->Fill(linkedECALPlusHCALResponse);
      nonMuonLinkedECALPlusHCALResponseVsP_->Fill(track.p(), linkedECALPlusHCALResponse);
      meanNonMuonLinkedECALPlusHCALResponseVsP_->Fill(track.p(), linkedECALPlusHCALResponse);
    }

    if (hasLinkedECAL) {
      pfLinkingCutflow_->Fill(4.0);
    } else {
      noLinkedECALTrackP_->Fill(track.p());
    }
    if (hasLinkedHCAL) {
      pfLinkingCutflow_->Fill(5.0);
    } else {
      noLinkedHCALTrackP_->Fill(track.p());
    }
    if (hasLinkedECAL || hasLinkedHCAL) {
      pfLinkingCutflow_->Fill(6.0);
    }
    if (hasLinkedECAL && hasLinkedHCAL) {
      pfLinkingCutflow_->Fill(7.0);
    }
  }

  pfBlockMatchEfficiencyVsP_->Fill(track.p(), hasPFBlockMatch);
  pfBlockMatchEfficiencyVsEta_->Fill(track.eta(), hasPFBlockMatch);
  pfBlockMatchEfficiencyVsGoodVertices_->Fill(numberOfGoodVertices, hasPFBlockMatch);
  uniquePFBlockMatchEfficiencyVsP_->Fill(track.p(), hasUniquePFBlockMatch);
  uniquePFBlockMatchEfficiencyVsEta_->Fill(track.eta(), hasUniquePFBlockMatch);
  uniquePFBlockMatchEfficiencyVsGoodVertices_->Fill(numberOfGoodVertices, hasUniquePFBlockMatch);
  linkedECALEfficiencyVsP_->Fill(track.p(), hasLinkedECAL);
  linkedECALEfficiencyVsEta_->Fill(track.eta(), hasLinkedECAL);
  linkedECALEfficiencyVsGoodVertices_->Fill(numberOfGoodVertices, hasLinkedECAL);
  linkedHCALEfficiencyVsP_->Fill(track.p(), hasLinkedHCAL);
  linkedHCALEfficiencyVsEta_->Fill(track.eta(), hasLinkedHCAL);
  linkedHCALEfficiencyVsGoodVertices_->Fill(numberOfGoodVertices, hasLinkedHCAL);
}

void SPRLowPUDataAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription description;
  description.add<edm::InputTag>("tracks", edm::InputTag("generalTracks"));
  description.add<edm::InputTag>("vertices", edm::InputTag("offlinePrimaryVertices"));
  description.add<edm::InputTag>("pfBlocks", edm::InputTag("particleFlowBlock"));
  description.add<edm::InputTag>("muons", edm::InputTag("muons"));
  description.add<double>("minVertexNdof", 4.0);
  description.add<double>("maxVertexAbsZ", 24.0);
  description.add<double>("maxVertexRho", 2.0);
  description.add<std::string>("trackQuality", "highPurity");
  description.add<double>("minTrackPt", 1.0);
  description.add<double>("minTrackP", 3.0);
  description.add<double>("maxTrackAbsEta", 1.0);
  description.add<double>("maxTrackNormalizedChi2", 5.0);
  description.add<double>("maxTrackRelativePError", 0.1);
  description.add<unsigned int>("minTrackerLayers", 8);
  description.add<unsigned int>("minPixelLayers", 2);
  description.add<double>("maxTrackAbsDxy", 0.02);
  description.add<double>("maxTrackAbsDz", 0.05);
  description.add<double>("isolationCone", 0.3);
  description.add<double>("minIsolationTrackPt", 0.5);
  description.add<double>("maxIsolationTrackAbsEta", 2.5);
  description.add<double>("maxRelativeIsolation", 0.1);
  descriptions.add("sprLowPUDataAnalyzer", description);
}

DEFINE_FWK_MODULE(SPRLowPUDataAnalyzer);
