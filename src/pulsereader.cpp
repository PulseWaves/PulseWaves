/*
===============================================================================

  FILE:  pulsereader.cpp
  
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
#include "pulsereader.hpp"

#include "pulsewavesdefinitions.hpp"
#include "pulseindex.hpp"
#include "pulsefilter.hpp"
#include "pulsetransform.hpp"

#include "pulsereader_pls.hpp"
#include "pulsereadermerged.hpp"

#ifndef PULSEWAVES_DLL
#include "pulsereader_lgw.hpp"
#include "pulsereader_gcw.hpp"
#include "pulsereader_csd.hpp"
#ifdef PULSEWAVES_SDF
#include "pulsereader_sdf.hpp"
#endif // PULSEWAVES_SDF
#ifdef PULSEWAVES_LAS
#include "pulsereader_las.hpp"
#endif // PULSEWAVES_LAS
#endif // PULSEWAVES_DLL

#include <stdlib.h>
#include <string.h>

PULSEreader::PULSEreader()
{
  header_is_populated = TRUE;
  npulses = 0;
  p_count = 0;
  waves = 0;
  read_simple = &PULSEreader::read_pulse_default;
  read_complex = 0;
  index = 0;
  filter = 0;
  transform = 0;
  r_min_x = 0;
  r_min_y = 0;
  r_max_x = 0;
  r_max_y = 0;
  t_ll_x = 0;
  t_ll_y = 0;
  t_size = 0;
  t_ur_x = 0;
  t_ur_y = 0;
  c_center_x = 0;
  c_center_y = 0;
  c_radius = 0;
  c_radius_squared = 0;
}
  
PULSEreader::~PULSEreader()
{
  if (index) delete index;
}

void PULSEreader::set_index(PULSEindex* index)
{
  if (this->index) delete this->index;
  this->index = index;
}

PULSEindex* PULSEreader::get_index() const
{
  return index;
}

void PULSEreader::set_filter(PULSEfilter* filter)
{
  this->filter = filter;
  if (filter && transform)
  {
    read_simple = &PULSEreader::read_pulse_filtered_and_transformed;
  }
  else if (filter)
  {
    read_simple = &PULSEreader::read_pulse_filtered;
  }
  else if (transform)
  {
    read_simple = &PULSEreader::read_pulse_transformed;
  }
  read_complex = &PULSEreader::read_pulse_default;
}

void PULSEreader::set_transform(PULSEtransform* transform)
{
  this->transform = transform;
  if (filter && transform)
  {
    read_simple = &PULSEreader::read_pulse_filtered_and_transformed;
  }
  else if (filter)
  {
    read_simple = &PULSEreader::read_pulse_filtered;
  }
  else if (transform)
  {
    read_simple = &PULSEreader::read_pulse_transformed;
  }
  read_complex = &PULSEreader::read_pulse_default;
}

void PULSEreader::reset_filter()
{
  if (filter) filter->reset();
}

BOOL PULSEreader::inside_tile(const F32 ll_x, const F32 ll_y, const F32 size)
{
  t_ll_x = ll_x;
  t_ll_y = ll_y;
  t_size = size;
  t_ur_x = ll_x + size;
  t_ur_y = ll_y + size;
  header.min_x = ll_x;
  header.min_y = ll_y;
  header.max_x = ll_x + size - 0.001f * header.x_scale_factor;
  header.max_y = ll_y + size - 0.001f * header.y_scale_factor;
  if (index) index->intersect_tile(ll_x, ll_y, size);
  if (filter || transform)
  {
    if (index)
      read_complex = &PULSEreader::read_pulse_inside_tile_indexed;
    else
      read_complex = &PULSEreader::read_pulse_inside_tile;
  }
  else
  {
    if (index)
      read_simple = &PULSEreader::read_pulse_inside_tile_indexed;
    else
      read_simple = &PULSEreader::read_pulse_inside_tile;
  }
  return TRUE;
}

BOOL PULSEreader::inside_circle(const F64 center_x, const F64 center_y, const F64 radius)
{
  c_center_x = center_x;
  c_center_y = center_y;
  c_radius = radius;
  c_radius_squared = radius*radius;
  header.min_x = center_x - radius;
  header.min_y = center_y - radius;
  header.max_x = center_x + radius;
  header.max_y = center_y + radius;
  if (index) index->intersect_circle(center_x, center_y, radius);
  if (filter || transform)
  {
    if (index)
      read_complex = &PULSEreader::read_pulse_inside_circle_indexed;
    else
      read_complex = &PULSEreader::read_pulse_inside_circle;
  }
  else
  {
    if (index)
      read_simple = &PULSEreader::read_pulse_inside_circle_indexed;
    else
      read_simple = &PULSEreader::read_pulse_inside_circle;
  }
  return TRUE;
}

BOOL PULSEreader::inside_rectangle(const F64 min_x, const F64 min_y, const F64 max_x, const F64 max_y)
{
  r_min_x = min_x;
  r_min_y = min_y;
  r_max_x = max_x;
  r_max_y = max_y;
  header.min_x = min_x;
  header.min_y = min_y;
  header.max_x = max_x;
  header.max_y = max_y;
  if (index) index->intersect_rectangle(min_x, min_y, max_x, max_y);
  if (filter || transform)
  {
    if (index)
      read_complex = &PULSEreader::read_pulse_inside_rectangle_indexed;
    else
      read_complex = &PULSEreader::read_pulse_inside_rectangle;
  }
  else
  {
    if (index)
      read_simple = &PULSEreader::read_pulse_inside_rectangle_indexed;
    else
      read_simple = &PULSEreader::read_pulse_inside_rectangle;
  }
  return TRUE;
}

BOOL PULSEreader::read_pulse_inside_tile()
{
  while (read_pulse_default())
  {
    if (pulse.inside_tile(t_ll_x, t_ll_y, t_ur_x, t_ur_y)) return TRUE;
  }
  return FALSE;
}

BOOL PULSEreader::read_pulse_inside_tile_indexed()
{
  while (index->seek_next((PULSEreader*)this))
  {
    if (read_pulse_default() && pulse.inside_tile(t_ll_x, t_ll_y, t_ur_x, t_ur_y)) return TRUE;
  }
  return FALSE;
}

BOOL PULSEreader::read_pulse_inside_circle()
{
  while (read_pulse_default())
  {
    if (pulse.inside_circle(c_center_x, c_center_y, c_radius_squared)) return TRUE;
  }
  return FALSE;
}

BOOL PULSEreader::read_pulse_inside_circle_indexed()
{
  while (index->seek_next((PULSEreader*)this))
  {
    if (read_pulse_default() && pulse.inside_circle(c_center_x, c_center_y, c_radius_squared)) return TRUE;
  }
  return FALSE;
}

BOOL PULSEreader::read_pulse_inside_rectangle()
{
  while (read_pulse_default())
  {
    if (pulse.inside_rectangle(r_min_x, r_min_y, r_max_x, r_max_y)) return TRUE;
  }
  return FALSE;
}

BOOL PULSEreader::read_pulse_inside_rectangle_indexed()
{
  while (index->seek_next((PULSEreader*)this))
  {
    if (read_pulse_default() && pulse.inside_rectangle(r_min_x, r_min_y, r_max_x, r_max_y)) return TRUE;
  }
  return FALSE;
}

BOOL PULSEreader::read_pulse_filtered()
{
  while ((this->*read_complex)())
  {
    pulse.compute_anchor_and_target_and_dir();
    if (!filter->filter(&pulse)) return TRUE;
  }
  return FALSE;
}

BOOL PULSEreader::read_pulse_transformed()
{
  if ((this->*read_complex)())
  {
    transform->transform(&pulse);
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreader::read_pulse_filtered_and_transformed()
{
  if (read_pulse_filtered())
  {
    transform->transform(&pulse);
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreadOpener::has_populated_header() const
{
  return (populate_header || (file_name && (strstr(file_name, ".pls") || strstr(file_name, ".plz") || strstr(file_name, ".LAS") || strstr(file_name, ".LAZ"))));
}

void PULSEreadOpener::reset()
{
  file_name_current = 0;
  file_name = 0;
}

PULSEreader* PULSEreadOpener::open(CHAR* other_file_name)
{
  if (file_names || other_file_name)
  {
    if (file_name_current == file_name_number && other_file_name == 0) return 0;
    if ((file_name_number > 1) && merged)
    {
      PULSEreaderMerged* pulsereadermerged = new PULSEreaderMerged();
      pulsereadermerged->set_scale_factor(scale_factor);
      pulsereadermerged->set_offset(offset);
      for (file_name_current = 0; file_name_current < file_name_number; file_name_current++) pulsereadermerged->add_file_name(file_names[file_name_current]);
      if (!pulsereadermerged->open())
      {
        fprintf(stderr,"ERROR: cannot open pulsereadermerged with %d file names\n", file_name_number);
        delete pulsereadermerged;
        return 0;
      }
      if (filter) pulsereadermerged->set_filter(filter);
      if (transform) pulsereadermerged->set_transform(transform);
      if (inside_tile) pulsereadermerged->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
      if (inside_circle) pulsereadermerged->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
      if (inside_rectangle) pulsereadermerged->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
      if (files_are_flightlines)
      {
        pulsereadermerged->header.file_source_ID = 0;
        pulsereadermerged->set_files_are_flightlines(TRUE);
      }
      return pulsereadermerged;
    }
    else
    {
      if (other_file_name)
      {
        file_name = other_file_name;
      }
      else
      {
        file_name = file_names[file_name_current];
        file_name_current++;
      }
      if (strstr(file_name, ".pls") || strstr(file_name, ".plz") || strstr(file_name, ".PLS") || strstr(file_name, ".PLZ"))
      {
        PULSEreaderPLS* pulsereaderpls = 0;
        if (scale_factor == 0 && offset == 0)
          pulsereaderpls = new PULSEreaderPLS();
/*
        else if (scale_factor != 0 && offset == 0)
          pulsereaderpls = new PULSEreaderPLSrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          pulsereaderpls = new PULSEreaderPLSreoffset(offset[0], offset[1], offset[2]);
        else
          pulsereaderpls = new PULSEreaderPLSrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
*/
        if (!pulsereaderpls->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open pulsereaderpls with file name '%s'\n", file_name);
          delete pulsereaderpls;
          return 0;
        }
        PULSEindex* index = new PULSEindex();
        if (index->read(file_name))
          pulsereaderpls->set_index(index);
        else
          delete index;
        if (filter) pulsereaderpls->set_filter(filter);
        if (transform) pulsereaderpls->set_transform(transform);
        if (inside_tile) pulsereaderpls->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereaderpls->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereaderpls->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        if (files_are_flightlines)
        {
          pulsereaderpls->header.file_source_ID = file_name_current;
          pulsereaderpls->pulse.pulse_source_ID = file_name_current;
        }
        return pulsereaderpls;
      }
