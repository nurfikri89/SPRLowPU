# import FWCore.ParameterSet.Config as cms

# process = cms.Process("PFANA")

# process.load("FWCore.MessageService.MessageLogger_cfi")

# process.maxEvents = cms.untracked.PSet(
#    input = cms.untracked.int32(-1)
# )

# process.source = cms.Source("PoolSource",
#    fileNames = cms.untracked.vstring()
# )

# process.TFileService = cms.Service("TFileService",
#    fileName = cms.string("histo_SinglePion_NoPU.root")
# )

# process.recoAnalyzer = cms.EDAnalyzer(
#    "RECOAnalyzer",
#    pfBlock = cms.InputTag("particleFlowBlock"),
#    primaryVertices = cms.InputTag("offlinePrimaryVertices"),
# )

# process.p = cms.Path(process.recoAnalyzer)













































import FWCore.ParameterSet.Config as cms

process = cms.Process("PFANA")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(
   input = cms.untracked.int32(-1)
)

process.source = cms.Source(
   "PoolSource",
   fileNames = cms.untracked.vstring()
)

process.TFileService = cms.Service(
   "TFileService",
   fileName = cms.string("histo_SinglePion_NoPU.root")
)

process.recoAnalyzer = cms.EDAnalyzer(
   "RECOAnalyzer",
   pfBlock = cms.InputTag("particleFlowBlock"),
   primaryVertices = cms.InputTag("offlinePrimaryVertices"),
   genParticles = cms.InputTag("genParticles"),
)

process.p = cms.Path(process.recoAnalyzer)