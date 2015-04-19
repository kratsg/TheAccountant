# TheAccountant - A RootCore Package

## Installing
The last stable analysis base used is **2.1.30**. To install,
```bash
mkdir myRootCore && cd $_
rcSetup Base,2.1.30
git clone https://github.com/kratsg/TheAccountant
git clone https://github.com/lawrenceleejr/Ext_RestFrames
rc checkout_pkg atlasinst/Institutes/UChicago/xAODAnaHelpers/tags/xAODAnaHelpers-00-00-04
source xAODAnaHelpers/scripts/ElectronEfficiencyCorrectionPatch_Base.2.1.29.sh
rc find_packages
rc compile_pkg Ext_RestFrames
rc compile_pkg TheAccountant
```

## Dependencies
 - dependencies are in [cmt/Makefile.RootCore](cmt/Makefile.RootCore)
 - uses Chris Rogan's bad-ass [RestFrames](https://github.com/crogan/RestFrames) package

### Functionality Included

<img src="https://github.com/kratsg/TheAccountant/raw/master/img/decayTree.png?raw=true" alt="Decay Tree" width="225" />
<img src="https://github.com/kratsg/TheAccountant/raw/master/img/visTree.png?raw=true" alt="Visible Tree" width="225" />
<img src="https://github.com/kratsg/TheAccountant/raw/master/img/invTree.png?raw=true" alt="Invisible Tree" width="225" />

#### [Audit.cxx](TheAccountant/Audit.h)

This algorithm sets up a decay tree with a pair of vis-invis particles in each hemisphere.

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
  --direct                 Run your jobs locally. (default: direct)
  --prooflite              Run your jobs using ProofLite (default: direct)
  --grid                   Run your jobs on the grid. (default: direct)
  --inputList              If enabled, will read in a text file containing a
                           list of files. (default: False)
  --inputDQ2               If enabled, will search using DQ2. Can be combined
                           with `--inputList`. (default: False)
  -v, --verbose            Enable verbose output of various levels. Default:
                           no verbosity (default: 0)

global algorithm options:
  --debug                  Enable verbose output of the algorithms. (default:
                           False)
  --eventInfo              EventInfo container name. (default: EventInfo)
  --jets                   Jet container name. (default: AntiKt10LCTopoJets)
  --bJets                  B-Jet container name. (default: )
  --met                    Missing Et container name. (default: MET_RefFinal)
  --electrons              Electrons container name. (default: )
  --muons                  Muons container name. (default: )
  --taujets                TauJets container name. (default: )
  --photons                Photons container name. (default: )

preselect options (all selections are exclusive: x > min, x < max):
  --jet_minNum             jet min num (default: 0)
  --jet_maxNum             jet max num (default: 100)
  --jet_minPt              jet min pt [GeV] (default: 0.0)
  --jet_maxPt              jet max pt [GeV] (default: 1000000.0)
  --jet_minMass            jet min mass [GeV] (default: 0.0)
  --jet_maxMass            jet max mass [GeV] (default: 1000000.0)
  --jet_minEta             jet min eta (default: -10.0)
  --jet_maxEta             jet max eta (default: 10.0)
  --jet_minPhi             jet min phi (default: -4.0)
  --jet_maxPhi             jet max phi (default: 4.0)
  --bjet_minNum            bjet min num (default: 0)
  --bjet_maxNum            bjet max num (default: 100)
  --bjet_minPt             bjet min pt [GeV] (default: 0.0)
  --bjet_maxPt             bjet max pt [GeV] (default: 1000000.0)
  --bjet_minMass           bjet min mass [GeV] (default: 0.0)
  --bjet_maxMass           bjet max mass [GeV] (default: 1000000.0)
  --bjet_minEta            bjet min eta (default: -10.0)
  --bjet_maxEta            bjet max eta (default: 10.0)
  --bjet_minPhi            bjet min phi (default: -4.0)
  --bjet_maxPhi            bjet max phi (default: 4.0)
  --bjet_MV1               bjet min MV1 (default: 0.0)

audit options:
  --passPreSel             Only run on events that pass the preselection.
                           (default: False)
  --no-minMassJigsaw       Disable the minMass Jigsaw (default: False)
  --no-contraBoostJigsaw   Disable the contraBoost Jigsaw (default: False)
  --no-hemiJigsaw          Disable the hemi Jigsaw (default: False)
  --drawDecayTreePlots     Enable to draw the decay tree plots and save the
                           canvas in the output ROOT file. Please only enable
                           this if running locally. (default: False)
```

### Tested Against AnalysisBase versions:
 - 2.1.30

#### Authors
- [Giordon Stark](https://github.com/kratsg)

##### Acknowledgements
- [Lawrence Lee](https://github.com/lawrenceleejr)
- [David Miller](https://github.com/fizisist)
- [Chris Rogan](https://github.com/crogan)
