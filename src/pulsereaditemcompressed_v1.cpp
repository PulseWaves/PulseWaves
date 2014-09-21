/*
===============================================================================

  FILE:  pulsereaditemcompressed_v1.cpp
  
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

#include "pulsereaditemcompressed_v1.hpp"

#include <assert.h>
#include <string.h>

struct PULSEpulse0
{
  I64 T;                        // start byte:  0
  I64 offset;                   //              8
  I32 anchor_X;                 //             16
  I32 anchor_Y;                 //             20
  I32 anchor_Z;                 //             24
  I32 target_X;                 //             28
  I32 target_Y;                 //             32
  I32 target_Z;                 //             36
  I16 first_returning_sample;   //             40
  I16 last_returning_sample;    //             42
  U16 descriptor_index : 8;     //             44
  U16 reserved : 4;             //             45
  U16 edge_of_scan_line : 1;
  U16 scan_direction : 1;
  U16 mirror_facet : 2;
  U8 intensity;                 //             46
  U8 classification;            //             47
};

#define PULSEZIP_T_MULTI 243
#define PULSEZIP_T_MULTI_MINUS -10
#define PULSEZIP_T_MULTI_UNCHANGED (PULSEZIP_T_MULTI - PULSEZIP_T_MULTI_MINUS + 1)
#define PULSEZIP_T_MULTI_CODE_FULL (PULSEZIP_T_MULTI_UNCHANGED + 1)
#define PULSEZIP_T_MULTI_TOTAL     (PULSEZIP_T_MULTI_CODE_FULL + 1) 

PULSEreadItemCompressed_PULSE0_v1::PULSEreadItemCompressed_PULSE0_v1(ArithmeticDecoder* dec)
{
  /* set decoder */
  assert(dec);
  this->dec = dec;

  /* setup context mapper */
  cm_descriptor_index.setup(PULSEZIP_DESCRIPTOR_CONTEXT_BITS);
  cm_bit_byte.setup(PULSEZIP_BIT_BYTE_CONTEXT_BITS);
  cm_classification.setup(PULSEZIP_CLASSIFICATION_CONTEXT_BITS);

  /* create models and integer compressors */
  m_T_diff_multi = dec->createSymbolModel(PULSEZIP_T_MULTI_TOTAL);
  m_T_last_diff_zero = dec->createSymbolModel(3);
  memset(m_changed_values, 0, PULSEZIP_CHANGED_VALUES_CONTEXT_NUMBER*sizeof(ArithmeticModel*));
  memset(m_descriptor_index, 0, PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER*sizeof(ArithmeticModel*));
  memset(m_bit_byte, 0, PULSEZIP_BIT_BYTE_CONTEXT_NUMBER*sizeof(ArithmeticModel*));
  memset(m_classification, 0, PULSEZIP_CLASSIFICATION_CONTEXT_NUMBER*sizeof(ArithmeticModel*));
  ic_T = new IntegerCompressor(dec, 32, 9); // 32 bits, 9 contexts
  ic_offset_diff = new IntegerCompressor(dec, 32, PULSEZIP_LAST_OFFSET_DIFF_ARRAY_SIZE);            // 32 bits, PULSEZIP_LAST_OFFSET_DIFF_ARRAY_SIZE context
  ic_anchor_dx = new IntegerCompressor(dec, 32, 4);                // 32 bits, 4 context
  ic_anchor_dy = new IntegerCompressor(dec, 32, 4);               // 32 bits, 4 contexts
  ic_anchor_z = new IntegerCompressor(dec, 32, 4);                // 32 bits, 4 contexts
  ic_target_dx = new IntegerCompressor(dec, 32, 4);                // 32 bits, 4 context
  ic_target_dy = new IntegerCompressor(dec, 32, 4);               // 32 bits, 4 contexts
  ic_target_z = new IntegerCompressor(dec, 32, 4);                // 32 bits, 4 contexts
  ic_first_returning_sample = new IntegerCompressor(dec, 16, PULSEZIP_TWO_PULSE_CONTEXT_NUMBER); // 16 bits, PULSEZIP_TWO_PULSE_CONTEXT_NUMBER context
  ic_last_returning_sample = new IntegerCompressor(dec, 16, PULSEZIP_TWO_PULSE_CONTEXT_NUMBER);  // 16 bits, PULSEZIP_TWO_PULSE_CONTEXT_NUMBER context
  ic_intensity = new IntegerCompressor(dec, 8, PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER);
}

