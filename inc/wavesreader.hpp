/*
===============================================================================

  FILE:  wavesreader.hpp
  
  CONTENTS:
  
    Abstract interface class to read Waves (as if) from the PulseWaves format.

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
#ifndef WAVES_READER_HPP
#define WAVES_READER_HPP

//#include "wavesheader.hpp"
//#include "waveswaves.hpp"

class ByteStreamIn;

class WAVESreader
{
public:
//  WAVESheader header;
//  WAVESwaves waves;

  I64 nsamples;
  I64 s_count;

  virtual BOOL read_waves() = 0;

  virtual ByteStreamIn* get_stream() const = 0;
  virtual void close(BOOL close_stream=TRUE) = 0;

  WAVESreader();
  virtual ~WAVESreader();
};

#endif
