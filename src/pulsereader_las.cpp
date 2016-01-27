/*
===============================================================================

  FILE:  pulsereader_las.cpp
  
  CONTENTS:
  
    see corresponding header file
  
  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2007-2014, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    see corresponding header file
  
===============================================================================
*/
#include "pulsereader_las.hpp"

#ifdef HAVE_LASLIB

#include "bytestreamin.hpp"
#include "bytestreamin_file.hpp"
#include "bytestreamin_istream.hpp"

#define LASLIB_DLL
#include "lasreader.hpp"
#include "laswaveform13reader.hpp"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>

BOOL PULSEreaderLAS::open(const char* file_name, U32 io_buffer_size)
{
  if (file_name == 0)
  {
    fprintf(stderr,"ERROR: file name pointer is zero\n");
    return FALSE;
  }

  // close an existing lasreader

  if (lasreader)
  {
    lasreader->close();
    delete lasreader;
    lasreader = 0;
  }

  // open the lasreader

  LASreadOpener lasreadopener;
  lasreader = lasreadopener.open(file_name);
  if (lasreader == 0)
  {
    fprintf(stderr,"ERROR: cannot open file '%s'\n", file_name);
    return FALSE;
  }

  // make sure the LAS file has pulse data

  if (lasreader->header.point_data_format != 4 && lasreader->header.point_data_format != 5 && lasreader->header.point_data_format != 9 && lasreader->header.point_data_format != 10)
  {
    fprintf(stderr,"ERROR: file '%s' contains points of type %d without waveforms\n", file_name, lasreader->header.point_data_format);
    return FALSE;
  }
  last_gps_time = 0.0;

  // check if the LAS file also has waves data

  laswaveform13reader = lasreadopener.open_waveform13(&lasreader->header);
  if (laswaveform13reader == 0)
  {
    fprintf(stderr,"WARNING: cannot open the waves of file '%s'.\n", file_name);
  }

  return open();
}