PULSEreadItemCompressed_PULSE0_v1::~PULSEreadItemCompressed_PULSE0_v1()
{
  U32 i;
 
  dec->destroySymbolModel(m_T_diff_multi);
  dec->destroySymbolModel(m_T_last_diff_zero);

  for (i = 0; i < PULSEZIP_CHANGED_VALUES_CONTEXT_NUMBER; i++)
  {
    if (m_changed_values[i]) dec->destroySymbolModel(m_changed_values[i]);
  }
  for (i = 0; i < PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER; i++)
  {
    if (m_descriptor_index[i]) dec->destroySymbolModel(m_descriptor_index[i]);
  }
  for (i = 0; i < PULSEZIP_BIT_BYTE_CONTEXT_NUMBER; i++)
  {
    if (m_bit_byte[i]) dec->destroySymbolModel(m_bit_byte[i]);
  }
  for (i = 0; i < PULSEZIP_CLASSIFICATION_CONTEXT_NUMBER; i++)
  {
    if (m_classification[i]) dec->destroySymbolModel(m_classification[i]);
  }
  delete ic_T;
  delete ic_offset_diff;
  delete ic_anchor_dx;
  delete ic_anchor_dy;
  delete ic_anchor_z;
  delete ic_target_dx;
  delete ic_target_dy;
  delete ic_target_z;
  delete ic_first_returning_sample;
  delete ic_last_returning_sample;
  delete ic_intensity;
}

BOOL PULSEreadItemCompressed_PULSE0_v1::init(const U8* item)
{
  U32 i;

  /* init state */
  last_T = *((I64*)item);
  last_T_diff = 0;
  multi_extreme_counter = 0;
  for (i = 0; i < PULSEZIP_LAST_OFFSET_DIFF_ARRAY_SIZE; i++)
  {
    last_offset_diff_array[i] = I32_MAX;
  }
  last_offset_diff = 0;
  for (i = 0; i < 4; i++)
  {
    last_anchor_x_diff_array[i] = I32_MAX;
    last_anchor_y_diff_array[i] = I32_MAX;
    last_anchor_z_diff_array[i] = I32_MAX;
  }
  last_anchor_x_diff = 0;
  last_anchor_y_diff = 0;
  last_anchor_z_diff = 0;
  last_descriptor_context = 0;
  last_bit_byte_change_mask = 0;
  cm_descriptor_index.init();
  cm_bit_byte.init();
  cm_classification.init();

  /* init models and integer compressors */
  dec->initSymbolModel(m_T_diff_multi);
  dec->initSymbolModel(m_T_last_diff_zero);
  for (i = 0; i < PULSEZIP_CHANGED_VALUES_CONTEXT_NUMBER; i++)
  {
    if (m_changed_values[i]) dec->initSymbolModel(m_changed_values[i]);
  }
  for (i = 0; i < PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER; i++)
  {
    if (m_descriptor_index[i]) dec->initSymbolModel(m_descriptor_index[i]);
  }
  for (i = 0; i < PULSEZIP_BIT_BYTE_CONTEXT_NUMBER; i++)
  {
    if (m_bit_byte[i]) dec->initSymbolModel(m_bit_byte[i]);
  }
  for (i = 0; i < PULSEZIP_CLASSIFICATION_CONTEXT_NUMBER; i++)
  {
    if (m_classification[i]) dec->initSymbolModel(m_classification[i]);
  }
  ic_T->initDecompressor();
  ic_offset_diff->initDecompressor();
  ic_anchor_dx->initDecompressor();
  ic_anchor_dy->initDecompressor();
  ic_anchor_z->initDecompressor();
  ic_target_dx->initDecompressor();
  ic_target_dy->initDecompressor();
  ic_target_z->initDecompressor();
  ic_first_returning_sample->initDecompressor();
  ic_last_returning_sample->initDecompressor();
  ic_intensity->initDecompressor();

  /* init last item */
  memcpy(last_item, item, PULSEWAVES_PULSE0_SIZE);

  return TRUE;
}