#ifndef PULSEWAVES_DLL
      else if (strstr(file_name, ".lgw") || strstr(file_name, ".LGW"))
      {
        PULSEreaderLGW* pulsereaderlgw = 0;
        if (scale_factor == 0 && offset == 0)
          pulsereaderlgw = new PULSEreaderLGW();
/*
        else if (scale_factor != 0 && offset == 0)
          pulsereaderlgw = new PULSEreaderLGWrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          pulsereaderlgw = new PULSEreaderLGWreoffset(offset[0], offset[1], offset[2]);
        else
          pulsereaderlgw = new PULSEreaderLGWrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
*/
        if (!pulsereaderlgw->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open pulsereaderlgw with file name '%s'\n", file_name);
          delete pulsereaderlgw;
          return 0;
        }
        PULSEindex* index = new PULSEindex();
        if (index->read(file_name))
          pulsereaderlgw->set_index(index);
        else
          delete index;
        if (filter) pulsereaderlgw->set_filter(filter);
        if (transform) pulsereaderlgw->set_transform(transform);
        if (inside_tile) pulsereaderlgw->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereaderlgw->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereaderlgw->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        if (files_are_flightlines)
        {
          pulsereaderlgw->header.file_source_ID = file_name_current;
          pulsereaderlgw->pulse.pulse_source_ID = file_name_current;
        }
        return pulsereaderlgw;
      }
      else if (strstr(file_name, ".lgc") || strstr(file_name, ".LGC"))
      {
        PULSEreaderGCW* pulsereadergcw = 0;
        if (scale_factor == 0 && offset == 0)
          pulsereadergcw = new PULSEreaderGCW();
/*
        else if (scale_factor != 0 && offset == 0)
          pulsereadergcw = new PULSEreaderGCWrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          pulsereadergcw = new PULSEreaderGCWreoffset(offset[0], offset[1], offset[2]);
        else
          pulsereadergcw = new PULSEreaderGCWrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
*/
        if (!pulsereadergcw->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open pulsereadergcw with file name '%s'\n", file_name);
          delete pulsereadergcw;
          return 0;
        }
        PULSEindex* index = new PULSEindex();
        if (index->read(file_name))
          pulsereadergcw->set_index(index);
        else
          delete index;
        if (filter) pulsereadergcw->set_filter(filter);
        if (transform) pulsereadergcw->set_transform(transform);
        if (inside_tile) pulsereadergcw->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereadergcw->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereadergcw->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        if (files_are_flightlines)
        {
          pulsereadergcw->header.file_source_ID = file_name_current;
          pulsereadergcw->pulse.pulse_source_ID = file_name_current;
        }
        return pulsereadergcw;
      }
      else if (strstr(file_name, ".csd") || strstr(file_name, ".CSD"))
      {
        PULSEreaderCSD* pulsereadercsd = 0;
        if (scale_factor == 0 && offset == 0)
          pulsereadercsd = new PULSEreaderCSD();
/*
        else if (scale_factor != 0 && offset == 0)
          pulsereadercsd = new PULSEreaderCSDrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          pulsereadercsd = new PULSEreaderCSDreoffset(offset[0], offset[1], offset[2]);
        else
          pulsereadercsd = new PULSEreaderCSDrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
*/
        if (!pulsereadercsd->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open pulsereadercsd with file name '%s'\n", file_name);
          delete pulsereadercsd;
          return 0;
        }
        PULSEindex* index = new PULSEindex();
        if (index->read(file_name))
          pulsereadercsd->set_index(index);
        else
          delete index;
        if (filter) pulsereadercsd->set_filter(filter);
        if (transform) pulsereadercsd->set_transform(transform);
        if (inside_tile) pulsereadercsd->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereadercsd->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereadercsd->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        if (files_are_flightlines)
        {
          pulsereadercsd->header.file_source_ID = file_name_current;
          pulsereadercsd->pulse.pulse_source_ID = file_name_current;
        }
        return pulsereadercsd;
      }
