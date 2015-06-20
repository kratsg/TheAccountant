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
cd xAODAnaHelpers && git checkout 6cd6f9ed && cd ../

git clone https://github.com/mileswu/JetSubstructureTools
mv JetSubstructureTools ../. && ln -s ../JetSubstructureTools/JetSubStructureUtils JetSubStructureUtils

rc find_packages
rc compile
```

### Dependencies
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

This algorithm applies a pre-selection to large-R jets, bjets. If the event does not pass a preselection, it is skipped via `wk()->skipEvent()`.

#### [Report.cxx](TheAccountant/Report.h)

This algorithm creates plots of large-R jets, bjets, and MET. It has functionality to create a separate set of plots for those events that pass the pre-selection as well as all events.

#### [CookTheBooks.py](scripts/CookTheBooks.py)

This is the macro that runs it all. [Start here and the world shall open up before you.](https://www.youtube.com/watch?v=5qH1pBTqvc4) I use a standard run

```
CookTheBooks.py input.root -v -f --jet_minNum=4 --jet_minPt=30 --bjet_minNum=2 --numLeadingJets=4 --optimizationDump --mode=branch --jetLargeR_minPtView=300
```

which will

- read the file(s) using branch mode
- apply the standard preselections
- dump an optimization ntuple
- plot the first 4 leading jets in the event
- only plot large-R jets with Pt > 300 GeV

See the help options `CookTheBooks.py -h` for more information about what you can actually do.

## Authors
- [Giordon Stark](https://github.com/kratsg)

### Acknowledgements
- [David Miller](https://github.com/fizisist)
- [Max Swiatlowski](https://github.com/mswiatlo)
- [Lawrence Lee](https://github.com/lawrenceleejr)
- [Chris Rogan](https://github.com/crogan)