inline void PULSEreadItemCompressed_PULSE0_v1::read(U8* item)
{
  I32 last_first_returning_sample = *((U16*)(last_item + 40));
  I32 last_last_returning_sample = *((U16*)(last_item + 42));
  I32 last_returning_sample_difference = last_last_returning_sample - last_first_returning_sample;

  // this context combines a change in the last bit byte with the last descriptor context 

  U32 remaining_context = last_bit_byte_change_mask | last_descriptor_context;

  // this context expresses a change in the last two bit bytes

  U32 last_two_bit_byte_changes = (U32)(last_bit_byte_change_mask != 0);

  // decompress which values have changed

  if (m_changed_values[remaining_context] == 0)
  {
    m_changed_values[remaining_context] = dec->createSymbolModel(64);
    dec->initSymbolModel(m_changed_values[remaining_context]);
  }
  // switch context based on a change in the last bit byte and the last descriptor context
  I32 changed_values = dec->decodeSymbol(m_changed_values[remaining_context]);

  if (changed_values)
  {
    // decompress the descriptor_index ... if it has changed
    if (changed_values & 8)
    {
      if (m_descriptor_index[remaining_context] == 0)
      {
        m_descriptor_index[remaining_context] = dec->createSymbolModel(256);
        dec->initSymbolModel(m_descriptor_index[remaining_context]);
      }
      last_item[44] = (U8)dec->decodeSymbol(m_descriptor_index[remaining_context]);
      // get a new descriptor context
      last_descriptor_context = cm_descriptor_index.getContext(last_item[44]);
      // update the remaining context with the new descriptor context
      remaining_context = last_bit_byte_change_mask | last_descriptor_context;
    }

    // this context adds a change in the current bit byte to the remaining context

    U32 two_pulse_context = remaining_context;

    // decompress the bit_byte (reserved, edge_of_flight_line, scan_direction_flag, mirror facet) ... if it has changed
    if (changed_values & 4)
    {
      // compute a local context
      U32 context_index = cm_bit_byte.getContext((remaining_context << 8) | last_item[45]); // last_bit_byte
      if (m_bit_byte[context_index] == 0)
      {
        m_bit_byte[context_index] = dec->createSymbolModel(256);
        dec->initSymbolModel(m_bit_byte[context_index]);
      }
      // switch context based on the locally computed context
      last_item[45] = (U8)dec->decodeSymbol(m_bit_byte[context_index]);
      // marks that there was a change in the bit byte for the next time around
      last_bit_byte_change_mask = PULSEZIP_LAST_BIT_BYTE_CHANGE_MASK;
      // marks that there was a change in the current bit byte
      two_pulse_context |= PULSEZIP_BIT_BYTE_CHANGE_MASK;
      last_two_bit_byte_changes |= 2;
    }

    // decompress the intensity ... if it has changed
    if (changed_values & 2)
    {
      last_item[46] = ic_intensity->decompress(last_item[46], last_descriptor_context);
    }

    // decompress the classification ... if it has changed
    if (changed_values & 1)
    {
      // compute a local context
      U32 context_index = cm_classification.getContext((remaining_context << 8) | last_item[47]); // last_classification
      if (m_classification[context_index] == 0)
      {
        m_classification[context_index] = dec->createSymbolModel(256);
        dec->initSymbolModel(m_classification[context_index]);
      }
      // switch context based on the locally computed context
      last_item[47] = dec->decodeSymbol(m_classification[context_index]);
    }

    // decompress the first_returning_sample ... if it has changed
    if (changed_values & 32)
    {
      *((U16*)(last_item + 40)) = ic_first_returning_sample->decompress(last_first_returning_sample, two_pulse_context);
    }

    // decompress the last_returning_sample ... if the returning_sample_difference has changed
    I32 prediction_last_returning_sample = I16_CLAMP(last_returning_sample_difference + *((U16*)(last_item + 40)));
    if (changed_values & 16)
    {
      *((U16*)(last_item + 42)) = ic_last_returning_sample->decompress(prediction_last_returning_sample, two_pulse_context);
    }
    else
    {
      *((U16*)(last_item + 42)) = prediction_last_returning_sample;
    }
  }

  // decompress GPS time
  
  if (last_T_diff == 0) // if the last integer difference was zero
  {
    I32 difference = dec->decodeSymbol(m_T_last_diff_zero);
    if (difference == 1) // the difference can be represented with 32 bits
    {
      last_T_diff = ic_T->decompress(0, 0);
      multi_extreme_counter = 0; 
      last_T += last_T_diff;
    }
    else if (difference == 2) // the difference *cannot* be represented with 32 bits
    {
      last_T = (I64)dec->readInt64();
    }
    else // the difference is zero
    {
    }
  }
  else
  {
    I32 multi = dec->decodeSymbol(m_T_diff_multi);
    if (multi == PULSEZIP_T_MULTI_UNCHANGED)
    {
      last_T += last_T_diff;
      multi_extreme_counter = 0;
    }
    else if (multi == 1)
    {
      last_T += ic_T->decompress(last_T_diff, 1);
      multi_extreme_counter = 0;
    }
    else if (multi < PULSEZIP_T_MULTI_UNCHANGED)
    {
      I32 T_diff;
      if (multi == 0)
      {
        T_diff = ic_T->decompress(0, 7);
        multi_extreme_counter++;
        if (multi_extreme_counter > 3)
        {
          last_T_diff = T_diff;
          multi_extreme_counter = 0;
        }
      }
      else if (multi < PULSEZIP_T_MULTI)
      {
        if (multi < 10)
          T_diff = ic_T->decompress(multi*last_T_diff, 2);
        else
          T_diff = ic_T->decompress(multi*last_T_diff, 3);
      }
      else if (multi == PULSEZIP_T_MULTI)
      {
        T_diff = ic_T->decompress(PULSEZIP_T_MULTI*last_T_diff, 4);
        multi_extreme_counter++;
        if (multi_extreme_counter > 3)
        {
          last_T_diff = T_diff;
          multi_extreme_counter = 0;
        }
      }
      else
      {
        multi = PULSEZIP_T_MULTI - multi;
        if (multi > PULSEZIP_T_MULTI_MINUS)
        {
          T_diff = ic_T->decompress(multi*last_T_diff, 5);
        }
        else
        {
          T_diff = ic_T->decompress(PULSEZIP_T_MULTI_MINUS*last_T_diff, 6);
          multi_extreme_counter++;
          if (multi_extreme_counter > 3)
          {
            last_T_diff = T_diff;
            multi_extreme_counter = 0;
          }
        }
      }
      last_T += T_diff;
    }
    else
    {
      assert(multi ==  PULSEZIP_T_MULTI_CODE_FULL);
      last_T = (I64)dec->readInt64();
    }
  }
  *((I64*)last_item) = last_T;

  // decompress offset

  I64 offset_diff_64;
  
  if (last_offset_diff_array[last_descriptor_context] == I32_MAX)
  {
    offset_diff_64 = ic_offset_diff->decompress(last_offset_diff, last_descriptor_context);
  }
  else
  {
    offset_diff_64 = ic_offset_diff->decompress(last_offset_diff_array[last_descriptor_context], last_descriptor_context);
  }
  // sometimes the difference cannot be represented with 32 bits
  if (offset_diff_64 == I32_MIN)
  {
    offset_diff_64 = dec->readInt64();
  }
  else
  {
    last_offset_diff = (I32)offset_diff_64;
    last_offset_diff_array[last_descriptor_context] = last_offset_diff;    
  }
  *((I64*)(last_item + 8)) = *((I64*)(last_item + 8)) + offset_diff_64;

  // decompress anchor coordinate

  I32 anchor_x_diff;
  I32 anchor_y_diff;
  I32 anchor_z_diff;

  if (last_anchor_x_diff_array[last_two_bit_byte_changes] == I32_MAX)
  {
    anchor_x_diff = ic_anchor_dx->decompress(last_anchor_x_diff, last_two_bit_byte_changes);
  }
  else
  {
    anchor_x_diff = ic_anchor_dx->decompress(last_anchor_x_diff_array[last_two_bit_byte_changes], last_two_bit_byte_changes);
  }
  last_anchor_x_diff_array[last_two_bit_byte_changes] = anchor_x_diff;
  last_anchor_x_diff = anchor_x_diff;

  if (last_anchor_y_diff_array[last_two_bit_byte_changes] == I32_MAX)
  {
    anchor_y_diff = ic_anchor_dy->decompress(last_anchor_y_diff, last_two_bit_byte_changes);
  }
  else
  {
    anchor_y_diff = ic_anchor_dy->decompress(last_anchor_y_diff_array[last_two_bit_byte_changes], last_two_bit_byte_changes);
  }
  last_anchor_y_diff_array[last_two_bit_byte_changes] = anchor_y_diff;
  last_anchor_y_diff = anchor_y_diff;

  if (last_anchor_z_diff_array[last_two_bit_byte_changes] == I32_MAX)
  {
    anchor_z_diff = ic_anchor_z->decompress(last_anchor_z_diff, last_two_bit_byte_changes);
  }
  else
  {
    anchor_z_diff = ic_anchor_z->decompress(last_anchor_z_diff_array[last_two_bit_byte_changes], last_two_bit_byte_changes);
  }
  last_anchor_z_diff_array[last_two_bit_byte_changes] = anchor_z_diff;
  last_anchor_z_diff = anchor_z_diff;

  *((I32*)(last_item + 16)) += anchor_x_diff;
  *((I32*)(last_item + 20)) += anchor_y_diff;
  *((I32*)(last_item + 24)) += anchor_z_diff;

  // decompress target coordinate

  *((I32*)(last_item + 28)) = ic_target_dx->decompress(*((I32*)(last_item + 28)) + anchor_x_diff);
  *((I32*)(last_item + 32)) = ic_target_dy->decompress(*((I32*)(last_item + 32)) + anchor_y_diff);
  *((I32*)(last_item + 36)) = ic_target_z->decompress(*((I32*)(last_item + 36)) + anchor_z_diff);

/*  // compress anchor_x coordinate
  median = 0;//last_x_diff_median5[m].get();
  diff = ((PULSEpulse0*)item)->x - ((PULSEpulse0*)last_item)->x;
  ic_dx->compress(median, diff, n==1);
  //last_x_diff_median5[m].add(diff);

  // compress y coordinate
  k_bits = ic_dx->getK();
  median = 0;//last_y_diff_median5[m].get();
  diff = ((PULSEpulse0*)item)->y - ((PULSEpulse0*)last_item)->y;
  ic_dy->compress(median, diff, (n==1) + ( k_bits < 20 ? U32_ZERO_BIT_0(k_bits) : 20 ));
  //last_y_diff_median5[m].add(diff);

  // compress z coordinate
  k_bits = (ic_dx->getK() + ic_dy->getK()) / 2;
  ic_z->compress(last_height[l], ((PULSEpulse0*)item)->z, (n==1) + (k_bits < 18 ? U32_ZERO_BIT_0(k_bits) : 18));
  last_height[l] = ((PULSEpulse0*)item)->z;
*/

  memcpy(item, last_item, PULSEWAVES_PULSE0_SIZE);
}

