/*
===============================================================================

  FILE:  pulsereadpulse.cpp
  
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

#include "pulsereadpulse.hpp"

#include "pulsezip.hpp"
#include "arithmeticdecoder.hpp"
#include "pulsereaditemraw.hpp"
#include "pulsereaditemcompressed_v1.hpp"

#include <stdlib.h>
#include <string.h>

PULSEreadPulse::PULSEreadPulse()
{
  pulse_size = 0;
  instream = 0;
  num_readers = 0;
  readers = 0;
  readers_raw = 0;
  readers_compressed = 0;
  dec = 0;
  // used for chunking
  chunk_size = U32_MAX;
  chunk_count = 0;
  current_chunk = 0;
  number_chunks = 0;
  tabled_chunks = 0;
  chunk_totals = 0;
  chunk_starts = 0;
  // used for seeking
  pulse_start = 0;
  seek_pulse = 0;
}

BOOL PULSEreadPulse::setup(U32 num_items, const PULSEitem* items, const PULSEzip* pulsezip)
{
  U32 i;

  // is pulsezip exists then we must use its items
  if (pulsezip)
  {
    if (num_items != pulsezip->num_items) return FALSE;
    if (items != pulsezip->items) return FALSE;
  }

  // create entropy decoder (if requested)
  dec = 0;

  if (pulsezip && pulsezip->compressor)
  {
    switch (pulsezip->coder)
    {
    case PULSEZIP_CODER_ARITHMETIC:
      dec = new ArithmeticDecoder();
      break;
    default:
      // entropy decoder not supported
      return FALSE;
    }
  }
 
  // initizalize the readers
  readers = 0;
  num_readers = num_items;

  // disable chunking
  chunk_size = U32_MAX;

  // always create the raw readers
  readers_raw = new PULSEreadItem*[num_readers];
  for (i = 0; i < num_readers; i++)
  {
    switch (items[i].type)
    {
    case PULSEitem::PULSE0:
      if (IS_LITTLE_ENDIAN())
        readers_raw[i] = new PULSEreadItemRaw_PULSE0_LE();
      else
        readers_raw[i] = new PULSEreadItemRaw_PULSE0_BE();
      break;
    case PULSEitem::PULSESOURCEID16:
      if (IS_LITTLE_ENDIAN())
        readers_raw[i] = new PULSEreadItemRaw_PULSESOURCEID16_LE();
      else
        readers_raw[i] = new PULSEreadItemRaw_PULSESOURCEID16_BE();
      break;
    case PULSEitem::PULSESOURCEID32:
      if (IS_LITTLE_ENDIAN())
        readers_raw[i] = new PULSEreadItemRaw_PULSESOURCEID32_LE();
      else
        readers_raw[i] = new PULSEreadItemRaw_PULSESOURCEID32_BE();
      break;
    case PULSEitem::EXTRABYTES:
      readers_raw[i] = new PULSEreadItemRaw_EXTRABYTES(items[i].size);
      break;
    default:
      return FALSE;
    }
    pulse_size += items[i].size;
  }

  if (dec)
  {
    readers_compressed = new PULSEreadItem*[num_readers];
    // seeks with compressed data need a seek pulse
    if (seek_pulse)
    {
      delete [] seek_pulse[0];
      delete [] seek_pulse;
    }
    seek_pulse = new U8*[num_items];
    if (!seek_pulse) return FALSE;
    seek_pulse[0] = new U8[pulse_size];
    if (!seek_pulse[0]) return FALSE;
    for (i = 0; i < num_readers; i++)
    {
      switch (items[i].type)
      {
      case PULSEitem::PULSE0:
        if (items[i].version == 1)
          readers_compressed[i] = new PULSEreadItemCompressed_PULSE0_v1(dec);
        else
          return FALSE;
        break;
      case PULSEitem::PULSESOURCEID16:
        if (items[i].version == 1)
          readers_compressed[i] = new PULSEreadItemCompressed_PULSESOURCEID16_v1(dec);
        else
          return FALSE;
        break;
      case PULSEitem::PULSESOURCEID32:
        if (items[i].version == 1)
          readers_compressed[i] = new PULSEreadItemCompressed_PULSESOURCEID32_v1(dec);
        else
          return FALSE;
        break;
      case PULSEitem::EXTRABYTES:
        if (items[i].version == 1)
          readers_compressed[i] = new PULSEreadItemCompressed_EXTRABYTES_v1(dec, items[i].size);
        else
          return FALSE;
        break;
      default:
        return FALSE;
      }
      if (i) seek_pulse[i] = seek_pulse[i-1]+items[i-1].size;
    }
    chunk_size = pulsezip->chunk_size;
    number_chunks = U32_MAX;
  }
  return TRUE;
}

BOOL PULSEreadPulse::init(ByteStreamIn* instream)
{
  if (!instream) return FALSE;
  this->instream = instream;

  // on very first init with chunking enabled
  if (number_chunks == U32_MAX)
  {
    if (!read_chunk_table())
    {
      return FALSE;
    }
    current_chunk = 0;
    if (chunk_totals) chunk_size = chunk_totals[1];
  }

  pulse_start = instream->tell();

  U32 i;
  for (i = 0; i < num_readers; i++)
  {
    ((PULSEreadItemRaw*)(readers_raw[i]))->init(instream);
  }

  if (dec)
  {
    readers = 0;
  }
  else
  {
    readers = readers_raw;
  }

  return TRUE;
}

BOOL PULSEreadPulse::seek(const U32 current, const U32 target)
{
  if (!instream->isSeekable()) return FALSE;
  U32 delta = 0;
  if (dec)
  {
    if (chunk_starts)
    {
      U32 target_chunk;
      if (chunk_totals)
      {
        target_chunk = search_chunk_table(target, 0, number_chunks);
        chunk_size = chunk_totals[target_chunk+1]-chunk_totals[target_chunk];
        delta = target - chunk_totals[target_chunk];
      }
      else
      {
        target_chunk = target/chunk_size;
        delta = target%chunk_size;
      }
      if (target_chunk >= tabled_chunks)
      {
        if (current_chunk < (tabled_chunks-1))
        {
          dec->done();
          current_chunk = (tabled_chunks-1);
          instream->seek(chunk_starts[current_chunk]);
          init(instream);
          chunk_count = 0;
        }
        delta += (chunk_size*(target_chunk-current_chunk) - chunk_count);
      }
      else if (current_chunk != target_chunk || current > target)
      {
        dec->done();
        current_chunk = target_chunk;
        instream->seek(chunk_starts[current_chunk]);
        init(instream);
        chunk_count = 0;
      }
      else
      {
        delta = target - current;
      }
    }
    else if (current > target)
    {
      dec->done();
      instream->seek(pulse_start);
      init(instream);
      delta = target;
    }
    else if (current < target)
    {
      delta = target - current;
    }
    while (delta)
    {
      read(seek_pulse);
      delta--;
    }
  }
  else
  {
    if (current != target)
    {
      instream->seek(pulse_start+pulse_size*target);
    }
  }
  return TRUE;
}

BOOL PULSEreadPulse::read(U8* const * pulse)
{
  U32 i;

  try
  {
    if (dec)
    {
      if (chunk_count == chunk_size)
      {
        current_chunk++;
        dec->done();
        init(instream);
        if (tabled_chunks == current_chunk) // no or incomplete chunk table?
        {
          if (current_chunk == number_chunks)
          {
            number_chunks += 256;
            chunk_starts = (I64*)realloc(chunk_starts, sizeof(I64)*number_chunks);
          }
          chunk_starts[tabled_chunks] = pulse_start; // needs fixing
          tabled_chunks++;
        }
        else if (chunk_totals) // variable sized chunks?
        {
          chunk_size = chunk_totals[current_chunk+1]-chunk_totals[current_chunk];
        }
        chunk_count = 0;
      }
      chunk_count++;

      if (readers)
      {
        for (i = 0; i < num_readers; i++)
        {
          readers[i]->read(pulse[i]);
        }
      }
      else
      {
        for (i = 0; i < num_readers; i++)
        {
          readers_raw[i]->read(pulse[i]);
          ((PULSEreadItemCompressed*)(readers_compressed[i]))->init(pulse[i]);
        }
        readers = readers_compressed;
        dec->init(instream);
      }
    }
    else
    {
      for (i = 0; i < num_readers; i++)
      {
        readers[i]->read(pulse[i]);
      }
    }
  }
  catch (...)
  {
    return FALSE;
  }
  return TRUE;
}

BOOL PULSEreadPulse::done()
{
  if (readers == readers_compressed)
  {
    if (dec) dec->done();
  }
  return TRUE;
}

BOOL PULSEreadPulse::read_chunk_table()
{
  // read the 8 bytes that store the location of the chunk table
  I64 chunk_table_start_position;
  try { instream->get64bitsLE((U8*)&chunk_table_start_position); } catch(...)
  {
    return FALSE;
  }

  // this is where the chunks start
  I64 chunks_start = instream->tell();

  if ((chunk_table_start_position + 8) == chunks_start)
  {
    // then compressor was interrupted before getting a chance to write the chunk table
    number_chunks = 256;
    chunk_starts = (I64*)malloc(sizeof(I64)*number_chunks);
    if (chunk_starts == 0)
    {
      return FALSE;
    }
    chunk_starts[0] = chunks_start;
    tabled_chunks = 1;
    return TRUE;
  }

  if (!instream->isSeekable())
  {
    // if the stream is not seekable we cannot seek to the chunk table but won't need it anyways
    tabled_chunks = 0;
    return TRUE;
  }

  if (chunk_table_start_position == -1)
  {
    // the compressor was writing to a non-seekable stream and wrote the chunk table start at the end
    if (!instream->seekEnd(8))
    {
      return FALSE;
    }
    try { instream->get64bitsLE((U8*)&chunk_table_start_position); } catch(...)
    {
      return FALSE;
    }
  }

  // read the chunk table
  try
  {
    instream->seek(chunk_table_start_position);
    U32 version;
    instream->get32bitsLE((U8*)&version);
    if (version != 0)
    {
      throw;
    }
    instream->get32bitsLE((U8*)&number_chunks);
    if (chunk_totals) delete [] chunk_totals;
    chunk_totals = 0;
    if (chunk_starts) free(chunk_starts);
    chunk_starts = 0;
    if (chunk_size == U32_MAX)
    {
      chunk_totals = new U32[number_chunks+1];
      if (chunk_totals == 0)
      {
        throw;
      }
      chunk_totals[0] = 0;
    }
    chunk_starts = (I64*)malloc(sizeof(I64)*(number_chunks+1));
    if (chunk_starts == 0)
    {
      throw;
    }
    chunk_starts[0] = chunks_start;
    tabled_chunks = 1;
    if (number_chunks > 0)
    {
      U32 i;
      dec->init(instream);
      IntegerCompressor ic(dec, 32, 2);
      ic.initDecompressor();
      for (i = 1; i <= number_chunks; i++)
      {
        if (chunk_size == U32_MAX) chunk_totals[i] = ic.decompress((i>1 ? chunk_totals[i-1] : 0), 0);
        chunk_starts[i] = ic.decompress((i>1 ? (U32)(chunk_starts[i-1]) : 0), 1);
        tabled_chunks++;
      }
      dec->done();
      for (i = 1; i <= number_chunks; i++)
      {
        if (chunk_size == U32_MAX) chunk_totals[i] += chunk_totals[i-1];
        chunk_starts[i] += chunk_starts[i-1];
      }
    }
  }
  catch (...)
  {
    // something went wrong while reading the chunk table
    if (chunk_totals) delete [] chunk_totals;
    chunk_totals = 0;
    // fix as many additional chunk_starts as possible
    U32 i;
    for (i = 1; i < tabled_chunks; i++)
    {
      chunk_starts[i] += chunk_starts[i-1];
    }
  }
  if (!instream->seek(chunks_start))
  {
    return FALSE;
  }
  return TRUE;
}

U32 PULSEreadPulse::search_chunk_table(const U32 index, const U32 lower, const U32 upper)
{
  if (lower + 1 == upper) return lower;
  U32 mid = (lower+upper)/2;
  if (index >= chunk_totals[mid])
    return search_chunk_table(index, mid, upper);
  else
    return search_chunk_table(index, lower, mid);
}

PULSEreadPulse::~PULSEreadPulse()
{
  U32 i;

  if (readers_raw)
  {
    for (i = 0; i < num_readers; i++)
    {
      delete readers_raw[i];
    }
    delete [] readers_raw;
  }

  if (readers_compressed)
  {
    for (i = 0; i < num_readers; i++)
    {
      delete readers_compressed[i];
    }
    delete [] readers_compressed;
  }

  if (dec)
  {
    delete dec;
  }

  if (chunk_totals) delete [] chunk_totals;
  if (chunk_starts) delete [] chunk_starts;

  if (seek_pulse)
  {
    delete [] seek_pulse[0];
    delete [] seek_pulse;
  }
}
