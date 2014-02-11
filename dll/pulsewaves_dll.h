/*
===============================================================================

  FILE:  pulsewaves_dll.h
  
  CONTENTS:
  
    A simple DLL interface to the PulseWaves library
  
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
  
    31 October 2013 - on flight from Denpasar to Bangkok adding lookup tables
    29 October 2012 - on train from Adelaide to Parafield adding read stubs
     5 September 2012 - at RIEGL headquarters to get PulseWaves into RiProcess
  
===============================================================================
*/

#ifndef PULSEWAVES_DLL_H
#define PULSEWAVES_DLL_H

#include <math.h>

#ifdef _WIN32
#   ifdef PULSEWAVES_DYN_LINK
#       ifdef PULSEWAVES_SOURCE
#           define PULSEWAVES_API __declspec(dllexport)
#       else
#           define PULSEWAVES_API __declspec(dllimport)
#       endif
#   else
#       define PULSEWAVES_API
#   endif
#else
#   define PULSEWAVES_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef int                pulsewaves_BOOL;
typedef unsigned char      pulsewaves_U8;
typedef unsigned short     pulsewaves_U16;
typedef unsigned int       pulsewaves_U32;
typedef char               pulsewaves_I8;
typedef short              pulsewaves_I16;
typedef int                pulsewaves_I32;
typedef __int64            pulsewaves_I64;
typedef char               pulsewaves_CHAR;
typedef float              pulsewaves_F32;
typedef double             pulsewaves_F64;
typedef void*              pulsewaves_POINTER;

#define PULSEWAVESDLL_USER_ID_SIZE      16
#define PULSEWAVESDLL_DESCRIPTION_SIZE  64

#define PULSEWAVESDLL_UNDEFINED         0

#define PULSEWAVESDLL_OUTGOING          1
#define PULSEWAVESDLL_RETURNING         2

#define PULSEWAVESDLL_OSCILLATING       1
#define PULSEWAVESDLL_LINE              2
#define PULSEWAVESDLL_CONIC             3

#define PULSEWAVESDLL_OPTICAL_CENTER_AND_ANCHOR_POINT_COINCIDE  0x0 
#define PULSEWAVESDLL_OPTICAL_CENTER_AND_ANCHOR_POINT_FLUCTUATE 0x8FFFFFFF

#define PULSEWAVESDLL_PULSE_FORMAT_0                              0
#define PULSEWAVESDLL_PULSE_ATTRIBUTES_PULSE_SOURCE_ID_16BIT      0x00000001
#define PULSEWAVESDLL_PULSE_ATTRIBUTES_PULSE_SOURCE_ID_32BIT      0x00000002

#define PULSEWAVESDLL_PULSE_FORMAT_0_SIZE                         48
#define PULSEWAVESDLL_PULSE_ATTRIBUTES_PULSE_SOURCE_ID_16BIT_SIZE 2
#define PULSEWAVESDLL_PULSE_ATTRIBUTES_PULSE_SOURCE_ID_32BIT_SIZE 4

#define PULSEWAVESDLL_EMPTY_TABLE_ENTRY           -2.0e+37f

#define PULSEWAVESDLL_TABLE_UNDEFINED             0
#define PULSEWAVESDLL_TABLE_INTENSITY_CORRECTION  1
#define PULSEWAVESDLL_TABLE_RANGE_CORRECTION      2

typedef struct pulsewaves_header
{
  pulsewaves_CHAR file_signature[16];                                   /* read only */
  pulsewaves_U32 global_parameters;                                     /* read only */
  pulsewaves_U32 file_source_ID;
  pulsewaves_U32 project_ID_GUID_data_1;
  pulsewaves_U16 project_ID_GUID_data_2;
  pulsewaves_U16 project_ID_GUID_data_3;
  pulsewaves_U8 project_ID_GUID_data_4[8];
  pulsewaves_CHAR system_identifier[PULSEWAVESDLL_DESCRIPTION_SIZE];
  pulsewaves_CHAR generating_software[PULSEWAVESDLL_DESCRIPTION_SIZE];  /* read only */
  pulsewaves_U16 file_creation_day;
  pulsewaves_U16 file_creation_year;
  pulsewaves_U8 version_major;                                          /* read only */
  pulsewaves_U8 version_minor;                                          /* read only */
  pulsewaves_U16 header_size;                                           /* read only */
  pulsewaves_I64 offset_to_pulse_data;                                  /* read only */
  pulsewaves_I64 number_of_pulses;
  pulsewaves_U32 pulse_format;
  pulsewaves_U32 pulse_attributes;
  pulsewaves_U32 pulse_size;
  pulsewaves_U32 pulse_compression;                                     /* read only */
  pulsewaves_I64 reserved;                                              /* read only */
  pulsewaves_U32 number_of_variable_length_records;                     /* read only */
  pulsewaves_I32 number_of_appended_variable_length_records;            /* read only */
  pulsewaves_F64 t_scale_factor;
  pulsewaves_F64 t_offset;
  pulsewaves_I64 min_T;
  pulsewaves_I64 max_T;
  pulsewaves_F64 x_scale_factor;
  pulsewaves_F64 y_scale_factor;
  pulsewaves_F64 z_scale_factor;
  pulsewaves_F64 x_offset;
  pulsewaves_F64 y_offset;
  pulsewaves_F64 z_offset;
  pulsewaves_F64 min_x;
  pulsewaves_F64 max_x;
  pulsewaves_F64 min_y;
  pulsewaves_F64 max_y;
  pulsewaves_F64 min_z;
  pulsewaves_F64 max_z;
} pulsewaves_header_struct;

