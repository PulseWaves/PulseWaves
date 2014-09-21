/*
===============================================================================

  FILE:  pulsewritepulse.hpp
  
  CONTENTS:
  
    Common interface for the classes that write pulses raw or compressed.

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
#ifndef PULSE_WRITE_PULSE_HPP
#define PULSE_WRITE_PULSE_HPP

#include "pulsewriteitem.hpp"

class PULSEzip;
class ByteStreamOut;
class ArithmeticEncoder;

class PULSEwritePulse
{
public:
  PULSEwritePulse();
  ~PULSEwritePulse();

  // should only be called *once*
  BOOL setup(const U32 num_items, const PULSEitem* items, const PULSEzip* pulsezip=0);

  BOOL init(ByteStreamOut* outstream);
  BOOL write(const U8 * const * pulse);
  BOOL chunk();
  BOOL done();

private:
  ByteStreamOut* outstream;
  U32 num_writers;
  PULSEwriteItem** writers;
  PULSEwriteItem** writers_raw;
  PULSEwriteItem** writers_compressed;
  ArithmeticEncoder* enc;
  // used for chunking
  U32 chunk_size;
  U32 chunk_count;
  U32 number_chunks;
  U32 alloced_chunks;
  U32* chunk_sizes;
  U32* chunk_bytes;
  I64 chunk_start_position;
  I64 chunk_table_start_position;
  BOOL add_chunk_to_table();
  BOOL write_chunk_table();
};

#endif