/*      else if (strstr(file_name, ".dat") || strstr(file_name, ".DAT"))
      {
        PULSEreaderDAT* pulsereaderdat;
        if (scale_factor == 0 && offset == 0)
          pulsereaderdat = new PULSEreaderDAT();
        else if (scale_factor != 0 && offset == 0)
          pulsereaderdat = new PULSEreaderDATrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          pulsereaderdat = new PULSEreaderDATreoffset(offset[0], offset[1], offset[2]);
        else
          pulsereaderdat = new PULSEreaderDATrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
        if (!pulsereaderdat->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open pulsereaderdat with file name '%s'\n", file_name);
          delete pulsereaderdat;
          return 0;
        }
        PULSEindex* index = new PULSEindex();
        if (index->read(file_name))
          pulsereaderdat->set_index(index);
        else
          delete index;
        if (filter) pulsereaderdat->set_filter(filter);
        if (transform) pulsereaderdat->set_transform(transform);
        if (inside_tile) pulsereaderdat->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereaderdat->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereaderdat->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        if (files_are_flightlines)
        {
          pulsereaderdat->header.file_source_ID = file_name_current;
          pulsereaderdat->pulse.pulse_source_ID = file_name_current;
        }
        return pulsereaderdat;
      }
*/
#ifdef PULSEWAVES_SDF
      else if (strstr(file_name, ".sdf") || strstr(file_name, ".SDF"))
      {
        PULSEreaderSDF* pulsereadersdf;
        if (scale_factor == 0 && offset == 0)
          pulsereadersdf = new PULSEreaderSDF();
/*
        else if (scale_factor != 0 && offset == 0)
          pulsereadersdf = new PULSEreaderSDFrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          pulsereadersdf = new PULSEreaderSDFreoffset(offset[0], offset[1], offset[2]);
        else
          pulsereadersdf = new PULSEreaderSDFrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
*/
        if (!pulsereadersdf->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open pulsereadersdf with file name '%s'\n", file_name);
          delete pulsereadersdf;
          return 0;
        }
        PULSEindex* index = new PULSEindex();
        if (index->read(file_name))
          pulsereadersdf->set_index(index);
        else
          delete index;
        if (filter) pulsereadersdf->set_filter(filter);
        if (transform) pulsereadersdf->set_transform(transform);
        if (inside_tile) pulsereadersdf->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereadersdf->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereadersdf->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        if (files_are_flightlines)
        {
          pulsereadersdf->header.file_source_ID = file_name_current;
          pulsereadersdf->pulse.pulse_source_ID = file_name_current;
        }
        return pulsereadersdf;
      }
