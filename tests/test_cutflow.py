# this is a cutflow tester for DSID 370139
from __future__ import print_function

import ROOT
import sys
from operator import itemgetter

# the cutflow is defined from top to bottom, adding them in sequence a&&b&&c&&...&&end
SR1 = [{'name': 'preselection', 'sel': '1', 'count': 26270},
       {'name': 'met', 'sel': 'met>=300.', 'count': 23043},
       {'name': 'njet', 'sel': 'multiplicity_jet>=8', 'count': 18883},
       {'name': 'nbjet', 'sel': 'multiplicity_jet_b>=4', 'count': 10159},
       {'name': 'mTb', 'sel': 'mTb>=160.', 'count': 8257},
       {'name': 'meff', 'sel': 'm_effective>=1200.', 'count': 8201},
       {'name': 'ntop', 'sel': 'multiplicity_topTag_veryloose>=0', 'count': 8201}]

SR4 = [{'name': 'preselection', 'sel': '1', 'count': 26270},
       {'name': 'met', 'sel': 'met>=400.', 'count': 18857},
       {'name': 'njet', 'sel': 'multiplicity_jet>=8', 'count': 15412},
       {'name': 'nbjet', 'sel': 'multiplicity_jet_b>=3', 'count': 13086},
       {'name': 'mTb', 'sel': 'mTb>=80.', 'count': 11836},
       {'name': 'meff', 'sel': 'm_effective>=1700.', 'count': 10470},
       {'name': 'ntop', 'sel': 'multiplicity_topTag_veryloose>=2', 'count': 6990}]

cutflows = {'SR1': SR1, 'SR4': SR4}

def main():
  if len(sys.argv) != 2:
    print("Please run by passing in the path to the ntuple to look at.")
    return
  f = ROOT.TFile.Open(sys.argv[1])
  t = f.Get('oTree')

  for name, cutflow in cutflows.iteritems():
    print("Looking at cutflow for {0:s}".format(name))
    for i in range(len(cutflow)):
      selection = ')&&('.join(map(itemgetter('sel'), cutflow[:i+1]))
      assString = " {0:>15s} should have {1:6d} raw events."
      if i == 0:
        assString = "\tAssert"+assString
      if i > 0:
        assString = "\t    &&"+assString
      count = cutflow[i]['count']
      print(assString.format(cutflow[i]['name'], count), end="")
      actualCount = t.GetEntries('({0:s})'.format(selection))
      print(" It has {0:6d} raw events.".format(actualCount))
      assert count == actualCount

if __name__ == '__main__':
  main()
  print("All validated!")
