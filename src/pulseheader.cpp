/*
===============================================================================

  FILE:  pulseheader.cpp
  
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
#include "pulseheader.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bytestreamin.hpp"
#include "bytestreamout.hpp"
#include "bytestreamin_array.hpp"
#include "bytestreamout_array.hpp"

void PULSEheader::set_bounding_box(F64 min_x, F64 min_y, F64 min_z, F64 max_x, F64 max_y, F64 max_z, BOOL auto_scale, BOOL auto_offset)
{
  if (auto_scale)
  {
    if ((-360.0 <= min_x)  && (-360.0 <= min_y) && (max_x < 360.0) && (max_y < 360.0))
    {
      x_scale_factor = 0.0000001;
      y_scale_factor = 0.0000001;
    }
    else
    {
      x_scale_factor = 0.01;
      y_scale_factor = 0.01;
    }
    z_scale_factor = 0.01;
  }
  if (auto_offset)
  {
    if ((-360.0 <= min_x)  && (-360.0 <= min_y) && (max_x < 360.0) && (max_y < 360.0))
    {
      x_offset = 0;
      y_offset = 0;
      z_offset = 0;
    }
    else
    {
      x_offset = ((I32)((min_x + max_x)/200000))*100000;
      y_offset = ((I32)((min_y + max_y)/200000))*100000;
      z_offset = ((I32)((min_z + max_z)/200000))*100000;
    }
  }
  this->min_x = x_offset + x_scale_factor*I32_QUANTIZE((min_x-x_offset)/x_scale_factor);
  this->min_y = x_offset + y_scale_factor*I32_QUANTIZE((min_y-x_offset)/y_scale_factor);
  this->min_z = x_offset + z_scale_factor*I32_QUANTIZE((min_z-x_offset)/z_scale_factor);
  this->max_x = x_offset + x_scale_factor*I32_QUANTIZE((max_x-x_offset)/x_scale_factor);
  this->max_y = x_offset + y_scale_factor*I32_QUANTIZE((max_y-x_offset)/y_scale_factor);
  this->max_z = x_offset + z_scale_factor*I32_QUANTIZE((max_z-x_offset)/z_scale_factor);
};

void PULSEheader::clean_header()
{
  memset((void*)this, 0, sizeof(PULSEheader));
  strcpy(file_signature, "PulseWavesPulse");
  version_major = PULSEWAVES_VERSION_MAJOR;
  version_minor = PULSEWAVES_VERSION_MINOR;
  header_size = PULSEWAVES_PULSEHEADER_SIZE;
  offset_to_pulse_data = PULSEWAVES_PULSEHEADER_SIZE;
  pulse_size = PULSEWAVES_PULSE0_SIZE;
  t_scale_factor = 0.000001; // time stamps in micro-seconds
  t_offset = 1000000000.0;   // time stamps in GPS Standard Time minus 1 billion seconds
  x_scale_factor = 0.01;
  y_scale_factor = 0.01;
  z_scale_factor = 0.01;
};

void PULSEheader::clean_user_data_in_header()
{
  if (user_data_in_header)
  {
    header_size -= user_data_in_header_size;
    delete [] user_data_in_header;
    user_data_in_header = 0;
    user_data_in_header_size = 0;
  }
};

void PULSEheader::clean_vlrs()
{
  if (vlrs)
  {
    U32 i;
    for (i = 0; i < number_of_variable_length_records; i++)
    {
      offset_to_pulse_data -= (vlrs[i].record_length_after_header + 64);
      if (vlrs[i].data && (vlrs[i].data != (U8*)extra_attributes))
      {
        delete [] vlrs[i].data;
      }
    }
    free(vlrs);
    vlrs = 0;
  }
  if (avlrs)
  {
    I32 i;
    for (i = 0; i < number_of_appended_variable_length_records; i++)
    {
      if (avlrs[i].data && (avlrs[i].data != (U8*)extra_attributes))
      {
        delete [] avlrs[i].data;
      }
    }
    free(avlrs);
    avlrs = 0;
  }
  if (scanners)
  {
    num_scanners = 0;
    free(scanners);
    scanners = 0;
  }
  if (descriptors)
  {
    num_descriptors = 0;
    free(descriptors);
    descriptors = 0;
  }
  if (geokeys)
  {
    delete [] geokeys;
    geokeys = 0;
    num_geokeys = 0;
    geokey_entries = 0;
    num_geokey_entries = 0;
  }
  if (geodouble_params)
  {
    delete [] geodouble_params;
    geodouble_params = 0;
    num_geodouble_params = 0;
  }
  if (geoascii_params)
  {
    delete [] geoascii_params;
    geoascii_params = 0;
    num_geoascii_params = 0;
  }
  if (geowkt_ogc_math)
  {
    delete [] geowkt_ogc_math;
    geowkt_ogc_math = 0;
    num_geowkt_ogc_math = 0;
  }
  if (geowkt_ogc_cs)
  {
    delete [] geowkt_ogc_cs;
    geowkt_ogc_cs = 0;
    num_geowkt_ogc_cs = 0;
  }
  number_of_variable_length_records = 0;
};

void PULSEheader::clean_user_data_after_header()
{
  if (user_data_after_header)
  {
    offset_to_pulse_data -= user_data_after_header_size;
    delete [] user_data_after_header;
    user_data_after_header = 0;
    user_data_after_header_size = 0;
  }
};

void PULSEheader::clean()
{
  clean_user_data_in_header();
  clean_vlrs();
  clean_user_data_after_header();
  clean_extra_attributes();
  clean_header();
};

BOOL PULSEheader::check() const
{
  if (strncmp(file_signature, "PulseWavesPulse", 15) != 0)
  {
    fprintf(stderr,"ERROR: wrong file signature '%s'\n", file_signature);
    return FALSE;
  }
  if ((version_major != PULSEWAVES_VERSION_MAJOR) || (version_minor > PULSEWAVES_VERSION_MINOR))
  {
    fprintf(stderr,"WARNING: unknown version %d.%d (should be 0.0)\n", version_major, version_minor);
  }
  if (header_size < PULSEWAVES_PULSEHEADER_SIZE)
  {
    fprintf(stderr,"ERROR: header size is %d but should be at least %d\n", header_size, PULSEWAVES_PULSEHEADER_SIZE);
    return FALSE;
  }
  if (offset_to_pulse_data < header_size)
  {
    fprintf(stderr,"ERROR: offset to pulse data %d is smaller than header size %d\n", (I32)offset_to_pulse_data, header_size);
    return FALSE;
  }
  if (pulse_format > 0)
  {
    fprintf(stderr,"WARNING: unknown pulse_format %d (should be 0)\n", pulse_format);
  }
  if ((pulse_attributes != 0) && (pulse_attributes != 1) && (pulse_attributes != 2))
  {
    fprintf(stderr,"WARNING: unknown pulse_attributes %d (should be 0, 1, or 2)\n", pulse_attributes);
  }
  if (pulse_compression != 0)
  {
    fprintf(stderr,"WARNING: unknown pulse_compression %d (should be 0)\n", pulse_compression);
  }
  if (reserved != 0)
  {
#ifdef _WIN32
    fprintf(stderr,"WARNING: reserved is %I64d (should be 0)\n", reserved);
#else
    fprintf(stderr,"WARNING: reserved is %lld (should be 0)\n", reserved);
#endif
  }
  if (t_scale_factor <= 0)
  {
    fprintf(stderr,"WARNING: t scale factor is %g.\n", t_scale_factor);
  }
  if (x_scale_factor <= 0)
  {
    fprintf(stderr,"WARNING: x scale factor is %g.\n", x_scale_factor);
  }
  if (y_scale_factor <= 0)
  {
    fprintf(stderr,"WARNING: y scale factor is %g.\n", y_scale_factor);
  }
  if (z_scale_factor <= 0)
  {
    fprintf(stderr,"WARNING: z scale factor is %g.\n", z_scale_factor);
  }
  if (max_x < min_x || max_y < min_y || max_z < min_z)
  {
    fprintf(stderr,"WARNING: invalid bounding box [ %g %g %g / %g %g %g ]\n", min_x, min_y, min_z, max_x, max_y, max_z);
  }
  return TRUE;
};

BOOL PULSEheader::add_vlr(const char* user_id, U32 record_id, I64 record_length_after_header, const U8* data)
{
  U32 i = 0;
  if (record_length_after_header < 0)
  {
    return FALSE;
  }
  if ((record_length_after_header > 0) && (data == 0))
  {
    return FALSE;
  }
  if (vlrs)
  {
    for (i = 0; i < number_of_variable_length_records; i++)
    {
      if ((strncmp(vlrs[i].user_id, user_id, PULSEWAVES_USER_ID_SIZE) == 0) && (vlrs[i].record_id == record_id))
      {
        if (vlrs[i].record_length_after_header)
        {
          offset_to_pulse_data -= (U32)vlrs[i].record_length_after_header;
          delete [] vlrs[i].data;
        }
        break;
      }
    }
    if (i == number_of_variable_length_records)
    {
      number_of_variable_length_records++;
      offset_to_pulse_data += PULSEWAVES_VLRHEADER_SIZE;
      vlrs = (PULSEvlr*)realloc(vlrs, sizeof(PULSEvlr)*number_of_variable_length_records);
      if (vlrs == 0)
      {
        return FALSE;
      }
      memset(&(vlrs[i]), 0, sizeof(PULSEvlr));
    }
  }
  else
  {
    number_of_variable_length_records = 1;
    offset_to_pulse_data += PULSEWAVES_VLRHEADER_SIZE;
    vlrs = (PULSEvlr*)malloc(sizeof(PULSEvlr)*number_of_variable_length_records);
    if (vlrs == 0)
    {
      return FALSE;
    }
    memset(vlrs, 0, sizeof(PULSEvlr)*number_of_variable_length_records);
  }
  vlrs[i].reserved = 0;
  strncpy(vlrs[i].user_id, user_id, PULSEWAVES_USER_ID_SIZE);
  vlrs[i].record_id = record_id;
  vlrs[i].record_length_after_header = record_length_after_header;
  memset(vlrs[i].description, 0, PULSEWAVES_DESCRIPTION_SIZE);
  sprintf(vlrs[i].description, "PulseWaves %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);
  if (record_length_after_header)
  {
    offset_to_pulse_data += record_length_after_header;
    vlrs[i].data = new U8[(U32)record_length_after_header];
    memcpy(vlrs[i].data, data, (U32)record_length_after_header);
  }
  else
  {
    vlrs[i].data = 0;
  }
  return TRUE;
};

BOOL PULSEheader::remove_vlr(U32 i)
{
  if (vlrs)
  {
    if (i < number_of_variable_length_records)
    {
      offset_to_pulse_data -= (PULSEWAVES_VLRHEADER_SIZE + (U32)vlrs[i].record_length_after_header);
      if (vlrs[i].record_length_after_header)
      {
        delete [] vlrs[i].data;
      }
      number_of_variable_length_records--;
      if (number_of_variable_length_records)
      {
        vlrs[i] = vlrs[number_of_variable_length_records];
        vlrs = (PULSEvlr*)realloc(vlrs, sizeof(PULSEvlr)*number_of_variable_length_records);
      }
      else
      {
        free(vlrs);
        vlrs = 0;
      }
    }
    return TRUE;
  }
  return FALSE;
};

BOOL PULSEheader::remove_vlr(const char* user_id, U32 record_id)
{
  U32 i;
  for (i = 0; i < number_of_variable_length_records; i++)
  {
    if ((strncmp(vlrs[i].user_id, user_id, PULSEWAVES_USER_ID_SIZE) == 0) && (vlrs[i].record_id == record_id))
    {
      return remove_vlr(i);
    }
  }
  return FALSE;
};


BOOL PULSEheader::add_avlr(const char* user_id, U32 record_id, I64 record_length_before_footer, U8* data)
{
  I32 i = 0;
  if (record_length_before_footer < 0)
  {
    return FALSE;
  }
  if ((record_length_before_footer > 0) && (data == 0))
  {
    return FALSE;
  }
  if (avlrs)
  {
    for (i = 0; i < number_of_appended_variable_length_records; i++)
    {
      if ((strncmp(avlrs[i].user_id, user_id, PULSEWAVES_USER_ID_SIZE) == 0) && (vlrs[i].record_id == record_id))
      {
        if (avlrs[i].record_length_before_footer)
        {
          delete [] avlrs[i].data;
        }
        break;
      }
    }
    if (i == number_of_appended_variable_length_records)
    {
      number_of_appended_variable_length_records++;
      avlrs = (PULSEavlr*)realloc(avlrs, sizeof(PULSEavlr)*number_of_appended_variable_length_records);
      memset(&(avlrs[i]), 0, sizeof(PULSEavlr));
    }
  }
  else
  {
    number_of_appended_variable_length_records = 1;
    avlrs = (PULSEavlr*)malloc(sizeof(PULSEavlr)*number_of_appended_variable_length_records);
    memset(avlrs, 0, sizeof(PULSEavlr)*number_of_appended_variable_length_records);
  }
  avlrs[i].reserved = 0;
  strncpy(avlrs[i].user_id, user_id, PULSEWAVES_USER_ID_SIZE);
  avlrs[i].record_id = record_id;
  avlrs[i].record_length_before_footer = record_length_before_footer;
  memset(avlrs[i].description, 0, PULSEWAVES_DESCRIPTION_SIZE);
  sprintf(avlrs[i].description, "PulseWaves %d.%d r%d (%d) by rapidlasso", PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);
  if (record_length_before_footer)
  {
    avlrs[i].data = data;
  }
  else
  {
    avlrs[i].data = 0;
  }
  return TRUE;
};

BOOL PULSEheader::remove_avlr(U32 i)
{
  if (avlrs)
  {
    if ((I32)i < number_of_appended_variable_length_records)
    {
      if (avlrs[i].record_length_before_footer)
      {
        delete [] avlrs[i].data;
      }
      number_of_appended_variable_length_records--;
      if (number_of_appended_variable_length_records)
      {
        avlrs[i] = avlrs[number_of_appended_variable_length_records];
        avlrs = (PULSEavlr*)realloc(avlrs, sizeof(PULSEavlr)*number_of_appended_variable_length_records);
      }
      else
      {
        free(avlrs);
        avlrs = 0;
      }
    }
    return TRUE;
  }
  return FALSE;
};

BOOL PULSEheader::remove_avlr(const char* user_id, U32 record_id)
{
  I32 i;
  for (i = 0; i < number_of_appended_variable_length_records; i++)
  {
    if ((strncmp(avlrs[i].user_id, user_id, PULSEWAVES_USER_ID_SIZE) == 0) && (avlrs[i].record_id == record_id))
    {
      return remove_avlr(i);
    }
  }
  return FALSE;
};

BOOL PULSEheader::set_geokey_entries(const U32 num_key_entries, const PULSEkeyentry* key_entries)
{
  if (num_key_entries == 0) return FALSE;
  if (key_entries == 0) return FALSE;
  if (this->geokeys) delete [] this->geokeys;
  this->geokeys = new PULSEgeokeys[num_key_entries+1];
  if (this->geokeys == 0) return FALSE;
  this->geokeys->key_directory_version = 1;
  this->geokeys->key_revision = 1;
  this->geokeys->minor_revision = 0;
  this->geokeys->number_of_keys = num_key_entries;
  geokey_entries = (PULSEkeyentry*)&this->geokeys[1];
  memcpy(geokey_entries, key_entries, sizeof(PULSEkeyentry)*num_key_entries);
  return add_vlr("PulseWaves_Proj", 34735, sizeof(PULSEgeokeys)*(num_key_entries+1), (U8*)this->geokeys);
}

BOOL PULSEheader::get_geokey_entries(U32* num_key_entries, PULSEkeyentry** key_entries) const
{
  if (this->geokeys)
  {
    *num_key_entries = this->geokeys->number_of_keys;
    *key_entries = this->geokey_entries;
  }
  else
  {
    *num_key_entries = 0;
    *key_entries = 0;
  }
  return TRUE;
}

BOOL PULSEheader::set_geodouble_params(const U32 num_double_params, const F64* double_params)
{
  if (num_double_params == 0) return FALSE;
  if (double_params == 0) return FALSE;
  if (geodouble_params) delete [] geodouble_params;
  num_geodouble_params = 0;
  geodouble_params = new F64[num_double_params];
  if (geodouble_params == 0) return FALSE;
  num_geodouble_params = num_double_params;
  memcpy(this->geodouble_params, geodouble_params, sizeof(F64)*num_geodouble_params);
  return add_vlr("PulseWaves_Proj", 34736, sizeof(F64)*num_geodouble_params, (U8*)this->geodouble_params);
}

BOOL PULSEheader::get_geodouble_params(U32* num_double_params, F64** double_params) const
{
  if (num_double_params == 0) return FALSE;
  if (double_params == 0) return FALSE;
  if (geodouble_params)
  {
    *num_double_params = num_geodouble_params;
    *double_params = geodouble_params;
  }
  else
  {
    *num_double_params = 0;
    *double_params = 0;
  }
  return TRUE;
}

BOOL PULSEheader::set_geoascii_params(const U32 num_ascii_params, const CHAR* ascii_params)
{
  if (num_ascii_params == 0) return FALSE;
  if (ascii_params == 0) return FALSE;
  if (this->geoascii_params) delete [] this->geoascii_params;
  this->geoascii_params = new CHAR[num_ascii_params];
  if (this->geoascii_params == 0) return FALSE;
  num_geoascii_params = num_ascii_params;
  memcpy(this->geoascii_params, ascii_params, num_geoascii_params);
  return add_vlr("PulseWaves_Proj", 34737, num_geoascii_params, (U8*)this->geoascii_params);
}

BOOL PULSEheader::get_geoascii_params(U32* num_ascii_params, CHAR** ascii_params) const
{
  if (num_geoascii_params == 0) return FALSE;
  if (geoascii_params == 0) return FALSE;
  if (geoascii_params)
  {
    *num_ascii_params = num_geoascii_params;
    *ascii_params = geoascii_params;
  }
  else
  {
    *num_ascii_params = 0;
    *ascii_params = 0;
  }
  return TRUE;
}

void PULSEheader::del_geokey_entries()
{
  if (geokeys)
  {
    remove_vlr("PulseWaves_Proj", 34735);
    geokeys = 0;
    geokey_entries = 0;
  }
}

void PULSEheader::del_geodouble_params()
{
  if (geodouble_params)
  {
    remove_vlr("PulseWaves_Proj", 34736);
    geodouble_params = 0;
  }
}

void PULSEheader::del_geoascii_params()
{
  if (geoascii_params)
  {
    remove_vlr("PulseWaves_Proj", 34737);
    geoascii_params = 0;
  }
}

BOOL PULSEheader::add_scanner(const PULSEscanner* scanner, U32 scanner_index, BOOL add_to_vlrs)
{
  if (num_scanners < scanner_index)
  {
    if (num_scanners)
    {
      scanners = (PULSEscanner**)realloc(scanners, scanner_index*sizeof(PULSEscanner*));
    }
    else
    {
      scanners = (PULSEscanner**)malloc(scanner_index*sizeof(PULSEscanner*));
    }
    if (scanners == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for %u PULSEscanner*\n", scanner_index);
      return FALSE;
    }
    memset(&(scanners[num_scanners]), 0, (scanner_index-num_scanners)*sizeof(PULSEscanner*));
    num_scanners = scanner_index;
  }
  if (scanners[scanner_index-1] == 0)
  {
    scanners[scanner_index-1] = (PULSEscanner*)malloc(sizeof(PULSEscanner));
    if (scanners[scanner_index-1] == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for PULSEscanner %u\n", scanner_index);
      return FALSE;
    }
  }
  *(scanners[scanner_index-1]) = *scanner;

  if (add_to_vlrs)
  {
    ByteStreamOutArray* bytestreamoutarray;

    if (IS_LITTLE_ENDIAN())
      bytestreamoutarray = new ByteStreamOutArrayLE();
    else
      bytestreamoutarray = new ByteStreamOutArrayBE();

    if (bytestreamoutarray == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for bytestreamoutarray of PULSEscanner %u\n", scanner_index);
      return FALSE;
    }

    if (!scanners[scanner_index-1]->save(bytestreamoutarray))
    {
      fprintf(stderr,"ERROR: saving PULSEscanner %u in or bytestreamoutarray\n", scanner_index);
      return FALSE;
    }

    BOOL success = add_vlr("PulseWaves_Spec", PULSEWAVES_SCANNER_RECORD_ID+scanner_index, bytestreamoutarray->getSize(), bytestreamoutarray->getData());
    delete bytestreamoutarray;
    return success;
  }

  return TRUE;
}

BOOL PULSEheader::get_scanner(PULSEscanner* scanner, U32 scanner_index) const
{
  if (scanner_index && (scanner_index <= num_scanners))
  {
    *scanner = *(scanners[scanner_index-1]);
    return TRUE;
  }
  *scanner = PULSEscanner();
  return FALSE;
}

BOOL PULSEheader::add_table(const PULSEtable* table, U32 table_index, BOOL add_to_vlrs)
{
  if (num_tables < table_index)
  {
    if (num_tables)
    {
      tables = (PULSEtable**)realloc(tables, table_index*sizeof(PULSEtable*));
    }
    else
    {
      tables = (PULSEtable**)malloc(table_index*sizeof(PULSEtable*));
    }
    if (tables == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for %u PULSEtable*\n", table_index);
      return FALSE;
    }
    memset(&(tables[num_tables]), 0, (table_index-num_tables)*sizeof(PULSEtable*));
    num_tables = table_index;
  }
  if (tables[table_index-1] == 0)
  {
    tables[table_index-1] = (PULSEtable*)malloc(sizeof(PULSEtable));
    if (tables[table_index-1] == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for PULSEtable %u\n", table_index);
      return FALSE;
    }
  }
  *(tables[table_index-1]) = *table;

  if (add_to_vlrs)
  {
    ByteStreamOutArray* bytestreamoutarray;

    if (IS_LITTLE_ENDIAN())
      bytestreamoutarray = new ByteStreamOutArrayLE();
    else
      bytestreamoutarray = new ByteStreamOutArrayBE();

    if (bytestreamoutarray == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for bytestreamoutarray of PULSEtable %u\n", table_index);
      return FALSE;
    }

    if (!tables[table_index-1]->save(bytestreamoutarray))
    {
      fprintf(stderr,"ERROR: saving PULSEtable %u in or bytestreamoutarray\n", table_index);
      return FALSE;
    }

    BOOL success = add_vlr("PulseWaves_Spec", PULSEWAVES_TABLE_RECORD_ID+table_index, bytestreamoutarray->getSize(), bytestreamoutarray->getData());
    delete bytestreamoutarray;
    return success;
  }

  return TRUE;
}

BOOL PULSEheader::get_table(PULSEtable* table, U32 table_index) const
{
  if (table_index && (table_index <= num_tables))
  {
    *table = *(tables[table_index-1]);
    return TRUE;
  }
  *table = PULSEtable();
  return FALSE;
}

BOOL PULSEheader::add_descriptor(const PULSEcomposition* composition, const PULSEsampling* samplings, U32 descriptor_index, BOOL add_to_vlrs)
{
  if (composition == 0)
  {
    fprintf(stderr,"ERROR: no composition for PULSEdescriptor %u\n", descriptor_index);
    return FALSE;
  }

  if ((composition->number_of_samplings > 0) && (samplings == 0))
  {
    fprintf(stderr,"ERROR: no samplings for PULSEdescriptor %u composed with number_of_samplings == %d\n", descriptor_index, composition->number_of_samplings);
    return FALSE;
  }

  if (descriptor_index == 0)
  {
    fprintf(stderr,"ERROR: descriptor index cannot be zero. must be between 1 and 255\n");
    return FALSE;
  }

  if (descriptor_index > 255)
  {
    fprintf(stderr,"ERROR: descriptor index cannot be %u. must be between 1 and 255\n", descriptor_index);
    return FALSE;
  }

  if (num_descriptors <= descriptor_index)
  {
    if (num_descriptors)
    {
      descriptors = (PULSEdescriptor**)realloc(descriptors, sizeof(PULSEdescriptor*)*(descriptor_index+1));
    }
    else
    {
      descriptors = (PULSEdescriptor**)malloc(sizeof(PULSEdescriptor*)*(descriptor_index+1));
    }
    if (descriptors == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for %u PULSEdescriptor*\n", descriptor_index);
      return FALSE;
    }
    memset(&(descriptors[num_descriptors]), 0, (descriptor_index - num_descriptors + 1) * sizeof(PULSEdescriptor*));
    num_descriptors = descriptor_index+1;
  }

  if (descriptors[descriptor_index] == 0)
  {
    descriptors[descriptor_index] = (PULSEdescriptor*)malloc(sizeof(PULSEdescriptor));
    if (descriptors[descriptor_index] == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for PULSEdescriptor %u\n", descriptor_index);
      return FALSE;
    }
    memset(descriptors[descriptor_index], 0, sizeof(PULSEdescriptor));
  }

  if (descriptors[descriptor_index]->composition == 0)
  {
    descriptors[descriptor_index]->composition = (PULSEcomposition*)malloc(sizeof(PULSEcomposition));
    if (descriptors[descriptor_index]->composition == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for PULSEcomposition of PULSEdescriptor %u\n", descriptor_index);
      return FALSE;
    }
  }
  *(descriptors[descriptor_index]->composition) = *composition;

  if (descriptors[descriptor_index]->samplings)
  {
    free(descriptors[descriptor_index]->samplings);
    descriptors[descriptor_index]->samplings = 0;
  }

  if (composition->number_of_samplings)
  {
    descriptors[descriptor_index]->samplings = (PULSEsampling*)malloc(composition->number_of_samplings*sizeof(PULSEsampling));
    if (descriptors[descriptor_index]->samplings == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for PULSEsampling[%u] of PULSEdescriptor %u\n", composition->number_of_samplings, descriptor_index);
      return FALSE;
    }
    for (U32 u = 0; u < composition->number_of_samplings; u++)
    {
      descriptors[descriptor_index]->samplings[u] = samplings[u];
    }
  }

  if (add_to_vlrs)
  {
    ByteStreamOutArray* bytestreamoutarray;

    if (IS_LITTLE_ENDIAN())
      bytestreamoutarray = new ByteStreamOutArrayLE();
    else
      bytestreamoutarray = new ByteStreamOutArrayBE();

    if (bytestreamoutarray == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for bytestreamoutarray of PULSEdescriptor %u\n", descriptor_index);
      return FALSE;
    }

    if (!descriptors[descriptor_index]->save(bytestreamoutarray))
    {
      fprintf(stderr,"ERROR: saving PULSEdescriptor %u in or bytestreamoutarray\n", descriptor_index);
      return FALSE;
    }

    BOOL success = add_vlr("PulseWaves_Spec", PULSEWAVES_DESCRIPTOR_RECORD_ID+descriptor_index, bytestreamoutarray->getSize(), bytestreamoutarray->getData());
    delete bytestreamoutarray;
    return success;
  }

  return TRUE;
}

U32 PULSEheader::add_descriptor_assign_index(const PULSEcomposition* composition, const PULSEsampling* samplings, BOOL add_to_vlrs)
{
  U32 descriptor_index = num_descriptors + 1;
  if (add_descriptor(composition, samplings, descriptor_index, add_to_vlrs))
  {
    return descriptor_index;
  }
  return 0;
}

BOOL PULSEheader::get_descriptor_composition(PULSEcomposition* composition, U32 descriptor_index) const
{
  if (composition == 0)
  {
    fprintf(stderr,"ERROR: zero pointer when getting PULSEcomposition for PULSEdescriptor %u\n", descriptor_index);
    return FALSE;
  }
  if (descriptor_index && (descriptor_index <= num_descriptors))
  {
    *composition = *(descriptors[descriptor_index]->composition);
    return TRUE;
  }
  *composition = PULSEcomposition();
  return FALSE;
}

BOOL PULSEheader::get_descriptor_samplings(PULSEsampling* samplings, U32 descriptor_index) const
{
  if (samplings == 0)
  {
    fprintf(stderr,"ERROR: zero pointer when getting PULSEsampling[] for PULSEdescriptor %u\n", descriptor_index);
    return FALSE;
  }
  if (descriptor_index && (descriptor_index <= num_descriptors))
  {
    I32 m, num_samplings = descriptors[descriptor_index]->composition->number_of_samplings;
    for (m = 0; m < num_samplings; m++)
    {
      samplings[m] = descriptors[descriptor_index]->samplings[m];
    }
    return TRUE;
  }
  samplings[0] = PULSEsampling();
  return FALSE;
}

U32 PULSEheader::find_descriptor(const PULSEcomposition* composition, const PULSEsampling* samplings)
{
  if (num_descriptors)
  {
    for (U32 u = 0; u < num_descriptors; u++)
    {
      if (descriptors[u])
      {
        if (descriptors[u]->is_equal(composition, samplings))
        {
          return u;
        }
      }
    }
  }
  return 0;
}

const PULSEdescriptor* PULSEheader::get_descriptor(U32 descriptor_index) const
{
  if (descriptor_index)
  {
    if (descriptor_index <= num_descriptors)
    {
      return descriptors[descriptor_index];
    }
  }
  return 0;
}

BOOL PULSEheader::update_extra_bytes()
{
  if (number_extra_attributes)
  {
    U16 record_length_after_header = sizeof(PULSEattribute)*number_extra_attributes;
    U8* data = new U8[record_length_after_header];
    if (data == 0)
    {
      fprintf(stderr,"ERROR: allocating memory for %d byte payload of VLR for Extra Bytes\n", record_length_after_header);
      return FALSE;
    }
    memcpy(data, extra_attributes, record_length_after_header);
    return add_vlr("PulseWaves_Spec", PULSEWAVES_EXTRABYTES_RECORD_ID, record_length_after_header, data);
  }
  else
  {
    return remove_vlr("PulseWaves_Spec", PULSEWAVES_EXTRABYTES_RECORD_ID);
  }
}

BOOL PULSEheader::load(ByteStreamIn* stream)
{
  // clean the header

  clean();

  // read the header variable after variable (to avoid alignment issues)

  try { stream->getBytes((U8*)file_signature, 16); } catch(...)
  {
    fprintf(stderr,"ERROR: reading file_signature\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&(global_parameters)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading global_parameters\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&(file_source_ID)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading file_source_ID\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&(project_ID_GUID_data_1)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading project_ID_GUID_data_1\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&(project_ID_GUID_data_2)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading project_ID_GUID_data_2\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&(project_ID_GUID_data_3)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading project_ID_GUID_data_3\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)project_ID_GUID_data_4, 8); } catch(...)
  {
    fprintf(stderr,"ERROR: reading project_ID_GUID_data_4\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)&(system_identifier), PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading system_identifier\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)&(generating_software), PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: reading generating_software\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&(file_creation_day)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading file_creation_day\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&(file_creation_year)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading file_creation_year\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)&(version_major), 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading version_major\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)&(version_minor), 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading version_minor\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&(header_size)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header_size\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(offset_to_pulse_data)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading offset_to_pulse_data\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(number_of_pulses)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_pulses\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&(pulse_format)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading pulse_format\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&(pulse_attributes)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading pulse_attributes\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&(pulse_size)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading pulse_size\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&(pulse_compression)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading pulse_compression\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(reserved)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading reserved\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&(number_of_variable_length_records)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_variable_length_records\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&(number_of_appended_variable_length_records)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading number_of_appended_variable_length_records\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(t_scale_factor)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading t_scale_factor\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(t_offset)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading t_offset\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(min_T)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading min_T\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(max_T)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading max_T\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(x_scale_factor)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading x_scale_factor\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(y_scale_factor)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading y_scale_factor\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(z_scale_factor)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z_scale_factor\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(x_offset)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading x_offset\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(y_offset)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading y_offset\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(z_offset)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading z_offset\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(min_x)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading min_x\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(max_x)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading max_x\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(min_y)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading min_y\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(max_y)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading max_y\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(min_z)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading min_z\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&(max_z)); } catch(...)
  {
    fprintf(stderr,"ERROR: reading max_z\n");
    return FALSE;
  }

  // load any number of user-defined bytes that might have been added to the header

  if (user_data_in_header_size)
  {
    user_data_in_header = new U8[user_data_in_header_size];
    if (user_data_in_header == 0)
    {
      fprintf(stderr,"ERROR: allocating %u bytes of data for user_data_in_header\n", user_data_in_header_size);
      return FALSE;
    }

    try { stream->getBytes((U8*)user_data_in_header, user_data_in_header_size); } catch(...)
    {
      fprintf(stderr,"ERROR: reading %u bytes of data into user_data_in_header\n", user_data_in_header_size);
      return FALSE;
    }
  }

  // check header contents

  if (!check())
  {
    return FALSE;
  }

  // read the variable length records into the header

  I64 vlrs_size = 0;

  if (number_of_variable_length_records)
  {
    vlrs = (PULSEvlr*)malloc(sizeof(PULSEvlr)*number_of_variable_length_records);
    memset(vlrs, 0, sizeof(PULSEvlr)*number_of_variable_length_records);
  
    U32 i;
    for (i = 0; i < number_of_variable_length_records; i++)
    {
      // make sure there are even enough bytes left to read the VLR header before the pulse block starts

      if ((offset_to_pulse_data - vlrs_size - header_size) < (PULSEWAVES_VLRHEADER_SIZE))
      {
        fprintf(stderr,"WARNING: only %d bytes until pulse block after reading %d of %d VLRs. skipping remaining VLRs ...\n", (I32)(offset_to_pulse_data - vlrs_size - header_size), i, number_of_variable_length_records);
        number_of_variable_length_records = i;
        break;
      }

      // load the VLR

      if (!vlrs[i].load(stream))
      {
        fprintf(stderr,"ERROR: loading vlrs[%d]\n", i);
        return FALSE;
      }

      // keep track on the number of bytes we have read so far

      vlrs_size += (vlrs[i].record_length_after_header + PULSEWAVES_VLRHEADER_SIZE);

      // special handling for known variable header tags

      if (strncmp(vlrs[i].user_id, "PulseWaves_Proj", PULSEWAVES_USER_ID_SIZE) == 0)
      {
        if (vlrs[i].record_id == 34735) // GeoKeyDirectoryTag
        {
          if (geokeys)
          {
            fprintf(stderr,"WARNING: variable length records contain more than one GeoKeyDirectoryTag\n");
            delete [] geokeys;
          }
          num_geokeys = ((U32)vlrs[i].record_length_after_header)/PULSEWAVES_GEO_KEY_SIZE;
          geokeys = new PULSEgeokeys[num_geokeys];
          if (geokeys == 0)
          {
            fprintf(stderr,"ERROR: allocating %u bytes of data for %u geo keys\n", num_geokeys*PULSEWAVES_GEO_KEY_SIZE, num_geokeys);
            return FALSE;
          }
          memcpy(geokeys, vlrs[i].data, num_geokeys*PULSEWAVES_GEO_KEY_SIZE);

          // check variable header geo keys contents

          if (geokeys->key_directory_version != 1)
          {
            fprintf(stderr,"WARNING: wrong geokeys->key_directory_version: %d != 1\n",geokeys->key_directory_version);
          }
          if (geokeys->key_revision != 1)
          {
            fprintf(stderr,"WARNING: wrong geokeys->key_revision: %d != 1\n",geokeys->key_revision);
          }
          if (geokeys->minor_revision != 0)
          {
            fprintf(stderr,"WARNING: wrong geokeys->minor_revision: %d != 0\n",geokeys->minor_revision);
          }
          num_geokey_entries = num_geokeys - 1;
          geokey_entries = (PULSEkeyentry*)&geokeys[1];
        }
        else if (vlrs[i].record_id == 34736) // GeoDoubleParamsTag
        {
          if (geodouble_params)
          {
            fprintf(stderr,"WARNING: variable length records contain more than one GeoF64ParamsTag\n");
            delete [] geodouble_params;
          }
          num_geodouble_params = (U32)(vlrs[i].record_length_after_header/8);
          geodouble_params = new F64[num_geodouble_params];
          if (geodouble_params == 0)
          {
            fprintf(stderr,"ERROR: allocating %u bytes of data for %u geo double params\n", num_geodouble_params*sizeof(F64), num_geodouble_params);
            return FALSE;
          }
          memcpy(geodouble_params, vlrs[i].data, num_geodouble_params*sizeof(F64));
        }
        else if (vlrs[i].record_id == 34737) // GeoAsciiParamsTag
        {
          if (geoascii_params)
          {
            fprintf(stderr,"WARNING: variable length records contain more than one GeoAsciiParamsTag\n");
            delete [] geoascii_params;
          }
          num_geoascii_params = (U32)vlrs[i].record_length_after_header;
          geoascii_params = new CHAR[num_geoascii_params];
          if (geoascii_params == 0)
          {
            fprintf(stderr,"ERROR: allocating %u bytes of data for geo ascii params\n", num_geoascii_params);
            return FALSE;
          }
          memcpy(geoascii_params, vlrs[i].data, num_geoascii_params);
        }
        else if (vlrs[i].record_id == 2111) // WKT OCG MATH string
        {
          if (geowkt_ogc_math)
          {
            delete [] geowkt_ogc_math;
            fprintf(stderr,"WARNING: variable length records contain more than one WKT OCG MATH string\n");
          }
          num_geowkt_ogc_math = (U32)vlrs[i].record_length_after_header;
          geowkt_ogc_math = new CHAR[num_geowkt_ogc_math];
          if (geowkt_ogc_math == 0)
          {
            fprintf(stderr,"ERROR: allocating %u bytes of data for geowkt ogc math\n", num_geowkt_ogc_math);
            return FALSE;
          }
          memcpy(geowkt_ogc_math, vlrs[i].data, num_geowkt_ogc_math);
        }
        else if (vlrs[i].record_id == 2112) // WKT OCG CS string
        {
          if (geowkt_ogc_cs)
          {
            delete [] geowkt_ogc_cs;
            fprintf(stderr,"WARNING: variable length records contain more than one WKT OCG CS string\n");
          }
          num_geowkt_ogc_cs = (U32)vlrs[i].record_length_after_header;
          geowkt_ogc_cs = new CHAR[num_geowkt_ogc_cs];
          if (geowkt_ogc_cs == 0)
          {
            fprintf(stderr,"ERROR: allocating %u bytes of data for geowkt ogc math\n", num_geowkt_ogc_cs);
            return FALSE;
          }
          memcpy(geowkt_ogc_cs, vlrs[i].data, num_geowkt_ogc_cs);
        }
      }
      else if (strncmp(vlrs[i].user_id, "PulseWaves_Spec", PULSEWAVES_USER_ID_SIZE) == 0)
      {
        if (vlrs[i].record_id == PULSEWAVES_EXTRABYTES_RECORD_ID) // ExtraBytes
        {
          fprintf(stderr,"WARNING: ExtraBytes not yet implemented\n");
        }
        else if (vlrs[i].record_id == PULSEWAVES_PULSEZIP_RECORD_ID) // PULSEzip
        {
          ByteStreamInArray* bytestreaminarray;

          if (IS_LITTLE_ENDIAN())
            bytestreaminarray = new ByteStreamInArrayLE(vlrs[i].data, vlrs[i].record_length_after_header);
          else
            bytestreaminarray = new ByteStreamInArrayBE(vlrs[i].data, vlrs[i].record_length_after_header);

          if (bytestreaminarray == 0)
          {
            fprintf(stderr,"ERROR: allocating memory for bytestreaminarray for vlrs[%d]\n", i);
            return FALSE;
          }

          if (pulsezip)
          {
            fprintf(stderr,"WARNING: multiple pulsezip VLRs ... keeping last one\n");
            delete pulsezip;
          }

          pulsezip = new PULSEzip();
          if (pulsezip == 0)
          {
            fprintf(stderr,"ERROR: allocating memory for PULSEzip\n");
            return FALSE;
          }

          if (!pulsezip->load(bytestreaminarray))
          {
            fprintf(stderr,"ERROR: parsing pulsezip VLR\n");
            delete bytestreaminarray;
            return FALSE;
          }

          // remove PULSEzip VRL from header

          vlrs_size -= (vlrs[i].record_length_after_header + PULSEWAVES_VLRHEADER_SIZE);
          offset_to_pulse_data -= (vlrs[i].record_length_after_header + PULSEWAVES_VLRHEADER_SIZE);
          number_of_variable_length_records--;
          i--;

          delete bytestreaminarray;
        }
        else if ((PULSEWAVES_DESCRIPTOR_RECORD_ID_MIN <= vlrs[i].record_id) && (vlrs[i].record_id <= PULSEWAVES_DESCRIPTOR_RECORD_ID_MAX)) // PulseDescriptor
        {
          ByteStreamInArray* bytestreaminarray;

          if (IS_LITTLE_ENDIAN())
            bytestreaminarray = new ByteStreamInArrayLE(vlrs[i].data, vlrs[i].record_length_after_header);
          else
            bytestreaminarray = new ByteStreamInArrayBE(vlrs[i].data, vlrs[i].record_length_after_header);

          if (bytestreaminarray == 0)
          {
            fprintf(stderr,"ERROR: allocating memory for bytestreaminarray for vlrs[%d]\n", i);
            return FALSE;
          }

          PULSEdescriptor descriptor;
          if (!descriptor.load(bytestreaminarray))
          {
            fprintf(stderr,"ERROR: parsing pulsedescriptor %d\n", vlrs[i].record_id - PULSEWAVES_DESCRIPTOR_RECORD_ID);
            delete bytestreaminarray;
            return FALSE;
          }
          delete bytestreaminarray;

          if (!add_descriptor(descriptor.composition, descriptor.samplings, vlrs[i].record_id - PULSEWAVES_DESCRIPTOR_RECORD_ID, FALSE))
          {
            return FALSE;
          }
        }
        else if ((PULSEWAVES_SCANNER_RECORD_ID_MIN <= vlrs[i].record_id) && (vlrs[i].record_id <= PULSEWAVES_SCANNER_RECORD_ID_MAX)) // Scanner
        {
          ByteStreamInArray* bytestreaminarray;

          if (IS_LITTLE_ENDIAN())
            bytestreaminarray = new ByteStreamInArrayLE(vlrs[i].data, vlrs[i].record_length_after_header);
          else
            bytestreaminarray = new ByteStreamInArrayBE(vlrs[i].data, vlrs[i].record_length_after_header);

          if (bytestreaminarray == 0)
          {
            fprintf(stderr,"ERROR: allocating memory for bytestreaminarray for vlrs[%d]\n", i);
            return FALSE;
          }

          PULSEscanner scanner;
          if (!scanner.load(bytestreaminarray))
          {
            fprintf(stderr,"ERROR: parsing scanner %d\n", vlrs[i].record_id - PULSEWAVES_SCANNER_RECORD_ID);
            delete bytestreaminarray;
            return FALSE;
          }
          delete bytestreaminarray;
          if (!add_scanner(&scanner, vlrs[i].record_id - PULSEWAVES_SCANNER_RECORD_ID, FALSE))
          {
            return FALSE;
          }
        }
      }
    }
  }

  // load any number of user-defined bytes that might have been added after the header

  user_data_after_header_size = (U32)(offset_to_pulse_data - vlrs_size) - header_size;
  if (user_data_after_header_size)
  {
    user_data_after_header = new U8[user_data_after_header_size];
    if (user_data_after_header == 0)
    {
      fprintf(stderr,"ERROR: allocating %d bytes of data for user_data_after_header\n", user_data_after_header_size);
      return FALSE;
    }
    try { stream->getBytes((U8*)user_data_after_header, user_data_after_header_size); } catch(...)
    {
      fprintf(stderr,"ERROR: reading %d bytes of data into user_data_after_header\n", user_data_after_header_size);
      return FALSE;
    }
  }

  if (number_of_appended_variable_length_records != 0)
  {
    if (!stream->isSeekable())
    {
      fprintf(stderr,"WARNING: file has %d AVLRs but stream is not seekable ...\n", number_of_appended_variable_length_records);
    }
    else if (number_of_appended_variable_length_records > 0)
    {
      I64 here = stream->tell();
      stream->seek(0);

      avlrs = (PULSEavlr*)malloc(sizeof(PULSEavlr)*number_of_appended_variable_length_records);
      if (avlrs == 0)
      {
        fprintf(stderr,"ERROR: allocating memory for %d avlrs\n", number_of_appended_variable_length_records);
        return FALSE;
      }
      memset(avlrs, 0, sizeof(PULSEavlr)*number_of_appended_variable_length_records);

      // read the appended variable length records into the header

      I64 avlrs_size = 0;
      I32 i;
      for (i = 0; i < number_of_appended_variable_length_records; i++)
      {
        // read variable length records variable after variable (to avoid alignment issues)

        try { stream->getBytes((U8*)avlrs[i].user_id, 16); } catch(...)
        {
          fprintf(stderr,"ERROR: reading avlrs[%d].user_id\n", i);
          return FALSE;
        }
        try { stream->get32bitsLE((U8*)&(avlrs[i].record_id)); } catch(...)
        {
          fprintf(stderr,"ERROR: reading avlrs[%d].record_id\n", i);
          return FALSE;
        }
        try { stream->get32bitsLE((U8*)&(avlrs[i].reserved)); } catch(...)
        {
          fprintf(stderr,"ERROR: reading avlrs[%d].reserved\n", i);
          return FALSE;
        }
        try { stream->get64bitsLE((U8*)&(avlrs[i].record_length_before_footer)); } catch(...)
        {
          fprintf(stderr,"ERROR: reading avlrs[%d].record_length_before_footer\n", i);
          return FALSE;
        }
        try { stream->getBytes((U8*)avlrs[i].description, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
        {
          fprintf(stderr,"ERROR: reading avlrs[%d].description\n", i);
          return FALSE;
        }

        // keep track on the number of bytes we have read so far

        avlrs_size += PULSEWAVES_VLRHEADER_SIZE;

        // check variable length record contents

        if (avlrs[i].reserved != 0x0)
        {
          fprintf(stderr,"WARNING: wrong avlrs[%d].reserved: %d != 0x0\n", i, avlrs[i].reserved);
        }

        // load data preceeding the footer of the appended variable length record

        if (avlrs[i].record_length_before_footer > 0)
        {
          if (strncmp(avlrs[i].user_id, "pulsezip encoded", PULSEWAVES_USER_ID_SIZE) == 0)
          {
          }
          else
          {
            avlrs[i].data = new U8[(U32)avlrs[i].record_length_before_footer];
            if (avlrs[i].data == 0)
            {
              fprintf(stderr,"ERROR: allocating %u bytes for payload of avlrs[%d]\n", (U32)avlrs[i].record_length_before_footer, i);
              return FALSE;
            }
            try { stream->getBytes((U8*)avlrs[i].data, (U32)avlrs[i].record_length_before_footer); } catch(...)
            {
              fprintf(stderr,"ERROR: reading %u bytes of data into avlrs[%d].data\n", (U32)avlrs[i].record_length_before_footer, i);
              return FALSE;
            }
          }
        }
        else
        {
          avlrs[i].data = 0;
        }

        // keep track on the number of bytes we have read so far

        avlrs_size += avlrs[i].record_length_before_footer;

        // special handling for known variable header tags

        if (strncmp(avlrs[i].user_id, "PulseWaves_Proj", PULSEWAVES_USER_ID_SIZE) == 0)
        {
          if (avlrs[i].record_id == 34735) // GeoKeyDirectoryTag
          {
            if (geokeys)
            {
              fprintf(stderr,"WARNING: variable length records contain more than one GeoKeyDirectoryTag\n");
            }
            geokeys = (PULSEgeokeys*)avlrs[i].data;

            // check variable header geo keys contents

            if (geokeys->key_directory_version != 1)
            {
              fprintf(stderr,"WARNING: wrong geokeys->key_directory_version: %d != 1\n",geokeys->key_directory_version);
            }
            if (geokeys->key_revision != 1)
            {
              fprintf(stderr,"WARNING: wrong geokeys->key_revision: %d != 1\n",geokeys->key_revision);
            }
            if (geokeys->minor_revision != 0)
            {
              fprintf(stderr,"WARNING: wrong geokeys->minor_revision: %d != 0\n",geokeys->minor_revision);
            }
            geokey_entries = (PULSEkeyentry*)&geokeys[1];
          }
          else if (avlrs[i].record_id == 34736) // GeoDoubleParamsTag
          {
            if (geodouble_params)
            {
              fprintf(stderr,"WARNING: variable length records contain more than one GeoF64ParamsTag\n");
            }
            geodouble_params = (F64*)avlrs[i].data;
          }
          else if (avlrs[i].record_id == 34737) // GeoAsciiParamsTag
          {
            if (geoascii_params)
            {
              fprintf(stderr,"WARNING: variable length records contain more than one GeoAsciiParamsTag\n");
            }
            geoascii_params = (CHAR*)avlrs[i].data;
          }
          else if (avlrs[i].record_id == 2111) // WKT OCG MATH string
          {
            if (geowkt_ogc_math)
            {
              delete [] geowkt_ogc_math;
              fprintf(stderr,"WARNING: variable length records contain more than one WKT OCG MATH string\n");
            }
            num_geowkt_ogc_math = (U32)avlrs[i].record_length_before_footer;
            geowkt_ogc_math = new CHAR[num_geowkt_ogc_math];
            if (geowkt_ogc_math == 0)
            {
              fprintf(stderr,"ERROR: allocating %u bytes of data for geowkt ogc math\n", num_geowkt_ogc_math);
              return FALSE;
            }
            memcpy(geowkt_ogc_math, avlrs[i].data, num_geowkt_ogc_math);
          }
          else if (avlrs[i].record_id == 2112) // WKT OCG CS string
          {
            if (geowkt_ogc_cs)
            {
              delete [] geowkt_ogc_cs;
              fprintf(stderr,"WARNING: variable length records contain more than one WKT OCG CS string\n");
            }
            num_geowkt_ogc_cs = (U32)avlrs[i].record_length_before_footer;
            geowkt_ogc_cs = new CHAR[num_geowkt_ogc_cs];
            if (geowkt_ogc_cs == 0)
            {
              fprintf(stderr,"ERROR: allocating %u bytes of data for geowkt ogc math\n", num_geowkt_ogc_cs);
              return FALSE;
            }
            memcpy(geowkt_ogc_cs, avlrs[i].data, num_geowkt_ogc_cs);
          }
        }
        else if (strncmp(avlrs[i].user_id, "PulseWaves_Spec", PULSEWAVES_USER_ID_SIZE) == 0)
        {
        }
      }
      stream->seek(here);
    }
    else if (number_of_appended_variable_length_records == -1)
    {
      fprintf(stderr,"WARNING: loading unknown number of AVLRs not yet implemented. ignoring ...\n");
    }
    else
    {
      fprintf(stderr,"WARNING: file has corrupt number of %d AVLRs. ignoring ...\n", number_of_appended_variable_length_records);
    }
  }
  return TRUE;
};

BOOL PULSEheader::save(ByteStreamOut* stream) const
{
  // adjusted header fields in case of internal VLRs

  I64 offset_to_pulse_data = this->offset_to_pulse_data;
  U32 number_of_variable_length_records = this->number_of_variable_length_records;
  U32 number_of_header_variable_length_records = this->number_of_variable_length_records;

  if (pulsezip)
  {
    offset_to_pulse_data += (PULSEWAVES_VLRHEADER_SIZE + pulsezip->get_payload());
    number_of_variable_length_records += 1;
  }

  // write header variable after variable (to avoid alignment issues)

  try { stream->putBytes((U8*)file_signature, 16); } catch(...)
  {
    fprintf(stderr,"ERROR: writing file_signature\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&(global_parameters)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing global_parameters\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&(file_source_ID)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing file_source_ID\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&(project_ID_GUID_data_1)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing project_ID_GUID_data_1\n");
    return FALSE;
  }
  try { stream->put16bitsLE((U8*)&(project_ID_GUID_data_2)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing project_ID_GUID_data_2\n");
    return FALSE;
  }
  try { stream->put16bitsLE((U8*)&(project_ID_GUID_data_3)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing project_ID_GUID_data_3\n");
    return FALSE;
  }
  try { stream->putBytes((U8*)project_ID_GUID_data_4, 8); } catch(...)
  {
    fprintf(stderr,"ERROR: writing project_ID_GUID_data_4\n");
    return FALSE;
  }
  try { stream->putBytes((U8*)system_identifier, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing system_identifier\n");
    return FALSE;
  }
  try { stream->putBytes((U8*)generating_software, PULSEWAVES_DESCRIPTION_SIZE); } catch(...)
  {
    fprintf(stderr,"ERROR: writing generating_software\n");
    return FALSE;
  }
  try { stream->put16bitsLE((U8*)&(file_creation_day)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing file_creation_day\n");
    return FALSE;
  }
  try { stream->put16bitsLE((U8*)&(file_creation_year)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing file_creation_year\n");
    return FALSE;
  }
  // check version major
  if (version_major != PULSEWAVES_VERSION_MAJOR)
  {
    fprintf(stderr,"WARNING: version_major is %d. writing %d instead.\n", version_major, PULSEWAVES_VERSION_MAJOR);
    try { stream->putByte(0); } catch(...)
    {
      fprintf(stderr,"ERROR: writing version_major\n");
      return FALSE;
    }
  }
  else
  {
    try { stream->putByte(version_major); } catch(...)
    {
      fprintf(stderr,"ERROR: writing version_major\n");
      return FALSE;
    }
  }
  // check version minor
  if (version_minor > PULSEWAVES_VERSION_MINOR)
  {
    fprintf(stderr,"WARNING: version_minor is %d. writing %d instead.\n", version_minor, PULSEWAVES_VERSION_MINOR);
    try { stream->putByte(0); } catch(...)
    {
      fprintf(stderr,"ERROR: writing version_minor\n");
      return FALSE;
    }
  }
  else
  {
    try { stream->putByte(version_minor); } catch(...)
    {
      fprintf(stderr,"ERROR: writing version_minor\n");
      return FALSE;
    }
  }
  try { stream->put16bitsLE((U8*)&(header_size)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing header_size\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(offset_to_pulse_data)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing offset_to_pulse_data\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(number_of_pulses)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_of_pulses\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&(pulse_format)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing pulse_format\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&(pulse_attributes)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing pulse_attributes\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&(pulse_size)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing pulse_size\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&(pulse_compression)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing pulse_compression\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(reserved)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing reserved\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&(number_of_variable_length_records)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_of_variable_length_records\n");
    return FALSE;
  }
  try { stream->put32bitsLE((U8*)&(number_of_appended_variable_length_records)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing number_of_appended_variable_length_records\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(t_scale_factor)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing t_scale_factor\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(t_offset)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing t_offset\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(min_T)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing min_T\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(max_T)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing max_T\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(x_scale_factor)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing x_scale_factor\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(y_scale_factor)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing y_scale_factor\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(z_scale_factor)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing z_scale_factor\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(x_offset)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing x_offset\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(y_offset)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing y_offset\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(z_offset)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing z_offset\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(min_x)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing min_x\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(max_x)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing max_x\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(min_y)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing min_y\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(max_y)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing max_y\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(min_z)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing min_z\n");
    return FALSE;
  }
  try { stream->put64bitsLE((U8*)&(max_z)); } catch(...)
  {
    fprintf(stderr,"ERROR: writing max_z\n");
    return FALSE;
  }

  // write any number of user-defined bytes that might have been added into the header

  if (user_data_in_header_size)
  {
    if (user_data_in_header)
    {
      try { stream->putBytes((U8*)user_data_in_header, user_data_in_header_size); } catch(...)
      {
        fprintf(stderr,"ERROR: writing %d bytes of data from user_data_in_header\n", user_data_in_header_size);
        return FALSE;
      }
    }
    else
    {
      fprintf(stderr,"ERROR: there should be %d bytes of data in user_data_in_header\n", user_data_in_header_size);
      return FALSE;
    }
  }

  // write header variable length records

  U32 i;
  for (i = 0; i < number_of_header_variable_length_records; i++)
  {
    if (!vlrs[i].save(stream))
    {
      fprintf(stderr,"ERROR: writing vlrs %u\n", i);
      return FALSE;
    }
  }

  // maybe write pulsezip VLR with compression parameters

  if (pulsezip)
  {
    char description[PULSEWAVES_DESCRIPTION_SIZE];
    sprintf(description, "PULSEzip %d.%d r%d of PulseWaves %d.%d r%d (%d) by rapidlasso", pulsezip->version_major, pulsezip->version_minor, pulsezip->version_revision, PULSEWAVES_VERSION_MAJOR, PULSEWAVES_VERSION_MINOR, PULSEWAVES_REVISION, PULSEWAVES_BUILD_DATE);

    ByteStreamOutArray* bytestreamoutarray;
    if (IS_LITTLE_ENDIAN())
      bytestreamoutarray = new ByteStreamOutArrayLE();
    else
      bytestreamoutarray = new ByteStreamOutArrayBE();
    if (bytestreamoutarray == 0)
    {
      fprintf(stderr,"ERROR: allocating bytestreamoutarray for writing PULSEzip VLR\n");
      return FALSE;
    }
    pulsezip->save(bytestreamoutarray);
    I64 record_length_after_header = bytestreamoutarray->getSize();
    U8* data = bytestreamoutarray->takeData();
    delete bytestreamoutarray;

    PULSEvlr pulsezip_vlr("PulseWaves_Spec", PULSEWAVES_PULSEZIP_RECORD_ID, description, record_length_after_header, data);
    pulsezip_vlr.save(stream);
  }

  // write any number of user-defined bytes that might have been added after the header

  if (user_data_after_header_size)
  {
    if (user_data_after_header)
    {
      try { stream->putBytes((U8*)user_data_after_header, user_data_after_header_size); } catch(...)
      {
        fprintf(stderr,"ERROR: writing %d bytes of data from user_data_after_header\n", user_data_after_header_size);
        return FALSE;
      }
    }
    else
    {
      fprintf(stderr,"ERROR: there should be %d bytes of data in user_data_after_header\n", user_data_after_header_size);
      return FALSE;
    }
  }
  return TRUE;
};

BOOL PULSEheader::save_to_txt(FILE* file) const
{
  // write header variable after variable

  fprintf(file, "file_signature:         %.16s\012", file_signature);
  fprintf(file, "global_parameters:      %d\012", global_parameters);
  fprintf(file, "file_source_ID:         %d\012", file_source_ID);
  fprintf(file, "project ID GUID:        %08X-%04X-%04X-%04X-%04X%08X\012", project_ID_GUID_data_1, project_ID_GUID_data_2, project_ID_GUID_data_3, *((U16*)(project_ID_GUID_data_4)), *((U16*)(project_ID_GUID_data_4+2)), *((U32*)(project_ID_GUID_data_4+4)));
  fprintf(file, "system_identifier:      '%.64s'\012", system_identifier);
  fprintf(file, "generating_software:    '%.64s'\012", generating_software);
  fprintf(file, "file_creation_day_year: %d %d\012", file_creation_day, file_creation_year);
  fprintf(file, "version:                %d.%d\012", version_major, version_minor);
  fprintf(file, "header_size:            %d\012", header_size);
#ifdef _WIN32
  fprintf(file, "offset_to_pulse_data:   %I64d\012", offset_to_pulse_data);
  fprintf(file, "number_of_pulses:       %I64d\012", number_of_pulses);
#else
  fprintf(file, "offset_to_pulse_data:   %lld\012", offset_to_pulse_data);
  fprintf(file, "number_of_pulses:       %lld\012", number_of_pulses);
#endif
  fprintf(file, "pulse_format:           %d\012", pulse_format);
  fprintf(file, "pulse_attributes:       %d\012", pulse_attributes);
  fprintf(file, "pulse_size:             %d\012", pulse_size);
  fprintf(file, "pulse_compression:      %d\012", pulse_compression);
#ifdef _WIN32
  fprintf(file, "reserved:               %I64d\012", reserved);
#else
  fprintf(file, "reserved:               %lld\012", reserved);
#endif
  fprintf(file, "number_of_vlrs:         %d\012", number_of_variable_length_records);
  fprintf(file, "number_of_avlrs:        %d\012", number_of_appended_variable_length_records);
  fprintf(file, "t_scale_factor:         %g\012", t_scale_factor);
  fprintf(file, "t_offset:               %g\012", t_offset);
#ifdef _WIN32
  fprintf(file, "min_T max_T             %I64d %I64d\012", min_T, max_T);
#else
  fprintf(file, "min_T max_T             %lld %lld\012", min_T, max_T);
#endif
  fprintf(file, "xyz_scale_factors:      %g %g %g\012", x_scale_factor, y_scale_factor, z_scale_factor);
  fprintf(file, "xyz_offsets:            %g %g %g\012", x_offset, y_offset, z_offset);
  fprintf(file, "min_x min_y min_z:      %g %g %g\012", min_x, min_y, min_z);
  fprintf(file, "max_x max_y max_z:      %g %g %g\012", max_x, max_y, max_z);

  // write any number of user-defined bytes that might have been added into the header

  if (user_data_in_header_size && user_data_in_header)
  {
    U32 i;
    for (i = 0; i < user_data_in_header_size; i++)
    {
      fprintf(file, "%d ", user_data_in_header[i]);
    }
    fprintf(file, "\012");
  }

  // write variable length records

  U32 i;
  for (i = 0; i < number_of_variable_length_records; i++)
  {
    if (!vlrs[i].save_to_txt(file))
    {
      fprintf(stderr,"ERROR: writing vlrs %u\n", i);
      return FALSE;
    }
  }

  // write any number of user-defined bytes that might have been added after the header

  if (user_data_after_header_size && user_data_after_header)
  {
    U32 i;
    for (i = 0; i < user_data_after_header_size; i++)
    {
      fprintf(file, "%d ", user_data_after_header[i]);
    }
    fprintf(file, "\012");
  }

  return TRUE;
};

PULSEheader::PULSEheader()
{
  clean_header();
};

PULSEheader::~PULSEheader()
{
  clean();
};