#endif // PULSEWAVES_SDF
#ifdef PULSEWAVES_LAS
      else if (strstr(file_name, ".las") || strstr(file_name, ".laz") || strstr(file_name, ".LAS") || strstr(file_name, ".LAZ"))
      {
        PULSEreaderLAS* pulsereaderlas;
        if (scale_factor == 0 && offset == 0)
          pulsereaderlas = new PULSEreaderLAS();
/*
        else if (scale_factor != 0 && offset == 0)
          pulsereaderlas = new PULSEreaderLASrescale(scale_factor[0], scale_factor[1], scale_factor[2]);
        else if (scale_factor == 0 && offset != 0)
          pulsereaderlas = new PULSEreaderLASreoffset(offset[0], offset[1], offset[2]);
        else
          pulsereaderlas = new PULSEreaderLASrescalereoffset(scale_factor[0], scale_factor[1], scale_factor[2], offset[0], offset[1], offset[2]);
*/
        if (!pulsereaderlas->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot open pulsereaderlas with file name '%s'\n", file_name);
          delete pulsereaderlas;
          return 0;
        }
        PULSEindex* index = new PULSEindex();
        if (index->read(file_name))
          pulsereaderlas->set_index(index);
        else
          delete index;
        if (filter) pulsereaderlas->set_filter(filter);
        if (transform) pulsereaderlas->set_transform(transform);
        if (inside_tile) pulsereaderlas->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereaderlas->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereaderlas->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        if (files_are_flightlines)
        {
          pulsereaderlas->header.file_source_ID = file_name_current;
          pulsereaderlas->pulse.pulse_source_ID = file_name_current;
        }
        return pulsereaderlas;
      }
