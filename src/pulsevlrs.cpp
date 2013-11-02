/*
===============================================================================

  FILE:  pulsevlrs.cpp
  
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
#include "pulsevlrs.hpp"

#include "bytestreamin.hpp"
#include "bytestreamout.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PULSEvlr::PULSEvlr()
{
  memset(this, 0, sizeof(PULSEvlr));
}

PULSEvlr::PULSEvlr(const CHAR* user_id, U32 record_id, const CHAR* description, I64 record_length_after_header, U8* data)
{
  memset(this, 0, sizeof(PULSEvlr));
  if (user_id)
  {
    strncpy(this->user_id, user_id, PULSEWAVES_USER_ID_SIZE-1);
  }
  this->record_id = record_id;
  if (description)
  {
    strncpy(this->description, description, PULSEWAVES_DESCRIPTION_SIZE-1);
  }
  this->record_length_after_header = record_length_after_header;
  this->data = data;
}

PULSEvlr::~PULSEvlr()
{
  if (data)
  {
    delete [] data;
  }
}

BOOL PULSEvlr::load(ByteStreamIn* stream)
{
  // read variable length records variable after variable (to avoid alignment issues)

  try { stream->getBytes((U8*)user_id, PULSEWAVES_USER_ID_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading user_id of VLR\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&record_id); } catch(...)
  {
    fprintf(stderr,"ERROR: reading record_id of VLR\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: reading reserved of VLR\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&record_length_after_header); } catch(...)
  {
    fprintf(stderr,"ERROR: reading record_length_after_header of VLR\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading description of VLR\n");
    return FALSE;
  }

  if (data) delete [] data;

  if (record_length_after_header)
  {
    data = new U8[(U32)record_length_after_header];
    
    if (data == 0)
    {
      fprintf(stderr,"ERROR: could not alloc %u bytes of data for VLR\n", (U32)record_length_after_header);
      return FALSE;
    }

    // special handling for known variable header tags

    BOOL loaded = FALSE;

    if (strncmp(user_id, "PulseWaves_Proj", PULSEWAVES_USER_ID_SIZE) == 0)
    {
      U32 i, num;

      if (record_id == 34735) // GeoKeyDirectoryTag
      {
        if ((record_length_after_header % PULSEWAVES_GEO_KEY_SIZE) != 0)
        {
          fprintf(stderr,"ERROR: payload of VLR containing geo keys is %u bytes, not a multiple of %d\n", (U32)record_length_after_header, PULSEWAVES_GEO_KEY_SIZE);
          return FALSE;
        }
        num = (U32)(record_length_after_header / PULSEWAVES_GEO_KEY_SIZE);

        // load four U16 one after the other for each geo key entry

        for (i = 0; i < num; i++)
        {
          try { stream->get16bitsLE((U8*)&data[8*i+0]); } catch(...)
          {
            fprintf(stderr,"ERROR: reading short 0 of key entry %u of geo key VLR\n", i);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&data[8*i+2]); } catch(...)
          {
            fprintf(stderr,"ERROR: reading short 1 of key entry %u of geo key VLR\n", i);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&data[8*i+4]); } catch(...)
          {
            fprintf(stderr,"ERROR: reading short 2 of key entry %u of geo key VLR\n", i);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&data[8*i+6]); } catch(...)
          {
            fprintf(stderr,"ERROR: reading short 3 of key entry %u of geo key VLR\n", i);
            return FALSE;
          }
        }
        loaded = TRUE;
      }
      else if (record_id == 34736) // GeoDoubleParamsTag
      {
        if ((record_length_after_header % 8) != 0)
        {
          fprintf(stderr,"ERROR: payload of VLR containing geo double params is %u bytes, not a multiple of 8\n", (U32)record_length_after_header);
          return FALSE;
        }
        num = (U32)(record_length_after_header / 8);

        // load num F64 one after the other

        for (i = 0; i < num; i++)
        {
          try { stream->get64bitsLE((U8*)&data[8*i]); } catch(...)
          {
            fprintf(stderr,"ERROR: reading double %u of geo double params VLR\n", i);
            return FALSE;
          }
        }
        loaded = TRUE;
      }
    }

    if (!loaded)
    {
      try { stream->getBytes((U8*)data, (U32)record_length_after_header); } catch(...)
      {
        fprintf(stderr,"ERROR: reading %u bytes of data for VLR\n", (U32)record_length_after_header);
        return FALSE;
      }
    }
  }
  else
  {
    data = 0;
  }

  // check variable length record contents

  if (reserved != 0x0)
  {
    fprintf(stderr,"WARNING: wrong reserved in VLR: %d != 0x0\n", reserved);
  }

  return TRUE;
};

BOOL PULSEvlr::save(ByteStreamOut* stream) const
{
  // check variable length records contents

  if (reserved != 0x0)
  {
    fprintf(stderr,"WARNING: wrong reserved: %d != 0x0\n", reserved);
  }

  // write variable length records variable after variable (to avoid alignment issues)

  try { stream->putBytes((U8*)user_id, PULSEWAVES_USER_ID_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing user_id of VLR\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&record_id); } catch(...)
  {
    fprintf(stderr,"ERROR: writing record_id of VLR\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: writing reserved of VLR\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&record_length_after_header); } catch(...)
  {
    fprintf(stderr,"ERROR: writing record_length_after_header of VLR\n");
    return FALSE;
  }
  try { stream->putBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing description of VLR\n");
    return FALSE;
  }

  // write the data following the header of the variable length record

  if (record_length_after_header)
  {
    if (data)
    {
      try { stream->putBytes((U8*)data, (U32)record_length_after_header); } catch(...)
      {
        fprintf(stderr,"ERROR: writing %u bytes of data for VLR\n", (U32)record_length_after_header);
        return FALSE;
      }
    }
    else
    {
      fprintf(stderr,"ERROR: there should be %u bytes of data in VLR\n", (U32)record_length_after_header);
      return FALSE;
    }
  }
  return TRUE;
};

BOOL PULSEvlr::save_to_txt(FILE* file) const
{
  return TRUE;
}

PULSEavlr::PULSEavlr()
{
  memset(this, 0, sizeof(PULSEavlr));
}

PULSEavlr::PULSEavlr(const CHAR* user_id, U32 record_id, const CHAR* description)
{
  memset(this, 0, sizeof(PULSEavlr));
  if (user_id)
  {
    strncpy(this->user_id, user_id, PULSEWAVES_USER_ID_SIZE);
  }
  this->record_id = record_id;
  if (description)
  {
    strncpy(this->description, description, PULSEWAVES_DESCRIPTION_SIZE);
  }
}

PULSEavlr::~PULSEavlr()
{
  if (data)
  {
    delete [] data;
  }
}

BOOL PULSEavlr::load(ByteStreamIn* stream)
{
  // read variable length records variable after variable (to avoid alignment issues)

  try { stream->getBytes((U8*)user_id, PULSEWAVES_USER_ID_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading user_id of AVLR\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&record_id); } catch(...)
  {
    fprintf(stderr,"ERROR: reading record_id of AVLR\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: reading reserved of AVLR\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&record_length_before_footer); } catch(...)
  {
    fprintf(stderr,"ERROR: reading record_length_before_footer of AVLR\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading description of AVLR\n");
    return FALSE;
  }

  if (data) delete [] data;

  if (record_length_before_footer)
  {
    data = new U8[(I32)record_length_before_footer];
    
    if (data == 0)
    {
      fprintf(stderr,"ERROR: could not alloc %u bytes of data for AVLR\n", (U32)record_length_before_footer);
      return FALSE;
    }

    try { stream->getBytes((U8*)data, (U32)record_length_before_footer); } catch(...)
    {
      fprintf(stderr,"ERROR: reading %u bytes of data for AVLR\n", (U32)record_length_before_footer);
      return FALSE;
    }
  }
  else
  {
    data = 0;
  }

  // check variable length record contents

  if (reserved != 0x0)
  {
    fprintf(stderr,"WARNING: wrong reserved in VLR: %d != 0x0\n", reserved);
  }

  return TRUE;
};

BOOL PULSEavlr::save(ByteStreamOut* stream) const
{
  // check variable length records contents

  if (reserved != 0x0)
  {
    fprintf(stderr,"WARNING: wrong reserved: %d != 0x0\n", reserved);
  }

  // write the data preceding the footer of the appended variable length record

  if (record_length_before_footer)
  {
    if (data)
    {
      try { stream->putBytes((U8*)data, (U32)record_length_before_footer); } catch(...)
      {
        fprintf(stderr,"ERROR: writing %u bytes of data for AVLR\n", (U32)record_length_before_footer);
        return FALSE;
      }
    }
    else
    {
      fprintf(stderr,"ERROR: there should be %u bytes of data in AVLR\n", (U32)record_length_before_footer);
      return FALSE;
    }
  }

  // write variable length records variable after variable (to avoid alignment issues)

  try { stream->putBytes((U8*)user_id, PULSEWAVES_USER_ID_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing user_id of AVLR\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&record_id); } catch(...)
  {
    fprintf(stderr,"ERROR: writing record_id of AVLR\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: writing reserved of AVLR\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&record_length_before_footer); } catch(...)
  {
    fprintf(stderr,"ERROR: writing record_length_before_footer of AVLR\n");
    return FALSE;
  }
  try { stream->putBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing description of AVLR\n");
    return FALSE;
  }

  return TRUE;
};

BOOL PULSEavlr::save_to_txt(FILE* file) const
{
  return TRUE;
}

U32 PULSEscanner::size_of_attributes() const
{
  U32 size = (2*4) + (2*PULSEWAVES_DESCRIPTION_SIZE) + (12*4) + (1*PULSEWAVES_DESCRIPTION_SIZE);
  return size;
}

BOOL PULSEscanner::load(ByteStreamIn* stream)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: reading size\n");
    return FALSE;
  }

  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSEscanner should be %u but is %u\n", size, this->size);
  }

  try { stream->get32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: reading reserved\n");
    return FALSE;
  }

  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSEscanner is %u instead of %u\n", this->reserved, reserved);
  }
  
  try { stream->getBytes((U8*)instrument, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading instrument\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)serial, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading serial\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&wave_length); } catch(...)
  {
    fprintf(stderr,"ERROR: reading wave_length\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&outgoing_pulse_width); } catch(...)
  {
    fprintf(stderr,"ERROR: reading outgoing_pulse_width\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&scan_pattern); } catch(...)
  {
    fprintf(stderr,"ERROR: reading scan_pattern\n");
    return FALSE;
  }

  if (scan_pattern > PULSEWAVES_CONIC)
  {
    fprintf(stderr,"WARNING: scan_pattern of PULSEscanner is %u\n", scan_pattern);
  }

  try { stream->get32bitsLE((U8*)&number_of_mirror_facets); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_mirror_facets\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&scan_frequency); } catch(...)
  {
    fprintf(stderr,"ERROR: reading scan_frequency\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&scan_angle_min); } catch(...)
  {
    fprintf(stderr,"ERROR: reading scan_angle_min\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&scan_angle_max); } catch(...)
  {
    fprintf(stderr,"ERROR: reading scan_angle_max\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&pulse_frequency); } catch(...)
  {
    fprintf(stderr,"ERROR: reading pulse_frequency\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&beam_diameter_at_exit_aperture); } catch(...)
  {
    fprintf(stderr,"ERROR: reading beam_diameter_at_exit_aperture\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&beam_divergence); } catch(...)
  {
    fprintf(stderr,"ERROR: reading beam_divergence\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&minimal_range); } catch(...)
  {
    fprintf(stderr,"ERROR: reading minimal_range\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&maximal_range); } catch(...)
  {
    fprintf(stderr,"ERROR: reading maximal_range\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading description\n");
    return FALSE;
  }

  return TRUE;
}

BOOL PULSEscanner::save(ByteStreamOut* stream) const
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSEscanner should be %u but is %u. ignoring %u ...\n", size, this->size, this->size);
  }

  try { stream->put32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: writing size\n");
    return FALSE;
  }

  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSEscanner should be %u but is %u. ignoring %u ...\n", reserved, this->reserved, this->reserved);
  }

  try { stream->put32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: writing reserved\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)instrument, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing instrument\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)serial, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing serial\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&wave_length); } catch(...)
  {
    fprintf(stderr,"ERROR: writing wave_length\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&outgoing_pulse_width); } catch(...)
  {
    fprintf(stderr,"ERROR: writing outgoing_pulse_width\n");
    return FALSE;
  }

  if (scan_pattern > PULSEWAVES_CONIC)
  {
    fprintf(stderr,"WARNING: scan_pattern of PULSEscanner is %u\n", scan_pattern);
  }

  try { stream->put32bitsLE((U8*)&scan_pattern); } catch(...)
  {
    fprintf(stderr,"ERROR: writing scan_pattern\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&number_of_mirror_facets); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_of_mirror_facets\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&scan_frequency); } catch(...)
  {
    fprintf(stderr,"ERROR: writing scan_frequency\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&scan_angle_min); } catch(...)
  {
    fprintf(stderr,"ERROR: writing scan_angle_min\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&scan_angle_max); } catch(...)
  {
    fprintf(stderr,"ERROR: writing scan_angle_max\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&pulse_frequency); } catch(...)
  {
    fprintf(stderr,"ERROR: writing pulse_frequency\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&beam_diameter_at_exit_aperture); } catch(...)
  {
    fprintf(stderr,"ERROR: writing beam_diameter_at_exit_aperture\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&beam_divergence); } catch(...)
  {
    fprintf(stderr,"ERROR: writing beam_divergence\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&minimal_range); } catch(...)
  {
    fprintf(stderr,"ERROR: writing minimal_range\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&maximal_range); } catch(...)
  {
    fprintf(stderr,"ERROR: writing maximal_range\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing description\n");
    return FALSE;
  }

  return TRUE;
}

BOOL PULSEscanner::save_to_txt(FILE* file) const
{
  return TRUE;
}

PULSEscanner & PULSEscanner::operator=(const PULSEscanner & scanner)
{
  this->size = scanner.size;
  this->reserved = scanner.reserved;
  memcpy(this->instrument, scanner.instrument, PULSEWAVES_DESCRIPTION_SIZE);
  memcpy(this->serial, scanner.serial, PULSEWAVES_DESCRIPTION_SIZE);
  this->wave_length = scanner.wave_length;
  this->outgoing_pulse_width = scanner.outgoing_pulse_width;
  this->scan_pattern = scanner.scan_pattern;
  this->number_of_mirror_facets = scanner.number_of_mirror_facets;
  this->scan_frequency = scanner.scan_frequency;
  this->scan_angle_min = scanner.scan_angle_min;
  this->scan_angle_max = scanner.scan_angle_max;
  this->pulse_frequency = scanner.pulse_frequency;
  this->beam_diameter_at_exit_aperture = scanner.beam_diameter_at_exit_aperture;
  this->beam_divergence = scanner.beam_divergence;
  this->minimal_range = scanner.minimal_range;
  this->maximal_range = scanner.maximal_range;
  memcpy(this->description, scanner.description, PULSEWAVES_DESCRIPTION_SIZE);
  return *this;
};

PULSEscanner::PULSEscanner()
{
  memset(this, 0, sizeof(PULSEscanner));
  size = size_of_attributes();
}

/************************************************/
/************************************************/
/************************************************/

