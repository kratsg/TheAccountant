#!/bin/bash

gttFiles="user.lgagnon:user.lgagnon.*.Gtt.DAOD_SUSY10.*_tag_10_v1_output_xAOD.root/"
ttbarIncFiles="user.lvalery:user.lvalery.410000.ttbar.DAOD_SUSY10.*_tag_10_v1_output_xAOD.root/"
ttbarExcFiles="user.lvalery:user.lvalery.407012.ttbar.DAOD_SUSY10.*_tag_10_v1_output_xAOD.root/"

globalOpts=(-v -f -yyyyyy --optimizationDump --inputDQ2 --badJetVeto)

run0L_a=(--baselineLeptonSelection="==0" --jet_minNum=4 --jet_minPt=30 --bjet_minNum=2 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --dPhiMin=0.4 --jet_maxEta=2.8 --jet_minEta=-2.8)
run0L_b=(--signalLeptonSelection="==0" --jet_minNum=4 --jet_minPt=30 --bjet_minNum=2 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --dPhiMin=0.4 --jet_maxEta=2.8 --jet_minEta=-2.8)
run1L=(--signalLeptonSelection=" >0" --jet_minNum=2 --jet_minPt=30 --bjet_minNum=1 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --dPhiMin=0.0 --jet_maxEta=2.8 --jet_minEta=-2.8)
outputDir="TA07_MBJ10V3"

mkdir -p "${outputDir}"

#0L_a
CookTheBooks.py --files="${gttFiles}" "${globalOpts[@]}" "${run0L_a[@]}" --submitDir="${outputDir}/Gtt_0L_a" condor > "${outputDir}/Gtt_0L_a.log" 2>&1 &
CookTheBooks.py --files="${ttbarIncFiles}" "${globalOpts[@]}" "${run0L_a[@]}" --truthMETFilter=" <200" --submitDir="${outputDir}/ttbarInc_0L_a" condor > "${outputDir}/ttbarInc_0L_a.log" 2>&1 &
CookTheBooks.py --files="${ttbarExcFiles}" "${globalOpts[@]}" "${run0L_a[@]}" --truthMETFilter=" >200" --submitDir="${outputDir}/ttbarExc_0L_a" condor > "${outputDir}/ttbarExc_0L_a.log" 2>&1 &

CookTheBooks.py --files="${gttFiles}" "${globalOpts[@]}" "${run0L_b[@]}" --submitDir="${outputDir}/Gtt_0L_b" condor > "${outputDir}/Gtt_0L_b.log" 2>&1 &
CookTheBooks.py --files="${ttbarIncFiles}" "${globalOpts[@]}" "${run0L_b[@]}" --truthMETFilter=" <200" --submitDir="${outputDir}/ttbarInc_0L_b" condor > "${outputDir}/ttbarInc_0L_b.log" 2>&1 &
CookTheBooks.py --files="${ttbarExcFiles}" "${globalOpts[@]}" "${run0L_b[@]}" --truthMETFilter=" >200" --submitDir="${outputDir}/ttbarExc_0L_b" condor > "${outputDir}/ttbarExc_0L_b.log" 2>&1 &


#1L
CookTheBooks.py --files="${gttFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/Gtt_1L" condor > "${outputDir}/Gtt_1L.log" 2>&1 &
CookTheBooks.py --files="${ttbarIncFiles}" "${globalOpts[@]}" "${run1L[@]}" --truthMETFilter=" <200" --submitDir="${outputDir}/ttbarInc_1L" condor > "${outputDir}/ttbarInc_1L.log" 2>&1 &
CookTheBooks.py --files="${ttbarExcFiles}" "${globalOpts[@]}" "${run1L[@]}" --truthMETFilter=" >200" --submitDir="${outputDir}/ttbarExc_1L" condor > "${outputDir}/ttbarExc_1L.log" 2>&1 &

GetWeights.py "${gttFiles}" "${ttbarIncFiles}" "${ttbarExcFiles}" --inputDQ2 -o "${outputDir}/weights.json" > "${outputDir}/weights.log" 2>&1 &
