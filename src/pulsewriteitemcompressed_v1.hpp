/*
===============================================================================

  FILE:  pulsewriteitemcompressed_v1.hpp
  
  CONTENTS:
  
    Implementation of PULSEitemReadCompressed for *all* items (version 1).

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
  
    17 February 2013 -- created at Seattle's Best coffee in Manila, Philippines

===============================================================================
*/
#ifndef PULSE_WRITE_ITEM_COMPRESSED_V1_HPP
#define PULSE_WRITE_ITEM_COMPRESSED_V1_HPP

#include "pulsewriteitem.hpp"
#include "pulseziphelper.hpp"
#include "entropyencoder.hpp"
#include "integercompressor.hpp"

class PULSEwriteItemCompressed_PULSE0_v1 : public PULSEwriteItemCompressed
{
public:

  PULSEwriteItemCompressed_PULSE0_v1(EntropyEncoder* enc);

  BOOL init(const U8* item);
  void write(const U8* item);

  ~PULSEwriteItemCompressed_PULSE0_v1();

private:
  EntropyEncoder* enc;
  U8 last_item[PULSEWAVES_PULSE0_SIZE];
  I32 last_T_diff;
  I64 last_T;
  I32 multi_extreme_counter;
  I32 last_offset_diff;
  I32 last_offset_diff_array[PULSEZIP_LAST_OFFSET_DIFF_ARRAY_SIZE];
  I32 last_anchor_x_diff_array[4];
  I32 last_anchor_y_diff_array[4];
  I32 last_anchor_z_diff_array[4];
  I32 last_anchor_x_diff;
  I32 last_anchor_y_diff;
  I32 last_anchor_z_diff;

  U32 last_descriptor_context;
  U32 last_bit_byte_change_mask;
  PULSEzipContextMapper8to7orLess cm_descriptor_index;
  PULSEzipContextMapper16to7orLess cm_bit_byte;
  PULSEzipContextMapper16to7orLess cm_classification;

  EntropyModel* m_T_diff_multi;
  EntropyModel* m_T_last_diff_zero;
  EntropyModel* m_changed_values[PULSEZIP_CHANGED_VALUES_CONTEXT_NUMBER];
  EntropyModel* m_descriptor_index[PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER];
  EntropyModel* m_bit_byte[PULSEZIP_BIT_BYTE_CONTEXT_NUMBER];
  EntropyModel* m_classification[PULSEZIP_CLASSIFICATION_CONTEXT_NUMBER];
  IntegerCompressor* ic_T;
  IntegerCompressor* ic_offset_diff;
  IntegerCompressor* ic_first_returning_sample;
  IntegerCompressor* ic_last_returning_sample;
  IntegerCompressor* ic_intensity;
  IntegerCompressor* ic_anchor_dx;
  IntegerCompressor* ic_anchor_dy;
  IntegerCompressor* ic_anchor_z;
  IntegerCompressor* ic_target_dx;
  IntegerCompressor* ic_target_dy;
  IntegerCompressor* ic_target_z;
};

#define PULSEZIP_PULSESOURCE16_ID_TABLE 16

class PULSEwriteItemCompressed_PULSESOURCEID16_v1 : public PULSEwriteItemCompressed
{
public:

  PULSEwriteItemCompressed_PULSESOURCEID16_v1(EntropyEncoder* enc);

  BOOL init(const U8* item);
  void write(const U8* item);

  ~PULSEwriteItemCompressed_PULSESOURCEID16_v1();

private:
  EntropyEncoder* enc;
  U16 last_pulse_source_ID;
  U16 oldest_pulse_source_IDs;
  U16 previous_pulse_source_IDs[PULSEZIP_PULSESOURCE16_ID_TABLE];
  EntropyModel* m_sameID;
  IntegerCompressor* ic_newID;
};

#define PULSEZIP_PULSESOURCE32_ID_TABLE 32

class PULSEwriteItemCompressed_PULSESOURCEID32_v1 : public PULSEwriteItemCompressed
{
public:

  PULSEwriteItemCompressed_PULSESOURCEID32_v1(EntropyEncoder* enc);

  BOOL init(const U8* item);
  void write(const U8* item);

  ~PULSEwriteItemCompressed_PULSESOURCEID32_v1();

private:
  EntropyEncoder* enc;
  U32 last_pulse_source_ID;
  U32 oldest_pulse_source_IDs;
  U32 previous_pulse_source_IDs[PULSEZIP_PULSESOURCE32_ID_TABLE];
  EntropyModel* m_sameID;
  IntegerCompressor* ic_newID;
};

class PULSEwriteItemCompressed_EXTRABYTES_v1 : public PULSEwriteItemCompressed
{
public:

  PULSEwriteItemCompressed_EXTRABYTES_v1(EntropyEncoder* enc, U32 number);

  BOOL init(const U8* item);
  void write(const U8* item);

  ~PULSEwriteItemCompressed_EXTRABYTES_v1();

private:
  EntropyEncoder* enc;
  U32 number;
  U8* last_item;

  EntropyModel** m_byte;
};

#endif
