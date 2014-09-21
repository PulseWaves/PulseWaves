/*
===============================================================================

  FILE:  pulsepulse.hpp
  
  CONTENTS:
  
    Describes an abtract interface to the attributes of a PulseWaves pulse.

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
  
    07 March 2012 -- created while watching Bayer loose 7:1 against Barcelona
  
===============================================================================
*/
#ifndef PULSE_PULSE_HPP
#define PULSE_PULSE_HPP

#include "pulseheader.hpp"
#include "pulseattributer.hpp"

#include <stdio.h>

class PULSEpulse
{
public:

  // these fields contain the data that describe each pulse

  I64 T;
  I64 offset;
  I32 anchor_X;
  I32 anchor_Y;
  I32 anchor_Z;
  I32 target_X;
  I32 target_Y;
  I32 target_Z;
  I16 first_returning_sample;
  I16 last_returning_sample;
  U16 descriptor_index : 8;
  U16 reserved : 4;
  U16 edge_of_scan_line : 1;
  U16 scan_direction : 1;
  U16 mirror_facet : 2;
  U8 intensity;
  U8 classification;

  // optional attributes

  U32 pulse_source_ID;

  BOOL has_pulse_source_ID;

  U8* extra_bytes;
  I32 extra_bytes_number;

  // pulse size (including optional attributes and extra bytes)

  U32 total_pulse_size;

  // for converting between x,y,z integers and scaled/translated coordinates

  const PULSEquantizer* quantizer;
  F64 anchor[3];
  F64 target[3];
  F32 dir[3];
  F64 first[3];
  F64 last[3];

  // for attributed access to the extra bytes

  const PULSEattributer* attributer;

  // this field provides generic access to the pulse

  U8** pulse;

  // these fields describe the pulse in terms of generic items

  U16 num_items;
  PULSEitem* items;

  // copy functions

  PULSEpulse & operator=(const PULSEpulse & other);
  void copy_to(U8* buffer) const;
  void copy_from(const U8* buffer);

  // convenient output to text for inspection

  BOOL save_to_txt(FILE* file) const;

  // these functions set the desired pulse format (and maybe add on extra attributes)

  BOOL setup(U16* num_items, PULSEitem** items, const U32 pulse_format, const U32 pulse_attributes, const U32 pulse_size, const U32 compressor=0);
  BOOL init(const PULSEheader* header, const PULSEattributer* attributer=0);
  BOOL init(const PULSEquantizer* quantizer, const U32 num_items, const PULSEitem* items, const PULSEattributer* attributer=0);

  BOOL inside_rectangle(const F64 r_min_x, const F64 r_min_y, const F64 r_max_x, const F64 r_max_y) const;
  BOOL inside_tile(const F32 ll_x, const F32 ll_y, const F32 ur_x, const F32 ur_y) const;
  BOOL inside_circle(const F64 center_x, const F64 center_y, F64 squared_radius) const;
  BOOL inside_box(const F64 min_x, const F64 min_y, const F64 min_z, const F64 max_x, const F64 max_y, const F64 max_z) const;

  void zero();
  void clean();

  // access functions

  inline void set_T(const F64 t) { T = quantizer->get_T(t); };
  inline void set_T(const I64 T) { this->T = T; };

  inline I64 get_T() const { return T; };
  inline I64 get_T_msec() const { return quantizer->get_T_msec(T); };
  inline I64 get_T_usec() const { return quantizer->get_T_usec(T); };
  inline I64 get_T_nsec() const { return quantizer->get_T_nsec(T); };
  inline I64 get_T_psec() const { return quantizer->get_T_psec(T); };

  inline I64 get_t() const { return T; };
  inline F64 get_t_msec() const { return quantizer->get_t_msec(T); };
  inline F64 get_t_usec() const { return quantizer->get_t_usec(T); };
  inline F64 get_t_nsec() const { return quantizer->get_t_nsec(T); };
  inline F64 get_t_psec() const { return quantizer->get_t_psec(T); };

