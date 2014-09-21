/*
===============================================================================

  FILE:  pulsewritewaves.hpp
  
  CONTENTS:
  
    Common interface for the classes that write waves raw or compressed.

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
#ifndef PULSE_WRITE_WAVES_HPP
#define PULSE_WRITE_WAVES_HPP

#include "waveswaves.hpp"
#include "bytestreamout.hpp"

class PULSEwriteWaves
{
public:

  virtual BOOL init(ByteStreamOut* outstream) = 0;
  virtual BOOL write(const WAVESwaves* waves) = 0;

  inline I64 get_current_offset() const { if (outstream == 0) { return 0; } return outstream->tell(); }

  PULSEwriteWaves() { outstream = 0; };
  virtual ~PULSEwriteWaves() {};

protected:
  ByteStreamOut* outstream;
};

#endif
