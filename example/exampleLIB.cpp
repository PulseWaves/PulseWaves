/*
===============================================================================

  FILE:  exampleLIB.cpp
  
  CONTENTS:
  
    This source code serves as an example how you can easily use PulseWaves to
    write your own waveform processing tools or how to import full waveformhow 
    LiDAR from and export full waveform LiDAR to the PulseWaves format.

  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2007-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    27 February 2012 -- created between TFS & FRA after in-flight meal & coffee
  
===============================================================================
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pulsereader.hpp"
#include "pulsewriter.hpp"

void usage(bool wait=false)
{
  fprintf(stderr,"usage:\n");
  fprintf(stderr,"exampleLIB -i in.gcw -o out.pls -verbose\n");
  fprintf(stderr,"exampleLIB -i in.sdf -o out.pls -verbose\n");
  fprintf(stderr,"exampleLIB -h\n");
  if (wait)
  {
    fprintf(stderr,"<press ENTER>\n");
    getc(stdin);
  }
  exit(1);
}

static void byebye(bool error=false, bool wait=false)
{
  if (wait)
  {
    fprintf(stderr,"<press ENTER>\n");
    getc(stdin);
  }
  exit(error);
}

static double taketime()
{
  return (double)(clock())/CLOCKS_PER_SEC;
}

int main(int argc, char *argv[])
{
  int i;
  bool verbose = false;
  double start_time = 0.0;

  PULSEreadOpener pulsereadopener;
  PULSEwriteOpener pulsewriteopener;

  if (argc == 1)
  {
    fprintf(stderr,"pls2pls.exe is better run in the command line\n");
    char file_name[256];
    fprintf(stderr,"enter input file: "); fgets(file_name, 256, stdin);
    file_name[strlen(file_name)-1] = '\0';
    pulsereadopener.set_file_name(file_name);
    fprintf(stderr,"enter output file: "); fgets(file_name, 256, stdin);
    file_name[strlen(file_name)-1] = '\0';
    pulsewriteopener.set_file_name(file_name);
  }
  else
  {
    pulsereadopener.parse(argc, argv);
    pulsewriteopener.parse(argc, argv);
  }

  for (i = 1; i < argc; i++)
  {
    if (argv[i][0] == '\0')
    {
      continue;
    }
    else if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"-help") == 0)
    {
      usage();
    }
    else if (strcmp(argv[i],"-v") == 0 || strcmp(argv[i],"-verbose") == 0)
    {
      verbose = true;
    }
    else if (i == argc - 2 && !pulsereadopener.active() && !pulsewriteopener.active())
    {
      pulsereadopener.set_file_name(argv[i]);
    }
    else if (i == argc - 1 && !pulsereadopener.active() && !pulsewriteopener.active())
    {
      pulsereadopener.set_file_name(argv[i]);
    }
    else if (i == argc - 1 && pulsereadopener.active() && !pulsewriteopener.active())
    {
      pulsewriteopener.set_file_name(argv[i]);
    }
    else
    {
      fprintf(stderr, "ERROR: cannot understand argument '%s'\n", argv[i]);
      usage();
    }
  }

  if (verbose) start_time = taketime();

  // check input & output

  if (!pulsereadopener.active())
  {
    fprintf(stderr,"ERROR: no input specified\n");
    usage(argc == 1);
  }

  if (!pulsewriteopener.active())
  {
    fprintf(stderr,"ERROR: no output specified\n");
    usage(argc == 1);
  }

  // open pulsereader

  PULSEreader* pulsereader = pulsereadopener.open();
  if (pulsereader == 0)
  {
    fprintf(stderr, "ERROR: could not open pulsereader\n");
    byebye(argc==1);
  }

  // open pulsewriter

  PULSEwriter* pulsewriter = pulsewriteopener.open(&pulsereader->header);
  if (pulsewriter == 0)
  {
    fprintf(stderr, "ERROR: could not open pulsewriter\n");
    byebye(argc==1);
  }

#ifdef _WIN32
  if (verbose) fprintf(stderr, "reading %I64d pulses from '%s' and writing them modified to '%s'.\n", pulsereader->npulses, pulsereadopener.get_file_name(), pulsewriteopener.get_file_name());
#else
  if (verbose) fprintf(stderr, "reading %lld pulses from '%s' and writing them modified to '%s'.\n", pulsereader->npulses, pulsereadopener.get_file_name(), pulsewriteopener.get_file_name());
#endif

  // loop over pulses and modify them

  // where there is a pulse to read
  while (pulsereader->read_pulse())
  {
    // modify the pulse
    pulsereader->pulse.intensity += 1;
    // write the modified pulse
    pulsewriter->write_pulse(&pulsereader->pulse);
    // add it to the inventory
    pulsewriter->update_inventory(&pulsereader->pulse);
  } 

  pulsewriter->update_header(&pulsereader->header, TRUE);

  I64 total_bytes = pulsewriter->close();
  delete pulsewriter;

#ifdef _WIN32
  if (verbose) fprintf(stderr,"total time: %g sec %I64d bytes for %I64d pulses\n", taketime()-start_time, total_bytes, pulsereader->p_count);
#else
  if (verbose) fprintf(stderr,"total time: %g sec %lld bytes for %lld pulses\n", taketime()-start_time, total_bytes, pulsereader->p_count);
#endif

  pulsereader->close();
  delete pulsereader;

  return 0;
}
