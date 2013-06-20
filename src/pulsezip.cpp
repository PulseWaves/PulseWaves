/*
===============================================================================

  FILE:  pulsezip.cpp
  
  CONTENTS:
  
    see corresponding header file
  
  PROGRAMMERS:

    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com

  COPYRIGHT:

    (c) 2007-2012, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    see corresponding header file
  
===============================================================================
*/
#include "pulsezip.hpp"
#include "mydefs.hpp"
#include "bytestreamin.hpp"
#include "bytestreamout.hpp"
#include <assert.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

PULSEzip::PULSEzip()
{
  compressor = PULSEZIP_COMPRESSOR_DEFAULT;
  coder = PULSEZIP_CODER_ARITHMETIC;
  version_major = PULSEZIP_VERSION_MAJOR;
  version_minor = PULSEZIP_VERSION_MINOR;
  version_revision = PULSEZIP_VERSION_REVISION;
  options = 0;
  num_items = 0;
  chunk_size = PULSEZIP_CHUNK_SIZE_DEFAULT;
  error_string = 0;
  items = 0;
}

PULSEzip::~PULSEzip()
{
  if (error_string) free(error_string);
  if (items) delete [] items;
}

// the data of the PULSEzip VLR
//     U16  compressor         2 bytes 
//     U16  coder              2 bytes 
//     U8   version_major      1 byte 
//     U8   version_minor      1 byte
//     U16  version_revision   2 bytes
//     U32  options            4 bytes 
//     U32  chunk_size         4 bytes
//     U16  num_items          2 bytes
//        U16 type                2 bytes * num_items
//        U16 size                2 bytes * num_items
//        U16 version             2 bytes * num_items
// which totals 18+6*num_items

// load (usually from the data of a VLR)
BOOL PULSEzip::load(ByteStreamIn* stream)
{
  U16 i;
  // load
  stream->get16bitsLE((U8*)&compressor);
  stream->get16bitsLE((U8*)&coder);
  version_major = stream->getByte();
  version_minor = stream->getByte();
  stream->get16bitsLE((U8*)&options);
  stream->get32bitsLE((U8*)&version_revision);
  stream->get32bitsLE((U8*)&chunk_size);
  stream->get16bitsLE((U8*)&num_items);

  // create item list
  if (items) delete [] items;
  items = new PULSEitem[num_items];

  // load the items
  for (i = 0; i < num_items; i++)
  {
    U16 type;
    stream->get16bitsLE((U8*)&type);    
    items[i].type = (PULSEitem::Type)type;
    stream->get16bitsLE((U8*)&(items[i].size));    
    stream->get16bitsLE((U8*)&(items[i].version));    
  }

  // check if we support the contents
  for (i = 0; i < num_items; i++)
  {
    if (!check_item(&items[i])) return FALSE;
  }
  return TRUE;
}

I64 PULSEzip::get_payload() const
{
  return 18+num_items*6;
}

// save to VLR data
BOOL PULSEzip::save(ByteStreamOut* stream) const
{
  // save
  stream->put16bitsLE((U8*)&compressor);
  stream->put16bitsLE((U8*)&coder);
  stream->putByte(version_major);
  stream->putByte(version_minor);
  stream->put16bitsLE((U8*)&version_revision);
  stream->put32bitsLE((U8*)&options);
  stream->put32bitsLE((U8*)&chunk_size);
  stream->put16bitsLE((U8*)&num_items);
  for (U16 i = 0; i < num_items; i++)
  {
    U16 type = (U16)items[i].type;
    stream->put16bitsLE((U8*)&type);
    stream->put16bitsLE((U8*)&(items[i].size));
    stream->put16bitsLE((U8*)&(items[i].version));
  }
  return TRUE;
}

const char* PULSEzip::get_error() const
{
  return error_string;
}

BOOL PULSEzip::return_error(const char* error)
{
  char err[256];
  sprintf(err, "%s (PULSEzip %d.%d r%d)", error, PULSEZIP_VERSION_MAJOR, PULSEZIP_VERSION_MINOR, PULSEZIP_VERSION_REVISION);
  if (error_string) free(error_string);
  error_string = strdup(err);
  return FALSE;
}

BOOL PULSEzip::check_compressor(const U16 compressor)
{
  if (compressor < PULSEZIP_COMPRESSOR_TOTAL) return TRUE;
  char error[64];
  sprintf(error, "compressor %d not supported", compressor);
  return return_error(error);
}

BOOL PULSEzip::check_coder(const U16 coder)
{
  if (coder < PULSEZIP_CODER_TOTAL) return TRUE;
  char error[64];
  sprintf(error, "coder %d not supported", coder);
  return return_error(error);
}

