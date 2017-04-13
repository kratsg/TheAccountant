from collections import defaultdict
import os
import glob
import subprocess

import argparse
parser = argparse.ArgumentParser(description="Combine your outputs by hadd'ing them")

parser.add_argument('top_level', metavar='<directory>', type=str, help='Top-Level directory containing files. Example: "TA03/*/fetch"')
parser.add_argument('file_pattern', metavar='<file pattern>', type=str, help='File pattern on which we group files to find LCS for merging. Example: "hist-*"')
parser.add_argument('-o', '--output', type=str, required=False, help='Output directory to store merged outputs', default=None)

# parse the arguments, throw errors if missing any
args = parser.parse_args()

# paths is a bunch of paths to where all your outputs have been dumped that you want to merge
paths = glob.glob(args.top_level)
outfiles = []
for path in paths:
    # we use the pattern of hist-* here, but it could be replaced with something else!
    files = glob.glob(os.path.join(path, args.file_pattern))

    bases = defaultdict(list)
    while files:
        f = files.pop(0)
        fname = os.path.basename(f)
        dirname = os.path.dirname(f)
        # let's see if it's of the form "base-#.root"
        parts = fname.rsplit('-', 1)

        if len(parts) == 1:
          parts = os.path.basename(dirname).split('.root')

        bases[parts[0]].append(f)
        # add all other files that contain this basepath
        bases[parts[0]].extend([i for i in files if parts[0] in i])
        # delete from the files we have now
        files = [i for i in files if parts[0] not in i]

    outdir = args.output if args.output is not None else path
    if not os.path.exists(outdir):
        os.makedirs(outdir)
    for outfile, infiles in bases.iteritems():
        cmd = "hadd {0:s}.merged {1:s}*".format(os.path.join(outdir, outfile), os.path.join(os.path.dirname(infiles[0]), outfile))
        print(cmd)
        subprocess.call([cmd], shell=True)
        outfiles.append("{0:s}.merged".format(outfile))

import pprint
pprint.pprint(outfiles)
