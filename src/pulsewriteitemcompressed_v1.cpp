/*
===============================================================================

  FILE:  pulsewriteitemcompressed_v1.cpp
  
  CONTENTS:
  
    see corresponding header file
  
  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2010-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    see corresponding header file
  
===============================================================================
*/

#include "pulsewriteitemcompressed_v1.hpp"

#include <assert.h>
#include <string.h>

/*
===============================================================================
                  PULSEwriteItemCompressed_PULSE0_v1
===============================================================================
*/

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

PULSEwriteItemCompressed_PULSE0_v1::PULSEwriteItemCompressed_PULSE0_v1(ArithmeticEncoder* enc)
{
  /* set encoder */
  assert(enc);
  this->enc = enc;

  /* setup context mapper */
  cm_descriptor_index.setup(PULSEZIP_DESCRIPTOR_CONTEXT_BITS);
  cm_bit_byte.setup(PULSEZIP_BIT_BYTE_CONTEXT_BITS);
  cm_classification.setup(PULSEZIP_CLASSIFICATION_CONTEXT_BITS);

  /* create models and integer compressors */
  m_T_diff_multi = enc->createSymbolModel(PULSEZIP_T_MULTI_TOTAL);
  m_T_last_diff_zero = enc->createSymbolModel(3);
  memset(m_changed_values, 0, PULSEZIP_CHANGED_VALUES_CONTEXT_NUMBER*sizeof(ArithmeticModel*));
  memset(m_descriptor_index, 0, PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER*sizeof(ArithmeticModel*));
  memset(m_bit_byte, 0, PULSEZIP_BIT_BYTE_CONTEXT_NUMBER*sizeof(ArithmeticModel*));
  memset(m_classification, 0, PULSEZIP_CLASSIFICATION_CONTEXT_NUMBER*sizeof(ArithmeticModel*));
  ic_T = new IntegerCompressor(enc, 32, 9); // 32 bits, 9 contexts
  ic_offset_diff = new IntegerCompressor(enc, 32, PULSEZIP_LAST_OFFSET_DIFF_ARRAY_SIZE);            // 32 bits, PULSEZIP_LAST_OFFSET_DIFF_ARRAY_SIZE context
  ic_anchor_dx = new IntegerCompressor(enc, 32, 4);                // 32 bits, 4 context
  ic_anchor_dy = new IntegerCompressor(enc, 32, 4);               // 32 bits, 4 contexts
  ic_anchor_z = new IntegerCompressor(enc, 32, 4);                // 32 bits, 4 contexts
  ic_target_dx = new IntegerCompressor(enc, 32, 4);                // 32 bits, 4 context
  ic_target_dy = new IntegerCompressor(enc, 32, 4);               // 32 bits, 4 contexts
  ic_target_z = new IntegerCompressor(enc, 32, 4);                // 32 bits, 4 contexts
  ic_first_returning_sample = new IntegerCompressor(enc, 16, PULSEZIP_TWO_PULSE_CONTEXT_NUMBER); // 16 bits, PULSEZIP_TWO_PULSE_CONTEXT_NUMBER context
  ic_last_returning_sample = new IntegerCompressor(enc, 16, PULSEZIP_TWO_PULSE_CONTEXT_NUMBER);  // 16 bits, PULSEZIP_TWO_PULSE_CONTEXT_NUMBER context
  ic_intensity = new IntegerCompressor(enc, 8, PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER);
}

