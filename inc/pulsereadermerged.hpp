/*
===============================================================================

  FILE:  pulsereadermerged.hpp
  
  CONTENTS:
  
    Reads PulseWaves full waveform LiDAR from more than one file and presents
    their contents on-the-fly as one larger PulseWaves file.

  PROGRAMMERS:

    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com

  COPYRIGHT:

    (c) 2007-2013, martin isenburg, rapidlasso - tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
     1 February 2013 -- created at Ali Mall after a late Pork Adobo Breakfast
  
===============================================================================
*/
#ifndef PULSE_READER_MERGED_HPP
#define PULSE_READER_MERGED_HPP

#include "pulsereader_pls.hpp"
#ifndef PULSEWAVES_DLL
#ifdef _WIN32
#include "pulsereader_las.hpp"
#endif // _WIN32
#include "pulsereader_gcw.hpp"
#include "pulsereader_lgw.hpp"
#endif

class PULSEreaderMerged : public PULSEreader
{
public:

  BOOL add_file_name(const char* file_name);
  void set_scale_factor(const F64* scale_factor);
  void set_offset(const F64* offset);
  void set_files_are_flightlines(BOOL files_are_flightlines);
  BOOL open();
  BOOL reopen();

  void set_filter(PULSEfilter* filter);
  void set_transform(PULSEtransform* transform);

  BOOL inside_tile(const F32 ll_x, const F32 ll_y, const F32 size);
  BOOL inside_circle(const F64 center_x, const F64 center_y, const F64 radius);
  BOOL inside_rectangle(const F64 min_x, const F64 min_y, const F64 max_x, const F64 max_y);

  I32 get_format() const;

  BOOL seek(const I64 p_index){ return FALSE; };
  BOOL read_waves();

  ByteStreamIn* get_pulse_stream() const { return 0; };
  ByteStreamIn* get_waves_stream() const { return 0; };
  void close(BOOL close_stream=TRUE);

  PULSEreaderMerged();
  ~PULSEreaderMerged();

protected:
  BOOL read_pulse_default();

private:
  BOOL open_next_file();
  void clean();

  PULSEreader* pulsereader;
  PULSEreaderPLS* pulsereaderpls;
#ifndef PULSEWAVES_DLL
#ifdef _WIN32
  PULSEreaderLAS* pulsereaderlas;
#endif // _WIN32
  PULSEreaderGCW* pulsereadergcw;
  PULSEreaderLGW* pulsereaderlgw;
#endif
  BOOL pulse_format_change;
  BOOL pulse_attribute_change;
  BOOL pulse_size_change;
  BOOL rescale;
  BOOL reoffset;
  F64* scale_factor;
  F64* offset;
  BOOL files_are_flightlines;
  U32 file_name_current;
  U32 file_name_number;
  U32 file_name_allocated;
  char** file_names;
  F64* bounding_boxes;
  U32 inside; // 0 = none, 1 = tile, 2 = circle, 3 = rectangle
};

#endif
