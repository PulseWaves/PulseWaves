/*
===============================================================================

  FILE:  pulsereader_lgw.cpp
  
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
#include "pulsereader_lgw.hpp"

#include "pulseutility.hpp"
#include "bytestreamin.hpp"
#include "bytestreamin_file.hpp"
#include "bytestreamin_istream.hpp"

#define LGW_PULSE_DESCRIPTOR 1

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>

static const I32 LGWpulse_size_v1_00 = 476;
static const I32 LGWpulse_size_v1_01 = 484;
static const I32 LGWpulse_size_v1_02 = 492;
static const I32 LGWpulse_size_v1_03 = 584;

static BOOL valid_lonlat(F64 lon, F64 lat)
{
  if (lon < -360.0 || lon > 360.0 || lat < -180.0 || lat > 180.0)
  {
    return FALSE;
  }
  return TRUE;
};

BOOL PULSEreaderLGW::load_pulse_v1_00()
{
  try { stream->get64bitsBE((U8*)&pulselgw.lon0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lon0\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lat0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lat0\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.z0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z0\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lon431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lon431\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lat431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lat431\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.z431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z431\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.sigmean); } catch(...)
  {
    fprintf(stderr,"ERROR: reading sigmean\n");
    return FALSE;
  }
  try { stream->getBytes(pulselgw.rxwave, 432); } catch(...)
  {
    fprintf(stderr,"ERROR: reading rxwave[432]\n");
    return FALSE;
  }
  return TRUE;
};

BOOL PULSEreaderLGW::load_pulse_v1_01()
{
  try { stream->get32bitsBE((U8*)&pulselgw.lfid); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lfid\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.shotnumber); } catch(...)
  {
    fprintf(stderr,"ERROR: reading shotnumber\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lon0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lon0\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lat0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lat0\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.z0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z0\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lon431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lon431\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lat431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lat431\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.z431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z431\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.sigmean); } catch(...)
  {
    fprintf(stderr,"ERROR: reading sigmean\n");
    return FALSE;
  }
  try { stream->getBytes(pulselgw.rxwave, 432); } catch(...)
  {
    fprintf(stderr,"ERROR: reading rxwave[432]\n");
    return FALSE;
  }
  return TRUE;
};

BOOL PULSEreaderLGW::load_pulse_v1_02()
{
  try { stream->get32bitsBE((U8*)&pulselgw.lfid); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lfid\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.shotnumber); } catch(...)
  {
    fprintf(stderr,"ERROR: reading shotnumber\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.utc_time); } catch(...)
  {
    fprintf(stderr,"ERROR: reading utc_time\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lon0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lon0\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lat0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lat0\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.z0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z0\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lon431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lon431\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lat431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lat431\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.z431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z431\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.sigmean); } catch(...)
  {
    fprintf(stderr,"ERROR: reading sigmean\n");
    return FALSE;
  }
  try { stream->getBytes(pulselgw.rxwave, 432); } catch(...)
  {
    fprintf(stderr,"ERROR: reading rxwave[432]\n");
    return FALSE;
  }
  return TRUE;
};

BOOL PULSEreaderLGW::load_pulse_v1_03()
{
  try { stream->get32bitsBE((U8*)&pulselgw.lfid); } catch(...)
  {
    fprintf(stderr,"ERROR: reading shotnumber\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.shotnumber); } catch(...)
  {
    fprintf(stderr,"ERROR: reading shotnumber\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.azimuth); } catch(...)
  {
    fprintf(stderr,"ERROR: reading azimuth\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.incidentangle); } catch(...)
  {
    fprintf(stderr,"ERROR: reading incidentangle\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.range); } catch(...)
  {
    fprintf(stderr,"ERROR: reading range\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.utc_time); } catch(...)
  {
    fprintf(stderr,"ERROR: reading utc_time\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lon0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lon0\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lat0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lat0\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.z0); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z0\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lon431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lon431\n");
    return FALSE;
  }
  try { stream->get64bitsBE((U8*)&pulselgw.lat431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading lat431\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.z431); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z431\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulselgw.sigmean); } catch(...)
  {
    fprintf(stderr,"ERROR: reading sigmean\n");
    return FALSE;
  }
  try { stream->getBytes(pulselgw.txwave, 80); } catch(...)
  {
    fprintf(stderr,"ERROR: reading txwave[80]\n");
    return FALSE;
  }
  try { stream->getBytes(pulselgw.rxwave, 432); } catch(...)
  {
    fprintf(stderr,"ERROR: reading rxwave[432]\n");
    return FALSE;
  }
  return TRUE;
};

BOOL PULSEreaderLGW::open(const char* file_name, U32 io_buffer_size)
{
  if (file_name == 0)
  {
    fprintf(stderr,"ERROR: file name pointer is zero\n");
    return FALSE;
  }

  file = fopen(file_name, "rb");
  if (file == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", file_name);
    return FALSE;
  }

  if (setvbuf(file, NULL, _IOFBF, io_buffer_size) != 0)
  {
    fprintf(stderr, "WARNING: setvbuf() failed with buffer size %u\n", io_buffer_size);
  }

  if (IS_LITTLE_ENDIAN())
    stream = new ByteStreamInFileLE(file);
  else
    stream = new ByteStreamInFileBE(file);

  if (stream == 0)
  {
    fprintf(stderr,"ERROR: stream pointer is zero\n");
    return FALSE;
  }

  return open();
}

BOOL PULSEreaderLGW::open()
{
  // clean the header

  header.clean();

  // set time offset to zero

  header.t_offset = 0.0;

  // read the first two pulses to determine the version

  PULSEpulseLGW lgwpulse1;
  PULSEpulseLGW lgwpulse2;

  if (!load_pulse_v1_00())
  {
    fprintf(stderr,"ERROR: reading first pulse as v1_00\n");
    return FALSE;
  }
  lgwpulse1 = pulselgw;
  if (!load_pulse_v1_00())
  {
    fprintf(stderr,"ERROR: reading second pulse as v1_00\n");
    return FALSE;
  }
  lgwpulse2 = pulselgw;
  
  if (!valid_lonlat(lgwpulse1.lon0, lgwpulse1.lat0) || !valid_lonlat(lgwpulse1.lon431, lgwpulse1.lat431) || !valid_lonlat(lgwpulse2.lon0, lgwpulse2.lat0) || !valid_lonlat(lgwpulse2.lon431, lgwpulse2.lat431))
  {
    try { stream->seek(0); } catch(...)
    {
      fprintf(stderr,"ERROR: cannot seek back to first pulse\n");
      return FALSE;
    }
 
    if (!load_pulse_v1_01())
    {
      fprintf(stderr,"ERROR: reading first pulse as v1_01\n");
      return FALSE;
    }
    lgwpulse1 = pulselgw;
    if (!load_pulse_v1_01())
    {
      fprintf(stderr,"ERROR: reading second pulse as v1_01\n");
      return FALSE;
    }
    lgwpulse2 = pulselgw;

    if (!valid_lonlat(lgwpulse1.lon0, lgwpulse1.lat0) || !valid_lonlat(lgwpulse1.lon431, lgwpulse1.lat431) || !valid_lonlat(lgwpulse2.lon0, lgwpulse2.lat0) || !valid_lonlat(lgwpulse2.lon431, lgwpulse2.lat431))
    {
      try { stream->seek(0); } catch(...)
      {
        fprintf(stderr,"ERROR: cannot seek back to first pulse\n");
        return FALSE;
      }

      if (!load_pulse_v1_02())
      {
        fprintf(stderr,"ERROR: reading first pulse as v1_02\n");
        return FALSE;
      }
      lgwpulse1 = pulselgw;
      if (!load_pulse_v1_02())
      {
        fprintf(stderr,"ERROR: reading second pulse as v1_02\n");
        return FALSE;
      }
      lgwpulse2 = pulselgw;
  
      if (!valid_lonlat(lgwpulse1.lon0, lgwpulse1.lat0) || !valid_lonlat(lgwpulse1.lon431, lgwpulse1.lat431) || !valid_lonlat(lgwpulse2.lon0, lgwpulse2.lat0) || !valid_lonlat(lgwpulse2.lon431, lgwpulse2.lat431))
      {
        try { stream->seek(0); } catch(...)
        {
          fprintf(stderr,"ERROR: cannot seek back to first pulse\n");
          return FALSE;
        }

        if (!load_pulse_v1_03())
        {
          fprintf(stderr,"ERROR: reading first pulse as v1_03\n");
          return FALSE;
        }
        lgwpulse1 = pulselgw;
        if (!load_pulse_v1_03())
        {
          fprintf(stderr,"ERROR: reading second pulse as v1_03\n");
          return FALSE;
        }
        lgwpulse2 = pulselgw;

        if (!valid_lonlat(lgwpulse1.lon0, lgwpulse1.lat0) || !valid_lonlat(lgwpulse1.lon431, lgwpulse1.lat431) || !valid_lonlat(lgwpulse2.lon0, lgwpulse2.lat0) || !valid_lonlat(lgwpulse2.lon431, lgwpulse2.lat431))
        {
          fprintf(stderr,"ERROR: unknown LVIS *.lgw file format. it is neither v1_00 nor v1_01 nor v1_02 nor v1_03.\n");
          return FALSE;
        }
        else
        {
          version = 103;
          load_pulse = &PULSEreaderLGW::load_pulse_v1_03;
          size = LGWpulse_size_v1_03;
        }
      }
      else
      {
        version = 102;
        load_pulse = &PULSEreaderLGW::load_pulse_v1_02;
        size = LGWpulse_size_v1_02;
      }
    }
    else
    {
      version = 101;
      load_pulse = &PULSEreaderLGW::load_pulse_v1_01;
      size = LGWpulse_size_v1_01;
    }
  }
  else
  {
    version = 100;
    load_pulse = &PULSEreaderLGW::load_pulse_v1_00;
    size = LGWpulse_size_v1_00;
  }

  // set some parameters

  memset(header.system_identifier, 0, PULSEWAVES_DESCRIPTION_SIZE);
  memset(header.generating_software, 0, PULSEWAVES_DESCRIPTION_SIZE);
  sprintf(header.system_identifier, "created by PULSEreaderLGW");
  sprintf(header.generating_software, "PulseWaves %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);

  header.file_creation_day = 333;
  header.file_creation_year = 2012;

  // use first pulse to determine some settings

  header.x_scale_factor = 1e-7;
  header.y_scale_factor = 1e-7;
  header.z_scale_factor = 0.01;

  header.x_offset = I32_QUANTIZE(pulselgw.lon0/10)*10;
  header.y_offset = I32_QUANTIZE(pulselgw.lat0/10)*10;
  header.z_offset = 0;

  header.min_x = header.max_x = pulselgw.lon0;
  header.min_y = header.max_y = pulselgw.lat0;
  header.min_z = header.max_z = pulselgw.z0;

  if (header.min_x > pulselgw.lon431) header.min_x = pulselgw.lon431;
  else if (header.max_x < pulselgw.lon431) header.max_x = pulselgw.lon431;

  if (header.min_y > pulselgw.lat431) header.min_y = pulselgw.lat431;
  else if (header.max_y < pulselgw.lat431) header.max_y = pulselgw.lat431;

  if (header.min_z > pulselgw.z431) header.min_z = pulselgw.z431;
  else if (header.max_z < pulselgw.z431) header.max_z = pulselgw.z431;

  // seek to the last pulse in the file

  try { stream->seekEnd(size); } catch(...)
  {
    fprintf(stderr,"ERROR: seeking to intermediate pulse\n");
    return FALSE;
  }

  I64 file_size = stream->tell();

  if ( (file_size % size) != 0 )
  {
    fprintf(stderr,"WARNING: odd file size. with data records of %d bytes there is a remainder of %d bytes\n", size, (I32)(file_size % size));
  }

  header.number_of_pulses = npulses = (file_size / size) + 1;
  p_count = 0;

  // use last pulse to update bounding box approximation

  (this->*load_pulse)();

  if (header.min_x > pulselgw.lon0) header.min_x = pulselgw.lon0;
  else if (header.max_x < pulselgw.lon0) header.max_x = pulselgw.lon0;

  if (header.min_y > pulselgw.lat0) header.min_y = pulselgw.lat0;
  else if (header.max_y < pulselgw.lat0) header.max_y = pulselgw.lat0;

  if (header.min_z > pulselgw.z0) header.min_z = pulselgw.z0;
  else if (header.max_z < pulselgw.z0) header.max_z = pulselgw.z0;

  if (header.min_x > pulselgw.lon431) header.min_x = pulselgw.lon431;
  else if (header.max_x < pulselgw.lon431) header.max_x = pulselgw.lon431;

  if (header.min_y > pulselgw.lat431) header.min_y = pulselgw.lat431;
  else if (header.max_y < pulselgw.lat431) header.max_y = pulselgw.lat431;

  if (header.min_z > pulselgw.z431) header.min_z = pulselgw.z431;
  else if (header.max_z < pulselgw.z431) header.max_z = pulselgw.z431;

  // load a few more pulses to update bounding box approximation

  try { stream->seek(npulses/4*size); } catch(...)
  {
    fprintf(stderr,"ERROR: seeking to intermediate pulse\n");
    return FALSE;
  }

  (this->*load_pulse)();

  if (header.min_x > pulselgw.lon0) header.min_x = pulselgw.lon0;
  else if (header.max_x < pulselgw.lon0) header.max_x = pulselgw.lon0;

  if (header.min_y > pulselgw.lat0) header.min_y = pulselgw.lat0;
  else if (header.max_y < pulselgw.lat0) header.max_y = pulselgw.lat0;

  if (header.min_z > pulselgw.z0) header.min_z = pulselgw.z0;
  else if (header.max_z < pulselgw.z0) header.max_z = pulselgw.z0;

  if (header.min_x > pulselgw.lon431) header.min_x = pulselgw.lon431;
  else if (header.max_x < pulselgw.lon431) header.max_x = pulselgw.lon431;

  if (header.min_y > pulselgw.lat431) header.min_y = pulselgw.lat431;
  else if (header.max_y < pulselgw.lat431) header.max_y = pulselgw.lat431;

  if (header.min_z > pulselgw.z431) header.min_z = pulselgw.z431;
  else if (header.max_z < pulselgw.z431) header.max_z = pulselgw.z431;

  try { stream->seek(npulses/4*2*size); } catch(...)
  {
    fprintf(stderr,"ERROR: seeking to intermediate pulse\n");
    return FALSE;
  }

  (this->*load_pulse)();

  if (header.min_x > pulselgw.lon0) header.min_x = pulselgw.lon0;
  else if (header.max_x < pulselgw.lon0) header.max_x = pulselgw.lon0;

  if (header.min_y > pulselgw.lat0) header.min_y = pulselgw.lat0;
  else if (header.max_y < pulselgw.lat0) header.max_y = pulselgw.lat0;

  if (header.min_z > pulselgw.z0) header.min_z = pulselgw.z0;
  else if (header.max_z < pulselgw.z0) header.max_z = pulselgw.z0;

  if (header.min_x > pulselgw.lon431) header.min_x = pulselgw.lon431;
  else if (header.max_x < pulselgw.lon431) header.max_x = pulselgw.lon431;

  if (header.min_y > pulselgw.lat431) header.min_y = pulselgw.lat431;
  else if (header.max_y < pulselgw.lat431) header.max_y = pulselgw.lat431;

  if (header.min_z > pulselgw.z431) header.min_z = pulselgw.z431;
  else if (header.max_z < pulselgw.z431) header.max_z = pulselgw.z431;

  try { stream->seek(npulses/4*3*size); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek to intermediate pulse\n");
    return FALSE;
  }

  (this->*load_pulse)();

  if (header.min_x > pulselgw.lon0) header.min_x = pulselgw.lon0;
  else if (header.max_x < pulselgw.lon0) header.max_x = pulselgw.lon0;

  if (header.min_y > pulselgw.lat0) header.min_y = pulselgw.lat0;
  else if (header.max_y < pulselgw.lat0) header.max_y = pulselgw.lat0;

  if (header.min_z > pulselgw.z0) header.min_z = pulselgw.z0;
  else if (header.max_z < pulselgw.z0) header.max_z = pulselgw.z0;

  if (header.min_x > pulselgw.lon431) header.min_x = pulselgw.lon431;
  else if (header.max_x < pulselgw.lon431) header.max_x = pulselgw.lon431;

  if (header.min_y > pulselgw.lat431) header.min_y = pulselgw.lat431;
  else if (header.max_y < pulselgw.lat431) header.max_y = pulselgw.lat431;

  if (header.min_z > pulselgw.z431) header.min_z = pulselgw.z431;
  else if (header.max_z < pulselgw.z431) header.max_z = pulselgw.z431;

  // go back to the beginning of the stream

  try { stream->seek(0); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek back to first pulse\n");
    return FALSE;
  }

  // create Projection

  PULSEkeyentry key_entries[4];

  // geographic coordinates
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

  // create scanner

  PULSEscanner scanner;
  scanner.wave_length = 1064.0f;                 // [nanometer]
  scanner.outgoing_pulse_width = 10.0f;          // [nanoseconds]
  scanner.beam_diameter_at_exit_aperture = 8.0f; // [millimeters]
  scanner.beam_divergence = 0.8f;                // [milliradians]
  strncpy(scanner.instrument, "NASA Laser Vegetation Imaging Sensor (LVIS)", PULSEWAVES_DESCRIPTION_SIZE);

  header.add_scanner(&scanner, 1);

  // create descriptor (composition plus samplings)

  PULSEcomposition composition;
  PULSEsampling samplings[2];

  composition.optical_center_to_anchor_point = PULSEWAVES_OPTICAL_CENTER_AND_ANCHOR_POINT_FLUCTUATE; // the duration between optical center and anchor point is not constant
  composition.number_of_extra_waves_bytes = 0;     
  composition.number_of_samplings = (version >= 103 ? 2 : 1);                // usually only the returning wave but starting with version 103 also the outgoing
  composition.sample_units = 2;                                              // [nanoseconds]
  composition.scanner_index = 1;

  strncpy(composition.description, "NASA Laser Vegetation Imaging Sensor (LVIS)", PULSEWAVES_DESCRIPTION_SIZE);

  I32 i = 0;

  if (version >= 103)
  {
    samplings[i].type = PULSEWAVES_OUTGOING;
    samplings[i].channel = 0;
    samplings[i].bits_for_duration_from_anchor = 0;            // the outgoing waveform has no duration from anchor information
    samplings[i].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
    samplings[i].bits_for_number_of_samples = 0;               // the number of samples per segment is fixed across all segments (i.e. it is always 8)
    samplings[i].number_of_segments = 1;                       // the number of segments per sampling is always 1
    samplings[i].number_of_samples = 80;                       // the number of samples per segment is always 80
    samplings[i].bits_per_sample = 8;                          // the number of bits per sample is always 8
    samplings[i].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
    samplings[i].sample_units = 2.0f;                          // [nanoseconds]
    samplings[i].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
    strncpy(samplings[i].description, "outgoing with 80 samples at 8 bits", PULSEWAVES_DESCRIPTION_SIZE);
    i++;
  }

  samplings[i].type = PULSEWAVES_RETURNING;
  samplings[i].channel = 0;
  samplings[i].bits_for_duration_from_anchor = 0;            // the returning waveform starts at the anchor
  samplings[i].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[i].bits_for_number_of_samples = 0;               // the number of samples per segment is fixed across all segments (i.e. it is always 8)
  samplings[i].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[i].number_of_samples = 432;                      // the number of samples per segment is always 432
  samplings[i].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[i].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[i].sample_units = 2.0f;                          // [nanoseconds]
  samplings[i].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[i].description, "returning with 432 samples at 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  header.add_descriptor(&composition, samplings, LGW_PULSE_DESCRIPTOR, TRUE);

  header_is_populated = FALSE;

  pulse.init(&header);

  return TRUE;
}

I32 PULSEreaderLGW::get_format() const
{
  return PULSEWAVES_FORMAT_LGW;
}

BOOL PULSEreaderLGW::seek(const I64 p_index)
{
  if (p_index < npulses)
  {
    try { stream->seek(p_index*size); } catch(...)
    {
#ifdef _WIN32
      fprintf(stderr,"WARNING: cannot seek to %I64d\n", p_index);
#else
      fprintf(stderr,"WARNING: cannot seek to %lld\n", p_index);
#endif
      return FALSE;
    }
    p_count = p_index;
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderLGW::read_pulse_default()
{
  if (p_count < npulses)
  {
    if (!(this->*load_pulse)())
    {
      fprintf(stderr,"WARNING: end-of-file after %u of %u pulses\n", (U32)p_count, (U32)npulses);
      return FALSE;
    }
    F64 anchor[3];
    F64 target[3];
    anchor[0] = pulselgw.lon0;
    anchor[1] = pulselgw.lat0;
    anchor[2] = pulselgw.z0;
    target[0] = pulselgw.lon0 + (pulselgw.lon431 - pulselgw.lon0) / 431 * 1000;
    target[1] = pulselgw.lat0 + (pulselgw.lat431 - pulselgw.lat0) / 431 * 1000;
    target[2] = pulselgw.z0 + (pulselgw.z431 - pulselgw.z0) / 431 * 1000;
    pulse.set_T(pulselgw.utc_time);
    pulse.offset = 0;
    pulse.set_anchor_and_target(anchor, target);
    pulse.first_returning_sample = 0;
    pulse.last_returning_sample = 431;
    pulse.descriptor_index = LGW_PULSE_DESCRIPTOR;
    p_count++;
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderLGW::read_waves()
{
  if (waves == 0)
  {
    if (!waveslgw.init(header.get_descriptor(LGW_PULSE_DESCRIPTOR)))
    {
      fprintf(stderr,"ERROR: cannot init waveslgw\n");
      return FALSE;
    }
    waves = &waveslgw;
  }
  if (version >= 103)
  {
    memcpy(waveslgw.get_sampling(0)->get_samples(), pulselgw.txwave, 80);
    memcpy(waveslgw.get_sampling(1)->get_samples(), pulselgw.rxwave, 432);
  }
  else
  {
    memcpy(waveslgw.get_sampling(0)->get_samples(), pulselgw.rxwave, 432);
  }
  return TRUE;
}

ByteStreamIn* PULSEreaderLGW::get_pulse_stream() const
{
  return stream;
}

ByteStreamIn* PULSEreaderLGW::get_waves_stream() const
{
  return stream;
}

void PULSEreaderLGW::close(BOOL close_streams)
{
  if (close_streams)
  {
    if (stream)
    {
      delete stream;
      stream = 0;
    }
    if (file)
    {
      fclose(file);
      file = 0;
    }
  }
}

PULSEreaderLGW::PULSEreaderLGW()
{
  file = 0;
  stream = 0;
  version = 0;
  size = 0;
}

PULSEreaderLGW::~PULSEreaderLGW()
{
  if (stream) close();
}
