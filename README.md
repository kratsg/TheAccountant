# TheAccountant - A RootCore Package

Run-2 analysis for SUSY search in gluino to stops.

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

- [Installing](#installing)
  - [Grabbing the SUSYTools cross section files](#grabbing-the-susytools-cross-section-files)
  - [Dependencies](#dependencies)
- [Functionality Included](#functionality-included)
  - [Algorithms and Scripts](#algorithms-and-scripts)
    - [Audit.cxx](#auditcxx)
    - [OptimizationDump.cxx](#optimizationdumpcxx)
    - [Preselect.cxx](#preselectcxx)
    - [Report.cxx](#reportcxx)
    - [CookTheBooks.py](#cookthebookspy)
    - [GetWeights.py](#getweightspy)
  - [Samples and Drivers and Bash, oh my](#samples-and-drivers-and-bash-oh-my)
    - [Direct](#direct)
    - [Prun](#prun)
    - [Condor](#condor)
    - [Input DQ2 Samples](#input-dq2-samples)
- [Authors](#authors)
  - [Acknowledgements](#acknowledgements)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->


## Installing
The last stable analysis base used is **2.3.41**. To install,
```bash
mkdir myRootCore && cd $_
rcSetup Base,2.3.41
git clone https://github.com/kratsg/TheAccountant
git clone https://github.com/lawrenceleejr/Ext_RestFrames
cd Ext_RestFrames && git checkout v1.5 && cd -
git clone https://github.com/kratsg/xAODJetReclustering

git clone https://github.com/UCATLAS/xAODAnaHelpers
cd xAODAnaHelpers && git checkout 00-03-30 && cd ../
python xAODAnaHelpers/scripts/checkoutASGtags.py 2.3.41

rc find_packages
rc compile
```

### Grabbing the SUSYTools cross section files

Most of the time, if you want to run the `GetWeights.py`, it helps to be able to checkout the latest information from SUSYTools. This is how I do it:

```bash
cd TheAccountant/data/metadata
svn export --force svn+ssh://gstark@svn.cern.ch/reps/atlasoff/PhysicsAnalysis/SUSYPhys/SUSYTools/trunk/data/susy_crosssections_13TeV.txt
svn export --force svn+ssh://gstark@svn.cern.ch/reps/atlasoff/PhysicsAnalysis/SUSYPhys/SUSYTools/trunk/data/mc15_13TeV/
cd -
```

### Dependencies
 - dependencies are in [cmt/Makefile.RootCore](cmt/Makefile.RootCore)
 - uses [Chris Rogan](https://github.com/crogan)'s bad-ass [RestFrames](https://github.com/crogan/RestFrames) package
 - uses [Miles Wu](https://github.com/mileswu)'s awesome [JetSubStructureTools](https://github.com/mileswu/JetSubstructureTools) package

## Functionality Included

<img src="https://github.com/kratsg/TheAccountant/raw/master/img/decayTree.png?raw=true" alt="Decay Tree" width="225" />
<img src="https://github.com/kratsg/TheAccountant/raw/master/img/visTree.png?raw=true" alt="Visible Tree" width="225" />
<img src="https://github.com/kratsg/TheAccountant/raw/master/img/invTree.png?raw=true" alt="Invisible Tree" width="225" />

### Algorithms and Scripts

#### Audit.cxx

This algorithm sets up a decay tree with a pair of vis-invis particles in each hemisphere.

#### OptimizationDump.cxx

This algorithm produces an ntuple used for [Optimization](https://github.com/kratsg/Optimization).

#### Preselect.cxx

This algorithm applies a pre-selection on objects in the event. If the event does not pass a preselection, the event is skipped via `wk()->skipEvent()`.

#### Report.cxx

This algorithm creates plots of large-R jets, bjets, and MET. It has functionality to create a separate set of plots for those events that pass the pre-selection as well as all events.

#### CookTheBooks.py

This is the macro that runs it all. [Start here and the world shall open up before you.](https://www.youtube.com/watch?v=5qH1pBTqvc4) I use a standard run

```
CookTheBooks.py --files input.root\
  -v -f --jet_minNum=4 --jet_minPt=30\
  --bjet_minNum=2 --numLeadingJets=4\
  --optimizationDump --mode=branch --jetLargeR_minPtView=300\
  direct
```

which will

- read the file(s) using branch mode
- apply the standard preselections
- dump an optimization ntuple
- plot the first 4 leading jets in the event
- only plot large-R jets with Pt > 300 GeV

See the help options `CookTheBooks.py -h` for more information about what you can actually do. Each of the specific sections in `CookTheBooks.py` can have a lot of options, so you might want to try something like `CookTheBooks.py -h audit` or `CookTheBooks.py -h preselect` to see only options for that specific algorithm or section.

#### GetWeights.py

This is the macro that will generate an appropriate `weights.json` for the given samples that you pass in. I use a standard run like

```bash
GetWeights.py mc15_13TeV:mc15_13TeV.foo.bar.baz/ "mc15_13TeV:mc15_13TeV.someOtherSample*" --inputGrid
```

which will use all information possible to make the most appropriate `weights.json` file we have.

### Samples and Drivers and Bash, oh my

So one of the bigger questions is - how do I run this? There's a few ways depending on what you want to do. There are various drivers supported which you can find out via

```
CookTheBooks.py -h drivers
```

to look up the drivers usable. As we use the `argparse` module with subcommands. One of the ways I've set this up consistently is to specify the driver you use at the end of the command. In this context, this means you must specify your files, the configuration for the algorithms; and then you can specify your driver and your driver's configurations. So you will end up writing a line that looks like

```
CookTheBooks.py --files input.root /path/to/sample/DAOD.pool.root --jets=AntiKt4LCTopoJets\
    condor --condorConfig1=foo --condorConfig2=bar
```

which is not often too bad. This forces you to realize that the driver is mainly independent of the actual execution of the script.

#### Direct

```bash
CookTheBooks.py direct -h
```

The direct driver is most often best for quick testing, but also running things locally. In this particular case, you can run on a single file, a bunch of files in different locations, or lots of different samples. Below are code examples that can help:

- Running on a single file
  ```bash
  CookTheBooks.py --files input.root direct
  ```

- Running on different files
  ```
  CookTheBooks.py --files input1.root /path/to/input2.root "other_inputs_*.root" direct
  ```

- Running on different samples
  ```
  CookTheBooks.py --files "/path/to/sampleA/*.root" /path/to/sampleB/test.root direct
  ```

Note that in the case where multiple root files exist in a given sample (using a `*`; such as in `sampleA` example above), you will want to surround the path name with quotes to prevent your shell from auto-expanding (makes it harder for me to determine that multiple files belong to a sample).

#### Prun

```bash
CookTheBooks.py prun -h
```

The prun driver is most often best if you need to run on a lot of files and you would rather not copy them and run locally... running on the grid. The code will check to make sure you have `prun` set up correctly via `localSetupPandaClient` (it will notify you if you don't, so don't worry). There are a lot of options available for the `PrunDriver`. Some are set by default based on experience of others and you should rarely need to manage it. The only required option for submitting jobs is to add a "tag" to the end as the form `_TA%s`.

In this driver, you can only run on DQ2 sample names, so this can be done by providing input file lists to read in, or just passing in the sample name instead. Here is an example of a job that was ran so you can get an idea:

```bash
CookTheBooks.py --files "mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r6630_r6264" prun --optGridOutputSampleName=TEST2
```

which is on bigpanda [here](http://bigpanda.cern.ch/task/6078737/).

#### Condor

```bash
CookTheBooks.py condor -h
```

The condor driver is the best if you're using UCTier3 + Condor here to flock everything. First, if you want the cache'ing server we have set up, you just need to export the `STORAGEPREFIX` on the submission node (where you run `CookTheBooks.py`)

```bash
export STORAGEPREFIX=root://uct3-xrd.mwt2.org/
```

and then after compiling - run `rc make_par` to make sure everything is up-to-date and package the files for transferring to the worker nodes. And then you can just use the condor driver like normal. An example of this script is in [scripts/run.sh](scripts/run.sh) which takes advantage of splitting up each submission into multiple executions for submission.

#### Input DQ2 Samples

One can specify that the input files are actually samples located on the grid or in one of the storage sites that are accessible via xrd. The best part is that you can specify sample name patterns rather than direct names -- each one will be added for you automatically. These can be run on any driver you choose, so long as you specify the `--inputGrid` option next to your `--files` option. As an example:

```bash
CookTheBooks.py --inputGrid --files "user.amarzin.*.ttbar.*_tag_05_output_xAOD.root/" ... {driver}
```

This will automatically grab the gLFNs for the files in each of the samples you specify and read them in over `xrootd` automatically.

## Authors
- [Giordon Stark](https://github.com/kratsg)

### Acknowledgements
- [David Miller](https://github.com/fizisist)
- [Max Swiatlowski](https://github.com/mswiatlo)
- [Natalie Harrison](https://github.com/nharrison314)
