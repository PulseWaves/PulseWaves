/*
===============================================================================

  FILE:  pulsewaves_dll.c
  
  CONTENTS:
      
    A simple set of linkable function signatures for the DLL of PulseWaves

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
  
    see header file

===============================================================================
*/

#include "pulsewaves_dll.h"

// DLL function definitions

#ifdef __cplusplus
extern "C"
{
#endif

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_get_version_def)
(
    pulsewaves_U8*                         version_major
    , pulsewaves_U8*                       version_minor
    , pulsewaves_U16*                      version_revision
    , pulsewaves_U32*                      version_build
);
pulsewaves_get_version_def pulsewaves_get_version_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_get_version
(
    pulsewaves_U8*                         version_major
    , pulsewaves_U8*                       version_minor
    , pulsewaves_U16*                      version_revision
    , pulsewaves_U32*                      version_build
)
{
  if (pulsewaves_get_version_ptr)
  {
    return (*pulsewaves_get_version_ptr)(version_major, version_minor, version_revision, version_build);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_create_def)
(
    pulsewaves_POINTER*                    pointer
);
pulsewaves_create_def pulsewaves_create_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_create
(
    pulsewaves_POINTER*                    pointer
)
{
  if (pulsewaves_create_ptr)
  {
    return (*pulsewaves_create_ptr)(pointer);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_get_error_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_CHAR**                    error
);
pulsewaves_get_error_def pulsewaves_get_error_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_get_error
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_CHAR**                    error
)
{
  if (pulsewaves_get_error_ptr)
  {
    return (*pulsewaves_get_error_ptr)(pointer, error);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_destroy_def)
(
    pulsewaves_POINTER       pointer
);
pulsewaves_destroy_def pulsewaves_destroy_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_destroy
(
    pulsewaves_POINTER       pointer
)
{
  if (pulsewaves_destroy_ptr)
  {
    return (*pulsewaves_destroy_ptr)(pointer);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_get_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_header_struct*            header
);
pulsewaves_header_get_def pulsewaves_header_get_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_header_struct*            header
)
{
  if (pulsewaves_header_get_ptr)
  {
    return (*pulsewaves_header_get_ptr)(pointer, header);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_set_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_header_struct*            header
);
pulsewaves_header_set_def pulsewaves_header_set_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_header_struct*            header
)
{
  if (pulsewaves_header_set_ptr)
  {
    return (*pulsewaves_header_set_ptr)(pointer, header);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_get_scanner_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_scanner_struct*           scanner
    , pulsewaves_U32                       scanner_index
);
pulsewaves_header_get_scanner_def pulsewaves_header_get_scanner_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_scanner
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_scanner_struct*           scanner
    , pulsewaves_U32                       scanner_index
)
{
  if (pulsewaves_header_get_scanner_ptr)
  {
    return (*pulsewaves_header_get_scanner_ptr)(pointer, scanner, scanner_index);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_add_scanner_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_scanner_struct*           scanner
    , pulsewaves_U32                       scanner_index
);
pulsewaves_header_add_scanner_def pulsewaves_header_add_scanner_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_scanner
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_scanner_struct*           scanner
    , pulsewaves_U32                       scanner_index
)
{
  if (pulsewaves_header_add_scanner_ptr)
  {
    return (*pulsewaves_header_add_scanner_ptr)(pointer, scanner, scanner_index);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_get_pulsedescriptor_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsecomposition_struct** composition
    , pulsewaves_pulsesampling_struct**    samplings
    , pulsewaves_U32                       descriptor_index
);
pulsewaves_header_get_pulsedescriptor_def pulsewaves_header_get_pulsedescriptor_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_pulsedescriptor
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsecomposition_struct** composition
    , pulsewaves_pulsesampling_struct**    samplings
    , pulsewaves_U32                       descriptor_index
)
{
  if (pulsewaves_header_get_pulsedescriptor_ptr)
  {
    return (*pulsewaves_header_get_pulsedescriptor_ptr)(pointer, composition, samplings, descriptor_index);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_add_pulsedescriptor_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsecomposition_struct*  composition
    , pulsewaves_pulsesampling_struct*     samplings
    , pulsewaves_U32                       descriptor_index
);
pulsewaves_header_add_pulsedescriptor_def pulsewaves_header_add_pulsedescriptor_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_pulsedescriptor
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsecomposition_struct*  composition
    , pulsewaves_pulsesampling_struct*     samplings
    , pulsewaves_U32                       descriptor_index
)
{
  if (pulsewaves_header_add_pulsedescriptor_ptr)
  {
    return (*pulsewaves_header_add_pulsedescriptor_ptr)(pointer, composition, samplings, descriptor_index);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_set_geokey_entries_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_pulsekeyentry_struct*     key_entries
);
pulsewaves_header_set_geokey_entries_def pulsewaves_header_set_geokey_entries_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set_geokey_entries
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_pulsekeyentry_struct*     key_entries
)
{
  if (pulsewaves_header_set_geokey_entries_ptr)
  {
    return (*pulsewaves_header_set_geokey_entries_ptr)(pointer, number, key_entries);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_get_geokey_entries_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_pulsekeyentry_struct**    key_entries
);
pulsewaves_header_get_geokey_entries_def pulsewaves_header_get_geokey_entries_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_geokey_entries
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_pulsekeyentry_struct**    key_entries
)
{
  if (pulsewaves_header_get_geokey_entries_ptr)
  {
    return (*pulsewaves_header_get_geokey_entries_ptr)(pointer, number, key_entries);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_set_geodouble_params_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_F64*                      geodouble_params
);
pulsewaves_header_set_geodouble_params_def pulsewaves_header_set_geodouble_params_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set_geodouble_params
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_F64*                      geodouble_params
)
{
  if (pulsewaves_header_set_geodouble_params_ptr)
  {
    return (*pulsewaves_header_set_geodouble_params_ptr)(pointer, number, geodouble_params);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_get_geodouble_params_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_F64**                     geodouble_params
);
pulsewaves_header_get_geodouble_params_def pulsewaves_header_get_geodouble_params_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_geodouble_params
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_F64**                     geodouble_params
)
{
  if (pulsewaves_header_get_geodouble_params_ptr)
  {
    return (*pulsewaves_header_get_geodouble_params_ptr)(pointer, number, geodouble_params);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_set_geoascii_params_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_CHAR*                     geoascii_params
);
pulsewaves_header_set_geoascii_params_def pulsewaves_header_set_geoascii_params_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_set_geoascii_params
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32                       number
    , pulsewaves_CHAR*                     geoascii_params
)
{
  if (pulsewaves_header_set_geoascii_params_ptr)
  {
    return (*pulsewaves_header_set_geoascii_params_ptr)(pointer, number, geoascii_params);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_get_geoascii_params_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_CHAR**                    geoascii_params
);
pulsewaves_header_get_geoascii_params_def pulsewaves_header_get_geoascii_params_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_geoascii_params
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_U32*                      number
    , pulsewaves_CHAR**                    geoascii_params
)
{
  if (pulsewaves_header_get_geoascii_params_ptr)
  {
    return (*pulsewaves_header_get_geoascii_params_ptr)(pointer, number, geoascii_params);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_add_vlr_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsevlr_struct*          vlr
    , pulsewaves_U8*                       data
);
pulsewaves_header_add_vlr_def pulsewaves_header_add_vlr_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_add_vlr
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsevlr_struct*          vlr
    , pulsewaves_U8*                       data
)
{
  if (pulsewaves_header_add_vlr_ptr)
  {
    return (*pulsewaves_header_add_vlr_ptr)(pointer, vlr, data);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_header_get_vlr_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsevlr_struct*          vlr
    , pulsewaves_U8**                      data
    , pulsewaves_U32                       index
);
pulsewaves_header_get_vlr_def pulsewaves_header_get_vlr_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_header_get_vlr
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulsevlr_struct*          vlr
    , pulsewaves_U8**                      data
    , pulsewaves_U32                       index
)
{
  if (pulsewaves_header_get_vlr_ptr)
  {
    return (*pulsewaves_header_get_vlr_ptr)(pointer, vlr, data, index);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_writer_open_def)
(
    pulsewaves_POINTER                     pointer
    , const pulsewaves_CHAR*               file_name
    , pulsewaves_BOOL                      compress
);
pulsewaves_writer_open_def pulsewaves_writer_open_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_open
(
    pulsewaves_POINTER                     pointer
    , const pulsewaves_CHAR*               file_name
    , pulsewaves_BOOL                      compress
)
{
  if (pulsewaves_writer_open_ptr)
  {
    return (*pulsewaves_writer_open_ptr)(pointer, file_name, compress);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_writer_write_pulse_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulse_struct*             pulse
);
pulsewaves_writer_write_pulse_def pulsewaves_writer_write_pulse_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_write_pulse
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulse_struct*             pulse
)
{
  if (pulsewaves_writer_write_pulse_ptr)
  {
    return (*pulsewaves_writer_write_pulse_ptr)(pointer, pulse);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_writer_write_waves_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_wavessampling_struct*     wavessamplings
    , pulsewaves_U32                       number_of_wavessamplings
);
pulsewaves_writer_write_waves_def pulsewaves_writer_write_waves_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_write_waves
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_wavessampling_struct*     wavessamplings
    , pulsewaves_U32                       number_of_wavessamplings
)
{
  if (pulsewaves_writer_write_waves_ptr)
  {
    return (*pulsewaves_writer_write_waves_ptr)(pointer, wavessamplings, number_of_wavessamplings);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_writer_close_def)
(
    pulsewaves_POINTER                     pointer
);
pulsewaves_writer_close_def pulsewaves_writer_close_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_writer_close
(
    pulsewaves_POINTER                     pointer
)
{
  if (pulsewaves_writer_close_ptr)
  {
    return (*pulsewaves_writer_close_ptr)(pointer);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_reader_open_def)
(
    pulsewaves_POINTER                     pointer
    , const pulsewaves_CHAR*               file_name
    , pulsewaves_BOOL*                     is_compressed
);
pulsewaves_reader_open_def pulsewaves_reader_open_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_open
(
    pulsewaves_POINTER                     pointer
    , const pulsewaves_CHAR*               file_name
    , pulsewaves_BOOL*                     is_compressed
)
{
  if (pulsewaves_reader_open_ptr)
  {
    return (*pulsewaves_reader_open_ptr)(pointer, file_name, is_compressed);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_reader_seek_pulse_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_I64                       index
);
pulsewaves_reader_seek_pulse_def pulsewaves_reader_seek_pulse_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_seek_pulse(
    pulsewaves_POINTER                     pointer
    , pulsewaves_I64                       index
)
{
  if (pulsewaves_reader_seek_pulse_ptr)
  {
    return (*pulsewaves_reader_seek_pulse_ptr)(pointer, index);
  }
  return 1;
}

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_reader_read_pulse_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulse_struct*             pulse
);
pulsewaves_reader_read_pulse_def pulsewaves_reader_read_pulse_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_read_pulse(
    pulsewaves_POINTER                     pointer
    , pulsewaves_pulse_struct*             pulse
)
{
  if (pulsewaves_reader_read_pulse_ptr)
  {
    return (*pulsewaves_reader_read_pulse_ptr)(pointer, pulse);
  }
  return 1;
}

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_reader_read_waves_def)
(
    pulsewaves_POINTER                     pointer
    , pulsewaves_wavessampling_struct**    wavessamplings
    , pulsewaves_U32*                      number_of_wavessamplings
);
pulsewaves_reader_read_waves_def pulsewaves_reader_read_waves_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_read_waves(
    pulsewaves_POINTER                     pointer
    , pulsewaves_wavessampling_struct**    wavessamplings
    , pulsewaves_U32*                      number_of_wavessamplings
)
{
  if (pulsewaves_reader_read_waves_ptr)
  {
    return (*pulsewaves_reader_read_waves_ptr)(pointer, wavessamplings, number_of_wavessamplings);
  }
  return 1;
}

/*---------------------------------------------------------------------------*/
typedef pulsewaves_I32 (*pulsewaves_reader_close_def)
(
    pulsewaves_POINTER                     pointer
);
pulsewaves_reader_close_def pulsewaves_reader_close_ptr = 0;
PULSEWAVES_API pulsewaves_I32
pulsewaves_reader_close
(
    pulsewaves_POINTER                     pointer
)
{
  if (pulsewaves_reader_close_ptr)
  {
    return (*pulsewaves_reader_close_ptr)(pointer);
  }
  return 1;
};

/*---------------------------------------------------------------------------*/
#include <windows.h>
pulsewaves_I32 pulsewaves_load_dll()
{
  // Load DLL file
  HINSTANCE hinstLib = LoadLibrary(TEXT("pulsewaves.dll"));
  if (hinstLib == NULL) {
     return 1;
  }

  // Get function pointers
  pulsewaves_get_version_ptr = (pulsewaves_get_version_def)GetProcAddress(hinstLib, "pulsewaves_get_version");
  if (pulsewaves_get_version_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_create_ptr = (pulsewaves_create_def)GetProcAddress(hinstLib, "pulsewaves_create");
  if (pulsewaves_create_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_get_error_ptr = (pulsewaves_get_error_def)GetProcAddress(hinstLib, "pulsewaves_get_error");
  if (pulsewaves_get_error_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_destroy_ptr = (pulsewaves_destroy_def)GetProcAddress(hinstLib, "pulsewaves_destroy");
  if (pulsewaves_destroy_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_get_ptr = (pulsewaves_header_get_def)GetProcAddress(hinstLib, "pulsewaves_header_get");
  if (pulsewaves_header_get_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_set_ptr = (pulsewaves_header_set_def)GetProcAddress(hinstLib, "pulsewaves_header_set");
  if (pulsewaves_header_set_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_get_scanner_ptr = (pulsewaves_header_get_scanner_def)GetProcAddress(hinstLib, "pulsewaves_header_get_scanner");
  if (pulsewaves_header_get_scanner_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_add_scanner_ptr = (pulsewaves_header_add_scanner_def)GetProcAddress(hinstLib, "pulsewaves_header_add_scanner");
  if (pulsewaves_header_add_scanner_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_get_pulsedescriptor_ptr = (pulsewaves_header_get_pulsedescriptor_def)GetProcAddress(hinstLib, "pulsewaves_header_get_pulsedescriptor");
  if (pulsewaves_header_get_pulsedescriptor_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_add_pulsedescriptor_ptr = (pulsewaves_header_add_pulsedescriptor_def)GetProcAddress(hinstLib, "pulsewaves_header_add_pulsedescriptor");
  if (pulsewaves_header_add_pulsedescriptor_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_set_geokey_entries_ptr = (pulsewaves_header_set_geokey_entries_def)GetProcAddress(hinstLib, "pulsewaves_header_set_geokey_entries");
  if (pulsewaves_header_set_geokey_entries_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_get_geokey_entries_ptr = (pulsewaves_header_get_geokey_entries_def)GetProcAddress(hinstLib, "pulsewaves_header_get_geokey_entries");
  if (pulsewaves_header_get_geokey_entries_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_set_geodouble_params_ptr = (pulsewaves_header_set_geodouble_params_def)GetProcAddress(hinstLib, "pulsewaves_header_set_geodouble_params");
  if (pulsewaves_header_set_geodouble_params_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_set_geoascii_params_ptr = (pulsewaves_header_set_geoascii_params_def)GetProcAddress(hinstLib, "pulsewaves_header_set_geoascii_params");
  if (pulsewaves_header_set_geoascii_params_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_header_add_vlr_ptr = (pulsewaves_header_add_vlr_def)GetProcAddress(hinstLib, "pulsewaves_header_add_vlr");
  if (pulsewaves_header_add_vlr_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_writer_open_ptr = (pulsewaves_writer_open_def)GetProcAddress(hinstLib, "pulsewaves_writer_open");
  if (pulsewaves_writer_open_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_writer_write_pulse_ptr = (pulsewaves_writer_write_pulse_def)GetProcAddress(hinstLib, "pulsewaves_writer_write_pulse");
  if (pulsewaves_writer_write_pulse_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_writer_write_waves_ptr = (pulsewaves_writer_write_waves_def)GetProcAddress(hinstLib, "pulsewaves_writer_write_waves");
  if (pulsewaves_writer_write_waves_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_writer_close_ptr = (pulsewaves_writer_close_def)GetProcAddress(hinstLib, "pulsewaves_writer_close");
  if (pulsewaves_writer_close_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_reader_open_ptr = (pulsewaves_reader_open_def)GetProcAddress(hinstLib, "pulsewaves_reader_open");
  if (pulsewaves_reader_open_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_reader_seek_pulse_ptr = (pulsewaves_reader_seek_pulse_def)GetProcAddress(hinstLib, "pulsewaves_reader_seek_pulse");
  if (pulsewaves_reader_seek_pulse_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_reader_read_pulse_ptr = (pulsewaves_reader_read_pulse_def)GetProcAddress(hinstLib, "pulsewaves_reader_read_pulse");
  if (pulsewaves_reader_read_pulse_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_reader_read_waves_ptr = (pulsewaves_reader_read_waves_def)GetProcAddress(hinstLib, "pulsewaves_reader_read_waves");
  if (pulsewaves_reader_read_waves_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  pulsewaves_reader_close_ptr = (pulsewaves_reader_close_def)GetProcAddress(hinstLib, "pulsewaves_reader_close");
  if (pulsewaves_reader_close_ptr == NULL) {
     FreeLibrary(hinstLib);
     return 1;
  }
  return 0;
};

#ifdef __cplusplus
}
#endif
