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

optional arguments:
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

audit options:
  --no-minMassJigsaw       Disable the minMass Jigsaw (default: False)
  --no-contraBoostJigsaw   Disable the contraBoost Jigsaw (default: False)
  --no-hemiJigsaw          Disable the hemi Jigsaw (default: True)
  --drawDecayTreePlots     Enable to draw the decay tree plots and save the
                           canvas in the output ROOT file. Please only enable
                           this if running locally. (default: False)
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
```

### Tested Against AnalysisBase versions:
 - 2.1.30

#### Authors
- [Giordon Stark](https://github.com/kratsg)

##### Acknowledgements
- [Lawrence Lee](https://github.com/lawrenceleejr)
- [David Miller](https://github.com/fizisist)
- [Chris Rogan](https://github.com/crogan)