U32 PULSElookupTable::size_of_attributes() const
{
  U32 size = (3*4) + (1*2) + (2*1) + (1*PULSEWAVES_DESCRIPTION_SIZE);
  return size;
}

BOOL PULSElookupTable::load(ByteStreamIn* stream)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  if (number_entries)
  {
    delete [] entries;
    number_entries = 0;
  }

  try { stream->get32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: reading size\n");
    return FALSE;
  }

  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSElookupTable should be %u but is %u\n", size, this->size);
  }

  try { stream->get32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: reading reserved\n");
    return FALSE;
  }

  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSElookupTable is %u instead of %u\n", this->reserved, reserved);
  }

  try { stream->get32bitsLE((U8*)&number_entries); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_entries\n");
    return FALSE;
  }

  try { stream->get16bitsLE((U8*)&unit_of_measurement); } catch(...)
  {
    fprintf(stderr,"ERROR: reading unit_of_measurement\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)&data_type, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading data_type\n");
    return FALSE;
  }

  if (data_type != 8)
  {
    fprintf(stderr,"ERROR: data_type %d not supported. only float (8) is supported.\n", data_type);
    return FALSE;
  }

  try { stream->getBytes((U8*)&options, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading options\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading description\n");
    return FALSE;
  }

  entries = new U8[number_entries*sizeof(F32)];
  if (entries == 0)
  {
    fprintf(stderr,"ERROR: allocating %u entries\n", number_entries);
    return FALSE;
  }

  U32 e;
  for (e = 0; e < number_entries; e++)
  {
    try { stream->get32bitsLE(entries+e*sizeof(F32)); } catch(...)
    {
      fprintf(stderr,"ERROR: reading entry %u\n", e);
      return FALSE;
    }
  }

  return TRUE;
}