BOOL PULSEreaderLAS::open()
{
  I32 i;

  // clean the header

  header.clean();

  // prepare the header 

  header.project_ID_GUID_data_1 = lasreader->header.project_ID_GUID_data_1;
  header.project_ID_GUID_data_2 = lasreader->header.project_ID_GUID_data_2;
  header.project_ID_GUID_data_3 = lasreader->header.project_ID_GUID_data_3;
  memcpy(header.project_ID_GUID_data_4, lasreader->header.project_ID_GUID_data_4, 8);

  memset(header.system_identifier, 0, PULSEWAVES_DESCRIPTION_SIZE);
  memset(header.generating_software, 0, PULSEWAVES_DESCRIPTION_SIZE);
  sprintf(header.system_identifier, "created by PULSEreaderLAS");
  sprintf(header.generating_software, "PulseWaves %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);

  header.file_creation_day = lasreader->header.file_creation_day;
  header.file_creation_year = lasreader->header.file_creation_year;

  header.number_of_pulses = lasreader->header.number_of_point_records;

  if (lasreader->header.global_encoding & 1)
  {
    header.t_offset = 1000000000;
  }
  else
  {
    header.t_offset = 0;
  }

  header.x_scale_factor = lasreader->header.x_scale_factor;
  header.y_scale_factor = lasreader->header.y_scale_factor;
  header.z_scale_factor = lasreader->header.z_scale_factor;

  header.x_offset = lasreader->header.x_offset;
  header.y_offset = lasreader->header.y_offset;
  header.z_offset = lasreader->header.z_offset;

  header.max_x = lasreader->header.max_x;
  header.min_x = lasreader->header.min_x;
  header.max_y = lasreader->header.max_y;
  header.min_y = lasreader->header.min_y;
  header.max_z = lasreader->header.max_z;
  header.min_z = lasreader->header.min_z;

  npulses = header.number_of_pulses;
  p_count = 0;

  // maybe copy the projection VLRs into the header

  if (lasreader->header.number_of_variable_length_records)
  {
    for (i = 0; i < lasreader->header.number_of_variable_length_records; i++)
    {
      if (strcmp(lasreader->header.vlrs[i].user_id, "LASF_Projection") == 0)
      {
        if (lasreader->header.vlrs[i].record_id == 34735 || lasreader->header.vlrs[i].record_id == 34736 || lasreader->header.vlrs[i].record_id == 34737) // GeoKeyDirectoryTag || GeoDoubleParamsTag || GeoAsciiParamsTag
        {
          U32 record_length_after_header = lasreader->header.vlrs[i].record_length_after_header;
          if (record_length_after_header)
          {
            U8* data = new U8[record_length_after_header];
            memcpy(data, lasreader->header.vlrs[i].data, record_length_after_header);
            header.add_vlr("PulseWaves_Proj", lasreader->header.vlrs[i].record_id, record_length_after_header, data);
            if (lasreader->header.vlrs[i].record_id == 34735)
            {
              // expose geokeys contents in header
              if (header.geokeys)
              {
                fprintf(stderr,"WARNING: variable length records contain more than one GeoKeyDirectoryTag\n");
              }
              header.geokeys = (PULSEgeokeys*)data;
              // check variable header geo keys contents
              if (header.geokeys->key_directory_version != 1)
              {
                fprintf(stderr,"WARNING: wrong geokeys->key_directory_version: %d != 1\n",header.geokeys->key_directory_version);
              }
              if (header.geokeys->key_revision != 1)
              {
                fprintf(stderr,"WARNING: wrong geokeys->key_revision: %d != 1\n",header.geokeys->key_revision);
              }
              if (header.geokeys->minor_revision != 0)
              {
                fprintf(stderr,"WARNING: wrong geokeys->minor_revision: %d != 0\n",header.geokeys->minor_revision);
              }
              // expose geokey_entries contents in header
              header.geokey_entries = (PULSEkeyentry*)&header.geokeys[1];
            }
            else if (lasreader->header.vlrs[i].record_id == 34736) // GeoDoubleParamsTag
            {
              if (header.geodouble_params)
              {
                fprintf(stderr,"WARNING: variable length records contain more than one GeoF64ParamsTag\n");
              }
              // expose geodouble_params contents in header
              header.geodouble_params = (F64*)data;
            }
            else if (lasreader->header.vlrs[i].record_id == 34737) // GeoAsciiParamsTag
            {
              if (header.geoascii_params)
              {
                fprintf(stderr,"WARNING: variable length records contain more than one GeoAsciiParamsTag\n");
              }
              // expose geoascii_params contents in header
              header.geoascii_params = (CHAR*)data;
            }
          }
        }
      }
    }
  }
  
  if (lasreader->header.vlr_wave_packet_descr)
  {
    for (i = 1; i < 255; i++)
    {
      if (lasreader->header.vlr_wave_packet_descr[i])
      {
        // create descriptor (composition plus sampling)

        PULSEcomposition composition;
        PULSEsampling sampling;

        composition.optical_center_to_anchor_point = PULSEWAVES_OPTICAL_CENTER_AND_ANCHOR_POINT_FLUCTUATE;          // the duration between optical center and anchor point is not constant
        composition.number_of_extra_waves_bytes = 0;     
        composition.number_of_samplings = 1;                                                                        // one one returning
        composition.sample_units = (F32)(0.001 * lasreader->header.vlr_wave_packet_descr[i]->getTemporalSpacing()); // [nanoseconds]
        composition.scanner_index = PULSEWAVES_UNDEFINED;
        strncpy(composition.description, "converted from ASPRS LAS 1.3 (with waveform)", PULSEWAVES_DESCRIPTION_SIZE);

        sampling.type = PULSEWAVES_RETURNING;
        sampling.channel = 0;
        sampling.bits_for_duration_from_anchor = 0;            // the returning waveform starts at the anchor
        sampling.bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
        sampling.bits_for_number_of_samples = 0;               // the number of samples per segment is fixed across all segments
        sampling.number_of_segments = 1;                       // the number of segments per sampling is always 1
        sampling.number_of_samples = lasreader->header.vlr_wave_packet_descr[i]->getNumberOfSamples(); 
        sampling.bits_per_sample = lasreader->header.vlr_wave_packet_descr[i]->getBitsPerSample();
        sampling.lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
        sampling.sample_units = composition.sample_units;      // [nanoseconds]
        sampling.compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
        sprintf(sampling.description, "%d samples at %d bits", sampling.number_of_samples, sampling.bits_per_sample);

        header.add_descriptor(&composition, &sampling, i, TRUE);
      }
    }
  }

  pulse.init(&header);

  header_is_populated = FALSE;

  return TRUE;
}

I32 PULSEreaderLAS::get_format() const
{
  return PULSEWAVES_FORMAT_LAS;
}

BOOL PULSEreaderLAS::seek(const I64 p_index)
{
  if (p_index < npulses)
  {
    if (seek_map == 0)
    {
      if (lasreader->seek(p_index))
      {
        p_count = p_index;
        return TRUE;
      }
    }
    else
    {
      if (lasreader->seek((I64)seek_map[p_index]))
      {
        p_count = p_index;
        return TRUE;
      }
    }
  }
  return FALSE;
}

BOOL PULSEreaderLAS::read_pulse_default()
{
  if (p_count < npulses)
  {
    while (true)
    {
      if (lasreader->read_point() == FALSE)
      {
        fprintf(stderr,"WARNING: end-of-file after %u of %u pulses\n", (U32)p_count, (U32)npulses);
        return FALSE;
      }
      if (last_gps_time != lasreader->point.gps_time)
      {
        last_gps_time = lasreader->point.gps_time;
        I32 index = lasreader->point.wavepacket.getIndex();
        if (index)
        {
          if (lasreader->header.vlr_wave_packet_descr[index])
          {
            I64 T = I64_QUANTIZE(lasreader->point.gps_time*1000000);
            pulse.set_T(T);
            pulse.offset = lasreader->point.wavepacket.getOffset();
            pulse.descriptor_index = index;
            pulse.edge_of_scan_line = lasreader->point.edge_of_flight_line;
            pulse.scan_direction = lasreader->point.scan_direction_flag;

            // construct the pulse

            I32 steps = lasreader->header.vlr_wave_packet_descr[index]->getNumberOfSamples() - 1;
            I32 temporal = 1000 * lasreader->header.vlr_wave_packet_descr[index]->getTemporalSpacing();
            F64 location = lasreader->point.wavepacket.getLocation();
        
            F64 target[3];
            target[0] = lasreader->point.wavepacket.getXt();
            target[1] = lasreader->point.wavepacket.getYt();
            target[2] = lasreader->point.wavepacket.getZt();

            F64 anchor[3];
            anchor[0] = lasreader->point.get_x() + location*target[0];
            anchor[1] = lasreader->point.get_y() + location*target[1];
            anchor[2] = lasreader->point.get_z() + location*target[2];

            target[0] = anchor[0] - target[0] * temporal;
            target[1] = anchor[1] - target[1] * temporal;
            target[2] = anchor[2] - target[2] * temporal;

            pulse.set_anchor_and_target(anchor, target);
            pulse.first_returning_sample = 0;
            pulse.last_returning_sample = steps;

            break;
          }
        }
      }
    }
    p_count++;
    if (p_count < lasreader->p_count)
    {
      if (seek_map == 0)
      {
        seek_map = (U32*)malloc(100000*sizeof(U32));
      }
      else if ((p_count % 100000) == 0)
      {
        seek_map = (U32*)realloc(seek_map,100000*sizeof(U32)*((p_count+100000)/100000));
      }
      seek_map[p_count-1] = (U32)lasreader->p_count-1;
    }
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderLAS::read_waves()
{
  if (laswaveform13reader == 0)
  {
    return FALSE;
  }
  if (!laswaveform13reader->read_waveform(&lasreader->point))
  {
    return FALSE;
  }
  if (waves_map[pulse.descriptor_index] == 0)
  {
    waves_map[pulse.descriptor_index] = new WAVESwaves();
    if (waves_map[pulse.descriptor_index] == 0)
    {
      fprintf(stderr,"ERROR: cannot allocate WAVESwaves for pulsedescriptor %d\n", pulse.descriptor_index);
      return FALSE;
    }
    if (!waves_map[pulse.descriptor_index]->init(header.get_descriptor(pulse.descriptor_index)))
    {
      fprintf(stderr,"ERROR: cannot init WAVESwaves for pulsedescriptor %d\n", pulse.descriptor_index);
      return FALSE;
    }
  }
  waves = waves_map[pulse.descriptor_index];
  memcpy(waves->get_sampling(0)->get_samples(), laswaveform13reader->samples, waves->get_sampling(0)->size()); 
/*
      F64 first[3];
      laswaveform13reader->has_samples_xyz();
      first[0] = laswaveform13reader->XYZsample[0];
      first[1] = laswaveform13reader->XYZsample[1];
      first[2] = laswaveform13reader->XYZsample[2];
      pulse.set_anchor(first);
      while (laswaveform13reader->has_samples_xyz())
      {
      }
      I32 steps = laswaveform13reader->nsamples - 1;
      pulse.dx = (laswaveform13reader->XYZsample[0] - first[0]) / steps;
      pulse.dy = (laswaveform13reader->XYZsample[1] - first[1]) / steps;
      pulse.dz = (laswaveform13reader->XYZsample[2] - first[2]) / steps;
      pulse.first_returning_sample = 0;
      pulse.last_returning_sample = steps;
*/
  return TRUE;
}

ByteStreamIn* PULSEreaderLAS::get_pulse_stream() const
{
  return (lasreader ? lasreader->get_stream() : 0);
}

ByteStreamIn* PULSEreaderLAS::get_waves_stream() const
{
//  return (laswaveform13reader ? laswaveform13reader->get_stream() : 0);
  return 0;
}

void PULSEreaderLAS::close(BOOL close_stream)
{
  if (lasreader) 
  {
    lasreader->close(close_stream);
    delete lasreader;
    lasreader = 0;
  }
  if (laswaveform13reader)
  {
    laswaveform13reader->close();
    delete laswaveform13reader;
    laswaveform13reader = 0;
  }
  for (I32 i = 0; i < 256; i++)
  {
    if (waves_map[i])
    {
      delete waves_map[i];
      waves_map[i] = 0;
    }
  }
}

PULSEreaderLAS::PULSEreaderLAS()
{
  lasreader = 0;
  laswaveform13reader = 0;
  last_gps_time = 0.0;
  memset(waves_map, 0, sizeof(WAVESwaves*)*256);
  seek_map = 0;
}

PULSEreaderLAS::~PULSEreaderLAS()
{
  if (lasreader) close();
  if (seek_map) free(seek_map);
}

#endif // HAVE_LASLIB