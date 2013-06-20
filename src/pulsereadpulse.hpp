/*
===============================================================================

  FILE:  pulsereadpulse.hpp
  
  CONTENTS:
  
    Common interface for the classes that read pulses raw or compressed.

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
  
    22 February 2012 -- created at Mamio Verde 500 meters above a lit up valley
  
===============================================================================
*/
#ifndef PULSE_READ_PULSE_HPP
#define PULSE_READ_PULSE_HPP

#include "pulsereaditem.hpp"

class PULSEzip;
class ByteStreamIn;
class EntropyDecoder;

class PULSEreadPulse
{
public:
  PULSEreadPulse();
  ~PULSEreadPulse();

  // should only be called *once*
  BOOL setup(const U32 num_items, const PULSEitem* items, const PULSEzip* pulsezip=0);

  BOOL init(ByteStreamIn* instream);
  BOOL seek(const U32 current, const U32 target);
  BOOL read(U8* const * pulse);
  BOOL done();

private:
  ByteStreamIn* instream;
  U32 num_readers;
  PULSEreadItem** readers;
  PULSEreadItem** readers_raw;
  PULSEreadItem** readers_compressed;
  EntropyDecoder* dec;
  // used for chunking
  U32 chunk_size;
  U32 chunk_count;
  U32 current_chunk;
  U32 number_chunks;
  U32 tabled_chunks;
  I64* chunk_starts;
  U32* chunk_totals;
  BOOL read_chunk_table();
  U32 search_chunk_table(const U32 index, const U32 lower, const U32 upper);
  // used for seeking
  I64 pulse_start;
  U32 pulse_size;
  U8** seek_pulse;
};

#endif