  inline void set_anchor_x(F64 anchor_x)
  {
    anchor_X = quantizer->get_X(anchor_x);
  }
  inline void set_anchor_y(F64 anchor_y)
  {
    anchor_Y = quantizer->get_Y(anchor_y);
  }
  inline void set_anchor_z(F64 anchor_z)
  {
    anchor_Z = quantizer->get_Z(anchor_z);
  }
  inline void set_target_x(F64 target_x)
  {
    target_X = quantizer->get_X(target_x);
  }
  inline void set_target_y(F64 target_y)
  {
    target_Y = quantizer->get_Y(target_y);
  }
  inline void set_target_z(F64 target_z)
  {
    target_Z = quantizer->get_Z(target_z);
  }

  inline void set_anchor_and_target(const F64* anchor, const F64* target)
  {
    set_anchor_x(anchor[0]);
    set_anchor_y(anchor[1]);
    set_anchor_z(anchor[2]);
    set_target_x(target[0]);
    set_target_y(target[1]);
    set_target_z(target[2]);
    compute_anchor_and_target_and_dir();
  };

  inline F64 compute_and_get_anchor_x() const { return quantizer->get_x(anchor_X); };
  inline F64 compute_and_get_anchor_y() const { return quantizer->get_y(anchor_Y); };
  inline F64 compute_and_get_anchor_z() const { return quantizer->get_z(anchor_Z); };

  inline F64 compute_and_get_target_x() const { return quantizer->get_x(target_X); };
  inline F64 compute_and_get_target_y() const { return quantizer->get_y(target_Y); };
  inline F64 compute_and_get_target_z() const { return quantizer->get_z(target_Z); };

  inline void compute_anchor()
  {
    anchor[0] = compute_and_get_anchor_x();
    anchor[1] = compute_and_get_anchor_y();
    anchor[2] = compute_and_get_anchor_z();
  }

  inline void compute_target()
  {
    target[0] = compute_and_get_target_x();
    target[1] = compute_and_get_target_y();
    target[2] = compute_and_get_target_z();
  }

  inline void compute_anchor_and_target()
  {
    compute_anchor();
    compute_target();
  };

  inline void compute_anchor_and_target_and_dir()
  {
    compute_anchor_and_target();
    dir[0] = (F32)((target[0]-anchor[0])/1000);
    dir[1] = (F32)((target[1]-anchor[1])/1000);
    dir[2] = (F32)((target[2]-anchor[2])/1000);
  }

  inline F64 get_anchor_x() const { return anchor[0]; };
  inline F64 get_anchor_y() const { return anchor[1]; };
  inline F64 get_anchor_z() const { return anchor[2]; };

  inline F64 get_target_x() const { return target[0]; };
  inline F64 get_target_y() const { return target[1]; };
  inline F64 get_target_z() const { return target[2]; };

  inline F32 get_dir_x() const { return dir[0]; };
  inline F32 get_dir_y() const { return dir[1]; };
  inline F32 get_dir_z() const { return dir[2]; };

  inline void get_anchor(F64* anchor) const
  {
    anchor[0] = get_anchor_x();
    anchor[1] = get_anchor_y();
    anchor[2] = get_anchor_z();
  };

  inline void get_target(F64* target) const
  {
    target[0] = get_target_x();
    target[1] = get_target_y();
    target[2] = get_target_z();
  };

  inline F64 compute_and_get_first_x() const { return anchor[0] + (dir[0]*first_returning_sample); };
  inline F64 compute_and_get_first_y() const { return anchor[1] + (dir[1]*first_returning_sample); };
  inline F64 compute_and_get_first_z() const { return anchor[2] + (dir[2]*first_returning_sample); };

  inline F64 compute_and_get_last_x() const { return anchor[0] + (dir[0]*last_returning_sample); };
  inline F64 compute_and_get_last_y() const { return anchor[1] + (dir[1]*last_returning_sample); };
  inline F64 compute_and_get_last_z() const { return anchor[2] + (dir[2]*last_returning_sample); };

