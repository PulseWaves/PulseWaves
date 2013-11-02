/*
===============================================================================

  FILE:  pulsereader_gcw.cpp
  
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
#include "pulsereader_gcw.hpp"

#include "pulseutility.hpp"
#include "bytestreamin.hpp"
#include "bytestreamin_file.hpp"
#include "bytestreamin_istream.hpp"

#define GCW_PULSE_DESCRIPTOR_INDEX_8_BIT  1
#define GCW_PULSE_DESCRIPTOR_INDEX_16_BIT 2

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>

static BOOL valid_lonlat(F64 lon, F64 lat)
{
  if (lon < -360.0 || lon > 360.0 || lat < -180.0 || lat > 180.0)
  {
    return FALSE;
  }
  return TRUE;
};

BOOL PULSEreaderGCW::read_pulse_lgc()
{
  try { pulse_stream->get64bitsLE((U8*)&pulselgc.wfi); } catch(...)
  {
    fprintf(stderr,"ERROR: reading wfi\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&pulselgc.t); } catch(...)
  {
    fprintf(stderr,"ERROR: reading t\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&pulselgc.e0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading e0\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&pulselgc.n0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading n0\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&pulselgc.h0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading h0\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&pulselgc.de); } catch(...)
  {
    fprintf(stderr,"ERROR: reading de\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&pulselgc.dn); } catch(...)
  {
    fprintf(stderr,"ERROR: reading dn\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&pulselgc.dh); } catch(...)
  {
    fprintf(stderr,"ERROR: reading dh\n");
    return FALSE;
  }
  try { pulse_stream->get16bitsLE((U8*)&pulselgc.wfoffset); } catch(...)
  {
    fprintf(stderr,"ERROR: reading wfoffset\n");
    return FALSE;
  }
  try { pulse_stream->get16bitsLE((U8*)&pulselgc.wflen); } catch(...)
  {
    fprintf(stderr,"ERROR: reading wflen\n");
    return FALSE;
  }
  try { pulse_stream->get16bitsLE((U8*)&pulselgc.strtwflen); } catch(...)
  {
    fprintf(stderr,"ERROR: reading strtwflen\n");
    return FALSE;
  }
  try { pulselgc.sampdepth = pulse_stream->getByte(); } catch(...)
  {
    fprintf(stderr,"ERROR: reading sampdepth\n");
    return FALSE;
  }
  try { pulselgc.res = pulse_stream->getByte(); } catch(...)
  {
    fprintf(stderr,"ERROR: reading res\n");
    return FALSE;
  }
  return TRUE;
};

BOOL PULSEreaderGCW::open(const CHAR* file_name, U32 io_buffer_size)
{
  if (file_name == 0)
  {
    fprintf(stderr,"ERROR: file name pointer is zero\n");
    return FALSE;
  }

  if (pulse_file)
  {
    fclose(pulse_file);
  }

  pulse_file = fopen(file_name, "rb");
  if (pulse_file == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", file_name);
    return FALSE;
  }

  if (setvbuf(pulse_file, NULL, _IOFBF, io_buffer_size) != 0)
  {
    fprintf(stderr, "WARNING: setvbuf() failed with buffer size %u\n", io_buffer_size);
  }

  if (pulse_stream)
  {
    delete pulse_stream;
  }

  if (IS_LITTLE_ENDIAN())
    pulse_stream = new ByteStreamInFileLE(pulse_file);
  else
    pulse_stream = new ByteStreamInFileBE(pulse_file);

  if (pulse_stream == 0)
  {
    fprintf(stderr,"ERROR: pulse_stream is zero\n");
    return FALSE;
  }

  if (this->file_name) free(this->file_name);
  this->file_name = strdup(file_name);

  return open();
}

BOOL PULSEreaderGCW::open()
{
  F64 xyz;

  // clean the header

  header.clean();
 
  // set some parameters

  memset(header.system_identifier, 0, PULSEWAVES_DESCRIPTION_SIZE);
  memset(header.generating_software, 0, PULSEWAVES_DESCRIPTION_SIZE);
  sprintf(header.system_identifier, "created by PULSEreaderGCW");
  sprintf(header.generating_software, "PulseWaves %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);

  header.file_creation_day = 333;
  header.file_creation_year = 2012;

  // read the first pulse 

  if (!read_pulse_lgc())
  {
    fprintf(stderr,"ERROR: reading first pulse\n");
    return FALSE;
  }
  
  // use first pulse to determine some settings

  BOOL long_lat_coordinates = valid_lonlat(pulselgc.e0, pulselgc.n0);

  if (long_lat_coordinates)
  {
    header.x_scale_factor = 1e-7;
    header.y_scale_factor = 1e-7;
    header.x_offset = I32_QUANTIZE(pulselgc.e0/10)*10;
    header.y_offset = I32_QUANTIZE(pulselgc.n0/10)*10;
  }
  else
  {
    header.x_scale_factor = 0.01;
    header.y_scale_factor = 0.01;
    header.x_offset = I32_QUANTIZE(pulselgc.e0/100000)*100000;
    header.y_offset = I32_QUANTIZE(pulselgc.n0/100000)*100000;
  }

  header.z_scale_factor = 0.01;
  header.z_offset = 0;

  pulse.last_returning_sample = pulselgc.wfoffset + pulselgc.wflen - 1;
  header.min_x = header.max_x = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset);
  header.min_y = header.max_y = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset);
  header.min_z = header.max_z = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset);

  xyz = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_x > xyz) header.min_x = xyz;
  else if (header.max_x < xyz) header.max_x = xyz;

  xyz = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_y > xyz) header.min_y = xyz;
  else if (header.max_y < xyz) header.max_y = xyz;

  xyz = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_z > xyz) header.min_z = xyz;
  else if (header.max_z < xyz) header.max_z = xyz;

  // seek to the last pulse in the file

  try { pulse_stream->seekEnd(56); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek to end of file\n");
    return FALSE;
  }

  I64 file_size = pulse_stream->tell();

  if ( (file_size % 56) != 0 )
  {
    fprintf(stderr,"WARNING: odd file size. with data records of %d bytes there is a remainder of %d bytes\n", 56, (I32)(file_size % 56));
  }

  header.number_of_pulses = npulses = (file_size / 56) + 1;
  p_count = 0;

  // use last pulse to update bounding box approximation

  if (!read_pulse_lgc())
  {
    fprintf(stderr,"ERROR: reading last pulse\n");
    return FALSE;
  }

  xyz = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset);
  if (header.min_x > xyz) header.min_x = xyz;
  else if (header.max_x < xyz) header.max_x = xyz;

  xyz = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset);
  if (header.min_y > xyz) header.min_y = xyz;
  else if (header.max_y < xyz) header.max_y = xyz;

  xyz = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset);
  if (header.min_z > xyz) header.min_z = xyz;
  else if (header.max_z < xyz) header.max_z = xyz;

  xyz = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_x > xyz) header.min_x = xyz;
  else if (header.max_x < xyz) header.max_x = xyz;

  xyz = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_y > xyz) header.min_y = xyz;
  else if (header.max_y < xyz) header.max_y = xyz;

  xyz = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_z > xyz) header.min_z = xyz;
  else if (header.max_z < xyz) header.max_z = xyz;

  // load a few more pulses to update bounding box approximation

  try { pulse_stream->seek(npulses/4*1*56); } catch(...)
  {
    fprintf(stderr,"ERROR: seeking to intermediate pulse\n");
    return FALSE;
  }

  if (!read_pulse_lgc())
  {
    fprintf(stderr,"ERROR: reading intermediate pulse\n");
    return FALSE;
  }

  xyz = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset);
  if (header.min_x > xyz) header.min_x = xyz;
  else if (header.max_x < xyz) header.max_x = xyz;

  xyz = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset);
  if (header.min_y > xyz) header.min_y = xyz;
  else if (header.max_y < xyz) header.max_y = xyz;

  xyz = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset);
  if (header.min_z > xyz) header.min_z = xyz;
  else if (header.max_z < xyz) header.max_z = xyz;

  xyz = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_x > xyz) header.min_x = xyz;
  else if (header.max_x < xyz) header.max_x = xyz;

  xyz = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_y > xyz) header.min_y = xyz;
  else if (header.max_y < xyz) header.max_y = xyz;

  xyz = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_z > xyz) header.min_z = xyz;
  else if (header.max_z < xyz) header.max_z = xyz;

  try { pulse_stream->seek(npulses/4*2*56); } catch(...)
  {
    fprintf(stderr,"ERROR: seeking to intermediate pulse\n");
    return FALSE;
  }

  if (!read_pulse_lgc())
  {
    fprintf(stderr,"ERROR: reading intermediate pulse\n");
    return FALSE;
  }

  xyz = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset);
  if (header.min_x > xyz) header.min_x = xyz;
  else if (header.max_x < xyz) header.max_x = xyz;

  xyz = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset);
  if (header.min_y > xyz) header.min_y = xyz;
  else if (header.max_y < xyz) header.max_y = xyz;

  xyz = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset);
  if (header.min_z > xyz) header.min_z = xyz;
  else if (header.max_z < xyz) header.max_z = xyz;

  xyz = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_x > xyz) header.min_x = xyz;
  else if (header.max_x < xyz) header.max_x = xyz;

  xyz = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_y > xyz) header.min_y = xyz;
  else if (header.max_y < xyz) header.max_y = xyz;

  xyz = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_z > xyz) header.min_z = xyz;
  else if (header.max_z < xyz) header.max_z = xyz;

  try { pulse_stream->seek(npulses/4*3*56); } catch(...)
  {
    fprintf(stderr,"ERROR: seeking to intermediate pulse\n");
    return FALSE;
  }

  if (!read_pulse_lgc())
  {
    fprintf(stderr,"ERROR: reading intermediate pulse\n");
    return FALSE;
  }

  xyz = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset);
  if (header.min_x > xyz) header.min_x = xyz;
  else if (header.max_x < xyz) header.max_x = xyz;

  xyz = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset);
  if (header.min_y > xyz) header.min_y = xyz;
  else if (header.max_y < xyz) header.max_y = xyz;

  xyz = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset);
  if (header.min_z > xyz) header.min_z = xyz;
  else if (header.max_z < xyz) header.max_z = xyz;

  xyz = pulselgc.e0 + ((F64)pulselgc.de)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_x > xyz) header.min_x = xyz;
  else if (header.max_x < xyz) header.max_x = xyz;

  xyz = pulselgc.n0 + ((F64)pulselgc.dn)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_y > xyz) header.min_y = xyz;
  else if (header.max_y < xyz) header.max_y = xyz;

  xyz = pulselgc.h0 + ((F64)pulselgc.dh)*(pulselgc.wfoffset + pulselgc.wflen - 1);
  if (header.min_z > xyz) header.min_z = xyz;
  else if (header.max_z < xyz) header.max_z = xyz;

  // go back to the beginning of the stream

  try { pulse_stream->seek(0); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek back to beginning\n");
    return FALSE;
  }

  if (long_lat_coordinates)
  {
    // create Projection

    PULSEkeyentry key_entries[4];

    // projected coordinates
    key_entries[0].key_id = 1024; // GTModelTypeGeoKey
    key_entries[0].tiff_tag_location = 0;
    key_entries[0].count = 1;
    key_entries[0].value_offset = 2; // ModelTypeGeographic

    // ellipsoid used with latitude/longitude coordinates
    key_entries[1].key_id = 2048; // GeographicTypeGeoKey
    key_entries[1].tiff_tag_location = 0;
    key_entries[1].count = 1;
    key_entries[1].value_offset = 4326; // WGS84

    // vertical units
    key_entries[2].key_id = 4099; // VerticalUnitsGeoKey
    key_entries[2].tiff_tag_location = 0;
    key_entries[2].count = 1;
    key_entries[2].value_offset = 9001;

    // vertical datum
    key_entries[3].key_id = 4096; // VerticalCSTypeGeoKey
    key_entries[3].tiff_tag_location = 0;
    key_entries[3].count = 1;
    key_entries[3].value_offset = 5030; // WGS84

    header.set_geokey_entries(4, key_entries);
  }

  // create scanner

  PULSEscanner scanner;
  scanner.wave_length = 1064;                 // [nanometer]
  scanner.outgoing_pulse_width = 10;          // [nanoseconds]
  scanner.beam_diameter_at_exit_aperture = 0; // [millimeters]
  scanner.beam_divergence = 0;                // [milliradians]

  header.add_scanner(&scanner, 1);

  // create pulse descriptors (composition + samplings)

  PULSEcomposition composition;
  PULSEsampling samplings[2];

  composition.optical_center_to_anchor_point = PULSEWAVES_OPTICAL_CENTER_AND_ANCHOR_POINT_COINCIDE; // the duration from the optical center to the anchor point is zero
  composition.number_of_extra_waves_bytes = 0; 
  composition.number_of_samplings = 2;                                                              // one outgoing, one returning
  composition.sample_units = 1.0f;                                                                  // [nanoseconds]
  composition.scanner_index = 1;

  memset(composition.description, 0, PULSEWAVES_DESCRIPTION_SIZE);
  strncpy(composition.description, "GeoLas GCW pulse (8 bit)", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[0].type = PULSEWAVES_OUTGOING;
  samplings[0].channel = 0;
  samplings[0].bits_for_duration_from_anchor = 0;            // the outgoing waveform segment starts at time zero at the anchor
  samplings[0].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[0].bits_for_number_of_samples = 16;              // the number of samples per segment is specified per segment with 16 bits
  samplings[0].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[0].number_of_samples = 0;                        // the number of samples per segment varies
  samplings[0].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[0].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[0].sample_units = 1.0f;                          // [nanoseconds]
  samplings[0].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[0].description, "outgoing at 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[1].type = PULSEWAVES_RETURNING;
  samplings[1].channel = 0;
  samplings[1].bits_for_duration_from_anchor = 16;           // the start of each waveform segment is specified with 16 bits (in sampling units) 
  samplings[1].scale_for_duration_from_anchor = 1.0f;        // the duration is specified in sampling unit increments (without fractions)
  samplings[1].offset_for_duration_from_anchor = 0.0f;       // the duration is specified with zero offset
  samplings[1].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[1].bits_for_number_of_samples = 16;              // the number of samples per segment is specified per segment with 16 bits
  samplings[1].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[1].number_of_samples = 0;                        // the number of samples per segment varies
  samplings[1].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[1].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[1].sample_units = 1.0f;                          // [nanoseconds]
  samplings[1].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[1].description, "returning at 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  header.add_descriptor(&composition, samplings, GCW_PULSE_DESCRIPTOR_INDEX_8_BIT, TRUE);

  memset(composition.description, 0, PULSEWAVES_DESCRIPTION_SIZE);
  strncpy(composition.description, "GeoLas GCW pulse (16 bit)", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[0].type = PULSEWAVES_OUTGOING;
  samplings[0].channel = 0;
  samplings[0].bits_for_duration_from_anchor = 0;            // the outgoing waveform segment starts at time zero at the anchor
  samplings[0].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[0].bits_for_number_of_samples = 16;              // the number of samples per segment is specified per segment with 16 bits
  samplings[0].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[0].number_of_samples = 0;                        // the number of samples per segment varies
  samplings[0].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[0].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[0].sample_units = 1.0f;                          // [nanoseconds]
  samplings[0].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[0].description, "outgoing at 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[1].type = PULSEWAVES_RETURNING;
  samplings[1].channel = 0;
  samplings[1].bits_for_duration_from_anchor = 16;           // the start of each waveform segment is specified with 16 bits (in sampling units) 
  samplings[1].scale_for_duration_from_anchor = 1.0f;        // the duration is specified in sampling unit increments (without fractions)
  samplings[1].offset_for_duration_from_anchor = 0.0f;       // the duration is specified with zero offset
  samplings[1].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[1].bits_for_number_of_samples = 16;              // the number of samples per segment is specified per segment with 16 bits
  samplings[1].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[1].number_of_samples = 0;                        // the number of samples per segment varies
  samplings[1].bits_per_sample = 16;                         // the number of bits per sample is always 16
  samplings[1].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[1].sample_units = 1.0f;                          // [nanoseconds]
  samplings[1].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[1].description, "returning at 16 bits", PULSEWAVES_DESCRIPTION_SIZE);

  header.add_descriptor(&composition, samplings, GCW_PULSE_DESCRIPTOR_INDEX_16_BIT, TRUE);

  pulse.init(&header);

  header_is_populated = FALSE;

  return TRUE;
}

BOOL PULSEreaderGCW::open_waves()
{
  if (file_name == 0)
  {
    return FALSE;
  }

  CHAR* temp_file_name = strdup(file_name);
  I32 len = strlen(temp_file_name);
  temp_file_name[len-3] = 'l';
  temp_file_name[len-2] = 'w';
  temp_file_name[len-1] = 'f';

  if (waves_file)
  {
    fclose(waves_file);
  }

  waves_file = fopen(temp_file_name, "rb");
  if (waves_file == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", temp_file_name);
    free(temp_file_name);
    return FALSE;
  }
  free(temp_file_name);

  if (waves_stream)
  {
    delete waves_stream;
  }

  if (IS_LITTLE_ENDIAN())
    waves_stream = new ByteStreamInFileLE(waves_file);
  else
    waves_stream = new ByteStreamInFileBE(waves_file);

  if (waves_stream == 0)
  {
    fprintf(stderr,"ERROR: waves_stream is zero\n");
    return FALSE;
  }

  if (!waves8bit.init(header.get_descriptor(GCW_PULSE_DESCRIPTOR_INDEX_8_BIT)))
  {
    fprintf(stderr,"ERROR: cannot init waves8bit\n");
    return FALSE;
  }

  if (!waves16bit.init(header.get_descriptor(GCW_PULSE_DESCRIPTOR_INDEX_16_BIT)))
  {
    fprintf(stderr,"ERROR: cannot init waves16bit\n");
    return FALSE;
  }

  return TRUE;
}

I32 PULSEreaderGCW::get_format() const
{
  return PULSEWAVES_FORMAT_GCW;
}

BOOL PULSEreaderGCW::seek(const I64 p_index)
{
  if (p_index < npulses)
  {
    try { pulse_stream->seek(p_index*56); } catch(...)
    {
      fprintf(stderr,"WARNING: cannot seek\n");
      return FALSE;
    }
    p_count = p_index;
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderGCW::read_pulse_default()
{
  // maybe we need to open the waves file
  if (read_pulse_lgc())
  {
    F64 anchor[3];
    anchor[0] = pulselgc.e0;
    anchor[1] = pulselgc.n0;
    anchor[2] = pulselgc.h0;
    F64 target[3];
    target[0] = pulselgc.e0 + pulselgc.de*1000;
    target[1] = pulselgc.n0 + pulselgc.dn*1000;
    target[2] = pulselgc.h0 + pulselgc.dh*1000;
    pulse.offset = pulselgc.wfi;
    pulse.set_anchor_and_target(anchor, target);
    pulse.first_returning_sample = pulselgc.wfoffset;
    pulse.last_returning_sample = pulselgc.wfoffset + pulselgc.wflen - 1;
    if (pulselgc.sampdepth == 0)
    {
      pulse.descriptor_index = GCW_PULSE_DESCRIPTOR_INDEX_8_BIT;
    }
    else
    {
      pulse.descriptor_index = GCW_PULSE_DESCRIPTOR_INDEX_16_BIT;
    }
    p_count++;
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderGCW::read_waves()
{
  // maybe we need to open the waves file
  if (waves_stream == 0)
  {
    if (!open_waves())
    {
      return FALSE;
    }
  }
  waves_stream->seek(pulselgc.wfi);
  if (pulselgc.sampdepth == 0)
  {
    // request memory for outgoing waveform
    if (!waves8bit.get_sampling(0)->set_number_of_samples_for_segment(pulselgc.strtwflen))
    {
      fprintf(stderr,"ERROR: setting number of outgoing 8 bit samples to %d\n", pulselgc.strtwflen);
    }
    // read outgoing waveform
    try { waves_stream->getBytes(waves8bit.get_sampling(0)->get_samples(), pulselgc.strtwflen); } catch(...)
    {
      fprintf(stderr,"ERROR: reading samplings[0] of %d outgoing 8 bit samples\n", pulselgc.strtwflen);
      return FALSE;
    }
    // set duration of returning waveform
    waves8bit.get_sampling(1)->set_duration_from_anchor_for_segment(pulselgc.wfoffset);
    // request memory for returning waveform
    if (!waves8bit.get_sampling(1)->set_number_of_samples_for_segment(pulselgc.wflen))
    {
      fprintf(stderr,"ERROR: setting number of returning 8 bit samples to %d\n", pulselgc.wflen);
    }
    // read returning waveform
    try { waves_stream->getBytes(waves8bit.get_sampling(1)->get_samples(), pulselgc.wflen); } catch(...)
    {
      fprintf(stderr,"ERROR: reading samplings[1] of %d returning 8 bit samples\n", pulselgc.wflen);
      return FALSE;
    }
    waves = &waves8bit;
  }
  else
  {
    // request memory outgoing waveform
    if (!waves16bit.get_sampling(0)->set_number_of_samples_for_segment(pulselgc.strtwflen))
    {
      fprintf(stderr,"ERROR: setting number of outgoing 8 bit samples to %d\n", pulselgc.strtwflen);
    }
    // read outgoing waveform
    try { waves_stream->getBytes(waves16bit.get_sampling(0)->get_samples(), pulselgc.strtwflen); } catch(...)
    {
      fprintf(stderr,"ERROR: reading samplings[0] of %d outgoing 8 bit samples\n", pulselgc.strtwflen);
      return FALSE;
    }
    // set duration of returning waveform
    waves16bit.get_sampling(1)->set_duration_from_anchor_for_segment(pulselgc.wfoffset);
    // request memory for returning waveform
    if (!waves16bit.get_sampling(1)->set_number_of_samples_for_segment(pulselgc.wflen))
    {
      fprintf(stderr,"ERROR: setting number of returning 16 bit samples to %d\n", pulselgc.wflen);
    }
    // read returning waveform
    try { waves_stream->getBytes(waves16bit.get_sampling(1)->get_samples(), pulselgc.wflen*2); } catch(...)
    {
      fprintf(stderr,"ERROR: reading samplings[1] of %d returning 16 bit samples\n", pulselgc.wflen);
      return FALSE;
    }
    waves = &waves16bit;
  }
  return TRUE;
}

ByteStreamIn* PULSEreaderGCW::get_pulse_stream() const
{
  return pulse_stream;
}

ByteStreamIn* PULSEreaderGCW::get_waves_stream() const
{
  return waves_stream;
}

void PULSEreaderGCW::close(BOOL close_streams)
{
  if (close_streams)
  {
    if (pulse_stream)
    {
      delete pulse_stream;
      pulse_stream = 0;
    }
    if (pulse_file)
    {
      fclose(pulse_file);
      pulse_file = 0;
    }
    if (waves_stream)
    {
      delete waves_stream;
      waves_stream = 0;
    }
    if (waves_file)
    {
      fclose(waves_file);
      waves_file = 0;
    }
  }
}

PULSEreaderGCW::PULSEreaderGCW()
{
  file_name = 0;
  pulse_file = 0;
  pulse_stream = 0;
  waves_file = 0;
  waves_stream = 0;
}

PULSEreaderGCW::~PULSEreaderGCW()
{
  if (file_name) free(file_name);
  if (pulse_stream || waves_stream) close();
}
