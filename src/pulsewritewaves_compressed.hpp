/*
===============================================================================

  FILE:  pulsewritewaves_compressed.hpp
  
  CONTENTS:
  
    Write waves compressed.

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
  
    4 July 2012 -- created in the ICE from FRA to MUC enroute to Salzburg AGIT  
  
===============================================================================
*/
#ifndef PULSE_WRITE_WAVES_COMPRESSED_HPP
#define PULSE_WRITE_WAVES_COMPRESSED_HPP

#include "pulsewritewaves.hpp"

class ArithmeticEncoder;
class IntegerCompressor;

class PULSEwriteWaves_compressed : public PULSEwriteWaves
{
public:

  BOOL init(ByteStreamOut* outstream);
  BOOL write(const WAVESwaves* waves);

  PULSEwriteWaves_compressed();
  ~PULSEwriteWaves_compressed();

private:
  ArithmeticEncoder* enc;
  IntegerCompressor* ic;
  IntegerCompressor* ic8;
  IntegerCompressor* ic16;
};

#endif