/*
===============================================================================
                  PULSEreadItemCompressed_PULSESOURCEID16_v1
===============================================================================
*/

PULSEreadItemCompressed_PULSESOURCEID16_v1::PULSEreadItemCompressed_PULSESOURCEID16_v1(ArithmeticDecoder* dec)
{
  /* set decoder */
  assert(dec);
  this->dec = dec;

  /* create models and integer compressors */
  m_sameID = dec->createSymbolModel(PULSEZIP_PULSESOURCE16_ID_TABLE+2);
  ic_newID = new IntegerCompressor(dec, 16);
}

PULSEreadItemCompressed_PULSESOURCEID16_v1::~PULSEreadItemCompressed_PULSESOURCEID16_v1()
{
  dec->destroySymbolModel(m_sameID);
  delete [] ic_newID;
}

BOOL PULSEreadItemCompressed_PULSESOURCEID16_v1::init(const U8* item)
{
  U32 i;

  /* init state */
  oldest_pulse_source_IDs = 0;
  for (i = 0; i < PULSEZIP_PULSESOURCE16_ID_TABLE; i++)
  {
    previous_pulse_source_IDs[i] = (U16)i;
  }

  /* init models and integer compressors */
  dec->initSymbolModel(m_sameID);
  ic_newID->initDecompressor();

  /* init last pulse */
  last_pulse_source_ID = *((U16*)item);
  return TRUE;
}

