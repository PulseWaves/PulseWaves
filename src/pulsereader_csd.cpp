/*
===============================================================================

  FILE:  pulsereader_csd.cpp
  
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
#include "pulsereader_csd.hpp"

#include "pulseutility.hpp"
#include "bytestreamin.hpp"
#include "bytestreamin_file.hpp"
#include "bytestreamin_istream.hpp"

#define CSD_PULSE_DESCRIPTOR_INDEX_0_SEG 1
#define CSD_PULSE_DESCRIPTOR_INDEX_1_SEG 2
#define CSD_PULSE_DESCRIPTOR_INDEX_2_SEG 3
#define CSD_PULSE_DESCRIPTOR_INDEX_3_SEG 4

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DISTANCE_LIGHT_TRAVELS_IN_ONE_NANOSECOND 0.299792459

static inline BOOL valid_lonlat(F64 lon, F64 lat)
{
  if (lon < -360.0 || lon > 360.0 || lat < -180.0 || lat > 180.0)
  {
    return FALSE;
  }
  return TRUE;
};

inline void VecScale3dv(F64 v[3], const F64 scale, const F64 a[3])
{
  v[0] = scale * a[0];
  v[1] = scale * a[1];
  v[2] = scale * a[2];
}

static inline void VecAdd3dv(F64 v[3], const F64 a[3], const F64 b[3])
{
  v[0] = a[0] + b[0];
  v[1] = a[1] + b[1];
  v[2] = a[2] + b[2];
}

static inline void VecUpdateMinMax3dv(F64 min[3], F64 max[3], const F64 v[3])
{
  if (v[0]<min[0]) min[0]=v[0]; else if (v[0]>max[0]) max[0]=v[0];
  if (v[1]<min[1]) min[1]=v[1]; else if (v[1]>max[1]) max[1]=v[1];
  if (v[2]<min[2]) min[2]=v[2]; else if (v[2]>max[2]) max[2]=v[2];
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

static const F64 PI = 3.141592653589793238462643383279502884197169;

BOOL PULSEpulseCSD::get_pos(F64 pos[3], I32 range_index, const F64 Rb_SOCS[3][3]) const
{
  if (range_index < 0) return FALSE;
  if (range_index > 3) return FALSE;
  if (ranges[range_index] == 0.0f) return FALSE;

  F64 alpha = scan_angle;
  F64 X_SOCS[3];

  // SOCS coordinates adapted from A. Roncat's matlab code

  //  (a) definition of Master Yoda aka Joe Liadsky
  //  X_SOCS = [0; sin(alpha); cos(alpha)] * (csdRecord.Ranges(1:numEchoes)');
        
  //  (b) own definition following ChR's PhD thesis
  //  X_SOCS = [0; -sin(alpha); -cos(alpha)] * (csdRecord.Ranges(1:numEchoes)');
        
  //  (c) Riegl definition
  X_SOCS[0] = sin(alpha);
  X_SOCS[1] = 0;
  X_SOCS[2] = -cos(alpha) * ranges[range_index];

  F64 temp_0[3];
  MatVecMult33dv(temp_0, Rb_SOCS, X_SOCS);
  F64 temp_1[3];
  MatVecMult33dv(temp_1, Rl_b, temp_0);
  F64 temp_2[3];
  MatVecMult33dv(temp_2, Re_l, temp_1);
  VecAdd3dv(pos, origin, temp_2);

  return TRUE;
}

BOOL PULSEpulseCSD::get_dir(F64 dir[3], const F64 Rb_SOCS[3][3]) const
{
  F64 alpha = scan_angle;
  F64 X_SOCS[3];

  // SOCS coordinates adapted from A. Roncat's matlab code

  //  (a) definition of Master Yoda aka Joe Liadsky
  //  X_SOCS = [0; sin(alpha); cos(alpha)] * (csdRecord.Ranges(1:numEchoes)');
        
  //  (b) own definition following ChR's PhD thesis
  //  X_SOCS = [0; -sin(alpha); -cos(alpha)] * (csdRecord.Ranges(1:numEchoes)');
        
  //  (c) Riegl definition
  X_SOCS[0] = sin(alpha);
  X_SOCS[1] = 0;
  X_SOCS[2] = -cos(alpha);

  F64 temp_0[3];
  MatVecMult33dv(temp_0, Rb_SOCS, X_SOCS);
  F64 temp_1[3];
  MatVecMult33dv(temp_1, Rl_b, temp_0);
  F64 temp_2[3];
  MatVecMult33dv(temp_2, Re_l, temp_1);

  VecScale3dv(dir, 0.5 * DISTANCE_LIGHT_TRAVELS_IN_ONE_NANOSECOND, temp_2);
  return TRUE;
}

BOOL PULSEreaderCSD::read_pulse_csd()
{
  try { pulse_stream->get64bitsLE((U8*)&pulsecsd.GPS_time); } catch(...)
  {
    fprintf(stderr,"ERROR: reading GPS_time\n");
    return FALSE;
  }
  try { pulsecsd.pulse_count = pulse_stream->getByte(); } catch(...)
  {
    fprintf(stderr,"ERROR: reading pulse_count\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&(pulsecsd.ranges[0])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading ranges[0]\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&(pulsecsd.ranges[1])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading ranges[1]\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&(pulsecsd.ranges[2])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading ranges[2]\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&(pulsecsd.ranges[3])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading ranges[3]\n");
    return FALSE;
  }
  try { pulse_stream->get16bitsLE((U8*)&(pulsecsd.intensities[0])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading intensities[0]\n");
    return FALSE;
  }
  try { pulse_stream->get16bitsLE((U8*)&(pulsecsd.intensities[1])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading intensities[1]\n");
    return FALSE;
  }
  try { pulse_stream->get16bitsLE((U8*)&(pulsecsd.intensities[2])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading intensities[2]\n");
    return FALSE;
  }
  try { pulse_stream->get16bitsLE((U8*)&(pulsecsd.intensities[3])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading intensities[3]\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&pulsecsd.scan_angle); } catch(...)
  {
    fprintf(stderr,"ERROR: reading scan_angle\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&pulsecsd.roll); } catch(...)
  {
    fprintf(stderr,"ERROR: reading roll\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&pulsecsd.pitch); } catch(...)
  {
    fprintf(stderr,"ERROR: reading pitch\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&pulsecsd.heading); } catch(...)
  {
    fprintf(stderr,"ERROR: reading heading\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&pulsecsd.latitude); } catch(...)
  {
    fprintf(stderr,"ERROR: reading latitude\n");
    return FALSE;
  }
  pulsecsd.latitude_degrees = pulsecsd.latitude * 180.0 / PI;
  try { pulse_stream->get64bitsLE((U8*)&pulsecsd.longitude); } catch(...)
  {
    fprintf(stderr,"ERROR: reading longitude\n");
    return FALSE;
  }
  pulsecsd.longitude_degrees = pulsecsd.longitude * 180.0 / PI;
  try { pulse_stream->get32bitsLE((U8*)&pulsecsd.elevation); } catch(...)
  {
    fprintf(stderr,"ERROR: reading elevation\n");
    return FALSE;
  }

  F64 sin_phi = sin(pulsecsd.latitude);
  F64 cos_phi = cos(pulsecsd.latitude);
  F64 sin_lambda = sin(pulsecsd.longitude);
  F64 cos_lambda = cos(pulsecsd.longitude);
  F64 N = 6378137.0 / sqrt(1.0 - (0.006694380004260827 * sin_phi * sin_phi));

  // Chris Parrish's definition adapted from A. Roncat's matlab code
  F64 roll = pulsecsd.roll + IMU_offsets[0];
  F64 pitch = pulsecsd.pitch + IMU_offsets[1];
  F64 heading = pulsecsd.heading + IMU_offsets[2];

  pulsecsd.origin[0] = (N + pulsecsd.elevation) * cos_phi * cos_lambda;
  pulsecsd.origin[1] = (N + pulsecsd.elevation) * cos_phi * sin_lambda;
  pulsecsd.origin[2] = ((N * (1.0 - 0.006694380004260827)) + pulsecsd.elevation) * sin_phi;

  // transformation matrices: local level to earth-fixed, body to local level
  pulsecsd.Re_l[0][0] = -sin_lambda;
  pulsecsd.Re_l[0][1] = -cos_lambda*sin_phi;
  pulsecsd.Re_l[0][2] = cos_lambda*cos_phi;
  pulsecsd.Re_l[1][0] = cos_lambda;
  pulsecsd.Re_l[1][1] = -sin_lambda*sin_phi;
  pulsecsd.Re_l[1][2] = sin_lambda*cos_phi;
  pulsecsd.Re_l[2][0] = 0;
  pulsecsd.Re_l[2][1] = cos_phi;
  pulsecsd.Re_l[2][2] = sin_phi;

  // Chis Parrish's variant adapted from A. Roncat's matlab code

  F64 rotMatrixMr[3][3];
  rotMatrixMr[0][0] = cos(pitch)*sin(heading);
  rotMatrixMr[0][1] = -1*cos(heading)*cos(roll)-sin(heading)*sin(pitch)*sin(roll);
  rotMatrixMr[0][2] = cos(heading)*sin(roll)-sin(heading)*sin(pitch)*cos(roll);
  rotMatrixMr[1][0] = cos(pitch)*cos(heading);
  rotMatrixMr[1][1] = sin(heading)*cos(roll)-cos(heading)*sin(pitch)*sin(roll);
  rotMatrixMr[1][2] = -1*sin(heading)*sin(roll)-cos(heading)*sin(pitch)*cos(roll);
  rotMatrixMr[2][0] = sin(pitch);
  rotMatrixMr[2][1] = cos(pitch)*sin(roll);
  rotMatrixMr[2][2] = cos(pitch)*cos(roll);

  F64 RieglToOptech[3][3];
  RieglToOptech[0][0] = 0;
  RieglToOptech[0][1] = 1;
  RieglToOptech[0][2] = 0;
  RieglToOptech[1][0] = -1;
  RieglToOptech[1][1] = 0;
  RieglToOptech[1][2] = 0;
  RieglToOptech[2][0] = 0;
  RieglToOptech[2][1] = 0;
  RieglToOptech[2][2] = 1;

  F64 temp[3][3];

  MatMatMult33dv(temp, rotMatrixMm, RieglToOptech);
  MatMatMult33dv(pulsecsd.Rl_b, rotMatrixMr, temp);

  return TRUE;
};

BOOL PULSEreaderCSD::open(const CHAR* file_name, U32 io_buffer_size)
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

BOOL PULSEreaderCSD::open()
{
  BOOL long_lat_coordinates = FALSE;

  // clean the header

  header.clean();
 
  // set some parameters

  memset(header.system_identifier, 0, PULSEWAVES_DESCRIPTION_SIZE);
  memset(header.generating_software, 0, PULSEWAVES_DESCRIPTION_SIZE);
  sprintf(header.system_identifier, "created by PULSEreaderCSD");
  sprintf(header.generating_software, "PulseWaves %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);

  header.file_creation_day = 333;
  header.file_creation_year = 2012;

  // read the CSD header variable after variable (to avoid alignment issues)

  CHAR signature[4];
  try { pulse_stream->getBytes((U8*)signature, 4); } catch(...)
  {
    fprintf(stderr,"ERROR: reading signature\n");
    return FALSE;
  }
  if (strcmp(signature, "CSD") != 0)
  {
    fprintf(stderr,"ERROR: wrong signature: %4s\n", signature);
    return FALSE;
  }
  CHAR vendor_id[64];
  try { pulse_stream->getBytes((U8*)vendor_id, 64); } catch(...)
  {
    fprintf(stderr,"ERROR: reading vendor_id\n");
    return FALSE;
  }
  if (strcmp(vendor_id, "Optech Incorporated") != 0)
  {
    fprintf(stderr,"WARNING: vendor_id: %s\n", vendor_id);
  }
  CHAR software_version[32];
  try { pulse_stream->getBytes((U8*)software_version, 32); } catch(...)
  {
    fprintf(stderr,"ERROR: reading software_version\n");
    return FALSE;
  }
  F32 format_version;
  try { pulse_stream->get32bitsLE((U8*)&(format_version)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading format_version\n");
    return FALSE;
  }
  I16 header_size;
  try { pulse_stream->get16bitsLE((U8*)&(header_size)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header_size\n");
    return FALSE;
  }
  try { pulse_stream->get16bitsLE((U8*)&(gps_week)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading gps_week\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(min_time)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading min_time\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(max_time)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading max_time\n");
    return FALSE;
  }
  try { pulse_stream->get32bitsLE((U8*)&(number_of_records)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_records\n");
    return FALSE;
  }
  try { pulse_stream->get16bitsLE((U8*)&(number_of_strips)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_strips\n");
    return FALSE;
  }
  try { pulse_stream->getBytes((U8*)strip_pointers, 256*4); } catch(...)
  {
    fprintf(stderr,"ERROR: reading strip_pointers[256]\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(misalignment_angles[0])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading misalignment_angles[0]\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(misalignment_angles[1])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading misalignment_angles[1]\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(misalignment_angles[2])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading misalignment_angles[2]\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(IMU_offsets[0])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading IMU_offsets[0]\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(IMU_offsets[1])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading IMU_offsets[1]\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(IMU_offsets[2])); } catch(...)
  {
    fprintf(stderr,"ERROR: reading IMU_offsets[2]\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(temperature)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading temperature\n");
    return FALSE;
  }
  try { pulse_stream->get64bitsLE((U8*)&(pressure)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading pressure\n");
    return FALSE;
  }
  U8 free_space[830];
  try { pulse_stream->getBytes((U8*)free_space, 830); } catch(...)
  {
    fprintf(stderr,"ERROR: reading free_space[830]\n");
    return FALSE;
  }

  // range calculation factor (sec -> m) adapted from A. Roncat's matlab code

  range_calculation_factor = 1.0 / (2.0 * (1.0 + 78.8 * (pressure) / (273.15 + temperature) * 1.0e-6));
  range_calculation_factor = range_calculation_factor * 299792458.0;

  // transformation matrix: SOCS to body frame adapted from A. Roncat's matlab code

  F64 sin_theta_X = sin(misalignment_angles[0]);
  F64 cos_theta_X = cos(misalignment_angles[0]);
  F64 sin_theta_Y = sin(misalignment_angles[1]);
  F64 cos_theta_Y = cos(misalignment_angles[1]);
  F64 sin_theta_Z = sin(misalignment_angles[2]);
  F64 cos_theta_Z = cos(misalignment_angles[2]);

  Rb_SOCS[0][0] = cos_theta_Y*cos_theta_Z;
  Rb_SOCS[0][1] = -cos_theta_X*sin_theta_Z+ sin_theta_X*sin_theta_Y*cos_theta_Z;
  Rb_SOCS[0][2] = sin_theta_X*sin_theta_Z + cos_theta_X*sin_theta_Y*cos_theta_Z;

  Rb_SOCS[1][0] = cos_theta_Y*sin_theta_Z;
  Rb_SOCS[1][1] = cos_theta_X*cos_theta_Z+ sin_theta_X*sin_theta_Y*sin_theta_Z;
  Rb_SOCS[1][2] = -sin_theta_X*cos_theta_Z + cos_theta_X*sin_theta_Y*sin_theta_Z;

  Rb_SOCS[2][0] = -sin_theta_Y;
  Rb_SOCS[2][1] = sin_theta_X*cos_theta_Y;
  Rb_SOCS[2][2] = cos_theta_X*cos_theta_Y;

  rotMatrixMm[0][0] = cos_theta_Y*cos_theta_Z;
  rotMatrixMm[0][1] = cos_theta_Y*sin_theta_Z;
  rotMatrixMm[0][2] = -1*sin_theta_Y;
  rotMatrixMm[1][0] = sin_theta_X*sin_theta_Y*cos_theta_Z-cos_theta_X*sin_theta_Z;
  rotMatrixMm[1][1] = sin_theta_X*sin_theta_Y*sin_theta_Z+cos_theta_X*cos_theta_Z;
  rotMatrixMm[1][2] = sin_theta_X*cos_theta_Y;
  rotMatrixMm[2][0] = cos_theta_X*sin_theta_Y*cos_theta_Z+sin_theta_X*sin_theta_Z;
  rotMatrixMm[2][1] = cos_theta_X*sin_theta_Y*sin_theta_Z-sin_theta_X*cos_theta_Z;
  rotMatrixMm[2][2] = cos_theta_X*cos_theta_Y;
  
  F64 pos[3];

  // read the first pulse 

  while (TRUE)
  {
    if (!read_pulse_csd())
    {
      fprintf(stderr,"ERROR: reading first pulse\n");
      return FALSE;
    }
  
    // use first pulse to determine some settings

    if (!pulsecsd.get_pos(pos, 0, Rb_SOCS))
    {
      fprintf(stderr,"WARNING: skipping pulse\n");
      continue;
    }
    break;
  }

  header.x_scale_factor = 0.01;
  header.y_scale_factor = 0.01;
  header.z_scale_factor = 0.01;

  header.x_offset = I32_QUANTIZE(pos[0]/100000)*100000;
  header.y_offset = I32_QUANTIZE(pos[1]/100000)*100000;
  header.z_offset = I32_QUANTIZE(pos[2]/100000)*100000;

  header.min_x = header.max_x = pos[0];
  header.min_y = header.max_y = pos[1];
  header.min_z = header.max_z = pos[2];

  if (!pulsecsd.get_pos(pos, 3, Rb_SOCS))
  {
    fprintf(stderr,"ERROR: reading range 3 of pulse %u\n", (U32)p_count);
  }
  else
  {
    if (header.min_x > pos[0]) header.min_x = pos[0];
    else if (header.max_x < pos[0]) header.max_x = pos[0];

    if (header.min_y > pos[1]) header.min_y = pos[1];
    else if (header.max_y < pos[1]) header.max_y = pos[1];

    if (header.min_z > pos[2]) header.min_z = pos[2];
    else if (header.max_z < pos[2]) header.max_z = pos[2];
  }

  // seek to the last pulse in the file

  try { pulse_stream->seekEnd(69); } catch(...)
  {
    fprintf(stderr,"ERROR: cannot seek to end of file\n");
    return FALSE;
  }

  I64 file_size = pulse_stream->tell() - 2048;

  if ( (file_size % 69) != 0 )
  {
    fprintf(stderr,"WARNING: odd file size. with data records of %d bytes there is a remainder of %d bytes\n", 69, (I32)(file_size % 69));
  }

  npulses = (file_size / 69) + 1;
  p_count = 0;

  if (number_of_records > (I32)npulses)
  {
    fprintf(stderr,"WARNING: CSD header reports %d CSD records but file contains only %d\n", number_of_records, (I32)npulses);
  }
  else if (number_of_records < (I32)npulses)
  {
    fprintf(stderr,"WARNING: CSD header reports %d CSD records but file contains as many as %d\n", number_of_records, (I32)npulses);
    npulses = number_of_records;
  }

  header.number_of_pulses = npulses;

  // use last pulse to update bounding box approximation

  if (!read_pulse_csd())
  {
    fprintf(stderr,"ERROR: reading last pulse\n");
    return FALSE;
  }

  if (!pulsecsd.get_pos(pos, 0, Rb_SOCS))
  {
    fprintf(stderr,"ERROR: reading range 0 of last pulse\n");
  }
  else
  {
    if (header.min_x > pos[0]) header.min_x = pos[0];
    else if (header.max_x < pos[0]) header.max_x = pos[0];

    if (header.min_y > pos[1]) header.min_y = pos[1];
    else if (header.max_y < pos[1]) header.max_y = pos[1];

    if (header.min_z > pos[2]) header.min_z = pos[2];
    else if (header.max_z < pos[2]) header.max_z = pos[2];
  }

  if (!pulsecsd.get_pos(pos, 3, Rb_SOCS))
  {
    fprintf(stderr,"ERROR: reading range 3 of last pulse\n");
  }
  else
  {
    if (header.min_x > pos[0]) header.min_x = pos[0];
    else if (header.max_x < pos[0]) header.max_x = pos[0];

    if (header.min_y > pos[1]) header.min_y = pos[1];
    else if (header.max_y < pos[1]) header.max_y = pos[1];

    if (header.min_z > pos[2]) header.min_z = pos[2];
    else if (header.max_z < pos[2]) header.max_z = pos[2];
  }

  // load a few more pulses to update bounding box approximation

  try { pulse_stream->seek(npulses/4*1*69 + 2048); } catch(...)
  {
    fprintf(stderr,"ERROR: seeking to intermediate pulse\n");
    return FALSE;
  }

  if (!read_pulse_csd())
  {
    fprintf(stderr,"ERROR: reading intermediate pulse\n");
    return FALSE;
  }

  if (!pulsecsd.get_pos(pos, 0, Rb_SOCS))
  {
    fprintf(stderr,"ERROR: reading range 0 of intermediate pulse\n");
  }
  else
  {
    if (header.min_x > pos[0]) header.min_x = pos[0];
    else if (header.max_x < pos[0]) header.max_x = pos[0];

    if (header.min_y > pos[1]) header.min_y = pos[1];
    else if (header.max_y < pos[1]) header.max_y = pos[1];

    if (header.min_z > pos[2]) header.min_z = pos[2];
    else if (header.max_z < pos[2]) header.max_z = pos[2];
  }

  if (!pulsecsd.get_pos(pos, 3, Rb_SOCS))
  {
    fprintf(stderr,"ERROR: reading range 3 of intermediate pulse\n");
  }
  else
  {
    if (header.min_x > pos[0]) header.min_x = pos[0];
    else if (header.max_x < pos[0]) header.max_x = pos[0];

    if (header.min_y > pos[1]) header.min_y = pos[1];
    else if (header.max_y < pos[1]) header.max_y = pos[1];

    if (header.min_z > pos[2]) header.min_z = pos[2];
    else if (header.max_z < pos[2]) header.max_z = pos[2];
  }

  try { pulse_stream->seek(npulses/4*2*69 + 2048); } catch(...)
  {
    fprintf(stderr,"ERROR: seeking to intermediate pulse\n");
    return FALSE;
  }

  if (!read_pulse_csd())
  {
    fprintf(stderr,"ERROR: reading intermediate pulse\n");
    return FALSE;
  }

  if (!pulsecsd.get_pos(pos, 0, Rb_SOCS))
  {
    fprintf(stderr,"ERROR: reading range 0 of intermediate pulse\n");
  }
  else
  {
    if (header.min_x > pos[0]) header.min_x = pos[0];
    else if (header.max_x < pos[0]) header.max_x = pos[0];

    if (header.min_y > pos[1]) header.min_y = pos[1];
    else if (header.max_y < pos[1]) header.max_y = pos[1];

    if (header.min_z > pos[2]) header.min_z = pos[2];
    else if (header.max_z < pos[2]) header.max_z = pos[2];
  }

  if (!pulsecsd.get_pos(pos, 3, Rb_SOCS))
  {
    fprintf(stderr,"ERROR: reading range 3 of intermediate pulse\n");
  }
  else
  {
    if (header.min_x > pos[0]) header.min_x = pos[0];
    else if (header.max_x < pos[0]) header.max_x = pos[0];

    if (header.min_y > pos[1]) header.min_y = pos[1];
    else if (header.max_y < pos[1]) header.max_y = pos[1];

    if (header.min_z > pos[2]) header.min_z = pos[2];
    else if (header.max_z < pos[2]) header.max_z = pos[2];
  }

  try { pulse_stream->seek(npulses/4*3*69 + 2048); } catch(...)
  {
    fprintf(stderr,"ERROR: seeking to intermediate pulse\n");
    return FALSE;
  }

  if (!read_pulse_csd())
  {
    fprintf(stderr,"ERROR: reading intermediate pulse\n");
    return FALSE;
  }

  if (!pulsecsd.get_pos(pos, 0, Rb_SOCS))
  {
    fprintf(stderr,"ERROR: reading range 0 of intermediate pulse\n");
  }
  else
  {
    if (header.min_x > pos[0]) header.min_x = pos[0];
    else if (header.max_x < pos[0]) header.max_x = pos[0];

    if (header.min_y > pos[1]) header.min_y = pos[1];
    else if (header.max_y < pos[1]) header.max_y = pos[1];

    if (header.min_z > pos[2]) header.min_z = pos[2];
    else if (header.max_z < pos[2]) header.max_z = pos[2];
  }

  if (!pulsecsd.get_pos(pos, 3, Rb_SOCS))
  {
    fprintf(stderr,"ERROR: reading range 3 of intermediate pulse\n");
  }
  else
  {
    if (header.min_x > pos[0]) header.min_x = pos[0];
    else if (header.max_x < pos[0]) header.max_x = pos[0];

    if (header.min_y > pos[1]) header.min_y = pos[1];
    else if (header.max_y < pos[1]) header.max_y = pos[1];

    if (header.min_z > pos[2]) header.min_z = pos[2];
    else if (header.max_z < pos[2]) header.max_z = pos[2];
  }

  // go back to the beginning of the stream

  try { pulse_stream->seek(2048); } catch(...)
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

  // create scanner with index 1

  PULSEscanner scanner;
  scanner.wave_length = 1064;                 // [nanometer]
  scanner.outgoing_pulse_width = 10;          // [nanoseconds]
  scanner.beam_diameter_at_exit_aperture = 0; // [millimeters]
  scanner.beam_divergence = 0;                // [milliradians]

  header.add_scanner(&scanner, 1, TRUE);

  // create pulse descriptors (composition + samplings)

  PULSEcomposition composition;
  PULSEsampling samplings[2];

  composition.optical_center_to_anchor_point = 0; // the duration from the optical center to the anchor point is zero
  composition.number_of_extra_waves_bytes = 0; 
  composition.number_of_samplings = 1;            // one outgoing only, none returning
  composition.sample_units = 1.0f;                // [nanoseconds]
  composition.scanner_index = 1;

  strncpy(composition.description, "Optech (1 outgoing only)", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[0].type = PULSEWAVES_OUTGOING;
  samplings[0].channel = 0;
  samplings[0].bits_for_duration_from_anchor = 0;            // the anchor point is the optical center. this is when the outgoing waveform starts
  samplings[0].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[0].bits_for_number_of_samples = 0;               // the number of samples is fixed (i.e. it is always 40)
  samplings[0].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[0].number_of_samples = 40;                       // the number of samples per segment is always 40
  samplings[0].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[0].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[0].sample_units = 1.0f;                          // [nanoseconds]
  samplings[0].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[0].description, "outgoing, 40 samples, 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  header.add_descriptor(&composition, samplings, CSD_PULSE_DESCRIPTOR_INDEX_0_SEG, TRUE);

  composition.optical_center_to_anchor_point = 0; // the duration from the optical center to the anchor point is zero
  composition.number_of_extra_waves_bytes = 0; 
  composition.number_of_samplings = 2;            // one outgoing, one returning (with one segment)
  composition.sample_units = 1.0f;                // [nanoseconds]
  composition.scanner_index = 1;

  strncpy(composition.description, "Optech (1 outgoing 1 returning)", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[0].type = PULSEWAVES_OUTGOING;
  samplings[0].channel = 0;
  samplings[0].bits_for_duration_from_anchor = 0;            // the outgoing waveform segment starts at time zero at the anchor
  samplings[0].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[0].bits_for_number_of_samples = 0;               // the number of samples is fixed (i.e. it is always 40)
  samplings[0].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[0].number_of_samples = 40;                       // the number of samples per segment is always 40
  samplings[0].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[0].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[0].sample_units = 1.0f;                          // [nanoseconds]
  samplings[0].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[0].description, "outgoing, 40 samples, 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[1].type = PULSEWAVES_RETURNING;
  samplings[1].channel = 0;
  samplings[1].bits_for_duration_from_anchor = 16;           // the start of each waveform segment is specified with 16 bits (in sampling units) 
  samplings[1].scale_for_duration_from_anchor = 1.0f;        // the duration is specified in sampling unit increments (without fractions)
  samplings[1].offset_for_duration_from_anchor = 0.0f;       // the duration is specified with zero offset
  samplings[1].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[1].bits_for_number_of_samples = 8;               // the number of samples per segment is specified per segment with 8 bits
  samplings[1].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[1].number_of_samples = 0;                        // the number of samples per segment varies
  samplings[1].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[1].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[1].sample_units = 1.0f;                          // [nanoseconds]
  samplings[1].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[1].description, "returning, 1 seg, vary, 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  header.add_descriptor(&composition, samplings, CSD_PULSE_DESCRIPTOR_INDEX_1_SEG, TRUE);

  composition.optical_center_to_anchor_point = 0; // the duration from the optical center to the anchor point is zero
  composition.number_of_extra_waves_bytes = 0; 
  composition.number_of_samplings = 2;            // one outgoing, one returning (with two segments)
  composition.sample_units = 1.0f;                // [nanoseconds]
  composition.scanner_index = 1;

  strncpy(composition.description, "Optech (1 outgoing 2 returning)", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[0].type = PULSEWAVES_OUTGOING;
  samplings[0].channel = 0;
  samplings[0].bits_for_duration_from_anchor = 0;            // the outgoing waveform segment starts at time zero at the anchor
  samplings[0].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[0].bits_for_number_of_samples = 0;               // the number of samples is fixed (i.e. it is always 40)
  samplings[0].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[0].number_of_samples = 40;                       // the number of samples per segment is always 40
  samplings[0].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[0].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[0].sample_units = 1.0f;                          // [nanoseconds]
  samplings[0].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[0].description, "outgoing, 40 samples, 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[1].type = PULSEWAVES_RETURNING;
  samplings[1].channel = 0;
  samplings[1].bits_for_duration_from_anchor = 16;           // the start of each waveform segment is specified with 16 bits (in sampling units) 
  samplings[1].scale_for_duration_from_anchor = 1.0f;        // the duration is specified in sampling unit increments (without fractions)
  samplings[1].offset_for_duration_from_anchor = 0.0f;       // the duration is specified with zero offset
  samplings[1].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[1].bits_for_number_of_samples = 8;               // the number of samples per segment is specified per segment with 8 bits
  samplings[1].number_of_segments = 2;                       // the number of segments per sampling is always 2
  samplings[1].number_of_samples = 0;                        // the number of samples per segment varies
  samplings[1].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[1].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[1].sample_units = 1.0f;                          // [nanoseconds]
  samplings[1].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[1].description, "returning, 2 seg, vary, 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  header.add_descriptor(&composition, samplings, CSD_PULSE_DESCRIPTOR_INDEX_2_SEG, TRUE);

  composition.optical_center_to_anchor_point = 0; // the duration from the optical center to the anchor point is zero
  composition.number_of_extra_waves_bytes = 0; 
  composition.number_of_samplings = 2;            // one outgoing, one returning (with two segments)
  composition.sample_units = 1.0f;                // [nanoseconds]
  composition.scanner_index = 1;

  strncpy(composition.description, "Optech (1 outgoing 3 returning)", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[0].type = PULSEWAVES_OUTGOING;
  samplings[0].channel = 0;
  samplings[0].bits_for_duration_from_anchor = 0;            // the outgoing waveform segment starts at time zero at the anchor
  samplings[0].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[0].bits_for_number_of_samples = 0;               // the number of samples is fixed (i.e. it is always 40)
  samplings[0].number_of_segments = 1;                       // the number of segments per sampling is always 1
  samplings[0].number_of_samples = 40;                       // the number of samples per segment is always 40
  samplings[0].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[0].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[0].sample_units = 1.0f;                          // [nanoseconds]
  samplings[0].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[0].description, "outgoing, 40 samples, 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  samplings[1].type = PULSEWAVES_RETURNING;
  samplings[1].channel = 0;
  samplings[1].bits_for_duration_from_anchor = 16;           // the start of each waveform segment is specified with 16 bits (in sampling units) 
  samplings[1].scale_for_duration_from_anchor = 1.0f;        // the duration is specified in sampling unit increments (without fractions)
  samplings[1].offset_for_duration_from_anchor = 0.0f;       // the duration is specified with zero offset
  samplings[1].bits_for_number_of_segments = 0;              // the number of segments is fixed (i.e. there is just one)
  samplings[1].bits_for_number_of_samples = 8;               // the number of samples per segment is specified per segment with 8 bits
  samplings[1].number_of_segments = 3;                       // the number of segments per sampling is always 3
  samplings[1].number_of_samples = 0;                        // the number of samples per segment varies
  samplings[1].bits_per_sample = 8;                          // the number of bits per sample is always 8
  samplings[1].lookup_table_index = PULSEWAVES_UNDEFINED;    // the index to the optional lookup table translating sample values to physical measurements
  samplings[1].sample_units = 1.0f;                          // [nanoseconds]
  samplings[1].compression = PULSEWAVES_UNCOMPRESSED;        // the samples are stored without compression
  strncpy(samplings[1].description, "returning, 3 seg, vary, 8 bits", PULSEWAVES_DESCRIPTION_SIZE);

  header.add_descriptor(&composition, samplings, CSD_PULSE_DESCRIPTOR_INDEX_3_SEG, TRUE);

  pulse.init(&header);

  header_is_populated = FALSE;

  return TRUE;
}

BOOL PULSEreaderCSD::open_waves()
{
  if (file_name == 0)
  {
    return FALSE;
  }

  CHAR* temp_file_name = strdup(file_name);
  I32 len = strlen(temp_file_name);

  temp_file_name[len-3] = 'i';
  temp_file_name[len-2] = 'd';
  temp_file_name[len-1] = 'x';

  FILE* waves_file_idx = fopen(temp_file_name, "rb");
  if (waves_file_idx == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", temp_file_name);
    free(temp_file_name);
    return FALSE;
  }

  temp_file_name[len-3] = 'n';
  temp_file_name[len-2] = 'd';
  temp_file_name[len-1] = 'f';

  if (waves_file)
  {
    fclose(waves_file);
  }

  waves_file = fopen(temp_file_name, "rb");
  if (waves_file == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", temp_file_name);
    fclose(waves_file_idx);
    free(temp_file_name);
    return FALSE;
  }

  free(temp_file_name);

  if (waves_stream)
  {
    delete waves_stream;
  }

  if (IS_LITTLE_ENDIAN())
    waves_stream = new ByteStreamInFileLE(waves_file_idx);
  else
    waves_stream = new ByteStreamInFileBE(waves_file_idx);

  if (waves_stream == 0)
  {
    fprintf(stderr,"ERROR: waves_stream is zero\n");
    fclose(waves_file_idx);
    fclose(waves_file);
    waves_file = 0;
    return FALSE;
  }

  // read the IDX header variable after variable (to avoid alignment issues)

  CHAR idx_file_name[40];
  try { waves_stream->getBytes((U8*)idx_file_name, 40); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_file_name\n");
    return FALSE;
  }
  CHAR idx_author[40];
  try { waves_stream->getBytes((U8*)idx_author, 40); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_author\n");
    return FALSE;
  }
  try { waves_stream->get32bitsLE((U8*)&(idx_number_of_frames_in_the_file)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_number_of_frames_in_the_file\n");
    return FALSE;
  }
  U32 idx_number_of_bytes_per_line;
  try { waves_stream->get32bitsLE((U8*)&(idx_number_of_bytes_per_line)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_number_of_bytes_per_line\n");
    return FALSE;
  }
  U32 idx_number_of_lines_per_frame;
  try { waves_stream->get32bitsLE((U8*)&(idx_number_of_lines_per_frame)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_number_of_lines_per_frame\n");
    return FALSE;
  }
  U32 idx_number_of_bits_per_sample;
  try { waves_stream->get32bitsLE((U8*)&(idx_number_of_bits_per_sample)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_number_of_bits_per_sample\n");
    return FALSE;
  }
  U32 idx_size_of_ancilliary_data_block;
  try { waves_stream->get32bitsLE((U8*)&(idx_size_of_ancilliary_data_block)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_size_of_ancilliary_data_block\n");
    return FALSE;
  }
  U32 idx_time_last_modified;
  try { waves_stream->get32bitsLE((U8*)&(idx_time_last_modified)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_time_last_modified\n");
    return FALSE;
  }
  U32 idx_gps_week_number;
  try { waves_stream->get32bitsLE((U8*)&(idx_gps_week_number)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_gps_week_number\n");
    return FALSE;
  }
  CHAR idx_csd_file_name[32];
  try { waves_stream->getBytes((U8*)idx_csd_file_name, 32); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_csd_file_name\n");
    return FALSE;
  }
  CHAR idx_user_comments[992];
  try { waves_stream->getBytes((U8*)idx_user_comments, 992); } catch(...)
  {
    fprintf(stderr,"ERROR: reading idx_user_comments\n");
    return FALSE;
  }

  if (idx_frame_start_times)
  {
    delete [] idx_frame_start_times;
  }
  idx_frame_start_times = new F64[idx_number_of_frames_in_the_file];
  if (idx_frame_start_times == 0)
  {
    fprintf(stderr,"ERROR: allocating %u idx_frame_start_times\n", idx_number_of_frames_in_the_file);
    return FALSE;
  }
  if (idx_frame_end_times)
  {
    delete [] idx_frame_end_times;
  }
  idx_frame_end_times = new F64[idx_number_of_frames_in_the_file];
  if (idx_frame_end_times == 0)
  {
    fprintf(stderr,"ERROR: allocating %u idx_frame_end_times\n", idx_number_of_frames_in_the_file);
    return FALSE;
  }
  if (idx_ndf_file_offsets)
  {
    delete [] idx_ndf_file_offsets;
  }
  idx_ndf_file_offsets = new I64[idx_number_of_frames_in_the_file];
  if (idx_ndf_file_offsets == 0)
  {
    fprintf(stderr,"ERROR: allocating %u idx_ndf_file_offsets\n", idx_number_of_frames_in_the_file);
    return FALSE;
  }

  for (U32 i = 0; i < idx_number_of_frames_in_the_file; i++)
  {
    I32 frame_number;
    try { waves_stream->get32bitsLE((U8*)&(frame_number)); } catch(...)
    {
      fprintf(stderr,"ERROR: reading frame_number %u of %u\n", i, idx_number_of_frames_in_the_file);
      return FALSE;
    }
    try { waves_stream->get64bitsLE((U8*)&(idx_frame_start_times[i])); } catch(...)
    {
      fprintf(stderr,"ERROR: reading frame_start_time %u of %u\n", i, idx_number_of_frames_in_the_file);
      return FALSE;
    }
    try { waves_stream->get64bitsLE((U8*)&(idx_frame_end_times[i])); } catch(...)
    {
      fprintf(stderr,"ERROR: reading idx_frame_end_time %u of %u\n", i, idx_number_of_frames_in_the_file);
      return FALSE;
    }
    try { waves_stream->get64bitsLE((U8*)&(idx_ndf_file_offsets[i])); } catch(...)
    {
      fprintf(stderr,"ERROR: reading idx_ndf_file_offsets %u of %u\n", i, idx_number_of_frames_in_the_file);
      return FALSE;
    }
    if (i == 0)
    {
      idx_frames_start = idx_frame_start_times[0];
      idx_frame_duration = idx_frame_end_times[0] - idx_frame_start_times[0];
      idx_time_increment = I32_QUANTIZE(idx_frame_duration / 16383.0 * 1000000.0);
    }
    else
    {
      if (idx_time_increment != I32_QUANTIZE((idx_frame_end_times[i] - idx_frame_start_times[i]) / 16383.0 * 1000000.0))
      {
        fprintf(stderr,"WARNING: inconsistent idx_time_increment %d versus %d at for frame %d\n", idx_time_increment, I32_QUANTIZE((idx_frame_end_times[i] - idx_frame_start_times[i] / 16383.0)), i);
        return FALSE;
      }
    }
  }

  delete waves_stream;

  if (IS_LITTLE_ENDIAN())
    waves_stream = new ByteStreamInFileLE(waves_file);
  else
    waves_stream = new ByteStreamInFileBE(waves_file);

  if (waves_stream == 0)
  {
    fprintf(stderr,"ERROR: waves_stream is zero\n");
    fclose(waves_file);
    waves_file = 0;
    return FALSE;
  }

  if (!waves0seg.init(header.get_descriptor(CSD_PULSE_DESCRIPTOR_INDEX_0_SEG)))
  {
    fprintf(stderr,"ERROR: cannot init waves0seg\n");
    return FALSE;
  }

  if (!waves1seg.init(header.get_descriptor(CSD_PULSE_DESCRIPTOR_INDEX_1_SEG)))
  {
    fprintf(stderr,"ERROR: cannot init waves1seg\n");
    return FALSE;
  }

  if (!waves2seg.init(header.get_descriptor(CSD_PULSE_DESCRIPTOR_INDEX_2_SEG)))
  {
    fprintf(stderr,"ERROR: cannot init waves2seg\n");
    return FALSE;
  }

  if (!waves3seg.init(header.get_descriptor(CSD_PULSE_DESCRIPTOR_INDEX_3_SEG)))
  {
    fprintf(stderr,"ERROR: cannot init waves3seg\n");
    return FALSE;
  }

  return TRUE;
}

I32 PULSEreaderCSD::get_format() const
{
  return PULSEWAVES_FORMAT_GCW;
}

BOOL PULSEreaderCSD::seek(const I64 p_index)
{
  if (p_index < npulses)
  {
    try { pulse_stream->seek((p_index*69)+2048); } catch(...)
    {
      fprintf(stderr,"WARNING: cannot seek\n");
      return FALSE;
    }
    p_count = p_index;
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderCSD::read_pulse_default()
{
  F64 dir[3];
  // maybe we need to open the waves file
  while (read_pulse_csd())
  {
    if (!pulsecsd.get_dir(dir, Rb_SOCS))
    {
      fprintf(stderr,"ERROR: reading dir of pulse\n");
      return FALSE;
    }
    F64 target[3];
    target[0] = pulsecsd.origin[0] + dir[0]*1000;
    target[1] = pulsecsd.origin[1] + dir[1]*1000;
    target[2] = pulsecsd.origin[2] + dir[2]*1000;
    pulse.offset = 0;
    pulse.set_anchor_and_target(pulsecsd.origin, target);
    if (pulsecsd.ranges[0])
    {
      pulse.first_returning_sample = I16_QUANTIZE(pulsecsd.ranges[0] / DISTANCE_LIGHT_TRAVELS_IN_ONE_NANOSECOND * 2);
      pulse.last_returning_sample = pulse.first_returning_sample+100;
    }
    p_count++;
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderCSD::read_waves()
{
  // maybe we need to open the waves file
  if (waves_stream == 0)
  {
    if (!open_waves())
    {
      return FALSE;
    }
  }

  if (pulsecsd.GPS_time < idx_frame_start_times[0])
  {
    return FALSE;
  }

  if (pulsecsd.GPS_time > idx_frame_end_times[idx_number_of_frames_in_the_file-1])
  {
    return FALSE;
  }

  // find correct frame and correct line

  I32 frame = I32_FLOOR((pulsecsd.GPS_time - idx_frames_start) / idx_frame_duration);

  // maybe we need to search brute force (this hopefully never happens)

  if ((pulsecsd.GPS_time < idx_frame_start_times[frame]) || (idx_frame_end_times[frame] < pulsecsd.GPS_time))
  {
    U32 f;
    for (f = 0; f < idx_number_of_frames_in_the_file; f++)
    {
      if ((idx_frame_start_times[f] <= pulsecsd.GPS_time) && (pulsecsd.GPS_time <= idx_frame_end_times[f]))
      {
        frame = f;
        break;
      }
    }
    if (f == idx_number_of_frames_in_the_file)
    {
      fprintf(stderr,"ERROR: cannot find frame with brute force search\n");
      return FALSE;
    }
  }

  F32 nanoseconds_since_frame_start = (F32)((pulsecsd.GPS_time - idx_frame_start_times[frame])*1000000.0);
  I32 line = I32_FLOOR(nanoseconds_since_frame_start/idx_time_increment);

//  fprintf(stderr,"%g in interval (%g %g). offset is %I64d\n", pulsecsd.GPS_time, idx_frame_start_times[frame], idx_frame_end_times[frame], idx_ndf_file_offsets[frame]);
//  F32 nanoseconds_since_line_start = nanoseconds_since_frame_start - line*idx_time_increment;
//  fprintf(stderr,"nanoseconds_since_frame_start %.2f line %d nanoseconds_since_line_start %.2f\n", nanoseconds_since_frame_start, line, nanoseconds_since_line_start);

  // read the correct line from the correct frame

  U16 line_number;

  // maybe we need to seek to the correct frame

  if ((last_frame != frame) || (line < last_line))
  {
    // seek
    waves_stream->seek(idx_ndf_file_offsets[frame]);
    last_frame = frame;
    last_line = -1;
  }

  if (last_line >= line)
  {
    fprintf(stderr,"WARNING: last_line (%d) >= line (%d). not reading new line.\n", last_line, line);
  }

  // skip to and read the correct line

  while (last_line < line)
  {
    last_line++;
    try { waves_stream->get16bitsLE((U8*)&(line_number)); } catch(...)
    {
      fprintf(stderr,"ERROR: reading line_number %d of %d\n", last_line, line);
      return FALSE;
    }
    try { waves_stream->get16bitsLE((U8*)&(offset_to_next_line)); } catch(...)
    {
      fprintf(stderr,"ERROR: reading offset_to_next_line %d of %d\n", last_line, line);
      return FALSE;
    }
    try { waves_stream->getBytes(line_data, offset_to_next_line); } catch(...)
    {
      fprintf(stderr,"ERROR: reading %d bytes for line %d of %d\n", offset_to_next_line, last_line, line);
      return FALSE;
    }
  }

  // determine how many segments we have

  I32 num_samples = 0;
  I32 num_segments = 0;
  I32 offset_total = 40;

  while (offset_total < offset_to_next_line)
  {
    num_segments++;
    num_samples = line_data[offset_total];
    offset_total += (1 + num_samples + 2);
  }

  WAVESsampling* sampling;

  // copy outgoing waveform

  if (num_segments == 0)
  {
    memcpy(waves0seg.get_sampling(0)->get_samples(), &(line_data[0]), 40);
    sampling = 0;
    waves = &waves0seg;
    pulse.descriptor_index = CSD_PULSE_DESCRIPTOR_INDEX_0_SEG;
  }
  else if (num_segments == 1)
  {
    memcpy(waves1seg.get_sampling(0)->get_samples(), &(line_data[0]), 40);
    sampling = waves1seg.get_sampling(1);
    waves = &waves1seg;
    pulse.descriptor_index = CSD_PULSE_DESCRIPTOR_INDEX_1_SEG;
  }
  else if (num_segments == 2)
  {
    memcpy(waves2seg.get_sampling(0)->get_samples(), &(line_data[0]), 40);
    sampling = waves2seg.get_sampling(1);
    waves = &waves2seg;
    pulse.descriptor_index = CSD_PULSE_DESCRIPTOR_INDEX_2_SEG;
  }
  else if (num_segments == 3)
  {
    memcpy(waves3seg.get_sampling(0)->get_samples(), &(line_data[0]), 40);
    sampling = waves3seg.get_sampling(1);
    waves = &waves3seg;
    pulse.descriptor_index = CSD_PULSE_DESCRIPTOR_INDEX_3_SEG;
  }
  else
  {
    fprintf(stderr,"ERROR: %d num_segments not implemented\n", num_segments);
    return FALSE;
  }

  // copy any returning waveform

  if (num_segments)
  {
    I32 first_returning_sample = I32_MAX;
    I32 last_returning_sample = I32_MIN;

    offset_total = 40;

    for (I32 s = 0; s < num_segments; s++)
    {
      sampling->set_active_segment(s);

      // get the number of samples in the next returning segment
      num_samples = line_data[offset_total];

      // request sufficient memory for this returning segment
      if (!sampling->set_number_of_samples_for_segment(num_samples))
      {
        fprintf(stderr,"ERROR: setting number of returning 8 bit samples to %d\n", num_samples);
        return FALSE;
      }

      // copy the samples of the returning segment
      memcpy(sampling->get_samples(), &(line_data[offset_total+1]), num_samples);

      // set duration from anchor
      I32 time = *((U16*)&(line_data[offset_total+1+num_samples]));
      time = time * 8;

      if (!sampling->set_duration_from_anchor_for_segment(time))
      {
        fprintf(stderr,"ERROR: duration %d from anchor to segment %d cannot be represented with %d bits\n", time, s, sampling->get_bits_for_duration_from_anchor());
        return FALSE;
      }

      if (time < first_returning_sample) first_returning_sample = time;
      time += num_samples;
      if (time > last_returning_sample) last_returning_sample = time;

      offset_total += (1 + num_samples + 2);
    }

    if (offset_total != offset_to_next_line)
    {
      fprintf(stderr,"ERROR: offset_total %d != %d\n", offset_total, offset_to_next_line);
      return FALSE;
    }

    pulse.first_returning_sample = U16_CLAMP(first_returning_sample);
    pulse.last_returning_sample = U16_CLAMP(last_returning_sample);
  }

  return TRUE;
}

ByteStreamIn* PULSEreaderCSD::get_pulse_stream() const
{
  return pulse_stream;
}

ByteStreamIn* PULSEreaderCSD::get_waves_stream() const
{
  return waves_stream;
}

void PULSEreaderCSD::close(BOOL close_streams)
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

PULSEreaderCSD::PULSEreaderCSD()
{
  file_name = 0;
  pulse_file = 0;
  pulse_stream = 0;
  waves_file = 0;
  waves_stream = 0;
  idx_frame_start_times = 0;
  idx_frame_end_times = 0;
  idx_ndf_file_offsets = 0;
  idx_frames_start = 0.0;
  idx_frame_duration = 0.0;
  idx_time_increment = 0;
  last_frame = -1;
  last_line = -1;
}

PULSEreaderCSD::~PULSEreaderCSD()
{
  if (file_name) free(file_name);
  if (pulse_stream || waves_stream) close();
}
