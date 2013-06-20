/*
===============================================================================

  FILE:  pulsedescriptor.cpp
  
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

#include "waveswaves.hpp"

#include "bytestreamout.hpp"
#include "bytestreamin.hpp"
#include "pulsepulse.hpp"
#include "pulsedescriptor.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

WAVESheader::WAVESheader()
{
  memset(this, 0, sizeof(WAVESheader));
  strcpy(file_signature, "PulseWavesWaves");
}

BOOL WAVESheader::save(ByteStreamOut* stream) const
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is NULL\n");
    return FALSE;
  }

  // write header variable after variable (to avoid alignment issues)

  try { stream->putBytes((U8*)file_signature, 16); } catch(...)
  {
    fprintf(stderr,"ERROR: writing file_signature\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&compression); } catch(...)
  {
    fprintf(stderr,"ERROR: writing compression\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)unused, 40); } catch(...)
  {
    fprintf(stderr,"ERROR: writing unused\n");
    return FALSE;
  }

  return TRUE;
}

BOOL WAVESheader::load(ByteStreamIn* stream)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is NULL\n");
    return FALSE;
  }

  // read the header variable after variable (to avoid alignment issues)

  try { stream->getBytes((U8*)file_signature, 16); } catch(...)
  {
    fprintf(stderr,"ERROR: reading file_signature\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&compression); } catch(...)
  {
    fprintf(stderr,"ERROR: reading compression\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)unused, 40); } catch(...)
  {
    fprintf(stderr,"ERROR: reading unused\n");
    return FALSE;
  }

  return TRUE;
}

BOOL WAVESheader::check() const
{
  if (strncmp(file_signature, "PulseWavesWaves", 15) != 0)
  {
    fprintf(stderr,"ERROR: wrong file signature '%s'\n", file_signature);
    return FALSE;
  }

  if (compression > 1)
  {
    fprintf(stderr,"WARNING: compression %d not supported\n", compression);
  }

  return TRUE;
}

BOOL WAVESsampling::init(const PULSEsampling* sampling)
{
  if (sampling == 0)
  {
    fprintf(stderr,"ERROR: sampling pointer is NULL\n");
    return FALSE;
  }

  if (sampling->scale_for_duration_from_anchor <= 0.0f)
  {
    fprintf(stderr,"ERROR: scale_for_duration_from_anchor of PULSEsampling is %g but needs to be larger than zero.\n", sampling->scale_for_duration_from_anchor);
    return FALSE;
  }

  if (sampling->unused != 0)
  {
    fprintf(stderr,"ERROR: unused of PULSEsampling is %d but needs to be zero.\n", sampling->unused);
    return FALSE;
  }

  if ((sampling->bits_for_number_of_segments == 0) && (sampling->number_of_segments == 0))
  {
    fprintf(stderr,"ERROR: bits_for_number_of_segments and number_of_segments of PULSEsampling cannot both be zero.\n");
    return FALSE;
  }

  if ((sampling->bits_for_number_of_samples == 0) && (sampling->number_of_samples == 0))
  {
    fprintf(stderr,"ERROR: bits_for_number_of_samples and number_of_samples of PULSEsampling cannot both be zero.\n");
    return FALSE;
  }

  if (sampling->bits_per_sample == 0)
  {
    fprintf(stderr,"ERROR: bits_per_sample of PULSEsampling cannot both be zero.\n");
    return FALSE;
  }

  if (sampling->compression != 0)
  {
    fprintf(stderr,"ERROR: compression of PULSEsampling is %d but needs to be zero.\n", sampling->compression);
    return FALSE;
  }

  type = sampling->type;
  channel = sampling->channel;
  unused = sampling->unused;
  bits_for_duration_from_anchor = sampling->bits_for_duration_from_anchor;
  scale_for_duration_from_anchor = sampling->scale_for_duration_from_anchor;
  offset_for_duration_from_anchor = sampling->offset_for_duration_from_anchor;
  bits_for_number_of_segments = sampling->bits_for_number_of_segments;
  bits_for_number_of_samples = sampling->bits_for_number_of_samples;
  number_of_segments = sampling->number_of_segments;
  number_of_samples = sampling->number_of_samples;
  bits_per_sample = sampling->bits_per_sample;
  lookup_table_index = sampling->lookup_table_index;
  sample_units = sampling->sample_units;
//  compression = sampling->compression;

  return TRUE;
}

void WAVESsampling::clean()
{
  xyz[0] = xyz[1] = xyz[2] = 0.0;
  sample = 0;
  type = 0;
  channel = 0;
  unused = 0;
  bits_for_duration_from_anchor = 0;
  scale_for_duration_from_anchor = 1.0f;
  offset_for_duration_from_anchor = 0;
  bits_for_number_of_segments = 0;
  bits_for_number_of_samples = 0;
  number_of_segments = 0;
  number_of_samples = 0;
  bits_per_sample = 0;
  compression = 0;
}

WAVESsampling::WAVESsampling()
{
  clean();
}

WAVESsampling::~WAVESsampling()
{
}

// ------------------- specialized for one segment ------------------

BOOL WAVESsamplingOneSegment::get_sample_xyz(const PULSEpulse* pulse, I32 sample_idx)
{
  xyz[0] = pulse->get_anchor_x() + pulse->get_dir_x()*(duration + sample_idx);
  xyz[1] = pulse->get_anchor_y() + pulse->get_dir_y()*(duration + sample_idx);
  xyz[2] = pulse->get_anchor_z() + pulse->get_dir_z()*(duration + sample_idx);
  return TRUE;
}

BOOL WAVESsamplingOneSegment::init(const PULSEsampling* sampling)
{
  if (!WAVESsampling::init(sampling))
  {
    return FALSE;
  }

  if (bits_for_number_of_segments != 0)
  {
    fprintf(stderr,"ERROR: bits_for_number_of_segments is %d for WAVESsamplingOneSegment\n", bits_for_number_of_segments);
    return FALSE;
  }

  if (number_of_segments != 1)
  {
    fprintf(stderr,"ERROR: number_of_segments is %d for WAVESsamplingOneSegment\n", number_of_segments);
    return FALSE;
  }

  return TRUE;
}

void WAVESsamplingOneSegment::clean()
{
  WAVESsampling::clean();
  if (samples) delete [] samples;
  samples = 0;
  num_samples = 0;
  quantized_duration = 0;
  duration = 0.0f;
}

WAVESsamplingOneSegment::WAVESsamplingOneSegment()
{
  samples = 0;
  number_of_segments = 1;
  clean();
}

WAVESsamplingOneSegment::~WAVESsamplingOneSegment()
{
  if (samples) delete [] samples;
}

BOOL WAVESsamplingOneSegment8bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingOneSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    if (samples) delete [] samples;
    samples = new U8[number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples for WAVESsamplingOneSegment8bit\n", number_of_samples);
      return FALSE;
    }
  }

  num_samples = number_of_samples;

  return TRUE;
}

WAVESsamplingOneSegment8bit::WAVESsamplingOneSegment8bit()
{
}

WAVESsamplingOneSegment8bit::~WAVESsamplingOneSegment8bit()
{
}

BOOL WAVESsamplingOneSegment16bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingOneSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    if (samples) delete [] samples;
    samples = new U8[2*number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples for WAVESsamplingOneSegment16bit\n", number_of_samples);
      return FALSE;
    }
  }

  num_samples = number_of_samples;

  return TRUE;
}

WAVESsamplingOneSegment16bit::WAVESsamplingOneSegment16bit()
{
}

WAVESsamplingOneSegment16bit::~WAVESsamplingOneSegment16bit()
{
}

WAVESsamplingOneSegment8bitFixed::WAVESsamplingOneSegment8bitFixed()
{
}

WAVESsamplingOneSegment8bitFixed::~WAVESsamplingOneSegment8bitFixed()
{
}

WAVESsamplingOneSegment16bitFixed::WAVESsamplingOneSegment16bitFixed()
{
}

WAVESsamplingOneSegment16bitFixed::~WAVESsamplingOneSegment16bitFixed()
{
}

BOOL WAVESsamplingOneSegment8bitVariable::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingOneSegment8bit::init(sampling))
  {
    return FALSE;
  }

  alloc_samples = number_of_samples;

  return TRUE;
}

WAVESsamplingOneSegment8bitVariable::WAVESsamplingOneSegment8bitVariable()
{
  alloc_samples = 0;
}

WAVESsamplingOneSegment8bitVariable::~WAVESsamplingOneSegment8bitVariable()
{
}

BOOL WAVESsamplingOneSegment16bitVariable::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingOneSegment16bit::init(sampling))
  {
    return FALSE;
  }

  alloc_samples = number_of_samples;

  return TRUE;
}

WAVESsamplingOneSegment16bitVariable::WAVESsamplingOneSegment16bitVariable()
{
  alloc_samples = 0;
}

WAVESsamplingOneSegment16bitVariable::~WAVESsamplingOneSegment16bitVariable()
{
}

// ------------------- specialized for two segments ------------------

BOOL WAVESsamplingTwoSegment::get_sample_xyz(const PULSEpulse* pulse, I32 sample_idx)
{
  if (active)
  {
    if (sample_idx < num_samples[1])
    {
      F32 dur = duration[1] + sample_idx;
      xyz[0] = pulse->get_anchor_x() + pulse->get_dir_x()*dur;
      xyz[1] = pulse->get_anchor_y() + pulse->get_dir_y()*dur;
      xyz[2] = pulse->get_anchor_z() + pulse->get_dir_z()*dur;
      return TRUE;
    }
  }
  else
  {
    if (sample_idx < num_samples[0])
    {
      F32 dur = duration[0] + sample_idx;
      xyz[0] = pulse->get_anchor_x() + pulse->get_dir_x()*dur;
      xyz[1] = pulse->get_anchor_y() + pulse->get_dir_y()*dur;
      xyz[2] = pulse->get_anchor_z() + pulse->get_dir_z()*dur;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL WAVESsamplingTwoSegment::init(const PULSEsampling* sampling)
{
  if (!WAVESsampling::init(sampling))
  {
    return FALSE;
  }

  if (bits_for_number_of_segments != 0)
  {
    fprintf(stderr,"ERROR: bits_for_number_of_segments is %d for WAVESsamplingTwoSegment\n", bits_for_number_of_segments);
    return FALSE;
  }

  if (number_of_segments != 2)
  {
    fprintf(stderr,"ERROR: number_of_segments is %d for WAVESsamplingTwoSegment\n", number_of_segments);
    return FALSE;
  }

  return TRUE;
}

void WAVESsamplingTwoSegment::clean()
{
  WAVESsampling::clean();
  if (samples[0]) delete [] samples[0];
  samples[0] = 0;
  num_samples[0] = 0;
  quantized_duration[0] = 0;
  duration[0] = 0.0f;
  if (samples[1]) delete [] samples[1];
  samples[1] = 0;
  num_samples[1] = 0;
  quantized_duration[1] = 0;
  duration[1] = 0.0f;
}

WAVESsamplingTwoSegment::WAVESsamplingTwoSegment()
{
  number_of_segments = 2;
  samples[0] = 0;
  samples[1] = 0;
  clean();
}

WAVESsamplingTwoSegment::~WAVESsamplingTwoSegment()
{
  if (samples[0]) delete [] samples[0];
  if (samples[1]) delete [] samples[1];
}

BOOL WAVESsamplingTwoSegment8bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingTwoSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    if (samples[0]) delete [] samples[0];
    samples[0] = new U8[number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[0] for WAVESsamplingTwoSegment8bit\n", number_of_samples);
      return FALSE;
    }
    if (samples[1]) delete [] samples[1];
    samples[1] = new U8[number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[1] for WAVESsamplingTwoSegment8bit\n", number_of_samples);
      return FALSE;
    }
  }

  num_samples[0] = number_of_samples;
  num_samples[1] = number_of_samples;

  return TRUE;
}

WAVESsamplingTwoSegment8bit::WAVESsamplingTwoSegment8bit()
{
}

WAVESsamplingTwoSegment8bit::~WAVESsamplingTwoSegment8bit()
{
}

BOOL WAVESsamplingTwoSegment16bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingTwoSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    if (samples[0]) delete [] samples[0];
    samples[0] = new U8[2*number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[0] for WAVESsamplingTwoSegment16bit\n", number_of_samples);
      return FALSE;
    }
    if (samples[1]) delete [] samples[1];
    samples[1] = new U8[2*number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[1] for WAVESsamplingTwoSegment16bit\n", number_of_samples);
      return FALSE;
    }
  }

  num_samples[0] = number_of_samples;
  num_samples[1] = number_of_samples;

  return TRUE;
}

WAVESsamplingTwoSegment16bit::WAVESsamplingTwoSegment16bit()
{
}

WAVESsamplingTwoSegment16bit::~WAVESsamplingTwoSegment16bit()
{
}

WAVESsamplingTwoSegment8bitFixed::WAVESsamplingTwoSegment8bitFixed()
{
}

WAVESsamplingTwoSegment8bitFixed::~WAVESsamplingTwoSegment8bitFixed()
{
}

WAVESsamplingTwoSegment16bitFixed::WAVESsamplingTwoSegment16bitFixed()
{
}

WAVESsamplingTwoSegment16bitFixed::~WAVESsamplingTwoSegment16bitFixed()
{
}

BOOL WAVESsamplingTwoSegment8bitVariable::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingTwoSegment8bit::init(sampling))
  {
    return FALSE;
  }

  alloc_samples[0] = number_of_samples;
  alloc_samples[1] = number_of_samples;

  return TRUE;
}

WAVESsamplingTwoSegment8bitVariable::WAVESsamplingTwoSegment8bitVariable()
{
  alloc_samples[0] = 0;
  alloc_samples[1] = 0;
}

WAVESsamplingTwoSegment8bitVariable::~WAVESsamplingTwoSegment8bitVariable()
{
}

BOOL WAVESsamplingTwoSegment16bitVariable::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingTwoSegment16bit::init(sampling))
  {
    return FALSE;
  }

  alloc_samples[0] = number_of_samples;
  alloc_samples[1] = number_of_samples;

  return TRUE;
}

WAVESsamplingTwoSegment16bitVariable::WAVESsamplingTwoSegment16bitVariable()
{
  alloc_samples[0] = 0;
  alloc_samples[1] = 0;
}

WAVESsamplingTwoSegment16bitVariable::~WAVESsamplingTwoSegment16bitVariable()
{
}

// ------------------- specialized for three segments ------------------
// ------------------- specialized for three segments ------------------
// ------------------- specialized for three segments ------------------

BOOL WAVESsamplingThreeSegment::get_sample_xyz(const PULSEpulse* pulse, I32 sample_idx)
{
  if (sample_idx < num_samples[active])
  {
    F32 dur = duration[active] + sample_idx;
    xyz[0] = pulse->get_anchor_x() + pulse->get_dir_x()*dur;
    xyz[1] = pulse->get_anchor_y() + pulse->get_dir_y()*dur;
    xyz[2] = pulse->get_anchor_z() + pulse->get_dir_z()*dur;
    return TRUE;
  }
  return FALSE;
}

BOOL WAVESsamplingThreeSegment::init(const PULSEsampling* sampling)
{
  if (!WAVESsampling::init(sampling))
  {
    return FALSE;
  }

  if (bits_for_number_of_segments != 0)
  {
    fprintf(stderr,"ERROR: bits_for_number_of_segments is %d for WAVESsamplingThreeSegment\n", bits_for_number_of_segments);
    return FALSE;
  }

  if (number_of_segments != 3)
  {
    fprintf(stderr,"ERROR: number_of_segments is %d for WAVESsamplingThreeSegment\n", number_of_segments);
    return FALSE;
  }

  return TRUE;
}

void WAVESsamplingThreeSegment::clean()
{
  WAVESsampling::clean();
  if (samples[0]) delete [] samples[0];
  samples[0] = 0;
  num_samples[0] = 0;
  quantized_duration[0] = 0;
  duration[0] = 0.0f;
  if (samples[1]) delete [] samples[1];
  samples[1] = 0;
  num_samples[1] = 0;
  quantized_duration[1] = 0;
  duration[1] = 0.0f;
  if (samples[2]) delete [] samples[2];
  samples[2] = 0;
  num_samples[2] = 0;
  quantized_duration[2] = 0;
  duration[2] = 0.0f;
}

WAVESsamplingThreeSegment::WAVESsamplingThreeSegment()
{
  number_of_segments = 3;
  samples[0] = 0;
  samples[1] = 0;
  samples[2] = 0;
  clean();
}

WAVESsamplingThreeSegment::~WAVESsamplingThreeSegment()
{
  if (samples[0]) delete [] samples[0];
  if (samples[1]) delete [] samples[1];
  if (samples[2]) delete [] samples[2];
}

BOOL WAVESsamplingThreeSegment8bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingThreeSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    if (samples[0]) delete [] samples[0];
    samples[0] = new U8[number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[0] for WAVESsamplingThreeSegment8bit\n", number_of_samples);
      return FALSE;
    }
    if (samples[1]) delete [] samples[1];
    samples[1] = new U8[number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[1] for WAVESsamplingThreeSegment8bit\n", number_of_samples);
      return FALSE;
    }
    if (samples[2]) delete [] samples[2];
    samples[2] = new U8[number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[2] for WAVESsamplingThreeSegment8bit\n", number_of_samples);
      return FALSE;
    }
  }

  num_samples[0] = number_of_samples;
  num_samples[1] = number_of_samples;
  num_samples[2] = number_of_samples;

  return TRUE;
}

WAVESsamplingThreeSegment8bit::WAVESsamplingThreeSegment8bit()
{
}

WAVESsamplingThreeSegment8bit::~WAVESsamplingThreeSegment8bit()
{
}

BOOL WAVESsamplingThreeSegment16bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingThreeSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    if (samples[0]) delete [] samples[0];
    samples[0] = new U8[2*number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[0] for WAVESsamplingThreeSegment16bit\n", number_of_samples);
      return FALSE;
    }
    if (samples[1]) delete [] samples[1];
    samples[1] = new U8[2*number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[1] for WAVESsamplingThreeSegment16bit\n", number_of_samples);
      return FALSE;
    }
    if (samples[2]) delete [] samples[2];
    samples[2] = new U8[2*number_of_samples];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating %d samples[2] for WAVESsamplingThreeSegment16bit\n", number_of_samples);
      return FALSE;
    }
  }

  num_samples[0] = number_of_samples;
  num_samples[1] = number_of_samples;
  num_samples[2] = number_of_samples;

  return TRUE;
}

WAVESsamplingThreeSegment16bit::WAVESsamplingThreeSegment16bit()
{
}

WAVESsamplingThreeSegment16bit::~WAVESsamplingThreeSegment16bit()
{
}

WAVESsamplingThreeSegment8bitFixed::WAVESsamplingThreeSegment8bitFixed()
{
}

WAVESsamplingThreeSegment8bitFixed::~WAVESsamplingThreeSegment8bitFixed()
{
}

WAVESsamplingThreeSegment16bitFixed::WAVESsamplingThreeSegment16bitFixed()
{
}

WAVESsamplingThreeSegment16bitFixed::~WAVESsamplingThreeSegment16bitFixed()
{
}

BOOL WAVESsamplingThreeSegment8bitVariable::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingThreeSegment8bit::init(sampling))
  {
    return FALSE;
  }

  alloc_samples[0] = number_of_samples;
  alloc_samples[1] = number_of_samples;
  alloc_samples[2] = number_of_samples;

  return TRUE;
}

WAVESsamplingThreeSegment8bitVariable::WAVESsamplingThreeSegment8bitVariable()
{
  alloc_samples[0] = 0;
  alloc_samples[1] = 0;
  alloc_samples[2] = 0;
}

WAVESsamplingThreeSegment8bitVariable::~WAVESsamplingThreeSegment8bitVariable()
{
}

BOOL WAVESsamplingThreeSegment16bitVariable::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingThreeSegment16bit::init(sampling))
  {
    return FALSE;
  }

  alloc_samples[0] = number_of_samples;
  alloc_samples[1] = number_of_samples;
  alloc_samples[2] = number_of_samples;

  return TRUE;
}

WAVESsamplingThreeSegment16bitVariable::WAVESsamplingThreeSegment16bitVariable()
{
  alloc_samples[0] = 0;
  alloc_samples[1] = 0;
  alloc_samples[2] = 0;
}

WAVESsamplingThreeSegment16bitVariable::~WAVESsamplingThreeSegment16bitVariable()
{
}

// ------------------- specialized for mutliple segments ------------------
// ------------------- specialized for mutliple segments ------------------
// ------------------- specialized for mutliple segments ------------------
// ------------------- specialized for mutliple segments ------------------

BOOL WAVESsamplingFixedMultiSegment::get_sample_xyz(const PULSEpulse* pulse, I32 sample_idx)
{
  F32 dur = duration[active] + sample_idx;
  xyz[0] = pulse->get_anchor_x() + pulse->get_dir_x()*dur;
  xyz[1] = pulse->get_anchor_y() + pulse->get_dir_y()*dur;
  xyz[2] = pulse->get_anchor_z() + pulse->get_dir_z()*dur;
  return TRUE;
}
  
BOOL WAVESsamplingFixedMultiSegment::init(const PULSEsampling* sampling)
{
  if (!WAVESsampling::init(sampling))
  {
    return FALSE;
  }
  if (number_of_segments)
  {
    if (number_of_segments > alloc_segments)
    {
      I32 i;
      if (samples)
      {
        for (i = 0; i < alloc_segments; i++)
        {
          if (samples[i]) delete [] samples[i];
        }
        delete [] samples;
      }
      samples = new U8*[number_of_segments];
      if (samples == 0)
      {
        fprintf(stderr,"ERROR: allocating samples[] for %d segments\n", number_of_segments);
        return FALSE;
      }
      memset(samples, 0, sizeof(U8*)*number_of_segments);
      if (num_samples) delete [] num_samples;
      num_samples = new I32[number_of_segments];
      if (num_samples == 0)
      {
        fprintf(stderr,"ERROR: allocating num_samples[] for %d segments\n", number_of_segments);
        return FALSE;
      }
      memset(num_samples, 0, sizeof(I32)*number_of_segments);
      if (alloc_samples) delete [] alloc_samples;
      alloc_samples = new I32[number_of_segments];
      if (alloc_samples == 0)
      {
        fprintf(stderr,"ERROR: allocating alloc_samples[] for %d segments\n", number_of_segments);
        return FALSE;
      }
      memset(alloc_samples, 0, sizeof(I32)*number_of_segments);
      if (quantized_duration) delete [] quantized_duration;
      quantized_duration = new I32[number_of_segments];
      if (quantized_duration == 0)
      {
        fprintf(stderr,"ERROR: allocating quantized_duration[] for %d segments\n", number_of_segments);
        return FALSE;
      }
      memset(quantized_duration, 0, sizeof(I32)*number_of_segments);
      if (duration) delete [] duration;
      duration = new F32[number_of_segments];
      if (duration == 0)
      {
        fprintf(stderr,"ERROR: allocating duration[] for %d segments\n", number_of_segments);
        return FALSE;
      }
      memset(duration, 0, sizeof(F32)*number_of_segments);
      alloc_segments = number_of_segments;
    }
  }
  return TRUE;
}

void WAVESsamplingFixedMultiSegment::clean()
{
  WAVESsampling::clean();
  if (samples)
  {
    for (I32 i = 0; i < number_of_segments; i++) if (samples[i]) delete [] samples[i];
    delete [] samples;
    samples = 0;
  }
  if (num_samples)
  {
    delete [] num_samples;
    num_samples = 0;
  }
  if (alloc_samples)
  {
    delete [] alloc_samples;
    alloc_samples = 0;
  }
  if (quantized_duration)
  {
    delete [] quantized_duration;
    quantized_duration = 0;
  }
  if (duration)
  {
    delete [] duration;
    duration = 0;
  }
}

WAVESsamplingFixedMultiSegment::WAVESsamplingFixedMultiSegment()
{
  active = 0;
  alloc_segments = 0;
  samples = 0;
  num_samples = 0;
  alloc_samples = 0;
  quantized_duration = 0;
  duration = 0;
  clean();
}

WAVESsamplingFixedMultiSegment::~WAVESsamplingFixedMultiSegment()
{
  if (samples)
  {
    for (I32 i = 0; i < number_of_segments; i++) if (samples[i]) delete [] samples[i];
    delete [] samples;
  }
  if (num_samples)
  {
    delete [] num_samples;
  }
  if (quantized_duration)
  {
    delete [] quantized_duration;
  }
  if (duration)
  {
    delete [] duration;
  }
}

BOOL WAVESsamplingFixedMultiSegment8bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingFixedMultiSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    for (I32 i = 0; i < number_of_segments; i++)
    {
      if (samples[i]) delete [] samples[i];
      samples[i] = new U8[number_of_samples];
      if (samples[i] == 0)
      {
        fprintf(stderr,"ERROR: allocating %d samples for segment %d of %d for WAVESsamplingFixedMultiSegment8bit\n", number_of_samples, i, number_of_segments);
        return FALSE;
      }
      num_samples[i] = number_of_samples;
      alloc_samples[i] = number_of_samples;
    }
  }

  return TRUE;
}

WAVESsamplingFixedMultiSegment8bit::WAVESsamplingFixedMultiSegment8bit()
{
}

WAVESsamplingFixedMultiSegment8bit::~WAVESsamplingFixedMultiSegment8bit()
{
}

BOOL WAVESsamplingFixedMultiSegment16bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingFixedMultiSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    if (samples) delete [] samples;
    for (I32 i = 0; i < number_of_segments; i++)
    {
      if (samples[i]) delete [] samples[i];
      samples[i] = new U8[2*number_of_samples];
      if (samples[i] == 0)
      {
        fprintf(stderr,"ERROR: allocating %d samples for segment %d of %d for WAVESsamplingFixedMultiSegment16bit\n", number_of_samples, i, number_of_segments);
        return FALSE;
      }
      num_samples[i] = number_of_samples;
      alloc_samples[i] = number_of_samples;
    }
  }

  return TRUE;
}

WAVESsamplingFixedMultiSegment16bit::WAVESsamplingFixedMultiSegment16bit()
{
}

WAVESsamplingFixedMultiSegment16bit::~WAVESsamplingFixedMultiSegment16bit()
{
}

WAVESsamplingFixedMultiSegment8bitFixed::WAVESsamplingFixedMultiSegment8bitFixed()
{
}

WAVESsamplingFixedMultiSegment8bitFixed::~WAVESsamplingFixedMultiSegment8bitFixed()
{
}

WAVESsamplingFixedMultiSegment16bitFixed::WAVESsamplingFixedMultiSegment16bitFixed()
{
}

WAVESsamplingFixedMultiSegment16bitFixed::~WAVESsamplingFixedMultiSegment16bitFixed()
{
}

BOOL WAVESsamplingFixedMultiSegment8bitVariable::set_number_of_samples_for_segment(I32 num_samples)
{
  if (num_samples > alloc_samples[active])
  {
    if (samples[active])
    {
      delete [] samples[active];
    }
    samples[active] = new U8[num_samples];
    if (samples[active] == 0) return FALSE;
    alloc_samples[active] = num_samples;
  }
  this->num_samples[active] = num_samples;
  return TRUE;
};


WAVESsamplingFixedMultiSegment8bitVariable::WAVESsamplingFixedMultiSegment8bitVariable()
{
}

WAVESsamplingFixedMultiSegment8bitVariable::~WAVESsamplingFixedMultiSegment8bitVariable()
{
}

BOOL WAVESsamplingFixedMultiSegment16bitVariable::set_number_of_samples_for_segment(I32 num_samples)
{
  if (num_samples > alloc_samples[active])
  {
    if (samples[active])
    {
      delete [] samples[active];
    }
    samples[active] = new U8[2*num_samples];
    if (samples[active] == 0) return FALSE;
    alloc_samples[active] = num_samples;
  }
  this->num_samples[active] = num_samples;
  return TRUE;
};

WAVESsamplingFixedMultiSegment16bitVariable::WAVESsamplingFixedMultiSegment16bitVariable()
{
}

WAVESsamplingFixedMultiSegment16bitVariable::~WAVESsamplingFixedMultiSegment16bitVariable()
{
}

BOOL WAVESsamplingVaryingMultiSegment::set_number_of_segments(I32 number_of_segments)
{
  if ((number_of_segments <= 0) || (number_of_segments > U16_MAX))
  {
    fprintf(stderr,"ERROR: number_of_segments is %d for WAVESsamplingFixedMultiSegment\n", number_of_segments);
    return FALSE;
  }
  if (number_of_segments > alloc_segments)
  {
    I32 i;
    if (samples)
    {
      for (i = 0; i < alloc_segments; i++)
      {
        if (samples[i]) delete [] samples[i];
      }
      delete [] samples;
    }
    samples = new U8*[number_of_segments];
    if (samples == 0)
    {
      fprintf(stderr,"ERROR: allocating samples[] for %d segments\n", number_of_segments);
      return FALSE;
    }
    memset(samples, 0, sizeof(U8*)*number_of_segments);
    if (num_samples) delete [] num_samples;
    num_samples = new I32[number_of_segments];
    if (num_samples == 0)
    {
      fprintf(stderr,"ERROR: allocating num_samples[] for %d segments\n", number_of_segments);
      return FALSE;
    }
    memset(num_samples, 0, sizeof(I32)*number_of_segments);
    if (alloc_samples) delete [] alloc_samples;
    alloc_samples = new I32[number_of_segments];
    if (alloc_samples == 0)
    {
      fprintf(stderr,"ERROR: allocating alloc_samples[] for %d segments\n", number_of_segments);
      return FALSE;
    }
    memset(alloc_samples, 0, sizeof(I32)*number_of_segments);
    if (quantized_duration) delete [] quantized_duration;
    quantized_duration = new I32[number_of_segments];
    if (quantized_duration == 0)
    {
      fprintf(stderr,"ERROR: allocating quantized_duration[] for %d segments\n", number_of_segments);
      return FALSE;
    }
    memset(quantized_duration, 0, sizeof(I32)*number_of_segments);
    if (duration) delete [] duration;
    duration = new F32[number_of_segments];
    if (duration == 0)
    {
      fprintf(stderr,"ERROR: allocating duration[] for %d segments\n", number_of_segments);
      return FALSE;
    }
    memset(duration, 0, sizeof(F32)*number_of_segments);
    alloc_segments = number_of_segments;
  }
  this->number_of_segments = (U16)number_of_segments; 
  return TRUE;
};

WAVESsamplingVaryingMultiSegment::WAVESsamplingVaryingMultiSegment()
{
}

WAVESsamplingVaryingMultiSegment::~WAVESsamplingVaryingMultiSegment()
{
}

BOOL WAVESsamplingVaryingMultiSegment8bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingVaryingMultiSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    for (I32 i = 0; i < number_of_segments; i++)
    {
      if (samples[i]) delete [] samples[i];
      samples[i] = new U8[number_of_samples];
      if (samples[i] == 0)
      {
        fprintf(stderr,"ERROR: allocating %d samples for segment %d of %d for WAVESsamplingVaryingMultiSegment8bit\n", number_of_samples, i, number_of_segments);
        return FALSE;
      }
      num_samples[i] = number_of_samples;
      alloc_samples[i] = number_of_samples;
    }
  }

  return TRUE;
}

WAVESsamplingVaryingMultiSegment8bit::WAVESsamplingVaryingMultiSegment8bit()
{
}

WAVESsamplingVaryingMultiSegment8bit::~WAVESsamplingVaryingMultiSegment8bit()
{
}

BOOL WAVESsamplingVaryingMultiSegment16bit::init(const PULSEsampling* sampling)
{
  if (!WAVESsamplingVaryingMultiSegment::init(sampling))
  {
    return FALSE;
  }

  if (number_of_samples != 0)
  {
    for (I32 i = 0; i < number_of_segments; i++)
    {
      if (samples[i]) delete [] samples[i];
      samples[i] = new U8[2*number_of_samples];
      if (samples[i] == 0)
      {
        fprintf(stderr,"ERROR: allocating %d samples for segment %d of %d for WAVESsamplingVaryingMultiSegment16bit\n", number_of_samples, i, number_of_segments);
        return FALSE;
      }
      num_samples[i] = number_of_samples;
      alloc_samples[i] = number_of_samples;
    }
  }

  return TRUE;
}

WAVESsamplingVaryingMultiSegment16bit::WAVESsamplingVaryingMultiSegment16bit()
{
}

WAVESsamplingVaryingMultiSegment16bit::~WAVESsamplingVaryingMultiSegment16bit()
{
}

WAVESsamplingVaryingMultiSegment8bitFixed::WAVESsamplingVaryingMultiSegment8bitFixed()
{
}

WAVESsamplingVaryingMultiSegment8bitFixed::~WAVESsamplingVaryingMultiSegment8bitFixed()
{
}

WAVESsamplingVaryingMultiSegment16bitFixed::WAVESsamplingVaryingMultiSegment16bitFixed()
{
}

WAVESsamplingVaryingMultiSegment16bitFixed::~WAVESsamplingVaryingMultiSegment16bitFixed()
{
}

BOOL WAVESsamplingVaryingMultiSegment8bitVariable::set_number_of_samples_for_segment(I32 num_samples)
{
  if (num_samples > alloc_samples[active])
  {
    if (samples[active])
    {
      delete [] samples[active];
    }
    samples[active] = new U8[num_samples];
    if (samples[active] == 0) return FALSE;
    alloc_samples[active] = num_samples;
  }
  this->num_samples[active] = num_samples;
  return TRUE;
};

WAVESsamplingVaryingMultiSegment8bitVariable::WAVESsamplingVaryingMultiSegment8bitVariable()
{
}

WAVESsamplingVaryingMultiSegment8bitVariable::~WAVESsamplingVaryingMultiSegment8bitVariable()
{
}

BOOL WAVESsamplingVaryingMultiSegment16bitVariable::set_number_of_samples_for_segment(I32 num_samples)
{
  if (num_samples > alloc_samples[active])
  {
    if (samples[active])
    {
      delete [] samples[active];
    }
    samples[active] = new U8[2*num_samples];
    if (samples[active] == 0) return FALSE;
    alloc_samples[active] = num_samples;
  }
  this->num_samples[active] = num_samples;
  return TRUE;
};

WAVESsamplingVaryingMultiSegment16bitVariable::WAVESsamplingVaryingMultiSegment16bitVariable()
{
}

WAVESsamplingVaryingMultiSegment16bitVariable::~WAVESsamplingVaryingMultiSegment16bitVariable()
{
}

BOOL WAVESwaves::init(const PULSEdescriptor* descriptor)
{
  if (descriptor == 0)
  {
    fprintf(stderr,"ERROR: descriptor pointer is NULL\n");
    return FALSE;
  }

  clean();

  number_of_extra_bytes = descriptor->composition->number_of_extra_waves_bytes;
  
  if (number_of_extra_bytes)
  {
    extra_bytes = new U8[number_of_extra_bytes];

    if (extra_bytes == 0)
    {
      fprintf(stderr,"ERROR: failed to allocate %d waves extra bytes\n", number_of_extra_bytes);
      return FALSE;
    }
  }

  number_of_samplings = descriptor->composition->number_of_samplings;

  if (number_of_samplings)
  {
    samplings = new WAVESsampling*[number_of_samplings];

    if (samplings == 0)
    {
      fprintf(stderr,"ERROR: failed to allocate %d WAVESsampling pointers\n", number_of_samplings);
      return FALSE;
    }
  }

  U16 m;
  PULSEsampling* sampling;
  for (m = 0; m < number_of_samplings; m++)
  {
    sampling = &(descriptor->samplings[m]);

    if (sampling->bits_for_number_of_segments == 0)
    {
      // then we have a fixed number of s segments (a "fixed segmenting" and *not* a varying one)

      if (sampling->number_of_segments == 1)
      {
        // then we have exactly one segment

        if (sampling->bits_for_number_of_samples == 0)
        {
          // then we have a fixed number of samples per segment  (a "fixed sampling" and *not* a varying one)

          if (sampling->bits_per_sample == 8)
          {
            // then we have 8 bits per sample

            samplings[m] = new WAVESsamplingOneSegment8bitFixed();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating one fixed segment of %d 8 bit samples\n", sampling->number_of_samples);
              return FALSE;
            }
          }
          else if (sampling->bits_per_sample == 16)
          {
            // then we have 16 bits per sample

            samplings[m] = new WAVESsamplingOneSegment16bitFixed();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating one fixed segment of %d 16 bit samples\n", sampling->number_of_samples);
              return FALSE;
            }
          }
          else
          {
            // then we have an unimplemented number of bits per sample

            fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
            return FALSE;
          }
        }
        else
        {
          // then we have a variable number of samples per segment  (a "variable sampling" and *not* a fixed one)

          if (sampling->bits_per_sample == 8)
          {
            // then we have 8 bits per sample

            samplings[m] = new WAVESsamplingOneSegment8bitVariable();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating one variable segment of 8 bit samples\n");
              return FALSE;
            }
          }
          else if (sampling->bits_per_sample == 16)
          {
            // then we have 16 bits per sample

            samplings[m] = new WAVESsamplingOneSegment16bitVariable();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating one variable segment of 16 bit samples\n");
              return FALSE;
            }
          }
          else
          {
            // then we have an unimplemented number of bits per sample

            fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
            return FALSE;
          }
        }
      }
      else if (sampling->number_of_segments == 2)
      {
        // then we have exactly two segments

        if (sampling->bits_for_number_of_samples == 0)
        {
          // then we have a fixed number of samples per segment  (a "fixed sampling" and *not* a varying one)

          if (sampling->bits_per_sample == 8)
          {
            // then we have 8 bits per sample

            samplings[m] = new WAVESsamplingTwoSegment8bitFixed();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating two fixed segments of %d 8 bit samples\n", sampling->number_of_samples);
              return FALSE;
            }
          }
          else if (sampling->bits_per_sample == 16)
          {
            // then we have 16 bits per sample

            samplings[m] = new WAVESsamplingTwoSegment16bitFixed();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating two fixed segments of %d 16 bit samples\n", sampling->number_of_samples);
              return FALSE;
            }
          }
          else
          {
            // then we have an unimplemented number of bits per sample

            fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
            return FALSE;
          }
        }
        else
        {
          // then we have a variable number of samples per segment  (a "variable sampling" and *not* a fixed one)

          if (sampling->bits_per_sample == 8)
          {
            // then we have 8 bits per sample

            samplings[m] = new WAVESsamplingTwoSegment8bitVariable();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating two variable segments of 8 bit samples\n");
              return FALSE;
            }
          }
          else if (sampling->bits_per_sample == 16)
          {
            // then we have 16 bits per sample

            samplings[m] = new WAVESsamplingTwoSegment16bitVariable();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating two variable segments of 16 bit samples\n");
              return FALSE;
            }
          }
          else
          {
            // then we have an unimplemented number of bits per sample

            fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
            return FALSE;
          }
        }
      }
      else if (sampling->number_of_segments == 3)
      {
        // then we have exactly three segments

        if (sampling->bits_for_number_of_samples == 0)
        {
          // then we have a fixed number of samples per segment  (a "fixed sampling" and *not* a varying one)

          if (sampling->bits_per_sample == 8)
          {
            // then we have 8 bits per sample

            samplings[m] = new WAVESsamplingThreeSegment8bitFixed();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating three fixed segments of %d 8 bit samples\n", sampling->number_of_samples);
              return FALSE;
            }
          }
          else if (sampling->bits_per_sample == 16)
          {
            // then we have 16 bits per sample

            samplings[m] = new WAVESsamplingThreeSegment16bitFixed();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating three fixed segments of %d 16 bit samples\n", sampling->number_of_samples);
              return FALSE;
            }
          }
          else
          {
            // then we have an unimplemented number of bits per sample

            fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
            return FALSE;
          }
        }
        else
        {
          // then we have a variable number of samples per segment  (a "variable sampling" and *not* a fixed one)

          if (sampling->bits_per_sample == 8)
          {
            // then we have 8 bits per sample

            samplings[m] = new WAVESsamplingThreeSegment8bitVariable();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating three variable segments of 8 bit samples\n");
              return FALSE;
            }
          }
          else if (sampling->bits_per_sample == 16)
          {
            // then we have 16 bits per sample

            samplings[m] = new WAVESsamplingThreeSegment16bitVariable();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating three variable segments of 16 bit samples\n");
              return FALSE;
            }
          }
          else
          {
            // then we have an unimplemented number of bits per sample

            fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
            return FALSE;
          }
        }
      }
      else
      {
        // then we have more than three segments

        if (sampling->bits_for_number_of_samples == 0)
        {
          // then we have a fixed number of samples per segment  (a "fixed sampling" and *not* a varying one)

          if (sampling->bits_per_sample == 8)
          {
            // then we have 8 bits per sample

            samplings[m] = new WAVESsamplingFixedMultiSegment8bitFixed();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating fixed number of fixed segments of %d 8 bit samples\n", sampling->number_of_samples);
              return FALSE;
            }
          }
          else if (sampling->bits_per_sample == 16)
          {
            // then we have 16 bits per sample

            samplings[m] = new WAVESsamplingFixedMultiSegment16bitFixed();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating fixed number of fixed segments of %d 16 bit samples\n", sampling->number_of_samples);
              return FALSE;
            }
          }
          else
          {
            // then we have an unimplemented number of bits per sample

            fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
            return FALSE;
          }
        }
        else
        {
          // then we have a variable number of samples per segment  (a "variable sampling" and *not* a fixed one)

          if (sampling->bits_per_sample == 8)
          {
            // then we have 8 bits per sample

            samplings[m] = new WAVESsamplingFixedMultiSegment8bitVariable();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating fixed number of variable segments of 8 bit samples\n");
              return FALSE;
            }
          }
          else if (sampling->bits_per_sample == 16)
          {
            // then we have 16 bits per sample

            samplings[m] = new WAVESsamplingFixedMultiSegment16bitVariable();
            if (samplings[m] == 0)
            {
              fprintf(stderr,"ERROR: allocating fixed number of variable segments of 16 bit samples\n");
              return FALSE;
            }
          }
          else
          {
            // then we have an unimplemented number of bits per sample

            fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
            return FALSE;
          }
        }
      }
    }
    else // this is the most flexible configuration where the number of segments per sampling can vary
    {
      if (sampling->bits_for_number_of_samples == 0)
      {
        // then we have a fixed number of samples per segment  (a "fixed sampling" and *not* a varying one)

        if (sampling->bits_per_sample == 8)
        {
          // then we have 8 bits per sample

          samplings[m] = new WAVESsamplingVaryingMultiSegment8bitFixed();
          if (samplings[m] == 0)
          {
            fprintf(stderr,"ERROR: allocating varying number of fixed segments of %d 8 bit samples\n", sampling->number_of_samples);
            return FALSE;
          }
        }
        else if (sampling->bits_per_sample == 16)
        {
          // then we have 16 bits per sample

          samplings[m] = new WAVESsamplingVaryingMultiSegment16bitFixed();
          if (samplings[m] == 0)
          {
            fprintf(stderr,"ERROR: allocating varying number of fixed segments of %d 16 bit samples\n", sampling->number_of_samples);
            return FALSE;
          }
        }
        else
        {
          // then we have an unimplemented number of bits per sample

          fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
          return FALSE;
        }
      }
      else
      {
        // then we have a variable number of samples per segment  (a "variable sampling" and *not* a fixed one)

        if (sampling->bits_per_sample == 8)
        {
          // then we have 8 bits per sample

          samplings[m] = new WAVESsamplingVaryingMultiSegment8bitVariable();
          if (samplings[m] == 0)
          {
            fprintf(stderr,"ERROR: allocating varying number of variable segments of 8 bit samples\n");
            return FALSE;
          }
        }
        else if (sampling->bits_per_sample == 16)
        {
          // then we have 16 bits per sample

          samplings[m] = new WAVESsamplingVaryingMultiSegment16bitVariable();
          if (samplings[m] == 0)
          {
            fprintf(stderr,"ERROR: allocating varying number of variable segments of 16 bit samples\n");
            return FALSE;
          }
        }
        else
        {
          // then we have an unimplemented number of bits per sample

          fprintf(stderr,"ERROR: %d bits_per_sample are not implemented\n", sampling->bits_per_sample);
          return FALSE;
        }
      }
    }
    
    // initialize the sampling

    if (!samplings[m]->init(sampling))
    {
      fprintf(stderr,"ERROR: initializing sampling %d\n", m);
      return FALSE;
    }
  }

  return TRUE;
};

void WAVESwaves::clean()
{
  if (samplings)
  {
    for (U16 m = 0; m < number_of_samplings; m++)
    {
      if (samplings[m]) delete samplings[m];
    }
    delete [] samplings;
  }
  memset(this, 0, sizeof(WAVESwaves));
};

BOOL WAVESwaves::save_to_txt(FILE* file) const
{
  if (file == 0) return FALSE;

  fprintf(file, "W\012");
  if (samplings)
  {
    for (U16 m = 0; m < number_of_samplings; m++)
    {
      fprintf(file, "m\012");
      for (U16 s = 0; s < samplings[m]->get_number_of_segments(); s++)
      {
        samplings[m]->set_active_segment(s);
        fprintf(file, "s %d %g\012", samplings[m]->get_number_of_samples_for_segment(), samplings[m]->get_duration_from_anchor_for_segment());
        for (U16 i = 0; i < samplings[m]->get_number_of_samples_for_segment(); i++)
        {
          fprintf(file, " %d", samplings[m]->get_sample(i));    
        }
        fprintf(file, "\012");
      }
    }
  }
  return TRUE;
};

WAVESwaves::WAVESwaves()
{
  samplings = 0;
  clean();
};

WAVESwaves::~WAVESwaves()
{
  clean();
};
