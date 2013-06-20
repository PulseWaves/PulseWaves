/*
===============================================================================

  FILE:  pulsewritepulse.cpp
  
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

#include "pulsewritepulse.hpp"

#include "pulsezip.hpp"
#include "arithmeticencoder.hpp"
#include "pulsewriteitemraw.hpp"
#include "pulsewriteitemcompressed_v1.hpp"

#include <string.h>
#include <stdlib.h>

PULSEwritePulse::PULSEwritePulse()
{
  outstream = 0;
  num_writers = 0;
  writers = 0;
  writers_raw = 0;
  writers_compressed = 0;
  enc = 0;
  // used for chunking
  chunk_size = U32_MAX;
  chunk_count = 0;
  number_chunks = 0;
  alloced_chunks = 0;
  chunk_sizes = 0;
  chunk_bytes = 0;
  chunk_table_start_position = 0;
  chunk_start_position = 0;
}

BOOL PULSEwritePulse::setup(const U32 num_items, const PULSEitem* items, const PULSEzip* pulsezip)
{
  U32 i;

  // is pulsezip exists then we must use its items
  if (pulsezip)
  {
    if (num_items != pulsezip->num_items) return FALSE;
    if (items != pulsezip->items) return FALSE;
  }

  // create entropy encoder (if requested)
  enc = 0;

  if (pulsezip && pulsezip->compressor)
  {
    switch (pulsezip->coder)
    {
    case PULSEZIP_CODER_ARITHMETIC:
      enc = new ArithmeticEncoder();
      break;
    default:
      // entropy decoder not supported
      return FALSE;
    }
  }

  // initizalize the writers
  writers = 0;
  num_writers = num_items;

  // disable chunking
  chunk_size = U32_MAX;

  // always create the raw writers
  writers_raw = new PULSEwriteItem*[num_writers];
  for (i = 0; i < num_writers; i++)
  {
    switch (items[i].type)
    {
    case PULSEitem::PULSE0:
      if (IS_LITTLE_ENDIAN())
        writers_raw[i] = new PULSEwriteItemRaw_PULSE0_LE();
      else
        writers_raw[i] = new PULSEwriteItemRaw_PULSE0_BE();
      break;
    case PULSEitem::PULSESOURCEID16:
      if (IS_LITTLE_ENDIAN())
        writers_raw[i] = new PULSEwriteItemRaw_PULSESOURCEID16_LE();
      else
        writers_raw[i] = new PULSEwriteItemRaw_PULSESOURCEID16_BE();
      break;
    case PULSEitem::PULSESOURCEID32:
      if (IS_LITTLE_ENDIAN())
        writers_raw[i] = new PULSEwriteItemRaw_PULSESOURCEID32_LE();
      else
        writers_raw[i] = new PULSEwriteItemRaw_PULSESOURCEID32_BE();
      break;
    case PULSEitem::EXTRABYTES:
      writers_raw[i] = new PULSEwriteItemRaw_EXTRABYTES(items[i].size);
      break;
    default:
      return FALSE;
    }
  }

  // if needed create the compressed writers and set versions
  if (enc)
  {
    writers_compressed = new PULSEwriteItem*[num_writers];
    for (i = 0; i < num_writers; i++)
    {
      switch (items[i].type)
      {
      case PULSEitem::PULSE0:
        if (items[i].version == 1)
          writers_compressed[i] = new PULSEwriteItemCompressed_PULSE0_v1(enc);
        else
          return FALSE;
        break;
      case PULSEitem::PULSESOURCEID16:
        if (items[i].version == 1)
          writers_compressed[i] = new PULSEwriteItemCompressed_PULSESOURCEID16_v1(enc);
        else
          return FALSE;
        break;
      case PULSEitem::PULSESOURCEID32:
        if (items[i].version == 1)
          writers_compressed[i] = new PULSEwriteItemCompressed_PULSESOURCEID32_v1(enc);
        else
          return FALSE;
        break;
      case PULSEitem::EXTRABYTES:
        if (items[i].version == 1)
         writers_compressed[i] = new PULSEwriteItemCompressed_EXTRABYTES_v1(enc, items[i].size);
        else
          return FALSE;
        break;
      default:
        return FALSE;
      }
    }
    if (pulsezip->chunk_size) chunk_size = pulsezip->chunk_size;
    chunk_count = 0;
    number_chunks = U32_MAX;
  }
  return TRUE;
}

BOOL PULSEwritePulse::init(ByteStreamOut* outstream)
{
  if (!outstream) return FALSE;
  this->outstream = outstream;

  // if chunking is enabled
  if (number_chunks == U32_MAX)
  {
    number_chunks = 0;
    if (outstream->isSeekable())
    {
      chunk_table_start_position = outstream->tell();
    }
    else
    {
      chunk_table_start_position = -1;
    }
    outstream->put64bitsLE((U8*)&chunk_table_start_position);
    chunk_start_position = outstream->tell();
  }

  U32 i;
  for (i = 0; i < num_writers; i++)
  {
    ((PULSEwriteItemRaw*)(writers_raw[i]))->init(outstream);
  }

  if (enc)
  {
    writers = 0;
  }
  else
  {
    writers = writers_raw;
  }

  return TRUE;
}

BOOL PULSEwritePulse::write(const U8 * const * pulse)
{
  U32 i;

  if (chunk_count == chunk_size)
  {
    enc->done();
    add_chunk_to_table();
    init(outstream);
    chunk_count = 0;
  }
  chunk_count++;

  if (writers)
  {
    for (i = 0; i < num_writers; i++)
    {
      writers[i]->write(pulse[i]);
    }
  }
  else
  {
    for (i = 0; i < num_writers; i++)
    {
      writers_raw[i]->write(pulse[i]);
      ((PULSEwriteItemCompressed*)(writers_compressed[i]))->init(pulse[i]);
    }
    writers = writers_compressed;
    enc->init(outstream);
  }
  return TRUE;
}

BOOL PULSEwritePulse::chunk()
{
  if (chunk_start_position == 0 || chunk_size != U32_MAX)
  {
    return FALSE;
  }
  enc->done();
  add_chunk_to_table();
  init(outstream);
  chunk_count = 0;
  return TRUE;
}

BOOL PULSEwritePulse::done()
{
  if (writers == writers_compressed)
  {
    enc->done();
    if (chunk_start_position)
    {
      if (chunk_count) add_chunk_to_table();
      return write_chunk_table();
    }
  }
  else if (writers == 0)
  {
    if (chunk_start_position)
    {
      return write_chunk_table();
    }
  }

  return TRUE;
}

BOOL PULSEwritePulse::add_chunk_to_table()
{
  if (number_chunks == alloced_chunks)
  {
    if (chunk_bytes == 0)
    {
      alloced_chunks = 1024;
      if (chunk_size == U32_MAX) chunk_sizes = (U32*)malloc(sizeof(U32)*alloced_chunks); 
      chunk_bytes = (U32*)malloc(sizeof(U32)*alloced_chunks); 
    }
    else
    {
      alloced_chunks *= 2;
      if (chunk_size == U32_MAX) chunk_sizes = (U32*)realloc(chunk_sizes, sizeof(U32)*alloced_chunks); 
      chunk_bytes = (U32*)realloc(chunk_bytes, sizeof(U32)*alloced_chunks); 
    }
    if (chunk_size == U32_MAX && chunk_sizes == 0) return FALSE;
    if (chunk_bytes == 0) return FALSE;
  }
  I64 position = outstream->tell();
  if (chunk_size == U32_MAX) chunk_sizes[number_chunks] = chunk_count;
  chunk_bytes[number_chunks] = (U32)(position - chunk_start_position);
  chunk_start_position = position;
  number_chunks++;
  return TRUE;
}

BOOL PULSEwritePulse::write_chunk_table()
{
  U32 i;
  I64 position = outstream->tell();
  if (chunk_table_start_position != -1) // stream is seekable
  {
    if (!outstream->seek(chunk_table_start_position))
    {
      return FALSE;
    }
    try { outstream->put64bitsLE((U8*)&position); } catch(...)
    {
      return FALSE;
    }
    if (!outstream->seek(position))
    {
      return FALSE;
    }
  }
  U32 version = 0;
  try { outstream->put32bitsLE((U8*)&version); } catch(...)
  {
    return FALSE;
  }
  try { outstream->put32bitsLE((U8*)&number_chunks); } catch(...)
  {
    return FALSE;
  }
  if (number_chunks > 0)
  {
    enc->init(outstream);
    IntegerCompressor ic(enc, 32, 2);
    ic.initCompressor();
    for (i = 0; i < number_chunks; i++)
    {
      if (chunk_size == U32_MAX) ic.compress((i ? chunk_sizes[i-1] : 0), chunk_sizes[i], 0);
      ic.compress((i ? chunk_bytes[i-1] : 0), chunk_bytes[i], 1);
    }
    enc->done();
  }
  if (chunk_table_start_position == -1) // stream is not-seekable
  {
    try { outstream->put64bitsLE((U8*)&position); } catch(...)
    {
      return FALSE;
    }
  }
  return TRUE;
}

PULSEwritePulse::~PULSEwritePulse()
{
  U32 i;

  if (writers_raw)
  {
    for (i = 0; i < num_writers; i++)
    {
      delete writers_raw[i];
    }
    delete [] writers_raw;
  }
  if (writers_compressed)
  {
    for (i = 0; i < num_writers; i++)
    {
      delete writers_compressed[i];
    }
    delete [] writers_compressed;
  }
  if (enc)
  {
    delete enc;
  }

  if (chunk_bytes) free(chunk_bytes);
}