typedef struct pulsewaves_scanner
{
  pulsewaves_CHAR instrument[PULSEWAVESDLL_DESCRIPTION_SIZE];
  pulsewaves_CHAR serial[PULSEWAVESDLL_DESCRIPTION_SIZE];
  pulsewaves_F32 wave_length;                    // [nanometer]
  pulsewaves_F32 outgoing_pulse_width;           // [nanoseconds]
  pulsewaves_U32 scan_pattern;                   // 0 = undefined, 1 = oscillating, 2 = line, 3 = conic
  pulsewaves_U32 number_of_mirror_facets;
  pulsewaves_F32 scan_frequency;                 // [hertz]
  pulsewaves_F32 scan_angle_min;                 // [degree]
  pulsewaves_F32 scan_angle_max;                 // [degree]
  pulsewaves_F32 pulse_frequency;                // [kilohertz]
  pulsewaves_F32 beam_diameter_at_exit_aperture; // [millimeters]
  pulsewaves_F32 beam_divergence;                // [milliradians]
  pulsewaves_F32 minimal_range;                  // [meters]
  pulsewaves_F32 maximal_range;                  // [meters]
  pulsewaves_CHAR description[PULSEWAVESDLL_DESCRIPTION_SIZE];
} pulsewaves_scanner_struct;

typedef struct pulsewaves_lookup_table
{
  pulsewaves_U32 number_entries;                 
  pulsewaves_U16 unit_of_measurement;            // 0 = undefined
  pulsewaves_U8  data_type;                      // must be float (8)
  pulsewaves_U8  options;                        // must be 0
  pulsewaves_CHAR description[PULSEWAVESDLL_DESCRIPTION_SIZE];
  pulsewaves_U8* entries;
} pulsewaves_lookup_table_struct;

typedef struct pulsewaves_pulsecomposition
{
  pulsewaves_I32 optical_center_to_anchor_point; // [sampling units]
  pulsewaves_U16 number_of_extra_waves_bytes;
  pulsewaves_U16 number_of_samplings;
  pulsewaves_F32 sample_units;                   // [nanoseconds]
  pulsewaves_U32 compression;
  pulsewaves_U32 scanner_index;
  pulsewaves_U32 reserved;
  pulsewaves_CHAR description[PULSEWAVESDLL_DESCRIPTION_SIZE];
} pulsewaves_pulsecomposition_struct;

typedef struct pulsewaves_pulsesampling
{
  pulsewaves_U8 type;
  pulsewaves_U8 channel;
  pulsewaves_U8 unused;
  pulsewaves_U8 bits_for_duration_from_anchor;
  pulsewaves_F32 scale_for_duration_from_anchor;
  pulsewaves_F32 offset_for_duration_from_anchor;
  pulsewaves_U8 bits_for_number_of_segments;
  pulsewaves_U8 bits_for_number_of_samples;
  pulsewaves_U16 number_of_segments;
  pulsewaves_U32 number_of_samples;
  pulsewaves_U16 bits_per_sample;
  pulsewaves_U16 lookup_table_index;
  pulsewaves_F32 sample_units;                   // [nanoseconds]
  pulsewaves_CHAR description[PULSEWAVESDLL_DESCRIPTION_SIZE];
} pulsewaves_pulsesampling_struct;

typedef struct pulsewaves_pulsekeyentry
{
  pulsewaves_U16 key_id;
  pulsewaves_U16 tiff_tag_location;
  pulsewaves_U16 count;
  pulsewaves_U16 value_offset;
} pulsewaves_pulsekeyentry_struct;

