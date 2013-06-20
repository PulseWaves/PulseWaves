/*
===============================================================================

  FILE:  pulsewriter_txt.cpp
  
  CONTENTS:
  
    see corresponding header file
  
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
  
    see corresponding header file
  
===============================================================================
*/
#include "pulsewriter_txt.hpp"

#include <stdlib.h>
#include <string.h>

BOOL PULSEwriterTXT::refile_pulse(FILE* file)
{
  pulse_file = file;
  return TRUE;
}

BOOL PULSEwriterTXT::refile_waves(FILE* file)
{
  waves_file = file;
  return TRUE;
}

BOOL PULSEwriterTXT::open(const char* pulse_file_name, const char* waves_file_name, const PULSEheader* header)
{
  // check arguments

  if (pulse_file_name == 0)
  {
    fprintf(stderr,"ERROR: pulse file name pointer is zero\n");
    return FALSE;
  }

  if (waves_file_name == 0)
  {
    fprintf(stderr,"ERROR: waves file name pointer is zero\n");
    return FALSE;
  }

  if (header == 0)
  {
    fprintf(stderr,"ERROR: header pointer is zero\n");
    return FALSE;
  }

  // check header contents

  if (!header->check()) return FALSE;

  // open output file(s)

  pulse_file = fopen(pulse_file_name, "w");
  if (pulse_file == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", pulse_file_name);
    return FALSE;
  }

  if (this->pulse_file_name) free(this->pulse_file_name);
  this->pulse_file_name = strdup(pulse_file_name);

  if (strcmp(pulse_file_name, waves_file_name) == 0)
  {
    waves_file = pulse_file;
  }
  else
  {
    waves_file = fopen(waves_file_name, "w");
    if (waves_file == 0)
    {
      fprintf(stderr, "ERROR: cannot open file '%s'\n", waves_file_name);
      return FALSE;
    }
  }

  if (this->waves_file_name) free(this->waves_file_name);
  this->waves_file_name = strdup(waves_file_name);

  return open(pulse_file, waves_file, header);
}

BOOL PULSEwriterTXT::open(FILE* pulse_file, FILE* waves_file, const PULSEheader* header)
{
  // check arguments

  if (pulse_file == 0)
  {
    fprintf(stderr,"ERROR: pulse file pointer is zero\n");
    return FALSE;
  }

  if (waves_file == 0)
  {
    fprintf(stderr,"ERROR: waves file pointer is zero\n");
    return FALSE;
  }

  if (header == 0)
  {
    fprintf(stderr,"ERROR: header pointer is zero\n");
    return FALSE;
  }

  // check header contents

  if (!header->check()) return FALSE;

  // copy output file(s)

  this->pulse_file = pulse_file;
  this->waves_file = waves_file;

  return open(header);
}

BOOL PULSEwriterTXT::open(const PULSEheader* header)
{
  // copy scale_and_offset

  quantizer.x_scale_factor = header->x_scale_factor;
  quantizer.y_scale_factor = header->y_scale_factor;
  quantizer.z_scale_factor = header->z_scale_factor;
  quantizer.x_offset = header->x_offset;
  quantizer.y_offset = header->y_offset;
  quantizer.z_offset = header->z_offset;

  // check if the requested pulse type is supported

  PULSEpulse pulse;
  
  if (!pulse.init(header, header)) return FALSE;

  // write the header

  if (!header->save_to_txt(pulse_file))
  {
    fprintf(stderr,"ERROR: saving the header\n");
    return FALSE;
  }

  npulses = header->number_of_pulses;
  p_count = 0;

  return TRUE;
}

