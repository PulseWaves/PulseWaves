/*
===============================================================================

  FILE:  pulsedescriptor.cpp
  
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

#include "pulsedescriptor.hpp"

#include "bytestreamin.hpp"
#include "bytestreamout.hpp"
#include "pulsewavesdefinitions.hpp"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

U32 PULSEsampling::size_of_attributes() const
{
  U32 size = (2*4) + (4*1) + (2*4) + (2*1) + (1*2) + (1*4) + (2*2) + (2*4) + (1*PULSEWAVES_DESCRIPTION_SIZE);
  return size;
}

BOOL PULSEsampling::load(ByteStreamIn* stream)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }
  
  I64 start = stream->tell();

  try { stream->get32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: reading size\n");
    return FALSE;
  }
  
  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSEsampling should be %u but is %u\n", size, this->size);
  }

  try { stream->get32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: reading reserved\n");
    return FALSE;
  }

  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSEsampling is %u instead of %u\n", this->reserved, reserved);
  }

  try { stream->getBytes((U8*)&type, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading type\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)&channel, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading channel\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)&unused, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading unused\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)&bits_for_duration_from_anchor, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading bits_for_duration_from_anchor\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&scale_for_duration_from_anchor); } catch(...)
  {
    fprintf(stderr,"ERROR: reading scale_for_duration_from_anchor\n");
    return FALSE;
  }

  if (scale_for_duration_from_anchor <= 0.0f)
  {
    fprintf(stderr,"WARNING: scale_for_duration_from_anchor of PULSEsampling is %g\n", scale_for_duration_from_anchor);
  }

  try { stream->get32bitsLE((U8*)&offset_for_duration_from_anchor); } catch(...)
  {
    fprintf(stderr,"ERROR: reading offset_for_duration_from_anchor\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)&bits_for_number_of_segments, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading bits_for_number_of_segments\n");
    return FALSE;
  }

  try { stream->getBytes((U8*)&bits_for_number_of_samples, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading bits_for_number_of_samples\n");
    return FALSE;
  }

  try { stream->get16bitsLE((U8*)&number_of_segments); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_segments\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&number_of_samples); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_samples\n");
    return FALSE;
  }

  try { stream->get16bitsLE((U8*)&bits_per_sample); } catch(...)
  {
    fprintf(stderr,"ERROR: reading bits_per_sample\n");
    return FALSE;
  }

  try { stream->get16bitsLE((U8*)&lookup_table_index); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lookup_table_index\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&sample_units); } catch(...)
  {
    fprintf(stderr,"ERROR: reading sample_units\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&compression); } catch(...)
  {
    fprintf(stderr,"ERROR: reading compression\n");
    return FALSE;
  }

  if ( (stream->tell() - start + PULSEWAVES_DESCRIPTION_SIZE) != size )
  {
    if ( !stream->seek(start + size - PULSEWAVES_DESCRIPTION_SIZE) )
    {
      fprintf(stderr,"ERROR: cannot seek to description\n");
      return FALSE;
    }
    this->size = size;
  }

  try { stream->getBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading description\n");
    return FALSE;
  }

  return TRUE;
};

BOOL PULSEsampling::save(ByteStreamOut* stream) const
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSEsampling should be %u but is %u. ignoring %u ...\n", size, this->size, this->size);
  }
  
  try { stream->put32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: writing size\n");
    return FALSE;
  }
  
  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSEsampling should be %u but is %u. ignoring %u ...\n", reserved, this->reserved, this->reserved);
  }

  try { stream->put32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: writing reserved\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)&type, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: writing type\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)&channel, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: writing channel\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)&unused, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: writing unused\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)&bits_for_duration_from_anchor, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: writing bits_for_duration_from_anchor\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&scale_for_duration_from_anchor); } catch(...)
  {
    fprintf(stderr,"ERROR: writing scale_for_duration_from_anchor\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&offset_for_duration_from_anchor); } catch(...)
  {
    fprintf(stderr,"ERROR: writing offset_for_duration_from_anchor\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)&bits_for_number_of_segments, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: writing bits_for_number_of_segments\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)&bits_for_number_of_samples, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: writing bits_for_number_of_samples\n");
    return FALSE;
  }

  try { stream->put16bitsLE((U8*)&number_of_segments); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_of_segments\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&number_of_samples); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_of_samples\n");
    return FALSE;
  }

  try { stream->put16bitsLE((U8*)&bits_per_sample); } catch(...)
  {
    fprintf(stderr,"ERROR: writing bits_per_sample\n");
    return FALSE;
  }

  try { stream->put16bitsLE((U8*)&lookup_table_index); } catch(...)
  {
    fprintf(stderr,"ERROR: writing lookup_table_index\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&sample_units); } catch(...)
  {
    fprintf(stderr,"ERROR: writing sample_units\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&compression); } catch(...)
  {
    fprintf(stderr,"ERROR: writing compression\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing description\n");
    return FALSE;
  }

  return TRUE;
};

BOOL PULSEsampling::is_equal(const PULSEsampling* sampling) const
{
  return (memcmp(this, sampling, size - PULSEWAVES_DESCRIPTION_SIZE) == 0); // disregard description[PULSEWAVES_DESCRIPTION_SIZE] char array
};

PULSEsampling::PULSEsampling()
{
  memset(this, 0, sizeof(PULSEsampling)); 
  size = size_of_attributes();
  scale_for_duration_from_anchor = 1.0f;
};

U32 PULSEcomposition::size_of_attributes() const
{
  U32 size = (3*4) + (2*2) + (3*4) + (1*PULSEWAVES_DESCRIPTION_SIZE);
  return size;
}

BOOL PULSEcomposition::load(ByteStreamIn* stream)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }
  
  I64 start = stream->tell();

  try { stream->get32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: reading size\n");
    return FALSE;
  }
  
  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSEcomposition should be %u but is %u\n", size, this->size);
  }

  try { stream->get32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: reading reserved\n");
    return FALSE;
  }

  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSEcomposition is %u instead of %u\n", this->reserved, reserved);
  }

  try { stream->get32bitsLE((U8*)&optical_center_to_anchor_point); } catch(...)
  {
    fprintf(stderr,"ERROR: reading optical_center_to_anchor_point\n");
    return FALSE;
  }

  try { stream->get16bitsLE((U8*)&number_of_extra_waves_bytes); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_extra_waves_bytes\n");
    return FALSE;
  }

  try { stream->get16bitsLE((U8*)&number_of_samplings); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_samplings\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&sample_units); } catch(...)
  {
    fprintf(stderr,"ERROR: reading sample_units\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&compression); } catch(...)
  {
    fprintf(stderr,"ERROR: reading compression\n");
    return FALSE;
  }

  try { stream->get32bitsLE((U8*)&scanner_index); } catch(...)
  {
    fprintf(stderr,"ERROR: reading scanner_index\n");
    return FALSE;
  }

  if ( (stream->tell() - start + PULSEWAVES_DESCRIPTION_SIZE) != PULSEWAVES_DESCRIPTION_SIZE )
  {
    if ( !stream->seek(start + size - PULSEWAVES_DESCRIPTION_SIZE) )
    {
      fprintf(stderr,"ERROR: cannot seek to description\n");
      return FALSE;
    }
    size = sizeof(PULSEcomposition);
  }

  try { stream->getBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading description\n");
    return FALSE;
  }

  return TRUE;
}

BOOL PULSEcomposition::save(ByteStreamOut* stream) const
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  U32 size = size_of_attributes(); 

  if (size != this->size)
  {
    fprintf(stderr,"WARNING: size of PULSEcomposition should be %u but is %u. ignoring %u ...\n", size, this->size, this->size);
  }

  try { stream->put32bitsLE((U8*)&size); } catch(...)
  {
    fprintf(stderr,"ERROR: writing size\n");
    return FALSE;
  }
  
  U32 reserved = 0;

  if (reserved != this->reserved)
  {
    fprintf(stderr,"WARNING: reserved of PULSEcomposition should be %u but is %u. ignoring %u ...\n", reserved, this->reserved, this->reserved);
  }

  try { stream->put32bitsLE((U8*)&reserved); } catch(...)
  {
    fprintf(stderr,"ERROR: writing reserved\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&optical_center_to_anchor_point); } catch(...)
  {
    fprintf(stderr,"ERROR: writing optical_center_to_anchor_point\n");
    return FALSE;
  }

  try { stream->put16bitsLE((U8*)&number_of_extra_waves_bytes); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_of_extra_waves_bytes\n");
    return FALSE;
  }

  try { stream->put16bitsLE((U8*)&number_of_samplings); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_of_samplings\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&sample_units); } catch(...)
  {
    fprintf(stderr,"ERROR: writing sample_units\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&compression); } catch(...)
  {
    fprintf(stderr,"ERROR: writing compression\n");
    return FALSE;
  }

  try { stream->put32bitsLE((U8*)&scanner_index); } catch(...)
  {
    fprintf(stderr,"ERROR: writing scanner_index\n");
    return FALSE;
  }

  try { stream->putBytes((U8*)description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing description\n");
    return FALSE;
  }

  return TRUE;
}

BOOL PULSEcomposition::is_equal(const PULSEcomposition* composition) const
{
  return (memcmp(this, composition,  size - PULSEWAVES_DESCRIPTION_SIZE) == 0); // disregard description[PULSEWAVES_DESCRIPTION_SIZE] char array
};

PULSEcomposition::PULSEcomposition()
{
  memset(this, 0, sizeof(PULSEcomposition));
  size = size_of_attributes();
};

BOOL PULSEdescriptor::load(ByteStreamIn* stream)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  if (composition == 0)
  {
    composition = new PULSEcomposition;
  }
  if (!composition->load(stream))
  {
    fprintf(stderr,"ERROR: loading composition\n");
    return FALSE;
  }
  if (samplings != 0)
  {
    delete [] samplings;
  }
  if (composition->number_of_samplings)
  {
    samplings = new PULSEsampling[composition->number_of_samplings];
    if (samplings == 0)
    {
      fprintf(stderr,"ERROR: allocating %u samplings\n", composition->number_of_samplings);
      return FALSE;
    }
    U32 i;
    for (i = 0; i < composition->number_of_samplings; i++)
    {
      if (!samplings[i].load(stream))
      {
        fprintf(stderr,"ERROR: loading sampling %u of %u\n", i, composition->number_of_samplings);
        return FALSE;
      }
    }
  }
  else
  {
    samplings = 0;
  }
  return TRUE;
}

BOOL PULSEdescriptor::save(ByteStreamOut* stream) const
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  if (composition == 0)
  {
    fprintf(stderr,"ERROR: no composition\n");
    return FALSE;
  }

  if (!composition->save(stream))
  {
    fprintf(stderr,"ERROR: saving composition\n");
    return FALSE;
  }

  if (composition->number_of_samplings)
  {
    if (samplings == 0)
    {
      fprintf(stderr,"ERROR: no samplings[%u]\n", composition->number_of_samplings);
      return FALSE;
    }
    U32 i;
    for (i = 0; i < composition->number_of_samplings; i++)
    {
      if (!samplings[i].save(stream))
      {
        fprintf(stderr,"ERROR: saving sampling %u of %u\n", i, composition->number_of_samplings);
        return FALSE;
      }
    }
  }
  return TRUE;
}

BOOL PULSEdescriptor::is_equal(const PULSEcomposition* composition, const PULSEsampling* samplings) const
{
  if (!this->composition->is_equal(composition))
  {
    return FALSE;
  }
  U32 i;
  for (i = 0; i < composition->number_of_samplings; i++)
  {
    if (!this->samplings[i].is_equal(&samplings[i]))
    {
      return FALSE;
    }
  }
  return TRUE;
};

BOOL PULSEdescriptor::is_equal(const PULSEdescriptor* descriptor) const
{
  if (descriptor)
  {
    return is_equal(descriptor->composition, descriptor->samplings);
  }
  return FALSE;
};

PULSEdescriptor::PULSEdescriptor()
{
  memset(this, 0, sizeof(PULSEdescriptor));
};
