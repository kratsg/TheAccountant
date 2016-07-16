#!/usr/bin/env python

# @file:    GetWeights.py
# @purpose: For a given list of samples, produce a working weights.json file
# @author:  Giordon Stark <gstark@cern.ch>
# @date:    August 2015
#
# @example:
# @code
# GetWeights.py --help
# @endcode
#

from __future__ import print_function
import logging

root_logger = logging.getLogger()
root_logger.addHandler(logging.StreamHandler())
getWeights_logger = logging.getLogger("getWeights")

import argparse
import os
import subprocess
import sys
import datetime
import time
import re
import json
from multiprocessing import Process, Queue

SCRIPT_START_TIME = datetime.datetime.now()

# think about using argcomplete
# https://argcomplete.readthedocs.org/en/latest/#activating-global-completion%20argcomplete

if __name__ == "__main__":

  # catch CTRL+C
  import signal
  def signal_handler(signal, frame):
    print("Exiting the program now. Have a nice day!{0:s}".format(" "*40))  # extra spaces just in case
    sys.exit(0)
  signal.signal(signal.SIGINT, signal_handler)

  did_regex = re.compile('\.?(?:00)?(\d{6,8})\.?')
  def get_did(filename):
    global did_regex
    m = did_regex.search(filename)
    if m is None: raise ValueError('Can\'t figure out the DID! Filename: {0:s}'.format(filename))
    return m.group(1)

  # pass in the filename, get a cutflow number
  def get_cutflow(fname, numErrors=0):
    if numErrors > 3:
      return fname
    try:
      f = ROOT.TFile.Open(fname, "READ")
      cutflow = f.Get("cut_flow")
      count = cutflow.GetBinContent(2)
      f.Close()
      return count
    except:
      getWeights_logger.exception("{0}\nAn exception was caught for {1:s}!".format("-"*20, fname))
      return get_cutflow(fname, numErrors+1)

  # if we want multiple custom formatters, use inheriting
  class CustomFormatter(argparse.ArgumentDefaultsHelpFormatter):
    pass

  __version__ = subprocess.check_output(["git", "describe", "--always"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()
  __short_hash__ = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"], cwd=os.path.dirname(os.path.realpath(__file__))).strip()

  parser = argparse.ArgumentParser(add_help=True, description='Make a proper weights file using cutflow information and susytools metadata.',
                                   usage='%(prog)s --files ... file [file ...] [options] {driver} [driver options]',
                                   formatter_class=lambda prog: CustomFormatter(prog, max_help_position=30))

  # http://stackoverflow.com/a/16981688
  parser._positionals.title = "required"
  parser._optionals.title = "optional"

  # positional argument, require the first argument to be the input filename
  parser.add_argument('files', type=str, nargs='+', help='input file(s) to read')
  parser.add_argument('-o', '--output', dest='output_filename', metavar='file', type=str, help='Output filename', default='weights.json')
  parser.add_argument('-f', '--force', dest='force_overwrite', action='store_true', help='Overwrite previous output if it exists.')
  parser.add_argument('--version', action='version', version='%(prog)s {version}'.format(version=__version__))

  parser.add_argument('--inputList', dest='use_inputFileList', action='store_true', help='If enabled, will read in a text file containing a list of files.')
  parser.add_argument('--inputGrid', dest='use_addGrid', action='store_true', help='If enabled, will search using DQ2. Can be combined with `--inputList`.')
  parser.add_argument('-v', '--verbose', dest='verbose', action='count', default=0, help='Enable verbose output of various levels. Default: no verbosity')
  parser.add_argument('-y', '--yes', dest='skip_confirm', action='count', default=0, help='Skip the configuration confirmations. Useful for when running in the background.')

  parser.add_argument('--debug', dest='debug', action='store_true', help='Enable verbose output of the algorithms.')

  # parse the arguments, throw errors if missing any
  args = parser.parse_args()

  # set verbosity for python printing
  if args.verbose < 4:
    getWeights_logger.setLevel(20 - args.verbose*5)
  else:
    getWeights_logger.setLevel(logging.NOTSET + 1)

  try:
    import timing

    # check submission directory
    if args.force_overwrite:
      getWeights_logger.info("removing {0:s}".format(args.output_filename))
      try:
        os.remove(args.output_filename)
      except OSError:
        pass
    else:
      # check if directory exists
      if os.path.exists(args.output_filename):
        raise OSError('Output file {0:s} already exists. Either re-run with -f/--force, choose a different --output, or rm it yourself. Just deal with it, dang it.'.format(args.output_filename))

    # they will need it to get it working
    if args.use_addGrid:
      if os.getenv('XRDSYS') is None:
        raise EnvironmentError('xrootd client is not setup. Run localSetupFAX or equivalent.')

    # at this point, we should import ROOT and do stuff
    import ROOT
    getWeights_logger.info("loading packages")
    ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")

    #Set up the job for xAOD access:
    ROOT.xAOD.Init("GetWeights").ignore();

    # create a new sample handler to describe the data files we use
    getWeights_logger.info("creating new sample handler")
    sh_all = ROOT.SH.SampleHandler()

    # this portion is just to output for verbosity
    if args.use_inputFileList:
      getWeights_logger.info("\t\tReading in file(s) containing list of files")
      if args.use_addGrid:
        getWeights_logger.info("\t\tAdding samples using addGrid")
      else:
        getWeights_logger.info("\t\tAdding using readFileList")
    else:
      if args.use_addGrid:
        getWeights_logger.info("\t\tAdding samples using addGrid")
      else:
        getWeights_logger.info("\t\tAdding samples using scanDir")

    for fname in args.files:
      if args.use_inputFileList:
        if args.use_addGrid:
          with open(fname, 'r') as f:
            for line in f:
              if line.startswith('#'): continue
              ROOT.SH.addGrid(sh_all, line.rstrip())
        else:
          ROOT.SH.readFileList(sh_all, os.path.basename(fname).replace('.list',''), fname)
      else:
        if args.use_addGrid:
          ROOT.SH.addGrid(sh_all, fname)
        else:
          # need to parse and split it up
          fname_base = os.path.basename(fname)
          sample_dir = os.path.dirname(os.path.abspath(fname))
          mother_dir = os.path.dirname(sample_dir)
          sh_list = ROOT.SH.DiskListLocal(mother_dir)
          ROOT.SH.scanDir(sh_all, sh_list, fname_base, os.path.basename(sample_dir))

    # print out the samples we found
    getWeights_logger.info("\t%d different dataset(s) found", len(sh_all))
    if not args.use_addGrid:
      for dataset in sh_all:
        getWeights_logger.info("\t\t%d files in %s", dataset.numFiles(), dataset.name())
    sh_all.printContent()

    if len(sh_all) == 0:
      getWeights_logger.info("No datasets found. Exiting.")
      sys.exit(0)

    # set the name of the tree in our files (should be configurable)
    sh_all.setMetaString("nc_tree", "CollectionTree")

    # read susy meta data (should be configurable)
    getWeights_logger.info("reading all metadata in $ROOTCOREBIN/data/TheAccountant")
    ROOT.SH.readSusyMetaDir(sh_all,"$ROOTCOREBIN/data/TheAccountant/metadata")
    ROOT.SH.readSusyMetaDir(sh_all,"$ROOTCOREBIN/data/TheAccountant/metadata/mc15_13TeV")

    # get cutflow number
    # if there is any sort of error with the file, retry up to 3 times
    #   before just completely erroring out
    def get_cutflow_parallel(queue, sample):
      did = get_did(sample.name())
      getWeights_logger.info("Analyzing DID#{0:s} from sample {1:s}".format(did, sample.name()))
      try:
        for fname in sample.makeFileList():
          queue.put((did, get_cutflow(fname)))
      except Exception, e:
        # we crashed
        getWeights_logger.exception("{0}\nAn exception was caught!".format("-"*20))

      # tell master we're done
      queue.put(None)

    # dictionary to hold results of all calculations
    weights = {}
    # a process for each sample
    num_procs = len(sh_all)
    # communication queue between slave processes and master
    queue = Queue()
    procs = list()

    getWeights_logger.info("spinning up {0:d} processes".format(num_procs))

    # cache lookup
    MFs = ROOT.SH.MetaFields
    # for each sample
    for sample in sh_all:
      did = get_did(sample.name())

      weights[did] = {'num events': 0,
                      'errors': [],
                      'cross section': sample.getMetaDouble(MFs.crossSection),
                      'filter efficiency': sample.getMetaDouble(MFs.filterEfficiency),
                      'k-factor': sample.getMetaDouble(MFs.kfactor),
                      'rel uncert': sample.getMetaDouble(MFs.crossSectionRelUncertainty)}
      p = Process(target=get_cutflow_parallel, args=(queue, sample))
      procs.append(p)
      p.start()

    finished = 0
    while finished < num_procs:
      item = queue.get()
      if item is None:
        finished += 1
      else:
        did, count = item
        getWeights_logger.info("DID#{0:s} returned {1:s}".format(did, str(count)))
        # we return the filename if we can't open it for reading
        try:
          weights[did]['num events'] += float(count)
        except (ValueError, TypeError) as e:
          weights[did]['errors'].append(count)
        # whatever happens, write to the file
        with open(args.output_filename, 'w+') as f:
          f.write(json.dumps(weights, sort_keys=True, indent=4))


    SCRIPT_END_TIME = datetime.datetime.now()

    with open(args.output_filename, 'w+') as f:
      f.write(json.dumps(weights, sort_keys=True, indent=4))

  except Exception, e:
    # we crashed
    getWeights_logger.exception("{0}\nAn exception was caught!".format("-"*20))
