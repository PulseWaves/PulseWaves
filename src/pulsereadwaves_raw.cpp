/*
===============================================================================

  FILE:  pulsereadwaves_raw.cpp
  
  CONTENTS:
  
    see corresponding header file
  
  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2010-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    see corresponding header file
  
===============================================================================
*/

#include "pulsereadwaves_raw.hpp"

#include "bytestreamin.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

BOOL PULSEreadWaves_raw::init(ByteStreamIn* instream)
{
  if (instream == 0)
  {
    fprintf(stderr,"ERROR: instream pointer is zero\n");
    return FALSE;
  }
  this->instream = instream;
  return TRUE;
}

BOOL PULSEreadWaves_raw::read(WAVESwaves* waves)
{
  if (waves == 0)
  {
    fprintf(stderr,"ERROR: waves pointer is zero\n");
    return FALSE;
  }

  // waves can potentially store additional bytes at the beginning of the waves data

  if (waves->get_number_of_extra_bytes())
  {
    try { instream->getBytes(waves->get_extra_bytes(), waves->get_number_of_extra_bytes()); } catch(...)
    {
      fprintf(stderr,"ERROR: reading %d extra waves bytes\n", waves->get_number_of_extra_bytes());
      return FALSE;
    }
  }

  // now loop over all samplings

  I32 m, s, i;
  WAVESsampling* sampling;
  for (m = 0; m < waves->get_number_of_samplings(); m++)
  {
    sampling = waves->get_sampling(m);

    // some samplings store a varying number of segments

    if (sampling->get_bits_for_number_of_segments())
    {
      if (sampling->get_bits_for_number_of_segments() == 8)
      {
        U8 number_of_segments;
        try { number_of_segments = instream->getByte(); } catch(...)
        {
          fprintf(stderr,"ERROR: cannot read byte for number_of_segments in sampling %d\n", m);
          return FALSE;
        }
        if (!sampling->set_number_of_segments(number_of_segments))
        {
          fprintf(stderr,"ERROR: cannot set number_of_segments %d for sampling %d\n", number_of_segments, m);
          return FALSE;
        }
      }
      else if (sampling->get_bits_for_number_of_segments() == 16)
      {
        U16 number_of_segments;
        try { instream->get16bitsLE((U8*)&number_of_segments); } catch(...)
        {
          fprintf(stderr,"ERROR: cannot read word for number_of_segments in sampling %d\n", m);
          return FALSE;
        }
        if (!sampling->set_number_of_segments(number_of_segments))
        {
          fprintf(stderr,"ERROR: cannot set number_of_segments %d for sampling %d\n", number_of_segments, m);
          return FALSE;
        }
      }
      else
      {
        fprintf(stderr,"ERROR: %d bits_for_number_of_segments not supported\n", sampling->get_bits_for_number_of_segments());
        return FALSE;
      }
    }

    // loop over the segments

    for (s = 0; s < sampling->get_number_of_segments(); s++)
    {

      // all subsequent calls to sampling will be in regards to segment s

      sampling->set_active_segment(s);

      // some samplings store a varying duration to the anchor per segment

      if (sampling->get_bits_for_duration_from_anchor())
      {
        if (sampling->get_bits_for_duration_from_anchor() == 16)
        {
          I16 duration_from_anchor;
          try { instream->get16bitsLE((U8*)&duration_from_anchor); } catch(...)
          {
            fprintf(stderr,"ERROR: cannot read word for duration_from_anchor of segment %d in sampling %d\n", s, m);
            return FALSE;
          }
          if (!sampling->set_quantized_duration_from_anchor_for_segment(duration_from_anchor))
          {
            fprintf(stderr,"ERROR: cannot set duration_from_anchor %d of segment %d in sampling %d\n", duration_from_anchor, s, m);
            return FALSE;
          }
        }
        else if (sampling->get_bits_for_duration_from_anchor() == 32)
        {
          I32 duration_from_anchor;
          try { instream->get32bitsLE((U8*)&duration_from_anchor); } catch(...)
          {
            fprintf(stderr,"ERROR: cannot read doubleword for duration_from_anchor of segment %d in sampling %d\n", s, m);
            return FALSE;
          }
          if (!sampling->set_quantized_duration_from_anchor_for_segment(duration_from_anchor))
          {
            fprintf(stderr,"ERROR: cannot set duration_from_anchor %d of segment %d in sampling %d\n", duration_from_anchor, s, m);
            return FALSE;
          }
        }
        else
        {
          fprintf(stderr,"ERROR: %d bits_for_duration_from_anchor not supported\n", sampling->get_bits_for_duration_from_anchor());
          return FALSE;
        }
      }

      // some samplings store a varying number of samples per segment 

      if (sampling->get_bits_for_number_of_samples())
      {
        if (sampling->get_bits_for_number_of_samples() == 8)
        {
          U8 number_of_samples;
          try { number_of_samples = instream->getByte(); } catch(...)
          {
            fprintf(stderr,"ERROR: cannot read byte for number_of_samples of segment %d in sampling %d\n", s, m);
            return FALSE;
          }
          if (!sampling->set_number_of_samples_for_segment(number_of_samples))
          {
            fprintf(stderr,"ERROR: cannot set number_of_samples %d for segment %d in sampling %d\n", number_of_samples, s, m);
            return FALSE;
          }
        }
        else if (sampling->get_bits_for_number_of_samples() == 16)
        {
          U16 number_of_samples;
          try { instream->get16bitsLE((U8*)&number_of_samples); } catch(...)
          {
            fprintf(stderr,"ERROR: cannot read word for number_of_samples of segment %d in sampling %d\n", s, m);
            return FALSE;
          }
          if (!sampling->set_number_of_samples_for_segment(number_of_samples))
          {
            fprintf(stderr,"ERROR: cannot set number_of_samples %d for segment %d in sampling %d\n", number_of_samples, s, m);
            return FALSE;
          }
        }
        else
        {
          fprintf(stderr,"ERROR: %d bits_for_number_of_samples not supported\n", sampling->get_bits_for_number_of_samples());
          return FALSE;
        }
      }

      I32 number_of_samples = sampling->get_number_of_samples_for_segment();
      if (sampling->get_bits_per_sample() == 8)
      {
        U8* samples = sampling->get_samples();
        try { instream->getBytes(samples, number_of_samples); } catch(...)
        {
          return FALSE;
        }
      }
      else if (sampling->get_bits_per_sample() == 16)
      {
        if (IS_LITTLE_ENDIAN())
        {
          U8* samples = sampling->get_samples();
          try { instream->getBytes(samples, 2*number_of_samples); } catch(...)
          {
            return FALSE;
          }
        }
        else
        {
          U16* samples = (U16*)sampling->get_samples();
          for (i = 0; i < number_of_samples; i++)
          {
            try { instream->get16bitsLE((U8*)&(samples[i])); } catch(...)
            {
              return FALSE;
            }
          }
        }
      }
      else
      {
        fprintf(stderr,"ERROR: %d bits_per_sample not supported\n", sampling->get_bits_per_sample());
        return FALSE;
      }
    }
  }

  return TRUE;
}

PULSEreadWaves_raw::PULSEreadWaves_raw()
{
  instream = 0;
}

PULSEreadWaves_raw::~PULSEreadWaves_raw()
{
}
