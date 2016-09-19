#!/bin/bash

dataFiles="${ROOTCOREBIN}/data/TheAccountant/data.list"
gttFiles="${ROOTCOREBIN}/data/TheAccountant/Gtt.list"
dijetFiles="${ROOTCOREBIN}/data/TheAccountant/dijet.list"
topEWFiles="${ROOTCOREBIN}/data/TheAccountant/topEW.list"
dibosonFiles="${ROOTCOREBIN}/data/TheAccountant/diboson.list"
singletopFiles="${ROOTCOREBIN}/data/TheAccountant/singletop.list"
WsherpaFiles="${ROOTCOREBIN}/data/TheAccountant/Wsherpa.list"
ZsherpaFiles="${ROOTCOREBIN}/data/TheAccountant/Zsherpa.list"
ttbarIncFiles="${ROOTCOREBIN}/data/TheAccountant/ttbarInc.list"
ttbarExcFiles="${ROOTCOREBIN}/data/TheAccountant/ttbarExc.list"

globalOpts=(-v -f -yyyyyy --optimizationDump --inputGrid --inputList --badJetVeto --rc_enable)

run0L=(--signalLeptonSelection="==0" --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=200 --dPhiMin=0.0)
run1L=(--signalLeptonSelection="==1" --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=200 --dPhiMin=0.0)
outputDir="2.4.18-0-0"

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

#GetWeights.py "${gttFiles}" "${dijetFiles}" "${topEWFiles}" "${dibosonFiles}" "${singletopFiles}" "${WsherpaFiles}" "${ZsherpaFiles}" "${ttbarIncFiles}" "${ttbarExcFiles}" --inputGrid -o "${outputDir}/weights.json" > "${outputDir}/weights.log" 2>&1 &
