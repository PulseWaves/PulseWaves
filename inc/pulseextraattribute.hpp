/*
===============================================================================

  FILE:  pulseextraattribute.hpp
  
  CONTENTS:
  
    Describes user-defineable "extra attributes" added to a pulse record.

  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2007-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    16 February 2012 -- created in the cafe where it smelled like 1917 Chestnut
  
===============================================================================
*/
#ifndef PULSE_EXTRA_ATTRIBUTE_HPP
#define PULSE_EXTRA_ATTRIBUTE_HPP

#include <string.h>
#include <stdlib.h>

#include "mydefs.hpp"

#include "pulseitem.hpp"

#define PULSE_EXTRA_ATTRIBUTE_U8  0
#define PULSE_EXTRA_ATTRIBUTE_I8  1
#define PULSE_EXTRA_ATTRIBUTE_U16 2
#define PULSE_EXTRA_ATTRIBUTE_I16 3
#define PULSE_EXTRA_ATTRIBUTE_U32 4
#define PULSE_EXTRA_ATTRIBUTE_I32 5
#define PULSE_EXTRA_ATTRIBUTE_U64 6
#define PULSE_EXTRA_ATTRIBUTE_I64 7
#define PULSE_EXTRA_ATTRIBUTE_F32 8
#define PULSE_EXTRA_ATTRIBUTE_F64 9

class PULSEattribute
{
public:
  U8 reserved[2];           // 2 bytes
  U8 data_type;             // 1 byte
  U8 options;               // 1 byte
  CHAR name[32];            // 32 bytes
  U32 offset_to_first_byte; // 4 bytes
  U64I64F64 no_data[3];     // 24 = 3*8 bytes
  U64I64F64 min[3];         // 24 = 3*8 bytes
  U64I64F64 max[3];         // 24 = 3*8 bytes
  F64 scale[3];             // 24 = 3*8 bytes
  F64 offset[3];            // 24 = 3*8 bytes
  CHAR description[64];     // 64 bytes

  PULSEattribute(U8 size)
  {
    if (size == 0) throw;
    memset(this, 0, sizeof(PULSEattribute));
    scale[0] = scale[1] = scale[2] = 1.0;
    this->options = size;
  };

  PULSEattribute(U32 type, const char* name, const char* description=0, U32 dim=1)
  {
    if (type > PULSE_EXTRA_ATTRIBUTE_F64) throw;
    if ((dim < 1) || (dim > 3)) throw;
    if (name == 0) throw;
    memset(this, 0, sizeof(PULSEattribute));
    scale[0] = scale[1] = scale[2] = 1.0;
    this->data_type = type*dim+1;
    strncpy(this->name, name, 32);
    if (description) strncpy(this->description, description, 64);
  }

