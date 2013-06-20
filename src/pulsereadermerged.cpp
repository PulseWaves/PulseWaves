/*
===============================================================================

  FILE:  pulsereadermerged.cpp
  
  CONTENTS:
  
    see corresponding header file
  
  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2012-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    see corresponding header file
  
===============================================================================
*/
#include "pulsereadermerged.hpp"

#include "pulseindex.hpp"
#include "pulsefilter.hpp"
#include "pulsetransform.hpp"

#include <stdlib.h>
#include <string.h>

BOOL PULSEreaderMerged::add_file_name(const char* file_name)
{
  // do we have a file name
  if (file_name == 0)
  {
    fprintf(stderr, "ERROR: file name pointer is NULL\n");
    return FALSE;
  }
  // does the file exist
  FILE* file = fopen(file_name, "r");
  if (file == 0)
  {
    fprintf(stderr, "ERROR: file '%s' cannot be opened\n", file_name);
    return FALSE;
  }
  fclose(file);
  // check file extension
  if (strstr(file_name, ".pls") || strstr(file_name, ".plz") || strstr(file_name, ".PLS") || strstr(file_name, ".PLZ"))
  {
#ifndef PULSEWAVES_DLL
#ifdef PULSEWAVES_LAS
    if (pulsereaderlas)
    {
      fprintf(stderr, "ERROR: cannot mix LAS with PLS. skipping '%s' ...\n", file_name);
      return FALSE;
    }
#endif // PULSEWAVES_LAS
    if (pulsereadergcw)
    {
      fprintf(stderr, "ERROR: cannot mix GCW with PLS. skipping '%s' ...\n", file_name);
      return FALSE;
    }
    if (pulsereaderlgw)
    {
      fprintf(stderr, "ERROR: cannot mix LGW with PLS. skipping '%s' ...\n", file_name);
      return FALSE;
    }
#endif
    if (pulsereaderpls == 0)
    {
      pulsereader = pulsereaderpls = new PULSEreaderPLS();
    }
  }
#ifndef PULSEWAVES_DLL
#ifdef PULSEWAVES_LAS
  else if (strstr(file_name, ".las") || strstr(file_name, ".laz") || strstr(file_name, ".LAS") || strstr(file_name, ".LAZ"))
  {
    if (pulsereaderpls)
    {
      fprintf(stderr, "ERROR: cannot mix PLS with LAS. skipping '%s' ...\n", file_name);
      return FALSE;
    }
    if (pulsereadergcw)
    {
      fprintf(stderr, "ERROR: cannot mix GCW with LAS. skipping '%s' ...\n", file_name);
      return FALSE;
    }
    if (pulsereaderlgw)
    {
      fprintf(stderr, "ERROR: cannot mix LGW with LAS. skipping '%s' ...\n", file_name);
      return FALSE;
    }
    if (pulsereaderlas == 0)
    {
      pulsereader = pulsereaderlas = new PULSEreaderLAS();
    }
  }
#endif // PULSEWAVES_LAS
  else if (strstr(file_name, ".gcw") || strstr(file_name, ".GCW"))
  {
    if (pulsereaderpls)
    {
      fprintf(stderr, "ERROR: cannot mix PLS with GCW. skipping '%s' ...\n", file_name);
      return FALSE;
    }
#ifdef PULSEWAVES_LAS
    if (pulsereaderlas)
    {
      fprintf(stderr, "ERROR: cannot mix LAS with GCW. skipping '%s' ...\n", file_name);
      return FALSE;
    }
#endif // PULSEWAVES_LAS
    if (pulsereaderlgw)
    {
      fprintf(stderr, "ERROR: cannot mix LGW with GCW. skipping '%s' ...\n", file_name);
      return FALSE;
    }
    if (pulsereadergcw == 0)
    {
      pulsereader = pulsereadergcw = new PULSEreaderGCW();
    }
  }
  else if (strstr(file_name, ".lgw") || strstr(file_name, ".LGW"))
  {
    if (pulsereaderpls)
    {
      fprintf(stderr, "ERROR: cannot mix PLS with LGW. skipping '%s' ...\n", file_name);
      return FALSE;
    }
#ifdef PULSEWAVES_LAS
    if (pulsereaderlas)
    {
      fprintf(stderr, "ERROR: cannot mix LAS with LGW. skipping '%s' ...\n", file_name);
      return FALSE;
    }
#endif // PULSEWAVES_LAS
    if (pulsereadergcw)
    {
      fprintf(stderr, "ERROR: cannot mix GCW with LGW. skipping '%s' ...\n", file_name);
      return FALSE;
    }
    if (pulsereaderlgw == 0)
    {
      pulsereader = pulsereaderlgw = new PULSEreaderLGW();
    }
  }
#endif
  else
  {
    fprintf(stderr, "ERROR: unknown format. skipping '%s' ...\n", file_name);
    return FALSE;
  }
  // add the file
  if (file_name_number == file_name_allocated)
  {
    file_name_allocated += 1024;
    if (file_names)
    {
      file_names = (char**)realloc(file_names, sizeof(char*)*file_name_allocated);
    }
    else
    {
      file_names = (char**)malloc(sizeof(char*)*file_name_allocated);
    }
    if (file_names == 0)
    {
      fprintf(stderr, "ERROR: alloc for file_names pointer array failed at %d\n", file_name_allocated);
      return FALSE;
    }
  }
  file_names[file_name_number] = strdup(file_name);
  file_name_number++;
  return TRUE;
}

