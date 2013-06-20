/*
===============================================================================

  FILE:  pulseutility.hpp
  
  CONTENTS:
  
    Simple utilities that come in handy when using the PulseWaves API.

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
  
    05 March 2012 -- created after eating a Hamburgesa Americana for 1.90 EUR
  
===============================================================================
*/
#ifndef PULSE_UTILITY_HPP
#define PULSE_UTILITY_HPP

#include "pulsepulse.hpp"

class PULSEinventory
{
public:
  BOOL active() const { return (first == FALSE); }; 
  I64 number_of_pulses;
  I64 min_T;
  I64 max_T;
  F64 min_x;
  F64 max_x;
  F64 min_y;
  F64 max_y;
  F64 min_z;
  F64 max_z;
  BOOL add(const PULSEpulse* pulse, BOOL only_count_pulses=FALSE);
  PULSEinventory();
private:
  BOOL first;
  BOOL first_returning;
};

class PULSEsummary
{
public:
  BOOL active() const { return (first == FALSE); }; 
  I64 number_of_pulses;
  PULSEpulse min;
  PULSEpulse max;
  F64 min_x;
  F64 max_x;
  F64 min_y;
  F64 max_y;
  F64 min_z;
  F64 max_z;
  BOOL add(const PULSEpulse* pulse);
  PULSEsummary();
private:
  BOOL first;
  BOOL first_returning;
};

class PULSEbin
{
public:
  void add(I32 item);
  void add(I64 item);
  void add(F64 item);
  void add(I32 item, I32 value);
  void report(FILE* file, const char* name=0, const char* name_avg=0) const;
  PULSEbin(F32 step);
  ~PULSEbin();
private:
  void add_to_bin(I32 bin);
  F64 total;
  I64 count;
  F32 one_over_step;
  BOOL first;
  I32 anker;
  I32 size_pos;
  I32 size_neg;
  U32* bins_pos;
  U32* bins_neg;
  F64* values_pos;
  F64* values_neg;
};

class PULSEhistogram
{
public:
  BOOL active() const { return is_active; }; 
  BOOL parse(int argc, char* argv[]);
  BOOL histo(const char* name, F32 step);
  BOOL histo_avg(const char* name, F32 step, const char* name_avg);
  void add(const PULSEpulse* pulse);
  void report(FILE* file) const;
  PULSEhistogram();
  ~PULSEhistogram();
private:
  BOOL is_active;
  // counter bins
  PULSEbin* T_bin;
  PULSEbin* offset_bin;
  PULSEbin* anchor_x_bin;
  PULSEbin* anchor_y_bin;
  PULSEbin* anchor_z_bin;
  PULSEbin* target_x_bin;
  PULSEbin* target_y_bin;
  PULSEbin* target_z_bin;
  PULSEbin* descriptor_bin;
  PULSEbin* intensity_bin;
  PULSEbin* classification_bin;
  PULSEbin* samples_bin;
  PULSEbin* anchor_X_bin;
  PULSEbin* anchor_Y_bin;
  PULSEbin* anchor_Z_bin;
  PULSEbin* target_X_bin;
  PULSEbin* target_Y_bin;
  PULSEbin* target_Z_bin;
  // averages bins
  PULSEbin* nada_bin_nada1;
  PULSEbin* nada_bin_nada2;
};

class PULSEoccupancyGrid
{
public:
  void reset();
  BOOL add(const PULSEpulse* pulse);
  BOOL add(I32 pos_x, I32 pos_y);
  BOOL occupied(const PULSEpulse* pulse) const;
  BOOL occupied(I32 pos_x, I32 pos_y) const;
  BOOL active() const;
  U32 get_num_occupied() const { return num_occupied; };
  BOOL write_asc_grid(const char* file_name) const;

  // read from file or write to file
//  BOOL read(ByteStreamIn* stream);
//  BOOL write(ByteStreamOut* stream) const;

  PULSEoccupancyGrid(F32 grid_spacing);
  ~PULSEoccupancyGrid();
  I32 min_x, min_y, max_x, max_y;
private:
  BOOL add_internal(I32 pos_x, I32 pos_y);
  F32 grid_spacing;
  I32 anker;
  I32* minus_ankers;
  U32 minus_minus_size;
  U32** minus_minus;
  U16* minus_minus_sizes;
  U32 minus_plus_size;
  U32** minus_plus;
  U16* minus_plus_sizes;
  I32* plus_ankers;
  U32 plus_minus_size;
  U32** plus_minus;
  U16* plus_minus_sizes;
  U32 plus_plus_size;
  U32** plus_plus;
  U16* plus_plus_sizes;
  U32 num_occupied;
};

#endif
