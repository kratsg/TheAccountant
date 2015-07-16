#!/usr/bin/env python

# @file:    CookTheBooks.py
# @purpose: run the analysis
# @author:  Giordon Stark <gstark@cern.ch>
# @date:    April 2015
#
# @example:
# @code
# CookTheBooks.py --help
# @endcode
#

from __future__ import print_function
import logging

root_logger = logging.getLogger()
root_logger.addHandler(logging.StreamHandler())
cookBooks_logger = logging.getLogger("cookBooks")

import argparse
import os
import subprocess
import sys
import datetime
import time

import getpass  # input without echo

# catch CTRL+C
import signal
def signal_handler(signal, frame):
  print("Exiting the program now. Have a nice day!     ")  # extra spaces just in case
  sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)


SCRIPT_START_TIME = datetime.datetime.now()

# think about using argcomplete
# https://argcomplete.readthedocs.org/en/latest/#activating-global-completion%20argcomplete

if __name__ == "__main__":
  # if we want multiple custom formatters, use inheriting
  class CustomFormatter(argparse.ArgumentDefaultsHelpFormatter):
    pass

  __version__ = subprocess.check_output(["git", "describe", "--always"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()
  __short_hash__ = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()

  parser = argparse.ArgumentParser(description='Become an accountant and cook the books!',
                                   usage='%(prog)s filename [filename] [options]',
                                   formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30))

  # http://stackoverflow.com/a/16981688
  parser._optionals.title = "job runner options"

  # positional argument, require the first argument to be the input filename
  parser.add_argument('input_filename', metavar='', type=str, nargs='+', help='input file(s) to read')
  parser.add_argument('--submitDir', dest='submit_dir', metavar='<directory>', type=str, required=False, help='Output directory to store the output.', default='submitDir')
  parser.add_argument('--nevents', dest='num_events', metavar='<n>', type=int, help='Number of events to process for all datasets.', default=0)
  parser.add_argument('--skip', dest='skip_events', metavar='<n>', type=int, help='Number of events to skip at start.', default=0)
  parser.add_argument('-f', '--force', dest='force_overwrite', action='store_true', help='Overwrite previous directory if it exists.')
  parser.add_argument('--version', action='version', version='%(prog)s {version}'.format(version=__version__))
  parser.add_argument('--mode', dest='access_mode', type=str, metavar='{class, branch}', choices=['class', 'branch'], default='class', help='Run using branch access mode or class access mode. See TheAccountant/wiki/Access-Mode for more information')

  # http://stackoverflow.com/questions/12303547/set-the-default-to-false-if-another-mutually-exclusive-argument-is-true
  group_driver = parser.add_mutually_exclusive_group()
  group_driver.add_argument('--direct', dest='driver', metavar='', action='store_const', const='direct', help='Run your jobs locally.')
  group_driver.add_argument('--prooflite', dest='driver', metavar='', action='store_const', const='prooflite', help='Run your jobs using ProofLite')
  group_driver.add_argument('--grid', dest='driver', metavar='', action='store_const', const='grid', help='Run your jobs on the grid.')
  group_driver.add_argument('--condor', dest='driver', metavar='', action='store_const', const='condor', help='Run your jobs on the condor.')
  group_driver.set_defaults(driver='direct')

  parser.add_argument('--inputList', dest='input_from_file', action='store_true', help='If enabled, will read in a text file containing a list of files.')
  parser.add_argument('--inputDQ2', dest='input_from_DQ2', action='store_true', help='If enabled, will search using DQ2. Can be combined with `--inputList`.')
  parser.add_argument('-v', '--verbose', dest='verbose', action='count', default=0, help='Enable verbose output of various levels. Default: no verbosity')
  parser.add_argument('-y', '--yes', dest='skip_confirm', action='store_true', help='Skip the configuration confirmation. Useful for when running in the background.')

  group_algorithms = parser.add_argument_group('global algorithm options')
  group_algorithms.add_argument('--debug', dest='debug', action='store_true', help='Enable verbose output of the algorithms.')
  group_algorithms.add_argument('--eventInfo', dest='eventInfo', metavar='', type=str, help='EventInfo container name.', default='EventInfo')
  group_algorithms.add_argument('--jetsLargeR', dest='inputLargeRJets', metavar='', type=str, help='Large-R jet container name.', default='FinalFatJets')
  group_algorithms.add_argument('--jets', dest='inputJets', metavar='', type=str, help='small-R jet container name.', default='FinalSignalJets')
  group_algorithms.add_argument('--met', dest='inputMET', metavar='', type=str, help='Missing Et container name.', default='Mettst')
  group_algorithms.add_argument('--metid', dest='inputMETName', metavar='', type=str, help='Missing Et container element to access.', default='Final')
  group_algorithms.add_argument('--electrons', dest='inputElectrons', metavar='', type=str, help='Electrons container name.', default='FinalSignalElectrons')
  group_algorithms.add_argument('--muons', dest='inputMuons', metavar='', type=str, help='Muons container name.', default='FinalSignalMuons')
  group_algorithms.add_argument('--taujets', dest='inputTauJets', metavar='', type=str, help='TauJets container name.', default='')
  group_algorithms.add_argument('--photons', dest='inputPhotons', metavar='', type=str, help='Photons container name.', default='')
  group_algorithms.add_argument('--decorJetTagsB', dest='decor_jetTags_b', metavar='', type=str, help='Decoration name for b-tags.', default='')
  group_algorithms.add_argument('--decorJetTagsTop', dest='decor_jetTags_top', metavar='', type=str, help='Decoration name for top-tags.', default='')
  group_algorithms.add_argument('--decorJetTagsW', dest='decor_jetTags_w', metavar='', type=str, help='Decoration name for w-tags.', default='')

  group_preselect = parser.add_argument_group('preselect options (all selections are inclusive: x >= min, x =< max)')
  group_preselect.add_argument('--jetLargeR_minNum',   type=int, metavar='', help='min num of large-R jets passing cuts',  default=0)
  group_preselect.add_argument('--jetLargeR_maxNum',   type=int, metavar='', help='max num of large-R jets passing cuts',  default = 100)
  group_preselect.add_argument('--jetLargeR_minPt',    type=float, metavar='', help='large-R jet min pt [GeV]',   default = 0.0)
  group_preselect.add_argument('--jetLargeR_maxPt',    type=float, metavar='', help='large-R jet max pt [GeV]',   default = 1.e6)
  group_preselect.add_argument('--jetLargeR_minMass',  type=float, metavar='', help='large-R jet min mass [GeV]', default = 0.0)
  group_preselect.add_argument('--jetLargeR_maxMass',  type=float, metavar='', help='large-R jet max mass [GeV]', default = 1.e6)
  group_preselect.add_argument('--jetLargeR_minEta',   type=float, metavar='', help='large-R jet min eta',  default = -10.0)
  group_preselect.add_argument('--jetLargeR_maxEta',   type=float, metavar='', help='large-R jet max eta',  default = 10.0)
  group_preselect.add_argument('--jetLargeR_minPhi',   type=float, metavar='', help='large-R jet min phi',  default = -4.0)
  group_preselect.add_argument('--jetLargeR_maxPhi',   type=float, metavar='', help='large-R jet max phi',  default = 4.0)

  group_preselect.add_argument('--jet_minNum',  type=int,   metavar='', help='min num of small-R jets passing cuts',  default = 0)
  group_preselect.add_argument('--jet_maxNum',  type=int,   metavar='', help='max num of small-R jets passing cuts',  default = 100)
  group_preselect.add_argument('--bjet_minNum', type=int,   metavar='', help='min num of small-R bjets passing cuts',  default = 0)
  group_preselect.add_argument('--bjet_maxNum', type=int,   metavar='', help='max num of small-R bjets passing cuts',  default = 100)
  group_preselect.add_argument('--jet_minPt',   type=float, metavar='', help='small-R jet min pt [GeV]',   default = 0.0)
  group_preselect.add_argument('--jet_maxPt',   type=float, metavar='', help='small-R jet max pt [GeV]',   default = 1.e6)
  group_preselect.add_argument('--jet_minMass', type=float, metavar='', help='small-R jet min mass [GeV]', default = 0.0)
  group_preselect.add_argument('--jet_maxMass', type=float, metavar='', help='small-R jet max mass [GeV]', default = 1.e6)
  group_preselect.add_argument('--jet_minEta',  type=float, metavar='', help='small-R jet min eta',  default = -10.0)
  group_preselect.add_argument('--jet_maxEta',  type=float, metavar='', help='small-R jet max eta',  default = 10.0)
  group_preselect.add_argument('--jet_minPhi',  type=float, metavar='', help='small-R jet min phi',  default = -4.0)
  group_preselect.add_argument('--jet_maxPhi',  type=float, metavar='', help='small-R jet max phi',  default = 4.0)
  group_preselect.add_argument('--bTag_wp',     type=str,   metavar='', help='small-R jet b-tag working point',  default = "Loose")
  group_preselect.add_argument('--dPhiMin',     type=float, metavar='', help='dPhiMin(jet, met) cut to apply.', default=0.4)
  group_preselect.add_argument('--minMET',      type=float, metavar='', help='Missing Et cut [GeV]', default=0.0)
  group_preselect.add_argument('--numLeptons',  type=int,   metavar='', help='Require exactly n leptons in the event.', default=0)
  group_preselect.add_argument('--triggerSelection', type=str, metavar='', help='Specify a pattern of triggers to select on.', default='')

  group_audit = parser.add_argument_group('audit options')
  group_audit.add_argument('--no-minMassJigsaw', dest='disable_minMassJigsaw', action='store_true', help='Disable the minMass Jigsaw')
  group_audit.add_argument('--no-contraBoostJigsaw', dest='disable_contraBoostJigsaw', action='store_true', help='Disable the contraBoost Jigsaw')
  group_audit.add_argument('--no-hemiJigsaw', dest='disable_hemiJigsaw', action='store_true', help='Disable the hemi Jigsaw')
  group_audit.add_argument('--drawDecayTreePlots', dest='drawDecayTreePlots', action='store_true', help='Enable to draw the decay tree plots and save the canvas in the output ROOT file. Please only enable this if running locally.')

  group_optimizationDump = parser.add_argument_group('optimization dump options')
  group_optimizationDump.add_argument('--optimizationDump', dest='optimization_dump', action='store_true', help='Enable to dump optimization ttree of values to cut against')

  group_report = parser.add_argument_group('report options')
  group_report.add_argument('--numLeadingJets', type=int, metavar='', help='Number of leading+subleading plots to make.', default=0)
  group_report.add_argument('--jet_minPtView', type=float, metavar='', help='Only plot jets that pass a minimum pt.', default=0.0)
  group_report.add_argument('--jetLargeR_minPtView', type=float, metavar='', help='Only plot large-R jets that pass a minimum pt.', default=0.0)

  # parse the arguments, throw errors if missing any
  args = parser.parse_args()

  # set verbosity for python printing
  if args.verbose < 5:
    cookBooks_logger.setLevel(25 - args.verbose*5)
  else:
    cookBooks_logger.setLevel(logging.NOTSET + 1)

  try:
    import timing
    import ROOT

    cookBooks_logger.info("loading packages")
    ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")

    if args.force_overwrite:
      cookBooks_logger.info("removing {0:s}".format(args.submit_dir))
      import shutil
      shutil.rmtree(args.submit_dir, True)

    #Set up the job for xAOD access:
    ROOT.xAOD.Init("CookTheBooks").ignore();

    # create a new sample handler to describe the data files we use
    cookBooks_logger.info("creating new sample handler")
    sh_all = ROOT.SH.SampleHandler()

    for fname in args.input_filename:
      if args.input_from_file:
        if args.input_from_DQ2:
          with open(fname, 'r') as f:
            for line in f:
                ROOT.SH.scanDQ2(sh_all, line)
        else:
          ROOT.SH.readFileList(sh_all, "sample", fname)
      else:
        if args.input_from_DQ2:
          ROOT.SH.scanDQ2(sh_all, fname)
        else:
          # need to parse and split it up
          fname_base = os.path.basename(fname)
          sample_dir = os.path.dirname(os.path.abspath(fname))
          mother_dir = os.path.dirname(sample_dir)
          sh_list = ROOT.SH.DiskListLocal(mother_dir)
          ROOT.SH.scanDir(sh_all, sh_list, fname_base, os.path.basename(sample_dir))

    # print out the samples we found
    cookBooks_logger.info("\t%d different dataset(s) found", len(sh_all))
    for dataset in sh_all:
      cookBooks_logger.info("\t\t%d files in %s", dataset.numFiles(), dataset.name())
    sh_all.printContent()

    if len(sh_all) == 0:
      cookBooks_logger.log(25, "No datasets found. Exiting.")
      sys.exit(0)

    # set the name of the tree in our files
    sh_all.setMetaString("nc_tree", "CollectionTree")

    # read susy meta data
    cookBooks_logger.info("reading all metadata in $ROOTCOREBIN/data/TheAccountant")
    ROOT.SH.readSusyMetaDir(sh_all,"$ROOTCOREBIN/data/TheAccountant")

    # this is the basic description of our job
    cookBooks_logger.info("creating new job")
    job = ROOT.EL.Job()
    job.sampleHandler(sh_all)

    if args.num_events > 0:
      cookBooks_logger.info("processing only %d events", args.num_events)
      job.options().setDouble(ROOT.EL.Job.optMaxEvents, args.num_events)

    if args.skip_events > 0:
      cookBooks_logger.info("skipping first %d events", args.skip_events)
      job.options().setDouble(ROOT.EL.Job.optSkipEvents, args.skip_events)

    job.options().setDouble(ROOT.EL.Job.optCacheSize, 50*1024*1024)
    job.options().setDouble(ROOT.EL.Job.optCacheLearnEntries, 50)

    # access mode branch
    if args.access_mode == 'branch':
      job.options().setString( ROOT.EL.Job.optXaodAccessMode, ROOT.EL.Job.optXaodAccessMode_branch )
    else:
      job.options().setString( ROOT.EL.Job.optXaodAccessMode, ROOT.EL.Job.optXaodAccessMode_class )

    algorithmConfiguration_string = []
    sleepTime = 50./1000.
    printStr = "\tsetting {0: >20}.m_{1:<30} = {2}"

    # add our algorithm to the job
    cookBooks_logger.info("creating algorithms")

    preselect = ROOT.Preselect()
    cookBooks_logger.info("\tcreating preselect algorithm")
    algorithmConfiguration_string.append("preselect algorithm options")
    for opt in map(lambda x: x.dest, group_preselect._group_actions):
      cookBooks_logger.info("\t%s", printStr.format('Preselect', opt, getattr(args, opt)))
      algorithmConfiguration_string.append(printStr.format('Preselect', opt, getattr(args, opt)))
      setattr(preselect, 'm_{0}'.format(opt), getattr(args, opt))
      time.sleep(sleepTime)

    audit = ROOT.Audit()
    cookBooks_logger.info("\tcreating audit algorithm")
    algorithmConfiguration_string.append("audit algorithm options")
    for opt in ['minMassJigsaw', 'contraBoostJigsaw', 'hemiJigsaw']:
      cookBooks_logger.info("\t%s", printStr.format('Audit', opt, not(getattr(args, 'disable_{0}'.format(opt)))))
      algorithmConfiguration_string.append(printStr.format('Audit', opt, not(getattr(args, 'disable_{0}'.format(opt)))))
      setattr(audit, 'm_{0}'.format(opt), not(getattr(args, 'disable_{0}'.format(opt))))
      time.sleep(sleepTime)
    for opt in ['drawDecayTreePlots']:
      cookBooks_logger.info("\t%s", printStr.format('Audit', opt, getattr(args, opt)))
      algorithmConfiguration_string.append(printStr.format('Audit', opt, getattr(args, opt)))
      setattr(audit, 'm_{0}'.format(opt), getattr(args, opt))
      time.sleep(sleepTime)

    optimization_dump = None
    if args.optimization_dump:
      optimization_dump = ROOT.OptimizationDump()
      cookBooks_logger.info("\tcreating optimization dump algorithm")
      algorithmConfiguration_string.append("optimization dump algorithm")
      # no other options for now...
      time.sleep(sleepTime)

    report = ROOT.Report()
    cookBooks_logger.info("\tcreating report algorithm")
    algorithmConfiguration_string.append("report algorithm options")
    for opt in map(lambda x: x.dest, group_report._group_actions):
      cookBooks_logger.info("\t%s", printStr.format('Report', opt, getattr(args, opt)))
      algorithmConfiguration_string.append(printStr.format('Report', opt, getattr(args, opt)))
      setattr(report, 'm_{0}'.format(opt), getattr(args, opt))
      time.sleep(sleepTime)

    cookBooks_logger.info("\tsetting global algorithm variables")
    algorithmConfiguration_string.append("global algorithm options")
    for alg in [preselect, audit, optimization_dump, report]:
      if alg is None: continue  # skip optimization_dump if not defined
      for opt in map(lambda x: x.dest, group_algorithms._group_actions):
        cookBooks_logger.info("\t%s", printStr.format(alg.ClassName(), opt, getattr(args, opt)))
        algorithmConfiguration_string.append(printStr.format(alg.ClassName(), opt, getattr(args, opt)))
        setattr(alg, 'm_{0}'.format(opt), getattr(args, opt))
        time.sleep(sleepTime)

    if not args.skip_confirm:
      print("Press enter to continue or CTRL+C to escape", end='\r')
      sys.stdout.flush()
      getpass.getpass(prompt='')

    cookBooks_logger.info("adding algorithms")
    job.algsAdd(preselect)
    job.algsAdd(audit)
    if args.optimization_dump:
      job.algsAdd(optimization_dump)
    job.algsAdd(report)

    # make the driver we want to use:
    # this one works by running the algorithm directly
    cookBooks_logger.info("creating driver")
    driver = None
    if (args.driver == "direct"):
      cookBooks_logger.info("\trunning on direct")
      driver = ROOT.EL.DirectDriver()
      cookBooks_logger.info("\tsubmit job")
      driver.submit(job, args.submit_dir)
    elif (args.driver == "prooflite"):
      cookBooks_logger.info("\trunning on prooflite")
      driver = ROOT.EL.ProofDriver()
      cookBooks_logger.info("\tsubmit job")
      driver.submit(job, args.submit_dir)
    elif (args.driver == "grid"):
      cookBooks_logger.info("\trunning on Grid")
      driver = ROOT.EL.PrunDriver()
      driver.options().setString("nc_outputSampleName", "user.gstark.%%in:name[2]%%.%%in:name[3]%%")
      #driver.options().setDouble("nc_disableAutoRetry", 1)
      driver.options().setDouble("nc_nFilesPerJob", 1)
      driver.options().setDouble(ROOT.EL.Job.optGridMergeOutput, 1);
      cookBooks_logger.info("\tsubmit job")
      driver.submitOnly(job, args.submit_dir)
    elif (args.driver == "condor"):
      driver = ROOT.EL.CondorDriver()
      driver.submit(job, args.submit_dir)

    SCRIPT_END_TIME = datetime.datetime.now()

    with open(os.path.join(args.submit_dir, 'CookTheBooks.log'), 'w+') as f:
      f.write(' '.join(['[{0}]'.format(__version__), os.path.basename(sys.argv[0])] + sys.argv[1:]))
      f.write('\n')
      f.write('Code:  https://github.com/kratsg/TheAccountant/tree/{0}\n'.format(__short_hash__))
      f.write('Start: {0}\nStop:  {1}\nDelta: {2}\n\n'.format(SCRIPT_START_TIME.strftime("%b %d %Y %H:%M:%S"), SCRIPT_END_TIME.strftime("%b %d %Y %H:%M:%S"), SCRIPT_END_TIME - SCRIPT_START_TIME))
      f.write('job runner options\n')
      for opt in ['input_filename', 'submit_dir', 'num_events', 'skip_events', 'force_overwrite', 'input_from_file', 'input_from_DQ2', 'verbose', 'driver']:
        f.write('\t{0: <51} = {1}\n'.format(opt, getattr(args, opt)))
      for algConfig_str in algorithmConfiguration_string:
        f.write('{0}\n'.format(algConfig_str))

  except Exception, e:
    # we crashed
    cookBooks_logger.exception("{0}\nAn exception was caught!".format("-"*20))
