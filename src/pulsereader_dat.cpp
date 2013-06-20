/*
===============================================================================

  FILE:  pulsereader_dat.cpp
  
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
#include "pulsereader_dat.hpp"

#include "bytestreamin.hpp"
#include "bytestreamin_file.hpp"
#include "bytestreamin_istream.hpp"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>

static const I32 DATpulse_size_v31 = 70456;

static BOOL valid_lonlat(F64 lon, F64 lat)
{
  if (lon < -360.0 || lon > 360.0 || lat < -180.0 || lat > 180.0)
  {
    return FALSE;
  }
  return TRUE;
};

BOOL PULSEreaderDAT::load_pulse_v31()
{
  I64 current = stream->tell();

  // seek to i_lat and i_lon
  try { stream->seek(current + 36); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek to i_lat and i_lon\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulsedat.i_lat); } catch(...)
  {
    fprintf(stderr,"ERROR: reading i_lat\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulsedat.i_lon); } catch(...)
  {
    fprintf(stderr,"ERROR: reading i_lon\n");
    return FALSE;
  }

  // seek to i_rng_geoid
  try { stream->seek(current + 68); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek to i_rng_geoid\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulsedat.i_rng_geoid); } catch(...)
  {
    fprintf(stderr,"ERROR: reading i_rng_geoid\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulsedat.i_topo_elev); } catch(...)
  {
    fprintf(stderr,"ERROR: reading i_topo_elev\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulsedat.i_Rng2PCProf); } catch(...)
  {
    fprintf(stderr,"ERROR: reading i_Rng2PCProf\n");
    return FALSE;
  }
  try { stream->get32bitsBE((U8*)&pulsedat.i_rng2CDProf); } catch(...)
  {
    fprintf(stderr,"ERROR: reading i_rng2CDProf\n");
    return FALSE;
  }

  // seek to end of record
  try { stream->seek(current + 70456); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek to end of record\n");
    return FALSE;
  }

  return TRUE;
};

BOOL PULSEreaderDAT::open(const char* file_name, U32 io_buffer_size)
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

BOOL PULSEreaderDAT::open()
{
  // clean the header

  header.clean();

  // currently we only support verison 31

  version = 31;
  load_pulse = &PULSEreaderDAT::load_pulse_v31;
  size = DATpulse_size_v31;

  // read the first pulse 

  if (!(this->*load_pulse)())
  {
    fprintf(stderr,"ERROR: reading first pulse\n");
    return FALSE;
  }

  // set some parameters

  memset(header.system_identifier, 0, PULSEWAVES_DESCRIPTION_SIZE);
  memset(header.generating_software, 0, PULSEWAVES_DESCRIPTION_SIZE);
  sprintf(header.system_identifier, "created by PULSEreaderDAT");
  sprintf(header.generating_software, "PulseWaves %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);

  header.file_creation_day = 333;
  header.file_creation_year = 2012;

  // use first pulse to determine some settings

  double z;

  header.x_scale_factor = 1e-7;
  header.y_scale_factor = 1e-7;
  header.z_scale_factor = 0.01;

  header.x_offset = I32_QUANTIZE(pulsedat.get_i_lon()/10)*10;
  header.y_offset = I32_QUANTIZE(pulsedat.get_i_lat()/10)*10;
  header.z_offset = 0;

  header.min_x = header.max_x = pulsedat.get_i_lon();
  header.min_y = header.max_y = pulsedat.get_i_lat();
  z = pulsedat.get_i_rng_geoid() - pulsedat.get_i_Rng2PCProf(); // start z of 532 nm Backscatter Profile
  header.min_z = header.max_z = z;

  z = pulsedat.get_i_rng_geoid() - pulsedat.get_i_rng2CDProf(); // start z of 1064 nm Backscatter Profile
  if (header.min_z > z) header.min_z = z;
  else if (header.max_z < z) header.max_z = z;

  // seek to the end of the file

  try { stream->seekEnd(0); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek to end of file\n");
    return FALSE;
  }

  I64 file_size = stream->tell();

  if ( (file_size % size) != 0 )
  {
    fprintf(stderr,"WARNING: odd file size. with data records of %d bytes there is a remainder of %d bytes\n", size, (I32)(file_size % size));
  }

  npulses = (file_size / size) + 1;
  p_count = 0;

  // seek to the last pulse in the file

  try { stream->seek(npulses*size); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek to the last pulse in the file\n");
    return FALSE;
  }

  // read the last pulse 

  if (!(this->*load_pulse)())
  {
    fprintf(stderr,"ERROR: reading last pulse\n");
    return FALSE;
  }

  // use last pulse to update bounding box approximation

  if (header.min_x > pulsedat.get_i_lon()) header.min_x = pulsedat.get_i_lon();
  else if (header.max_x < pulsedat.get_i_lon()) header.max_x = pulsedat.get_i_lon();

  if (header.min_y > pulsedat.get_i_lat()) header.min_y = pulsedat.get_i_lat();
  else if (header.max_y < pulsedat.get_i_lat()) header.max_y = pulsedat.get_i_lat();

  z = pulsedat.get_i_rng_geoid() - pulsedat.get_i_Rng2PCProf(); // start z of 532 nm Backscatter Profile
  if (header.min_z > z) header.min_z = z;
  else if (header.max_z < z) header.max_z = z;

  z = pulsedat.get_i_rng_geoid() - pulsedat.get_i_rng2CDProf(); // start z of 1064 nm Backscatter Profile
  if (header.min_z > z) header.min_z = z;
  else if (header.max_z < z) header.max_z = z;

  try { stream->seek(0); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek back to first pulse\n");
    return FALSE;
  }

  // create Projection

  PULSEkeyentry geokeys[4];

  // projected coordinates
  geokeys[0].key_id = 1024; // GTModelTypeGeoKey
  geokeys[0].tiff_tag_location = 0;
  geokeys[0].count = 1;
  geokeys[0].value_offset = 2; // ModelTypeGeographic

  // ellipsoid used with latitude/longitude coordinates
  geokeys[1].key_id = 2048; // GeographicTypeGeoKey
  geokeys[1].tiff_tag_location = 0;
  geokeys[1].count = 1;
  geokeys[1].value_offset = 4326; // WGS84

  // vertical units
  geokeys[2].key_id = 4099; // VerticalUnitsGeoKey
  geokeys[2].tiff_tag_location = 0;
  geokeys[2].count = 1;
  geokeys[2].value_offset = 9001;

  // vertical datum
  geokeys[3].key_id = 4096; // VerticalCSTypeGeoKey
  geokeys[3].tiff_tag_location = 0;
  geokeys[3].count = 1;
  geokeys[3].value_offset = 5030; // WGS84

  header.set_geokeys(4, geokeys);

  // create Pulse Description
/*
  PULSEdescription pulsedescription;
  pulsedescription.optical_center_to_anchor_point = 0xFFFFFFFFFFFFFFFF;
  pulsedescription.sample_units = 2;                   // [nanoseconds]
  pulsedescription.scanner_id = 0;
  pulsedescription.wavelength = 1064; // [nanometer]
  pulsedescription.outgoing_pulse_width = 10; // [nanoseconds]
  pulsedescription.beam_diameter_at_exit_aperture = 0; // ??
  pulsedescription.beam_divergence = 0; // ??
  strncpy(pulsedescription.description, "Laser Vegetation Imaging Sensor", 64);
  pulsedescription.num_samplings = 1;

  PULSEsampling samplings;
  samplings.bits_per_sample = 8;
  samplings.number_of_samples = 432;
  samplings.type = PULSEWAVES_RETURNING;
  samplings.channel = 0;
  samplings.segment = 0;
  samplings.sample_units = 2; // [nanoseconds]
  samplings.digitizer_gain = 0; // ??? [Volt]
  samplings.digitizer_offset = 0;   // ??? [Volt]
  strncpy(samplings.description, "432 samples at 8 bits", 64);

  header.add_pulsedescriptor(&pulsedescription, &samplings);
*/

  pulse.init(&header, 0);

  return TRUE;
}