BOOL PULSElookupTable::save(ByteStreamOut* stream) const
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSElookupTable should be %u but is %u. ignoring %u ...\n", size, this->size, this->size);
  }

  try { stream->put32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: writing size\n");
    return FALSE;
  }

  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSElookupTable should be %u but is %u. ignoring %u ...\n", reserved, this->reserved, this->reserved);
  }

  try { stream->put32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: writing reserved\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&number_entries); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_entries\n");
    return FALSE;
  }

  try { stream->put16bitsLE((U8*)&unit_of_measurement); } catch(...)
  {
    fprintf(stderr,"ERROR: writing unit_of_measurement\n");
    return FALSE;
  }

  if (data_type != 8)
  {
    fprintf(stderr,"ERROR: data_type %d not supported. only float (8) is supported.\n", data_type);
    return FALSE;
  }

  try { stream->putBytes((U8*)&data_type, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: writing data_type\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)&options, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: writing options\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing description\n");
    return FALSE;
  }

  U32 e;
  for (e = 0; e < number_entries; e++)
  {
    try { stream->put32bitsLE(entries+e*sizeof(F32)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing entry %u\n", e);
      return FALSE;
    }
  }

  return TRUE;
}

BOOL PULSElookupTable::save_to_txt(FILE* file) const
{
  return TRUE;
}

