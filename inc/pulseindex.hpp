/*
===============================================================================

  FILE:  pulseindex.hpp
  
  CONTENTS:
  
    This class can create a spatial indexing, store a spatial indexing, write
    a spatial indexing to file, read a spatial indexing from file, and - most
    importantly - it can be used together with a pulsereader for efficient access
    to a particular spatial region.

    NOT FULLY IMPLEMENTED

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
#ifndef PULSE_INDEX_HPP
#define PULSE_INDEX_HPP

#include "pulsepulse.hpp"

class PULSEquadtree;
class PULSEinterval;
class PULSEreader;
class ByteStreamIn;
class ByteStreamOut;

class PULSEindex
{
public:
  PULSEindex() {};
  ~PULSEindex() {};

/*

  // create spatial index
  void prepare(PULSEquadtree* quadtree, I32 threshold=1000);
  BOOL add(const PULSEpulse* pulse, const U32 index);
  void complete(U32 minimum_pulses=100000, I32 maximum_intervals=-1);
*/

  // read from file or write to file
  BOOL read(const char* file_name) { return FALSE; };
/*
  BOOL write(const char* file_name) const;
  BOOL read(ByteStreamIn* stream);
  BOOL write(ByteStreamOut* stream) const;

*/
  // intersect
  BOOL intersect_rectangle(const F64 r_min_x, const F64 r_min_y, const F64 r_max_x, const F64 r_max_y) { return FALSE; };
  BOOL intersect_tile(const F32 ll_x, const F32 ll_y, const F32 size) { return FALSE; };
  BOOL intersect_circle(const F64 center_x, const F64 center_y, const F64 radius) { return FALSE; };

/*
  // access the intersected intervals
  BOOL get_intervals();
  BOOL has_intervals();

  U32 start;
  U32 end;
  U32 full;
  U32 total;
  U32 cells;
*/

  // read or seek next interval pulse
  BOOL read_next(PULSEreader* lasreader) { return FALSE; };
  BOOL seek_next(PULSEreader* lasreader) { return FALSE; };

/*
  // for debugging
  void print(BOOL verbose);

  // for visualization
  PULSEquadtree* get_quadtree() const;
  PULSEinterval* get_interval() const;

private:
  BOOL merge_intervals();

  PULSEquadtree* quadtree;
  PULSEinterval* interval;
  BOOL have_interval;
*/
};

#endif
