/*
===============================================================================

  FILE:  pulsereadwaves_raw.hpp
  
  CONTENTS:
  
    Write waves raw.

  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2010-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    26 June 2012 -- created in sunny Sommerhausen eating Kuerbiskernbroetchen
  
===============================================================================
*/
#ifndef PULSE_READ_WAVES_RAW_HPP
#define PULSE_READ_WAVES_RAW_HPP

#include "pulsereadwaves.hpp"

class ByteStreamIn;

class PULSEreadWaves_raw : public PULSEreadWaves
{
public:

  BOOL init(ByteStreamIn* instream);
  BOOL read(WAVESwaves* waves);

  PULSEreadWaves_raw();
  ~PULSEreadWaves_raw();
};

#endif
