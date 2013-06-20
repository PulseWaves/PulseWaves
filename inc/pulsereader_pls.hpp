/*
===============================================================================

  FILE:  pulsereader_pls.hpp
  
  CONTENTS:
  
    Reads LiDAR Pulses from the PulseWaves format (*.pls).

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
#ifndef PULSE_READER_PLS_HPP
#define PULSE_READER_PLS_HPP

#include "pulsereader.hpp"

#include <stdio.h>

#ifdef LZ_WIN32_VC6
#include <fstream.h>
#else
#include <istream>
#include <fstream>
using namespace std;
#endif

class PULSEreadPulse;
class PULSEreadWaves;

class PULSEreaderPLS : public PULSEreader
{
public:

  BOOL open(const char* file_name, U32 io_buffer_size=65536);
  BOOL open(FILE* file);
  BOOL open(istream& stream);

  I32 get_format() const;

  BOOL seek(const I64 p_index);
  BOOL read_waves();

  ByteStreamIn* get_pulse_stream() const;
  ByteStreamIn* get_waves_stream() const;
  void close(BOOL close_streams=TRUE);

  PULSEreaderPLS();
  virtual ~PULSEreaderPLS();

protected:
  virtual BOOL open(ByteStreamIn* stream);
  virtual BOOL read_pulse_default();

private:
  BOOL decompress_waves;
  CHAR* file_name;
  FILE* pulse_file;
  FILE* waves_file;
  ByteStreamIn* pulse_stream;
  ByteStreamIn* waves_stream;
  PULSEreadPulse* pulse_reader;
  PULSEreadWaves* waves_reader;
  BOOL open_waves();
  WAVESwaves* waves_map[256];
};

#endif
