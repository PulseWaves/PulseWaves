/*
===============================================================================

  FILE:  pulsepulse.cpp
  
  CONTENTS:
  
    see corresponding header file
  
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
  
    see corresponding header file
  
===============================================================================
*/
#include "pulsepulse.hpp"

#include <stdlib.h>
#include <string.h>

PULSEpulse & PULSEpulse::operator=(const PULSEpulse & other)
{
  T = other.T;
  offset = other.offset;
  anchor_X = other.anchor_X;
  anchor_Y = other.anchor_Y;
  anchor_Z = other.anchor_Z;
  target_X = other.target_X;
  target_Y = other.target_Y;
  target_Z = other.target_Z;
  first_returning_sample = other.first_returning_sample;
  last_returning_sample = other.last_returning_sample;
  descriptor_index = other.descriptor_index;
  reserved = other.reserved;
  edge_of_scan_line = other.edge_of_scan_line;
  scan_direction = other.scan_direction;
  mirror_facet = other.mirror_facet;
  intensity = other.intensity;
  classification = other.classification;
  if (other.extra_bytes && extra_bytes)
  {
    memcpy(extra_bytes, other.extra_bytes, extra_bytes_number);
  }
  return *this;
};

void PULSEpulse::copy_to(U8* buffer) const
{
  U32 i;
  U32 b = 0;
  for (i = 0; i < num_items; i++)
  {
    memcpy(&buffer[b], pulse[i], items[i].size);
    b += items[i].size;
  }
};

void PULSEpulse::copy_from(const U8* buffer)
{
  U32 i;
  U32 b = 0;
  for (i = 0; i < num_items; i++)
  {
    memcpy(pulse[i], &buffer[b], items[i].size);
    b += items[i].size;
  }
};

BOOL PULSEpulse::save_to_txt(FILE* file) const
{
  if (file == 0) return FALSE;

  fprintf(file, "P\012");    
#ifdef _WIN32
  fprintf(file, "%I64d\012", T);    
  fprintf(file, "%I64d\012", offset);    
#else
  fprintf(file, "%lld\012", T);    
  fprintf(file, "%lld\012", offset);    
#endif
  fprintf(file, "%d %d %d\012", anchor_X, anchor_Y, anchor_Z);    
  fprintf(file, "%d %d %d\012", target_X, target_Y, target_Z);
  fprintf(file, "%d %d\012", first_returning_sample, last_returning_sample);    
  fprintf(file, "%d\012", descriptor_index);    
  fprintf(file, "%d %d %d %d\012", scan_direction, edge_of_scan_line, mirror_facet, reserved);    
  fprintf(file, "%d %d\012", intensity, classification);
  if (has_pulse_source_ID)
  {
    fprintf(file, "%u\012", pulse_source_ID);
  }
  return TRUE;
};

// these functions set the desired pulse format (and maybe add on extra attributes)

BOOL PULSEpulse::setup(U16* num_items, PULSEitem** items, const U32 pulse_format, const U32 pulse_attributes, const U32 pulse_size, const U32 compressor)
{
  I32 extra_bytes_number = (I32)pulse_size;

  // switch over the point types we know
  switch (pulse_format)
  {
  case 0:
    extra_bytes_number -= (PULSEWAVES_PULSE0_SIZE);
    break;
  default:
    fprintf(stderr, "ERROR: pulse format %d unknown\n", pulse_format);
    return FALSE;
  }

  // check the pulse attributes we know
  has_pulse_source_ID = FALSE;
  if (pulse_attributes & PULSEWAVES_PULSESOURCEID16)
  {
    extra_bytes_number -= PULSEWAVES_PULSESOURCEID16_SIZE;
    has_pulse_source_ID = TRUE;
  }
  else if (pulse_attributes & PULSEWAVES_PULSESOURCEID32)
  {
    extra_bytes_number -= PULSEWAVES_PULSESOURCEID32_SIZE;
    has_pulse_source_ID = TRUE;
  }
  else if (pulse_attributes)
  {
    fprintf(stderr, "WARNING: pulse contains unknown attributes %u\n", pulse_attributes);
  }

  if (extra_bytes_number < 0)
  {
    fprintf(stderr, "ERROR: pulse size %u is %d bytes too small for format %u with attributes %u\n", pulse_size, -extra_bytes_number, pulse_format, pulse_attributes);
    return FALSE;
  }

  // create item description

  (*num_items) = 1 + !!(extra_bytes_number) + !!(has_pulse_source_ID);
  (*items) = new PULSEitem[*num_items];

  U16 i = 1;
  (*items)[0].type = PULSEitem::PULSE0;
  (*items)[0].size = PULSEWAVES_PULSE0_SIZE;
  (*items)[0].version = 0;
  if (has_pulse_source_ID)
  {
    if (pulse_attributes & PULSEWAVES_PULSESOURCEID16)
    {
      (*items)[i].type = PULSEitem::PULSESOURCEID16;
      (*items)[i].size = PULSEWAVES_PULSESOURCEID16_SIZE;
      (*items)[i].version = 0;
      i++;
    }
    else if (pulse_attributes & PULSEWAVES_PULSESOURCEID32)
    {
      (*items)[i].type = PULSEitem::PULSESOURCEID32;
      (*items)[i].size = PULSEWAVES_PULSESOURCEID32_SIZE;
      (*items)[i].version = 0;
      i++;
    }
  }
  if (extra_bytes_number)
  {
    (*items)[i].type = PULSEitem::EXTRABYTES;
    (*items)[i].size = extra_bytes_number;
    (*items)[i].version = 0;
    i++;
  }
  return TRUE;
};