PULSElookupTable::PULSElookupTable()
{
  memset(this, 0, sizeof(PULSElookupTable));
  size = size_of_attributes();
}

PULSElookupTable::~PULSElookupTable()
{
  if (number_entries)
  {
    delete [] entries;
  }
}

/************************************************/
/************************************************/
/************************************************/

U32 PULSEtable::size_of_attributes() const
{
  U32 size = (3*4) + (1*PULSEWAVES_DESCRIPTION_SIZE);
  return size;
}

BOOL PULSEtable::load(ByteStreamIn* stream)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  if (number_tables)
  {
    U32 t;
    for (t = 0; t < number_tables; t++)
    {
      delete tables[t];
    }
    delete [] tables;
    number_tables = 0;
  }

  try { stream->get32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: reading size\n");
    return FALSE;
  }

  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSEtable should be %u but is %u\n", size, this->size);
  }

  try { stream->get32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: reading reserved\n");
    return FALSE;
  }

  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSEtable is %u instead of %u\n", this->reserved, reserved);
  }

  try { stream->get32bitsLE((U8*)&number_tables); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_tables\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading description\n");
    return FALSE;
  }

  if (number_tables)
  {
    tables = new PULSElookupTable*[number_tables];
    if (tables == 0)
    {
      fprintf(stderr,"ERROR: allocating %u lookup table pointers\n", number_tables);
      return FALSE;
    }
    U32 t;
    for (t = 0; t < number_tables; t++)
    {
      tables[t] = new PULSElookupTable();
      if (tables[t] == 0)
      {
        fprintf(stderr,"ERROR: allocating lookup table %u\n", t);
        return FALSE;
      }
      if (!(tables[t]->load(stream)))
      {
        fprintf(stderr,"ERROR: reading lookup table %u\n", t);
        return FALSE;
      }
    }
  }

  return TRUE;
}