BOOL PULSEwriterTXT::write_pulse(const PULSEpulse* pulse)
{
  if (pulse == 0)
  {
    fprintf(stderr,"ERROR: pulse pointer is zero\n");
    return FALSE;
  }
  if (pulse_file == 0)
  {
    fprintf(stderr,"ERROR: pulse_file pointer is zero\n");
    return FALSE;
  }
  if (!pulse->save_to_txt(pulse_file))
  {
#ifdef _WIN32
    fprintf(stderr,"ERROR: writing pulse %I64d\n", p_count);
#else
    fprintf(stderr,"ERROR: writing pulse %lld\n", p_count);
#endif
    return FALSE;
  }
  p_count++;
  return TRUE;
}

BOOL PULSEwriterTXT::write_waves(const WAVESwaves* waves)
{
  if (waves == 0)
  {
    fprintf(stderr,"ERROR: waves pointer is zero\n");
    return FALSE;
  }
  if (waves_file == 0)
  {
    fprintf(stderr,"ERROR: waves_file pointer is zero\n");
    return FALSE;
  }
  if (!waves->save_to_txt(waves_file))
  {
#ifdef _WIN32
    fprintf(stderr,"ERROR: writing waves for pulse %I64d\n", p_count);
#else
    fprintf(stderr,"ERROR: writing waves for pulse %lld\n", p_count);
#endif
    return FALSE;
  }
  return TRUE;
}

I64 PULSEwriterTXT::get_current_offset() const
{
  if (waves_file == 0) 
  {
    return 0;
  }
  return ftell(waves_file);
}

BOOL PULSEwriterTXT::update_header(const PULSEheader* header, BOOL use_inventory, BOOL update_extra_bytes)
{
  if (pulse_file == 0)
  {
    fprintf(stderr,"ERROR: file pointer is zero\n");
    return FALSE;
  }
  if ((pulse_file == stdout) || (pulse_file == stderr))
  {
    fprintf(stderr,"ERROR: file is not seekable\n");
    return FALSE;
  }

  return TRUE;
}

I64 PULSEwriterTXT::close(BOOL update_header)
{
  I64 bytes = 0;

  if (p_count != npulses)
  {
#ifdef _WIN32
    fprintf(stderr,"WARNING: written %I64d pulses but expected %I64d pulses\n", p_count, npulses);
#else
    fprintf(stderr,"WARNING: written %lld pulses but expected %lld pulses\n", p_count, npulses);
#endif
  }

  if (update_header && p_count != npulses)
  {
    if ((pulse_file == stdout) || (pulse_file == stderr))
    {
#ifdef _WIN32
      fprintf(stderr, "ERROR: pulse_file not seekable. cannot update header from %I64d to %I64d pulses.\n", npulses, p_count);
#else
      fprintf(stderr, "ERROR: pulse_file not seekable. cannot update header from %lld to %lld pulses.\n", npulses, p_count);
#endif
    }
    else
    {
#ifdef _WIN32
      fprintf(stderr, "WARNING: update_header not implemented. will not update header from %I64d to %I64d pulses.\n", npulses, p_count);
#else
      fprintf(stderr, "ERROR: update_header not implemented. will not update header from %lld to %lld pulses.\n", npulses, p_count);
#endif
    }
  }

  if (pulse_file)
  {
    bytes = ftell(pulse_file);
    fclose(pulse_file);
    pulse_file = 0;
  }

  if (strcmp(pulse_file_name, waves_file_name) == 0)
  {
    waves_file = 0;
  }
  else if (waves_file)
  {
    bytes += ftell(waves_file);
    fclose(waves_file);
    waves_file = 0;
  }

  npulses = p_count;
  p_count = 0;

  return bytes;
}

PULSEwriterTXT::PULSEwriterTXT()
{
  pulse_file_name = 0;
  waves_file_name = 0;
  pulse_file = 0;
  waves_file = 0;
}

PULSEwriterTXT::~PULSEwriterTXT()
{
  if (pulse_file_name) free(pulse_file_name);
  if (waves_file_name) free(waves_file_name);
  if (pulse_file) fclose(pulse_file);
  if (waves_file) fclose(waves_file);
}