void PULSEreaderMerged::set_scale_factor(const F64* scale_factor)
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

void PULSEreaderMerged::set_offset(const F64* offset)
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

void PULSEreaderMerged::set_files_are_flightlines(BOOL files_are_flightlines)
{
  this->files_are_flightlines = files_are_flightlines;
}

BOOL PULSEreaderMerged::open()
{
  if (file_name_number == 0)
  {
    fprintf(stderr, "ERROR: no valid file names\n");
    return FALSE;
  }

  // allocate space for the individual bounding_boxes
  if (bounding_boxes) delete [] bounding_boxes;
  bounding_boxes = new F64[file_name_number*4];

  // clean  header
  header.clean();

  // combine all headers

  U32 i;
  for (i = 0; i < file_name_number; i++)
  {
    // open the pulsereader with the next file name
    if (pulsereaderpls)
    {
      if (!pulsereaderpls->open(file_names[i]))
      {
        fprintf(stderr, "ERROR: could not open pulsereaderpls for file '%s'\n", file_names[i]);
        return FALSE;
      }
    }
#ifndef PULSEWAVES_DLL
#ifdef PULSEWAVES_LAS
    else if (pulsereaderlas)
    {
      if (!pulsereaderlas->open(file_names[i]))
      {
        fprintf(stderr, "ERROR: could not open pulsereaderlas for file '%s'\n", file_names[i]);
        return FALSE;
      }
    }
#endif // PULSEWAVES_LAS
    else if (pulsereadergcw)
    {
      if (!pulsereadergcw->open(file_names[i]))
      {
        fprintf(stderr, "ERROR: could not open pulsereadergcw for file '%s'\n", file_names[i]);
        return FALSE;
      }
    }
    else if (pulsereaderlgw)
    {
      if (!pulsereaderlgw->open(file_names[i]))
      {
        fprintf(stderr, "ERROR: could not open pulsereaderlgw for file '%s'\n", file_names[i]);
        return FALSE;
      }
    }
#endif
    else
    {
      fprintf(stderr, "ERROR: no pulsereader for '%s'\n", file_names[i]);
      return FALSE;
    }

    // record individual bounding box info
    bounding_boxes[4*i+0] = pulsereader->header.min_x;
    bounding_boxes[4*i+1] = pulsereader->header.min_y;
    bounding_boxes[4*i+2] = pulsereader->header.max_x;
    bounding_boxes[4*i+3] = pulsereader->header.max_y;
    // populate the merged header
    if (i == 0)
    {
      // use the header info from the first file
      header = pulsereader->header;
      // zero the pointers of the other header so they don't get deallocated twice
      pulsereader->header.user_data_in_header = 0;
      pulsereader->header.vlrs = 0;
      pulsereader->header.avlrs = 0;
      pulsereader->header.pulsezip = 0;
      pulsereader->header.user_data_after_header = 0;
      // count the total number of pulses
      npulses = header.number_of_pulses;
      // special check for extra attributes
      if (header.number_extra_attributes)
      {
        header.number_extra_attributes = 0;
        header.init_extra_attributes(pulsereader->header.number_extra_attributes, pulsereader->header.extra_attributes);
      }
    }
    else
    {
      // count the total number of pulses
      npulses += pulsereader->header.number_of_pulses;
      // but increment point counters and widen the bounding box
      header.number_of_pulses += pulsereader->header.number_of_pulses;
      if (header.max_x < pulsereader->header.max_x) header.max_x = pulsereader->header.max_x;
      if (header.max_y < pulsereader->header.max_y) header.max_y = pulsereader->header.max_y;
      if (header.max_z < pulsereader->header.max_z) header.max_z = pulsereader->header.max_z;
      if (header.min_x > pulsereader->header.min_x) header.min_x = pulsereader->header.min_x;
      if (header.min_y > pulsereader->header.min_y) header.min_y = pulsereader->header.min_y;
      if (header.min_z > pulsereader->header.min_z) header.min_z = pulsereader->header.min_z;
      // a pulse format change could be problematic
      if (header.pulse_format != pulsereader->header.pulse_format)
      {
        if (!pulse_format_change) fprintf(stderr, "WARNING: files have different pulse formats: %d vs %d\n", header.pulse_format, pulsereader->header.pulse_format);
        pulse_format_change = TRUE;
      }
      // a pulse atttribute change could be problematic
      if (header.pulse_attributes != pulsereader->header.pulse_attributes)
      {
        if (!pulse_attribute_change) fprintf(stderr, "WARNING: files have different pulse attributes: %d vs %d\n", header.pulse_attributes, pulsereader->header.pulse_attributes);
        pulse_attribute_change = TRUE;
      }
      // a pulse size change could be problematic
      if (header.pulse_size != pulsereader->header.pulse_size)
      {
        if (!pulse_size_change) fprintf(stderr, "WARNING: files have different pulse sizes: %d vs %d\n", header.pulse_size, pulsereader->header.pulse_size);
        pulse_size_change = TRUE;
      }
      // and check if we need to requantize anchor and target points because scalefactor of offsets change
      if (header.x_scale_factor != pulsereader->header.x_scale_factor ||
          header.y_scale_factor != pulsereader->header.y_scale_factor ||
          header.z_scale_factor != pulsereader->header.z_scale_factor)
      {
//        if (!rescale) fprintf(stderr, "WARNING: files have different scale factors: %g %g %g vs %g %g %g\n", header.x_scale_factor, header.y_scale_factor, header.z_scale_factor, pulsereader->header.x_scale_factor, pulsereader->header.y_scale_factor, pulsereader->header.z_scale_factor);
        rescale = TRUE;
      }
      if (header.x_offset != pulsereader->header.x_offset ||
          header.y_offset != pulsereader->header.y_offset ||
          header.z_offset != pulsereader->header.z_offset)
      {
//        if (!reoffset) fprintf(stderr, "WARNING: files have different offsets: %g %g %g vs %g %g %g\n", header.x_offset, header.y_offset, header.z_offset, pulsereader->header.x_offset, pulsereader->header.y_offset, pulsereader->header.z_offset);
        reoffset = TRUE;
      }
    }
    pulsereader->close();
  }

  // was it requested to rescale or reoffset

  if (scale_factor)
  {
    if (scale_factor[0] && (header.x_scale_factor != scale_factor[0]))
    {
      header.x_scale_factor = scale_factor[0];
      rescale = TRUE;
    }
    if (scale_factor[1] && (header.y_scale_factor != scale_factor[1]))
    {
      header.y_scale_factor = scale_factor[1];
      rescale = TRUE;
    }
    if (scale_factor[2] && (header.z_scale_factor != scale_factor[2]))
    {
      header.z_scale_factor = scale_factor[2];
      rescale = TRUE;
    }
  }
  if (offset)
  {
    if (header.x_offset != offset[0])
    {
      header.x_offset = offset[0];
      reoffset = TRUE;
    }
    if (header.y_offset != offset[1])
    {
      header.y_offset = offset[1];
      reoffset = TRUE;
    }
    if (header.z_offset != offset[2])
    {
      header.z_offset = offset[2];
      reoffset = TRUE;
    }
  }

  // initialize the pulse with the header info

  if (header.pulsezip)
  {
    if (!pulse.init(&header, header.pulsezip->num_items, header.pulsezip->items)) return FALSE;
  }
  else
  {
    if (!pulse.init(&header)) return FALSE;
  }

  // check if the header can support the enlarged bounding box

  // check x

  if ((((header.max_x - header.x_offset) / header.x_scale_factor) > I32_MAX) || (((header.min_x - header.x_offset) / header.x_scale_factor) < I32_MIN))
  {
    // maybe we can fix it by adjusting the offset (and if needed by lowering the resolution via the scale factor)
    F64 x_offset = (F64)I64_QUANTIZE((header.min_x + header.max_x)/2);
    F64 x_scale_factor = header.x_scale_factor;
    while ((((header.max_x - x_offset) / x_scale_factor) > I32_MAX) || (((header.min_x - x_offset) / x_scale_factor) < I32_MIN))
    {
      x_scale_factor *= 10;
    }
    if (header.x_scale_factor != x_scale_factor)
    {
      fprintf(stderr, "WARNING: i changed x_scale_factor from %g to %g to accommodate enlarged bounding box\n", header.x_scale_factor, x_scale_factor);
      header.x_scale_factor = x_scale_factor;
      rescale = TRUE;
    }
    // maybe we changed the resolution ... so do we really need to adjuste the offset
    if ((((header.max_x - header.x_offset) / x_scale_factor) > I32_MAX) || (((header.min_x - header.x_offset) / x_scale_factor) < I32_MIN))
    {
      fprintf(stderr, "WARNING: i changed x_offset from %g to %g to accommodate enlarged bounding box\n", header.x_offset, x_offset);
      header.x_offset = x_offset;
      reoffset = TRUE;
    }
  }
    
  // check y

  if ((((header.max_y - header.y_offset) / header.y_scale_factor) > I32_MAX) || (((header.min_y - header.y_offset) / header.y_scale_factor) < I32_MIN))
  {
    // maybe we can fix it by adjusting the offset (and if needed by lowering the resolution via the scale factor)
    F64 y_offset = (F64)I64_QUANTIZE((header.min_y + header.max_y)/2);
    F64 y_scale_factor = header.y_scale_factor;
    while ((((header.max_y - y_offset) / y_scale_factor) > I32_MAX) || (((header.min_y - y_offset) / y_scale_factor) < I32_MIN))
    {
      y_scale_factor *= 10;
    }
    if (header.y_scale_factor != y_scale_factor)
    {
      fprintf(stderr, "WARNING: i changed y_scale_factor from %g to %g to accommodate enlarged bounding box\n", header.y_scale_factor, y_scale_factor);
      header.y_scale_factor = y_scale_factor;
      rescale = TRUE;
    }
    // maybe we changed the resolution ... so do we really need to adjuste the offset
    if ((((header.max_y - header.y_offset) / y_scale_factor) > I32_MAX) || (((header.min_y - header.y_offset) / y_scale_factor) < I32_MIN))
    {
      fprintf(stderr, "WARNING: i changed y_offset from %g to %g to accommodate enlarged bounding box\n", header.y_offset, y_offset);
      header.y_offset = y_offset;
      reoffset = TRUE;
    }
  }
    
  // check z

  if ((((header.max_z - header.z_offset) / header.z_scale_factor) > I32_MAX) || (((header.min_z - header.z_offset) / header.z_scale_factor) < I32_MIN))
  {
    // maybe we can fix it by adjusting the offset (and if needed by lowering the resolution via the scale factor)
    F64 z_offset = (F64)I64_QUANTIZE((header.min_z + header.max_z)/2);
    F64 z_scale_factor = header.z_scale_factor;
    while ((((header.max_z - z_offset) / z_scale_factor) > I32_MAX) || (((header.min_z - z_offset) / z_scale_factor) < I32_MIN))
    {
      z_scale_factor *= 10;
    }
    if (header.z_scale_factor != z_scale_factor)
    {
      fprintf(stderr, "WARNING: i changed  z_scale_factor from %g to %g to accommodate enlarged bounding box\n", header.z_scale_factor, z_scale_factor);
      header.z_scale_factor = z_scale_factor;
      rescale = TRUE;
    }
    // maybe we changed the resolution ... so do we really need to adjuste the offset
    if ((((header.max_z - header.z_offset) / z_scale_factor) > I32_MAX) || (((header.min_z - header.z_offset) / z_scale_factor) < I32_MIN))
    {
      fprintf(stderr, "WARNING: i changed z_offset from %g to %g to accommodate enlarged bounding box\n", header.z_offset, z_offset);
      header.z_offset = z_offset;
      reoffset = TRUE;
    }
  }


  if (rescale || reoffset)
  {
    fprintf(stderr, "ERROR: rescale and reoffset not implemented for PULSEreaderMerged\n");
    return FALSE;
  }

/*
    if (pulsereaderpls)
    {
      delete pulsereaderpls;
      if (rescale && reoffset)
        pulsereaderpls = new PULSEreaderPLSrescalereoffset(header.x_scale_factor, header.y_scale_factor, header.z_scale_factor, header.x_offset, header.y_offset, header.z_offset);
      else if (rescale)
        pulsereaderpls = new PULSEreaderPLSrescale(header.x_scale_factor, header.y_scale_factor, header.z_scale_factor);
      else
        pulsereaderpls = new PULSEreaderPLSreoffset(header.x_offset, header.y_offset, header.z_offset);
      pulsereader = pulsereaderpls;
    }
  }
*/

  p_count = 0;
  file_name_current = 0;

  return TRUE;
}