BOOL PULSEtable::save(ByteStreamOut* stream) const
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSEtable should be %u but is %u. ignoring %u ...\n", size, this->size, this->size);
  }

  try { stream->put32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: writing size\n");
    return FALSE;
  }

  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSEtable should be %u but is %u. ignoring %u ...\n", reserved, this->reserved, this->reserved);
  }

  try { stream->put32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: writing reserved\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&number_tables); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_tables\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing description\n");
    return FALSE;
  }

  U32 t;
  for (t = 0; t < number_tables; t++)
  {
    if (!(tables[t]->save(stream)))
    {
      fprintf(stderr,"ERROR: writing lookup table %u\n", t);
      return FALSE;
    }
  }

  return TRUE;
}

BOOL PULSEtable::save_to_txt(FILE* file) const
{
  return TRUE;
}

PULSEtable::PULSEtable()
{
  memset(this, 0, sizeof(PULSEtable));
  size = size_of_attributes();
}

PULSEtable::~PULSEtable()
{
  if (number_tables)
  {
    U32 t;
    for (t = 0; t < number_tables; t++)
    {
      delete tables[t];
    }
    delete [] tables;
  }
}

/*
  // write pulsezip VLR with compression parameters

  if (pulsezip)
  {
    // write variable length records variable after variable (to avoid alignment issues)

    U16 reserved = 0xAABB;
    try { stream->put16bitsLE((U8*)&(reserved)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing reserved %d\n", (I32)reserved);
      return FALSE;
    }
    U8 user_id[16] = "pulsezip encoded\0";
    try { stream->putBytes((U8*)user_id, PULSEWAVES_USER_ID_SIZE))
    {
      fprintf(stderr,"ERROR: writing user_id %s\n", user_id);
      return FALSE;
    }
    U16 record_id = 22204;
    try { stream->put16bitsLE((U8*)&(record_id)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing record_id %d\n", (I32)record_id);
      return FALSE;
    }
    U16 record_length_after_header = pulsezip_vlr_data_size;
    try { stream->put16bitsLE((U8*)&(record_length_after_header)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing record_length_after_header %d\n", (I32)record_length_after_header);
      return FALSE;
    }
    char description[PULSEWAVES_DESCRIPTION_SIZE];
    memset(description, 0, PULSEWAVES_DESCRIPTION_SIZE);
    sprintf(description, "by pulsezip of PULSEtools (%d)", PULSE_TOOLS_VERSION);  
    try { stream->putBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE))
    {
      fprintf(stderr,"ERROR: writing description %s\n", description);
      return FALSE;
    }
    // write the data following the header of the variable length record
    //     U16  compressor         2 bytes 
    //     U16  coder              2 bytes 
    //     U8   version_major      1 byte 
    //     U8   version_minor      1 byte
    //     U16  version_revision   2 bytes
    //     U32  options            4 bytes 
    //     U32  chunk_size         4 bytes
    //     I64  num_pulses         8 bytes
    //     I64  num_bytes          8 bytes
    //     U16  num_items          2 bytes
    //        U16 type                2 bytes * num_items
    //        U16 size                2 bytes * num_items
    //        U16 version             2 bytes * num_items
    // which totals 34+6*num_items

    try { stream->put16bitsLE((U8*)&(pulsezip->compressor)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing compressor %d\n", (I32)compressor);
      return FALSE;
    }
    try { stream->put16bitsLE((U8*)&(pulsezip->coder)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing coder %d\n", (I32)pulsezip->coder);
      return FALSE;
    }
    try { stream->putByte(pulsezip->version_major))
    {
      fprintf(stderr,"ERROR: writing version_major %d\n", pulsezip->version_major);
      return FALSE;
    }
    try { stream->putByte(pulsezip->version_minor))
    {
      fprintf(stderr,"ERROR: writing version_minor %d\n", pulsezip->version_minor);
      return FALSE;
    }
    try { stream->put16bitsLE((U8*)&(pulsezip->version_revision)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing version_revision %d\n", pulsezip->version_revision);
      return FALSE;
    }
    try { stream->put32bitsLE((U8*)&(pulsezip->options)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing options %d\n", (I32)pulsezip->options);
      return FALSE;
    }
    try { stream->put32bitsLE((U8*)&(pulsezip->chunk_size)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing chunk_size %d\n", pulsezip->chunk_size);
      return FALSE;
    }
    try { stream->put64bitsLE((U8*)&(pulsezip->num_pulses)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing num_pulses %d\n", (I32)pulsezip->num_pulses);
      return FALSE;
    }
    try { stream->put64bitsLE((U8*)&(pulsezip->num_bytes)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing num_bytes %d\n", (I32)pulsezip->num_bytes);
      return FALSE;
    }
    try { stream->put16bitsLE((U8*)&(pulsezip->num_items)); } catch(...)
    {
      fprintf(stderr,"ERROR: writing num_items %d\n", pulsezip->num_items);
      return FALSE;
    }
    for (i = 0; i < pulsezip->num_items; i++)
    {
      try { stream->put16bitsLE((U8*)&(pulsezip->items[i].type)); } catch(...)
      {
        fprintf(stderr,"ERROR: writing type %d of item %d\n", pulsezip->items[i].type, i);
        return FALSE;
      }
      try { stream->put16bitsLE((U8*)&(pulsezip->items[i].size)); } catch(...)
      {
        fprintf(stderr,"ERROR: writing size %d of item %d\n", pulsezip->items[i].size, i);
        return FALSE;
      }
      try { stream->put16bitsLE((U8*)&(pulsezip->items[i].version)); } catch(...)
      {
        fprintf(stderr,"ERROR: writing version %d of item %d\n", pulsezip->items[i].version, i);
        return FALSE;
      }
    }
  }
*/

