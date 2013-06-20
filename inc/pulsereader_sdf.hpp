/*
===============================================================================

  FILE:  pulsereader_sdf.hpp
  
  CONTENTS:
  
    Reads LiDAR Pulses and Waves from Riegl's SDF format (*.sdf) via RiWaveLib.

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
  
    04 September 2012 -- added POF trajectory information while at RIEGL
    07 March 2012 -- created after the Orange Tree was pruned to down to nada
  
===============================================================================
*/
#ifndef PULSE_READER_SDF_HPP
#define PULSE_READER_SDF_HPP

#include "pulsereader.hpp"

class PULSEreaderSDF : public PULSEreader
{
public:

  BOOL open(const char* file_name);

  I32 get_format() const;

  BOOL seek(const I64 p_index);
  BOOL read_waves();

  ByteStreamIn* get_pulse_stream() const;
  ByteStreamIn* get_waves_stream() const;
  void close(BOOL close_streams=TRUE);

  PULSEreaderSDF();
  virtual ~PULSEreaderSDF();

protected:
  virtual BOOL open();
  virtual BOOL read_pulse_default();

private:
  PULSEcomposition composition;
  PULSEsampling samplings[5];
  void* fwifc_handle;
  U32 fwifc_error;
  void* sdf_first_sample;
  F64 IMU_offsets[3];
  F64 misalignment_angles[3];
  F64 Rb_SOCS[3][3];
  U32 sbf_sample_count;
  F64 sbf_time_sorg;
  F64 sbf_sampling_time;
  void fwifc_execute(U32 result);
  WAVESwaves* waves_map[256];
#ifdef HAVE_POF_FILE_FOR_SDF_FILE
  BOOL get_origin_and_direction(F64 time_external, F64* origin, F64* direction);
  void* pofifc_handle;
#endif // HAVE_POF_FILE_FOR_SDF_FILE
};

#endif
