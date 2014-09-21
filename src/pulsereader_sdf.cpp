/*
===============================================================================

  FILE:  pulsereader_sdf.cpp

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
#include "pulsereader_sdf.hpp"

#include "fwifc.h"
#ifdef HAVE_POF_FILE_FOR_SDF_FILE
#include "pofifc.h"
#endif

#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>
using namespace std;

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <windows.h>

// this function evaluates the return code from interface functions
// if the return code is 0 the function was executed successful
// if the return code is not 0 the function failed and the error message
// is retrieved. Then the error message is formatted and an exception
// with this message is thrown
void PULSEreaderSDF::fwifc_execute(U32 result)
{
  fwifc_error = result;
  if (!fwifc_error) return;
  fwifc_csz message;
  fwifc_get_last_error(&message);
  throw runtime_error(message);
}

#define DISTANCE_LIGHT_TRAVELS_IN_ONE_NANOSECOND 0.299792459
static const F64 PI = 3.141592653589793238462643383279502884197169;

inline void VecScale3dv(F64 v[3], const F64 scale, const F64 a[3])
{
  v[0] = scale * a[0];
  v[1] = scale * a[1];
  v[2] = scale * a[2];
}

static inline void MatVecMult33dv(F64 v[3], const F64 A[3][3], const F64 a[3])
{
  v[0] =  A[0][0]*a[0] + A[0][1]*a[1] + A[0][2]*a[2];
  v[1] =  A[1][0]*a[0] + A[1][1]*a[1] + A[1][2]*a[2];
  v[2] =  A[2][0]*a[0] + A[2][1]*a[1] + A[2][2]*a[2];
}

static inline void MatMatMult33dv(F64 V[3][3], const F64 A[3][3], const F64 B[3][3])
{
  V[0][0] =  A[0][0]*B[0][0] + A[0][1]*B[1][0] + A[0][2]*B[2][0];
  V[0][1] =  A[0][0]*B[0][1] + A[0][1]*B[1][1] + A[0][2]*B[2][1];
  V[0][2] =  A[0][0]*B[0][2] + A[0][1]*B[1][2] + A[0][2]*B[2][2];

  V[1][0] =  A[1][0]*B[0][0] + A[1][1]*B[1][0] + A[1][2]*B[2][0];
  V[1][1] =  A[1][0]*B[0][1] + A[1][1]*B[1][1] + A[1][2]*B[2][1];
  V[1][2] =  A[1][0]*B[0][2] + A[1][1]*B[1][2] + A[1][2]*B[2][2];

  V[2][0] =  A[2][0]*B[0][0] + A[2][1]*B[1][0] + A[2][2]*B[2][0];
  V[2][1] =  A[2][0]*B[0][1] + A[2][1]*B[1][1] + A[2][2]*B[2][1];
  V[2][2] =  A[2][0]*B[0][2] + A[2][1]*B[1][2] + A[2][2]*B[2][2];
}

#ifdef HAVE_POF_FILE_FOR_SDF_FILE

BOOL PULSEreaderSDF::get_origin_and_direction(F64 time_external, F64* origin, F64* direction)
{
  int err;
  pofifc_record_t record;

  if (err = pofifc_find_time((pofifc_file_t)pofifc_handle, time_external, &record, (pofifc_bool_t)1))
  {
    fprintf(stderr,"ERROR: cannot find trajectory record for time %g (err %d)\n", time_external, err);
    return FALSE;
  }

  F64 lat_rad = record.lat * PI / 180.0;
  F64 lon_rad = record.lon * PI / 180.0;

  F64 sin_phi = sin(lat_rad);
  F64 cos_phi = cos(lat_rad);
  F64 sin_lambda = sin(lon_rad);
  F64 cos_lambda = cos(lon_rad);
  F64 N = 6378137.0 / sqrt(1.0 - (0.006694380004260827 * sin_phi * sin_phi));

  // Chris Parrish's definition adapted from A. Roncat's matlab code
  F64 roll = record.roll * PI / 180.0; // + IMU_offsets[0];
  F64 pitch = record.pitch * PI / 180.0; // + IMU_offsets[1];
  F64 heading = record.yaw * PI / 180.0; // + IMU_offsets[2];

  F64 wgs84[3];
  wgs84[0] = (N + record.alt) * cos_phi * cos_lambda;
  wgs84[1] = (N + record.alt) * cos_phi * sin_lambda;
  wgs84[2] = ((N * (1.0 - 0.006694380004260827)) + record.alt) * sin_phi;

  // transformation matrices: local level to earth-fixed, body to local level
  F64 Re_l[3][3];

  Re_l[0][0] = -cos_lambda*sin_phi;
  Re_l[0][1] = -sin_lambda;
  Re_l[0][2] = -cos_lambda*cos_phi;
  Re_l[1][0] = -sin_lambda*sin_phi;
  Re_l[1][1] = cos_lambda;
  Re_l[1][2] = -sin_lambda*cos_phi;
  Re_l[2][0] = cos_phi;
  Re_l[2][1] = 0;
  Re_l[2][2] = -sin_phi;

  // Chis Parrish's variant adapted from A. Roncat's matlab code

  F64 rotMatrixMr[3][3];

  rotMatrixMr[0][0] = cos(heading)*cos(pitch);
  rotMatrixMr[0][1] = cos(heading)*sin(pitch)*sin(roll)-sin(heading)*cos(roll);
  rotMatrixMr[0][2] = sin(heading)*sin(roll)+sin(heading)*sin(pitch)*cos(roll);
  rotMatrixMr[1][0] = sin(heading)*cos(pitch);
  rotMatrixMr[1][1] = sin(heading)*sin(pitch)*sin(roll)+cos(heading)*cos(roll);
  rotMatrixMr[1][2] = sin(heading)*sin(pitch)-cos(heading)*cos(pitch)*sin(roll);
  rotMatrixMr[2][0] = -sin(pitch);
  rotMatrixMr[2][1] = cos(pitch)*sin(roll);
  rotMatrixMr[2][2] = cos(pitch)*cos(roll);

  F64 temp_1[3];
  F64 temp_2[3];
  MatVecMult33dv(temp_1, rotMatrixMr, origin);
  MatVecMult33dv(temp_2, Re_l, temp_1);
  origin[0] = temp_2[0] + wgs84[0];
  origin[1] = temp_2[1] + wgs84[1];
  origin[2] = temp_2[2] + wgs84[2];

  MatVecMult33dv(temp_1, rotMatrixMr, direction);
  MatVecMult33dv(temp_2, Re_l, temp_1);

  VecScale3dv(direction, 0.5 * DISTANCE_LIGHT_TRAVELS_IN_ONE_NANOSECOND, temp_2);

  return TRUE;
}

#endif // HAVE_POF_FILE_FOR_SDF_FILE

BOOL PULSEreaderSDF::open(const char* file_name)
{
  if (file_name == 0)
  {
    fprintf(stderr,"ERROR: file name pointer is zero\n");
    return FALSE;
  }

  // load RiWave DLL
  if (fwifc_load_dll())
  {
    fprintf(stderr,"ERROR: failed to load RiWave DLL\n");
    return FALSE;
  }

#ifdef HAVE_POF_FILE_FOR_SDF_FILE

  // load POFfile DLL

  if (pofifc_load_dll())
  {
    fprintf(stderr,"ERROR: failed to load POFfile DLL\n");
    return FALSE;
  }

#endif // HAVE_POF_FILE_FOR_SDF_FILE

  // close an existing waveform file

  if (fwifc_handle)
  {
    try
    {
      fwifc_execute(fwifc_close((fwifc_file)fwifc_handle));
    }
    catch (exception& e)
    {
      fprintf(stderr,"ERROR: %s\n", e.what());
      return FALSE;
    }
    catch (...)
    {
      fprintf(stderr,"ERROR: unknown exception in fwifc_close()\n");
      return FALSE;
    }
    fwifc_handle = 0;
  }

  // open waveform file:

  try
  {
    fwifc_execute(fwifc_open(file_name, (fwifc_file*)&fwifc_handle));
  }
  catch (exception& e)
  {
    fprintf(stderr,"ERROR: %s\n", e.what());
    return FALSE;
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_open()\n");
    return FALSE;
  }

#ifdef HAVE_POF_FILE_FOR_SDF_FILE

  // open trajectory file

  CHAR* temp_file_name = strdup(file_name);
  I32 len = strlen(temp_file_name);

  temp_file_name[len-3] = 'p';
  temp_file_name[len-2] = 'o';
  temp_file_name[len-1] = 'f';

  if (pofifc_open(temp_file_name, (pofifc_file_t*)&pofifc_handle))
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", temp_file_name);
    free(temp_file_name);
    return FALSE;
  }

  free(temp_file_name);

#endif // HAVE_POF_FILE_FOR_SDF_FILE

  return open();
}

BOOL PULSEreaderSDF::open()
{
  // retrieve header information from waveform file:

  fwifc_csz instrument;           /* the instrument type */
  fwifc_csz serial;               /* serial number of the instrument */
  fwifc_csz epoch;                /* of time_external can be a datetime */
                                  /* "2010-11-16T00:00:00" or */
                                  /* "DAYSEC" or "WEEKSEC" or */
                                  /* "UNKNOWN" if not known */
  fwifc_float64_t v_group;        /* group velocity in meters/second */
  fwifc_float64_t sampling_time;  /* sampling interval in seconds */
  fwifc_uint16_t flags;           /* GPS synchronized, ... */
  fwifc_uint16_t num_facets;      /* number of mirror facets */

  try
  {
    fwifc_execute(
      fwifc_get_info(
          (fwifc_file)fwifc_handle,
          &instrument,
          &serial,
          &epoch,
          &v_group,
          &sampling_time,
          &flags,
          &num_facets
      )
    );
  }
  catch (exception& e)
  {
    fprintf(stderr,"ERROR: %s\n", e.what());
    return FALSE;
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_get_info()\n");
    return FALSE;
  }

