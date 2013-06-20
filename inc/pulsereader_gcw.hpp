/*
===============================================================================

  FILE:  pulsereader_gcw.hpp
  
  CONTENTS:
  
    Reads LiDAR Pulses and Waves from the Geocoded Calibrated Waveform format
    by GeoLas Consulting (*.lgc + *.lwf).

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
  
    11 June 2012 -- created back from the ESA BIC interview in Oberpfaffenhofen
  
===============================================================================
*/
#ifndef PULSE_READER_GCW_HPP
#define PULSE_READER_GCW_HPP

#include "pulsereader.hpp"

#include <stdio.h>

class PULSEpulseLGC
{
public:
  I32 wfi;       // WFI __i64 8 bytes Offset of first sample of start pulse waveform in LWF file, in bytes from beginning of file
  F64 t;         // T DOUBLE 8 bytes GPS time tag of laser shot [GPS seconds of the week]
  F64 e0;        // E0 DOUBLE 8 bytes Easting of first sample of start pulse waveform [m]
  F64 n0;        // N0 DOUBLE 8 bytes Northing of first sample of start pulse waveform [m]
  F32 h0;        // H0 FLOAT 4 bytes Ellipsoidal height of first sample of start waveform [m]
  F32 de;        // dE FLOAT 4 bytes Easting component of measurement ray (waveform vector) in units of waveform samples [bins] (1 ns ~ 0.15 m)
  F32 dn;        // dN FLOAT 4 bytes Northing component of measurement ray [m]
  F32 dh;        // dH FLOAT 4 bytes Elevation component of measurement ray [m] (negative, measurement vector points towards the Earth’s surface)
  U16 wfoffset;  // WFOFFSET USHORT 2 bytes Offset of first sample of surface return waveform in bins, measured from the first sample of the start pulse waveform
  U16 wflen;     // WFLEN USHORT 2 bytes Number of samples in surface return waveform
  U16 strtwflen; // STRTWFLEN USHORT 2 bytes Number of samples in start pulse waveform
  U8 sampdepth;  // SAMPDEPTH BYTE 1 byte 0 if waveform samples are 1 byte/sample, 1 if waveform samples are 2 bytes/sample
  U8 res;        // RES BYTE 1 byte reserved (always 0)
  PULSEpulseLGC() { memset(this, 0, sizeof(this)); };
};

class PULSEreaderGCW : public PULSEreader
{
public:

  BOOL open(const char* file_name, U32 io_buffer_size=65536);

  I32 get_format() const;

  BOOL seek(const I64 p_index);
  BOOL read_waves();

  ByteStreamIn* get_pulse_stream() const;
  ByteStreamIn* get_waves_stream() const;
  void close(BOOL close_streams=TRUE);

  PULSEreaderGCW();
  virtual ~PULSEreaderGCW();

protected:
  virtual BOOL open();
  virtual BOOL read_pulse_default();

private:
  PULSEpulseLGC pulselgc;
  WAVESwaves waves8bit;
  WAVESwaves waves16bit;
  CHAR* file_name;
  FILE* pulse_file;
  FILE* waves_file;
  ByteStreamIn* pulse_stream;
  ByteStreamIn* waves_stream;
  BOOL read_pulse_lgc();
  BOOL read_waves_lwf();
  BOOL open_waves();
};

#endif
