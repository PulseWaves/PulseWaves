/*
===============================================================================

  FILE:  pulsereader_dat.hpp
  
  CONTENTS:
  
    Reads LiDAR Pulses and Waves from NASA's GLAS format (*.dat).

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
  
    07 March 2012 -- created while watching Bayer loose 7:1 against Barcelona
  
===============================================================================
*/
#ifndef PULSE_READER_DAT_HPP
#define PULSE_READER_DAT_HPP

#include "pulsereader.hpp"

#include <stdio.h>

class PULSEpulseNativeDAT
{
public:
  I32 i_rec_ndx;              // GLAS record index 	0 	i4b 	4
  I32 i_UTCTime[2];           // Transmit time of first shot in frame in J2000 (referenced from noon on 01 January 2000) 	4 	i4b (2) 	8
  I32 i_beam_coelev;          // Co-elevation 	12 	i4b 	4
  I32 i_beam_azimuth;         // Azimuth 	16 	i4b 	4
  I8 i_spare0[16];            // Spares 	20 	i1b (16) 	16
  I32 i_lat;                  // Profile coordinate, latitude 	36 	i4b 	4
  I32 i_lon;                  // Profile coordinate, longitude 	40 	i4b 	4
  I8 i_APID_AvFlg[8];         // APID data availability flag 	44 	i1b (8) 	8
  I8 i_OrbFlg[2];             // Orbit flag 	52 	i1b (2) 	2
  U16 i_LidarQF;              // Lidar frame quality flag 	54 	i2b, unsigned 	2
  I16 i_AttFlg1;              // Attitude flag 1 	56 	i2b 	2
  I8 i_surfType;              // Region type 	58 	i1b (1) 	1
  I8 i_Spare1;                // Spares 	59 	i1b (1) 	1
  I32 i_SolAng;               // Solar incidence angle 	60 	i4b 	4
  I32 i_pad_angle;            // PAD angle 	64 	i4b 	4
  I32 i_rng_geoid;            // Range of satellite above geoid 	68 	i4b 	4
  I32 i_topo_elev;            // Topographic elevation of surface above geoid 	72 	i4b 	4
  I32 i_Rng2PCProf;           // Start range of 532 nm backscatter profile 	76 	i4b 	4
  I32 i_rng2CDProf;           // Start range of 1064 nm backscatter profile 	80 	i4b 	4
  I32 i1_g_bg[4];             // 532 nm background at 1 Hz 	84 	i4b (4) 	16
  I32 i5_g_bg[20];            // 532 nm background at 5 Hz 	100 	i4b (4,5) 	80
  I32 i40_g_bg[160];          // 532 nm background at 40 Hz 	180 	i4b (4,40) 	640
  I32 i5_ir_bg[20];           // 1064 nm background at 5 Hz 	820 	i4b (4,5) 	80
  I32 i40_ir_bg[160];         // 1064 nm background at 40 Hz 	900 	i4b (4,40) 	640
  I32 i5_g_TxNrg_EU[5];       // 532 nm laser transmit energy at 5 Hz 	1540 	i4b (5) 	20
  I32 i40_g_TxNrg_EU[10];     // 532 nm laser transmit energy at 40 Hz 	1560 	i4b (40) 	160
  I32 i5_ir_TxNrgEU[5];       // 1064 nm laser transmit energy at 5 Hz 	1720 	i4b (5) 	20
  I32 i40_ir_TxNrgEU[10];     // 1064 nm laser transmit energy at 40 Hz 	1740 	i4b (40) 	160
  I8 i_g_TxNrg_qf[10];        // 532 nm laser transmitted energy quality flag 	1900 	i1b (10) 	10
  I8 i_ir_TxNrg_qf[10];       // 1064 nm laser transmitted energy quality flag 	1910 	i1b (10) 	10
  I32 i_atm_dem;              // DEM value at current location from 1 km x 1 km grid 	1920 	i4b 	4
  I8 i_metFlg;                // Atmospheric source/quality flag 	1924 	i1b 	1
  I8 i_ir_bin_shift;          // 1064 nm vertical alignment offset 	1925 	i1b 	1
  I8 i_Spare2[6];             // Spares 	1926 	i1b (6) 	6
  I32 i_g_cal_cof[3];         // 532 nm backscatter calibration coefficient 	1932 	i4b (3) 	12
  I32 i_ir_cal_cof[2];        // 1064 nm backscatter calibration coefficient 	1944 	i4b (2) 	8
  I32 i5_g_bscs[2740];        // 532 nm merged attenuated backscatter profile 40 km to -1 km 	1952 	i4b (548,5) 	10960
  I32 i40_g_bscs[5920];       // 532 nm merged attenuated backscatter profile 10 km to -1 km 	12912 	i4b (148,40) 	23680
  I32 i5_ir_bscs[1400];       // 1064 nm attenuated backscatter profile 20 km to -1 km 	36592 	i4b (280,5) 	5600
  I32 i40_ir_bscs[5920];      // 1064 nm attenuated backscatter profile 10 km to -1 km 	42192 	i4b (148,40) 	23680
  I32 i_g_mbscs[548];         // 532 nm molecular backscatter cross section profile 40 km to -1 km 	65872 	i4b (548) 	2192
  I32 i_ir_mbscs[280];        // 1064 nm molecular backscatter cross section profile 20 km to -1 km 	68064 	i4b (280) 	1120
  I32 i1_int_ret;             // 532 nm integrated return from 40 km to 20 km 	69184 	i4b 	4
  I8 i40_g_sat_prof[740];     // 532 nm saturation flag profile 10 km to -1 km 	69188 	i1b (740) 	740
  I8 i5_g_sat_prof[343];      // 532 nm saturation flag profile 40 km to -1 km 	69928 	i1b (343) 	343
  I8 i_spare3[5];             // Spares 	70271 	i1b (5) 	5
  I8 i_532AttBS_Flag[18];     // 532 nm attenuated backscatter vertical profile flag 	70276 	i1b (18) 	18
  I8 i_1064AttBS_Flag[18];    // 1064 nm attenuated backscatter vertical profile flag 	70294 	i1b (18) 	18
  I8 i_AttFlg3;               // Attitude flag 3 	70312 	i1b 	1
  I8 i_DitheringEnabledFlag;  // Dithering enabled flag 	70313 	i1b 	1
  I16 i_timecorflg;           // Time correction flag 	70314 	i2b 	2
  I16 i_Surface_temp;         // Surface temperature 	70316 	i2b 	2
  I16 i_Surface_pres;         // Surface pressure 	70318 	i2b 	2
  I16 i_Surface_relh;         // Relative humidity 	70320 	i2b 	2
  I16 i_Surface_wind;         // Surface wind speed 	70322 	i2b 	2
  I16 i_Surface_wdir;         // Surface wind direction azimuth from north 	70324 	i2b 	2
  I8 i_spare4[130];           // Spares 	70326 	i1b (130) 	130

  F64 get_i_lon() const { return 0.0000001*i_lon; };
  F64 get_i_lat() const { return 0.0000001*i_lat; };
  F64 get_i_rng_geoid() const { return 0.01*i_rng_geoid; };
  F64 get_i_Rng2PCProf() const { return 0.01*i_Rng2PCProf; };
  F64 get_i_rng2CDProf() const { return 0.01*i_rng2CDProf; };

  PULSEpulseNativeDAT() { memset(this, 0, sizeof(this)); };
};

class PULSEreaderDAT : public PULSEreader
{
public:

  BOOL open(const char* file_name, U32 io_buffer_size=65536);

  I32 get_format() const;

  BOOL seek(const I64 p_index);

  ByteStreamIn* get_stream() const;
  void close(BOOL close_stream=TRUE);

  PULSEreaderDAT();
  virtual ~PULSEreaderDAT();

protected:
  virtual BOOL open();
  virtual BOOL read_pulse_default();

private:
  FILE* file;
  ByteStreamIn* stream;
  I32 version;
  I32 size;
  PULSEpulseNativeDAT pulsedat;
  BOOL (PULSEreaderDAT::*load_pulse)();
  BOOL load_pulse_v31();
};

#endif