inline void PULSEreadItemCompressed_PULSESOURCEID16_v1::read(U8* item)
{
  U32 code = dec->decodeSymbol(m_sameID);

  if (code == 1)
  {
    previous_pulse_source_IDs[oldest_pulse_source_IDs] = last_pulse_source_ID;
    oldest_pulse_source_IDs++;
    if (oldest_pulse_source_IDs == PULSEZIP_PULSESOURCE16_ID_TABLE)
    {
      oldest_pulse_source_IDs = 0;
    }
    last_pulse_source_ID = ic_newID->decompress(last_pulse_source_ID);
  }
  else if (code != 0)
  {
    last_pulse_source_ID = previous_pulse_source_IDs[code-2];
  }

  *((U16*)item) = last_pulse_source_ID;
}

/*
===============================================================================
                  PULSEreadItemCompressed_PULSESOURCEID32_v1
===============================================================================
*/

PULSEreadItemCompressed_PULSESOURCEID32_v1::PULSEreadItemCompressed_PULSESOURCEID32_v1(ArithmeticDecoder* dec)
{
  /* set decoder */
  assert(dec);
  this->dec = dec;

  /* create models and integer compressors */
  m_sameID = dec->createSymbolModel(PULSEZIP_PULSESOURCE32_ID_TABLE+2);
  ic_newID = new IntegerCompressor(dec, 32);
}

