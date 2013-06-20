/*
===============================================================================

  FILE:  pulseitemreadraw.hpp
  
  CONTENTS:
  
    Implementation of PULSEitemReadRaw for *all* items that compose a pulse.

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
#ifndef PULSE_READ_ITEM_RAW_HPP
#define PULSE_READ_ITEM_RAW_HPP

#include "pulsereaditem.hpp"

#include <assert.h>

class PULSEreadItemRaw_PULSE0_LE : public PULSEreadItemRaw
{
public:
  PULSEreadItemRaw_PULSE0_LE(){};
  inline void read(U8* item)
  {
    instream->getBytes(item, 48);
  }
};

class PULSEreadItemRaw_PULSESOURCEID16_LE : public PULSEreadItemRaw
{
public:
  PULSEreadItemRaw_PULSESOURCEID16_LE(){};
  inline void read(U8* item)
  {
    instream->getBytes(item, 2);
  }
};

class PULSEreadItemRaw_PULSESOURCEID32_LE : public PULSEreadItemRaw
{
public:
  PULSEreadItemRaw_PULSESOURCEID32_LE(){};
  inline void read(U8* item)
  {
    instream->getBytes(item, 4);
  }
};

class PULSEreadItemRaw_PULSE0_BE : public PULSEreadItemRaw
{
public:
  PULSEreadItemRaw_PULSE0_BE(){};
  inline void read(U8* item)
  {
    instream->getBytes(swapped, 48);
    ENDIAN_SWAP_64(&swapped[ 0], &item[ 0]);    // gps_time
    ENDIAN_SWAP_64(&swapped[ 8], &item[ 8]);    // offset
    ENDIAN_SWAP_32(&swapped[16], &item[16]);    // X
    ENDIAN_SWAP_32(&swapped[20], &item[20]);    // Y
    ENDIAN_SWAP_32(&swapped[24], &item[24]);    // Z
    ENDIAN_SWAP_32(&swapped[28], &item[28]);    // dx
    ENDIAN_SWAP_32(&swapped[32], &item[32]);    // dy
    ENDIAN_SWAP_32(&swapped[36], &item[36]);    // dz
    ENDIAN_SWAP_16(&swapped[40], &item[40]);    // first_returning_sample
    ENDIAN_SWAP_16(&swapped[42], &item[42]);    // last_returning_sample
    ENDIAN_SWAP_16(&swapped[44], &item[44]);    // descriptor_index, scan_direction_flag, edge_of_flight_line, mirror facet
    *((U16*)&item[46]) = *((U16*)&item[46]);    // intensity, classification
  };
private:
  U8 swapped[48];
};

class PULSEreadItemRaw_PULSESOURCEID16_BE : public PULSEreadItemRaw
{
public:
  PULSEreadItemRaw_PULSESOURCEID16_BE(){};
  inline void read(U8* item)
  {
    instream->getBytes(swapped, 2);
    ENDIAN_SWAP_16(&swapped[0], &item[0]);      // pulse_source_ID
  }
private:
  U8 swapped[2];
};

class PULSEreadItemRaw_PULSESOURCEID32_BE : public PULSEreadItemRaw
{
public:
  PULSEreadItemRaw_PULSESOURCEID32_BE(){};
  inline void read(U8* item)
  {
    instream->getBytes(swapped, 4);
    ENDIAN_SWAP_32(&swapped[0], &item[0]);      // pulse_source_ID
  }
private:
  U8 swapped[4];
};

class PULSEreadItemRaw_EXTRABYTES : public PULSEreadItemRaw
{
public:
  PULSEreadItemRaw_EXTRABYTES(U32 number)
  {
    this->number = number;
  }
  inline void read(U8* item)
  {
    instream->getBytes(item, number);
  };
private:
  U32 number;
};

#endif
