/*
===============================================================================

  FILE:  pulsedescriptor.hpp
  
  CONTENTS:
  
    Describes the way that outgoing and returning waveform of a pulse is stored
    in the PulseWaves. There can be multiple pulsedescriptors each describing a
    different composition of samplings.

  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2007-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    02 March 2012 -- created before celebrating Silke's birthday at Hafen 2
  
===============================================================================
*/
#ifndef PULSE_DESCRIPTOR_HPP
#define PULSE_DESCRIPTOR_HPP

#include "mydefs.hpp"
#include "pulsewavesdefinitions.hpp"

class ByteStreamIn;
class ByteStreamOut;

class PULSEsampling
{
public:
  // start of attributes
  U32 size;                                    // byte-aligned size from start to end of attributes (including the PULSEWAVES_DESCRIPTION_SIZE bytes for description)
  U32 reserved;                                // must be zero
  U8 type;                                     // 0 - undefined, 1 - outgoing, 2 - returning
  U8 channel;
  U8 unused;                                   // must be zero
  U8 bits_for_duration_from_anchor;            // 0, 8, 16, or 32
  F32 scale_for_duration_from_anchor;          // default is 1.0f
  F32 offset_for_duration_from_anchor;         // default is 0.0f
  U8 bits_for_number_of_segments;              // 0 or 8 or 16
  U8 bits_for_number_of_samples;               // 0 or 8 or 16
  U16 number_of_segments;
  U32 number_of_samples;
  U16 bits_per_sample;                         // 8 or 16
  U16 lookup_table_index;                      // index of 1 or higher to PULSEtable stored in VLR/AVLR, 0 means no lookup table. 
  F32 sample_units;                            // [nanoseconds]
  U32 compression;                             // must be zero
  // space for new attributes
  // ...
  // space for new attributes
  CHAR description[PULSEWAVES_DESCRIPTION_SIZE];
  // end of attributes

  U32 size_of_attributes() const;

  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;

  BOOL is_equal(const PULSEsampling* sampling) const;
  PULSEsampling();
};

class PULSEcomposition
{
public:
  // start of attributes
  U32 size;                           // byte-aligned size from start to end of attributes (including the PULSEWAVES_DESCRIPTION_SIZE bytes for description)
  U32 reserved;                       // must be zero
  I32 optical_center_to_anchor_point; // a fixed offset between the two [sampling units]
  U16 number_of_extra_waves_bytes;    // must be zero
  U16 number_of_samplings;
  U32 scanner_index;
  F32 sample_units;                   // [nanoseconds]
  U32 compression;                    // must be zero
  // space for new attributes
  // ...
  // space for new attributes
  CHAR description[PULSEWAVES_DESCRIPTION_SIZE];
  // end of attributes

  U32 size_of_attributes() const;

  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;

  BOOL is_equal(const PULSEcomposition* composition) const;
  PULSEcomposition();
};

class PULSEdescriptor
{
public:
  PULSEcomposition* composition;
  PULSEsampling* samplings;

  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;

  BOOL is_equal(const PULSEcomposition* composition, const PULSEsampling* sampling) const;
  BOOL is_equal(const PULSEdescriptor* descriptor) const;
  PULSEdescriptor();
};

#endif