  inline void compute_first()
  {
    first[0] = compute_and_get_first_x();
    first[1] = compute_and_get_first_y();
    first[2] = compute_and_get_first_z();
  };

  inline void compute_last()
  {
    last[0] = compute_and_get_last_x();
    last[1] = compute_and_get_last_y();
    last[2] = compute_and_get_last_z();
  };

  inline void compute_first_and_last()
  {
    compute_first();
    compute_last();
  };

  inline F64 get_first_x() const { return first[0]; };
  inline F64 get_first_y() const { return first[1]; };
  inline F64 get_first_z() const { return first[2]; };

  inline F64 get_last_x() const { return last[0]; };
  inline F64 get_last_y() const { return last[1]; };
  inline F64 get_last_z() const { return last[2]; };


  // generic extra attribute functions

  inline void get_extra_attribute(I32 index, U8* data) const
  {
    memcpy(data, extra_bytes + attributer->extra_attribute_array_offsets[index], attributer->extra_attribute_sizes[index]);
  }

  inline void set_extra_attribute(I32 index, const U8* data) 
  {
    memcpy(extra_bytes + attributer->extra_attribute_array_offsets[index], data, attributer->extra_attribute_sizes[index]);
  }

  // typed and offset attribute functions (more efficient)

  inline void get_extra_attribute(I32 offset, U8 &data) const { data = extra_bytes[offset]; }
  inline void set_extra_attribute(I32 offset, U8 data) { extra_bytes[offset] = data; }
  inline void get_extra_attribute(I32 offset, I8 &data) const { data = (I8)(extra_bytes[offset]); }
  inline void set_extra_attribute(I32 offset, I8 data) { extra_bytes[offset] = data; }
  inline void get_extra_attribute(I32 offset, U16 &data) const { data = *((U16*)(extra_bytes + offset)); }
  inline void set_extra_attribute(I32 offset, U16 data) { *((U16*)(extra_bytes + offset)) = data; }
  inline void get_extra_attribute(I32 offset, I16 &data) const { data = *((I16*)(extra_bytes + offset)); }
  inline void set_extra_attribute(I32 offset, I16 data) { *((I16*)(extra_bytes + offset)) = data; }
  inline void get_extra_attribute(I32 offset, U32 &data) const { data = *((U32*)(extra_bytes + offset)); }
  inline void set_extra_attribute(I32 offset, U32 data) { *((U32*)(extra_bytes + offset)) = data; }
  inline void get_extra_attribute(I32 offset, I32 &data) const { data = *((I32*)(extra_bytes + offset)); }
  inline void set_extra_attribute(I32 offset, I32 data) { *((I32*)(extra_bytes + offset)) = data; }
  inline void get_extra_attribute(I32 offset, U64 &data) const { data = *((U64*)(extra_bytes + offset)); }
  inline void set_extra_attribute(I32 offset, U64 data) { *((U64*)(extra_bytes + offset)) = data; }
  inline void get_extra_attribute(I32 offset, I64 &data) const { data = *((I64*)(extra_bytes + offset)); }
  inline void set_extra_attribute(I32 offset, I64 data) { *((I64*)(extra_bytes + offset)) = data; }
  inline void get_extra_attribute(I32 offset, F32 &data) const { data = *((F32*)(extra_bytes + offset)); }
  inline void set_extra_attribute(I32 offset, F32 data) { *((F32*)(extra_bytes + offset)) = data; }
  inline void get_extra_attribute(I32 offset, F64 &data) const { data = *((F64*)(extra_bytes + offset)); }
  inline void set_extra_attribute(I32 offset, F64 data) { *((F64*)(extra_bytes + offset)) = data; }

  PULSEpulse();
  PULSEpulse(const PULSEpulse & other);
  ~PULSEpulse();
};

#endif
