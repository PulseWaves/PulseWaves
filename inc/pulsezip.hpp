/*
===============================================================================

  FILE:  pulsezip.hpp
  
  CONTENTS:
  
    Contains the definitions of all the currently supported coding schemes.

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
  
    23 February 2012 -- created after arriving at 13 Calle de las Mercedes
  
===============================================================================
*/
#ifndef PULSEZIP_HPP
#define PULSEZIP_HPP

#include <stdio.h>
#include "pulseitem.hpp"

class ByteStreamIn;
class ByteStreamOut;

#define PULSEZIP_VERSION_MAJOR                  0
#define PULSEZIP_VERSION_MINOR                  0
#define PULSEZIP_VERSION_REVISION               0

#define PULSEZIP_COMPRESSOR_NONE                0
#define PULSEZIP_COMPRESSOR_DEFAULT             1
#define PULSEZIP_COMPRESSOR_TOTAL               2

#define PULSEZIP_CODER_ARITHMETIC               0
#define PULSEZIP_CODER_TOTAL                    1

#define PULSEZIP_CHUNK_SIZE_DEFAULT         50000

#define PULSEZIP_DESCRIPTOR_CONTEXT_BITS        7
#define PULSEZIP_BIT_BYTE_CONTEXT_BITS          7
#define PULSEZIP_CLASSIFICATION_CONTEXT_BITS    7

#define PULSEZIP_LAST_BIT_BYTE_CHANGE_BIT      (PULSEZIP_DESCRIPTOR_CONTEXT_BITS)
#define PULSEZIP_LAST_BIT_BYTE_CHANGE_MASK     (1u << PULSEZIP_LAST_BIT_BYTE_CHANGE_BIT)
#define PULSEZIP_BIT_BYTE_CHANGE_BIT           (PULSEZIP_LAST_BIT_BYTE_CHANGE_BIT + 1)
#define PULSEZIP_BIT_BYTE_CHANGE_MASK          (1u << PULSEZIP_BIT_BYTE_CHANGE_BIT)

#define PULSEZIP_CHANGED_VALUES_CONTEXT_NUMBER (1u << (PULSEZIP_DESCRIPTOR_CONTEXT_BITS+1))
#define PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER     (1u << (PULSEZIP_DESCRIPTOR_CONTEXT_BITS+1))
#define PULSEZIP_BIT_BYTE_CONTEXT_NUMBER       (1u << (PULSEZIP_BIT_BYTE_CONTEXT_BITS))
#define PULSEZIP_CLASSIFICATION_CONTEXT_NUMBER (1u << (PULSEZIP_CLASSIFICATION_CONTEXT_BITS))

#define PULSEZIP_TWO_PULSE_CONTEXT_NUMBER      (1u << (PULSEZIP_DESCRIPTOR_CONTEXT_BITS+2))

#define PULSEZIP_LAST_OFFSET_DIFF_ARRAY_SIZE   (1u << PULSEZIP_DESCRIPTOR_CONTEXT_BITS)

class PULSEzip
{
public:

  // supported version control
  BOOL check_compressor(const U16 compressor);
  BOOL check_coder(const U16 coder);
  BOOL check_item(const PULSEitem* item);
  BOOL check_items(const U16 num_items, const PULSEitem* items);
  BOOL check();

  // go back and forth between item array and pulse type & size
  BOOL setup(U16* num_items, PULSEitem** items, const U32 format, const U32 attributes, const U32 size, const U32 compression=PULSEZIP_COMPRESSOR_DEFAULT);
  BOOL is_standard(const U16 num_items, const PULSEitem* items, U32* format=0, U32* attributes=0, U32* size=0);
  BOOL is_standard(U32* format=0, U32* attributes=0, U32* size=0);

  // setup
  BOOL setup(const U32 format, const U32 attributes, const U32 size, const U32 compression=PULSEZIP_COMPRESSOR_DEFAULT);
  BOOL setup(const U16 num_items, const PULSEitem* items, const U32 compression);
  BOOL set_chunk_size(const U32 chunk_size);           /* for compressor only */
  BOOL request_version(const U16 requested_version);   /* for compressor only */

  // in case a function returns false this string describes the problem
  const CHAR* get_error() const;

  // stored in PULSEzip VLR data section
  U16 compressor;
  U16 coder;
  U8 version_major;
  U8 version_minor;
  U16 version_revision;
  U32 options;
  U32 chunk_size; 
  U16 num_items;
  PULSEitem* items;

  PULSEzip();
  ~PULSEzip();

  // for serializing
  I64 get_payload() const;
  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;
  BOOL save_to_txt(FILE* file) const;

private:
  BOOL return_error(const CHAR* err);
  CHAR* error_string;
};

#endif
