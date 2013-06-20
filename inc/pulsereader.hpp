/*
===============================================================================

  FILE:  pulsereader.hpp
  
  CONTENTS:
  
    Abstract interface class to read Pulses (as if) from the PulseWaves format.

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
#ifndef PULSE_READER_HPP
#define PULSE_READER_HPP

#include "pulseheader.hpp"
#include "pulsepulse.hpp"
#include "waveswaves.hpp"

class PULSEindex;
class PULSEfilter;
class PULSEtransform;
class ByteStreamIn;

class PULSEreader
{
public:
  BOOL header_is_populated;

  PULSEheader header;
  PULSEpulse pulse;
  WAVESwaves* waves;

  I64 npulses;
  I64 p_count;

  virtual I32 get_format() const = 0;

  void set_index(PULSEindex* index);
  PULSEindex* get_index() const;
  virtual void set_filter(PULSEfilter* filter);
  virtual void set_transform(PULSEtransform* transform);
  void reset_filter();

  void use_default_reader() { };
//  void use_files_are_flightlines_reader() { read_simple = &PULSEreader::read_pulse_files_are_flightlines; };

  virtual BOOL inside_tile(const F32 ll_x, const F32 ll_y, const F32 size);
  virtual BOOL inside_circle(const F64 center_x, const F64 center_y, const F64 radius);
  virtual BOOL inside_rectangle(const F64 min_x, const F64 min_y, const F64 max_x, const F64 max_y);

  virtual BOOL seek(const I64 p_index) = 0;

  BOOL read_pulse() { return (this->*read_simple)(); };
  virtual BOOL read_waves() = 0;

  inline F64 get_min_x() const { return header.min_x; };
  inline F64 get_min_y() const { return header.min_y; };
  inline F64 get_min_z() const { return header.min_z; };

  inline F64 get_max_x() const { return header.max_x; };
  inline F64 get_max_y() const { return header.max_y; };
  inline F64 get_max_z() const { return header.max_z; };

/*
  inline F64 get_x() const { return pulse.get_x(); };
  inline F64 get_y() const { return pulse.get_y(); };
  inline F64 get_z() const { return pulse.get_z(); };

  inline F64 get_x(const I32 X) const { return header.get_x(X); };
  inline F64 get_y(const I32 Y) const { return header.get_y(Y); };
  inline F64 get_z(const I32 Z) const { return header.get_z(Z); };

  inline I32 get_X(const F64 x) const { return header.get_X(x); };
  inline I32 get_Y(const F64 y) const { return header.get_X(y); };
  inline I32 get_Z(const F64 z) const { return header.get_X(z); };
*/

  virtual ByteStreamIn* get_pulse_stream() const = 0;
  virtual ByteStreamIn* get_waves_stream() const = 0;
  virtual void close(BOOL close_streams=TRUE) = 0;

  PULSEreader();
  virtual ~PULSEreader();

protected:
  virtual BOOL read_pulse_default() = 0;
  virtual BOOL read_files_are_flightlines() { if (read_pulse_default()) { pulse.pulse_source_ID = header.file_source_ID; return TRUE; } return FALSE; };

  PULSEindex* index;
  PULSEfilter* filter;
  PULSEtransform* transform;

  F64 r_min_x, r_min_y, r_max_x, r_max_y;
  F32 t_ll_x, t_ll_y, t_size, t_ur_x, t_ur_y;
  F64 c_center_x, c_center_y, c_radius, c_radius_squared;

private:
  BOOL (PULSEreader::*read_simple)();
  BOOL (PULSEreader::*read_complex)();

  BOOL read_pulse_inside_tile();
  BOOL read_pulse_inside_tile_indexed();
  BOOL read_pulse_inside_circle();
  BOOL read_pulse_inside_circle_indexed();
  BOOL read_pulse_inside_rectangle();
  BOOL read_pulse_inside_rectangle_indexed();
  BOOL read_pulse_filtered();
  BOOL read_pulse_transformed();
  BOOL read_pulse_filtered_and_transformed();
};

class PULSEreadOpener
{
public:
  const CHAR* get_file_name() const;
  void set_files_are_flightlines(const BOOL files_are_flightlines);
  void set_file_name(const CHAR* file_name, BOOL unique=FALSE);
  BOOL add_file_name(const CHAR* file_name, BOOL unique=FALSE);
  void delete_file_name(U32 file_name_id);
  BOOL set_file_name_current(U32 file_name_id);
  U32 get_file_name_number() const;
  const CHAR* get_file_name(U32 number) const;
  I32 get_file_format(U32 number) const;
  void set_merged(const BOOL merged);
  BOOL get_merged() const;
  void set_scale_factor(const F64* scale_factor);
  void set_offset(const F64* offset);
  void add_extra_attribute(I32 data_type, const CHAR* name, const CHAR* description=0, F64 scale=1.0, F64 offset=0.0);
  void set_populate_header(BOOL populate_header);
  const CHAR* get_parse_string() const;
  void usage() const;
  BOOL parse(int argc, char* argv[]);
  BOOL has_populated_header() const;
  BOOL active() const;
  const PULSEfilter* get_filter() const { return filter; };
  const PULSEtransform* get_transform() const { return transform; };
  void reset();
  PULSEreader* open(CHAR* other_file_name=0);
  BOOL reopen(PULSEreader* pulsereader);
  PULSEreadOpener();
  ~PULSEreadOpener();
private:
#ifdef _WIN32
  void add_file_name_windows(const CHAR* file_name, BOOL unique=FALSE);
#endif
  CHAR** file_names;
  CHAR* file_name;
  BOOL merged;
  U32 file_name_number;
  U32 file_name_allocated;
  U32 file_name_current;
  F64* scale_factor;
  F64* offset;
  BOOL files_are_flightlines;
  I32 number_extra_attributes;
  I32 extra_attribute_data_types[10];
  CHAR* extra_attribute_names[10];
  CHAR* extra_attribute_descriptions[10];
  F64 extra_attribute_scales[10];
  F64 extra_attribute_offsets[10];
  BOOL populate_header;

  // optional extras
  PULSEindex* index;
  PULSEfilter* filter;
  PULSEtransform* transform;

  // optional clipping
  F32* inside_tile;
  F64* inside_circle;
  F64* inside_rectangle;
};

#endif