PULSEreadItemCompressed_PULSESOURCEID32_v1::~PULSEreadItemCompressed_PULSESOURCEID32_v1()
{
  dec->destroySymbolModel(m_sameID);
  delete [] ic_newID;
}

BOOL PULSEreadItemCompressed_PULSESOURCEID32_v1::init(const U8* item)
{
  U32 i;

  /* init state */
  oldest_pulse_source_IDs = 0;
  for (i = 0; i < PULSEZIP_PULSESOURCE32_ID_TABLE; i++)
  {
    previous_pulse_source_IDs[i] = i;
  }

  /* init models and integer compressors */
  dec->initSymbolModel(m_sameID);
  ic_newID->initDecompressor();

  /* init last pulse */
  last_pulse_source_ID = *((U32*)item);
  return TRUE;
}

inline void PULSEreadItemCompressed_PULSESOURCEID32_v1::read(U8* item)
{
  U32 code = dec->decodeSymbol(m_sameID);

  if (code == 1)
  {
    previous_pulse_source_IDs[oldest_pulse_source_IDs] = last_pulse_source_ID;
    oldest_pulse_source_IDs++;
    if (oldest_pulse_source_IDs == PULSEZIP_PULSESOURCE32_ID_TABLE)
    {
      oldest_pulse_source_IDs = 0;
    }
    last_pulse_source_ID = ic_newID->decompress(last_pulse_source_ID);
  }
  else if (code != 0)
  {
    last_pulse_source_ID = previous_pulse_source_IDs[code-2];
  }

  *((U32*)item) = last_pulse_source_ID;
}

/*
===============================================================================
                    PULSEreadItemCompressed_EXTRABYTES_v1
===============================================================================
*/

PULSEreadItemCompressed_EXTRABYTES_v1::PULSEreadItemCompressed_EXTRABYTES_v1(ArithmeticDecoder* dec, U32 number)
{
  U32 i;

  /* set decoder */
  assert(dec);
  this->dec = dec;
  assert(number);
  this->number = number;

  /* create models and integer compressors */
  m_byte = new ArithmeticModel*[number];
  for (i = 0; i < number; i++)
  {
    m_byte[i] = dec->createSymbolModel(256);
  }

  /* create last item */
  last_item = new U8[number];
}

PULSEreadItemCompressed_EXTRABYTES_v1::~PULSEreadItemCompressed_EXTRABYTES_v1()
{
  U32 i;
  for (i = 0; i < number; i++)
  {
    dec->destroySymbolModel(m_byte[i]);
  }
  delete [] m_byte;
  delete [] last_item;
}

BOOL PULSEreadItemCompressed_EXTRABYTES_v1::init(const U8* item)
{
  U32 i;
  /* init state */

  /* init models and integer compressors */
  for (i = 0; i < number; i++)
  {
    dec->initSymbolModel(m_byte[i]);
  }

  /* init last item */
  memcpy(last_item, item, number);
  return TRUE;
}

inline void PULSEreadItemCompressed_EXTRABYTES_v1::read(U8* item)
{
  U32 i;
  I32 value;
  for (i = 0; i < number; i++)
  {
    value = last_item[i] + dec->decodeSymbol(m_byte[i]);
    item[i] = U8_FOLD(value);
  }
  memcpy(last_item, item, number);
}
