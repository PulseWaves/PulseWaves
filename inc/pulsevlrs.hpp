/*
===============================================================================

  FILE:  pulsevlrs.hpp
  
  CONTENTS:
  
    Describes the generic and the pre-defined (Appended) Variable Length Records
    that the PulseWaves format supports.

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
  
    27 February 2012 -- created between TFS and FRA before the in-flight meal
  
===============================================================================
*/
#ifndef PULSE_VLRS_HPP
#define PULSE_VLRS_HPP

#include <stdio.h>
#include "mydefs.hpp"
#include "pulsewavesdefinitions.hpp"

class ByteStreamIn;
class ByteStreamOut;

class PULSEvlr
{
public:
  CHAR user_id[PULSEWAVES_USER_ID_SIZE]; 
  U32 record_id;
  U32 reserved;
  I64 record_length_after_header;
  CHAR description[PULSEWAVES_DESCRIPTION_SIZE];
  U8* data;

  // constructor and destructor
  PULSEvlr();
  PULSEvlr(const CHAR* user_id, U32 record_id, const CHAR* description=0, I64 record_length_after_header=0, U8* data=0);
  ~PULSEvlr();

  // for serializing
  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;
  BOOL save_to_txt(FILE* file) const;
};

class PULSEavlr
{
public:
  CHAR user_id[PULSEWAVES_USER_ID_SIZE]; 
  U32 record_id;
  U32 reserved;
  I64 record_length_before_footer;
  CHAR description[PULSEWAVES_DESCRIPTION_SIZE];
  U8* data;

  // constructor and destructor
  PULSEavlr();
  PULSEavlr(const CHAR* user_id, U32 record_id, const CHAR* description=0);
  ~PULSEavlr();

  // for serializing
  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;
  BOOL save_to_txt(FILE* file) const;
};

class PULSEscanner
{
public:
  // start of attributes
  U32 size;                           // byte-aligned size from start to end of attributes (including the PULSEWAVES_DESCRIPTION_SIZE bytes for description)
  U32 reserved; 
  CHAR instrument[PULSEWAVES_DESCRIPTION_SIZE];
  CHAR serial[PULSEWAVES_DESCRIPTION_SIZE];
  F32 wave_length;                    // [nanometer]
  F32 outgoing_pulse_width;           // [nanoseconds]
  U32 scan_pattern;                   // 0 = undefined, 1 = oscillating, 2 = line, 3 = conic
  U32 number_of_mirror_facets;        //
  F32 scan_frequency;                 // [hertz]
  F32 scan_angle_min;                 // [degree]
  F32 scan_angle_max;                 // [degree]
  F32 pulse_frequency;                // [kilohertz]
  F32 beam_diameter_at_exit_aperture; // [millimeters]
  F32 beam_divergence;                // [milliradians]
  F32 minimal_range;                  // [meters]
  F32 maximal_range;                  // [meters]
  // space for new attributes
  // ...
  // space for new attributes
  CHAR description[PULSEWAVES_DESCRIPTION_SIZE];
  // end of attributes

  U32 size_of_attributes() const;

  // for serializing
  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;
  BOOL save_to_txt(FILE* file) const;

  PULSEscanner & operator=(const PULSEscanner & scanner);
  PULSEscanner();
};

class PULSElookupTable
{
public:
  // start of attributes
  U32 size;                           // byte-aligned size from start to end of attributes (including the PULSEWAVES_DESCRIPTION_SIZE bytes for description)
  U32 reserved; 
  U32 number_entries;
  U16 unit_of_measurement;
  U8 data_type;             
  U8 options;
  // space for new attributes
  // ...
  // space for new attributes
  CHAR description[PULSEWAVES_DESCRIPTION_SIZE];
  // end of attributes

  U32 size_of_attributes() const;

  // the actual table entries
  U8* entries;

  // for serializing
  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;
  BOOL save_to_txt(FILE* file) const;

  PULSElookupTable();
  ~PULSElookupTable();
};

class PULSEtable
{
public:
  // start of attributes
  U32 size;                           // byte-aligned size from start to end of attributes (including the PULSEWAVES_DESCRIPTION_SIZE bytes for description)
  U32 reserved; 
  U32 number_tables;
  // space for new attributes
  // ...
  // space for new attributes
  CHAR description[PULSEWAVES_DESCRIPTION_SIZE];
  // end of attributes

  U32 size_of_attributes() const;

  // an array of pointers to actual lookup tables
  PULSElookupTable** tables;

  // for serializing
  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;
  BOOL save_to_txt(FILE* file) const;

  PULSEtable();
  ~PULSEtable();
};

class PULSEgeokeys
{
public:
  U16 key_directory_version;
  U16 key_revision;
  U16 minor_revision;
  U16 number_of_keys;
};

class PULSEkeyentry
{
public:
  U16 key_id;
  U16 tiff_tag_location;
  U16 count;
  U16 value_offset;
};

#endif
