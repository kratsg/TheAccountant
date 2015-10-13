#!/bin/bash

dataFiles="user.mswiatlo.*.data.*_tag_13_xAOD_5_output_xAOD.root/"
gttFiles="user.mswiatlo.*.Gtt.*_tag_13_xAOD_5_output_xAOD.root/"
dijetFiles="user.mswiatlo.*.dijet.*_tag_13_xAOD_5_output_xAOD.root/"
topEWFiles="user.mswiatlo.*.topEW.*_tag_13_xAOD_5_output_xAOD.root/"
#dibosonFiles="user.mswiatlo.*.diboson.*_tag_13_xAOD_5_output_xAOD.root/"
singletopFiles="user.mswiatlo.*.singletop.*_tag_13_xAOD_5_output_xAOD.root/"
WsherpaFiles="user.mswiatlo.*.W_sherpa.*_tag_13_xAOD_5_output_xAOD.root/"
ZsherpaFiles="user.mswiatlo.*.Z_sherpa.*_tag_13_xAOD_5_output_xAOD.root/"
ttbarIncFiles="user.mswiatlo.410000.ttbar.*_tag_13_xAOD_5_output_xAOD.root/"
ttbarExcFiles="user.mswiatlo.407012.ttbar.*_tag_13_xAOD_5_output_xAOD.root/"

globalOpts=(-v -f -yyyyyy --optimizationDump --inputDQ2 --badJetVeto)

run0L=(--signalLeptonSelection="==0" --jet_minNum=4 --jet_minPt=30 --bjet_minNum=2 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=200 --dPhiMin=0.4 --jet_maxEta=2.8 --jet_minEta=-2.8)
run1L=(--signalLeptonSelection="==1" --jet_minNum=4 --jet_minPt=30 --bjet_minNum=2 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=200 --dPhiMin=0.0 --jet_maxEta=2.8 --jet_minEta=-2.8)
outputDir="TA01_MBJ13V5"

mkdir -p "${outputDir}"

#0L
CookTheBooks.py --files="${dataFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/data_0L" condor > "${outputDir}/data_0L.log" 2>&1 &
CookTheBooks.py --files="${gttFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/Gtt_0L" condor > "${outputDir}/Gtt_0L.log" 2>&1 &
CookTheBooks.py --files="${dijetFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/dijet_0L" condor > "${outputDir}/dijet_0L.log" 2>&1 &
CookTheBooks.py --files="${topEWFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/topEW_0L" condor > "${outputDir}/topEW_0L.log" 2>&1 &
#CookTheBooks.py --files="${dibosonFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/diboson_0L" condor > "${outputDir}/diboson_0L.log" 2>&1 &
CookTheBooks.py --files="${singletopFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/singletop_0L" condor > "${outputDir}/singletop_0L.log" 2>&1 &
CookTheBooks.py --files="${WsherpaFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/Wsherpa_0L" condor > "${outputDir}/Wsherpa_0L.log" 2>&1 &
CookTheBooks.py --files="${ZsherpaFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/Zsherpa_0L" condor > "${outputDir}/Zsherpa_0L.log" 2>&1 &
CookTheBooks.py --files="${ttbarIncFiles}" "${globalOpts[@]}" "${run0L[@]}" --truthMETFilter=" <200" --submitDir="${outputDir}/ttbarInc_0L" condor > "${outputDir}/ttbarInc_0L.log" 2>&1 &
CookTheBooks.py --files="${ttbarExcFiles}" "${globalOpts[@]}" "${run0L[@]}" --truthMETFilter=" >200" --submitDir="${outputDir}/ttbarExc_0L" condor > "${outputDir}/ttbarExc_0L.log" 2>&1 &

#1L
CookTheBooks.py --files="${dataFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/data_1L" condor > "${outputDir}/data_1L.log" 2>&1 &
CookTheBooks.py --files="${gttFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/Gtt_1L" condor > "${outputDir}/Gtt_1L.log" 2>&1 &
CookTheBooks.py --files="${dijetFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/dijet_1L" condor > "${outputDir}/dijet_1L.log" 2>&1 &
CookTheBooks.py --files="${topEWFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/topEW_1L" condor > "${outputDir}/topEW_1L.log" 2>&1 &
#CookTheBooks.py --files="${dibosonFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/diboson_1L" condor > "${outputDir}/diboson_1L.log" 2>&1 &
CookTheBooks.py --files="${singletopFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/singletop_1L" condor > "${outputDir}/singletop_1L.log" 2>&1 &
CookTheBooks.py --files="${WsherpaFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/Wsherpa_1L" condor > "${outputDir}/Wsherpa_1L.log" 2>&1 &
CookTheBooks.py --files="${ZsherpaFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/Zsherpa_1L" condor > "${outputDir}/Zsherpa_1L.log" 2>&1 &
CookTheBooks.py --files="${ttbarIncFiles}" "${globalOpts[@]}" "${run1L[@]}" --truthMETFilter=" <200" --submitDir="${outputDir}/ttbarInc_1L" condor > "${outputDir}/ttbarInc_1L.log" 2>&1 &
CookTheBooks.py --files="${ttbarExcFiles}" "${globalOpts[@]}" "${run1L[@]}" --truthMETFilter=" >200" --submitDir="${outputDir}/ttbarExc_1L" condor > "${outputDir}/ttbarExc_1L.log" 2>&1 &

#GetWeights.py "${gttFiles}" "${dijetFiles}" "${topEWFiles}" "${dibosonFiles}" "${singletopFiles}" "${WsherpaFiles}" "${ZsherpaFiles}" "${ttbarIncFiles}" "${ttbarExcFiles}" --inputDQ2 -o "${outputDir}/weights.json" > "${outputDir}/weights.log" 2>&1 &
