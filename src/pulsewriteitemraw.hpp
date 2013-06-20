/*
===============================================================================

  FILE:  pulsewriteitemraw.hpp
  
  CONTENTS:
  
    Implementation of PULSEwriteItemRaw for *all* items that compose a pulse.

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
#ifndef PULSE_WRITE_ITEM_RAW_HPP
#define PULSE_WRITE_ITEM_RAW_HPP

#include "pulsewriteitem.hpp"

#include <assert.h>

class PULSEwriteItemRaw_PULSE0_LE : public PULSEwriteItemRaw
{
public:
  PULSEwriteItemRaw_PULSE0_LE(){};
  inline void write(const U8* item)
  {
    outstream->putBytes(item, 48);
  };
};

class PULSEwriteItemRaw_PULSESOURCEID16_LE : public PULSEwriteItemRaw
{
public:
  PULSEwriteItemRaw_PULSESOURCEID16_LE(){};
  inline void write(const U8* item)
  {
    outstream->putBytes(item, 2);
  };
};

class PULSEwriteItemRaw_PULSESOURCEID32_LE : public PULSEwriteItemRaw
{
public:
  PULSEwriteItemRaw_PULSESOURCEID32_LE(){};
  inline void write(const U8* item)
  {
    outstream->putBytes(item, 4);
  };
};

class PULSEwriteItemRaw_PULSE0_BE : public PULSEwriteItemRaw
{
public:
  PULSEwriteItemRaw_PULSE0_BE(){};
  inline void write(const U8* item)
  {
    ENDIAN_SWAP_64(&item[ 0], &swapped[ 0]);    // gps_time
    ENDIAN_SWAP_64(&item[ 8], &swapped[ 8]);    // offset
    ENDIAN_SWAP_32(&item[16], &swapped[16]);    // X
    ENDIAN_SWAP_32(&item[20], &swapped[20]);    // Y
    ENDIAN_SWAP_32(&item[24], &swapped[24]);    // Z
    ENDIAN_SWAP_32(&item[28], &swapped[28]);    // dx
    ENDIAN_SWAP_32(&item[32], &swapped[32]);    // dy
    ENDIAN_SWAP_32(&item[36], &swapped[36]);    // dz
    ENDIAN_SWAP_16(&item[40], &swapped[40]);    // first_returning_sample
    ENDIAN_SWAP_16(&item[42], &swapped[42]);    // last_returning_sample
    ENDIAN_SWAP_16(&item[44], &swapped[44]);    // descriptor_index, scan_direction_flag, edge_of_flight_line
    *((U16*)&swapped[46]) = *((U16*)&item[46]); // intensity, classification
    outstream->putBytes(swapped, 48);
  };
private:
  U8 swapped[48];
};

class PULSEwriteItemRaw_PULSESOURCEID16_BE : public PULSEwriteItemRaw
{
public:
  PULSEwriteItemRaw_PULSESOURCEID16_BE(){};
  inline void write(const U8* item)
  {
    ENDIAN_SWAP_16(&item[0], &swapped[0]);      // pulse_source_ID
    outstream->putBytes(swapped, 2);
  };
private:
  U8 swapped[2];
};

class PULSEwriteItemRaw_PULSESOURCEID32_BE : public PULSEwriteItemRaw
{
public:
  PULSEwriteItemRaw_PULSESOURCEID32_BE(){};
  inline void write(const U8* item)
  {
    ENDIAN_SWAP_32(&item[0], &swapped[0]);      // pulse_source_ID
    outstream->putBytes(swapped, 4);
  };
private:
  U8 swapped[4];
};

class PULSEwriteItemRaw_EXTRABYTES : public PULSEwriteItemRaw
{
public:
  PULSEwriteItemRaw_EXTRABYTES(U32 number)
  {
    this->number = number;
  }
  inline void write(const U8* item)
  {
    outstream->putBytes(item, number);
  };
private:
  U32 number;
};

#endif
