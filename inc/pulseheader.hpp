/*
===============================================================================

  FILE:  pulseheader.hpp
  
  CONTENTS:
  
    Describes the Pulse file (*.pls) header.

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
  
    16 February 2012 -- created in the cafe where it smelled like 1917 Chestnut
  
===============================================================================
*/
#ifndef PULSE_HEADER_HPP
#define PULSE_HEADER_HPP

#include <stdio.h>

#include "pulsezip.hpp"
#include "pulsevlrs.hpp"
#include "pulsedescriptor.hpp"
#include "pulsequantizer.hpp"
#include "pulseattributer.hpp"

class ByteStreamIn;
class ByteStreamOut;

class PULSEheader : public PULSEquantizer, public PULSEattributer
{
public:                                                    // start byte
  CHAR file_signature[16];                                 //          0
  U32 global_parameters;                                   //         16
  U32 file_source_ID;                                      //         20
  U32 project_ID_GUID_data_1;                              //         24
  U16 project_ID_GUID_data_2;                              //         28
  U16 project_ID_GUID_data_3;                              //         30
  U8 project_ID_GUID_data_4[8];                            //         32
  CHAR system_identifier[PULSEWAVES_DESCRIPTION_SIZE];     //         40
  CHAR generating_software[PULSEWAVES_DESCRIPTION_SIZE];   //        104
  U16 file_creation_day;                                   //        168
  U16 file_creation_year;                                  //        170
  U8 version_major;                                        //        172
  U8 version_minor;                                        //        173
  U16 header_size;                                         //        174
  I64 offset_to_pulse_data;                                //        176
  I64 number_of_pulses;                                    //        184
  U32 pulse_format;                                        //        192
  U32 pulse_attributes;                                    //        196
  U32 pulse_size;                                          //        200
  U32 pulse_compression;                                   //        204
  I64 reserved;                                            //        208
  U32 number_of_variable_length_records;                   //        216
  I32 number_of_appended_variable_length_records;          //        220
  // t_scale_factor                                                  224
  // t_offset                                                        232
  I64 min_T;                                               //        240
  I64 max_T;                                               //        248
  // x_scale_factor                                                  256
  // y_scale_factor                                                  264
  // z_scale_factor                                                  272
  // x_offset                                                        280
  // y_offset                                                        288
  // z_offset                                                        296
  F64 min_x;                                               //        304
  F64 max_x;                                               //        312
  F64 min_y;                                               //        320
  F64 max_y;                                               //        328
  F64 min_z;                                               //        336
  F64 max_z;                                               //        344

  U32 user_data_in_header_size;
  U8* user_data_in_header;

  PULSEvlr* vlrs;
  PULSEavlr* avlrs;

  U32 num_geokeys;
  PULSEgeokeys* geokeys;
  U32 num_geokey_entries;
  PULSEkeyentry* geokey_entries;
  U32 num_geodouble_params;
  F64* geodouble_params;
  U32 num_geoascii_params;
  CHAR* geoascii_params;
  U32 num_geowkt_ogc_math;
  CHAR* geowkt_ogc_math;
  U32 num_geowkt_ogc_cs;
  CHAR* geowkt_ogc_cs;

  PULSEzip* pulsezip;

  U32 user_data_after_header_size;
  U8* user_data_after_header;

  // set bounding box

  void set_bounding_box(F64 min_x, F64 min_y, F64 min_z, F64 max_x, F64 max_y, F64 max_z, BOOL auto_scale=TRUE, BOOL auto_offset=TRUE);

  // clean functions

  void clean_header();
  void clean_user_data_in_header();
  void clean_vlrs();
  void clean_user_data_after_header();
  void clean();

  PULSEheader & operator=(const PULSEquantizer & quantizer)
  {
    this->x_scale_factor = quantizer.x_scale_factor;
    this->y_scale_factor = quantizer.y_scale_factor;
    this->z_scale_factor = quantizer.z_scale_factor;
    this->x_offset = quantizer.x_offset;
    this->y_offset = quantizer.y_offset;
    this->z_offset = quantizer.z_offset;
    return *this;
  };

  PULSEheader & operator=(const PULSEheader & header)
  {
    memcpy(this, &header, sizeof(PULSEheader));
    user_data_after_header = 0;
    vlrs = 0;
    avlrs = 0;
    pulsezip = 0;
    scanners = 0;
    descriptors = 0;
    extra_attributes = 0;
    number_extra_attributes = 0;
    return *this;
  };

  BOOL check() const;

  BOOL add_vlr(const char* user_id, U32 record_id, I64 record_length_after_header, const U8* data);
  BOOL remove_vlr(U32 i);
  BOOL remove_vlr(const char* user_id, U32 record_id);
  BOOL add_avlr(const char* user_id, U32 record_id, I64 record_length_before_footer, U8* data);
  BOOL remove_avlr(U32 i);
  BOOL remove_avlr(const char* user_id, U32 record_id);

  BOOL set_geokey_entries(const U32 num_key_entries, const PULSEkeyentry* key_entries);
  BOOL set_geodouble_params(const U32 num_double_params, const F64* double_params);
  BOOL set_geoascii_params(const U32 num_ascii_params, const CHAR* ascii_params);
  BOOL get_geokey_entries(U32* num_key_entries, PULSEkeyentry** key_entries) const;
  BOOL get_geodouble_params(U32* num_double_params, F64** double_params) const;
  BOOL get_geoascii_params(U32* num_ascii_params, CHAR** ascii_params) const;
  void del_geokey_entries();
  void del_geodouble_params();
  void del_geoascii_params();

  BOOL add_scanner(const PULSEscanner* scanner, U32 scanner_index, BOOL add_to_vlrs=TRUE);
  BOOL get_scanner(PULSEscanner* scanner, U32 scanner_index) const;

  U32 find_descriptor(const PULSEcomposition* composition, const PULSEsampling* samplings);
  const PULSEdescriptor* get_descriptor(U32 descriptor_index) const;
  BOOL get_descriptor_composition(PULSEcomposition* composition, U32 descriptor_index) const;
  BOOL get_descriptor_samplings(PULSEsampling* samplings, U32 descriptor_index) const;
  BOOL add_descriptor(const PULSEcomposition* composition, const PULSEsampling* samplings, U32 descriptor_index, BOOL add_to_vlrs=TRUE);
  U32 add_descriptor(const PULSEcomposition* composition, const PULSEsampling* samplings, BOOL add_to_vlrs=TRUE);

  BOOL update_extra_bytes();

  // for serializing
  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;
  BOOL save_to_txt(FILE* file) const;

  PULSEheader();
  ~PULSEheader();

private:
  U32 num_scanners;
  PULSEscanner** scanners;
  U32 num_descriptors;
  PULSEdescriptor** descriptors;
};

#endif
