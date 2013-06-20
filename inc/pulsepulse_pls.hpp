/*
===============================================================================

  FILE:  pulsepulse_pls.hpp
  
  CONTENTS:
  
    Extends the abtract interface to the attributes of a PulseWaves pulse
    for all pulse types of the *.pls format.

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
  
    20 March 2012 -- created while watching Bayer loose 7:1 against Barcelona
  
===============================================================================
*/
#ifndef PULSE_PULSE_PLS_HPP
#define PULSE_PULSE_PLS_HPP

#include "pulsepulse.hpp"
#include "pulsequantizer.hpp"
#include "pulseattributer.hpp"

#include <stdio.h>

class PULSEpulse_pls : public PULSEpulse
{
public:

  // these fields contain the data that describe each pulse

  U64I64F64 gps_time;
  I64 offset;
  I32 X;
  I32 Y;
  I32 Z;
  F32 dx;
  F32 dy;
  F32 dz;
  U16 descriptor_index : 14;
  U8 scan_direction_flag : 1;
  U8 edge_of_flight_line : 1;
  U16 first_returning_sample;
  U16 last_returning_sample;

  U8* extra_bytes;
  I32 extra_bytes_number;

  U32 total_pulse_size;

  // for converting between x,y,z integers and scaled/translated coordinates

  const PULSEquantizer* quantizer;
  F64 anchor[3];
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

  PULSEpulse(const PULSEpulse & other)
  {
    *this = other;
  }

  PULSEpulse & operator=(const PULSEpulse & other)
  {
    gps_time.u64 = other.gps_time.u64;
    offset = other.offset;
    X = other.X;
    Y = other.Y;
    Z = other.Z;
    dx = other.dx;
    dy = other.dy;
    dz = other.dz;
    descriptor_index = other.descriptor_index;
    scan_direction_flag = other.scan_direction_flag;
    edge_of_flight_line = other.edge_of_flight_line;
    if (other.extra_bytes && extra_bytes)
    {
      memcpy(extra_bytes, other.extra_bytes, extra_bytes_number);
    }
    return *this;
  };

  void copy_to(U8* buffer) const
  {
    U32 i;
    U32 b = 0;
    for (i = 0; i < num_items; i++)
    {
      memcpy(&buffer[b], pulse[i], items[i].size);
      b += items[i].size;
    }
  };

  void copy_from(const U8* buffer)
  {
    U32 i;
    U32 b = 0;
    for (i = 0; i < num_items; i++)
    {
      memcpy(pulse[i], &buffer[b], items[i].size);
      b += items[i].size;
    }
  };

  // these functions set the desired pulse format (and maybe add on extra attributes)

  BOOL setup(U16* num_items, PULSEitem** items, const U32 pulse_format, const I32 pulse_size, const U32 compressor=0)
  {
    I32 extra_bytes_number = 0;

    // switch over the point types we know
    switch (pulse_format)
    {
    case 0:
      extra_bytes_number = pulse_size - PULSEWAVES_PULSE10_SIZE;
      break;
    default:
      fprintf(stderr, "ERROR: pulse format %d unknown", pulse_format);
      return FALSE;
    }

    if (pulse_size < 0)
    {
      extra_bytes_number = 0;
    }
    else if (extra_bytes_number < 0)
    {
      fprintf(stderr, "ERROR: pulse size %d too small for pulse type %d by %d bytes", pulse_size, pulse_format, -extra_bytes_number);
      return FALSE;
    }

    // create item description

    (*num_items) = 1 + !!(extra_bytes_number);
    (*items) = new PULSEitem[*num_items];

    U16 i = 1;
    (*items)[0].type = PULSEitem::PULSE10;
    (*items)[0].size = PULSEWAVES_PULSE10_SIZE;
    (*items)[0].version = 0;
    if (extra_bytes_number)
    {
      (*items)[i].type = PULSEitem::BYTES;
      (*items)[i].size = extra_bytes_number;
      (*items)[i].version = 0;
      i++;
    }
    return TRUE;
  };

  BOOL setup(const U32 pulse_format, const I32 pulse_size, const U16 compressor=0)
  {
    return FALSE;
  };

  BOOL setup(const U16 num_items, const PULSEitem* items, const U16 compressor=0)
  {
    return FALSE;
  };

  BOOL init(const PULSEquantizer* quantizer, const U32 pulse_format, const I32 pulse_size=-1, const PULSEattributer* attributer=0)
  {
    // clean the pulse

    clean();

    // switch over the pulse types we know

    if (!setup(&num_items, &items, pulse_format, pulse_size))
    {
      fprintf(stderr,"ERROR: unknown pulse format %u with pulse size %d\n", pulse_format, pulse_size);
      return FALSE;
    }

    // create pulse's item pointers

    pulse = new U8*[num_items];

    U16 i;
    for (i = 0; i < num_items; i++)
    {
      total_pulse_size += items[i].size;
      switch (items[i].type)
      {
      case PULSEitem::PULSE10:
        this->pulse[i] = (U8*)&(this->gps_time.u64);
        break;
      case PULSEitem::BYTES:
        extra_bytes_number = items[i].size;
        extra_bytes = new U8[extra_bytes_number];
        this->pulse[i] = extra_bytes;
        break;
      default:
        return FALSE;
      }
    }
    this->quantizer = quantizer;
    this->attributer = attributer;
    return TRUE;
  };

