/*
===============================================================================

  FILE:  pulsereader_lgw.hpp
  
  CONTENTS:
  
    Reads LiDAR Pulses and Waves from NASA's LVIS format (*.lgw).

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
#ifndef PULSE_READER_LGW_HPP
#define PULSE_READER_LGW_HPP

#include "pulsereader.hpp"

#include <stdio.h>

class PULSEpulseLGW
{
public:
  U32 lfid;
  U32 shotnumber;
  F32 azimuth;
  F32 incidentangle;
  F32 range;
  F64 utc_time;
  F64 lon0;
  F64 lat0;
  F32 z0;
  F64 lon431;
  F64 lat431;
  F32 z431;
  F32 sigmean;
  U8 txwave[80];
  U8 rxwave[432];

  PULSEpulseLGW() { memset(this, 0, sizeof(this)); };
};

class PULSEreaderLGW : public PULSEreader
{
public:

  BOOL open(const char* file_name, U32 io_buffer_size=65536);

  I32 get_format() const;

  BOOL seek(const I64 p_index);
  BOOL read_waves();

  ByteStreamIn* get_pulse_stream() const;
  ByteStreamIn* get_waves_stream() const;
  void close(BOOL close_streams=TRUE);

  PULSEreaderLGW();
  virtual ~PULSEreaderLGW();

protected:
  virtual BOOL open();
  virtual BOOL read_pulse_default();

private:
  FILE* file;
  ByteStreamIn* stream;
  I32 version;
  I32 size;
  PULSEpulseLGW pulselgw;
  WAVESwaves waveslgw;
  BOOL (PULSEreaderLGW::*load_pulse)();
  BOOL load_pulse_v1_03();
  BOOL load_pulse_v1_02();
  BOOL load_pulse_v1_01();
  BOOL load_pulse_v1_00();
};

#endif
