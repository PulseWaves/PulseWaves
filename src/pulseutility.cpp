/*
===============================================================================

  FILE:  pulseutility.cpp
  
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
#include "pulseutility.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PULSEinventory::PULSEinventory()
{
  number_of_pulses = 0;
  min_T = max_T = 0;
  min_x = max_x = 0.0;
  min_y = max_y = 0.0;
  min_z = max_z = 0.0;
  first = TRUE;
  first_returning = TRUE;
}

BOOL PULSEinventory::add(const PULSEpulse* pulse, BOOL only_count)
{
  BOOL has_returning_waveform = (pulse->first_returning_sample || pulse->last_returning_sample);
  number_of_pulses++;
  if (!only_count)
  {
    if (first)
    {
      min_T = max_T = pulse->T;
      first = FALSE;
    }
    else
    {
      if (pulse->T < min_T) min_T = pulse->T;
      else if (pulse->T > max_T) max_T = pulse->T;
    }
    // special handling as there may be pulses without returning waveforms
    if (first_returning)
    {
      if (has_returning_waveform)
      {
        min_x = max_x = pulse->first[0];
        min_y = max_y = pulse->first[1];
        min_z = max_z = pulse->first[2];
        first_returning = FALSE;
      }
      else
      {
        return TRUE;
      }
    }
    else
    {
      if (has_returning_waveform)
      {
        if (pulse->first[0] < min_x) min_x = pulse->first[0];
        else if (pulse->first[0] > max_x) max_x = pulse->first[0];
        if (pulse->first[1] < min_y) min_y = pulse->first[1];
        else if (pulse->first[1] > max_y) max_y = pulse->first[1];
        if (pulse->first[2] < min_z) min_z = pulse->first[2];
        else if (pulse->first[2] > max_z) max_z = pulse->first[2];
      }
      else
      {
        return TRUE;
      }
    }
    if (pulse->last[0] < min_x) min_x = pulse->last[0];
    else if (pulse->last[0] > max_x) max_x = pulse->last[0];
    if (pulse->last[1] < min_y) min_y = pulse->last[1];
    else if (pulse->last[1] > max_y) max_y = pulse->last[1];
    if (pulse->last[2] < min_z) min_z = pulse->last[2];
    else if (pulse->last[2] > max_z) max_z = pulse->last[2];
  }
  return TRUE;
}

PULSEsummary::PULSEsummary()
{
  number_of_pulses = 0;
  min_x = max_x = 0.0;
  min_y = max_y = 0.0;
  min_z = max_z = 0.0;
  first = TRUE;
  first_returning = TRUE;
}

BOOL PULSEsummary::add(const PULSEpulse* pulse)
{
  BOOL has_returning_waveform = (pulse->first_returning_sample || pulse->last_returning_sample);
  number_of_pulses++;
  if (first)
  {
    min = *pulse;
    max = *pulse;
    first = FALSE;
  }
  else
  {
    if (pulse->T < min.T) min.T = pulse->T;
    else if (pulse->T > max.T) max.T = pulse->T;
    if (pulse->offset < min.offset) min.offset = pulse->offset;
    else if (pulse->offset > max.offset) max.offset = pulse->offset;
    if (pulse->anchor_X < min.anchor_X) min.anchor_X = pulse->anchor_X;
    else if (pulse->anchor_X > max.anchor_X) max.anchor_X = pulse->anchor_X;
    if (pulse->anchor_Y < min.anchor_Y) min.anchor_Y = pulse->anchor_Y;
    else if (pulse->anchor_Y > max.anchor_Y) max.anchor_Y = pulse->anchor_Y;
    if (pulse->anchor_Z < min.anchor_Z) min.anchor_Z = pulse->anchor_Z;
    else if (pulse->anchor_Z > max.anchor_Z) max.anchor_Z = pulse->anchor_Z;
    if (pulse->target_X < min.target_X) min.target_X = pulse->target_X;
    else if (pulse->target_X > max.target_X) max.target_X = pulse->target_X;
    if (pulse->target_Y < min.target_Y) min.target_Y = pulse->target_Y;
    else if (pulse->target_Y > max.target_Y) max.target_Y = pulse->target_Y;
    if (pulse->target_Z < min.target_Z) min.target_Z = pulse->target_Z;
    else if (pulse->target_Z > max.target_Z) max.target_Z = pulse->target_Z;
    if (pulse->descriptor_index < min.descriptor_index) min.descriptor_index = pulse->descriptor_index;
    else if (pulse->descriptor_index > max.descriptor_index) max.descriptor_index = pulse->descriptor_index;
    if (pulse->reserved < min.reserved) min.reserved = pulse->reserved;
    else if (pulse->reserved > max.reserved) max.reserved = pulse->reserved;
    if (pulse->edge_of_scan_line < min.edge_of_scan_line) min.edge_of_scan_line = pulse->edge_of_scan_line;
    else if (pulse->edge_of_scan_line > max.edge_of_scan_line) max.edge_of_scan_line = pulse->edge_of_scan_line;
    if (pulse->scan_direction < min.scan_direction) min.scan_direction = pulse->scan_direction;
    else if (pulse->scan_direction > max.scan_direction) max.scan_direction = pulse->scan_direction;
    if (pulse->mirror_facet < min.mirror_facet) min.mirror_facet = pulse->mirror_facet;
    else if (pulse->mirror_facet > max.mirror_facet) max.mirror_facet = pulse->mirror_facet;
    if (pulse->intensity < min.intensity) min.intensity = pulse->intensity;
    else if (pulse->intensity > max.intensity) max.intensity = pulse->intensity;
    if (pulse->classification < min.classification) min.classification = pulse->classification;
    else if (pulse->classification > max.classification) max.classification = pulse->classification;
  }
  // special handling as there may be pulses without returning waveforms
  if (first_returning)
  {
	  if (has_returning_waveform)
	  {
      min.first_returning_sample = max.first_returning_sample = pulse->first_returning_sample;
      min.last_returning_sample = max.last_returning_sample = pulse->last_returning_sample;
      min_x = max_x = pulse->first[0];
      min_y = max_y = pulse->first[1];
      min_z = max_z = pulse->first[2];
      first_returning = FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    if (has_returning_waveform)
    {
      if (pulse->first_returning_sample < min.first_returning_sample) min.first_returning_sample = pulse->first_returning_sample;
      else if (pulse->first_returning_sample > max.first_returning_sample) max.first_returning_sample = pulse->first_returning_sample;
      if (pulse->last_returning_sample < min.last_returning_sample) min.last_returning_sample = pulse->last_returning_sample;
      else if (pulse->last_returning_sample > max.last_returning_sample) max.last_returning_sample = pulse->last_returning_sample;
      if (pulse->first[0] < min_x) min_x = pulse->first[0];
      else if (pulse->first[0] > max_x) max_x = pulse->first[0];
      if (pulse->first[1] < min_y) min_y = pulse->first[1];
      else if (pulse->first[1] > max_y) max_y = pulse->first[1];
      if (pulse->first[2] < min_z) min_z = pulse->first[2];
      else if (pulse->first[2] > max_z) max_z = pulse->first[2];
    }
    else
    {
      return TRUE;
    }
  }
  if (pulse->last[0] < min_x) min_x = pulse->last[0];
  else if (pulse->last[0] > max_x) max_x = pulse->last[0];
  if (pulse->last[1] < min_y) min_y = pulse->last[1];
  else if (pulse->last[1] > max_y) max_y = pulse->last[1];
  if (pulse->last[2] < min_z) min_z = pulse->last[2];
  else if (pulse->last[2] > max_z) max_z = pulse->last[2];
  return TRUE;
}

PULSEbin::PULSEbin(F32 step)
{
  total = 0;
  count = 0;
  this->one_over_step = 1.0f/step;
  first = TRUE;
  size_pos = 0;
  size_neg = 0;
  bins_pos = 0;
  bins_neg = 0;
  values_pos = 0;
  values_neg = 0;
}

PULSEbin::~PULSEbin()
{
  if (bins_pos) free(bins_pos);
  if (bins_neg) free(bins_neg);
  if (values_pos) free(values_pos);
  if (values_neg) free(values_neg);
}

void PULSEbin::add(I32 item)
{
  total += item;
  count++;
  I32 bin = I32_FLOOR(one_over_step*item);
  add_to_bin(bin);
}

void PULSEbin::add(F64 item)
{
  total += item;
  count++;
  I32 bin = I32_FLOOR(one_over_step*item);
  add_to_bin(bin);
}

void PULSEbin::add(I64 item)
{
  total += item;
  count++;
  I32 bin = I32_FLOOR(one_over_step*item);
  add_to_bin(bin);
}

void PULSEbin::add_to_bin(I32 bin)
{
  if (first)
  {
    anker = bin;
    first = FALSE;
  }
  bin = bin - anker;
  if (bin >= 0)
  {
    if (bin >= size_pos)
    {
      I32 i;
      if (size_pos == 0)
      {
        size_pos = bin + 1024;
        bins_pos = (U32*)malloc(sizeof(U32)*size_pos);
        for (i = 0; i < size_pos; i++) bins_pos[i] = 0;
      }
      else
      {
        I32 new_size = bin + 1024;
        bins_pos = (U32*)realloc(bins_pos, sizeof(U32)*new_size);
        for (i = size_pos; i < new_size; i++) bins_pos[i] = 0;
        size_pos = new_size;
      }
    }
    bins_pos[bin]++;
  }
  else
  {
    bin = -(bin+1);
    if (bin >= size_neg)
    {
      I32 i;
      if (size_neg == 0)
      {
        size_neg = bin + 1024;
        bins_neg = (U32*)malloc(sizeof(U32)*size_neg);
        for (i = 0; i < size_neg; i++) bins_neg[i] = 0;
      }
      else
      {
        I32 new_size = bin + 1024;
        bins_neg = (U32*)realloc(bins_neg, sizeof(U32)*new_size);
        for (i = size_neg; i < new_size; i++) bins_neg[i] = 0;
        size_neg = new_size;
      }
    }
    bins_neg[bin]++;
  }
}

void PULSEbin::add(I32 item, I32 value)
{
  total += item;
  count++;
  I32 bin = I32_FLOOR(one_over_step*item);
  if (first)
  {
    anker = bin;
    first = FALSE;
  }
  bin = bin - anker;
  if (bin >= 0)
  {
    if (bin >= size_pos)
    {
      I32 i;
      if (size_pos == 0)
      {
        size_pos = 1024;
        bins_pos = (U32*)malloc(sizeof(U32)*size_pos);
        values_pos = (F64*)malloc(sizeof(F64)*size_pos);
        for (i = 0; i < size_pos; i++) { bins_pos[i] = 0; values_pos[i] = 0; }
      }
      else
      {
        I32 new_size = bin + 1024;
        bins_pos = (U32*)realloc(bins_pos, sizeof(U32)*new_size);
        values_pos = (F64*)realloc(values_pos, sizeof(F64)*new_size);
        for (i = size_pos; i < new_size; i++) { bins_pos[i] = 0; values_pos[i] = 0; }
        size_pos = new_size;
      }
    }
    bins_pos[bin]++;
    values_pos[bin] += value;
  }
  else
  {
    bin = -(bin+1);
    if (bin >= size_neg)
    {
      I32 i;
      if (size_neg == 0)
      {
        size_neg = 1024;
        bins_neg = (U32*)malloc(sizeof(U32)*size_neg);
        values_neg = (F64*)malloc(sizeof(F64)*size_pos);
        for (i = 0; i < size_neg; i++) { bins_neg[i] = 0; values_neg[i] = 0; }
      }
      else
      {
        I32 new_size = bin + 1024;
        bins_neg = (U32*)realloc(bins_neg, sizeof(U32)*new_size);
        values_neg = (F64*)realloc(values_neg, sizeof(F64)*new_size);
        for (i = size_neg; i < new_size; i++) { bins_neg[i] = 0; values_neg[i] = 0; }
        size_neg = new_size;
      }
    }
    bins_neg[bin]++;
    values_neg[bin] += value;
  }
}

void PULSEbin::report(FILE* file, const char* name, const char* name_avg) const
{
  I32 i, bin;
  if (name)
  {
    if (values_pos)
    {
      if (name_avg)
        fprintf(file, "%s histogram of %s averages with bin size %g\012", name, name_avg, 1.0f/one_over_step);
      else
        fprintf(file, "%s histogram of averages with bin size %g\012", name, 1.0f/one_over_step);
    }
    else
      fprintf(file, "%s histogram with bin size %g\012", name, 1.0f/one_over_step);
  }
  if (size_neg)
  {
    for (i = size_neg-1; i >= 0; i--)
    {
      if (bins_neg[i])
      {
        bin = -(i+1) + anker;
        if (one_over_step == 1)
        {
          if (values_neg)
            fprintf(file, "  bin %d has average %g (of %d)\012", bin, values_neg[i]/bins_neg[i], bins_neg[i]);
          else
            fprintf(file, "  bin %d has %d\012", bin, bins_neg[i]);
        }
        else
        {
          if (values_neg)
            fprintf(file, "  bin [%g,%g) has average %g (of %d)\012", ((F32)bin)/one_over_step, ((F32)(bin+1))/one_over_step, values_neg[i]/bins_neg[i], bins_neg[i]);
          else
            fprintf(file, "  bin [%g,%g) has %d\012", ((F32)bin)/one_over_step, ((F32)(bin+1))/one_over_step, bins_neg[i]);
        }
      }
    }
  }
  if (size_pos)
  {
    for (i = 0; i < size_pos; i++)
    {
      if (bins_pos[i])
      {
        bin = i + anker;
        if (one_over_step == 1)
        {
          if (values_pos)
            fprintf(file, "  bin %d has average %g (of %d)\012", bin, values_pos[i]/bins_pos[i], bins_pos[i]);
          else
            fprintf(file, "  bin %d has %d\012", bin, bins_pos[i]);
        }
        else
        {
          if (values_pos)
            fprintf(file, "  bin [%g,%g) average has %g (of %d)\012", ((F32)bin)/one_over_step, ((F32)(bin+1))/one_over_step, values_pos[i]/bins_pos[i], bins_pos[i]);
          else
            fprintf(file, "  bin [%g,%g) has %d\012", ((F32)bin)/one_over_step, ((F32)(bin+1))/one_over_step, bins_pos[i]);
        }
      }
    }
  }
  if (name)
    fprintf(file, "  average %s %g\012", name, total/count);
  else
    fprintf(file, "  average %g\012", total/count);
}

PULSEhistogram::PULSEhistogram()
{
  is_active = FALSE;
  // counter bins
  T_bin = 0;
  offset_bin = 0;
  anchor_x_bin = 0;
  anchor_y_bin = 0;
  anchor_z_bin = 0;
  target_x_bin = 0;
  target_y_bin = 0;
  target_z_bin = 0;
  descriptor_bin = 0;
  intensity_bin = 0;
  classification_bin = 0;
  samples_bin = 0;
  anchor_X_bin = 0;
  anchor_Y_bin = 0;
  anchor_Z_bin = 0;
  target_X_bin = 0;
  target_Y_bin = 0;
  target_Z_bin = 0;
}

PULSEhistogram::~PULSEhistogram()
{
  // counter bins
  if (T_bin) delete T_bin;
  if (offset_bin) delete offset_bin;
  if (anchor_x_bin) delete anchor_x_bin;
  if (anchor_y_bin) delete anchor_y_bin;
  if (anchor_z_bin) delete anchor_z_bin;
  if (target_x_bin) delete target_x_bin;
  if (target_y_bin) delete target_y_bin;
  if (target_z_bin) delete target_z_bin;
  if (descriptor_bin) delete descriptor_bin;
  if (intensity_bin) delete intensity_bin;
  if (classification_bin) delete classification_bin;
  if (samples_bin) delete samples_bin;
  if (anchor_X_bin) delete anchor_X_bin;
  if (anchor_Y_bin) delete anchor_Y_bin;
  if (anchor_Z_bin) delete anchor_Z_bin;
  if (target_X_bin) delete target_X_bin;
  if (target_Y_bin) delete target_Y_bin;
  if (target_Z_bin) delete target_Z_bin;
}

BOOL PULSEhistogram::parse(int argc, char* argv[])
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
      return TRUE;
    }
    else if (strcmp(argv[i],"-histo") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: name step\n", argv[i]);
        return FALSE;
      }
      if (!histo(argv[i+1], (F32)atof(argv[i+2]))) return FALSE;
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-histo_avg") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: name step name_avg\n", argv[i]);
        return FALSE;
      }
      if (!histo_avg(argv[i+1], (F32)atof(argv[i+2]), argv[i+3])) return FALSE;
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3; 
    }
  }
  return TRUE;
}

BOOL PULSEhistogram::histo(const char* name, F32 step)
{
  if (strstr(name, "T") != 0)
    T_bin = new PULSEbin(step);
  else if (strstr(name, "offset") != 0)
    offset_bin = new PULSEbin(step);
  else if (strcmp(name, "anchor_x") == 0)
    anchor_x_bin = new PULSEbin(step);
  else if (strcmp(name, "anchor_y") == 0)
    anchor_y_bin = new PULSEbin(step);
  else if (strcmp(name, "anchor_z") == 0)
    anchor_z_bin = new PULSEbin(step);
  else if (strcmp(name, "target_x") == 0)
    target_x_bin = new PULSEbin(step);
  else if (strcmp(name, "target_y") == 0)
    target_y_bin = new PULSEbin(step);
  else if (strcmp(name, "target_z") == 0)
    target_z_bin = new PULSEbin(step);
  else if (strcmp(name, "descriptor") == 0)
    descriptor_bin = new PULSEbin(step);
  else if (strcmp(name, "intensity") == 0)
    intensity_bin = new PULSEbin(step);
  else if (strstr(name, "classification") != 0)
    classification_bin = new PULSEbin(step);
  else if (strstr(name, "samples") != 0)
    samples_bin = new PULSEbin(step);
  else if (strcmp(name, "anchor_X") == 0)
    anchor_X_bin = new PULSEbin(step);
  else if (strcmp(name, "anchor_Y") == 0)
    anchor_Y_bin = new PULSEbin(step);
  else if (strcmp(name, "anchor_Z") == 0)
    anchor_Z_bin = new PULSEbin(step);
  else if (strcmp(name, "target_X") == 0)
    target_X_bin = new PULSEbin(step);
  else if (strcmp(name, "target_Y") == 0)
    target_Y_bin = new PULSEbin(step);
  else if (strcmp(name, "target_Z") == 0)
    target_Z_bin = new PULSEbin(step);
  else 
  {
    fprintf(stderr,"ERROR: histogram of '%s' not implemented\n", name);
    return FALSE;
  }
  is_active = TRUE;
  return TRUE;
}

BOOL PULSEhistogram::histo_avg(const char* name, F32 step, const char* name_avg)
{
  if (strcmp(name, "nada") == 0)
  {
    if (strcmp(name_avg, "nada1") == 0)
      nada_bin_nada1 = new PULSEbin(step);
    else if (strstr(name_avg, "nada2") != 0)
      nada_bin_nada2 = new PULSEbin(step);
    else
    {
      fprintf(stderr,"ERROR: histogram of '%s' with '%s' averages not implemented\n", name, name_avg);
      return FALSE;
    }
  }
  else
  {
    fprintf(stderr,"ERROR: histogram of '%s' not implemented\n", name);
    return FALSE;
  }
  is_active = TRUE;
  return TRUE;
}

void PULSEhistogram::add(const PULSEpulse* pulse)
{
  // counter bins
  if (T_bin) T_bin->add(pulse->T);
  if (offset_bin) offset_bin->add(pulse->offset);
  if (anchor_x_bin) anchor_x_bin->add(pulse->get_anchor_x());
  if (anchor_y_bin) anchor_y_bin->add(pulse->get_anchor_y());
  if (anchor_z_bin) anchor_z_bin->add(pulse->get_anchor_z());
  if (target_x_bin) anchor_x_bin->add(pulse->get_target_x());
  if (target_y_bin) anchor_y_bin->add(pulse->get_target_y());
  if (target_z_bin) anchor_z_bin->add(pulse->get_target_z());
  if (descriptor_bin) descriptor_bin->add(pulse->descriptor_index);
  if (intensity_bin) intensity_bin->add(pulse->intensity);
  if (classification_bin) classification_bin->add(pulse->classification);
  if (samples_bin) samples_bin->add(pulse->last_returning_sample-pulse->first_returning_sample+1);
  if (anchor_X_bin) anchor_X_bin->add(pulse->anchor_X);
  if (anchor_Y_bin) anchor_Y_bin->add(pulse->anchor_Y);
  if (anchor_Z_bin) anchor_Z_bin->add(pulse->anchor_Z);
  if (target_X_bin) target_X_bin->add(pulse->target_X);
  if (target_Y_bin) target_Y_bin->add(pulse->target_Y);
  if (target_Z_bin) target_Z_bin->add(pulse->target_Z);
}

void PULSEhistogram::report(FILE* file) const
{
  // counter bins
  if (T_bin) T_bin->report(file, "raw T timestamp");
  if (offset_bin) offset_bin->report(file, "offset");
  if (anchor_x_bin) anchor_x_bin->report(file, "x of anchor");
  if (anchor_y_bin) anchor_y_bin->report(file, "y of anchor");
  if (anchor_z_bin) anchor_z_bin->report(file, "z of anchor");
  if (target_x_bin) target_x_bin->report(file, "x of target");
  if (target_y_bin) target_y_bin->report(file, "y of target");
  if (target_z_bin) target_z_bin->report(file, "z of target");
  if (descriptor_bin) descriptor_bin->report(file, "descriptor");
  if (intensity_bin) intensity_bin->report(file, "intensity");
  if (classification_bin) classification_bin->report(file, "classification");
  if (samples_bin) samples_bin->report(file, "samples");
  if (anchor_X_bin) anchor_X_bin->report(file, "raw X of anchor");
  if (anchor_Y_bin) anchor_Y_bin->report(file, "raw Y of anchor");
  if (anchor_Z_bin) anchor_Z_bin->report(file, "raw Z of anchor");
  if (target_X_bin) target_X_bin->report(file, "raw X of target");
  if (target_Y_bin) target_Y_bin->report(file, "raw Y of target");
  if (target_Z_bin) target_Z_bin->report(file, "raw Z of target");
  // averages bins
//  if (scan_angle_bin_z) scan_angle_bin_z->report(file, "scan angle", "z coordinate");
}

BOOL PULSEoccupancyGrid::add(const PULSEpulse* pulse)
{
  I32 pos_x, pos_y;
  if (grid_spacing < 0)
  {
    grid_spacing = -grid_spacing;
    pos_x = I32_FLOOR(pulse->get_anchor_x() / grid_spacing);
    pos_y = I32_FLOOR(pulse->get_anchor_y() / grid_spacing);
    anker = pos_y;
    min_x = max_x = pos_x;
    min_y = max_y = pos_y;
  }
  else
  {
    pos_x = I32_FLOOR(pulse->get_anchor_x() / grid_spacing);
    pos_y = I32_FLOOR(pulse->get_anchor_y() / grid_spacing);
    if (pos_x < min_x) min_x = pos_x; else if (pos_x > max_x) max_x = pos_x;
    if (pos_y < min_y) min_y = pos_y; else if (pos_y > max_y) max_y = pos_y;
  }
  return add_internal(pos_x, pos_y);
}

BOOL PULSEoccupancyGrid::add(I32 pos_x, I32 pos_y)
{
  if (grid_spacing < 0)
  {
    grid_spacing = -grid_spacing;
    anker = pos_y;
    min_x = max_x = pos_x;
    min_y = max_y = pos_y;
  }
  else
  {
    if (pos_x < min_x) min_x = pos_x; else if (pos_x > max_x) max_x = pos_x;
    if (pos_y < min_y) min_y = pos_y; else if (pos_y > max_y) max_y = pos_y;
  }
  return add_internal(pos_x, pos_y);
}

BOOL PULSEoccupancyGrid::add_internal(I32 pos_x, I32 pos_y)
{
  pos_y = pos_y - anker;
  BOOL no_x_anker = FALSE;
  U32* array_size;
  I32** ankers;
  U32*** array;
  U16** array_sizes;
  if (pos_y < 0)
  {
    pos_y = -pos_y - 1;
    ankers = &minus_ankers;
    if ((U32)pos_y < minus_plus_size && minus_plus_sizes[pos_y])
    {
      pos_x -= minus_ankers[pos_y];
      if (pos_x < 0)
      {
        pos_x = -pos_x - 1;
        array_size = &minus_minus_size;
        array = &minus_minus;
        array_sizes = &minus_minus_sizes;
      }
      else
      {
        array_size = &minus_plus_size;
        array = &minus_plus;
        array_sizes = &minus_plus_sizes;
      }
    }
    else
    {
      no_x_anker = TRUE;
      array_size = &minus_plus_size;
      array = &minus_plus;
      array_sizes = &minus_plus_sizes;
    }
  }
  else
  {
    ankers = &plus_ankers;
    if ((U32)pos_y < plus_plus_size && plus_plus_sizes[pos_y])
    {
      pos_x -= plus_ankers[pos_y];
      if (pos_x < 0)
      {
        pos_x = -pos_x - 1;
        array_size = &plus_minus_size;
        array = &plus_minus;
        array_sizes = &plus_minus_sizes;
      }
      else
      {
        array_size = &plus_plus_size;
        array = &plus_plus;
        array_sizes = &plus_plus_sizes;
      }
    }
    else
    {
      no_x_anker = TRUE;
      array_size = &plus_plus_size;
      array = &plus_plus;
      array_sizes = &plus_plus_sizes;
    }
  }
  // maybe grow banded grid in y direction
  if ((U32)pos_y >= *array_size)
  {
    U32 array_size_new = ((pos_y/1024)+1)*1024;
    if (*array_size)
    {
      if (array == &minus_plus || array == &plus_plus) *ankers = (I32*)realloc(*ankers, array_size_new*sizeof(I32));
      *array = (U32**)realloc(*array, array_size_new*sizeof(U32*));
      *array_sizes = (U16*)realloc(*array_sizes, array_size_new*sizeof(U16));
    }
    else
    {
      if (array == &minus_plus || array == &plus_plus) *ankers = (I32*)malloc(array_size_new*sizeof(I32));
      *array = (U32**)malloc(array_size_new*sizeof(U32*));
      *array_sizes = (U16*)malloc(array_size_new*sizeof(U16));
    }
    for (U32 i = *array_size; i < array_size_new; i++)
    {
      (*array)[i] = 0;
      (*array_sizes)[i] = 0;
    }
    *array_size = array_size_new;
  }
  // is this the first x anker for this y pos?
  if (no_x_anker)
  {
    (*ankers)[pos_y] = pos_x;
    pos_x = 0;
  }
  // maybe grow banded grid in x direction
  U32 pos_x_pos = pos_x/32;
  if (pos_x_pos >= (*array_sizes)[pos_y])
  {
    U32 array_sizes_new = ((pos_x_pos/256)+1)*256;
    if ((*array_sizes)[pos_y])
    {
      (*array)[pos_y] = (U32*)realloc((*array)[pos_y], array_sizes_new*sizeof(U32));
    }
    else
    {
      (*array)[pos_y] = (U32*)malloc(array_sizes_new*sizeof(U32));
    }
    for (U16 i = (*array_sizes)[pos_y]; i < array_sizes_new; i++)
    {
      (*array)[pos_y][i] = 0;
    }
    (*array_sizes)[pos_y] = array_sizes_new;
  }
  U32 pos_x_bit = 1 << (pos_x%32);
  if ((*array)[pos_y][pos_x_pos] & pos_x_bit) return FALSE;
  (*array)[pos_y][pos_x_pos] |= pos_x_bit;
  num_occupied++;
  return TRUE;
}

BOOL PULSEoccupancyGrid::occupied(const PULSEpulse* pulse) const
{
  I32 pos_x = I32_FLOOR(pulse->get_anchor_x() / grid_spacing);
  I32 pos_y = I32_FLOOR(pulse->get_anchor_y() / grid_spacing);
  return occupied(pos_x, pos_y);
}

BOOL PULSEoccupancyGrid::occupied(I32 pos_x, I32 pos_y) const
{
  if (grid_spacing < 0)
  {
    return FALSE;
  }
  pos_y = pos_y - anker;
  U32 array_size;
  const I32* ankers;
  const U32* const * array;
  const U16* array_sizes;
  if (pos_y < 0)
  {
    pos_y = -pos_y - 1;
    ankers = minus_ankers;
    if ((U32)pos_y < minus_plus_size && minus_plus_sizes[pos_y])
    {
      pos_x -= minus_ankers[pos_y];
      if (pos_x < 0)
      {
        pos_x = -pos_x - 1;
        array_size = minus_minus_size;
        array = minus_minus;
        array_sizes = minus_minus_sizes;
      }
      else
      {
        array_size = minus_plus_size;
        array = minus_plus;
        array_sizes = minus_plus_sizes;
      }
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    ankers = plus_ankers;
    if ((U32)pos_y < plus_plus_size && plus_plus_sizes[pos_y])
    {
      pos_x -= plus_ankers[pos_y];
      if (pos_x < 0)
      {
        pos_x = -pos_x - 1;
        array_size = plus_minus_size;
        array = plus_minus;
        array_sizes = plus_minus_sizes;
      }
      else
      {
        array_size = plus_plus_size;
        array = plus_plus;
        array_sizes = plus_plus_sizes;
      }
    }
    else
    {
      return FALSE;
    }
  }
  // maybe out of bounds in y direction
  if ((U32)pos_y >= array_size)
  {
    return FALSE;
  }
  // maybe out of bounds in x direction
  U32 pos_x_pos = pos_x/32;
  if (pos_x_pos >= array_sizes[pos_y])
  {
    return FALSE;
  }
  U32 pos_x_bit = 1 << (pos_x%32);
  if (array[pos_y][pos_x_pos] & pos_x_bit) return TRUE;
  return FALSE;
}

BOOL PULSEoccupancyGrid::active() const
{
  if (grid_spacing < 0) return FALSE;
  return TRUE;
}

void PULSEoccupancyGrid::reset()
{
  min_x = min_y = max_x = max_y = 0;
  if (grid_spacing > 0) grid_spacing = -grid_spacing;
  if (minus_minus_size)
  {
    for (U32 i = 0; i < minus_minus_size; i++) if (minus_minus[i]) free(minus_minus[i]);
    free(minus_minus);
    minus_minus = 0;
    free(minus_minus_sizes);
    minus_minus_sizes = 0;
    minus_minus_size = 0;
  }
  if (minus_plus_size)
  {
    free(minus_ankers);
    minus_ankers = 0;
    for (U32 i = 0; i < minus_plus_size; i++) if (minus_plus[i]) free(minus_plus[i]);
    free(minus_plus);
    minus_plus = 0;
    free(minus_plus_sizes);
    minus_plus_sizes = 0;
    minus_plus_size = 0;
  }
  if (plus_minus_size)
  {
    for (U32 i = 0; i < plus_minus_size; i++) if (plus_minus[i]) free(plus_minus[i]);
    free(plus_minus);
    plus_minus = 0;
    free(plus_minus_sizes);
    plus_minus_sizes = 0;
    plus_minus_size = 0;
  }
  if (plus_plus_size)
  {
    free(plus_ankers);
    plus_ankers = 0;
    for (U32 i = 0; i < plus_plus_size; i++) if (plus_plus[i]) free(plus_plus[i]);
    free(plus_plus);
    plus_plus = 0;
    free(plus_plus_sizes);
    plus_plus_sizes = 0;
    plus_plus_size = 0;
  }
  num_occupied = 0;
}

BOOL PULSEoccupancyGrid::write_asc_grid(const char* file_name) const
{
  FILE* file = fopen(file_name, "w");
  if (file == 0) return FALSE;
  fprintf(file, "ncols %d\012", max_x-min_x+1);
  fprintf(file, "nrows %d\012", max_y-min_y+1);
  fprintf(file, "xllcorner %f\012", grid_spacing*min_x);
  fprintf(file, "yllcorner %f\012", grid_spacing*min_y);
  fprintf(file, "cellsize %lf\012", grid_spacing);
  fprintf(file, "NODATA_value %d\012", 0);
  fprintf(file, "\012");
  I32 pos_x, pos_y;
  for (pos_y = min_y; pos_y <= max_y; pos_y++)
  {
    for (pos_x = min_x; pos_x <= max_x; pos_x++)
    {
      if (occupied(pos_x, pos_y))
      {
        fprintf(file, "1 ");
      }
      else
      {
        fprintf(file, "0 ");
      }
    }
    fprintf(file, "\012");
  }
  fclose(file);
  return TRUE;
}

PULSEoccupancyGrid::PULSEoccupancyGrid(F32 grid_spacing)
{
  min_x = min_y = max_x = max_y = 0;
  this->grid_spacing = -grid_spacing;
  minus_ankers = 0;
  minus_minus_size = 0;
  minus_minus = 0; 
  minus_minus_sizes = 0;
  minus_plus_size = 0;
  minus_plus = 0;
  minus_plus_sizes = 0;
  plus_ankers = 0;
  plus_minus_size = 0;
  plus_minus = 0;
  plus_minus_sizes = 0;
  plus_plus_size = 0;
  plus_plus = 0;
  plus_plus_sizes = 0;
  num_occupied = 0;
}

PULSEoccupancyGrid::~PULSEoccupancyGrid()
{
  reset();
}

