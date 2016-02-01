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

SCRIPT_START_TIME = datetime.datetime.now()

# think about using argcomplete
# https://argcomplete.readthedocs.org/en/latest/#activating-global-completion%20argcomplete

if __name__ == "__main__":

  import getpass  # input without echo
  # catch CTRL+C
  import signal
  def signal_handler(signal, frame):
    print("Exiting the program now. Have a nice day!{0:s}".format(" "*40))  # extra spaces just in case
    sys.exit(0)
  signal.signal(signal.SIGINT, signal_handler)

  def user_confirm(args, level=0):
    # skip confirmation if they increase it enough
    if args.skip_confirm > level: return True
    print("Press enter to continue or CTRL+C to escape. To skip this next time, add -{0:s}".format('y'*(level+1)), end='\r')
    sys.stdout.flush()
    getpass.getpass(prompt='')

  # if we want multiple custom formatters, use inheriting
  class CustomFormatter(argparse.ArgumentDefaultsHelpFormatter):
    pass

  class _HelpAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
      if not values:
        parser.print_help()
      else:
        available_groups = [group.title for group in parser._action_groups]
        if values in available_groups:
          action_group = parser._action_groups[available_groups.index(values)]
          formatter = parser._get_formatter()
          formatter.start_section(action_group.title)
          formatter.add_text(action_group.description)
          formatter.add_arguments(action_group._group_actions)
          formatter.end_section()
          parser._print_message(formatter.format_help())
        else:
          print("That is not a valid subsection. Chose from {{{0:s}}}".format(','.join(available_groups)))
      parser.exit()

  __version__ = subprocess.check_output(["git", "describe", "--always"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()
  __short_hash__ = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()

  parser = argparse.ArgumentParser(add_help=False, description='Become an accountant and cook the books!',
                                   usage='%(prog)s --files ... file [file ...] [options] {driver} [driver options]',
                                   formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30))

  # there are really no positional arguments in the automatic group
  parser_requiredNamed = parser.add_argument_group('required named arguments')

  # add custom help
  parser.add_argument('-h', '--help', metavar='subsection', nargs='?', action=_HelpAction, help='show this help message and exit. You can also pass in the name of a subsection.')

  # http://stackoverflow.com/a/16981688
  parser._positionals.title = "required"
  parser._optionals.title = "optional"

  # positional argument, require the first argument to be the input filename
  parser_requiredNamed.add_argument('--files', dest='input_filename', metavar='file', type=str, nargs='+', required=True, help='input file(s) to read')
  parser.add_argument('--submitDir', dest='submit_dir', metavar='<directory>', type=str, required=False, help='Output directory to store the output.', default='submitDir')
  parser.add_argument('--nevents', dest='num_events', metavar='<n>', type=int, help='Number of events to process for all datasets.', default=0)
  parser.add_argument('--skip', dest='skip_events', metavar='<n>', type=int, help='Number of events to skip at start.', default=0)
  parser.add_argument('-f', '--force', dest='force_overwrite', action='store_true', help='Overwrite previous directory if it exists.')
  parser.add_argument('--version', action='version', version='%(prog)s {version}'.format(version=__version__))
  parser.add_argument('--mode', dest='access_mode', type=str, metavar='{class, branch}', choices=['class', 'branch'], default='class', help='Run using branch access mode or class access mode. See TheAccountant/wiki/Access-Mode for more information')

  parser.add_argument('--inputList', dest='use_inputFileList', action='store_true', help='If enabled, will read in a text file containing a list of files.')
  parser.add_argument('--inputGrid', dest='use_addGrid', action='store_true', help='If enabled, will search using DQ2. Can be combined with `--inputList`.')
  parser.add_argument('-v', '--verbose', dest='verbose', action='count', default=0, help='Enable verbose output of various levels. Default: no verbosity')
  parser.add_argument('-y', '--yes', dest='skip_confirm', action='count', default=0, help='Skip the configuration confirmations. Useful for when running in the background.')

  group_algorithms = parser.add_argument_group('global', description='global algorithm options')
  group_algorithms.add_argument('--debug', dest='debug', action='store_true', help='Enable verbose output of the algorithms.')
  group_algorithms.add_argument('--eventInfo', dest='eventInfo', metavar='', type=str, help='EventInfo container name.', default='EventInfo')
  group_algorithms.add_argument('--jets', dest='inputJets', metavar='', type=str, help='small-R jet container name.', default='STCalibAntiKt4EMTopoJets')
  group_algorithms.add_argument('--electrons', dest='inputElectrons', metavar='', type=str, help='Electrons container name.', default='STCalibElectrons')
  group_algorithms.add_argument('--muons', dest='inputMuons', metavar='', type=str, help='Muons container name.', default='STCalibMuons')
  group_algorithms.add_argument('--met', dest='inputMET', metavar='', type=str, help='Missing Et container name.', default='Mettst')
  group_algorithms.add_argument('--metid', dest='inputMETName', metavar='', type=str, help='Missing Et container element to access.', default='Final')
  group_algorithms.add_argument('--jetsLargeR', dest='inputLargeRJets', metavar='', type=str, help='Large-R jet container name.', default='FinalFatJets')
  group_algorithms.add_argument('--taujets', dest='inputTauJets', metavar='', type=str, help='TauJets container name.', default='')
  group_algorithms.add_argument('--photons', dest='inputPhotons', metavar='', type=str, help='Photons container name.', default='')
  group_algorithms.add_argument('--decorJetTagsB', dest='decor_jetTags_b', metavar='', type=str, help='Decoration name for b-tags.', default='')
  group_algorithms.add_argument('--decorJetTagsTop', dest='decor_jetTags_top', metavar='', type=str, help='Decoration name for top-tags.', default='')
  group_algorithms.add_argument('--decorJetTagsW', dest='decor_jetTags_w', metavar='', type=str, help='Decoration name for w-tags.', default='')
  group_algorithms.add_argument('--rc_enable', action='store_true', help='Enable to use RC for large-R jets instead of standard large-R jets. Input will be the small-R jets that pass preselection. This ignores the jetsLargeR option for container name. See Preselect for configuration details.')

  group_preselect = parser.add_argument_group('preselect', description='all selections are inclusive: x >= min, x =< max')
  group_preselect.add_argument('--jet_minNum',  type=int,   metavar='', help='min num of small-R jets passing cuts',  default = 4)
  group_preselect.add_argument('--jet_maxNum',  type=int,   metavar='', help='max num of small-R jets passing cuts',  default = 100)
  group_preselect.add_argument('--jet_minPt',   type=float, metavar='', help='small-R jet min pt [GeV]',   default = 30.0)
  group_preselect.add_argument('--jet_maxPt',   type=float, metavar='', help='small-R jet max pt [GeV]',   default = 1.e6)
  group_preselect.add_argument('--jet_minEta',  type=float, metavar='', help='small-R jet min eta',  default = -2.8)
  group_preselect.add_argument('--jet_maxEta',  type=float, metavar='', help='small-R jet max eta',  default = 2.8)
  group_preselect.add_argument('--jet_minPhi',  type=float, metavar='', help='small-R jet min phi',  default = -4.0)
  group_preselect.add_argument('--jet_maxPhi',  type=float, metavar='', help='small-R jet max phi',  default = 4.0)

  group_preselect.add_argument('--bTag_wp',     type=str,   metavar='', help='small-R jet b-tag working point',  default = "Loose")
  group_preselect.add_argument('--bjet_minNum', type=int,   metavar='', help='min num of small-R bjets passing cuts',  default = 2)
  group_preselect.add_argument('--bjet_maxNum', type=int,   metavar='', help='max num of small-R bjets passing cuts',  default = 100)
  group_preselect.add_argument('--bjet_minPt',   type=float, metavar='', help='small-R bjet min pt [GeV]',   default = 30.0)
  group_preselect.add_argument('--bjet_maxPt',   type=float, metavar='', help='small-R bjet max pt [GeV]',   default = 1.e6)
  group_preselect.add_argument('--bjet_minEta',  type=float, metavar='', help='small-R bjet min eta',  default = -2.5)
  group_preselect.add_argument('--bjet_maxEta',  type=float, metavar='', help='small-R bjet max eta',  default = 2.5)
  group_preselect.add_argument('--bjet_minPhi',  type=float, metavar='', help='small-R bjet min phi',  default = -4.0)
  group_preselect.add_argument('--bjet_maxPhi',  type=float, metavar='', help='small-R bjet max phi',  default = 4.0)

  group_preselect.add_argument('--jetLargeR_minNum',   type=int, metavar='', help='min num of large-R jets passing cuts',  default=0)
  group_preselect.add_argument('--jetLargeR_maxNum',   type=int, metavar='', help='max num of large-R jets passing cuts',  default = 100)
  group_preselect.add_argument('--jetLargeR_minPt',    type=float, metavar='', help='large-R jet min pt [GeV]',   default = 100.0)
  group_preselect.add_argument('--jetLargeR_maxPt',    type=float, metavar='', help='large-R jet max pt [GeV]',   default = 1.e6)
  group_preselect.add_argument('--jetLargeR_minMass',  type=float, metavar='', help='large-R jet min mass [GeV]', default = 0.0)
  group_preselect.add_argument('--jetLargeR_maxMass',  type=float, metavar='', help='large-R jet max mass [GeV]', default = 1.e6)
  group_preselect.add_argument('--jetLargeR_minEta',   type=float, metavar='', help='large-R jet min eta',  default = -2.0)
  group_preselect.add_argument('--jetLargeR_maxEta',   type=float, metavar='', help='large-R jet max eta',  default = 2.0)
  group_preselect.add_argument('--jetLargeR_minPhi',   type=float, metavar='', help='large-R jet min phi',  default = -4.0)
  group_preselect.add_argument('--jetLargeR_maxPhi',   type=float, metavar='', help='large-R jet max phi',  default = 4.0)

  group_preselect.add_argument('--topTag_wp',     type=str,   metavar='', help='large-R jet top-tagging working point',  default = "VeryLoose")
  group_preselect.add_argument('--topTag_minNum',   type=int, metavar='', help='min num of top-tagged large-R jets',  default=0)
  group_preselect.add_argument('--topTag_maxNum',   type=int, metavar='', help='max num of top-tagged large-R jets',  default = 100)
  group_preselect.add_argument('--topTag_minPt',    type=float, metavar='', help='top-tagged large-R jet min pt [GeV]',   default = 300.0)
  group_preselect.add_argument('--topTag_maxPt',    type=float, metavar='', help='top-tagged large-R jet max pt [GeV]',   default = 1.e6)
  group_preselect.add_argument('--topTag_minMass',  type=float, metavar='', help='top-tagged large-R jet min mass [GeV]', default = 100.0)
  group_preselect.add_argument('--topTag_maxMass',  type=float, metavar='', help='top-tagged large-R jet max mass [GeV]', default = 1.e6)
  group_preselect.add_argument('--topTag_minEta',   type=float, metavar='', help='top-tagged large-R jet min eta',  default = -2.0)
  group_preselect.add_argument('--topTag_maxEta',   type=float, metavar='', help='top-tagged large-R jet max eta',  default = 2.0)
  group_preselect.add_argument('--topTag_minPhi',   type=float, metavar='', help='top-tagged large-R jet min phi',  default = -4.0)
  group_preselect.add_argument('--topTag_maxPhi',   type=float, metavar='', help='top-tagged large-R jet max phi',  default = 4.0)

  group_preselect.add_argument('--rc_radius', type=float, metavar='', help='Radius of reclustered jet', default = 1.0)
  group_preselect.add_argument('--rc_inputPt', type=float, metavar='', help='Filter input jet pT', default = 0.0)
  group_preselect.add_argument('--rc_trimPtFrac', type=float, metavar='', help='pT fraction trimming to apply to RC jet', default = 0.0)


  group_preselect.add_argument('--dPhiMin',     type=float, metavar='', help='dPhiMin(jet, met) cut to apply.', default=0.0)
  group_preselect.add_argument('--minMET',      type=float, metavar='', help='Missing Et cut [GeV]', default=0.0)
  group_preselect.add_argument('--baselineLeptonSelection',  type=str,   metavar='', help='Pass in string of the format XX#. XX can be {"==", "!=", ">=", "<=", " >", " <"} and # is a number. Note that for " >" and " <" - you need the preceding space.', default="")
  group_preselect.add_argument('--signalLeptonSelection',  type=str,   metavar='', help='Pass in string of the format XX#. XX can be {"==", "!=", ">=", "<=", " >", " <"} and # is a number. Note that for " >" and " <" - you need the preceding space.', default="")
  group_preselect.add_argument('--badJetVeto', action='store_true', help='Enable a veto on an event if there is at least one bad jet')
  group_preselect.add_argument('--truthMETFilter', type=str, metavar='', help='Pass in a string of the format XX#. XX can be {"==", "!=", ">=", "<=", " >", " <"} and # is a number [in GeV]. Note that for " >" and " <" - you need the preceding space.', default="")

  group_preselect.add_argument('--triggerSelection', type=str, metavar='', help='Specify a pattern of triggers to select on.', default='')

  group_audit = parser.add_argument_group('audit')
  group_audit.add_argument('--no-minMassJigsaw', dest='disable_minMassJigsaw', action='store_true', help='Disable the minMass Jigsaw')
  group_audit.add_argument('--no-contraBoostJigsaw', dest='disable_contraBoostJigsaw', action='store_true', help='Disable the contraBoost Jigsaw')
  group_audit.add_argument('--no-hemiJigsaw', dest='disable_hemiJigsaw', action='store_true', help='Disable the hemi Jigsaw')
  group_audit.add_argument('--drawDecayTreePlots', dest='drawDecayTreePlots', action='store_true', help='Enable to draw the decay tree plots and save the canvas in the output ROOT file. Please only enable this if running locally.')

  group_optimizationDump = parser.add_argument_group('optimization')
  group_optimizationDump.add_argument('--optimizationDump', dest='optimization_dump', action='store_true', help='Enable to dump optimization ttree of values to cut against')
  group_optimizationDump.add_argument('--rcTrimFrac', type=float, metavar='', help='fraction to trim from rc jets',default=0.05)

  group_report = parser.add_argument_group('report')
  group_report.add_argument('--numLeadingJets', type=int, metavar='', help='Number of leading+subleading plots to make.', default=0)
  group_report.add_argument('--jet_minPtView', type=float, metavar='', help='Only plot jets that pass a minimum pt.', default=0.0)
  group_report.add_argument('--jetLargeR_minPtView', type=float, metavar='', help='Only plot large-R jets that pass a minimum pt.', default=0.0)
  group_report.add_argument('--jet_maxAbsEtaView', type=float, metavar='', help='Only plot jets with abs(eta) < cut.', default=2.5)
  group_report.add_argument('--jetLargeR_maxAbsEtaView', type=float, metavar='', help='Only plot large-R jets with abs(eta) < cut.', default=1.6)

  driverUsageStr = 'CookTheBooks.py --files ... file [file ...] [options] {0:s} [{0:s} options]'
  # first is the driver
  drivers_parser = parser.add_subparsers(prog='CookTheBooks.py', title='drivers', dest='driver', description='specify where to run jobs')
  direct = drivers_parser.add_parser('direct', help='Run your jobs locally.', usage=driverUsageStr.format('direct'), formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30))
  prooflite = drivers_parser.add_parser('prooflite', help='Run your jobs using ProofLite', usage=driverUsageStr.format('prooflite'), formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30))
  prun = drivers_parser.add_parser('prun', help='Run your jobs on the grid using prun. Use prun --help for descriptions of the options.', usage=driverUsageStr.format('prun'), formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30))
  condor = drivers_parser.add_parser('condor', help='Flock your jobs to condor', usage=driverUsageStr.format('condor'), formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30))
  lsf = drivers_parser.add_parser('lsf', help='Flock your jobs to lsf', usage=driverUsageStr.format('lsf'), formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30))
  local = drivers_parser.add_parser('local', help='Run your jobs locally.', usage=driverUsageStr.format('local'), formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30))

 # standard options for other drivers
  #.add_argument('--optCacheLearnEntries', type=str, required=False, default=None)
  #.add_argument('--optCacheSize', type=str, required=False, default=None)
  #.add_argument('--optD3PDCacheMinByte', type=str, required=False, default=None)
  #.add_argument('--optD3PDCacheMinByteFraction', type=str, required=False, default=None)
  #.add_argument('--optD3PDCacheMinEvent', type=str, required=False, default=None)
  #.add_argument('--optD3PDCacheMinEventFraction', type=str, required=False, default=None)
  #.add_argument('--optD3PDPerfStats', type=str, required=False, default=None)
  #.add_argument('--optD3PDReadStats', type=str, required=False, default=None)
  #.add_argument('--optDisableMetrics', type=str, required=False, default=None)
  #.add_argument('--optEventsPerWorker', type=str, required=False, default=None)
  #.add_argument('--optFilesPerWorker', type=str, required=False, default=None)
  #.add_argument('--optMaxEvents', type=str, required=False, default=None)
  #.add_argument('--optPerfTree', type=str, required=False, default=None)
  #.add_argument('--optPrintPerFileStats', type=str, required=False, default=None)
  #.add_argument('--optRemoveSubmitDir', type=str, required=False, default=None)
  #.add_argument('--optResetShell', type=str, required=False, default=None)
  #.add_argument('--optSkipEvents', type=str, required=False, default=None)
  #.add_argument('--optSubmitFlags', type=str, required=False, default=None)
  #.add_argument('--optXAODPerfStats', type=str, required=False, default=None)
  #.add_argument('--optXAODReadStats', type=str, required=False, default=None)
  #.add_argument('--optXaodAccessMode', type=str, required=False, default=None)
  #.add_argument('--optXaodAccessMode_branch', type=str, required=False, default=None)
  #.add_argument('--optXaodAccessMode_class', type=str, required=False, default=None)

  # define arguments for prun driver
  prun.add_argument('--optGridCloud',            metavar='', type=str, required=False, default=None)
  prun.add_argument('--optGridDestSE',           metavar='', type=str, required=False, default="MWT2_UC_LOCALGROUPDISK")
  prun.add_argument('--optGridExcludedSite',     metavar='', type=str, required=False, default=None)
  prun.add_argument('--optGridExpress',          metavar='', type=str, required=False, default=None)
  prun.add_argument('--optGridMaxCpuCount',      metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridMaxNFilesPerJob',  metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridMaxFileSize',      metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridMemory',           metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridMergeOutput',      metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridNFiles',           metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridNFilesPerJob',     metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridNGBPerJob',        metavar='', type=int, required=False, default=2)
  prun.add_argument('--optGridNJobs',            metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridNoSubmit',         metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridSite',             metavar='', type=str, required=False, default=None)
  prun.add_argument('--optGridUseChirpServer',   metavar='', type=int, required=False, default=None)
  prun.add_argument('--optTmpDir',               metavar='', type=str, required=False, default=None)
  prun.add_argument('--optRootVer',              metavar='', type=str, required=False, default=None)
  prun.add_argument('--optCmtConfig',            metavar='', type=str, required=False, default=None)
  prun.add_argument('--optGridDisableAutoRetry', metavar='', type=int, required=False, default=None)
  prun.add_argument('--optGridOutputSampleName', metavar='', type=str, required=True, help='Define the value for _TAXX')

  # define arguments for condor driver
  condor.add_argument('--optCondorConf', metavar='', type=str, required=False, default='stream_output = true')

  # define arguments for lsf driver
  lsf.add_argument('--optLSFConf', metavar='', type=str, required=False, default='-q short')

  # parse the arguments, throw errors if missing any
  args = parser.parse_args()

  # set verbosity for python printing
  if args.verbose < 4:
    cookBooks_logger.setLevel(20 - args.verbose*5)
  else:
    cookBooks_logger.setLevel(logging.NOTSET + 1)

  try:
    import timing

    # check environment variables for various options first before trying to do anything else
    if args.driver == 'prun':
      required_environment_variables = ['PATHENA_GRID_SETUP_SH', 'PANDA_CONFIG_ROOT', 'ATLAS_LOCAL_PANDACLIENT_PATH', 'PANDA_SYS', 'ATLAS_LOCAL_PANDACLI_VERSION']
      for env_var in required_environment_variables:
        if os.getenv(env_var) is None:
          raise EnvironmentError('Panda client is not setup. Run localSetupPandaClient.')

    # check submission directory
    if args.force_overwrite:
      cookBooks_logger.info("removing {0:s}".format(args.submit_dir))
      import shutil
      shutil.rmtree(args.submit_dir, True)
    else:
      # check if directory exists
      if os.path.exists(args.submit_dir):
        raise OSError('Output directory {0:s} already exists. Either re-run with -f/--force, choose a different --submitDir, or rm -rf it yourself. Just deal with it, dang it.'.format(args.submit_dir))

    # they will need it to get it working
    use_addGrid = (args.use_addGrid)|(args.driver in ['prun', 'condor','lsf'])
    if use_addGrid:
      if os.getenv('XRDSYS') is None:
        raise EnvironmentError('xrootd client is not setup. Run localSetupFAX or equivalent.')

    # at this point, we should import ROOT and do stuff
    import ROOT
    cookBooks_logger.info("loading packages")
    ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")
    # load the standard algorithm since pyroot delays quickly
    ROOT.EL.Algorithm()

    #Set up the job for xAOD access:
    ROOT.xAOD.Init("CookTheBooks").ignore();

    # check that we have appropriate drivers
    if args.driver == 'prun':
      if getattr(ROOT.EL, 'PrunDriver') is None:
          raise KeyError('Cannot load the Prun driver from EventLoop. Did you not compile it?')
    elif args.driver == 'condor':
      if getattr(ROOT.EL, 'CondorDriver') is None:
        raise KeyError('Cannot load the Condor driver from EventLoop. Did you not compile it?')
    elif args.driver == 'lsf':
      if getattr(ROOT.EL, 'LSFDriver') is None:
        raise KeyError('Cannot load the LSF driver from EventLoop. Did you not compile it?')

    # create a new sample handler to describe the data files we use
    cookBooks_logger.info("creating new sample handler")
    sh_all = ROOT.SH.SampleHandler()

    # this portion is just to output for verbosity
    if args.use_inputFileList:
      cookBooks_logger.info("\t\tReading in file(s) containing list of files")
      if use_addGrid:
        cookBooks_logger.info("\t\tAdding samples using addGrid")
      else:
        cookBooks_logger.info("\t\tAdding using readFileList")
    else:
      if use_addGrid:
        cookBooks_logger.info("\t\tAdding samples using addGrid")
      else:
        cookBooks_logger.info("\t\tAdding samples using scanDir")

    for fname in args.input_filename:
      if args.use_inputFileList:
        if use_addGrid:
          with open(fname, 'r') as f:
            for line in f:
              if line.startswith('#'): continue
              ROOT.SH.addGrid(sh_all, line.rstrip())
        else:
          ROOT.SH.readFileList(sh_all, "sample", fname)
      else:
        if use_addGrid:
          ROOT.SH.addGrid(sh_all, fname)
        else:
          '''
          if fname.startswith("root://"):
            # magic!
            server, path = [string[::-1] for string in fname[::-1].split("//",1)][::-1]
            sh_list = ROOT.SH.DiskListXRD(server, '/{0:s}'.format(path), True)
            ROOT.SH.ScanDir().scan(sh_all, sh_list)
          else:
          '''
          # need to parse and split it up
          fname_base = os.path.basename(fname)
          sample_dir = os.path.dirname(os.path.abspath(fname))
          mother_dir = os.path.dirname(sample_dir)
          sh_list = ROOT.SH.DiskListLocal(mother_dir)
          ROOT.SH.scanDir(sh_all, sh_list, fname_base, os.path.basename(sample_dir))

    # print out the samples we found
    cookBooks_logger.info("\t%d different dataset(s) found", len(sh_all))
    if not use_addGrid:
      for dataset in sh_all:
        cookBooks_logger.info("\t\t%d files in %s", dataset.numFiles(), dataset.name())
    sh_all.printContent()

    if len(sh_all) == 0:
      cookBooks_logger.info("No datasets found. Exiting.")
      sys.exit(0)

    # set the name of the tree in our files (should be configurable)
    sh_all.setMetaString("nc_tree", "CollectionTree")

    # read susy meta data (should be configurable)
    cookBooks_logger.info("reading all metadata in $ROOTCOREBIN/data/TheAccountant")
    ROOT.SH.readSusyMetaDir(sh_all,"$ROOTCOREBIN/data/TheAccountant")

    # this is the basic description of our job
    cookBooks_logger.info("creating new job")
    job = ROOT.EL.Job()
    job.sampleHandler(sh_all)

    if args.driver == 'lsf':
      job.options().setBool(ROOT.EL.Job.optResetShell, False);

    if args.num_events > 0:
      cookBooks_logger.info("\tprocessing only %d events", args.num_events)
      job.options().setDouble(ROOT.EL.Job.optMaxEvents, args.num_events)

    if args.skip_events > 0:
      cookBooks_logger.info("\tskipping first %d events", args.skip_events)
      job.options().setDouble(ROOT.EL.Job.optSkipEvents, args.skip_events)

    # should be configurable
    job.options().setDouble(ROOT.EL.Job.optCacheSize, 50*1024*1024)
    job.options().setDouble(ROOT.EL.Job.optCacheLearnEntries, 50)

    # access mode branch
    if args.access_mode == 'branch':
      cookBooks_logger.info("\tusing branch access mode: ROOT.EL.Job.optXaodAccessMode_branch")
      job.options().setString( ROOT.EL.Job.optXaodAccessMode, ROOT.EL.Job.optXaodAccessMode_branch )
    else:
      cookBooks_logger.info("\tusing class access mode: ROOT.EL.Job.optXaodAccessMode_class")
      job.options().setString( ROOT.EL.Job.optXaodAccessMode, ROOT.EL.Job.optXaodAccessMode_class )

    # this is where we go over and process all algorithms
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

    user_confirm(args, 0)

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

    user_confirm(args, 1)

    optimization_dump = None
    if args.optimization_dump:
      optimization_dump = ROOT.OptimizationDump()
      cookBooks_logger.info("\tcreating optimization dump algorithm")
      algorithmConfiguration_string.append("optimization dump algorithm")
      # no other options for now...
      for opt in map(lambda x: x.dest, group_optimizationDump._group_actions):
        cookBooks_logger.info("\t%s", printStr.format('OptimizationDump', opt, getattr(args, opt)))
        algorithmConfiguration_string.append(printStr.format('OptimizationDump', opt, getattr(args, opt)))
        setattr(optimization_dump, 'm_{0}'.format(opt), getattr(args, opt))
        time.sleep(sleepTime)
     # time.sleep(sleepTime)

      user_confirm(args, 2)

    report = ROOT.Report()
    cookBooks_logger.info("\tcreating report algorithm")
    algorithmConfiguration_string.append("report algorithm options")
    for opt in map(lambda x: x.dest, group_report._group_actions):
      cookBooks_logger.info("\t%s", printStr.format('Report', opt, getattr(args, opt)))
      algorithmConfiguration_string.append(printStr.format('Report', opt, getattr(args, opt)))
      setattr(report, 'm_{0}'.format(opt), getattr(args, opt))
      time.sleep(sleepTime)

    user_confirm(args, 2+args.optimization_dump)

    cookBooks_logger.info("\tsetting global algorithm variables")
    algorithmConfiguration_string.append("global algorithm options")
    for alg in [preselect, audit, optimization_dump, report]:
      if alg is None: continue  # skip optimization_dump if not defined
      for opt in map(lambda x: x.dest, group_algorithms._group_actions):
        cookBooks_logger.info("\t%s", printStr.format(alg.ClassName(), opt, getattr(args, opt)))
        algorithmConfiguration_string.append(printStr.format(alg.ClassName(), opt, getattr(args, opt)))
        setattr(alg, 'm_{0}'.format(opt), getattr(args, opt))
        time.sleep(sleepTime)

    user_confirm(args, 3+args.optimization_dump)

    cookBooks_logger.info("adding algorithms")
    job.algsAdd(preselect)
    job.algsAdd(audit)
    if args.optimization_dump:
      job.algsAdd(optimization_dump)
    job.algsAdd(report)

    # make the driver we want to use:
    # this one works by running the algorithm directly
    cookBooks_logger.info("creating driver")
    cookBooks_logger.info("\trunning on {0:s}".format(args.driver))
    driver = None
    if (args.driver == "direct"):
      driver = ROOT.EL.DirectDriver()
    elif (args.driver == "prooflite"):
      driver = ROOT.EL.ProofDriver()
    elif (args.driver == "prun"):
      driver = ROOT.EL.PrunDriver()

      for opt, t in map(lambda x: (x.dest, x.type), prun._actions):
        if getattr(args, opt) is None: continue  # skip if not set
        if opt in ['help', 'optGridOutputSampleName']: continue  # skip some options
        if t in [float]:
          setter = 'setDouble'
        elif t in [int]:
          setter = 'setInteger'
        elif t in [bool]:
          setter = 'setBool'
        else:
          setter = 'setString'

        getattr(driver.options(), setter)(getattr(ROOT.EL.Job, opt), getattr(args, opt))
        cookBooks_logger.info("\t - driver.options().{0:s}({1:s}, {2})".format(setter, getattr(ROOT.EL.Job, opt), getattr(args, opt)))


      # "user.%nickname%.%in:name[2]%.%in:name[3]%.%in:name[4]%.%in:name[5]%.%in:name[6]%_TA{0:s}
      # "user.%nickname%.%in:name%_TA{0:s}"
      #nc_outputSampleNameStr = "user.%nickname%.%in:name[1]%.%in:name[2]%.%in:name[5]%.%in:name[6]%_TA{0:s}".format(args.optGridOutputSampleName)
      nc_outputSampleNameStr = "user.%nickname%.%in:name[4]%.%in:name[5]%.%in:name[6]%.%in:name[7]%_TA{0:s}".format(args.optGridOutputSampleName)
      driver.options().setString("nc_outputSampleName", nc_outputSampleNameStr)
      cookBooks_logger.info("\t - driver.options().setString(nc_outputSampleName, {0:s})".format(nc_outputSampleNameStr))

    elif (args.driver == "condor"):
      driver = ROOT.EL.CondorDriver()
      driver.options().setBool(ROOT.EL.Job.optBatchSharedFileSystem, False)
      driver.options().setString(ROOT.EL.Job.optCondorConf, args.optCondorConf)

    elif (args.driver == "lsf"):
      driver = ROOT.EL.LSFDriver()
      driver.options().setString(ROOT.EL.Job.optSubmitFlags, args.optLSFConf)
    elif (args.driver == "local"):
      driver = ROOT.EL.LocalDriver()

    user_confirm(args, 4+args.optimization_dump)

    cookBooks_logger.info("\tsubmit job")
    if args.driver in ["prun", "condor","lsf","local"]:
      driver.submitOnly(job, args.submit_dir)
    else:
      driver.submit(job, args.submit_dir)

    SCRIPT_END_TIME = datetime.datetime.now()

    with open(os.path.join(args.submit_dir, 'CookTheBooks.log'), 'w+') as f:
      f.write(' '.join(['[{0}]'.format(__version__), os.path.basename(sys.argv[0])] + sys.argv[1:]))
      f.write('\n')
      f.write('Code:  https://github.com/kratsg/TheAccountant/tree/{0}\n'.format(__short_hash__))
      f.write('Start: {0}\nStop:  {1}\nDelta: {2}\n\n'.format(SCRIPT_START_TIME.strftime("%b %d %Y %H:%M:%S"), SCRIPT_END_TIME.strftime("%b %d %Y %H:%M:%S"), SCRIPT_END_TIME - SCRIPT_START_TIME))
      f.write('job runner options\n')
      for opt in ['input_filename', 'submit_dir', 'num_events', 'skip_events', 'force_overwrite', 'use_inputFileList', 'use_addGrid', 'verbose', 'driver']:
        f.write('\t{0: <51} = {1}\n'.format(opt, getattr(args, opt)))
      for algConfig_str in algorithmConfiguration_string:
        f.write('{0}\n'.format(algConfig_str))

  except Exception, e:
    # we crashed
    cookBooks_logger.exception("{0}\nAn exception was caught!".format("-"*20))
