/*
===============================================================================

  FILE:  exampleLIB1.cpp
  
  CONTENTS:
  
    This source code serves as annother example how you can use PulseWaves to
    write your own waveform processing tools.

  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2007-2015, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    15 November 2015 -- created between flights going ITM->HND->MNL->SIN->KUL
  
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
  fprintf(stderr,"example1LIB -i in.gcw -o out.txt -verbose\n");
  fprintf(stderr,"example1LIB -i in.sdf -o out.txt -verbose\n");
  fprintf(stderr,"example1LIB -h\n");
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
    fprintf(stderr,"example1LIB.exe is better run in the command line\n");
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

  // open output ASCII file

  FILE* file = fopen(pulsewriteopener.get_file_name(), "w");
  if (file == 0)
  {
    fprintf(stderr, "ERROR: could not open '%s' \n", pulsewriteopener.get_file_name());
    byebye(argc==1);
  }

#ifdef _WIN32
  if (verbose) fprintf(stderr, "reading %I64d pulses from '%s' and writing their waveforms with time stamps to '%s'.\n", pulsereader->npulses, pulsereadopener.get_file_name(), pulsewriteopener.get_file_name());
#else
  if (verbose) fprintf(stderr, "reading %lld pulses from '%s' and writing their waveforms with time stamps to '%s'.\n", pulsereader->npulses, pulsereadopener.get_file_name(), pulsewriteopener.get_file_name());
#endif

  // as long as there is a pulse to read

  while (pulsereader->read_pulse())
  {
#ifdef _WIN32
    fprintf(file, "pulse %I64d of %I64d with time stamp T = %I64d\012", pulsereader->p_count, pulsereader->npulses, pulsereader->pulse.T);
#else
    fprintf(file, "pulse %lld of %lld with time stamp T = %lld\012", pulsereader->p_count, pulsereader->npulses, pulsereader->pulse.T);
#endif
   
    // report outgoing and returning waveforms

    if (pulsereader->read_waves())
    {
      BOOL outgoing_waveform_found = FALSE;
      BOOL returning_waveform_found = FALSE;

      // first report outgoing waveforms

      U16 m;
      for (m = 0; m < pulsereader->waves->get_number_of_samplings(); m++)
      {
        WAVESsampling* sampling = pulsereader->waves->get_sampling(m);
        if (sampling->get_type() == PULSEWAVES_OUTGOING)
        {
          if (outgoing_waveform_found)
          {
            fprintf(file, "  WARNING: outgoing waveform found in more than one samplings\012");
          }
          outgoing_waveform_found = TRUE;

          if (sampling->get_number_of_segments() == 0)
          {
            fprintf(file, "  WARNING: outgoing waveform sampling %u has no segments\012", (U32)m);
          }
          else
          {
            if (sampling->get_number_of_segments() > 1)
            {
              fprintf(file, "  WARNING: outgoing waveform sampling %u has %u segments \012", (U32)m, (U32)sampling->get_number_of_segments());
            }
            fprintf(file, "  outgoing waveform:\012");
          }

          U16 n;
          for (n = 0; n < sampling->get_number_of_segments(); n++)
          {
            sampling->set_active_segment(n);
            fprintf(file, "  segment %u starts at time %g: ", (U32)n, sampling->get_duration_from_anchor_for_segment());

            I32 s;
            for (s = 0; s < sampling->get_number_of_samples(); s++)
            {
              fprintf(file, "%d ", sampling->get_sample(s));
            }
            fprintf(file, "\012");
          }
        }
      }

      if (!outgoing_waveform_found)
      {
        fprintf(file, "  WARNING: no outgoing waveform found\012");
      }

      // then report all returning waveforms

      for (m = 0; m < pulsereader->waves->get_number_of_samplings(); m++)
      {
        WAVESsampling* sampling = pulsereader->waves->get_sampling(m);
        if (sampling->get_type() == PULSEWAVES_RETURNING)
        {
          returning_waveform_found = TRUE;
          fprintf(file, "  returning waveform:\012");

          U16 n;
          for (n = 0; n < sampling->get_number_of_segments(); n++)
          {
            sampling->set_active_segment(n);
            fprintf(file, "  segment %u starts at time %g: ", (U32)n, sampling->get_duration_from_anchor_for_segment());

            I32 s;
            for (s = 0; s < sampling->get_number_of_samples(); s++)
            {
              fprintf(file, "%d ", sampling->get_sample(s));
            }
            fprintf(file, "\012");
          }
        }
      }

      if (!returning_waveform_found)
      {
        fprintf(file, "  WARNING: no returning waveform found\012");
      }
    }
    else
    {
      fprintf(file, "  WARNING: failed to read waves\120");
    }
  }

  fclose(file);
  file = 0;

#ifdef _WIN32
  if (verbose) fprintf(stderr,"total time: %g sec for %I64d pulses\n", taketime()-start_time, pulsereader->p_count);
#else
  if (verbose) fprintf(stderr,"total time: %g sec for %lld pulses\n", taketime()-start_time, pulsereader->p_count);
#endif

  pulsereader->close();
  delete pulsereader;

  return 0;
}
