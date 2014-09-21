/*
===============================================================================

  FILE:  pulsewriter_pls.hpp
  
  CONTENTS:
  
    Writes Pulses in the PulseWaves (*.pls) format.

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
  
    16 February 2012 -- created in the cafe where it smelled like 1917 Chestnut
  
===============================================================================
*/
#ifndef PULSE_WRITER_PLS_HPP
#define PULSE_WRITER_PLS_HPP

#include "pulsewriter.hpp"

#include <stdio.h>

#ifdef LZ_WIN32_VC6
#include <fstream.h>
#else
#include <istream>
#include <fstream>
using namespace std;
#endif

class ByteStreamOut;
class PULSEwritePulse;
class PULSEwriteWaves;

class PULSEwriterPLS : public PULSEwriter
{
public:

  BOOL refile_pulse(FILE* file_pulse);
  BOOL refile_waves(FILE* file_waves);

  BOOL open(PULSEheader* header, U32 compress=0);
  BOOL open(const char* file_name, PULSEheader* header, U32 compress=0, U32 io_buffer_size=65536);
  BOOL open(FILE* file, PULSEheader* header, U32 compress=0);
  BOOL open(ostream& ostream, PULSEheader* header, U32 compress=0);
  BOOL open_waves();

  BOOL write_pulse(const PULSEpulse* pulse);
  BOOL write_waves(const WAVESwaves* waves);

  I64 get_current_offset() const;
  BOOL update_header(const PULSEheader* header, BOOL use_inventory=TRUE, BOOL update_extra_bytes=FALSE);
  I64 close(BOOL update_npulses=true);

  PULSEwriterPLS();
  ~PULSEwriterPLS();

private:
  BOOL compress;
  CHAR* file_name;
  FILE* pulse_file;
  FILE* waves_file;
  ByteStreamOut* pulse_stream;
  ByteStreamOut* waves_stream;
  PULSEwritePulse* pulse_writer;
  PULSEwriteWaves* waves_writer;
  BOOL open(ByteStreamOut* stream, PULSEheader* header, U32 compress);
};

#endif