BOOL PULSEzip::check_item(const PULSEitem* item)
{
  switch (item->type)
  {
  case PULSEitem::PULSE0:
    if (item->size != PULSEWAVES_PULSE0_SIZE) return return_error("PULSE0 has size != PULSEWAVES_PULSE0_SIZE");
    if (item->version > 1) return return_error("PULSE0 has version > 1");
    break;
  case PULSEitem::PULSESOURCEID16:
    if (item->size != PULSEWAVES_PULSESOURCEID16_SIZE) return return_error("PULSESOURCEID16 has size != PULSEWAVES_PULSESOURCEID16_SIZE");
    if (item->version > 1) return return_error("PULSESOURCEID16 has version > 1");
    break;
  case PULSEitem::PULSESOURCEID32:
    if (item->size != PULSEWAVES_PULSESOURCEID32_SIZE) return return_error("PULSESOURCEID32 has size != PULSEWAVES_PULSESOURCEID32_SIZE");
    if (item->version > 1) return return_error("PULSESOURCEID32 has version > 1");
    break;
  case PULSEitem::EXTRABYTES:
    if (item->size == 0) return return_error("EXTRABYTES has zero size");
    if (item->version > 1) return return_error("EXTRABYTES has version > 1");
    break;
  default:
    if (1)
    {
      char error[64];
      sprintf(error, "item unknown (%d,%d,%d)", item->type, item->size, item->version);
      return return_error(error);
    }
  }
  return TRUE;
}

BOOL PULSEzip::check_items(const U16 num_items, const PULSEitem* items)
{
  if (num_items == 0) return return_error("number of items cannot be zero");
  if (items == 0) return return_error("items pointer cannot be NULL");
  U16 i;
  for (i = 0; i < num_items; i++)
  {
    if (!check_item(&items[i])) return FALSE;
  }
  return TRUE;
}

BOOL PULSEzip::check()
{
  if (!check_compressor(compressor)) return FALSE;
  if (!check_coder(coder)) return FALSE;
  if (!check_items(num_items, items)) return FALSE;
  return TRUE;
}

BOOL PULSEzip::setup(const U32 format, const U32 attributes, const U32 size, const U32 compression)
{
  if (!check_compressor((U16)compression)) return FALSE;
  this->num_items = 0;
  if (this->items) delete [] this->items;
  this->items = 0;
  if (!setup(&num_items, &items, format, attributes, size, compression)) return FALSE;
  this->compressor = (U16)compression;
  if (chunk_size == 0) chunk_size = PULSEZIP_CHUNK_SIZE_DEFAULT;
  return TRUE;
}

BOOL PULSEzip::setup(const U16 num_items, const PULSEitem* items, const U32 compression)
{
  // check input
  if (!check_compressor((U16)compression)) return FALSE;
  if (!check_items(num_items, items)) return FALSE;

  // setup compressor
  this->compressor = (U16)compression;
  if (chunk_size == 0) chunk_size = PULSEZIP_CHUNK_SIZE_DEFAULT;

  // prepare items
  this->num_items = 0;
  if (this->items) delete [] this->items;
  this->items = 0;
  this->num_items = num_items;
  this->items = new PULSEitem[num_items];

  // setup items
  U16 i;
  for (i = 0; i < num_items; i++)
  {
    this->items[i] = items[i];
  }

  return TRUE;
}

BOOL PULSEzip::setup(U16* num_items, PULSEitem** items, const U32 format, const U32 attributes, const U32 size, const U32 compression)
{
  I32 extra_bytes_number = 0;

  // switch over the point types that we know
  switch (format)
  {
  case PULSEWAVES_PULSE0:
    extra_bytes_number = (I32)size - PULSEWAVES_PULSE0_SIZE;
    break;
  default:
    if (1)
    {
      char error[64];
      sprintf(error, "pulse format %d unknown", format);
      return return_error(error);
    }
  }

  BOOL have_pulse_source_ID = FALSE;
  // check for attributes that we know
  if (attributes)
  {
    if (attributes & PULSEWAVES_PULSESOURCEID16)
    {
      have_pulse_source_ID = TRUE;
      extra_bytes_number = (I32)size - PULSEWAVES_PULSESOURCEID16_SIZE;
    }
    else if (attributes & PULSEWAVES_PULSESOURCEID32)
    {
      have_pulse_source_ID = TRUE;
      extra_bytes_number = (I32)size - PULSEWAVES_PULSESOURCEID32_SIZE;
    }
  }

  if (extra_bytes_number < 0)
  {
    char error[64];
    sprintf(error, "size %u too small for format %u and attributes %u by %d bytes", size, format, attributes, -extra_bytes_number);
    return return_error(error);
  }

  // create item description

  (*num_items) = 1 + !!(have_pulse_source_ID) + !!(extra_bytes_number);
  (*items) = new PULSEitem[*num_items];

  U16 i = 1;
  (*items)[0].type = PULSEitem::PULSE0;
  (*items)[0].size = PULSEWAVES_PULSE0_SIZE;
  (*items)[0].version = 0;
  if (have_pulse_source_ID)
  {
    if (attributes & PULSEWAVES_PULSESOURCEID16)
    {
      (*items)[i].type = PULSEitem::PULSESOURCEID16;
      (*items)[i].size = PULSEWAVES_PULSESOURCEID16_SIZE;
      (*items)[i].version = 0;
      i++;
    }
    else
    {
      (*items)[i].type = PULSEitem::PULSESOURCEID32;
      (*items)[i].size = PULSEWAVES_PULSESOURCEID32_SIZE;
      (*items)[i].version = 0;
      i++;
    }
  }
  if (compression) request_version(1);
  assert(i == *num_items);
  return TRUE;
}

