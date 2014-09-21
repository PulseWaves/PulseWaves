/*
===============================================================================

  FILE:  pulsewritewaves_raw.cpp
  
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

#include "pulsewritewaves_raw.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

BOOL PULSEwriteWaves_raw::init(ByteStreamOut* outstream)
{
  if (outstream == 0)
  {
    fprintf(stderr,"ERROR: outstream pointer is zero\n");
    return FALSE;
  }
  this->outstream = outstream;
  return TRUE;
}

BOOL PULSEwriteWaves_raw::write(const WAVESwaves* waves)
{
  if (waves == 0)
  {
    fprintf(stderr,"ERROR: waves pointer is zero\n");
    return FALSE;
  }

  // waves can potentially store additional bytes at the beginning of the waves data

  if (waves->get_number_of_extra_bytes())
  {
    try { outstream->putBytes(waves->get_extra_bytes(), waves->get_number_of_extra_bytes()); } catch(...)
    {
      fprintf(stderr,"ERROR: writing %d extra waves bytes\n", waves->get_number_of_extra_bytes());
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
        U8 number_of_segments = (U8)(sampling->get_number_of_segments());
        try { outstream->putByte(number_of_segments); } catch(...)
        {
          return FALSE;
        }
      }
      else if (sampling->get_bits_for_number_of_segments() == 16)
      {
        U16 number_of_segments = (U16)(sampling->get_number_of_segments());
        try { outstream->put16bitsLE((U8*)&number_of_segments); } catch(...)
        {
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

      // some samplings store a varying distance to the anchor per segment

      if (sampling->get_bits_for_duration_from_anchor())
      {
        if (sampling->get_bits_for_duration_from_anchor() == 16)
        {
          I16 duration_from_anchor = (I16)(sampling->get_quantized_duration_from_anchor_for_segment());
          try { outstream->put16bitsLE((U8*)&duration_from_anchor); } catch(...)
          {
            return FALSE;
          }
        }
        else if (sampling->get_bits_for_duration_from_anchor() == 32)
        {
          I32 duration_from_anchor = (I32)(sampling->get_quantized_duration_from_anchor_for_segment());
          try { outstream->put32bitsLE((U8*)&duration_from_anchor); } catch(...)
          {
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
          U8 number_of_samples = (U8)(sampling->get_number_of_samples_for_segment());
          try { outstream->putByte(number_of_samples); } catch(...)
          {
            return FALSE;
          }
        }
        else if (sampling->get_bits_for_number_of_samples() == 16)
        {
          U16 number_of_samples = (U16)(sampling->get_number_of_samples_for_segment());
          try { outstream->put16bitsLE((U8*)&number_of_samples); } catch(...)
          {
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
        const U8* samples = (const U8*)sampling->get_samples();
        try { outstream->putBytes(samples, number_of_samples); } catch(...)
        {
          return FALSE;
        }
      }
      else if (sampling->get_bits_per_sample() == 16)
      {
        if (IS_LITTLE_ENDIAN())
        {
          const U8* samples = (const U8*)sampling->get_samples();
          try { outstream->putBytes(samples, 2*number_of_samples); } catch(...)
          {
            return FALSE;
          }
        }
        else
        {
          const U16* samples = (const U16*)sampling->get_samples();
          for (i = 0; i < number_of_samples; i++)
          {
            try { outstream->put16bitsLE((U8*)&(samples[i])); } catch(...)
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

PULSEwriteWaves_raw::PULSEwriteWaves_raw()
{
  outstream = 0;
}

PULSEwriteWaves_raw::~PULSEwriteWaves_raw()
{
}