/*
  fwifc_uint32_t c, count;      // length of returned table
  fwifc_float64_t* abscissa;    // table values, valid until next
  fwifc_float64_t* ordinate;    // call into library
  try
  {
    fwifc_get_calib((fwifc_file)fwifc_handle, FWIFC_CALIB_AMPL_CH0, &count, &abscissa, &ordinate);
    fprintf(stderr,"FWIFC_CALIB_AMPL_CH0\n");
    for (c = 0; c < count; c++)
    {
      fprintf(stderr,"%g %g\n", abscissa[c], ordinate[c]);
    }
    fwifc_get_calib((fwifc_file)fwifc_handle, FWIFC_CALIB_AMPL_CH1, &count, &abscissa, &ordinate);
    fprintf(stderr,"FWIFC_CALIB_AMPL_CH1\n");
    for (c = 0; c < count; c++)
    {
      fprintf(stderr,"%g %g\n", abscissa[c], ordinate[c]);
    }
    fwifc_get_calib((fwifc_file)fwifc_handle, FWIFC_CALIB_RNG_CH0, &count, &abscissa, &ordinate);
    fprintf(stderr,"FWIFC_CALIB_RNG_CH0\n");
    for (c = 0; c < count; c++)
    {
      fprintf(stderr,"%g %g\n", abscissa[c], ordinate[c]);
    }
    fwifc_get_calib((fwifc_file)fwifc_handle, FWIFC_CALIB_RNG_CH1, &count, &abscissa, &ordinate);
    fprintf(stderr,"FWIFC_CALIB_RNG_CH1\n");
    for (c = 0; c < count; c++)
    {
      fprintf(stderr,"%g %g\n", abscissa[c], ordinate[c]);
    }
  }
  catch (exception& e)
  {
    fprintf(stderr,"ERROR: %s\n", e.what());
    return FALSE;
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_seek(1)\n");
    return FALSE;
  }
*/

  // clean the header

  header.clean();

  // set the global encoding flag

  if (strcmp(epoch, "WEEKSEC") != 0)
  {
    fprintf(stderr,"WARNING: epoch '%s' not supported yet\n", epoch);
  }

  // retrieve sample blocks from waveform file

  fwifc_float64_t time_sorg;      /* start of range gate in s */
  fwifc_float64_t time_external;  /* external time in s relative to epoch */
  fwifc_float64_t origin[3];      /* origin vector in m */
  fwifc_float64_t direction[3];   /* direction vector (dimensionless) */
  fwifc_uint16_t  facet;          /* facet number (0 to num_facets-1) */
  fwifc_uint32_t  sbl_count;      /* number of sample blocks */
  fwifc_uint32_t  sbl_size;       /* size of sample block in bytes */
  fwifc_sbl_t*    psbl_first;     /* pointer to first sample block */

  // seek to first waveform

  try
  {
    fwifc_execute(fwifc_seek((fwifc_file)fwifc_handle, 1));
  }
  catch (exception& e)
  {
    if (fwifc_error == FWIFC_ERR_MISSING_INDEX)
    {
      fprintf(stderr,"reindexing ...\n");
      try
      {
        fwifc_execute(fwifc_reindex((fwifc_file)fwifc_handle));
      }
      catch (exception& e)
      {
        fprintf(stderr,"ERROR: %s\n", e.what());
        return FALSE;
      }
      catch (...)
      {
        fprintf(stderr,"ERROR: unknown exception in fwifc_get_info()\n");
        return FALSE;
      }
    }
    else
    {
      fprintf(stderr,"ERROR: %s\n", e.what());
      return FALSE;
    }
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_seek(1)\n");
    return FALSE;
  }

  // read first pulse & waveform

  try
  {
    fwifc_execute(
      fwifc_read(
          (fwifc_file)fwifc_handle,
          &time_sorg,
          &time_external,
          &origin[0],
          &direction[0],
          &flags,
          &facet,
          &sbl_count,
          &sbl_size,
          &psbl_first
      )
    );
  }
  catch (exception& e)
  {
    fprintf(stderr,"ERROR: %s\n", e.what());
    return FALSE;
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_read(1)\n");
    return FALSE;
  }

  #ifdef HAVE_POF_FILE_FOR_SDF_FILE

  // convert to WGS 1984

  if (!get_origin_and_direction(time_external, origin, direction))
  {
    fprintf(stderr,"ERROR: in in get_origin_and_direction() for pulse (1) at time %g\n", time_external);
    return FALSE;
  }

  #endif // HAVE_POF_FILE_FOR_SDF_FILE

  header.min_x = header.max_x = origin[0];
  header.min_y = header.max_y = origin[1];
  header.min_z = header.max_z = origin[2];

  // seek to last waveform

  try
  {
    fwifc_execute(fwifc_seek((fwifc_file)fwifc_handle, 0xFFFFFFFF));
  }
  catch (exception& e)
  {
    fprintf(stderr,"ERROR: %s\n", e.what());
    return FALSE;
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_seek(0xFFFFFFFF)\n");
    return FALSE;
  }

  // get the record number of the last waveform
  fwifc_uint32_t  num_records;
  try
  {
    fwifc_execute(fwifc_tell((fwifc_file)fwifc_handle, &num_records));
  }
  catch (exception& e)
  {
    fprintf(stderr,"ERROR: %s\n", e.what());
    return FALSE;
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_tell()\n");
    return FALSE;
  }

  // read last pulse & waveform

  try
  {
    fwifc_execute(
      fwifc_read(
          (fwifc_file)fwifc_handle,
          &time_sorg,
          &time_external,
          &origin[0],
          &direction[0],
          &flags,
          &facet,
          &sbl_count,
          &sbl_size,
          &psbl_first
      )
    );
  }
  catch (exception& e)
  {
    fprintf(stderr,"ERROR: %s\n", e.what());
    return FALSE;
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_read(0xFFFFFFFF)\n");
    return FALSE;
  }

  if (sbl_count)
  {

#ifdef HAVE_POF_FILE_FOR_SDF_FILE

    // convert to WGS 1984

    if (!get_origin_and_direction(time_external, origin, direction))
    {
      fprintf(stderr,"ERROR: in get_origin_and_direction() for pulse(0xFFFFFFFF) at time %g\n", time_external);
    }
    else
    {
      if (header.min_x > origin[0]) header.min_x = origin[0];
      else if (header.max_x < origin[0]) header.max_x = origin[0];

      if (header.min_y > origin[1]) header.min_y = origin[1];
      else if (header.max_y < origin[1]) header.max_y = origin[1];

      if (header.min_z > origin[2]) header.min_z = origin[2];
      else if (header.max_z < origin[2]) header.max_z = origin[2];
    }

#else // HAVE_POF_FILE_FOR_SDF_FILE

    if (header.min_x > origin[0]) header.min_x = origin[0];
    else if (header.max_x < origin[0]) header.max_x = origin[0];

    if (header.min_y > origin[1]) header.min_y = origin[1];
    else if (header.max_y < origin[1]) header.max_y = origin[1];

    if (header.min_z > origin[2]) header.min_z = origin[2];
    else if (header.max_z < origin[2]) header.max_z = origin[2];

#endif // HAVE_POF_FILE_FOR_SDF_FILE

  }

  // prepare the header

  memset(header.system_identifier, 0, PULSEWAVES_DESCRIPTION_SIZE);
  memset(header.generating_software, 0, PULSEWAVES_DESCRIPTION_SIZE);
  sprintf(header.system_identifier, "created by PULSEreaderSDF");
  sprintf(header.generating_software, "PulseWaves %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);

//  header.file_creation_day = lasreader->header.file_creation_day;
//  header.file_creation_year = lasreader->header.file_creation_year;

  num_records = 500000; // DDDDDDDDDDDDDEEEEEEEEEEEEEBBBBUUUUG  DEBUG

  header.number_of_pulses = num_records;

  header.x_scale_factor = 0.01;
  header.y_scale_factor = 0.01;
  header.z_scale_factor = 0.01;

  header.x_offset = I32_QUANTIZE((header.min_x+header.max_x)/200000)*100000;
  header.y_offset = I32_QUANTIZE((header.min_y+header.max_y)/200000)*100000;
  header.z_offset = I32_QUANTIZE((header.min_z+header.max_z)/200000)*100000;

  npulses = header.number_of_pulses;
  p_count = 0;

  pulse.init(&header);

  // seek back to first waveform

  try
  {
    fwifc_execute(fwifc_seek((fwifc_file)fwifc_handle, 1));
  }
  catch (exception& e)
  {
    fprintf(stderr,"ERROR: %s\n", e.what());
    return FALSE;
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_seek(1)\n");
    return FALSE;
  }

  // create scanner

  PULSEscanner scanner;
  strncpy(scanner.instrument, instrument, PULSEWAVES_DESCRIPTION_SIZE);
  strncpy(scanner.serial, serial, PULSEWAVES_DESCRIPTION_SIZE);
  scanner.wave_length = 1064.0f;                 // [nanometer]
  scanner.outgoing_pulse_width = 4.0f;           // [nanoseconds]
  scanner.beam_diameter_at_exit_aperture = 0.0f; // [millimeters]
  scanner.beam_divergence = 0.0f;                // [milliradians]

  header.add_scanner(&scanner, 1, TRUE);

  // init pulse descriptor

  I32 no_samplings = 0;

  composition.optical_center_to_anchor_point = 0; // the optical center and the anchor point are identical
  composition.number_of_extra_waves_bytes = 0;
  composition.sample_units = (F32)(sampling_time/1e-9); // [nanoseconds]
  composition.scanner_index = 1;

  sprintf(composition.description, "Riegl %s %s", instrument, serial);

  sbf_sampling_time = sampling_time;

  // loop over all waveforms, find all pulse descriptors, and update the bounding box

  while (num_records)
  {
    num_records--;

    try
    {
      fwifc_execute(
        fwifc_read(
            (fwifc_file)fwifc_handle,
            &time_sorg,
            &time_external,
            &origin[0],
            &direction[0],
            &flags,
            &facet,
            &sbl_count,
            &sbl_size,
            &psbl_first
        )
      );
    }
    catch (exception& e)
    {
      fprintf(stderr,"ERROR: %s\n", e.what());
      return FALSE;
    }
    catch (...)
    {
      fprintf(stderr,"ERROR: unknown exception in fwifc_read(all)\n");
      return FALSE;
    }

    if (sbl_count)
    {

#ifdef HAVE_POF_FILE_FOR_SDF_FILE

      // convert to WGS 1984

      if (!get_origin_and_direction(time_external, origin, direction))
      {
        fprintf(stderr,"WARNING: in get_origin_and_direction() for pulse (%u) at time %g\n", (U32)(npulses-num_records), time_external);
      }
      else
      {
        if (header.min_x > origin[0]) header.min_x = origin[0];
        else if (header.max_x < origin[0]) header.max_x = origin[0];

        if (header.min_y > origin[1]) header.min_y = origin[1];
        else if (header.max_y < origin[1]) header.max_y = origin[1];

        if (header.min_z > origin[2]) header.min_z = origin[2];
        else if (header.max_z < origin[2]) header.max_z = origin[2];
      }

#else // HAVE_POF_FILE_FOR_SDF_FILE

      if (header.min_x > origin[0]) header.min_x = origin[0];
      else if (header.max_x < origin[0]) header.max_x = origin[0];

      if (header.min_y > origin[1]) header.min_y = origin[1];
      else if (header.max_y < origin[1]) header.max_y = origin[1];

      if (header.min_z > origin[2]) header.min_z = origin[2];
      else if (header.max_z < origin[2]) header.max_z = origin[2];

#endif // HAVE_POF_FILE_FOR_SDF_FILE

    }

    // first iteration over sample blocks

    fwifc_sbl_t* psbl = psbl_first;
    U32 sbi, ref_number = 0, low_number = 0, high_number = 0;
    for (sbi = 0; sbi < sbl_count; sbi++)
    {
      if (psbl->channel != 2) // no SHP
      {
        if (psbl->channel == 3) // Ref
        {
          ref_number++;
        }
        else if (psbl->channel == 1) // LP
        {
          low_number++;
        }
        else if (psbl->channel == 0) // HP
        {
          high_number++;
        }
      }
      psbl++;
    }

    composition.number_of_samplings = ref_number+low_number+high_number;

    if (composition.number_of_samplings == 0)
    {
      if (!no_samplings) fprintf(stderr,"WARNING: found pulse without samplings\n");
      no_samplings++;
      continue;
    }

    if (low_number == 0)
    {
      if (high_number == 0)
      {
        fprintf(stderr,"WARNING: no return samplings in pulse\n");
      }
      else
      {
        fprintf(stderr,"WARNING: only high return samplings in pulse\n");
      }
    }

    // second iteration over sample blocks

    I32 i = 0;
    psbl = psbl_first;
    for (sbi = 0; sbi < sbl_count; ++sbi)
    {
      if (psbl->channel != 2) // no SHP
      {
        samplings[i].bits_for_duration_from_anchor = 32;
        samplings[i].bits_for_number_of_segments = 0;                  // the number of segments is fixed as there is just one segment per sampling
        samplings[i].bits_for_number_of_samples = 0;                   // the number of samples per segment is fixed for the one segment
        samplings[i].number_of_segments = 1;                           // the fixed number of segments per sampling
        samplings[i].number_of_samples = psbl->sample_count;           // the fixed number of samples
        samplings[i].bits_per_sample = 8*psbl->sample_size;            // the fixed number of bits per sample
        samplings[i].lookup_table_index = PULSEWAVES_UNDEFINED;        // the index to the optional lookup table translating sample values to physical measurements
        samplings[i].sample_units = composition.sample_units;          // [nanoseconds]

        switch (psbl->channel)
        {
          case 3: // Ref
            samplings[i].type = PULSEWAVES_OUTGOING;
            samplings[i].channel = 0;
            samplings[i].scale_for_duration_from_anchor = 1.0f;
            sprintf(samplings[i].description, "ref (%d at %d bits)", samplings[i].number_of_samples, samplings[i].bits_per_sample);
            break;
          case 1: // LP
            samplings[i].type = PULSEWAVES_RETURNING;
            samplings[i].channel = 0;
            samplings[i].scale_for_duration_from_anchor = 0.1f;
            sprintf(samplings[i].description, "low (%d samples at %d bits)", samplings[i].number_of_samples, samplings[i].bits_per_sample);
            break;
          case 0: // HP
            samplings[i].type = PULSEWAVES_RETURNING;
            samplings[i].channel = 1;
            samplings[i].scale_for_duration_from_anchor = 0.1f;
            sprintf(samplings[i].description, "high (%d samples at %d bits)", samplings[i].number_of_samples, samplings[i].bits_per_sample);
            break;
        }
        // psbl->time_sosbl is the internal time stamp of the sample block
        // (= 1st sample)
        // to calculate this time stamp as external time use:
        // time_external + (time_sosbl - time_sorg)
        // in this example we calculate the sample indices starting at
        // time_sorg for axis X:
  //      first = (psbl->time_sosbl - time_sorg) / sampling_time;
        // iterate sample values of block (si = 0 to psbl->sample_count-1):
        /*
        fwifc_sample_t* ps = psbl->sample;
        U32 si;
        for (si = 0; si < psbl->sample_count; ++si)
        {
          fwifc_sample_t sample_value = *(ps);
          ps++;
  //        cout << (first+si) << " " << sample_value << endl;
        }
        */
        i++;
      }
      psbl++;
    }

    if (header.find_descriptor(&composition, samplings) == 0)
    {
      fprintf(stderr, "samplings: %d ref: %d low: %d high: %d\n", composition.number_of_samplings, ref_number, low_number, high_number);
      header.add_descriptor_assign_index(&composition, samplings, TRUE);
    }
  }

  // seek (one final time) back to first waveform

  try
  {
    fwifc_execute(fwifc_seek((fwifc_file)fwifc_handle, 1));
  }
  catch (exception& e)
  {
    fprintf(stderr,"ERROR: %s\n", e.what());
    return FALSE;
  }
  catch (...)
  {
    fprintf(stderr,"ERROR: unknown exception in fwifc_seek(1)\n");
    return FALSE;
  }

  if (no_samplings)
  {
    fprintf(stderr,"WARNING: found %d pulse(s) without samplings\n", no_samplings);
  }

  header_is_populated = FALSE;

    // transformation matrix: SOCS to body frame adapted from A. Roncat's matlab code

  F64 boresight_angles[3] = {0,0,0};

  F64 sin_theta_X = sin(boresight_angles[0]);
  F64 cos_theta_X = cos(boresight_angles[0]);
  F64 sin_theta_Y = sin(boresight_angles[1]);
  F64 cos_theta_Y = cos(boresight_angles[1]);
  F64 sin_theta_Z = sin(boresight_angles[2]);
  F64 cos_theta_Z = cos(boresight_angles[2]);

  Rb_SOCS[0][0] = cos_theta_Y*cos_theta_Z;
  Rb_SOCS[0][1] = -cos_theta_X*sin_theta_Z+ sin_theta_X*sin_theta_Y*cos_theta_Z;
  Rb_SOCS[0][2] = sin_theta_X*sin_theta_Z + cos_theta_X*sin_theta_Y*cos_theta_Z;

  Rb_SOCS[1][0] = cos_theta_Y*sin_theta_Z;
  Rb_SOCS[1][1] = cos_theta_X*cos_theta_Z+ sin_theta_X*sin_theta_Y*sin_theta_Z;
  Rb_SOCS[1][2] = -sin_theta_X*cos_theta_Z + cos_theta_X*sin_theta_Y*sin_theta_Z;

  Rb_SOCS[2][0] = -sin_theta_Y;
  Rb_SOCS[2][1] = sin_theta_X*cos_theta_Y;
  Rb_SOCS[2][2] = cos_theta_X*cos_theta_Y;

  return TRUE;
}

