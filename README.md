# TheAccountant - A RootCore Package

Run-2 analysis for SUSY search in gluino to stops.

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

- [Installing](#installing)
  - [Dependencies](#dependencies)
- [Functionality Included](#functionality-included)
  - [Algorithms and Scripts](#algorithms-and-scripts)
    - [[Audit.cxx](TheAccountant/Audit.h)](#auditcxxtheaccountantaudith)
    - [[Preselect.cxx](TheAccountant/Preselect.h)](#preselectcxxtheaccountantpreselecth)
    - [[Report.cxx](TheAccountant/Report.h)](#reportcxxtheaccountantreporth)
    - [[CookTheBooks.py](scripts/CookTheBooks.py)](#cookthebookspyscriptscookthebookspy)
  - [Samples and Drivers and Bash, oh my](#samples-and-drivers-and-bash-oh-my)
    - [Direct](#direct)
    - [Prun](#prun)
    - [Input DQ2 Samples](#input-dq2-samples)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->


## Installing
The last stable analysis base used is **2.3.19**. To install,
```bash
mkdir myRootCore && cd $_
rcSetup Base,2.3.19
git clone https://github.com/kratsg/TheAccountant
git clone https://github.com/lawrenceleejr/Ext_RestFrames
git clone https://github.com/kratsg/xAODJetReclustering

git clone https://github.com/UCATLAS/xAODAnaHelpers
cd xAODAnaHelpers && git checkout 00-03-12 && cd ../
python xAODAnaHelpers/scripts/checkoutASGtags.py 2.3.19

# if you want to use a non-shared condor
#   - read http://twiki.mwt2.org/bin/view/UCTier3/CondorUCT3

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

### Algorithms and Scripts

#### [Audit.cxx](TheAccountant/Audit.h)

This algorithm sets up a decay tree with a pair of vis-invis particles in each hemisphere.

#### [Preselect.cxx](TheAccountant/Preselect.h)

This algorithm applies a pre-selection to large-R jets, bjets. If the event does not pass a preselection, it is skipped via `wk()->skipEvent()`.

#### [Report.cxx](TheAccountant/Report.h)

This algorithm creates plots of large-R jets, bjets, and MET. It has functionality to create a separate set of plots for those events that pass the pre-selection as well as all events.

#### [CookTheBooks.py](scripts/CookTheBooks.py)

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

#### Input DQ2 Samples

One can specify that the input files are actually samples located on the grid or in one of the storage sites that are accessible via xrd. The best part is that you can specify sample name patterns rather than direct names -- each one will be added for you automatically. These can be run on any driver you choose, so long as you specify the `--inputDQ2` option next to your `--files` option. As an example:

```bash
CookTheBooks.py --inputDQ2 --files "user.amarzin.*.ttbar.*_tag_05_output_xAOD.root/" ... {driver}
```

This will automatically grab the gLFNs for the files in each of the samples you specify and read them in over `xrootd` automatically.

## Authors
- [Giordon Stark](https://github.com/kratsg)

### Acknowledgements
- [David Miller](https://github.com/fizisist)
- [Max Swiatlowski](https://github.com/mswiatlo)
- [Lawrence Lee](https://github.com/lawrenceleejr)
- [Chris Rogan](https://github.com/crogan)