void PULSEreaderMerged::set_filter(PULSEfilter* filter)
{
  this->filter = filter;
}

void PULSEreaderMerged::set_transform(PULSEtransform* transform)
{
  this->transform = transform;
}

BOOL PULSEreaderMerged::inside_tile(const F32 ll_x, const F32 ll_y, const F32 size)
{
  inside = 1;
  t_ll_x = ll_x;
  t_ll_y = ll_y;
  t_size = size;
  t_ur_x = ll_x + size;
  t_ur_y = ll_y + size;
  header.min_x = ll_x;
  header.min_y = ll_y;
  header.max_x = ll_x + size - 0.001f * header.x_scale_factor;
  header.max_y = ll_y + size - 0.001f * header.y_scale_factor;
  return TRUE;
}

BOOL PULSEreaderMerged::inside_circle(const F64 center_x, const F64 center_y, const F64 radius)
{
  inside = 2;
  c_center_x = center_x;
  c_center_y = center_y;
  c_radius = radius;
  c_radius_squared = radius*radius;
  header.min_x = center_x - radius;
  header.min_y = center_y - radius;
  header.max_x = center_x + radius;
  header.max_y = center_y + radius;
  return TRUE;
}

BOOL PULSEreaderMerged::inside_rectangle(const F64 min_x, const F64 min_y, const F64 max_x, const F64 max_y)
{
  inside = 3;
  r_min_x = min_x;
  r_min_y = min_y;
  r_max_x = max_x;
  r_max_y = max_y;
  header.min_x = min_x;
  header.min_y = min_y;
  header.max_x = max_x;
  header.max_y = max_y;
  return TRUE;
}

