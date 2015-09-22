#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "SampleHandler/DiskListLocal.h"
#include <TSystem.h>

#include <TheAccountant/OptimizationDump.h>

int main( int argc, char* argv[] ) {

  // Take the submit directory from the input if provided:
  std::string submitDir = "submitDir";
  if( argc > 1 ) submitDir = argv[ 1 ];

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // Construct the samples to run on:
  SH::SampleHandler sh;

  std::string filename = "ttbar_xaod_nolarge_newst_systlink.root";
  std::string dataPath = gSystem->ExpandPathName("/share/t3data2/");
  SH::DiskListLocal list (dataPath);
  SH::scanDir (sh, list, filename, "swiatlow"); // specifying one particular sample

  // Set the name of the input TTree. It's always "CollectionTree"
  // for xAOD files.
  sh.setMetaString( "nc_tree", "CollectionTree" );

  // Print what we found:
  sh.print();

  // Create an EventLoop job:
  EL::Job job;
  job.sampleHandler( sh );


  OptimizationDump* optDump = new OptimizationDump();
  optDump->m_debug      = true;
  optDump->m_eventInfo  = "EventInfo";
  optDump->m_inputLargeRJets  = "";
  optDump->m_inputJets = "STCalibAntiKt4EMTopoJets";
  optDump->m_inputMET   = "";

  // Attach algorithms
  job.algsAdd( optDump );

  // Run the job using the local/direct driver:
  EL::DirectDriver driver;
  driver.submit( job, submitDir );

  return 0;
}


