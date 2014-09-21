/*
===============================================================================

  FILE:  pulsewritewaves_compressed.cpp
  
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

#include "pulsewritewaves_compressed.hpp"

#include "arithmeticencoder.hpp"
#include "integercompressor.hpp"
#include "pulsewavesdefinitions.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

BOOL PULSEwriteWaves_compressed::init(ByteStreamOut* outstream)
{
  if (outstream == 0)
  {
    fprintf(stderr,"ERROR: outstream pointer is zero\n");
    return FALSE;
  }
  this->outstream = outstream;
  if (enc) delete enc;
  enc = new ArithmeticEncoder();
  if (enc == 0)
  {
    fprintf(stderr,"ERROR: allocating ArithmeticEncoder\n");
    return FALSE;
  }
  if (ic) delete ic;
  ic = new IntegerCompressor(enc, 32);
  if (ic == 0)
  {
    fprintf(stderr,"ERROR: allocating IntegerCompressor\n");
    return FALSE;
  }
  if (ic8) delete ic8;
  ic8 = new IntegerCompressor(enc, 8, 2);
  if (ic8 == 0)
  {
    fprintf(stderr,"ERROR: allocating IntegerCompressor 8 bit\n");
    return FALSE;
  }
  if (ic16) delete ic16;
  ic16 = new IntegerCompressor(enc, 16, 2);
  if (ic16 == 0)
  {
    fprintf(stderr,"ERROR: allocating IntegerCompressor 16 bit\n");
    return FALSE;
  }
  return TRUE;
}

BOOL PULSEwriteWaves_compressed::write(const WAVESwaves* waves)
{
  if (waves == 0)
  {
    fprintf(stderr,"ERROR: waves pointer is zero\n");
    return FALSE;
  }

  if (outstream == 0)
  {
    fprintf(stderr,"ERROR: outstream pointer is zero\n");
    return FALSE;
  }

  if (enc == 0)
  {
    fprintf(stderr,"ERROR: enc pointer is zero\n");
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

  // initialize the entropy coder

  if (!enc->init(outstream))
  {
    fprintf(stderr,"ERROR: initializing ArithmeticEncoder\n");
    return FALSE;
  }

  // init the integer compressor

  ic->initCompressor();

  // loop over all samplings and compress

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
      I32 number_of_segments = sampling->get_number_of_segments();
      try { ic->compress(1, number_of_segments); } catch(...)
      {
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
        curr_duration = sampling->get_quantized_duration_from_anchor_for_segment();
        try { ic->compress(last_duration, curr_duration); } catch(...)
        {
          return FALSE;
        }
        last_duration = curr_duration;
      }

      // some samplings store a varying number of samples per segment 

      if (sampling->get_bits_for_number_of_samples())
      {
        curr_nsamples = sampling->get_number_of_samples_for_segment();
        try { ic->compress(last_nsamples, curr_nsamples); } catch(...)
        {
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

      // pick integer compressor

      IntegerCompressor* used_ic;
      if (sampling->get_bits_per_sample() == 8)
      {
        ic8->initCompressor();
        used_ic = ic8;
      }
      else if (sampling->get_bits_per_sample() == 16)
      {
        ic16->initCompressor();
        used_ic = ic16;
      }
      else
      {
        used_ic = ic;
      }

      // compress samples

      last_sample = 0;
      for (i = 0; i < sampling->get_number_of_samples(); i++)
      {
        curr_sample = sampling->get_sample(i);
        try { used_ic->compress(last_sample, curr_sample, context); } catch(...)
        {
          return FALSE;
        }
        last_sample = curr_sample; 
      }
    }
  }

  // flush the entropy encoder
  enc->done();

  return TRUE;
}

PULSEwriteWaves_compressed::PULSEwriteWaves_compressed()
{
  outstream = 0;
  enc = 0;
  ic = ic8 = ic16 = 0;
}

PULSEwriteWaves_compressed::~PULSEwriteWaves_compressed()
{
  if (ic) delete ic;
  if (ic8) delete ic8;
  if (ic16) delete ic16;
  if (enc) delete enc;
}
