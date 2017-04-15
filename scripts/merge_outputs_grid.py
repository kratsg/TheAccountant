import os
import re
import glob
import subprocess

import argparse
parser = argparse.ArgumentParser(description="Combine your grid outputs by hadd'ing them")

parser.add_argument('top_level', metavar='<directory>', type=str, help='Top-Level directory containing files. Example: "*output_histfitter.root/"')
parser.add_argument('file_pattern', metavar='<file pattern>', type=str, help='File pattern on which we group files to find LCS for merging. Example: "*.root"')
parser.add_argument('-o', '--output', type=str, required=False, help='Output directory to store merged outputs', default='.')
parser.add_argument('-f', '--force', action='store_true', required=False, help='Overwrite existing output files', default=None)

# parse the arguments, throw errors if missing any
args = parser.parse_args()

# paths is a bunch of paths to where all your outputs have been dumped that you want to merge
paths = glob.glob(args.top_level)
outfiles = []
for path in paths:

    outdir = args.output if args.output is not None else path
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    outfile = re.sub("\.root/$",".merged", path)
    cmd = "hadd {0:s} {1:s}".format(os.path.join(outdir, outfile), os.path.join(os.path.dirname(path), args.file_pattern))
    if args.force:
        cmd = "hadd -f {0:s} {1:s}".format(os.path.join(outdir, outfile), os.path.join(os.path.dirname(path), args.file_pattern))
    print(cmd)
    subprocess.call([cmd], shell=True)
    outfiles.append("{0:s}".format(outfile))

import pprint
pprint.pprint(outfiles)