/*
          header.pulsezip = new PULSEzip();

          // read this data following the header of the variable length record
          //     U16  compressor         2 bytes 
          //     U32  coder              2 bytes 
          //     U8   version_major      1 byte 
          //     U8   version_minor      1 byte
          //     U16  version_revision   2 bytes
          //     U32  options            4 bytes 
          //     I32  chunk_size         4 bytes
          //     I64  num_pulses         8 bytes
          //     I64  num_bytes          8 bytes
          //     U16  num_items          2 bytes
          //        U16 type                2 bytes * num_items
          //        U16 size                2 bytes * num_items
          //        U16 version             2 bytes * num_items
          // which totals 34+6*num_items

          try { stream->get16bitsLE((U8*)&(header.pulsezip->compressor)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading compressor %d\n", (I32)header.pulsezip->compressor);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(header.pulsezip->coder)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading coder %d\n", (I32)header.pulsezip->coder);
            return FALSE;
          }
          try { stream->getBytes((U8*)&(header.pulsezip->version_major), 1); } catch(...)
          {
            fprintf(stderr,"ERROR: reading version_major %d\n", header.pulsezip->version_major);
            return FALSE;
          }
          try { stream->getBytes((U8*)&(header.pulsezip->version_minor), 1); } catch(...)
          {
            fprintf(stderr,"ERROR: reading version_minor %d\n", header.pulsezip->version_minor);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(header.pulsezip->version_revision)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading version_revision %d\n", header.pulsezip->version_revision);
            return FALSE;
          }
          try { stream->get32bitsLE((U8*)&(header.pulsezip->options)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading options %d\n", (I32)header.pulsezip->options);
            return FALSE;
          }
          try { stream->get32bitsLE((U8*)&(header.pulsezip->chunk_size)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading chunk_size %d\n", header.pulsezip->chunk_size);
            return FALSE;
          }
          try { stream->get64bitsLE((U8*)&(header.pulsezip->num_pulses)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading num_pulses %d\n", (I32)header.pulsezip->num_pulses);
            return FALSE;
          }
          try { stream->get64bitsLE((U8*)&(header.pulsezip->num_bytes)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading num_bytes %d\n", (I32)header.pulsezip->num_bytes);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(header.pulsezip->num_items)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading num_items %d\n", header.pulsezip->num_items);
            return FALSE;
          }
          header.pulsezip->items = new PULSEitem[header.pulsezip->num_items];
          for (j = 0; j < header.pulsezip->num_items; j++)
          {
            U16 type, size, version;
            try { stream->get16bitsLE((U8*)&type); } catch(...)
            {
              fprintf(stderr,"ERROR: reading type %d of item %d\n", type, j);
              return FALSE;
            }
            try { stream->get16bitsLE((U8*)&size); } catch(...)
            {
              fprintf(stderr,"ERROR: reading size %d of item %d\n", size, j);
              return FALSE;
            }
            try { stream->get16bitsLE((U8*)&version); } catch(...)
            {
              fprintf(stderr,"ERROR: reading version %d of item %d\n", version, j);
              return FALSE;
            }
            header.pulsezip->items[j].type = (PULSEitem::Type)type;
            header.pulsezip->items[j].size = size;
            header.pulsezip->items[j].version = version;
          }
*/