BOOL PULSEzip::set_chunk_size(const U32 chunk_size)
{
  if (num_items == 0) return return_error("call setup() before setting chunk size");
  this->chunk_size = chunk_size;
  return TRUE;
}

BOOL PULSEzip::request_version(const U16 requested_version)
{
  if (num_items == 0) return return_error("call setup() before requesting version");
  if (compressor == PULSEZIP_COMPRESSOR_NONE)
  {
    if (requested_version > 0) return return_error("without compression version is always 0");
  }
  else
  {
    if (requested_version < 1) return return_error("with compression version is at least 1");
    if (requested_version > 1) return return_error("version larger than 1 not supported");
  }
  U16 i;
  for (i = 0; i < num_items; i++)
  {
    switch (items[i].type)
    {
    case PULSEitem::PULSE0:
    case PULSEitem::PULSESOURCEID16:
    case PULSEitem::PULSESOURCEID32:
    case PULSEitem::EXTRABYTES:
        items[i].version = requested_version;
        break;
    default:
        return return_error("item type not supported");
    }
  }
  return TRUE;
}

BOOL PULSEzip::is_standard(U32 *format, U32 *attributes, U32 *size)
{
  return is_standard(num_items, items, format, attributes, size);
}

BOOL PULSEzip::is_standard(const U16 num_items, const PULSEitem* items, U32 *format, U32 *attributes, U32 *size)
{
  U16 i;

  if (num_items == 0) return return_error("number of PULSEitem entries is zero");
  if (items == 0) return return_error("PULSEitem array is zero");

  if (format) *format = PULSEWAVES_PULSE0;
  if (attributes) *attributes = 0;
  if (size)
  {
    *size = 0;
    for (i = 0; i < num_items; i++)
    {
      *size += items[i].size;
    }
  }

  if (items[0].is_type(PULSEitem::PULSE0))
  {
    if (items[0].size != PULSEWAVES_PULSE0_SIZE)
    {
      return_error("PULSEitem PULSE0 has wrong size");
    }
  }
  else
  {
    return_error("first PULSEitem is not PULSE0");
  }
  
  i = 1;
  while (i < num_items)
  {
    if (items[i].is_type(PULSEitem::PULSESOURCEID16))
    {
      if (items[i].size != PULSEWAVES_PULSESOURCEID16_SIZE)
      {
        return_error("PULSEitem PULSESOURCEID16 has wrong size");
      }
      if (attributes) *attributes |= PULSEWAVES_PULSESOURCEID16;
    }
    else if (items[i].is_type(PULSEitem::PULSESOURCEID32))
    {
      if (items[i].size != PULSEWAVES_PULSESOURCEID32_SIZE)
      {
        return_error("PULSEitem PULSESOURCEID32 has wrong size");
      }
      if (attributes) *attributes |= PULSEWAVES_PULSESOURCEID32;
    }
    else if (items[i].is_type(PULSEitem::EXTRABYTES))
    {
      if (items[i].size == 0)
      {
        return_error("PULSEitem EXTRABYTES has zero size");
      }
    }
    i++;
  }
  return TRUE;
}

BOOL PULSEitem::is_type(PULSEitem::Type t) const
{
  if (t != type) return FALSE;
  switch (t)
  {
  case PULSE0:
      if (size != PULSEWAVES_PULSE0_SIZE) return FALSE;
      break;
  case PULSESOURCEID16:
      if (size != PULSEWAVES_PULSESOURCEID16_SIZE) return FALSE;
      break;
  case PULSESOURCEID32:
      if (size != PULSEWAVES_PULSESOURCEID32_SIZE) return FALSE;
      break;
  case EXTRABYTES:
      if (size == 0) return FALSE;
      break;
  default:
      return FALSE;
  }
  return TRUE;
}

const char* PULSEitem::get_name() const
{
  switch (type)
  {
  case PULSE0:
      return "PULSE0";
      break;
  case PULSESOURCEID16:
      return "PULSESOURCEID16";
      break;
  case PULSESOURCEID32:
      return "PULSESOURCEID32";
      break;
  case EXTRABYTES:
      return "EXTRABYTES";
      break;
  default:
      break;
  }
  return 0;
}

