/*
===============================================================================

  FILE:  waveswriter_pls.hpp
  
  CONTENTS:
  
    Writes Waves in the PulseWaves (*.pls/*.wvs) format.

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
  
    19 June 2012 -- created in the train on the way to Geosummit 2012 in Berne
  
===============================================================================
*/
#ifndef WAVES_WRITER_PLS_HPP
#define WAVES_WRITER_PLS_HPP

#include "waveswriter.hpp"

#include <stdio.h>

#ifdef LZ_WIN32_VC6
#include <fstream.h>
#else
#include <istream>
#include <fstream>
using namespace std;
#endif

class ByteStreamOut;
class WAVESwritePulse;

class WAVESwriterPLS : public WAVESwriter
{
public:

  BOOL refile(FILE* file);

  BOOL open(const PULSEheader* header, U32 compressor=0);
  BOOL open(const char* file_name, const PULSEheader* header, U32 compressor=0, U32 io_buffer_size=65536);
  BOOL open(FILE* file, const PULSEheader* header, U32 compressor=0);
  BOOL open(ostream& ostream, const PULSEheader* header, U32 compressor=0);

  BOOL write_pulse(const PULSEpulse* pulse);

  BOOL update_header(const PULSEheader* header, BOOL use_inventory=TRUE, BOOL update_extra_bytes=FALSE);
  I64 close(BOOL update_npulses=true);

  WAVESwriterPLS();
  ~WAVESwriterPLS();

private:
  BOOL open(ByteStreamOut* stream, const PULSEheader* header, U32 compressor);
  ByteStreamOut* stream;
  WAVESwritePulse* writer;
  FILE* file;
  I64 header_start_position;
};

#endif
