/*
===============================================================================

  FILE:  pulsereader_las.hpp
  
  CONTENTS:
  
    Reads LiDAR Pulses and Waves from the LAS 1.3 or LAS 1.4 format (*.las).

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
  
    28 February 2012 -- created back in DE just before meeting Marcus in FRA
  
===============================================================================
*/
#ifndef PULSE_READER_LAS_HPP
#define PULSE_READER_LAS_HPP

#include "pulsereader.hpp"

#include <stdio.h>

#ifdef LZ_WIN32_VC6
#include <fstream.h>
#else
#include <istream>
#include <fstream>
using namespace std;
#endif

class LASreader;
class LASwaveform13reader;

class PULSEreaderLAS : public PULSEreader
{
public:

  BOOL open(const char* file_name, U32 io_buffer_size=65536);

  I32 get_format() const;

  BOOL seek(const I64 p_index);
  BOOL read_waves();

  ByteStreamIn* get_pulse_stream() const;
  ByteStreamIn* get_waves_stream() const;
  void close(BOOL close_streams=TRUE);

  PULSEreaderLAS();
  virtual ~PULSEreaderLAS();

protected:
  virtual BOOL open();
  virtual BOOL read_pulse_default();

private:
  LASreader* lasreader;
  LASwaveform13reader* laswaveform13reader;
  U8 descriptor_map[256];
  WAVESwaves* waves_map[256];
  F64 last_gps_time;
  U32* seek_map;
};

#endif
