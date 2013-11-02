/*
===============================================================================

  FILE:  pulsewavesdefinitions.hpp
  
  CONTENTS:
  
    A definition file that contains version numbers and many other constants.

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
  
    16 February 2012 -- created in the cafe where it smelled like 1917 Chestnut
  
===============================================================================
*/
#ifndef PULSEWAVES_DEFINITIONS_HPP
#define PULSEWAVES_DEFINITIONS_HPP

#define PULSEWAVES_VERSION_MAJOR     0
#define PULSEWAVES_VERSION_MINOR     3
#define PULSEWAVES_REVISION          8
#define PULSEWAVES_BUILD_DATE        131102
#define PULSE_TOOLS_VERSION          PULSEWAVES_BUILD_DATE

#define PULSEWAVES_UNDEFINED         0

#define PULSEWAVES_OUTGOING          1
#define PULSEWAVES_RETURNING         2

#define PULSEWAVES_OSCILLATING       1
#define PULSEWAVES_LINE              2
#define PULSEWAVES_CONIC             3

#define PULSEWAVES_OPTICAL_CENTER_AND_ANCHOR_POINT_COINCIDE  ((I32)0x0)
#define PULSEWAVES_OPTICAL_CENTER_AND_ANCHOR_POINT_FLUCTUATE ((I32)0x8FFFFFFF)

#define PULSEWAVES_UNCOMPRESSED      0

#define PULSEWAVES_FORMAT_DEFAULT 0
#define PULSEWAVES_FORMAT_PLS     1
#define PULSEWAVES_FORMAT_PLZ     2
#define PULSEWAVES_FORMAT_LAS     3
#define PULSEWAVES_FORMAT_LAZ     4
#define PULSEWAVES_FORMAT_LGW     5
#define PULSEWAVES_FORMAT_GCW     6
#define PULSEWAVES_FORMAT_SDF     7
#define PULSEWAVES_FORMAT_DAT     8
#define PULSEWAVES_FORMAT_TXT     9

#define PULSEWAVES_EXTRABYTES_RECORD_ID              4
#define PULSEWAVES_PULSEZIP_RECORD_ID                5

#define PULSEWAVES_SCANNER_MIN                       1
#define PULSEWAVES_SCANNER_MAX                     255
#define PULSEWAVES_SCANNER_RECORD_ID            100000
#define PULSEWAVES_SCANNER_RECORD_ID_MIN    (PULSEWAVES_SCANNER_RECORD_ID + PULSEWAVES_SCANNER_MIN)
#define PULSEWAVES_SCANNER_RECORD_ID_MAX    (PULSEWAVES_SCANNER_RECORD_ID + PULSEWAVES_SCANNER_MAX)

#define PULSEWAVES_DESCRIPTOR_MIN                    1
#define PULSEWAVES_DESCRIPTOR_MAX                  255
#define PULSEWAVES_DESCRIPTOR_RECORD_ID         200000
#define PULSEWAVES_DESCRIPTOR_RECORD_ID_MIN (PULSEWAVES_DESCRIPTOR_RECORD_ID + PULSEWAVES_DESCRIPTOR_MIN)
#define PULSEWAVES_DESCRIPTOR_RECORD_ID_MAX (PULSEWAVES_DESCRIPTOR_RECORD_ID + PULSEWAVES_DESCRIPTOR_MAX)

#define PULSEWAVES_TABLE_MIN                         1
#define PULSEWAVES_TABLE_MAX                       255
#define PULSEWAVES_TABLE_RECORD_ID              300000
#define PULSEWAVES_TABLE_RECORD_ID_MIN      (PULSEWAVES_TABLE_RECORD_ID + PULSEWAVES_TABLE_MIN)
#define PULSEWAVES_TABLE_RECORD_ID_MAX      (PULSEWAVES_TABLE_RECORD_ID + PULSEWAVES_TABLE_MAX)

#define PULSEWAVES_USER_ID_SIZE             16
#define PULSEWAVES_DESCRIPTION_SIZE         64
#define PULSEWAVES_VLRHEADER_SIZE           96
#define PULSEWAVES_PULSEHEADER_SIZE        352

#define PULSEWAVES_GEO_KEY_SIZE              8

#define PULSEWAVES_PULSE0                    0
#define PULSEWAVES_PULSESOURCEID16  0x00000001
#define PULSEWAVES_PULSESOURCEID32  0x00000002

#define PULSEWAVES_PULSE0_SIZE              48
#define PULSEWAVES_PULSESOURCEID16_SIZE      2
#define PULSEWAVES_PULSESOURCEID32_SIZE      4

#endif
