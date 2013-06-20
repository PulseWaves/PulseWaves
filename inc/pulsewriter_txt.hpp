/*
===============================================================================

  FILE:  pulsewriter_txt.hpp
  
  CONTENTS:
  
    Writes Pulses in an ASCII verion of the PulseWaves (*.txt) format.

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
  
    7 Juni 2012 -- created ICE 20 from Wels to Wuerzburg after AGIT in Salzburg
  
===============================================================================
*/
#ifndef PULSE_WRITER_TXT_HPP
#define PULSE_WRITER_TXT_HPP

#include "pulsewriter.hpp"

#include <stdio.h>

class PULSEwriterTXT : public PULSEwriter
{
public:

  BOOL refile_pulse(FILE* file_pulse);
  BOOL refile_waves(FILE* file_waves);

  BOOL open(const char* pulse_file_name, const char* waves_file_name, const PULSEheader* header);
  BOOL open(FILE* pulse_file, FILE* waves_file, const PULSEheader* header);
  BOOL open_waves() { return (waves_file ? TRUE : FALSE); };

  BOOL write_pulse(const PULSEpulse* pulse);
  BOOL write_waves(const WAVESwaves* waves);

  I64 get_current_offset() const;
  BOOL update_header(const PULSEheader* header, BOOL use_inventory=TRUE, BOOL update_extra_bytes=FALSE);
  I64 close(BOOL update_npulses=true);

  PULSEwriterTXT();
  ~PULSEwriterTXT();

private:
  BOOL open(const PULSEheader* header);
  CHAR* pulse_file_name;
  CHAR* waves_file_name;
  FILE* pulse_file;
  FILE* waves_file;
};

#endif