  BOOL init(const PULSEquantizer* quantizer, const U32 num_items, const PULSEitem* items, const PULSEattributer* attributer=0)
  {
    U32 i,e;

    // clean the pulse

    clean();

    // create item description

    this->num_items = num_items;
    if (this->items) delete [] this->items;
    this->items = new PULSEitem[num_items];
    if (this->pulse) delete [] this->pulse;
    this->pulse = new U8*[num_items];

    for (i = 0, e = 0; i < num_items; i++)
    {
      this->items[i] = items[i];
      total_pulse_size += items[i].size;

      switch (items[i].type)
      {
      case PULSEitem::PULSE10:
        this->pulse[i] = (U8*)&(this->gps_time.u64);
        break;
      case PULSEitem::BYTES:
        extra_bytes_number = items[i].size;
        extra_bytes = new U8[extra_bytes_number];
        this->pulse[i] = extra_bytes;
        break;
      default:
        return FALSE;
      }
    }
    this->quantizer = quantizer;
    this->attributer = attributer;
    return TRUE;
  };

  BOOL inside_rectangle(const F64 r_min_x, const F64 r_min_y, const F64 r_max_x, const F64 r_max_y) const
  {
    F64 xy;
    xy = get_x();
    if (xy < r_min_x || xy > r_max_x) return FALSE;
    xy = get_y();
    if (xy < r_min_y || xy > r_max_y) return FALSE;
    return TRUE;
  }

  BOOL inside_tile(const F32 ll_x, const F32 ll_y, const F32 ur_x, const F32 ur_y) const
  {
    F64 xy;
    xy = get_x();
    if (xy < ll_x || xy >= ur_x) return FALSE;
    xy = get_y();
    if (xy < ll_y || xy >= ur_y) return FALSE;
    return TRUE;
  }

  BOOL inside_circle(const F64 center_x, const F64 center_y, F64 squared_radius) const
  {
    F64 dx = center_x - get_x();
    F64 dy = center_y - get_y();
    return ((dx*dx+dy*dy) < squared_radius);
  }

  BOOL inside_box(const F64 min_x, const F64 min_y, const F64 min_z, const F64 max_x, const F64 max_y, const F64 max_z) const
  {
    F64 xyz;
    xyz = get_x();
    if (xyz < min_x || xyz > max_x) return FALSE;
    xyz = get_y();
    if (xyz < min_y || xyz > max_y) return FALSE;
    xyz = get_z();
    if (xyz < min_z || xyz > max_z) return FALSE;
    return TRUE;
  }

  void zero()
  {
    gps_time.u64 = 0;
    offset=0;
    X=Y=Z=0;
    dx=dy=dz=0;
    edge_of_flight_line=0;
    scan_direction_flag=0;
  };

  void clean()
  {
    zero();

    if (extra_bytes)
    {
      delete [] extra_bytes;
      extra_bytes = 0;
    }

    extra_bytes_number = 0;
    total_pulse_size = 0;

    if (pulse) delete [] pulse;
    pulse = 0;

    if (items) delete [] items;
    items = 0;
    num_items = 0;
  };

  PULSEpulse()
  {
    extra_bytes = 0;
    pulse = 0;
    items = 0;
    clean();
  };

  inline F64 get_x() const { return quantizer->get_x(X); };
  inline F64 get_y() const { return quantizer->get_y(Y); };
  inline F64 get_z() const { return quantizer->get_z(Z); };

  inline void set_x(const F64 x) { this->X = quantizer->get_X(x); };
  inline void set_y(const F64 y) { this->Y = quantizer->get_Y(y); };
  inline void set_z(const F64 z) { this->Z = quantizer->get_Z(z); };

  inline void get_anchor()
  {
    anchor[0] = get_x();
    anchor[1] = get_y();
    anchor[2] = get_z();
  };

  inline void get_anchor(F64* anchor)
  {
    anchor[0] = get_x();
    anchor[1] = get_y();
    anchor[2] = get_z();
  };

  inline void set_anchor()
  {
    set_x(anchor[0]);
    set_y(anchor[1]);
    set_z(anchor[2]);
  };

  inline void set_anchor(const F64* anchor)
  {
    set_x(anchor[0]);
    set_y(anchor[1]);
    set_z(anchor[2]);
  };

  inline void get_anchor_and_first()
  {
    get_anchor();
    if (first_returning_sample)
    {
      first[0] = anchor[0]+first_returning_sample*dx;
      first[1] = anchor[1]+first_returning_sample*dy;
      first[2] = anchor[2]+first_returning_sample*dz;
    }
    else
    {
      first[0] = anchor[0];
      first[1] = anchor[1];
      first[2] = anchor[2];
    }
  };

  inline void get_anchor_and_last()
  {
    get_anchor();
    if (last_returning_sample)
    {
      last[0] = anchor[0]+last_returning_sample*dx;
      last[1] = anchor[1]+last_returning_sample*dy;
      last[2] = anchor[2]+last_returning_sample*dz;
    }
    else
    {
      last[0] = anchor[0];
      last[1] = anchor[1];
      last[2] = anchor[2];
    }
  };

  inline void get_anchor_first_last()
  {
    get_anchor();
    if (first_returning_sample)
    {
      first[0] = anchor[0]+first_returning_sample*dx;
      first[1] = anchor[1]+first_returning_sample*dy;
      first[2] = anchor[2]+first_returning_sample*dz;
    }
    else
    {
      first[0] = anchor[0];
      first[1] = anchor[1];
      first[2] = anchor[2];
    }
    if (last_returning_sample)
    {
      last[0] = anchor[0]+last_returning_sample*dx;
      last[1] = anchor[1]+last_returning_sample*dy;
      last[2] = anchor[2]+last_returning_sample*dz;
    }
    else
    {
      last[0] = anchor[0];
      last[1] = anchor[1];
      last[2] = anchor[2];
    }
  };

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

  ~PULSEpulse()
  {
    clean();
  };
};

#endif
