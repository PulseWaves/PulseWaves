/*
===============================================================================

  FILE:  pulsereadwaves_compressed.cpp
  
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

#include "pulsereadwaves_compressed.hpp"

#include "arithmeticdecoder.hpp"
#include "integercompressor.hpp"
#include "pulsewavesdefinitions.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

BOOL PULSEreadWaves_compressed::init(ByteStreamIn* instream)
{
  if (instream == 0)
  {
    fprintf(stderr,"ERROR: instream pointer is zero\n");
    return FALSE;
  }
  this->instream = instream;
  if (dec) delete dec;
  dec = new ArithmeticDecoder();
  if (dec == 0)
  {
    fprintf(stderr,"ERROR: allocating ArithmeticDecoder\n");
    return FALSE;
  }
  if (ic) delete ic;
  ic = new IntegerCompressor(dec, 32);
  if (ic == 0)
  {
    fprintf(stderr,"ERROR: allocating IntegerCompressor\n");
    return FALSE;
  }
  if (ic8) delete ic8;
  ic8 = new IntegerCompressor(dec, 8, 2);
  if (ic8 == 0)
  {
    fprintf(stderr,"ERROR: allocating IntegerCompressor 8 bit\n");
    return FALSE;
  }
  if (ic16) delete ic16;
  ic16 = new IntegerCompressor(dec, 16, 2);
  if (ic16 == 0)
  {
    fprintf(stderr,"ERROR: allocating IntegerCompressor 16 bit\n");
    return FALSE;
  }
  return TRUE;
}

BOOL PULSEreadWaves_compressed::read(WAVESwaves* waves)
{
  if (waves == 0)
  {
    fprintf(stderr,"ERROR: waves pointer is zero\n");
    return FALSE;
  }

  if (instream == 0)
  {
    fprintf(stderr,"ERROR: instream pointer is zero\n");
    return FALSE;
  }

  if (dec == 0)
  {
    fprintf(stderr,"ERROR: dec pointer is zero\n");
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

  // initialize the entropy coder

  BOOL success = FALSE;

  try { success = dec->init(instream); } catch(...)
  {
    fprintf(stderr,"ERROR: initializing ArithmeticDecoder from instream\n");
    return FALSE;
  }

  if (!success)
  {
    fprintf(stderr,"ERROR: initializing ArithmeticDecoder\n");
    return FALSE;
  }

  // init the integer decompressor

  ic->initDecompressor();

  // now loop over all samplings

  I32 m, s, i;
  WAVESsampling* sampling;

  U32 context;
  I32 curr_duration, last_duration = 0;
  I32 curr_nsamples, last_nsamples = 0;
  I32 curr_sample, last_sample;

  for (m = 0; m < waves->get_number_of_samplings(); m++)
  {
    sampling = waves->get_sampling(m);

    // some samplings store a varying number of segments

    if (sampling->get_bits_for_number_of_segments())
    {
      I32 number_of_segments;
      try { number_of_segments = ic->decompress(1); } catch(...)
      {
        return FALSE;
      }
      if (!sampling->set_number_of_segments(number_of_segments))
      {
        fprintf(stderr,"ERROR: setting number of segments to %d for sampling %d\n", number_of_segments, m);
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
        try { curr_duration = ic->decompress(last_duration); } catch(...)
        {
          return FALSE;
        }
        if (!sampling->set_quantized_duration_from_anchor_for_segment(curr_duration))
        {
          fprintf(stderr,"ERROR: setting duration from anchor to %d for segment %d of sampling %d\n", curr_duration, s, m);
          return FALSE;
        }
        last_duration = curr_duration;
      }

      // some samplings store a varying number of samples per segment 

      if (sampling->get_bits_for_number_of_samples())
      {
        try { curr_nsamples = ic->decompress(last_nsamples); } catch(...)
        {
          return FALSE;
        }
        if (!sampling->set_number_of_samples_for_segment(curr_nsamples))
        {
          fprintf(stderr,"ERROR: setting number of samples to %d for segment %d of sampling %d\n", curr_nsamples, s, m);
          return FALSE;
        }
        last_nsamples = curr_nsamples;
      }

      // pick arithmetic context

      if (sampling->get_type() == PULSEWAVES_RETURNING)
      {
        context = 0;
      }
      else
      {
        context = 1;
      }

      // pick integer compressor & decompress samples
      last_sample = 0;
      if (sampling->get_bits_per_sample() == 8)
      {
        U8* samples = sampling->get_samples();
        ic8->initDecompressor();
        for (i = 0; i < sampling->get_number_of_samples(); i++)
        {
          try { curr_sample = ic8->decompress(last_sample, context); } catch(...)
          {
            return FALSE;
          }
          samples[i] = (U8)curr_sample;
          last_sample = curr_sample;
        }
      }
      else if (sampling->get_bits_per_sample() == 16)
      {
        U16* samples = (U16*)sampling->get_samples();
        ic16->initDecompressor();
        for (i = 0; i < sampling->get_number_of_samples(); i++)
        {
          try { curr_sample = ic16->decompress(last_sample, context); } catch(...)
          {
            return FALSE;
          }
          samples[i] = (U16)curr_sample;
          last_sample = curr_sample;
        }
      }
      else
      {
        fprintf(stderr,"ERROR: %d bits_per_sample not supported\n", sampling->get_bits_per_sample());
        return FALSE;
      }
    }
  }

  // flush the entropy decoder
  dec->done();

  return TRUE;
}

PULSEreadWaves_compressed::PULSEreadWaves_compressed()
{
  instream = 0;
  dec = 0;
  ic = ic8 = ic16 = 0;
}

PULSEreadWaves_compressed::~PULSEreadWaves_compressed()
{
  if (ic) delete ic;
  if (ic8) delete ic8;
  if (ic16) delete ic16;
  if (dec) delete dec;
}
