#!/bin/bash

dataFiles="user.lvalery.*.data.*_tag_05_output_xAOD.root/"
gttFiles="user.lvalery.*.Gtt.*_tag_05_output_xAOD.root/"
dijetFiles="user.mleblanc.*.dijet.*_tag_06_output_xAOD.root/"
topEWFiles="user.lgagnon.*.topEW.*_tag_05_01_output_xAOD.root/"
dibosonFiles="user.lgagnon.*.diboson.*_tag_05_output_xAOD.root/"
singletopFiles="user.lvalery.*.singletop.*_tag_05_01_output_xAOD.root/"
WsherpaFiles="user.gstark.*.W_sherpa*.*_tag_05_output_xAOD.root/"
ZsherpaFiles="user.lgagnon.*.Z_sherpa.*_tag_05_output_xAOD.root/"
ttbarIncFiles="user.amarzin.410000.ttbar.DAOD_SUSY10.e3698_s2608_s2183_r6765_r6282_p2375_tag_06_v3_output_xAOD.root/"
ttbarExcFiles="user.amarzin.407012.ttbar.DAOD_SUSY10.e4023_s2608_r6765_r6282_p2375_tag_06_v3_output_xAOD.root/"

globalOpts=(-v -f -yyyyyy --optimizationDump --inputDQ2 --badJetVeto --electrons="FinalSignalElectrons" --muons="FinalSignalMuons" --jets="FinalSignalJets")

run0L=(--baselineLeptonSelection="==0" --jet_minNum=4 --jet_minPt=30 --bjet_minNum=2 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --dPhiMin=0.4)
run1L=(--signalLeptonSelection=" >0" --jet_minNum=2 --jet_minPt=30 --bjet_minNum=1 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --dPhiMin=0.4)
outputDir="TA06_MBJ05"

mkdir -p "${outputDir}"

#0L
CookTheBooks.py --files="${dataFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/data_0L" condor > "${outputDir}/data_0L.log" 2>&1 &
CookTheBooks.py --files="${gttFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/Gtt_0L" condor > "${outputDir}/Gtt_0L.log" 2>&1 &
CookTheBooks.py --files="${dijetFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/dijet_0L" condor > "${outputDir}/dijet_0L.log" 2>&1 &
CookTheBooks.py --files="${topEWFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/topEW_0L" condor > "${outputDir}/topEW_0L.log" 2>&1 &
CookTheBooks.py --files="${dibosonFiles}" "${globalOpts[@]}" "${run0L[@]}" --submitDir="${outputDir}/diboson_0L" condor > "${outputDir}/diboson_0L.log" 2>&1 &
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
CookTheBooks.py --files="${dibosonFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/diboson_1L" condor > "${outputDir}/diboson_1L.log" 2>&1 &
CookTheBooks.py --files="${singletopFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/singletop_1L" condor > "${outputDir}/singletop_1L.log" 2>&1 &
CookTheBooks.py --files="${WsherpaFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/Wsherpa_1L" condor > "${outputDir}/Wsherpa_1L.log" 2>&1 &
CookTheBooks.py --files="${ZsherpaFiles}" "${globalOpts[@]}" "${run1L[@]}" --submitDir="${outputDir}/Zsherpa_1L" condor > "${outputDir}/Zsherpa_1L.log" 2>&1 &
CookTheBooks.py --files="${ttbarIncFiles}" "${globalOpts[@]}" "${run1L[@]}" --truthMETFilter=" <200" --submitDir="${outputDir}/ttbarInc_1L" condor > "${outputDir}/ttbarInc_1L.log" 2>&1 &
CookTheBooks.py --files="${ttbarExcFiles}" "${globalOpts[@]}" "${run1L[@]}" --truthMETFilter=" >200" --submitDir="${outputDir}/ttbarExc_1L" condor > "${outputDir}/ttbarExc_1L.log" 2>&1 &

GetWeights.py "${gttFiles}" "${dijetFiles}" "${topEWFiles}" "${dibosonFiles}" "${singletopFiles}" "${WsherpaFiles}" "${ZsherpaFiles}" "${ttbarIncFiles}" "${ttbarExcFiles}" -o "${outputDir}/weights.json" > "${outputDir}/weights.log" 2>&1 &