I32 PULSEreaderMerged::get_format() const
{
  return pulsereader->get_format();
}

BOOL PULSEreaderMerged::read_pulse_default()
{
  if (file_name_current == 0)
  {
    if (!open_next_file()) return FALSE;
  }

  while (true)
  {
    if (pulsereader->read_pulse())
    {
      pulse = pulsereader->pulse;
      p_count++;
      return TRUE;
    }
    if (pulsereaderpls == 0)
    {
      if (header.max_x < pulsereader->header.max_x) header.max_x = pulsereader->header.max_x;
      if (header.max_y < pulsereader->header.max_y) header.max_y = pulsereader->header.max_y;
      if (header.max_z < pulsereader->header.max_z) header.max_z = pulsereader->header.max_z;
      if (header.min_x > pulsereader->header.min_x) header.min_x = pulsereader->header.min_x;
      if (header.min_y > pulsereader->header.min_y) header.min_y = pulsereader->header.min_y;
      if (header.min_z > pulsereader->header.min_z) header.min_z = pulsereader->header.min_z;
    }
    pulsereader->close();
    pulse.zero();
    if (!open_next_file()) return FALSE;
  }
  return FALSE;
}

BOOL PULSEreaderMerged::read_waves()
{
  // do we have a pulse reader
  if (pulsereader)
  {
    return pulsereader->read_waves();
  }
  return FALSE;
}