I32 PULSEreaderDAT::get_format() const
{
  return PULSEWAVES_FORMAT_DAT;
}

BOOL PULSEreaderDAT::seek(const I64 p_index)
{
  if (p_index < npulses)
  {
    try { stream->seek(p_index*size); } catch(...)
    {
      fprintf(stderr,"WARNING: cannot seek\n");
      return FALSE;
    }
    p_count = p_index;
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderDAT::read_pulse_default()
{
  if (p_count < npulses)
  {
    if (!(this->*load_pulse)())
    {
      fprintf(stderr,"WARNING: end-of-file after %u of %u pulses\n", (U32)p_count, (U32)npulses);
      return FALSE;
    }
    pulse.set_x(pulsedat.i_lon);
    pulse.set_y(pulsedat.i_lat);
    pulse.set_z(100.0*pulsedat.i_rng_geoid);
//    pulse.dx = (pulsedat.lon431 - pulsedat.lon0) / 431;
//    pulse.dy = (pulsedat.lat431 - pulsedat.lat0) / 431;
//    pulse.dz = (pulsedat.z431 - pulsedat.z0) / 431;
    pulse.first_returning_sample = 0;
    pulse.last_returning_sample = 431;
    pulse.descriptor_index = 0;
//    pulse.scan_direction_flag = 0;
//    pulse.edge_of_flight_line = 0;
    p_count++;
    return TRUE;
  }
  return FALSE;
}

ByteStreamIn* PULSEreaderDAT::get_stream() const
{
  return stream;
}

void PULSEreaderDAT::close(BOOL close_stream)
{
  if (close_stream)
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

PULSEreaderDAT::PULSEreaderDAT()
{
  file = 0;
  stream = 0;
  version = 0;
  size = 0;
}

PULSEreaderDAT::~PULSEreaderDAT()
{
  if (stream) close();
}
