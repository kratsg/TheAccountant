#!/bin/bash

#0L
CookTheBooks.py --files "user.lvalery.*.Gtt.*_tag_05_output_xAOD.root/" "user.lvalery.*.data.*_tag_05_output_xAOD.root/" "user.mleblanc.*.dijet.*_tag_06_output_xAOD.root/" "user.lgagnon.*.topEW.*_tag_05_01_output_xAOD.root/" "user.lgagnon.*.Z_sherpa.*_tag_05_output_xAOD.root/" "user.lgagnon.*.diboson.*_tag_05_output_xAOD.root/" "user.lvalery.*.singletop.*_tag_05_01_output_xAOD.root/" "user.gstark.*.W_sherpa*.*_tag_05_output_xAOD.root/" --optimizationDump -v -f -yyyyyy --baselineLeptonSelection="==0" --jet_minNum=4 --jet_minPt=30 --bjet_minNum=2 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --inputDQ2 --badJetVeto --submitDir=TA06_MBJ05_0L condor > TA06_MBJ05_0L.log 2>&1 &

#1L
CookTheBooks.py --files "user.lvalery.*.Gtt.*_tag_05_output_xAOD.root/" "user.lvalery.*.data.*_tag_05_output_xAOD.root/" "user.mleblanc.*.dijet.*_tag_06_output_xAOD.root/" "user.lgagnon.*.topEW.*_tag_05_01_output_xAOD.root/" "user.lgagnon.*.Z_sherpa.*_tag_05_output_xAOD.root/" "user.lgagnon.*.diboson.*_tag_05_output_xAOD.root/" "user.lvalery.*.singletop.*_tag_05_01_output_xAOD.root/" "user.gstark.*.W_sherpa*.*_tag_05_output_xAOD.root/" --optimizationDump -v -f -yyyyyy --signalLeptonSelection=" >0" --jet_minNum=2 --jet_minPt=30 --bjet_minNum=1 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --inputDQ2 --badJetVeto --submitDir=TA06_MBJ05_1L condor > TA06_MBJ05_1L.log 2>&1 &

#0L ttbar inclusive
CookTheBooks.py --files="user.amarzin.410000.ttbar.DAOD_SUSY10.e3698_s2608_s2183_r6765_r6282_p2375_tag_06_v3_output_xAOD.root/" --optimizationDump -v -f -yyyyyy --baselineLeptonSelection="==0" --jet_minNum=4 --jet_minPt=30 --bjet_minNum=2 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --inputDQ2 --truthMETFilter=" <200" --badJetVeto --submitDir=TA06_incl_ttbar_0L condor > TA06_incl_ttbar_0L.log 2>&1 &

#1L ttbar inclusive
CookTheBooks.py --files="user.amarzin.410000.ttbar.DAOD_SUSY10.e3698_s2608_s2183_r6765_r6282_p2375_tag_06_v3_output_xAOD.root/" --optimizationDump -v -f -yyyyyy --signalLeptonSelection=" >0" --jet_minNum=2 --jet_minPt=30 --bjet_minNum=1 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --inputDQ2 --truthMETFilter=" <200" --badJetVeto --submitDir=TA06_incl_ttbar_1L condor > TA06_incl_ttbar_1L.log 2>&1 &

#0L ttbar exclusive
CookTheBooks.py --files="user.amarzin.407012.ttbar.DAOD_SUSY10.e4023_s2608_r6765_r6282_p2375_tag_06_v3_output_xAOD.root/" --optimizationDump -v -f -yyyyyy --baselineLeptonSelection="==0" --jet_minNum=4 --jet_minPt=30 --bjet_minNum=2 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --inputDQ2 --truthMETFilter=" >200" --badJetVeto --submitDir=TA06_excl_ttbar_0L condor > TA06_excl_ttbar_0L.log 2>&1 &

#1L ttbar exclusive
CookTheBooks.py --files="user.amarzin.407012.ttbar.DAOD_SUSY10.e4023_s2608_r6765_r6282_p2375_tag_06_v3_output_xAOD.root/" --optimizationDump -v -f -yyyyyy --signalLeptonSelection=" >0" --jet_minNum=2 --jet_minPt=30 --bjet_minNum=1 --numLeadingJets=4 --jetLargeR_minPtView=300 --minMET=50 --inputDQ2 --truthMETFilter=" >200" --badJetVeto --submitDir=TA06_excl_ttbar_1L condor > TA06_excl_ttbar_1L.log 2>&1 &