BOOL PULSEpulse::init(const PULSEheader* header, const PULSEattributer* attributer)
{
  // clean the pulse

  clean();

  // switch over the pulse types we know

  if (!setup(&num_items, &items, header->pulse_format, header->pulse_attributes, header->pulse_size))
  {
    fprintf(stderr,"ERROR: unknown pulse format %u with attributes %u of pulse size %d\n", header->pulse_format, header->pulse_attributes, header->pulse_size);
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
    case PULSEitem::PULSE0:
      this->pulse[i] = (U8*)&(this->T);
      break;
    case PULSEitem::PULSESOURCEID16:
    case PULSEitem::PULSESOURCEID32:
      this->pulse[i] = (U8*)&(this->pulse_source_ID);
      has_pulse_source_ID = TRUE;
      break;
    case PULSEitem::EXTRABYTES:
      extra_bytes_number = items[i].size;
      extra_bytes = new U8[extra_bytes_number];
      this->pulse[i] = extra_bytes;
      break;
    default:
      return FALSE;
    }
  }
  this->quantizer = header;
  this->attributer = attributer;
  return TRUE;
};

BOOL PULSEpulse::init(const PULSEquantizer* quantizer, const U32 num_items, const PULSEitem* items, const PULSEattributer* attributer)
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
    case PULSEitem::PULSE0:
      this->pulse[i] = (U8*)&(this->T);
      break;
    case PULSEitem::PULSESOURCEID16:
    case PULSEitem::PULSESOURCEID32:
      this->pulse[i] = (U8*)&(this->pulse_source_ID);
      break;
    case PULSEitem::EXTRABYTES:
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

BOOL PULSEpulse::inside_rectangle(const F64 r_min_x, const F64 r_min_y, const F64 r_max_x, const F64 r_max_y) const
{
  F64 x = get_first_x();
  F64 y = get_first_y();
  if ((x >= r_min_x) && (x < r_max_x) && (y >= r_min_y) && (y < r_max_y)) return TRUE;
  x = get_last_x();
  y = get_last_y();
  if ((x >= r_min_x) && (x < r_max_x) && (y >= r_min_y) && (y < r_max_y)) return TRUE;
  return FALSE;
}

BOOL PULSEpulse::inside_tile(const F32 ll_x, const F32 ll_y, const F32 ur_x, const F32 ur_y) const
{
  F64 x = get_first_x();
  F64 y = get_first_y();
  if ((x >= ll_x) && (x < ur_x) && (y >= ll_y) && (y < ur_y)) return TRUE;
  x = get_last_x();
  y = get_last_y();
  if ((x >= ll_x) && (x < ur_x) && (y >= ll_y) && (y < ur_y)) return TRUE;
  return FALSE;
}

BOOL PULSEpulse::inside_circle(const F64 center_x, const F64 center_y, F64 squared_radius) const
{
  F64 dx = center_x - get_first_x();
  F64 dy = center_x - get_first_y();
  if ((dx*dx+dy*dy) < squared_radius) return TRUE;
  dx = center_x - get_last_x();
  dy = center_y - get_last_y();
  return ((dx*dx+dy*dy) < squared_radius);
};

BOOL PULSEpulse::inside_box(const F64 min_x, const F64 min_y, const F64 min_z, const F64 max_x, const F64 max_y, const F64 max_z) const
{
  F64 xyz;
  xyz = get_first_x();
  if ((xyz < min_x) || (xyz >= max_x)) return FALSE;
  xyz = get_first_y();
  if ((xyz < min_y) || (xyz >= max_y)) return FALSE;
  xyz = get_first_z();
  if ((xyz < min_z) || (xyz >= max_z)) return FALSE;
  xyz = get_last_x();
  if ((xyz < min_x) || (xyz >= max_x)) return FALSE;
  xyz = get_last_y();
  if ((xyz < min_y) || (xyz >= max_y)) return FALSE;
  xyz = get_last_z();
  if ((xyz < min_z) || (xyz >= max_z)) return FALSE;
  return TRUE;
};

void PULSEpulse::zero()
{
  T = 0;
  offset = 0;
  anchor_X = anchor_Y = anchor_Z = 0;
  target_X = target_Y = target_Z = 0;
  first_returning_sample = 0;
  last_returning_sample = 0;
  descriptor_index = 0;
  reserved = 0;
  scan_direction = 0;
  edge_of_scan_line = 0;
  mirror_facet = 0;
  intensity = 0;
  classification = 0;
};

void PULSEpulse::clean()
{
  zero();

  has_pulse_source_ID = FALSE;

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

PULSEpulse::PULSEpulse()
{
  extra_bytes = 0;
  pulse = 0;
  items = 0;
  clean();
};

PULSEpulse::PULSEpulse(const PULSEpulse & other)
{
  *this = other;
}

PULSEpulse::~PULSEpulse()
{
  clean();
}