  inline BOOL set_no_data(U8 no_data, I32 dim=0) { if ((0 == get_type()) && (dim < get_dim())) { this->no_data[dim].u64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(I8 no_data, I32 dim=0) { if ((1 == get_type()) && (dim < get_dim())) { this->no_data[dim].i64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(U16 no_data, I32 dim=0) { if ((2 == get_type()) && (dim < get_dim())) { this->no_data[dim].u64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(I16 no_data, I32 dim=0) { if ((3 == get_type()) && (dim < get_dim())) { this->no_data[dim].i64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(U32 no_data, I32 dim=0) { if ((4 == get_type()) && (dim < get_dim())) { this->no_data[dim].u64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(I32 no_data, I32 dim=0) { if ((5 == get_type()) && (dim < get_dim())) { this->no_data[dim].i64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(U64 no_data, I32 dim=0) { if ((6 == get_type()) && (dim < get_dim())) { this->no_data[dim].u64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(I64 no_data, I32 dim=0) { if ((7 == get_type()) && (dim < get_dim())) { this->no_data[dim].i64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(F32 no_data, I32 dim=0) { if ((8 == get_type()) && (dim < get_dim())) { this->no_data[dim].f64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(F64 no_data, I32 dim=0) { if ((9 == get_type()) && (dim < get_dim())) { this->no_data[dim].f64 = no_data; options |= 0x01; return TRUE; } return FALSE; }

  inline void set_min(U8* min, I32 dim=0) { this->min[dim] = cast(min); options |= 0x02; }
  inline void update_min(U8* min, I32 dim=0) { this->min[dim] = smallest(cast(min), this->min[dim]); }
  inline BOOL set_min(U8 min, I32 dim=0) { if ((0 == get_type()) && (dim < get_dim())) { this->min[dim].u64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(I8 min, I32 dim=0) { if ((1 == get_type()) && (dim < get_dim())) { this->min[dim].i64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(U16 min, I32 dim=0) { if ((2 == get_type()) && (dim < get_dim())) { this->min[dim].u64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(I16 min, I32 dim=0) { if ((3 == get_type()) && (dim < get_dim())) { this->min[dim].i64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(U32 min, I32 dim=0) { if ((4 == get_type()) && (dim < get_dim())) { this->min[dim].u64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(I32 min, I32 dim=0) { if ((5 == get_type()) && (dim < get_dim())) { this->min[dim].i64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(U64 min, I32 dim=0) { if ((6 == get_type()) && (dim < get_dim())) { this->min[dim].u64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(I64 min, I32 dim=0) { if ((7 == get_type()) && (dim < get_dim())) { this->min[dim].i64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(F32 min, I32 dim=0) { if ((8 == get_type()) && (dim < get_dim())) { this->min[dim].f64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(F64 min, I32 dim=0) { if ((9 == get_type()) && (dim < get_dim())) { this->min[dim].f64 = min; options |= 0x02; return TRUE; } return FALSE; }

  inline void set_max(U8* max, I32 dim=0) { this->max[dim] = cast(max); options |= 0x04; }
  inline void update_max(U8* max, I32 dim=0) { this->max[dim] = biggest(cast(max), this->max[dim]); }
  inline BOOL set_max(U8 max, I32 dim=0) { if ((0 == get_type()) && (dim < get_dim())) { this->max[dim].u64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(I8 max, I32 dim=0) { if ((1 == get_type()) && (dim < get_dim())) { this->max[dim].i64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(U16 max, I32 dim=0) { if ((2 == get_type()) && (dim < get_dim())) { this->max[dim].u64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(I16 max, I32 dim=0) { if ((3 == get_type()) && (dim < get_dim())) { this->max[dim].i64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(U32 max, I32 dim=0) { if ((4 == get_type()) && (dim < get_dim())) { this->max[dim].u64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(I32 max, I32 dim=0) { if ((5 == get_type()) && (dim < get_dim())) { this->max[dim].i64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(U64 max, I32 dim=0) { if ((6 == get_type()) && (dim < get_dim())) { this->max[dim].u64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(I64 max, I32 dim=0) { if ((7 == get_type()) && (dim < get_dim())) { this->max[dim].i64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(F32 max, I32 dim=0) { if ((8 == get_type()) && (dim < get_dim())) { this->max[dim].f64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(F64 max, I32 dim=0) { if ((9 == get_type()) && (dim < get_dim())) { this->max[dim].f64 = max; options |= 0x04; return TRUE; } return FALSE; }

  inline BOOL set_scale(F64 scale, I32 dim=0) { if (data_type) { this->scale[dim] = scale; options |= 0x08; return TRUE; } return FALSE; }
  inline BOOL set_offset(F64 offset, I32 dim=0) { if (data_type) { this->offset[dim] = offset; options |= 0x10; return TRUE; } return FALSE; }

  inline BOOL has_no_data() const { return options & 0x01; }
  inline BOOL has_min() const { return options & 0x02; }
  inline BOOL has_max() const { return options & 0x04; }
  inline BOOL has_scale() const { return options & 0x08; }
  inline BOOL has_offset() const { return options & 0x10; }

  inline U32 get_size() const
  {
    if (data_type)
    {
      const U32 size_table[10] = { 1, 1, 2, 2, 4, 4, 8, 8, 4, 8 };
      U32 type = get_type();
      U32 dim = get_dim();
      return size_table[type]*dim;
    }
    else
    {
      return options;
    }
  }

private:
  inline I32 get_type() const
  {
    return ((I32)data_type - 1)%10;
  }
  inline I32 get_dim() const
  {
    return 1 + ((I32)data_type - 1)/10;
  }
  inline U64I64F64 cast(U8* value) const
  {
    I32 type = get_type();
    U64I64F64 casted_value;
    if (type == 0)
      casted_value.u64 = *((U8*)value);
    else if (type == 1)
      casted_value.i64 = *((I8*)value);
    else if (type == 2)
      casted_value.u64 = *((U16*)value);
    else if (type == 3)
      casted_value.i64 = *((I16*)value);
    else if (type == 4)
      casted_value.u64 = *((U32*)value);
    else if (type == 5)
      casted_value.i64 = *((I32*)value);
    else if (type == 6)
      casted_value.u64 = *((U64*)value);
    else if (type == 7)
      casted_value.i64 = *((I64*)value);
    else if (type == 8)
      casted_value.f64 = *((F32*)value);
    else
      casted_value.f64 = *((F64*)value);
    return casted_value;
  }
  inline U64I64F64 smallest(U64I64F64 a, U64I64F64 b) const
  {
    I32 type = get_type();
    if (type >= 8) // float compare
    {
      if (a.f64 < b.f64) return a;
      else               return b;
    }
    if (type & 1) // int compare
    {
      if (a.i64 < b.i64) return a;
      else               return b;
    }
    if (a.u64 < b.u64) return a;
    else               return b;
  }
  inline U64I64F64 biggest(U64I64F64 a, U64I64F64 b) const
  {
    I32 type = get_type();
    if (type >= 8) // float compare
    {
      if (a.f64 > b.f64) return a;
      else               return b;
    }
    if (type & 1) // int compare
    {
      if (a.i64 > b.i64) return a;
      else               return b;
    }
    if (a.u64 > b.u64) return a;
    else               return b;
  }
};

#endif