void PULSEreaderMerged::close(BOOL close_stream)
{
  if (pulsereader) 
  {
    pulsereader->close(close_stream);
  }
}

BOOL PULSEreaderMerged::reopen()
{
  p_count = 0;
  file_name_current = 0;
  if (filter) filter->reset();
  return TRUE;
}

void PULSEreaderMerged::clean()
{
  if (pulsereader) 
  {
    delete pulsereader;
    pulsereader = 0;
    pulsereaderpls = 0;
#ifndef PULSEWAVES_DLL
#ifdef PULSEWAVES_LAS
    pulsereaderlas = 0;
#endif // PULSEWAVES_LAS
    pulsereadergcw = 0;
    pulsereaderlgw = 0;
#endif
  }
  pulse_format_change = FALSE;
  pulse_size_change = FALSE;
  rescale = FALSE;
  reoffset = FALSE;
  if (scale_factor)
  {
    delete [] scale_factor;
    scale_factor = 0;
  }
  if (offset)
  {
    delete [] offset;
    offset = 0;
  }
  if (file_names)
  {
    U32 i;
    for (i = 0; i < file_name_number; i++)
    {
      free(file_names[i]);
    }
    delete [] file_names;
    file_names = 0;
  }
  if (bounding_boxes)
  {
    delete [] bounding_boxes;
    bounding_boxes = 0;
  }
  file_name_current = 0;
  file_name_number = 0;
  file_name_allocated = 0;
  inside = 0;
}