I32 PULSEreaderSDF::get_format() const
{
  return PULSEWAVES_FORMAT_SDF;
}

BOOL PULSEreaderSDF::seek(const I64 p_index)
{
  if (p_index < npulses)
  {
    try
    {
      fwifc_execute(fwifc_seek((fwifc_file)fwifc_handle, (fwifc_uint32_t)p_index+1));
    }
    catch (exception& e)
    {
      fprintf(stderr,"ERROR: %s\n", e.what());
      return FALSE;
    }
    catch (...)
    {
      fprintf(stderr,"ERROR: unknown exception in fwifc_seek(%u)\n",(U32)p_index+1);
      return FALSE;
    }
    p_count = p_index;
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderSDF::read_pulse_default()
{
  if (p_count < npulses)
  {
    while (true)
    {
      fwifc_float64_t time_sorg;      /* start of range gate in s */
      fwifc_float64_t time_external;  /* external time in s relative to epoch */
      fwifc_float64_t origin[3];      /* origin vector in m */
      fwifc_float64_t direction[3];   /* direction vector (dimensionless) */
      fwifc_uint16_t  flags;          /* GPS synchronized, ... */
      fwifc_uint16_t  facet;          /* facet number (0 to num_facets-1) */
      fwifc_uint32_t  sbl_count;      /* number of sample blocks */
      fwifc_uint32_t  sbl_size;       /* size of sample block in bytes */
      fwifc_sbl_t*    psbl_first;     /* pointer to first sample block */

      // read pulse & waveform

      try
      {
        fwifc_execute(
          fwifc_read(
              (fwifc_file)fwifc_handle,
              &time_sorg,
              &time_external,
              &origin[0],
              &direction[0],
              &flags,
              &facet,
              &sbl_count,
              &sbl_size,
              &psbl_first
          )
        );
      }
      catch (exception& e)
      {
        fprintf(stderr,"ERROR in fwifc_read(): %s\n", e.what());
        fprintf(stderr,"WARNING: end-of-file after %u of %u pulses\n", (U32)p_count, (U32)npulses);
        return FALSE;
      }
      catch (...)
      {
        fprintf(stderr,"ERROR in fwifc_read()\n");
        fprintf(stderr,"WARNING: end-of-file after %u of %u pulses\n", (U32)p_count, (U32)npulses);
        return FALSE;
      }

      if (sbl_count)
      {

#ifdef HAVE_POF_FILE_FOR_SDF_FILE

        // convert to WGS 1984

        if (!get_origin_and_direction(time_external, origin, direction))
        {
          fprintf(stderr,"WARNING: in get_origin_and_direction() for pulse (%u) at time %g\n", (U32)p_count, time_external);
          continue;
        }

#endif // HAVE_POF_FILE_FOR_SDF_FILE

        // first iteration over sample blocks

        fwifc_sbl_t* psbl = psbl_first;
        U32 sbi, ref_number = 0, low_number = 0, high_number = 0;
        for (sbi = 0; sbi < sbl_count; sbi++)
        {
          if (psbl->channel != 2) // no SHP
          {
            if (psbl->channel == 3) // Ref
            {
              ref_number++;
            }
            else if (psbl->channel == 1) // LP
            {
              low_number++;
            }
            else if (psbl->channel == 0) // HP
            {
              high_number++;
            }
          }
          psbl++;
        }

        composition.number_of_samplings = ref_number+low_number+high_number;

        if (composition.number_of_samplings == 0)
        {
          fprintf(stderr,"WARNING: found pulse with only SHP samplings\n");
        }

        if (low_number == 0)
        {
          if (high_number == 0)
          {
            fprintf(stderr,"WARNING: no return samplings in pulse\n");
          }
          else
          {
            fprintf(stderr,"WARNING: only high return samplings in pulse\n");
          }
        }

        // second iteration over sample blocks

        I32 i = 0;
        F64 time_first = F32_MAX;
        F64 time_last = F32_MIN;

        psbl = psbl_first;
        for (sbi = 0; sbi < sbl_count; ++sbi)
        {
          if (psbl->channel != 2) // no SHP
          {
            samplings[i].bits_for_duration_from_anchor = 32;
            samplings[i].bits_for_number_of_segments = 0;                  // the number of segments is fixed as there is just one segment per sampling
            samplings[i].bits_for_number_of_samples = 0;                   // the number of samples per segment is fixed for the one segment
            samplings[i].number_of_segments = 1;                           // the fixed number of segments per sampling
            samplings[i].number_of_samples = psbl->sample_count;           // the fixed number of samples
            samplings[i].bits_per_sample = 8*psbl->sample_size;            // the fixed number of bits per sample
            samplings[i].lookup_table_index = PULSEWAVES_UNDEFINED;        // the index to the optional lookup table translating sample values to physical measurements
            samplings[i].sample_units = composition.sample_units;          // [nanoseconds]
            samplings[i].compression = PULSEWAVES_UNCOMPRESSED;            // the samples are stored without compression

            switch (psbl->channel)
            {
              case 3: // Ref
                samplings[i].type = PULSEWAVES_OUTGOING;
                samplings[i].channel = 0;
                samplings[i].scale_for_duration_from_anchor = 1.0f;
                break;
              case 1: // LP
                samplings[i].type = PULSEWAVES_RETURNING;
                samplings[i].channel = 0;
                samplings[i].scale_for_duration_from_anchor = 0.1f;
                break;
              case 0: // HP
                samplings[i].type = PULSEWAVES_RETURNING;
                samplings[i].channel = 1;
                samplings[i].scale_for_duration_from_anchor = 0.1f;
                break;
            }
            if (psbl->channel < 2)
            {
              if (psbl->time_sosbl < time_first) time_first = psbl->time_sosbl;
              if ((psbl->time_sosbl + sbf_sampling_time*psbl->sample_count) > time_last) time_last = psbl->time_sosbl + sbf_sampling_time*psbl->sample_count;
            }
            i++;
          }
          psbl++;
        }

        direction[0] = origin[0] + direction[0]*1000;
        direction[1] = origin[1] + direction[1]*1000;
        direction[2] = origin[2] + direction[2]*1000;

        pulse.set_T(time_external);
        pulse.offset = 0;
        pulse.set_anchor_and_target(origin, direction);
        pulse.descriptor_index = header.find_descriptor(&composition, samplings);
        pulse.edge_of_scan_line = 0;
        pulse.scan_direction = 0;
        pulse.mirror_facet = (facet & 3);
        pulse.first_returning_sample = I16_QUANTIZE((time_first-time_sorg)/sbf_sampling_time);
        pulse.last_returning_sample = I16_QUANTIZE((time_last-time_sorg)/sbf_sampling_time);

        sdf_first_sample = psbl_first;
        sbf_sample_count = sbl_count;
        sbf_time_sorg = time_sorg;

        p_count++;
        return TRUE;
      }
    }
  }
  return FALSE;
}

BOOL PULSEreaderSDF::read_waves()
{
  if (sdf_first_sample == 0)
  {
    fprintf(stderr,"ERROR: have no pulse to read waves for\n");
    return FALSE;
  }
  if (waves_map[pulse.descriptor_index] == 0)
  {
    waves_map[pulse.descriptor_index] = new WAVESwaves();
    if (waves_map[pulse.descriptor_index] == 0)
    {
      fprintf(stderr,"ERROR: cannot allocate WAVESwaves\n");
      return FALSE;
    }
    if (!waves_map[pulse.descriptor_index]->init(header.get_descriptor(pulse.descriptor_index)))
    {
      fprintf(stderr,"ERROR: cannot init WAVESwaves\n");
      return FALSE;
    }
  }
  waves = waves_map[pulse.descriptor_index];

  fwifc_sbl_t* psbl = (fwifc_sbl_t*)sdf_first_sample;
  U32 i, sbf;
  for (i = sbf = 0; sbf < sbf_sample_count; ++sbf)
  {
    if (psbl->channel != 2) // no SHP
    {
      waves->get_sampling(i)->set_duration_from_anchor_for_segment((F32)((psbl->time_sosbl-sbf_time_sorg)/sbf_sampling_time));
      memcpy(waves->get_sampling(i)->get_samples(), psbl->sample, waves->get_sampling(i)->size());
      i++;
    }
    psbl++;
  }
  return TRUE;
}

ByteStreamIn* PULSEreaderSDF::get_pulse_stream() const
{
  return 0;
}

ByteStreamIn* PULSEreaderSDF::get_waves_stream() const
{
  return 0;
}

void PULSEreaderSDF::close(BOOL close_stream)
{
  if (fwifc_handle)
  {
    try
    {
      fwifc_execute(fwifc_close((fwifc_file)fwifc_handle));
    }
    catch (exception& e)
    {
      fprintf(stderr,"ERROR: %s\n", e.what());
    }
    catch (...)
    {
      fprintf(stderr,"ERROR: unknown exception in fwifc_close()\n");
    }
    fwifc_handle = 0;
  }
  for (I32 i = 0; i < 256; i++)
  {
    if (waves_map[i])
    {
      delete waves_map[i];
      waves_map[i] = 0;
    }
  }

#ifdef HAVE_POF_FILE_FOR_SDF_FILE

  if (pofifc_handle)
  {
    try
    {
//      pofifc_execute(pofifc_close((pofifc_file_t)pofifc_handle));
      if (pofifc_close((pofifc_file_t)pofifc_handle)) throw 4711;
    }
    catch (exception& e)
    {
      fprintf(stderr,"ERROR: %s\n", e.what());
    }
    catch (...)
    {
      fprintf(stderr,"ERROR: unknown exception in pofifc_close()\n");
    }
    pofifc_handle = 0;
  }

#endif // HAVE_POF_FILE_FOR_SDF_FILE

}

PULSEreaderSDF::PULSEreaderSDF()
{
  fwifc_handle = 0;
  fwifc_error = FWIFC_NO_ERROR;
  sdf_first_sample = 0;
  sbf_sample_count = 0;
  sbf_time_sorg = 0;
  memset(waves_map, 0, sizeof(WAVESwaves*)*256);
#ifdef HAVE_POF_FILE_FOR_SDF_FILE
  pofifc_handle = 0;
#endif // HAVE_POF_FILE_FOR_SDF_FILE
}

PULSEreaderSDF::~PULSEreaderSDF()
{
  if (fwifc_handle) close();
}

/*
I32 PULSEreaderSDF::add_pulsedescriptor(void* psbl_first, I32 sbl_number)
{
  fwifc_sbl_t* psbl = (fwifc_sbl_t*)psbl_first;
  U32 u, ref_number = 0, low_number = 0, high_number = 0;
  for (u = 0; u < sbl_number; u++)
  {
    if (psbl->channel != 2) // no SHP
    {
      if (psbl->channel == 3) // Ref
      {
        ref_number++;
      }
      else if (psbl->channel == 1) // LP
      {
        low_number++;
      }
      else if (psbl->channel == 0) // HP
      {
        high_number++;
      }
    }
    psbl++;
  }

    if ((ref_number+low_number+high_number) == 0)
    {
      if (!no_samplings) fprintf(stderr,"WARNING: found pulse without samplings\n");
      no_samplings++;
    }
    else
    {
      if (low_number == 0)
      {
        if (high_number == 0)
        {
          fprintf(stderr,"WARNING: no return samplings in pulse\n");
        }
        else
        {
         fprintf(stderr,"WARNING: only high return samplings in pulse\n");
        }
      }
      composition.num_samplings = ref_number+low_number+high_number;
    }

    // second iteration over sample blocks

    I32 i = 0, ref_count = 0, low_count = 0, high_count = 0;
    psbl = psbl_first;
    for (sbi = 0; sbi < sbl_count; ++sbi)
    {
      if (psbl->channel != 2) // no SHP
      {
        samplings[i].bits_for_duration_from_anchor = 16;
        samplings[i].bits_per_sample = 8*psbl->sample_size;
        samplings[i].number_of_samples =  psbl->sample_count;
        samplings[i].sample_units = 1000000; // [attoseconds] 1 picoseconds = 1000000 attoseconds
        samplings[i].digitizer_gain = 0; // [Volt]
        samplings[i].digitizer_offset = 0; // [Volt]

        switch (psbl->channel)
        {
          case 3: // Ref
            samplings[i].type = PULSEWAVES_OUTGOING;
            samplings[i].fractional_bits_for_duration_from_anchor = 0;
            samplings[i].channel = 0;
            samplings[i].segment = ref_count;
            ref_count++;
            sprintf(samplings[i].description, "ref %d (%d at %d bits)", samplings[i].segment, samplings[i].number_of_samples, samplings[i].bits_per_sample);
            break;
          case 1: // LP
            samplings[i].type = PULSEWAVES_RETURNING;
            samplings[i].fractional_bits_for_duration_from_anchor = 8;
            samplings[i].channel = 0;
            samplings[i].segment = low_count;
            low_count++;
            sprintf(samplings[i].description, "low %d (%d samples at %d bits)", samplings[i].segment, samplings[i].number_of_samples, samplings[i].bits_per_sample);
            break;
          case 0: // HP
            samplings[i].type = PULSEWAVES_RETURNING;
            samplings[i].fractional_bits_for_duration_from_anchor = 8;
            samplings[i].channel = 1;
            samplings[i].segment = high_count;
            high_count++;
            sprintf(samplings[i].description, "high %d (%d samples at %d bits)", samplings[i].segment, samplings[i].number_of_samples, samplings[i].bits_per_sample);
            break;
        }
        // psbl->time_sosbl is the internal time stamp of the sample block
        // (= 1st sample)
        // to calculate this time stamp as external time use:
        // time_external + (time_sosbl - time_sorg)
        // in this example we calculate the sample indices starting at
        // time_sorg for axis X:
  //      first = (psbl->time_sosbl - time_sorg) / sampling_time;
        // iterate sample values of block (si = 0 to psbl->sample_count-1):
        fwifc_sample_t* ps = psbl->sample;
        U32 si;
        for (si = 0; si < psbl->sample_count; ++si)
        {
          fwifc_sample_t sample_value = *(ps);
          ps++;
  //        cout << (first+si) << " " << sample_value << endl;
        }
        i++;
      }
      psbl++;
    }

    if (header.get_pulsedescriptor(&composition, samplings) == -1)
    {
      fprintf(stderr, "samplings: %d ref: %d low: %d high: %d\n", composition.num_samplings, ref_number, low_number, high_number);
      header.add_pulsedescriptor(&composition, samplings);
    }*/
