/*
===============================================================================

  FILE:  pulsereader_csd.hpp
  
  CONTENTS:
  
    Reads LiDAR Pulses and Waves from the Optech's Corrected Sensor Data (CSD)
    format (*.csd).

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
  
    25 July 2012 -- created enroute Woerth See after 10:04 train was cancelled
  
===============================================================================
*/
#ifndef PULSE_READER_CSD_HPP
#define PULSE_READER_CSD_HPP

#include "pulsereader.hpp"

#include <stdio.h>

class PULSEpulseCSD
{
public:
  F64 GPS_time;
  U8 pulse_count;
  F32 ranges[4];         // [meters] (1st, 2nd, 3rd, last
  I16 intensities[4];    // (1st, 2nd, 3rd, last
  F32 scan_angle;        // [radians]
  F32 roll;              // [radians]
  F32 pitch;             // [radians]
  F32 heading;           // [radians]
  F64 latitude;          // [radians]
  F64 longitude;         // [radians]
  F64 latitude_degrees;  // [radians]
  F64 longitude_degrees; // [radians]
  F32 elevation;         // [meters]

  F64 origin[3];

  F64 Re_l[3][3];
  F64 Rl_b[3][3];

  BOOL get_pos(F64 pos[3], I32 range_index, const F64 Rb_SOCS[3][3]) const;
  BOOL get_dir(F64 dir[3], const F64 Rb_SOCS[3][3]) const;

  PULSEpulseCSD() { memset(this, 0, sizeof(this)); };
};

class PULSEreaderCSD : public PULSEreader
{
public:

  BOOL open(const char* file_name, U32 io_buffer_size=65536);

  I32 get_format() const;

  BOOL seek(const I64 p_index);
  BOOL read_waves();

  ByteStreamIn* get_pulse_stream() const;
  ByteStreamIn* get_waves_stream() const;
  void close(BOOL close_streams=TRUE);

  PULSEreaderCSD();
  virtual ~PULSEreaderCSD();

protected:
  virtual BOOL open();
  virtual BOOL read_pulse_default();

private:
  I16 gps_week;
  F64 min_time;
  F64 max_time;
  I32 number_of_records;
  I16 number_of_strips;
  I32 strip_pointers[256];
  F64 IMU_offsets[3];
  F64 misalignment_angles[3];
  F64 Rb_SOCS[3][3];
  F64 rotMatrixMm[3][3];
  F64 temperature;
  F64 pressure;
  F64 range_calculation_factor;
  PULSEpulseCSD pulsecsd;
  U32 idx_number_of_frames_in_the_file;
  F64* idx_frame_start_times;
  F64* idx_frame_end_times;
  I64* idx_ndf_file_offsets;
  F64 idx_frames_start;
  F64 idx_frame_duration;
  I32 idx_time_increment;
  I32 last_frame;
  I32 last_line;
  U16 offset_to_next_line;
  U8 line_data[65536];
  WAVESwaves waves0seg;
  WAVESwaves waves1seg;
  WAVESwaves waves2seg;
  WAVESwaves waves3seg;
  CHAR* file_name;
  FILE* pulse_file;
  FILE* waves_file;
  ByteStreamIn* pulse_stream;
  ByteStreamIn* waves_stream;
  BOOL read_pulse_csd();
  BOOL read_waves_lwf();
  BOOL open_waves();
};

#endif