PULSEwriteItemCompressed_PULSE0_v1::~PULSEwriteItemCompressed_PULSE0_v1()
{
  U32 i;
 
  enc->destroySymbolModel(m_T_diff_multi);
  enc->destroySymbolModel(m_T_last_diff_zero);

  for (i = 0; i < PULSEZIP_CHANGED_VALUES_CONTEXT_NUMBER; i++)
  {
    if (m_changed_values[i]) enc->destroySymbolModel(m_changed_values[i]);
  }
  for (i = 0; i < PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER; i++)
  {
    if (m_descriptor_index[i]) enc->destroySymbolModel(m_descriptor_index[i]);
  }
  for (i = 0; i < PULSEZIP_BIT_BYTE_CONTEXT_NUMBER; i++)
  {
    if (m_bit_byte[i]) enc->destroySymbolModel(m_bit_byte[i]);
  }
  for (i = 0; i < PULSEZIP_CLASSIFICATION_CONTEXT_NUMBER; i++)
  {
    if (m_classification[i]) enc->destroySymbolModel(m_classification[i]);
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

BOOL PULSEwriteItemCompressed_PULSE0_v1::init(const U8* item)
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
  enc->initSymbolModel(m_T_diff_multi);
  enc->initSymbolModel(m_T_last_diff_zero);
  for (i = 0; i < PULSEZIP_CHANGED_VALUES_CONTEXT_NUMBER; i++)
  {
    if (m_changed_values[i]) enc->initSymbolModel(m_changed_values[i]);
  }
  for (i = 0; i < PULSEZIP_DESCRIPTOR_CONTEXT_NUMBER; i++)
  {
    if (m_descriptor_index[i]) enc->initSymbolModel(m_descriptor_index[i]);
  }
  for (i = 0; i < PULSEZIP_BIT_BYTE_CONTEXT_NUMBER; i++)
  {
    if (m_bit_byte[i]) enc->initSymbolModel(m_bit_byte[i]);
  }
  for (i = 0; i < PULSEZIP_CLASSIFICATION_CONTEXT_NUMBER; i++)
  {
    if (m_classification[i]) enc->initSymbolModel(m_classification[i]);
  }
  ic_T->initCompressor();
  ic_offset_diff->initCompressor();
  ic_anchor_dx->initCompressor();
  ic_anchor_dy->initCompressor();
  ic_anchor_z->initCompressor();
  ic_target_dx->initCompressor();
  ic_target_dy->initCompressor();
  ic_target_z->initCompressor();
  ic_first_returning_sample->initCompressor();
  ic_last_returning_sample->initCompressor();
  ic_intensity->initCompressor();

  /* init last item */
  memcpy(last_item, item, PULSEWAVES_PULSE0_SIZE);

  return TRUE;
}

inline void PULSEwriteItemCompressed_PULSE0_v1::write(const U8* item)
{
  I32 last_first_returning_sample = *((U16*)(last_item + 40));
  I32 last_last_returning_sample = *((U16*)(last_item + 42));
  I32 last_returning_sample_difference = last_last_returning_sample - last_first_returning_sample;

  I32 first_returning_sample = *((U16*)(item + 40));
  I32 last_returning_sample = *((U16*)(item + 42));
  I32 returning_sample_difference = last_returning_sample - first_returning_sample;

  // this context combines a change in the last bit byte with the last descriptor context 

  U32 remaining_context = last_bit_byte_change_mask | last_descriptor_context;

  // this context expresses a change in the last two bit bytes

  U32 last_two_bit_byte_changes = (U32)(last_bit_byte_change_mask != 0);

  // compress which values have changed
  I32 changed_values = ((last_first_returning_sample != first_returning_sample) << 5) |           // first_returning_sample
                       ((last_returning_sample_difference != returning_sample_difference) << 4) | // returning_sample_difference
                       ((last_item[44] != item[44]) << 3) |                                       // descriptor_index
                       ((last_item[45] != item[45]) << 2) |                                       // bit_byte
                       ((last_item[46] != item[46]) << 1) |                                       // intensity  
                       ((last_item[47] != item[47]) << 0);                                        // classification

  if (m_changed_values[remaining_context] == 0)
  {
    m_changed_values[remaining_context] = enc->createSymbolModel(64);
    enc->initSymbolModel(m_changed_values[remaining_context]);
  }
  // switch context based on a change in the last bit byte and the last descriptor context
  enc->encodeSymbol(m_changed_values[remaining_context], changed_values);

  if (changed_values)
  {
    // compress the descriptor_index ... if it has changed
    if (changed_values & 8)
    {
      if (m_descriptor_index[remaining_context] == 0)
      {
        m_descriptor_index[remaining_context] = enc->createSymbolModel(256);
        enc->initSymbolModel(m_descriptor_index[remaining_context]);
      }
      // switch context based on a change in the last bit byte and the last descriptor context
      enc->encodeSymbol(m_descriptor_index[remaining_context], item[44]);
      // get a new descriptor context
      last_descriptor_context = cm_descriptor_index.getContext(item[44]);
      // update the remaining context with the new descriptor context
      remaining_context = last_bit_byte_change_mask | last_descriptor_context;
    }

    // this context adds a change in the current bit byte to the remaining context

    U32 two_pulse_context = remaining_context;

    // compress the bit_byte (reserved, edge_of_flight_line, scan_direction_flag, mirror facet) ... if it has changed
    if (changed_values & 4)
    {
      // compute a local context
      U32 context_index = cm_bit_byte.getContext((remaining_context << 8) | last_item[45]); // last_bit_byte
      if (m_bit_byte[context_index] == 0)
      {
        m_bit_byte[context_index] = enc->createSymbolModel(256);
        enc->initSymbolModel(m_bit_byte[context_index]);
      }
      // switch context based on the locally computed context
      enc->encodeSymbol(m_bit_byte[context_index], item[45]);
      // marks that there was a change in the bit byte for the next time around
      last_bit_byte_change_mask = PULSEZIP_LAST_BIT_BYTE_CHANGE_MASK;
      // marks that there was a change in the current bit byte
      two_pulse_context |= PULSEZIP_BIT_BYTE_CHANGE_MASK;
      last_two_bit_byte_changes |= 2;
    }

    // compress the intensity ... if it has changed
    if (changed_values & 2)
    {
      ic_intensity->compress(last_item[46], item[46], last_descriptor_context);
    }

    // compress the classification ... if it has changed
    if (changed_values & 1)
    {
      // compute a local context
      U32 context_index = cm_classification.getContext((remaining_context << 8) | last_item[47]); // last_classification
      if (m_classification[context_index] == 0)
      {
        m_classification[context_index] = enc->createSymbolModel(256);
        enc->initSymbolModel(m_classification[context_index]);
      }
      // switch context based on the locally computed context
      enc->encodeSymbol(m_classification[context_index], item[47]);
    }

    // compress the first_returning_sample ... if it has changed
    if (changed_values & 32)
    {
      ic_first_returning_sample->compress(last_first_returning_sample, first_returning_sample, two_pulse_context);
    }

    // compress the last_returning_sample ... if the returning_sample_difference has changed
    if (changed_values & 16)
    {
      I32 prediction_last_returning_sample = I16_CLAMP(last_returning_sample_difference + first_returning_sample);
      ic_last_returning_sample->compress(prediction_last_returning_sample, last_returning_sample, two_pulse_context);
    }
  }

  // compress GPS time stamp
  
  I64 T = *((I64*)item);

  // calculate the difference to the previous time stamp

  I64 T_diff_64 = T - last_T;
  I32 T_diff = (I32)T_diff_64;

  // ideally the difference can be represented with 32 bits

  if (T_diff_64 == (I64)(T_diff))
  {
    if (last_T_diff == 0) // the previous difference was zero
    {
      if (T == last_T)
      {
        enc->encodeSymbol(m_T_last_diff_zero, 0); // the time stamp has not changed
      }
      else
      {
        enc->encodeSymbol(m_T_last_diff_zero, 1); // the time stamp has changed
        ic_T->compress(0, T_diff, 0);
        last_T_diff = T_diff;
        multi_extreme_counter = 0; 
      }
    }
    else if (last_T_diff == T_diff) // the previous difference was not zero and is identical to the current difference
    {
      enc->encodeSymbol(m_T_diff_multi, PULSEZIP_T_MULTI_UNCHANGED);
      multi_extreme_counter = 0;
    }
    else // the previous difference was not zero and is *not* identical to the current difference
    {
      // compute multiplier between current and previous difference
      F32 multi_f = ((F32)T_diff) / ((F32)(last_T_diff));
      I32 multi = I32_QUANTIZE(multi_f);

      // compress the residual T_diff in dependance on the multiplier
      if (multi == 1)
      {
        // this is the case we assume we get most often for regular spaced pulses
        enc->encodeSymbol(m_T_diff_multi, 1);
        ic_T->compress(last_T_diff, T_diff, 1);
        multi_extreme_counter = 0; 
      }
      else if (multi > 0)
      {
        if (multi < PULSEZIP_T_MULTI) // positive multipliers up to PULSEZIP_T_MULTI are compressed directly
        {
          enc->encodeSymbol(m_T_diff_multi, multi);
          if (multi < 10)
            ic_T->compress(multi*last_T_diff, T_diff, 2);
          else
            ic_T->compress(multi*last_T_diff, T_diff, 3);
        }
        else
        {
          enc->encodeSymbol(m_T_diff_multi, PULSEZIP_T_MULTI);
          ic_T->compress(PULSEZIP_T_MULTI*last_T_diff, T_diff, 4);
          multi_extreme_counter++;
          if (multi_extreme_counter > 3)
          {
            last_T_diff = T_diff;
            multi_extreme_counter = 0;
          }
        }
      }
      else if (multi < 0)
      {
        if (multi > PULSEZIP_T_MULTI_MINUS) // negative multipliers larger than PULSEZIP_T_MULTI_MINUS are compressed directly
        {
          enc->encodeSymbol(m_T_diff_multi, PULSEZIP_T_MULTI - multi);
          ic_T->compress(multi*last_T_diff, T_diff, 5);
        }
        else
        {
          enc->encodeSymbol(m_T_diff_multi, PULSEZIP_T_MULTI - PULSEZIP_T_MULTI_MINUS);
          ic_T->compress(PULSEZIP_T_MULTI_MINUS*last_T_diff, T_diff, 6);
          multi_extreme_counter++;
          if (multi_extreme_counter > 3)
          {
            last_T_diff = T_diff;
            multi_extreme_counter = 0;
          }
        }
      }
      else
      {
        enc->encodeSymbol(m_T_diff_multi, 0);
        ic_T->compress(0, T_diff, 7);
        multi_extreme_counter++;
        if (multi_extreme_counter > 3)
        {
          last_T_diff = T_diff;
          multi_extreme_counter = 0;
        }
      }
    }
  }
  else // the difference is huge
  {
    if (last_T_diff == 0)
    {
      enc->encodeSymbol(m_T_last_diff_zero, 2); // the previous difference was zero
    }
    else
    {
      enc->encodeSymbol(m_T_diff_multi, PULSEZIP_T_MULTI_CODE_FULL); // the previous difference was non-zero
    }
    enc->writeInt64((U64)T);
  }
  last_T = T;

  // compress offset

  I64 offset = *((I64*)(item + 8));
  I64 offset_diff_64 = offset - *((I64*)(last_item + 8));
  I32 offset_diff = (I32)offset_diff_64;

  // ideally the difference can be represented with 32 bits
  if ((offset_diff_64 == (I64)(offset_diff)) && (offset_diff != I32_MIN))
  {
    if (last_offset_diff_array[last_descriptor_context] == I32_MAX)
    {
      ic_offset_diff->compress(last_offset_diff, offset_diff, last_descriptor_context);
    }
    else
    {
      ic_offset_diff->compress(last_offset_diff_array[last_descriptor_context], offset_diff, last_descriptor_context);
    }
    last_offset_diff_array[last_descriptor_context] = offset_diff;
    last_offset_diff = offset_diff;
  }
  else
  {
    if (last_offset_diff_array[last_descriptor_context])
    {
      ic_offset_diff->compress(last_offset_diff_array[last_descriptor_context], I32_MIN, last_descriptor_context);
    }
    else
    {
      ic_offset_diff->compress(last_offset_diff, offset_diff, last_descriptor_context);
    }
    enc->writeInt64(offset);
  }

  // compress difference of anchor coordinate in respect to previous difference

  I32 anchor_x_diff = *((I32*)(item + 16)) - *((I32*)(last_item + 16));
  I32 anchor_y_diff = *((I32*)(item + 20)) - *((I32*)(last_item + 20));
  I32 anchor_z_diff = *((I32*)(item + 24)) - *((I32*)(last_item + 24));

  if (last_anchor_x_diff_array[last_two_bit_byte_changes] == I32_MAX)
  {
    ic_anchor_dx->compress(last_anchor_x_diff, anchor_x_diff, last_two_bit_byte_changes);
  }
  else
  {
    ic_anchor_dx->compress(last_anchor_x_diff_array[last_two_bit_byte_changes], anchor_x_diff, last_two_bit_byte_changes);
  }
  last_anchor_x_diff_array[last_two_bit_byte_changes] = anchor_x_diff;
  last_anchor_x_diff = anchor_x_diff;

  if (last_anchor_y_diff_array[last_two_bit_byte_changes] == I32_MAX)
  {
    ic_anchor_dy->compress(last_anchor_y_diff, anchor_y_diff, last_two_bit_byte_changes);
  }
  else
  {
    ic_anchor_dy->compress(last_anchor_y_diff_array[last_two_bit_byte_changes], anchor_y_diff, last_two_bit_byte_changes);
  }
  last_anchor_y_diff_array[last_two_bit_byte_changes] = anchor_y_diff;
  last_anchor_y_diff = anchor_y_diff;
  if (last_anchor_z_diff_array[last_two_bit_byte_changes] == I32_MAX)
  {
    ic_anchor_z->compress(last_anchor_z_diff, anchor_z_diff, last_two_bit_byte_changes);
  }
  else
  {
    ic_anchor_z->compress(last_anchor_z_diff_array[last_two_bit_byte_changes], anchor_z_diff, last_two_bit_byte_changes);
  }
  last_anchor_z_diff_array[last_two_bit_byte_changes] = anchor_z_diff;
  last_anchor_z_diff = anchor_z_diff;

  // compress difference to *predicted* target coordinate based on actual difference for anchor in respect to previous difference

  ic_target_dx->compress(*((I32*)(last_item + 28)) + anchor_x_diff, *((I32*)(item + 28)));
  ic_target_dy->compress(*((I32*)(last_item + 32)) + anchor_y_diff, *((I32*)(item + 32)));
  ic_target_z->compress(*((I32*)(last_item + 36)) + anchor_z_diff, *((I32*)(item + 36)));

  last_anchor_x_diff = anchor_x_diff;
  last_anchor_y_diff = anchor_y_diff;
  last_anchor_z_diff = anchor_z_diff;

  // copy the last item
  memcpy(last_item, item, PULSEWAVES_PULSE0_SIZE);
}

/*
===============================================================================
                  PULSEwriteItemCompressed_PULSESOURCEID16_v1
===============================================================================
*/

PULSEwriteItemCompressed_PULSESOURCEID16_v1::PULSEwriteItemCompressed_PULSESOURCEID16_v1(ArithmeticEncoder* enc)
{
  /* set encoder */
  assert(enc);
  this->enc = enc;

  /* create models and integer compressors */
  m_sameID = enc->createSymbolModel(PULSEZIP_PULSESOURCE16_ID_TABLE+2);
  ic_newID = new IntegerCompressor(enc, 16);
}

PULSEwriteItemCompressed_PULSESOURCEID16_v1::~PULSEwriteItemCompressed_PULSESOURCEID16_v1()
{
  enc->destroySymbolModel(m_sameID);
  delete [] ic_newID;
}

BOOL PULSEwriteItemCompressed_PULSESOURCEID16_v1::init(const U8* item)
{
  U32 i;

  /* init state */
  oldest_pulse_source_IDs = 0;
  for (i = 0; i < PULSEZIP_PULSESOURCE16_ID_TABLE; i++)
  {
    previous_pulse_source_IDs[i] = (U16)i;
  }

  /* init models and integer compressors */
  enc->initSymbolModel(m_sameID);
  ic_newID->initCompressor();

  /* init last pulse */
  last_pulse_source_ID = *((U16*)item);
  return TRUE;
}

inline void PULSEwriteItemCompressed_PULSESOURCEID16_v1::write(const U8* item)
{
  U16 pulse_source_ID = *((U16*)item);
  if (pulse_source_ID == last_pulse_source_ID)
  {
    enc->encodeSymbol(m_sameID, 0); // same as last
  }
  else
  {
    U32 i;
    for (i = 0; i < PULSEZIP_PULSESOURCE16_ID_TABLE; i++)
    {
      if (pulse_source_ID == previous_pulse_source_IDs[i])
      {
        break;
      }
    }
    if (i < PULSEZIP_PULSESOURCE16_ID_TABLE)
    {
      enc->encodeSymbol(m_sameID, i+2); // same as previous
    }
    else
    {
      enc->encodeSymbol(m_sameID, 1); // new
      ic_newID->compress(last_pulse_source_ID, pulse_source_ID);
      previous_pulse_source_IDs[oldest_pulse_source_IDs] = last_pulse_source_ID;
      oldest_pulse_source_IDs++;
      if (oldest_pulse_source_IDs == PULSEZIP_PULSESOURCE16_ID_TABLE)
      {
        oldest_pulse_source_IDs = 0;
      }
    }
    last_pulse_source_ID = pulse_source_ID;
  }
}

/*
===============================================================================
                  PULSEwriteItemCompressed_PULSESOURCEID32_v1
===============================================================================
*/

PULSEwriteItemCompressed_PULSESOURCEID32_v1::PULSEwriteItemCompressed_PULSESOURCEID32_v1(ArithmeticEncoder* enc)
{
  /* set encoder */
  assert(enc);
  this->enc = enc;

  /* create models and integer compressors */
  m_sameID = enc->createSymbolModel(PULSEZIP_PULSESOURCE32_ID_TABLE+2);
  ic_newID = new IntegerCompressor(enc, 32);
}

PULSEwriteItemCompressed_PULSESOURCEID32_v1::~PULSEwriteItemCompressed_PULSESOURCEID32_v1()
{
  enc->destroySymbolModel(m_sameID);
  delete [] ic_newID;
}

BOOL PULSEwriteItemCompressed_PULSESOURCEID32_v1::init(const U8* item)
{
  U32 i;

  /* init state */
  oldest_pulse_source_IDs = 0;
  for (i = 0; i < PULSEZIP_PULSESOURCE32_ID_TABLE; i++)
  {
    previous_pulse_source_IDs[i] = i;
  }

  /* init models and integer compressors */
  enc->initSymbolModel(m_sameID);
  ic_newID->initCompressor();

  /* init last pulse */
  last_pulse_source_ID = *((U32*)item);
  return TRUE;
}

inline void PULSEwriteItemCompressed_PULSESOURCEID32_v1::write(const U8* item)
{
  U16 pulse_source_ID = *((U32*)item);
  if (pulse_source_ID == last_pulse_source_ID)
  {
    enc->encodeSymbol(m_sameID, 0); // same as last
  }
  else
  {
    U32 i;
    for (i = 0; i < PULSEZIP_PULSESOURCE32_ID_TABLE; i++)
    {
      if (pulse_source_ID == previous_pulse_source_IDs[i])
      {
        break;
      }
    }
    if (i < PULSEZIP_PULSESOURCE32_ID_TABLE)
    {
      enc->encodeSymbol(m_sameID, i+2); // same as previous
    }
    else
    {
      enc->encodeSymbol(m_sameID, 1); // new
      ic_newID->compress(last_pulse_source_ID, pulse_source_ID);
      previous_pulse_source_IDs[oldest_pulse_source_IDs] = last_pulse_source_ID;
      oldest_pulse_source_IDs++;
      if (oldest_pulse_source_IDs == PULSEZIP_PULSESOURCE32_ID_TABLE)
      {
        oldest_pulse_source_IDs = 0;
      }
    }
    last_pulse_source_ID = pulse_source_ID;
  }
}

/*
===============================================================================
                    PULSEwriteItemCompressed_EXTRABYTES_v1
===============================================================================
*/

PULSEwriteItemCompressed_EXTRABYTES_v1::PULSEwriteItemCompressed_EXTRABYTES_v1(ArithmeticEncoder* enc, U32 number)
{
  U32 i;

  /* set encoder */
  assert(enc);
  this->enc = enc;
  assert(number);
  this->number = number;

  /* create models and integer compressors */
  m_byte = new ArithmeticModel*[number];
  for (i = 0; i < number; i++)
  {
    m_byte[i] = enc->createSymbolModel(256);
  }

  /* create last item */
  last_item = new U8[number];
}

PULSEwriteItemCompressed_EXTRABYTES_v1::~PULSEwriteItemCompressed_EXTRABYTES_v1()
{
  U32 i;
  for (i = 0; i < number; i++)
  {
    enc->destroySymbolModel(m_byte[i]);
  }
  delete [] m_byte;
  delete [] last_item;
}

BOOL PULSEwriteItemCompressed_EXTRABYTES_v1::init(const U8* item)
{
  U32 i;
  /* init state */

  /* init models and integer compressors */
  for (i = 0; i < number; i++)
  {
    enc->initSymbolModel(m_byte[i]);
  }

  /* init last pulse */
  memcpy(last_item, item, number);
  return TRUE;
}

inline void PULSEwriteItemCompressed_EXTRABYTES_v1::write(const U8* item)
{
  U32 i;
  I32 diff;
  for (i = 0; i < number; i++)
  {
    diff = item[i] - last_item[i];
    enc->encodeSymbol(m_byte[i], U8_FOLD(diff));
  }
  memcpy(last_item, item, number);
}