PULSEreaderMerged::PULSEreaderMerged()
{
  pulsereader = 0;
  pulsereaderpls = 0;
#ifndef PULSEWAVES_DLL
#ifdef PULSEWAVES_LAS
  pulsereaderlas = 0;
#endif // PULSEWAVES_LAS
  pulsereadergcw = 0;
  pulsereaderlgw = 0;
#endif
  scale_factor = 0;
  offset = 0;
  files_are_flightlines = FALSE;
  file_names = 0;
  bounding_boxes = 0;
  clean();
}

PULSEreaderMerged::~PULSEreaderMerged()
{
  if (pulsereader) close();
  clean();
}

BOOL PULSEreaderMerged::open_next_file()
{
  while (file_name_current < file_name_number)
  {
    if (inside)
    {
      // check if bounding box overlaps requested bounding box
      if (inside < 3) // tile or circle
      {
        if (bounding_boxes[4*file_name_current+0] >= header.max_x) { file_name_current++; continue; }
        if (bounding_boxes[4*file_name_current+1] >= header.max_y) { file_name_current++; continue; }
      }
      else // rectangle
      {
        if (bounding_boxes[4*file_name_current+0] > header.max_x) { file_name_current++; continue; }
        if (bounding_boxes[4*file_name_current+1] > header.max_y) { file_name_current++; continue; }
      }
      if (bounding_boxes[4*file_name_current+2] < header.min_x) { file_name_current++; continue; }
      if (bounding_boxes[4*file_name_current+3] < header.min_y) { file_name_current++; continue; }
    }
    // open the pulsereader with the next file name
    if (pulsereaderpls)
    {
      if (!pulsereaderpls->open(file_names[file_name_current]))
      {
        fprintf(stderr, "ERROR: could not open pulsereaderpls for file '%s'\n", file_names[file_name_current]);
        return FALSE;
      }
      PULSEindex* index = new PULSEindex;
      if (index->read(file_names[file_name_current]))
        pulsereaderpls->set_index(index);
      else
        delete index;
    }
#ifndef PULSEWAVES_DLL
#ifdef PULSEWAVES_LAS
    else if (pulsereaderlas)
    {
      if (!pulsereaderlas->open(file_names[file_name_current]))
      {
        fprintf(stderr, "ERROR: could not open pulsereaderlas for file '%s'\n", file_names[file_name_current]);
        return FALSE;
      }
      PULSEindex* index = new PULSEindex;
      if (index->read(file_names[file_name_current]))
        pulsereaderlas->set_index(index);
      else
        delete index;
    }
#endif // PULSEWAVES_LAS
    else if (pulsereadergcw)
    {
      if (!pulsereadergcw->open(file_names[file_name_current]))
      {
        fprintf(stderr, "ERROR: could not open pulsereadergcw for file '%s'\n", file_names[file_name_current]);
        return FALSE;
      }
      PULSEindex* index = new PULSEindex;
      if (index->read(file_names[file_name_current]))
        pulsereadergcw->set_index(index);
      else
        delete index;
    }
    else if (pulsereaderlgw)
    {
      if (!pulsereaderlgw->open(file_names[file_name_current]))
      {
        fprintf(stderr, "ERROR: could not open pulsereaderlgw for file '%s'\n", file_names[file_name_current]);
        return FALSE;
      }
      PULSEindex* index = new PULSEindex;
      if (index->read(file_names[file_name_current]))
        pulsereaderlgw->set_index(index);
      else
        delete index;
    }
#endif
    else
    {
      fprintf(stderr, "ERROR: no pulsereader for '%s'\n", file_names[file_name_current]);
      return FALSE;
    }

    file_name_current++;
    if (files_are_flightlines)
    {
//      transform->setPulseSource(file_name_current);
    }
    if (filter) pulsereader->set_filter(filter);
    if (transform) pulsereader->set_transform(transform);
    if (inside)
    {
      if (inside == 1) pulsereader->inside_tile(t_ll_x, t_ll_y, t_size);
      else if (inside == 2) pulsereader->inside_circle(c_center_x, c_center_y, c_radius);
      else pulsereader->inside_rectangle(r_min_x, r_min_y, r_max_x, r_max_y);
    }
    return TRUE;
  }
  return FALSE;
}
