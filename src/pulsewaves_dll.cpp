/*
===============================================================================

  FILE:  pulsewaves_dll.cpp
  
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

#define PULSEWAVES_DYN_LINK
#define PULSEWAVES_SOURCE
#include "pulsewaves_dll.h"

#include "pulsewaves.hpp"
#include "pulsewriter_pls.hpp"
#include "pulsereader_pls.hpp"
#include "pulsedescriptor.hpp"

typedef struct pulsewaves {
    PULSEheader header;
    PULSEpulse pulse;
    U32 waves_alloc;
    WAVESwaves** waves;
    PULSEwriterPLS writer; 
    PULSEreaderPLS reader; 
    BOOL is_reading;
    BOOL is_writing;
    BOOL pulse_was_written;
    U32 descriptor_index;
    CHAR error[1024]; 
    // memory used when reading
    I32 pulsecomposition_array_alloc;
    pulsewaves_wavessampling* pulsecomposition_array;
    I32 pulsesamplings_array_alloc;
    pulsewaves_pulsesampling* pulsesamplings_array;
    I32 wavessamplings_array_alloc;
    pulsewaves_wavessampling* wavessamplings_array;
    I32 segments_array_alloc;
    I32* segments_array_num_samples;
    F32* segments_array_durations;
    U8** segments_samples_array;
    I32 samples_array_alloc;
    U8* samples_array;
} pulsewaves_struct;

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_get_version(
    pulsewaves_U8*                         version_major
    , pulsewaves_U8*                       version_minor
    , pulsewaves_U16*                      version_revision
    , pulsewaves_U32*                      version_build
)
{
  try
  {
    *version_major = PULSEWAVES_VERSION_MAJOR;
    *version_minor = PULSEWAVES_VERSION_MINOR;
    *version_revision = PULSEWAVES_REVISION;
    *version_build = PULSEWAVES_BUILD_DATE;
  }
  catch (...)
  {
    return 1;
  }

  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_create(
    pulsewaves_POINTER*                    pointer
)
{
  if (pointer == 0) return 1;

  try
  {
    pulsewaves_struct* pulsewaves = new pulsewaves_struct;
    if (pulsewaves == 0)
    {
      return 1;
    }
    sprintf(pulsewaves->reader.header.generating_software, "PulseWaves DLL %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);
    sprintf(pulsewaves->header.generating_software, "PulseWaves DLL %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);
    pulsewaves->is_reading = FALSE;
    pulsewaves->is_writing = FALSE;
    pulsewaves->pulse_was_written = FALSE; 
    pulsewaves->descriptor_index = U32_MAX;
    pulsewaves->waves_alloc = 0;
    pulsewaves->waves = 0;
    pulsewaves->wavessamplings_array_alloc = 0;
    pulsewaves->wavessamplings_array = 0;
    pulsewaves->segments_array_alloc = 0;
    pulsewaves->segments_array_num_samples = 0;
    pulsewaves->segments_array_durations = 0;
    pulsewaves->segments_samples_array = 0;
    pulsewaves->samples_array_alloc = 0;
    pulsewaves->samples_array = 0;
    pulsewaves->error[0] = '\0';
    *pointer = pulsewaves;
  }
  catch (...)
  {
    return 1;
  }

  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_get_error(
    pulsewaves_POINTER                     pointer
    , pulsewaves_CHAR**                    error
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    *error = pulsewaves->error;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_get_error");
    return 1;
  }

  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_destroy(
    pulsewaves_POINTER                     pointer
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (pulsewaves->wavessamplings_array_alloc)
    {
      free(pulsewaves->wavessamplings_array);
    }
    if (pulsewaves->segments_array_alloc)
    {
      free(pulsewaves->segments_array_num_samples);
      free(pulsewaves->segments_array_durations);
      free(pulsewaves->segments_samples_array);
    }
    if (pulsewaves->samples_array_alloc)
    {
      free(pulsewaves->samples_array);
    }
    delete pulsewaves;
  }
  catch (...)
  {
    return 1;
  }

  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get(
    pulsewaves_POINTER                     pointer
    , pulsewaves_header_struct*            header
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (header == 0)
    {
      sprintf(pulsewaves->error, "header pointer is zero");
      return 1;
    }

    memcpy(header->file_signature, pulsewaves->reader.header.file_signature, 16);                                              /* read only */
    header->global_parameters = pulsewaves->reader.header.global_parameters;                                                   /* read only */
    header->file_source_ID = pulsewaves->reader.header.file_source_ID;
    header->project_ID_GUID_data_1 = pulsewaves->reader.header.project_ID_GUID_data_1;
    header->project_ID_GUID_data_2 = pulsewaves->reader.header.project_ID_GUID_data_2;
    header->project_ID_GUID_data_3 = pulsewaves->reader.header.project_ID_GUID_data_3;
    memcpy(header->project_ID_GUID_data_4, pulsewaves->reader.header.project_ID_GUID_data_4, 8);
    memcpy(header->system_identifier, pulsewaves->reader.header.system_identifier, PULSEWAVES_DESCRIPTION_SIZE);
    memcpy(header->generating_software, pulsewaves->reader.header.generating_software, PULSEWAVES_DESCRIPTION_SIZE);           /* read only */
    header->file_creation_day = pulsewaves->reader.header.file_creation_day;
    header->file_creation_year = pulsewaves->reader.header.file_creation_year;
    header->version_major = pulsewaves->reader.header.version_major;                                                           /* read only */
    header->version_minor = pulsewaves->reader.header.version_minor;                                                           /* read only */
    header->header_size = pulsewaves->reader.header.header_size;                                                               /* read only */
    header->offset_to_pulse_data = pulsewaves->reader.header.offset_to_pulse_data;                                             /* read only */
    header->number_of_pulses = pulsewaves->reader.header.number_of_pulses;
    header->pulse_format = pulsewaves->reader.header.pulse_format;
    header->pulse_attributes = pulsewaves->reader.header.pulse_attributes;
    header->pulse_size = pulsewaves->reader.header.pulse_size;
    header->pulse_compression = pulsewaves->reader.header.pulse_compression;                                                   /* read only */
    header->reserved = pulsewaves->reader.header.reserved;                                                                     /* read only */
    header->number_of_variable_length_records = pulsewaves->reader.header.number_of_variable_length_records;                   /* read only */
    header->number_of_appended_variable_length_records = pulsewaves->reader.header.number_of_appended_variable_length_records; /* read only */
    header->t_scale_factor = pulsewaves->reader.header.t_scale_factor;
    header->t_offset = pulsewaves->reader.header.t_offset;
    header->min_T = pulsewaves->reader.header.min_T;
    header->max_T = pulsewaves->reader.header.max_T;
    header->x_scale_factor = pulsewaves->reader.header.x_scale_factor;
    header->y_scale_factor = pulsewaves->reader.header.y_scale_factor;
    header->z_scale_factor = pulsewaves->reader.header.z_scale_factor;
    header->x_offset = pulsewaves->reader.header.x_offset;
    header->y_offset = pulsewaves->reader.header.y_offset;
    header->z_offset = pulsewaves->reader.header.z_offset;
    header->min_x = pulsewaves->reader.header.min_x;
    header->max_x = pulsewaves->reader.header.max_x;
    header->min_y = pulsewaves->reader.header.min_y;
    header->max_y = pulsewaves->reader.header.max_y;
    header->min_z = pulsewaves->reader.header.min_z;
    header->max_z = pulsewaves->reader.header.max_z;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_get");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set(
    pulsewaves_POINTER                     pointer
    , pulsewaves_header_struct*            header
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (header == 0)
    {
      sprintf(pulsewaves->error, "header pointer is zero");
      return 1;
    }
    
    if (pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "cannot set header after pulsewaves_writer was opened");
      return 1;
    }

    pulsewaves->header.file_source_ID = header->file_source_ID;
    pulsewaves->header.project_ID_GUID_data_1 = header->project_ID_GUID_data_1;
    pulsewaves->header.project_ID_GUID_data_2 = header->project_ID_GUID_data_2;
    pulsewaves->header.project_ID_GUID_data_3 = header->project_ID_GUID_data_3;
    memcpy(pulsewaves->header.project_ID_GUID_data_4, header->project_ID_GUID_data_4, 8);
    memcpy(pulsewaves->header.system_identifier, header->system_identifier, PULSEWAVES_DESCRIPTION_SIZE);
    pulsewaves->header.file_creation_day = header->file_creation_day;
    pulsewaves->header.file_creation_year = header->file_creation_year;
    pulsewaves->header.number_of_pulses = header->number_of_pulses;
    pulsewaves->header.pulse_format = header->pulse_format;
    pulsewaves->header.pulse_attributes = header->pulse_attributes;
    pulsewaves->header.pulse_size = header->pulse_size;
    pulsewaves->header.t_scale_factor = header->t_scale_factor;
    pulsewaves->header.t_offset = header->t_offset;
    pulsewaves->header.min_T = header->min_T;
    pulsewaves->header.max_T = header->max_T;
    pulsewaves->header.x_scale_factor = header->x_scale_factor;
    pulsewaves->header.y_scale_factor = header->y_scale_factor;
    pulsewaves->header.z_scale_factor = header->z_scale_factor;
    pulsewaves->header.x_offset = header->x_offset;
    pulsewaves->header.y_offset = header->y_offset;
    pulsewaves->header.z_offset = header->z_offset;
    pulsewaves->header.min_x = header->min_x;
    pulsewaves->header.max_x = header->max_x;
    pulsewaves->header.min_y = header->min_y;
    pulsewaves->header.max_y = header->max_y;
    pulsewaves->header.min_z = header->min_z;
    pulsewaves->header.max_z = header->max_z;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_set");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_scanner(
    pulsewaves_POINTER                     pointer
    , pulsewaves_scanner_struct*           scanner
    , pulsewaves_U32                       scanner_index
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (scanner == 0)
    {
      sprintf(pulsewaves->error, "scanner pointer is zero");
      return 1;
    }

    if (scanner_index > PULSEWAVES_SCANNER_MAX)
    {
      sprintf(pulsewaves->error, "scanner_index %u outside of valid range from %u to %u", scanner_index, 0, PULSEWAVES_SCANNER_MAX);
      return 1;
    }

    PULSEscanner s;
    if (scanner_index > 0)
    {
      if (!pulsewaves->reader.header.get_scanner(&s, scanner_index))
      {
        sprintf(pulsewaves->error, "cannot get scanner with scanner_index %u", scanner_index);
        return 1;
      }
    }

    memcpy(scanner->instrument, s.instrument, PULSEWAVES_DESCRIPTION_SIZE);
    memcpy(scanner->serial, s.serial, PULSEWAVES_DESCRIPTION_SIZE);
    scanner->wave_length = s.wave_length;                                       // [nanometer]
    scanner->outgoing_pulse_width = s.outgoing_pulse_width;                     // [nanoseconds]
    scanner->scan_pattern = s.scan_pattern;                                     // 0 = undefined, 1 = oscillating, 2 = line, 3 = conic
    scanner->number_of_mirror_facets = s.number_of_mirror_facets;               // 0 = undefined
    scanner->scan_frequency = s.scan_frequency;                                 // [hertz]
    scanner->scan_angle_min = s.scan_angle_min;                                 // [degree]
    scanner->scan_angle_max = s.scan_angle_max;                                 // [degree]
    scanner->pulse_frequency = s.pulse_frequency;                               // [kilohertz]
    scanner->beam_diameter_at_exit_aperture = s.beam_diameter_at_exit_aperture; // [millimeters]
    scanner->beam_divergence = s.beam_divergence;                               // [milliradians]
    scanner->minimal_range = s.minimal_range;                                   // [meters]
    scanner->maximal_range = s.maximal_range;                                   // [meters]
    memcpy(scanner->description, s.description, PULSEWAVES_DESCRIPTION_SIZE);
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_get_scanner %u", scanner_index);
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_scanner(
    pulsewaves_POINTER                     pointer
    , pulsewaves_scanner_struct*           scanner
    , pulsewaves_U32                       scanner_index
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (scanner == 0)
    {
      sprintf(pulsewaves->error, "scanner pointer is zero");
      return 1;
    }

    if (pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "cannot add scanner after pulsewaves_writer was opened");
      return 1;
    }

    if ((scanner_index < PULSEWAVES_SCANNER_MIN) || (PULSEWAVES_SCANNER_MAX < scanner_index))
    {
      sprintf(pulsewaves->error, "scanner_index %u outside of valid range from %u to %u", scanner_index, PULSEWAVES_SCANNER_MIN, PULSEWAVES_SCANNER_MAX);
      return 1;
    }

    PULSEscanner s;
    strncpy(s.instrument, scanner->instrument, PULSEWAVES_DESCRIPTION_SIZE);
    strncpy(s.serial, scanner->serial, PULSEWAVES_DESCRIPTION_SIZE);
    s.wave_length = scanner->wave_length;                                       // [nanometer]
    s.outgoing_pulse_width = scanner->outgoing_pulse_width;                     // [nanoseconds]
    s.scan_pattern = scanner->scan_pattern;                                     // 0 = undefined, 1 = oscillating, 2 = line, 3 = conic
    s.number_of_mirror_facets = scanner->number_of_mirror_facets;               // 0 = undefined
    s.scan_frequency = scanner->scan_frequency;                                 // [hertz]
    s.scan_angle_min = scanner->scan_angle_min;                                 // [degree]
    s.scan_angle_max = scanner->scan_angle_max;                                 // [degree]
    s.pulse_frequency = scanner->pulse_frequency;                               // [kilohertz]
    s.beam_diameter_at_exit_aperture = scanner->beam_diameter_at_exit_aperture; // [millimeters]
    s.beam_divergence = scanner->beam_divergence;                               // [milliradians]
    s.minimal_range = scanner->minimal_range;                                   // [meters]
    s.maximal_range = scanner->maximal_range;                                   // [meters]
    strncpy(s.description, scanner->description, PULSEWAVES_DESCRIPTION_SIZE);

    if (!pulsewaves->header.add_scanner(&s, scanner_index))
    {
      sprintf(pulsewaves->error, "adding scanner with scanner_index %u to header", scanner_index);
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_add_scanner %u", scanner_index);
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_lookup_tables(
    pulsewaves_POINTER                 pointer
    , pulsewaves_U32*                  number
    , pulsewaves_lookup_table_struct** lookup_tables
    , pulsewaves_U32                   table_index
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (lookup_tables == 0)
    {
      sprintf(pulsewaves->error, "lookup_tables pointer is zero");
      return 1;
    }

    if (table_index > PULSEWAVES_TABLE_MAX)
    {
      sprintf(pulsewaves->error, "table_index %u outside of valid range from %u to %u", table_index, 0, PULSEWAVES_TABLE_MAX);
      return 1;
    }

    PULSEtable table;
    if (table_index > 0)
    {
      if (!pulsewaves->reader.header.get_table(&table, table_index))
      {
        *number = 0;
        *lookup_tables = 0;
        return 0;
      }
    }
    *number = table.number_tables;
    if (table.number_tables)
    {
      *lookup_tables = (pulsewaves_lookup_table*)malloc(sizeof(pulsewaves_lookup_table)*table.number_tables);
      if (*lookup_tables == 0)
      {
        sprintf(pulsewaves->error, "allocating memory for %u table(s) for table_index %u", table.number_tables, table_index);
        return 1;
      }
      U32 t;
      for (t = 0; t < table.number_tables; t++)
      {
        (*lookup_tables)[t].number_entries = table.tables[t]->number_entries;
        (*lookup_tables)[t].unit_of_measurement = table.tables[t]->unit_of_measurement;
        (*lookup_tables)[t].data_type = table.tables[t]->data_type;
//        (*lookup_tables)[t]->
      }
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_get_tables %u", table_index);
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_lookup_tables(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_lookup_table_struct*      lookup_tables
    , pulsewaves_U32                       table_index
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (lookup_tables == 0)
    {
      sprintf(pulsewaves->error, "lookup_tables pointer is zero");
      return 1;
    }

    if (pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "cannot add lookup tables after pulsewaves_writer was opened");
      return 1;
    }

    if ((table_index < PULSEWAVES_TABLE_MIN) || (PULSEWAVES_TABLE_MAX < table_index))
    {
      sprintf(pulsewaves->error, "table_index %u outside of valid range from %u to %u", table_index, PULSEWAVES_TABLE_MIN, PULSEWAVES_TABLE_MAX);
      return 1;
    }

    PULSEtable table;
    table.number_tables = number;

    if (number)
    {
      table.tables = new PULSElookupTable*[number];
      if (table.tables == 0)
      {
        sprintf(pulsewaves->error, "allocating memory for %u PULSElookupTable pointer(s) for table_index %u", table.number_tables, table_index);
        return 1;
      }

      U32 t;
      for (t = 0; t < number; t++)
      {
        table.tables[t] = new PULSElookupTable;
        if (table.tables[t] == 0)
        {
          sprintf(pulsewaves->error, "allocating memory for PULSElookupTable %u for table_index %u", t, table_index);
          return 1;
        }
        table.tables[t]->number_entries = lookup_tables[t].number_entries;
        table.tables[t]->unit_of_measurement = lookup_tables[t].unit_of_measurement;
        table.tables[t]->data_type = 8;
        strncpy(table.tables[t]->description, lookup_tables[t].description, PULSEWAVES_DESCRIPTION_SIZE);
        if (lookup_tables[t].number_entries)
        {
          table.tables[t]->entries = new U8[sizeof(F32)*lookup_tables[t].number_entries];
          if (table.tables[t]->entries == 0)
          {
            sprintf(pulsewaves->error, "allocating memory for %u entries for PULSElookupTable %u of table_index %u", lookup_tables[t].number_entries, t, table_index);
            return 1;
          }
          memcpy(table.tables[t]->entries, lookup_tables[t].entries, sizeof(F32)*lookup_tables[t].number_entries);
        }
      }
    }
    sprintf(table.description, "PulseWaves DLL %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);

    if (!pulsewaves->header.add_table(&table, table_index))
    {
      sprintf(pulsewaves->error, "adding lookup tables with table_index %u to header", table_index);
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_add_tables %u", table_index);
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_pulsedescriptor(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsecomposition_struct** pulsecomposition
    , pulsewaves_pulsesampling_struct**    pulsesamplings
    , pulsewaves_U32                       pulsedescriptor_index
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (pulsecomposition == 0)
    {
      sprintf(pulsewaves->error, "pulsecomposition pointer pointer for pulsedescriptor %u is zero", pulsedescriptor_index);
      return 1;
    }

    if (pulsesamplings == 0)
    {
      sprintf(pulsewaves->error, "pulsesamplings pointer pointer for pulsedescriptor %u is zero", pulsedescriptor_index);
      return 1;
    }

    if ((pulsedescriptor_index < PULSEWAVES_DESCRIPTOR_MIN) || (PULSEWAVES_DESCRIPTOR_MAX < pulsedescriptor_index))
    {
      sprintf(pulsewaves->error, "pulsedescriptor_index %u outside of valid range from %u to %u", pulsedescriptor_index, PULSEWAVES_DESCRIPTOR_MIN, PULSEWAVES_DESCRIPTOR_MAX);
      return 1;
    }

    const PULSEdescriptor* pulsedescriptor = pulsewaves->header.get_descriptor(pulsedescriptor_index);

    if (pulsedescriptor)
    {

    }

/*

  PULSEcomposition composition;

  composition.optical_center_to_anchor_point = pulsecomposition->optical_center_to_anchor_point; // the optical center and the anchor point are identical
  composition.number_of_extra_waves_bytes = 0;
  composition.number_of_samplings = pulsecomposition->number_of_samplings;
  composition.sample_units = pulsecomposition->sample_units; // [nanoseconds]
  composition.scanner_index = pulsecomposition->scanner_index;
  composition.compression = 0;
  strncpy(composition.description, pulsecomposition->description, PULSEWAVES_DESCRIPTION_SIZE);

  PULSEsampling* samplings = 0;

  if (composition.number_of_samplings)
  {
    samplings = new PULSEsampling[composition.number_of_samplings];
    for (I32 i = 0; i < composition.number_of_samplings; i++)
    {
      samplings[i].type = pulsesamplings[i].type;
      samplings[i].channel = pulsesamplings[i].channel;
      samplings[i].bits_for_duration_from_anchor = pulsesamplings[i].bits_for_duration_from_anchor;
      samplings[i].scale_for_duration_from_anchor = pulsesamplings[i].scale_for_duration_from_anchor;
      samplings[i].offset_for_duration_from_anchor = pulsesamplings[i].offset_for_duration_from_anchor;
      samplings[i].bits_for_number_of_segments = pulsesamplings[i].bits_for_number_of_segments;
      samplings[i].bits_for_number_of_samples = pulsesamplings[i].bits_for_number_of_samples;
      samplings[i].number_of_segments = pulsesamplings[i].number_of_segments;
      samplings[i].number_of_samples = pulsesamplings[i].number_of_samples;
      samplings[i].bits_per_sample = pulsesamplings[i].bits_per_sample;
      samplings[i].sample_units = pulsesamplings[i].sample_units; // [nanoseconds]
      strncpy(samplings[i].description, pulsesamplings[i].description, PULSEWAVES_DESCRIPTION_SIZE);
    }
  }

  if (!pulsewaves->header.add_descriptor(&composition, samplings, pulsedescriptor_index))
  {
    sprintf(pulsewaves->error, "adding pulsedescriptor %u", pulsedescriptor_index);
    return 1;
  }

  if (pulsewaves->waves_alloc <= pulsedescriptor_index)
  {
    if (pulsewaves->waves == 0)
    {
      pulsewaves->waves = (WAVESwaves**)malloc(sizeof(WAVESwaves*)*(pulsedescriptor_index+1));
    }
    else
    {
      pulsewaves->waves = (WAVESwaves**)realloc(pulsewaves->waves, sizeof(WAVESwaves*)*(pulsedescriptor_index+1));
    }

    if (pulsewaves->waves == 0)
    {
      sprintf(pulsewaves->error, "allocating memory for waves[] of pulsedescriptor %u", pulsedescriptor_index);
      return 1;
    }

    memset(&(pulsewaves->waves[pulsewaves->waves_alloc]), 0, (pulsedescriptor_index - pulsewaves->waves_alloc + 1) * sizeof(WAVESwaves*));

    pulsewaves->waves_alloc = pulsedescriptor_index+1;
  }

  if (pulsewaves->waves[pulsedescriptor_index]) delete pulsewaves->waves[pulsedescriptor_index];

  pulsewaves->waves[pulsedescriptor_index] = new WAVESwaves();

  if (!pulsewaves->waves[pulsedescriptor_index]->init(pulsewaves->header.get_descriptor(pulsedescriptor_index)))
  {
    sprintf(pulsewaves->error, "initializing waves for pulsedescriptor %u", pulsedescriptor_index);
    return 1;
  }

  if (samplings) delete [] samplings;
*/

    *pulsecomposition = 0;
    *pulsesamplings = 0;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_get_pulsedescriptor %u", pulsedescriptor_index);
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_pulsedescriptor(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsecomposition_struct*  pulsecomposition
    , pulsewaves_pulsesampling_struct*     pulsesamplings
    , pulsewaves_U32                       pulsedescriptor_index
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (pulsecomposition == 0)
    {
      sprintf(pulsewaves->error, "pulsecomposition pointer pointer for pulsedescriptor %u is zero", pulsedescriptor_index);
      return 1;
    }

    if (pulsesamplings == 0)
    {
      sprintf(pulsewaves->error, "pulsesamplings pointer pointer for pulsedescriptor %u is zero", pulsedescriptor_index);
      return 1;
    }

    if (pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "cannot set header after pulsewaves_writer was opened");
      return 1;
    }

    if ((pulsedescriptor_index < PULSEWAVES_DESCRIPTOR_MIN) || (PULSEWAVES_DESCRIPTOR_MAX < pulsedescriptor_index))
    {
      sprintf(pulsewaves->error, "pulsedescriptor_index %u outside of valid range from %u to %u", pulsedescriptor_index, PULSEWAVES_DESCRIPTOR_MIN, PULSEWAVES_DESCRIPTOR_MAX);
      return 1;
    }

    PULSEcomposition composition;

    composition.optical_center_to_anchor_point = pulsecomposition->optical_center_to_anchor_point;
    composition.number_of_extra_waves_bytes = 0;
    composition.number_of_samplings = pulsecomposition->number_of_samplings;
    composition.sample_units = pulsecomposition->sample_units; // [nanoseconds]
    composition.scanner_index = pulsecomposition->scanner_index;
    composition.compression = 0;
    strncpy(composition.description, pulsecomposition->description, PULSEWAVES_DESCRIPTION_SIZE);

    PULSEsampling* samplings = 0;

    if (composition.number_of_samplings)
    {
      samplings = new PULSEsampling[composition.number_of_samplings];
      for (I32 i = 0; i < composition.number_of_samplings; i++)
      {
        // check
        if ((pulsesamplings[i].type != PULSEWAVESDLL_OUTGOING) && (pulsesamplings[i].type != PULSEWAVESDLL_RETURNING))
        {
          sprintf(pulsewaves->error, "for samplings[%d] of pulsedescriptor %u the type is %d (should be %d or %d)", i, pulsedescriptor_index, pulsesamplings[i].type, PULSEWAVESDLL_OUTGOING, PULSEWAVESDLL_RETURNING);
          return 1;
        }
        if ((pulsesamplings[i].bits_for_duration_from_anchor != 0) && (pulsesamplings[i].bits_for_duration_from_anchor != 8) && (pulsesamplings[i].bits_for_duration_from_anchor != 16) && (pulsesamplings[i].bits_for_duration_from_anchor != 32))
        {
          sprintf(pulsewaves->error, "for samplings[%d] of pulsedescriptor %u the bits_for_duration_from_anchor is %d (should be 0, 8, 16 or 32)", i, pulsedescriptor_index, pulsesamplings[i].bits_for_duration_from_anchor);
          return 1;
        }
        if (pulsesamplings[i].scale_for_duration_from_anchor <= 0.0f)
        {
          sprintf(pulsewaves->error, "for samplings[%d] of pulsedescriptor %u the scale_for_duration_from_anchor is %g (should be greater than 0)", i, pulsedescriptor_index, pulsesamplings[i].scale_for_duration_from_anchor);
          return 1;
        }
        if ((pulsesamplings[i].bits_for_number_of_segments != 0) && (pulsesamplings[i].number_of_segments != 0))
        {
          sprintf(pulsewaves->error, "for samplings[%d] of pulsedescriptor %u the bits_for_number_of_segments is %d and the number_of_segments is %d (one should be zero)", i, pulsedescriptor_index, pulsesamplings[i].bits_for_number_of_segments, pulsesamplings[i].number_of_segments);
          return 1;
        }
        if ((pulsesamplings[i].bits_for_number_of_segments == 0) && (pulsesamplings[i].number_of_segments == 0))
        {
          sprintf(pulsewaves->error, "for samplings[%d] of pulsedescriptor %u the bits_for_number_of_segments is %d and the number_of_segments is %d (one should be non-zero)", i, pulsedescriptor_index, pulsesamplings[i].bits_for_number_of_segments, pulsesamplings[i].number_of_segments);
          return 1;
        }
        if ((pulsesamplings[i].bits_for_number_of_samples != 0) && (pulsesamplings[i].number_of_samples != 0))
        {
          sprintf(pulsewaves->error, "for samplings[%d] of pulsedescriptor %u the bits_for_number_of_samples is %d and the number_of_samples is %d (one should be zero)", i, pulsedescriptor_index, pulsesamplings[i].bits_for_number_of_samples, pulsesamplings[i].number_of_samples);
          return 1;
        }
        if ((pulsesamplings[i].bits_for_number_of_samples == 0) && (pulsesamplings[i].number_of_samples == 0))
        {
          sprintf(pulsewaves->error, "for samplings[%d] of pulsedescriptor %u the bits_for_number_of_samples is %d and the number_of_samples is %d (one should be non-zero)", i, pulsedescriptor_index, pulsesamplings[i].bits_for_number_of_samples, pulsesamplings[i].number_of_samples);
          return 1;
        }
        if ((pulsesamplings[i].bits_per_sample != 8) && (pulsesamplings[i].bits_per_sample != 16))
        {
          sprintf(pulsewaves->error, "for samplings[%d] of pulsedescriptor %u the bits_per_sample is %d (should be 8 or 16)", i, pulsedescriptor_index, pulsesamplings[i].bits_per_sample);
          return 1;
        }
        if (pulsesamplings[i].sample_units <= 0.0f)
        {
          sprintf(pulsewaves->error, "for samplings[%d] of pulsedescriptor %u the sample_units is %g (should be greater than 0)", i, pulsedescriptor_index, pulsesamplings[i].bits_per_sample);
          return 1;
        }
        // copy
        samplings[i].type = pulsesamplings[i].type;
        samplings[i].channel = pulsesamplings[i].channel;
        samplings[i].bits_for_duration_from_anchor = pulsesamplings[i].bits_for_duration_from_anchor;
        samplings[i].scale_for_duration_from_anchor = pulsesamplings[i].scale_for_duration_from_anchor;
        samplings[i].offset_for_duration_from_anchor = pulsesamplings[i].offset_for_duration_from_anchor;
        samplings[i].bits_for_number_of_segments = pulsesamplings[i].bits_for_number_of_segments;
        samplings[i].bits_for_number_of_samples = pulsesamplings[i].bits_for_number_of_samples;
        samplings[i].number_of_segments = pulsesamplings[i].number_of_segments;
        samplings[i].number_of_samples = pulsesamplings[i].number_of_samples;
        samplings[i].bits_per_sample = pulsesamplings[i].bits_per_sample;
        samplings[i].sample_units = pulsesamplings[i].sample_units; // [nanoseconds]
        strncpy(samplings[i].description, pulsesamplings[i].description, PULSEWAVES_DESCRIPTION_SIZE);
      }
    }

    if (!pulsewaves->header.add_descriptor(&composition, samplings, pulsedescriptor_index))
    {
      sprintf(pulsewaves->error, "adding pulsedescriptor %u", pulsedescriptor_index);
      return 1;
    }

    if (pulsewaves->waves_alloc <= pulsedescriptor_index)
    {
      if (pulsewaves->waves == 0)
      {
        pulsewaves->waves = (WAVESwaves**)malloc(sizeof(WAVESwaves*)*(pulsedescriptor_index+1));
      }
      else
      {
        pulsewaves->waves = (WAVESwaves**)realloc(pulsewaves->waves, sizeof(WAVESwaves*)*(pulsedescriptor_index+1));
      }

      if (pulsewaves->waves == 0)
      {
        sprintf(pulsewaves->error, "allocating memory for waves[] of pulsedescriptor %u", pulsedescriptor_index);
        return 1;
      }

      memset(&(pulsewaves->waves[pulsewaves->waves_alloc]), 0, (pulsedescriptor_index - pulsewaves->waves_alloc + 1) * sizeof(WAVESwaves*));

      pulsewaves->waves_alloc = pulsedescriptor_index+1;
    }

    if (pulsewaves->waves[pulsedescriptor_index]) delete pulsewaves->waves[pulsedescriptor_index];

    pulsewaves->waves[pulsedescriptor_index] = new WAVESwaves();

    if (!pulsewaves->waves[pulsedescriptor_index]->init(pulsewaves->header.get_descriptor(pulsedescriptor_index)))
    {
      sprintf(pulsewaves->error, "initializing waves for pulsedescriptor %u", pulsedescriptor_index);
      return 1;
    }

    if (samplings) delete [] samplings;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_add_pulsedescriptor %u", pulsedescriptor_index);
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set_geokey_entries(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_pulsekeyentry_struct*     key_entries
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (number == 0)
    {
      sprintf(pulsewaves->error, "number of key_entries is zero");
      return 1;
    }

    if (key_entries == 0)
    {
      sprintf(pulsewaves->error, "keyentries pointer is zero");
      return 1;
    }

    if (pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "cannot set geokeys after pulsewaves_writer was opened");
      return 1;
    }

    if (!pulsewaves->header.set_geokey_entries(number, (PULSEkeyentry*)key_entries))
    {
      sprintf(pulsewaves->error, "setting %u geokeyentries", number);
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_set_geokey_entries");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_geokey_entries(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_pulsekeyentry_struct**    key_entries
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (number == 0)
    {
      sprintf(pulsewaves->error, "pointer to number of keyentries is zero");
      return 1;
    }

    if (key_entries == 0)
    {
      sprintf(pulsewaves->error, "pointer to key_entries pointer is zero");
      return 1;
    }

    if (!pulsewaves->is_reading)
    {
      sprintf(pulsewaves->error, "cannot get geokeys before pulsewaves_reader was opened");
      return 1;
    }

    if (!pulsewaves->header.get_geokey_entries(number, (PULSEkeyentry**)key_entries))
    {
      sprintf(pulsewaves->error, "getting geokeyentries");
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_get_geokey_entries");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set_geodouble_params(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_F64*                      geodouble_params
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (number == 0)
    {
      sprintf(pulsewaves->error, "number of geodouble_params is zero");
      return 1;
    }

    if (geodouble_params == 0)
    {
      sprintf(pulsewaves->error, "geodouble_params pointer is zero");
      return 1;
    }

    if (pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "cannot set geodouble_params after pulsewaves_writer was opened");
      return 1;
    }

    if (!pulsewaves->header.set_geodouble_params(number, (F64*)geodouble_params))
    {
      sprintf(pulsewaves->error, "setting %u geodouble_params", number);
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_set_geodouble_params");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_geodouble_params(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_F64**                     geodouble_params
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (number == 0)
    {
      sprintf(pulsewaves->error, "pointer to number of geodouble_params is zero");
      return 1;
    }

    if (geodouble_params == 0)
    {
      sprintf(pulsewaves->error, "pointer to geodouble_params pointer is zero");
      return 1;
    }

    if (!pulsewaves->is_reading)
    {
      sprintf(pulsewaves->error, "cannot get geodouble_params before pulsewaves_reader was opened");
      return 1;
    }

    if (!pulsewaves->header.get_geodouble_params((U32*)number, (F64**)geodouble_params))
    {
      sprintf(pulsewaves->error, "getting geodouble_params");
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_get_geodouble_params");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set_geoascii_params(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_CHAR*                     geoascii_params
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (number == 0)
    {
      sprintf(pulsewaves->error, "number of geoascii_params is zero");
      return 1;
    }

    if (geoascii_params == 0)
    {
      sprintf(pulsewaves->error, "geoascii_params pointer is zero");
      return 1;
    }

    if (pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "cannot set geoascii_params after pulsewaves_writer was opened");
      return 1;
    }

    if (!pulsewaves->header.set_geoascii_params(number, (CHAR*)geoascii_params))
    {
      sprintf(pulsewaves->error, "setting %u geoascii_params", number);
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_set_geoascii_params");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_geoascii_params(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_CHAR**                    geoascii_params
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (number == 0)
    {
      sprintf(pulsewaves->error, "pointer to number of geoascii_params is zero");
      return 1;
    }

    if (geoascii_params == 0)
    {
      sprintf(pulsewaves->error, "pointer to geoascii_params pointer is zero");
      return 1;
    }

    if (!pulsewaves->is_reading)
    {
      sprintf(pulsewaves->error, "cannot get geoascii_params before pulsewaves_reader was opened");
      return 1;
    }

    if (!pulsewaves->header.get_geoascii_params((U32*)number, (CHAR**)geoascii_params))
    {
      sprintf(pulsewaves->error, "getting geoascii_params");
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_get_geoascii_params");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_vlr(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsevlr_struct*          pulsevlr
    , pulsewaves_U8*                       data
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (pulsevlr == 0)
    {
      sprintf(pulsewaves->error, "pulsevlr pointer is zero");
      return 1;
    }

    if ((pulsevlr->record_length_after_header > 0) && (data == 0))
    {
      sprintf(pulsewaves->error, "data pointer is zero but VLR has record length of %u after header", (U32)pulsevlr->record_length_after_header);
      return 1;
    }

    if (pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "cannot add vlr after pulsewaves_writer was opened");
      return 1;
    }

    if (!pulsewaves->header.add_vlr(pulsevlr->user_id, pulsevlr->record_id, pulsevlr->record_length_after_header, data))
    {
      sprintf(pulsewaves->error, "adding pulsevlr with user_id %s and record_id %d", pulsevlr->user_id, pulsevlr->record_id);
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_add_vlr");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_vlr(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsevlr_struct*          pulsevlr
    , pulsewaves_U8**                      data
    , pulsewaves_U32                       index
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (pulsevlr == 0)
    {
      sprintf(pulsewaves->error, "pulsevlr pointer is zero");
      return 1;
    }

    if (data == 0)
    {
      sprintf(pulsewaves->error, "pointer to data pointer is zero");
      return 1;
    }

    if (!pulsewaves->is_reading)
    {
      sprintf(pulsewaves->error, "cannot get vlr before pulsewaves_reader was opened");
      return 1;
    }

    if (index >= pulsewaves->reader.header.number_of_variable_length_records)
    {
      sprintf(pulsewaves->error, "adding pulsevlr with user_id %s and record_id %d", pulsevlr->user_id, pulsevlr->record_id);
      return 1;
    }
    
    strncpy(pulsevlr->user_id, pulsewaves->reader.header.vlrs[index].user_id, PULSEWAVES_USER_ID_SIZE);
    pulsevlr->record_id = pulsewaves->reader.header.vlrs[index].record_id;
    pulsevlr->reserved = pulsewaves->reader.header.vlrs[index].reserved;
    pulsevlr->record_length_after_header = pulsewaves->reader.header.vlrs[index].record_length_after_header;
    strncpy(pulsevlr->description, pulsewaves->reader.header.vlrs[index].description, PULSEWAVES_DESCRIPTION_SIZE);
    *data = pulsewaves->reader.header.vlrs[index].data;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_header_add_vlr");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_open(
    pulsewaves_POINTER                     pointer
    , const pulsewaves_CHAR*               file_name
    , pulsewaves_BOOL                      compress
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (file_name == 0)
    {
      sprintf(pulsewaves->error, "file_name pointer is zero");
      return 1;
    }

    // open the pulse file

    if (!pulsewaves->writer.open(file_name, &pulsewaves->header, compress, compress))
    {
      sprintf(pulsewaves->error, "cannot open writer for '%s'", file_name);
      return 1;
    }

    // init the pulse

    if (!pulsewaves->pulse.init(&pulsewaves->header))
    {
      sprintf(pulsewaves->error, "cannot init pulse for '%s'", file_name);
      return 1;
    }

    // open the waves file

    if (!pulsewaves->writer.open_waves())
    {
      sprintf(pulsewaves->error, "cannot open waves writer for '%s'", file_name);
      return 1;
    }

    pulsewaves->is_writing = TRUE;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_writer_open '%s'", file_name);
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_write_pulse(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulse_struct*             pulse
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (pulse == 0)
    {
      sprintf(pulsewaves->error, "pulse pointer is zero");
      return 1;
    }

    if (!pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "writing pulses before pulsewaves_writer was opened");
      return 1;
    }

    if (pulsewaves->pulse_was_written)
    {
      sprintf(pulsewaves->error, "writing pulse before waves were written");
      return 1;
    }

    pulsewaves->pulse_was_written = TRUE;

    pulsewaves->pulse.T = pulse->T;
    pulsewaves->pulse.offset = pulsewaves->writer.get_current_offset();
    pulsewaves->pulse.set_anchor_and_target(pulse->anchor, pulse->target);
    pulsewaves->pulse.first_returning_sample = pulse->first_returning_sample;
    pulsewaves->pulse.last_returning_sample = pulse->last_returning_sample;
    pulsewaves->pulse.descriptor_index = pulse->descriptor_index;
    pulsewaves->pulse.edge_of_scan_line = pulse->edge_of_scan_line;
    pulsewaves->pulse.scan_direction = pulse->scan_direction;
    pulsewaves->pulse.mirror_facet = pulse->mirror_facet;
    pulsewaves->pulse.intensity = pulse->intensity;
    pulsewaves->pulse.classification = pulse->classification;

    // write the pulse
    if (!pulsewaves->writer.write_pulse(&pulsewaves->pulse))
    {
      sprintf(pulsewaves->error, "writing pulse");
      return 1;
    }

    // only add it to the inventory if it is a proper pulse (with first and last populated)
    if (pulse->first_returning_sample != pulse->last_returning_sample)
    {
      pulsewaves->pulse.compute_anchor_and_target_and_dir();
      pulsewaves->pulse.compute_first_and_last();
      pulsewaves->writer.update_inventory(&pulsewaves->pulse);
    }
    else
    {
      pulsewaves->writer.update_inventory(&pulsewaves->pulse, TRUE);
    }

    pulsewaves->descriptor_index = pulse->descriptor_index;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_writer_write_pulse");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_write_waves(
    pulsewaves_POINTER                     pointer
    , pulsewaves_wavessampling_struct*     wavessamplings    
    , pulsewaves_U32                       number_of_wavessamplings
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (wavessamplings == 0)
    {
      sprintf(pulsewaves->error, "wavessamplings pointer is zero");
      return 1;
    }

    if (number_of_wavessamplings == 0)
    {
      sprintf(pulsewaves->error, "number_of_wavessamplings is zero");
      return 1;
    }

    if (!pulsewaves->is_writing)
    {
      sprintf(pulsewaves->error, "writing waves before pulsewaves_writer was opened");
      return 1;
    }

    if (!pulsewaves->pulse_was_written)
    {
      sprintf(pulsewaves->error, "writing waves before pulse was written");
      return 1;
    }

    pulsewaves->pulse_was_written = FALSE;

    if ((pulsewaves->descriptor_index < PULSEWAVES_DESCRIPTOR_MIN) || (PULSEWAVES_DESCRIPTOR_MAX < pulsewaves->descriptor_index))
    {
      sprintf(pulsewaves->error, "pulsedescriptor_index %u of last pulse outside of valid range from %u to %u", pulsewaves->descriptor_index, PULSEWAVES_DESCRIPTOR_MIN, PULSEWAVES_DESCRIPTOR_MAX);
      return 1;
    }

    if (pulsewaves->descriptor_index > pulsewaves->waves_alloc)
    {
      sprintf(pulsewaves->error, "pulsedescriptor_index %u of last pulse larger than largest known descriptor index %u", pulsewaves->descriptor_index, pulsewaves->waves_alloc);
      return 1;
    }

    WAVESwaves* waves = pulsewaves->waves[pulsewaves->descriptor_index];

    if (waves == 0)
    {
      sprintf(pulsewaves->error, "pulsedescriptor_index %u of last pulse references unknown descriptor");
      return 1;
    }

    if (waves->get_number_of_samplings() != number_of_wavessamplings)
    {
      sprintf(pulsewaves->error, "number of samplings should be %d not %d according to pulsedescriptor with index %d", waves->get_number_of_samplings(), number_of_wavessamplings, pulsewaves->descriptor_index);
      return 1;
    }

    for (I32 m = 0; m < number_of_wavessamplings; m++)
    {
      WAVESsampling* sampling = waves->get_sampling(m);
      if (!sampling->set_number_of_segments(wavessamplings[m].num_segments))
      {
        sprintf(pulsewaves->error, "number of segments of sampling %d should be %d not %d according to pulsedescriptor with index %d", m, sampling->get_number_of_segments(), wavessamplings[m].num_segments, pulsewaves->descriptor_index);
        return 1;
      }
      for (I32 s = 0; s < wavessamplings[m].num_segments; s++)
      {
        sampling->set_active_segment(s);
        if (!sampling->set_number_of_samples_for_segment(wavessamplings[m].num_samples[s]))
        {
          sprintf(pulsewaves->error, "number of samples of segment %d of sampling %d should be %d not %d according to pulsedescriptor with index %d", s, m, sampling->get_number_of_samples_for_segment(), wavessamplings[m].num_samples[s], pulsewaves->descriptor_index);
          return 1;
        }
        if (sampling->get_bits_for_duration_from_anchor())
        {
          if (!sampling->set_duration_from_anchor_for_segment(wavessamplings[m].durations[s]))
          {
            sprintf(pulsewaves->error, "duration %g of segment %d of sampling %d could not be set for pulsedescriptor with index %d because quantized duration (after subtracting offset_for_duration_from_anchor and dividing by scale_for_duration_from_anchor) does not fit into bits_for_duration_from_anchor", wavessamplings[m].durations[s], s, m, pulsewaves->descriptor_index);
            return 1;
          }
        }
        memcpy(sampling->get_samples(), wavessamplings[m].samples[s], sampling->size());
      }
    }

    // write the waves
    if (!pulsewaves->writer.write_waves(waves))
    {
      sprintf(pulsewaves->error, "writing waves");
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_writer_write_pulse");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_close(
    pulsewaves_POINTER                     pointer
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (!pulsewaves->writer.update_header(&pulsewaves->header, TRUE))
    {
      sprintf(pulsewaves->error, "updating header during close of writer");
      return 1;
    }

    pulsewaves->writer.close();

    pulsewaves->is_writing = FALSE;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_writer_close");
    return 1;
  }
   
  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_open(
    pulsewaves_POINTER                     pointer
    , const pulsewaves_CHAR*               file_name
    , pulsewaves_BOOL*                     is_compressed
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    if (file_name == 0)
    {
      sprintf(pulsewaves->error, "file_name pointer is zero");
      return 1;
    }

    // open the file
    if (!pulsewaves->reader.open(file_name))
    {
      sprintf(pulsewaves->error, "reader cannot open file '%s'", file_name);
      return 1;
    }

    if (pulsewaves->reader.header.pulsezip)
    {
      *is_compressed = 1;
    }
    else
    {
      *is_compressed = 0;
    }

    pulsewaves->is_reading = TRUE;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_reader_open");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_seek_pulse(
    pulsewaves_POINTER                     pointer
    , pulsewaves_I64                       index
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    // seek to the pulse
    if (!pulsewaves->reader.seek(index))
    {
#ifdef _WIN32
      sprintf(pulsewaves->error, "reader cannot seek to index %I64d for file with %I64d pulses", index, pulsewaves->reader.npulses);
#else
      sprintf(pulsewaves->error, "reader cannot seek to index %lld for file with %lld pulses", index, pulsewaves->reader.npulses);
#endif
      return 1;
    }
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_reader_seek_pulse");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_read_pulse(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulse_struct*             pulse
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    // read the pulse
    if (!pulsewaves->reader.read_pulse())
    {
#ifdef _WIN32
      sprintf(pulsewaves->error, "reader cannot read pulse %I64d of %I64d pulses", pulsewaves->reader.p_count, pulsewaves->reader.npulses);
#else
      sprintf(pulsewaves->error, "reader cannot read pulse %lld of %lld pulses", pulsewaves->reader.p_count, pulsewaves->reader.npulses);
#endif
      return 1;
    }

    pulse->T = pulsewaves->reader.pulse.T;
    pulsewaves->reader.pulse.get_anchor(pulse->anchor);
    pulsewaves->reader.pulse.get_target(pulse->target);
    pulse->first_returning_sample = pulsewaves->reader.pulse.first_returning_sample;
    pulse->last_returning_sample = pulsewaves->reader.pulse.last_returning_sample;
    pulse->descriptor_index = pulsewaves->reader.pulse.descriptor_index;
    pulse->edge_of_scan_line = pulsewaves->reader.pulse.edge_of_scan_line;
    pulse->scan_direction = pulsewaves->reader.pulse.scan_direction;
    pulse->mirror_facet = pulsewaves->reader.pulse.mirror_facet;
    pulse->intensity = pulsewaves->reader.pulse.intensity;
    pulse->classification = pulsewaves->reader.pulse.classification;
    pulse->pulse_source_ID = pulsewaves->reader.pulse.pulse_source_ID;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_reader_read_pulse");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_read_waves(
    pulsewaves_POINTER                     pointer
    , pulsewaves_wavessampling_struct**    wavessamplings    
    , pulsewaves_U32*                      number_of_wavessamplings
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    // read the waves
    if (!pulsewaves->reader.read_waves())
    {
#ifdef _WIN32
      sprintf(pulsewaves->error, "reader cannot read waves for pulse %I64d of %I64d", pulsewaves->reader.p_count, pulsewaves->reader.npulses);
#else
      sprintf(pulsewaves->error, "reader cannot read waves for pulse %lld of %lld", pulsewaves->reader.p_count, pulsewaves->reader.npulses);
#endif
      return 1;
    }

    // get waves
    WAVESwaves* waves = pulsewaves->reader.waves;

    // create sufficiently large array for samplings
    I32 wavessamplings_array_size = waves->get_number_of_samplings();

    // allocate number of samplings
    if (wavessamplings_array_size > pulsewaves->wavessamplings_array_alloc)
    {
      if (pulsewaves->wavessamplings_array_alloc)
      {
        pulsewaves->wavessamplings_array = (pulsewaves_wavessampling*)realloc(pulsewaves->wavessamplings_array, sizeof(pulsewaves_wavessampling)*wavessamplings_array_size);
        if (pulsewaves->wavessamplings_array == 0)
        {
          sprintf(pulsewaves->error, "reallocating %d wavessamplings", wavessamplings_array_size);
          return 1;
        }
      }
      else
      {
        pulsewaves->wavessamplings_array = (pulsewaves_wavessampling*)malloc(sizeof(pulsewaves_wavessampling)*wavessamplings_array_size);
        if (pulsewaves->wavessamplings_array == 0)
        {
          sprintf(pulsewaves->error, "allocating %d wavessamplings", wavessamplings_array_size);
          return 1;
        }
      }
      pulsewaves->wavessamplings_array_alloc = wavessamplings_array_size;
    }

    WAVESsampling* sampling;
    I32 helper;

    // first loop over waves to compute array size
    I32 m,s;
    I32 segments_array_size = 0;
    I32 samples_array_size = 0;

    for (m = 0; m < wavessamplings_array_size; m++)
    {
      sampling = waves->get_sampling(m);
      helper = sampling->get_number_of_segments();
      segments_array_size += helper;
      for (s = 0; s < helper; s++)
      {
        sampling->set_active_segment(s);
        samples_array_size += sampling->get_number_of_samples_for_segment() * sampling->get_bytes_per_sample();
      }
    }

    // create sufficiently large arrays
    if (segments_array_size > pulsewaves->segments_array_alloc)
    {
      if (pulsewaves->segments_array_alloc)
      {
        pulsewaves->segments_array_num_samples = (I32*)realloc(pulsewaves->segments_array_num_samples, sizeof(I32)*segments_array_size);
        if (pulsewaves->segments_array_num_samples == 0)
        {
          sprintf(pulsewaves->error, "reallocating %d segments_array_num_samples", segments_array_size);
          return 1;
        }
        pulsewaves->segments_array_durations = (F32*)realloc(pulsewaves->segments_array_durations, sizeof(F32)*segments_array_size);
        if (pulsewaves->segments_array_durations == 0)
        {
          sprintf(pulsewaves->error, "reallocating %d segments_array_durations", segments_array_size);
          return 1;
        }
        pulsewaves->segments_samples_array = (U8**)realloc(pulsewaves->segments_samples_array, sizeof(U8*)*segments_array_size);
        if (pulsewaves->segments_samples_array == 0)
        {
          sprintf(pulsewaves->error, "reallocating %d segments_samples_array pointers", segments_array_size);
          return 1;
        }
      }
      else
      {
        pulsewaves->segments_array_num_samples = (I32*)malloc(sizeof(I32)*segments_array_size);
        if (pulsewaves->segments_array_num_samples == 0)
        {
          sprintf(pulsewaves->error, "allocating %d segments_array_num_samples", segments_array_size);
          return 1;
        }
        pulsewaves->segments_array_durations = (F32*)malloc(sizeof(F32)*segments_array_size);
        if (pulsewaves->segments_array_durations == 0)
        {
          sprintf(pulsewaves->error, "allocating %d segments_array_durations", segments_array_size);
          return 1;
        }
        pulsewaves->segments_samples_array = (U8**)malloc(sizeof(U8*)*segments_array_size);
        if (pulsewaves->segments_samples_array == 0)
        {
          sprintf(pulsewaves->error, "allocating %d segments_samples_array pointers", segments_array_size);
          return 1;
        }
      }
      pulsewaves->segments_array_alloc = segments_array_size;
    }
    if (samples_array_size > pulsewaves->samples_array_alloc)
    {
      if (pulsewaves->samples_array_alloc)
      {
        pulsewaves->samples_array = (U8*)realloc(pulsewaves->samples_array, sizeof(U8)*samples_array_size);
        if (pulsewaves->samples_array == 0)
        {
          sprintf(pulsewaves->error, "reallocating %d samples_array pointers", samples_array_size);
          return 1;
        }
      }
      else
      {
        pulsewaves->samples_array = (U8*)malloc(sizeof(U8)*samples_array_size);
        if (pulsewaves->samples_array == 0)
        {
          sprintf(pulsewaves->error, "allocating %d samples_array pointers", samples_array_size);
          return 1;
        }
      }
      pulsewaves->samples_array_alloc = samples_array_size;
    }

    // second loop over waves to fill in arrays
    segments_array_size = 0;
    samples_array_size = 0;
    for (m = 0; m < wavessamplings_array_size; m++)
    {
      sampling = waves->get_sampling(m);
      pulsewaves->wavessamplings_array[m].num_segments = sampling->get_number_of_segments();
      pulsewaves->wavessamplings_array[m].num_samples = &(pulsewaves->segments_array_num_samples[segments_array_size]);
      pulsewaves->wavessamplings_array[m].durations = &(pulsewaves->segments_array_durations[segments_array_size]);
      pulsewaves->wavessamplings_array[m].samples = &(pulsewaves->segments_samples_array[segments_array_size]);
      segments_array_size += pulsewaves->wavessamplings_array[m].num_segments;
      for (s = 0; s < pulsewaves->wavessamplings_array[m].num_segments; s++)
      {
        sampling->set_active_segment(s);
        pulsewaves->wavessamplings_array[m].num_samples[s] = sampling->get_number_of_samples_for_segment();
        if (sampling->get_bits_for_duration_from_anchor())
          pulsewaves->wavessamplings_array[m].durations[s] = sampling->get_duration_from_anchor_for_segment();
        else
          pulsewaves->wavessamplings_array[m].durations[s] = 0.0f;
        pulsewaves->wavessamplings_array[m].samples[s] = &(pulsewaves->samples_array[samples_array_size]);
        helper = pulsewaves->wavessamplings_array[m].num_samples[s] * sampling->get_bytes_per_sample();
        memcpy(pulsewaves->wavessamplings_array[m].samples[s], sampling->get_samples(), helper);
        samples_array_size += helper;
      }
    }

    // populate output
    *wavessamplings = pulsewaves->wavessamplings_array;
    *number_of_wavessamplings = wavessamplings_array_size;
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_reader_read_waves");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}

/*---------------------------------------------------------------------------*/
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_close(
    pulsewaves_POINTER                     pointer
)
{
  if (pointer == 0) return 1;
  pulsewaves_struct* pulsewaves = (pulsewaves_struct*)pointer;

  try
  {
    pulsewaves->reader.close();
  }
  catch (...)
  {
    sprintf(pulsewaves->error, "internal error in pulsewaves_reader_close");
    return 1;
  }

  pulsewaves->error[0] = '\0';
  return 0;
}