typedef struct pulsewaves_pulsevlr
{
  pulsewaves_CHAR user_id[PULSEWAVESDLL_USER_ID_SIZE]; 
  pulsewaves_U32 record_id;
  pulsewaves_U32 reserved;                                              /* read only */
  pulsewaves_I64 record_length_after_header;
  pulsewaves_CHAR description[PULSEWAVESDLL_DESCRIPTION_SIZE];          /* read only */
} pulsewaves_pulsevlr_struct;

typedef struct pulsewaves_pulse
{
  pulsewaves_I64 T;
  pulsewaves_F64 anchor[3];
  pulsewaves_F64 target[3];
  pulsewaves_I16 first_returning_sample;
  pulsewaves_I16 last_returning_sample;
  pulsewaves_U16 descriptor_index;
  pulsewaves_U16 edge_of_scan_line;
  pulsewaves_U16 scan_direction;
  pulsewaves_U16 mirror_facet;
  pulsewaves_U8 intensity;
  pulsewaves_U8 classification;
  pulsewaves_U32 pulse_source_ID;
} pulsewaves_pulse_struct;

typedef struct pulsewaves_wavessampling
{
  pulsewaves_I32 num_segments;
  pulsewaves_I32* num_samples;
  pulsewaves_F32* durations;
  pulsewaves_U8** samples;
} pulsewaves_wavessampling_struct;

/*---------------------------------------------------------------------------*/
/*-------------- DLL functions to manage a PulseWaves file ------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_load_dll
(
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_get_version
(
    pulsewaves_U8*                         version_major
    , pulsewaves_U8*                       version_minor
    , pulsewaves_U16*                      version_revision
    , pulsewaves_U32*                      version_build
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_get_error
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_CHAR**                    error
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_create(
    pulsewaves_POINTER*                    pointer
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_destroy(
    pulsewaves_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
/*------------- DLL functions to write and read PulseWaves ------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get(
    pulsewaves_POINTER                     pointer
    , pulsewaves_header_struct*            header
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set(
    pulsewaves_POINTER                     pointer
    , pulsewaves_header_struct*            header
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_scanner(
    pulsewaves_POINTER                     pointer
    , pulsewaves_scanner_struct*           scanner
    , pulsewaves_U32                       scanner_index
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_scanner(
    pulsewaves_POINTER                     pointer
    , pulsewaves_scanner_struct*           scanner
    , pulsewaves_U32                       scanner_index
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_lookup_tables(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_lookup_table_struct**     tables
    , pulsewaves_U32                       lookup_tables_index
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_lookup_tables(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_lookup_table_struct*      tables
    , pulsewaves_U32                       lookup_tables_index
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_pulsedescriptor(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsecomposition_struct** composition
    , pulsewaves_pulsesampling_struct**    samplings
    , pulsewaves_U32                       descriptor_index
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_pulsedescriptor(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsecomposition_struct*  descriptor
    , pulsewaves_pulsesampling_struct*     samplings
    , pulsewaves_U32                       descriptor_index
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set_geokey_entries(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_pulsekeyentry_struct*     key_entries
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_geokey_entries(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_pulsekeyentry_struct**    key_entries
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set_geodouble_params(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_F64*                      geodouble_params
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_geodouble_params(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_F64**                     geodouble_params
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set_geoascii_params(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_CHAR*                     geodouble_ascii
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_geoascii_params(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_CHAR**                    geodouble_ascii
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_vlr(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsevlr_struct*          vlr
    , pulsewaves_U8*                       data
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_open(
    pulsewaves_POINTER                     pointer
    , const pulsewaves_CHAR*               file_name
    , pulsewaves_BOOL                      compress
    );

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_write_pulse(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulse_struct*             pulse
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_write_waves(
    pulsewaves_POINTER                     pointer
    , pulsewaves_wavessampling_struct*     wavessamplings
    , pulsewaves_U32                       number_of_wavessamplings
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_close(
    pulsewaves_POINTER                     pointer
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_open(
    pulsewaves_POINTER                     pointer
    , const pulsewaves_CHAR*               file_name
    , pulsewaves_BOOL*                     is_compressed
    );

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_seek_pulse(
    pulsewaves_POINTER                     pointer
    , pulsewaves_I64                       index
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_read_pulse(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulse_struct*             pulse
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_read_waves(
    pulsewaves_POINTER                     pointer
    , pulsewaves_wavessampling_struct**    wavessamplings
    , pulsewaves_U32*                      number_of_wavessamplings
);

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_close(
    pulsewaves_POINTER                     pointer
);

#ifdef __cplusplus
}
#endif

#endif /* PULSEWAVES_DLL_H */
