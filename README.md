# TheAccountant - A RootCore Package

Run-2 analysis for SUSY search in gluino to stops.

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

  - [Installing](#installing)
  - [Dependencies](#dependencies)
  - [Functionality Included](#functionality-included)
      - [[Audit.cxx](TheAccountant/Audit.h)](#auditcxxtheaccountantaudith)
      - [[Preselect.cxx](TheAccountant/Preselect.h)](#preselectcxxtheaccountantpreselecth)
      - [[Report.cxx](TheAccountant/Report.h)](#reportcxxtheaccountantreporth)
      - [[CookTheBooks.py](scripts/CookTheBooks.py)](#cookthebookspyscriptscookthebookspy)
  - [Tested Against AnalysisBase versions:](#tested-against-analysisbase-versions)
- [Authors](#authors)
  - [Acknowledgements](#acknowledgements)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->


## Installing
The last stable analysis base used is **2.3.12**. To install,
```bash
mkdir myRootCore && cd $_
rcSetup Base,2.3.12
git clone https://github.com/kratsg/TheAccountant
git clone https://github.com/lawrenceleejr/Ext_RestFrames
git clone https://github.com/kratsg/xAODJetReclustering
git clone https://github.com/UCATLAS/xAODAnaHelpers
git clone https://github.com/kratsg/JetSubstructureTools
ln -s JetSubstructureTools/JetSubStructureUtils JetSubStructureUtils
rc find_packages
rc compile
```

## Dependencies
 - dependencies are in [cmt/Makefile.RootCore](cmt/Makefile.RootCore)
 - uses [Chris Rogan](https://github.com/crogan)'s bad-ass [RestFrames](https://github.com/crogan/RestFrames) package
 - uses [Miles Wu](https://github.com/mileswu)'s awesome [JetSubStructureTools](https://github.com/mileswu/JetSubstructureTools) package

## Functionality Included

<img src="https://github.com/kratsg/TheAccountant/raw/master/img/decayTree.png?raw=true" alt="Decay Tree" width="225" />
<img src="https://github.com/kratsg/TheAccountant/raw/master/img/visTree.png?raw=true" alt="Visible Tree" width="225" />
<img src="https://github.com/kratsg/TheAccountant/raw/master/img/invTree.png?raw=true" alt="Invisible Tree" width="225" />

#### [Audit.cxx](TheAccountant/Audit.h)

This algorithm sets up a decay tree with a pair of vis-invis particles in each hemisphere.

#### [Preselect.cxx](TheAccountant/Preselect.h)

This algorithm applies a pre-selection to large-R jets, bjets. Decorates `EventInfo` container with `SG::AuxElement::Decorator< int > pass_preSel("pass_preSel")`.

#### [Report.cxx](TheAccountant/Report.h)

This algorithm creates plots of large-R jets, bjets, and MET. It has functionality to create a separate set of plots for those events that pass the pre-selection as well as all events.

#### [CookTheBooks.py](scripts/CookTheBooks.py)
```
usage: CookTheBooks.py filename [filename] [options]

Become an accountant and cook the books!

positional arguments:
                           input file(s) to read

job runner options:
  -h, --help               show this help message and exit
  --submitDir <directory>  Output directory to store the output. (default:
                           submitDir)
  --nevents <n>            Number of events to process for all datasets.
                           (default: 0)
  --skip <n>               Number of events to skip at start. (default: 0)
  -f, --force              Overwrite previous directory if it exists.
                           (default: False)
  --version                show program's version number and exit
  --direct                 Run your jobs locally. (default: direct)
  --prooflite              Run your jobs using ProofLite (default: direct)
  --grid                   Run your jobs on the grid. (default: direct)
  --condor                 Run your jobs on the condor. (default: direct)
  --inputList              If enabled, will read in a text file containing a
                           list of files. (default: False)
  --inputDQ2               If enabled, will search using DQ2. Can be combined
                           with `--inputList`. (default: False)
  -v, --verbose            Enable verbose output of various levels. Default:
                           no verbosity (default: 0)

global algorithm options:
  --debug                  Enable verbose output of the algorithms. (default:
                           False)
  --passPreSel             For Audit: only run on events that pass pre-
                           selection. For Report: enable additional plots for
                           events that pass pre-selection. (default: False)
  --eventInfo              EventInfo container name. (default: EventInfo)
  --jetsLargeR             Large-R jet container name. (default:
                           AntiKt10LCTopoJets)
  --jets                   small-R jet container name. (default:
                           AntiKt4LCTopoJets)
  --met                    Missing Et container name. (default: MET_RefFinal)
  --electrons              Electrons container name. (default: )
  --muons                  Muons container name. (default: )
  --taujets                TauJets container name. (default: )
  --photons                Photons container name. (default: )
  --decorJetTagsB          Decoration name for b-tags. (default: isB)
  --decorJetTagsTop        Decoration name for top-tags. (default: isTop)
  --decorJetTagsW          Decoration name for w-tags. (default: isW)

preselect options (all selections are inclusive: x >= min, x =< max):
  --jetLargeR_minNum       min num of large-R jets passing cuts (default: 0)
  --jetLargeR_maxNum       max num of large-R jets passing cuts (default: 100)
  --jetLargeR_minPt        large-R jet min pt [GeV] (default: 0.0)
  --jetLargeR_maxPt        large-R jet max pt [GeV] (default: 1000000.0)
  --jetLargeR_minMass      large-R jet min mass [GeV] (default: 0.0)
  --jetLargeR_maxMass      large-R jet max mass [GeV] (default: 1000000.0)
  --jetLargeR_minEta       large-R jet min eta (default: -10.0)
  --jetLargeR_maxEta       large-R jet max eta (default: 10.0)
  --jetLargeR_minPhi       large-R jet min phi (default: -4.0)
  --jetLargeR_maxPhi       large-R jet max phi (default: 4.0)
  --jet_minNum             min num of small-R jets passing cuts (default: 0)
  --jet_maxNum             max num of small-R jets passing cuts (default: 100)
  --bjet_minNum            min num of small-R bjets passing cuts (default: 0)
  --bjet_maxNum            max num of small-R bjets passing cuts (default:
                           100)
  --jet_minPt              small-R jet min pt [GeV] (default: 0.0)
  --jet_maxPt              small-R jet max pt [GeV] (default: 1000000.0)
  --jet_minMass            small-R jet min mass [GeV] (default: 0.0)
  --jet_maxMass            small-R jet max mass [GeV] (default: 1000000.0)
  --jet_minEta             small-R jet min eta (default: -10.0)
  --jet_maxEta             small-R jet max eta (default: 10.0)
  --jet_minPhi             small-R jet min phi (default: -4.0)
  --jet_maxPhi             small-R jet max phi (default: 4.0)
  --jet_MV1                small-R jet min MV1 (default: 0.0)

audit options:
  --no-minMassJigsaw       Disable the minMass Jigsaw (default: False)
  --no-contraBoostJigsaw   Disable the contraBoost Jigsaw (default: False)
  --no-hemiJigsaw          Disable the hemi Jigsaw (default: False)
  --drawDecayTreePlots     Enable to draw the decay tree plots and save the
                           canvas in the output ROOT file. Please only enable
                           this if running locally. (default: False)

optimization dump options:
  --optimizationDump       Enable to dump optimization ttree of values to cut
                           against (default: False)

report options:
  --numLeadingJets         Number of leading+subleading plots to make.
                           (default: 0)
```

## Tested Against AnalysisBase versions:
 - 2.3.12

# Authors
- [Giordon Stark](https://github.com/kratsg)

## Acknowledgements
- [David Miller](https://github.com/fizisist)
- [Max Swiatlowski](https://github.com/mswiatlo)
- [Lawrence Lee](https://github.com/lawrenceleejr)
- [Chris Rogan](https://github.com/crogan)