#endif // PULSEWAVES_LAS
#endif // PULSEWAVES_DLL
    }
  }
  fprintf(stderr,"ERROR: unknown pulsewaves format '%s'\n", file_name);
  return 0;
}

BOOL PULSEreadOpener::reopen(PULSEreader* pulsereader)
{
  if (file_names)
  {
    if ((file_name_number > 1) && merged)
    {
/*
      PULSEreaderMerged* pulsereadermerged = (PULSEreaderMerged*)pulsereader;
      if (!pulsereadermerged->reopen())
      {
        fprintf(stderr,"ERROR: cannot reopen pulsereadermerged\n");
        return FALSE;
      }
      if (filter) pulsereadermerged->reset_filter();
      if (inside_tile) pulsereadermerged->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
      if (inside_circle) pulsereadermerged->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
      if (inside_rectangle) pulsereadermerged->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
      return TRUE;
*/
    }
    else
    {
      if (!file_name) return FALSE;
      if (strstr(file_name, ".pls") || strstr(file_name, ".plz") || strstr(file_name, ".PLS") || strstr(file_name, ".PLZ"))
      {
        PULSEreaderPLS* pulsereaderpls = (PULSEreaderPLS*)pulsereader;
        if (!pulsereaderpls->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen pulsereaderpls with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) pulsereaderpls->reset_filter();
        if (inside_tile) pulsereaderpls->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereaderpls->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereaderpls->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
#ifndef PULSEWAVES_DLL
      else if (strstr(file_name, ".lgw") || strstr(file_name, ".LGW"))
      {
        PULSEreaderLGW* pulsereaderlgw = (PULSEreaderLGW*)pulsereader;
        if (!pulsereaderlgw->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen pulsereaderlgw with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) pulsereaderlgw->reset_filter();
        if (inside_tile) pulsereaderlgw->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereaderlgw->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereaderlgw->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
      else if (strstr(file_name, ".lgc") || strstr(file_name, ".LGC"))
      {
        PULSEreaderGCW* pulsereadergcw = (PULSEreaderGCW*)pulsereader;
        if (!pulsereadergcw->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen pulsereadergcw with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) pulsereadergcw->reset_filter();
        if (inside_tile) pulsereadergcw->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereadergcw->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereadergcw->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
      else if (strstr(file_name, ".lgc") || strstr(file_name, ".LGC"))
      {
        PULSEreaderCSD* pulsereadercsd = (PULSEreaderCSD*)pulsereader;
        if (!pulsereadercsd->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen pulsereadercsd with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) pulsereadercsd->reset_filter();
        if (inside_tile) pulsereadercsd->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereadercsd->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereadercsd->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
/*
      else if (strstr(file_name, ".dat") || strstr(file_name, ".DAT"))
      {
        PULSEreaderDAT* pulsereaderdat = (PULSEreaderDAT*)pulsereader;
        if (!pulsereaderdat->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen pulsereaderdat with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) pulsereaderdat->reset_filter();
        if (inside_tile) pulsereaderdat->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereaderdat->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereaderdat->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
*/
#ifdef PULSEWAVES_SDF
      else if (strstr(file_name, ".sdf") || strstr(file_name, ".SDF"))
      {
        PULSEreaderSDF* pulsereadersdf = (PULSEreaderSDF*)pulsereader;
        if (!pulsereadersdf->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen pulsereadersdf with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) pulsereadersdf->reset_filter();
        if (inside_tile) pulsereadersdf->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereadersdf->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereadersdf->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
#endif // PULSEWAVES_SDF
#ifdef PULSEWAVES_LAS
      else if (strstr(file_name, ".las") || strstr(file_name, ".laz") || strstr(file_name, ".LAS") || strstr(file_name, ".LAZ"))
      {
        PULSEreaderLAS* pulsereaderlas = (PULSEreaderLAS*)pulsereader;
        if (!pulsereaderlas->open(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen pulsereaderlas with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) pulsereaderlas->reset_filter();
        if (inside_tile) pulsereaderlas->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereaderlas->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereaderlas->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
      }
#endif // PULSEWAVES_LAS
	  else
      {
/*
        PULSEreaderTXT* pulsereadertxt = (PULSEreaderTXT*)pulsereader;
        if (!pulsereadertxt->reopen(file_name))
        {
          fprintf(stderr,"ERROR: cannot reopen pulsereadertxt with file name '%s'\n", file_name);
          return FALSE;
        }
        if (filter) pulsereadertxt->reset_filter();
        if (inside_tile) pulsereadertxt->inside_tile(inside_tile[0], inside_tile[1], inside_tile[2]);
        if (inside_circle) pulsereadertxt->inside_circle(inside_circle[0], inside_circle[1], inside_circle[2]);
        if (inside_rectangle) pulsereadertxt->inside_rectangle(inside_rectangle[0], inside_rectangle[1], inside_rectangle[2], inside_rectangle[3]);
        return TRUE;
*/
      }
#endif // PULSEWAVES_DLL
    }
  }
  fprintf(stderr,"ERROR: no pulsereader input specified\n");
  return FALSE;
}

void PULSEreadOpener::usage() const
{
  fprintf(stderr,"Supported Pulse Inputs\n");
  fprintf(stderr,"  -i lidar.pls\n");
  fprintf(stderr,"  -i lidar.plz\n");
#ifndef PULSEWAVES_DLL
  fprintf(stderr,"  -i lidar.lgw\n");
  fprintf(stderr,"  -i lidar.lgc\n");
#ifdef PULSEWAVES_SDF
  fprintf(stderr,"  -i lidar.sdf\n");
#endif // PULSEWAVES_SDF
#ifdef PULSEWAVES_LAS
  fprintf(stderr,"  -i lidar.las\n");
  fprintf(stderr,"  -i lidar.laz\n");
#endif // PULSEWAVES_LAS
#endif // PULSEWAVES_DLL
  fprintf(stderr,"  -h\n");
}

BOOL PULSEreadOpener::parse(int argc, char* argv[])
{
  int i;
  for (i = 1; i < argc; i++)
  {
    if (argv[i][0] == '\0')
    {
      continue;
    }
    else if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"-help") == 0)
    {
      PULSEfilter().usage();
      PULSEtransform().usage();
      usage();
      return TRUE;
    }
    else if (strcmp(argv[i],"-i") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs at least 1 argument: file_name or wild_card\n", argv[i]);
        return FALSE;
      }
      *argv[i]='\0';
      i+=1;
      do
      {
#ifdef _WIN32
        add_file_name_windows(argv[i]);
#else // _WIN32
        add_file_name(argv[i]);
#endif // _WIN32
        *argv[i]='\0';
        i+=1;
      } while (i < argc && *argv[i] != '-');
      i-=1;
    }
    else if (strcmp(argv[i],"-inside_tile") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: ll_x, ll_y, size\n", argv[i]);
        return FALSE;
      }
      if (inside_tile == 0) inside_tile = new F32[3];
      inside_tile[0] = (F32)atof(argv[i+1]);
      inside_tile[1] = (F32)atof(argv[i+2]);
      inside_tile[2] = (F32)atof(argv[i+3]);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3; 
    }
    else if (strcmp(argv[i],"-inside_circle") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: center_x, center_y, radius\n", argv[i]);
        return FALSE;
      }
      if (inside_circle == 0) inside_circle = new F64[3];
      inside_circle[0] = atof(argv[i+1]);
      inside_circle[1] = atof(argv[i+2]);
      inside_circle[2] = atof(argv[i+3]);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
    }
    else if (strcmp(argv[i],"-inside") == 0 || strcmp(argv[i],"-inside_rectangle") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x, min_y, max_x, max_y\n", argv[i]);
        return FALSE;
      }
      if (inside_rectangle == 0) inside_rectangle = new F64[4];
      inside_rectangle[0] = atof(argv[i+1]);
      inside_rectangle[1] = atof(argv[i+2]);
      inside_rectangle[2] = atof(argv[i+3]);
      inside_rectangle[3] = atof(argv[i+4]);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-lof") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: list_of_files\n", argv[i]);
        return FALSE;
      }
      FILE* file = fopen(argv[i+1], "r");
      if (file == 0)
      {
        fprintf(stderr, "ERROR: cannot open '%s'\n", argv[i+1]);
        return FALSE;
      }
      CHAR line[1024];
      while (fgets(line, 1024, file))
      {
        // find end of line
        int len = strlen(line) - 1;
        // remove extra white spaces and line return at the end 
        while (len > 0 && ((line[len] == '\n') || (line[len] == ' ') || (line[len] == '\t') || (line[len] == '\012')))
        {
          line[len] = '\0';
          len--;
        }
        add_file_name(line);
      }
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-rescale") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: rescale_x, rescale_y, rescale_z\n", argv[i]);
        return FALSE;
      }
      F64 scale_factor[3];
      scale_factor[0] = atof(argv[i+1]);
      scale_factor[1] = atof(argv[i+2]);
      scale_factor[2] = atof(argv[i+3]);
      set_scale_factor(scale_factor);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
    }
    else if (strcmp(argv[i],"-reoffset") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: reoffset_x, reoffset_y, reoffset_z\n", argv[i]);
        return FALSE;
      }
      F64 offset[3];
			offset[0] = atof(argv[i+1]);
			offset[1] = atof(argv[i+2]);
			offset[2] = atof(argv[i+3]);
      set_offset(offset);
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
    }
    else if (strcmp(argv[i],"-files_are_flightlines") == 0)
    {
      set_files_are_flightlines(TRUE);
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-iadd_extra") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: data_type name description\n", argv[i]);
        return FALSE;
      }
      if (((i+4) < argc) && (argv[i+4][0] != '-'))
      {
        if (((i+5) < argc) && (argv[i+5][0] != '-'))
        {
          add_extra_attribute(atoi(argv[i+1]), argv[i+2], argv[i+3], atof(argv[i+4]), atof(argv[i+5]));
          *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; *argv[i+5]='\0'; i+=5;
        }
        else
        {
          add_extra_attribute(atoi(argv[i+1]), argv[i+2], argv[i+3], atof(argv[i+4]));
          *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4;
        }
      }
      else
      {
        add_extra_attribute(atoi(argv[i+1]), argv[i+2], argv[i+3]);
        *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
      }
    }
    else if (strcmp(argv[i],"-single") == 0)
    {
      set_merged(FALSE);
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-merged") == 0)
    {
      set_merged(TRUE);
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-populate") == 0)
    {
      set_populate_header(TRUE);
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-do_not_populate") == 0)
    {
      set_populate_header(FALSE);
      *argv[i]='\0';
    }
  }

  if (filter) filter->clean();
  else filter = new PULSEfilter();
  if (!filter->parse(argc, argv))
  {
    delete filter;
    return FALSE;
  }
  if (!filter->active())
  {
    delete filter;
    filter = 0;
  }

  if (transform) transform->clean();
  else transform = new PULSEtransform();
  if (!transform->parse(argc, argv))
  {
    delete transform;
    return FALSE;
  }
  if (!transform->active())
  {
    delete transform;
    transform = 0;
  }

  return TRUE;
}

const CHAR* PULSEreadOpener::get_file_name() const
{
  if (file_name)
    return file_name;
  if (file_name_number)
    return file_names[0];
  return 0;
}

const CHAR* PULSEreadOpener::get_file_name(U32 number) const
{
  return file_names[number];
}

I32 PULSEreadOpener::get_file_format(U32 number) const
{
  if (number < file_name_number)
  {
    int end = strlen(file_names[number]) - 4;
    if (strstr(file_names[number] + end, ".pls") || strstr(file_names[number] + end, ".PLS"))
    {
      return PULSEWAVES_FORMAT_PLS;
    }
    else if (strstr(file_names[number] + end, ".plz") || strstr(file_names[number] + end, ".PLZ"))
    {
      return PULSEWAVES_FORMAT_PLZ;
    }
    else if (strstr(file_names[number] + end, ".las") || strstr(file_names[number] + end, ".LAS"))
    {
      return PULSEWAVES_FORMAT_LAS;
    }
    else if (strstr(file_names[number] + end, ".laz") || strstr(file_names[number] + end, ".LAZ"))
    {
      return PULSEWAVES_FORMAT_LAZ;
    }
    else if (strstr(file_names[number] + end, ".lgc") || strstr(file_names[number] + end, ".LGC"))
    {
      return PULSEWAVES_FORMAT_GCW;
    }
    else if (strstr(file_names[number] + end, ".lgw") || strstr(file_names[number] + end, ".LGW"))
    {
      return PULSEWAVES_FORMAT_LGW;
    }
    else
    {
      return PULSEWAVES_FORMAT_TXT;
    }
  }
  else
  {
    return PULSEWAVES_FORMAT_DEFAULT;
  }
}

void PULSEreadOpener::set_merged(const BOOL merged)
{
  this->merged = merged;
}

BOOL PULSEreadOpener::get_merged() const
{
  return merged;
}

void PULSEreadOpener::set_files_are_flightlines(const BOOL files_are_flightlines)
{
  this->files_are_flightlines = files_are_flightlines;
}

void PULSEreadOpener::set_file_name(const CHAR* file_name, BOOL unique)
{
#ifdef _WIN32
  add_file_name_windows(file_name, unique);
#else // _WIN32
  add_file_name(file_name, unique);
#endif // _WIN32
}

BOOL PULSEreadOpener::add_file_name(const CHAR* file_name, BOOL unique)
{
  if (unique)
  {
    U32 i;
    for (i = 0; i < file_name_number; i++)
    {
      if (strcmp(file_names[i], file_name) == 0)
      {
        return FALSE;
      }
    }
  }
  if (file_name_number == file_name_allocated)
  {
    if (file_names)
    {
      file_name_allocated *= 2;
      file_names = (CHAR**)realloc(file_names, sizeof(CHAR*)*file_name_allocated);
    }
    else
    {
      file_name_allocated = 16;
      file_names = (CHAR**)malloc(sizeof(CHAR*)*file_name_allocated);
    }
    if (file_names == 0)
    {
      fprintf(stderr, "ERROR: alloc for file_names pulseer array failed at %d\n", file_name_allocated);
    }
  }
  file_names[file_name_number] = strdup(file_name);
  file_name_number++;
  return TRUE;
}

void PULSEreadOpener::delete_file_name(U32 file_name_id)
{
  if (file_name_id < file_name_number)
  {
    U32 i;
    free(file_names[file_name_id]);
    for (i = file_name_id+1; i < file_name_number; i++)
    {
      file_names[i-1] = file_names[i];
    }
  }
  file_name_number--;
}

BOOL PULSEreadOpener::set_file_name_current(U32 file_name_id)
{
  if (file_name_id < file_name_number)
  {
    file_name_current = file_name_id;
    return TRUE;
  }
  return FALSE;
}

#ifdef _WIN32
#include <windows.h>
void PULSEreadOpener::add_file_name_windows(const CHAR* file_name, BOOL unique)
{
  HANDLE h;
  WIN32_FIND_DATA info;
  h = FindFirstFile(file_name, &info);
  if (h != INVALID_HANDLE_VALUE)
  {
    // find the path
    int len = strlen(file_name);
    while (len && file_name[len] != '\\') len--;
    if (len)
    {
      len++;
      CHAR full_file_name[512];
      strncpy(full_file_name, file_name, len);
	  do
	  {
        sprintf(&full_file_name[len], "%s", info.cFileName);
        add_file_name(full_file_name, unique);
  	  } while (FindNextFile(h, &info));
    }
    else
    {
      do
      {
        add_file_name((const char*)info.cFileName, unique);
  	  } while (FindNextFile(h, &info));
    }
	FindClose(h);
  }
}
#endif // _WIN32

U32 PULSEreadOpener::get_file_name_number() const
{
  return file_name_number;
}

void PULSEreadOpener::set_scale_factor(const F64* scale_factor)
{
  if (scale_factor)
  {
    if (this->scale_factor == 0) this->scale_factor = new F64[3];
    this->scale_factor[0] = scale_factor[0];
    this->scale_factor[1] = scale_factor[1];
    this->scale_factor[2] = scale_factor[2];
  }
  else if (this->scale_factor)
  {
    delete [] this->scale_factor;
    this->scale_factor = 0;
  }
}

void PULSEreadOpener::set_offset(const F64* offset)
{
  if (offset)
  {
    if (this->offset == 0) this->offset = new F64[3];
    this->offset[0] = offset[0];
    this->offset[1] = offset[1];
    this->offset[2] = offset[2];
  }
  else if (this->offset)
  {
    delete [] this->offset;
    this->offset = 0;
  }
}

void PULSEreadOpener::add_extra_attribute(I32 data_type, const CHAR* name, const CHAR* description, F64 scale, F64 offset)
{
  extra_attribute_data_types[number_extra_attributes] = data_type;
  extra_attribute_names[number_extra_attributes] = (name ? strdup(name) : 0);
  extra_attribute_descriptions[number_extra_attributes] = (description ? strdup(description) : 0);
  extra_attribute_scales[number_extra_attributes] = scale;
  extra_attribute_offsets[number_extra_attributes] = offset;
  number_extra_attributes++;
}

void PULSEreadOpener::set_populate_header(BOOL populate_header)
{
  this->populate_header = populate_header;
}

BOOL PULSEreadOpener::active() const
{
  return (file_name_current < file_name_number);
}

PULSEreadOpener::PULSEreadOpener()
{
  file_names = 0;
  file_name = 0;
  merged = TRUE;
  scale_factor = 0;
  offset = 0;
  files_are_flightlines = FALSE;
  number_extra_attributes = 0;
  for (I32 i = 0; i < 10; i++)
  {
    extra_attribute_data_types[i] = 0;
    extra_attribute_names[i] = 0;
    extra_attribute_descriptions[i] = 0;
    extra_attribute_scales[i] = 1.0;
    extra_attribute_offsets[i] = 0.0;
  }
  populate_header = TRUE;
  file_name_number = 0;
  file_name_allocated = 0;
  file_name_current = 0;
  inside_tile = 0;
  inside_circle = 0;
  inside_rectangle = 0;
  filter = 0;
  transform = 0;
}

PULSEreadOpener::~PULSEreadOpener()
{
  if (file_names)
  {
    U32 i;
    for (i = 0; i < file_name_number; i++) free(file_names[i]);
    free(file_names);
  }
  if (scale_factor) delete [] scale_factor;
  if (offset) delete [] offset;
  if (inside_tile) delete [] inside_tile;
  if (inside_circle) delete [] inside_circle;
  if (inside_rectangle) delete [] inside_rectangle;
  if (filter) delete filter;
  if (transform) delete transform;
}
