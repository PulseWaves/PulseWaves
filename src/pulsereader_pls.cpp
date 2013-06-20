/*
===============================================================================

  FILE:  pulsereader_pls.cpp
  
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
#include "pulsereader_pls.hpp"

#include "bytestreamin.hpp"
#include "bytestreamin_file.hpp"
#include "bytestreamin_istream.hpp"
#include "pulsereadpulse.hpp"
#include "pulsereadwaves_raw.hpp"
#include "pulsereadwaves_compressed.hpp"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>

BOOL PULSEreaderPLS::open(const char* file_name, U32 io_buffer_size)
{
  if (file_name == 0)
  {
    fprintf(stderr,"ERROR: file name pointer is zero\n");
    return FALSE;
  }

  pulse_file = fopen(file_name, "rb");
  if (pulse_file == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", file_name);
    return FALSE;
  }

  if (setvbuf(pulse_file, NULL, _IOFBF, io_buffer_size) != 0)
  {
    fprintf(stderr, "WARNING: setvbuf() failed with buffer size %u\n", io_buffer_size);
  }

  // create input
  ByteStreamIn* in;
  if (IS_LITTLE_ENDIAN())
    in = new ByteStreamInFileLE(pulse_file);
  else
    in = new ByteStreamInFileBE(pulse_file);

  if (this->file_name) free(this->file_name);
  this->file_name = strdup(file_name);

  return open(in);
}

BOOL PULSEreaderPLS::open(FILE* file)
{
  if (file == 0)
  {
    fprintf(stderr,"ERROR: file pointer is zero\n");
    return FALSE;
  }

#ifdef _WIN32
  if (file == stdin)
  {
    if(_setmode( _fileno( stdin ), _O_BINARY ) == -1 )
    {
      fprintf(stderr, "ERROR: cannot set stdin to binary (untranslated) mode\n");
      return FALSE;
    }
  }
#endif

  // create input
  ByteStreamIn* in;
  if (IS_LITTLE_ENDIAN())
    in = new ByteStreamInFileLE(file);
  else
    in = new ByteStreamInFileBE(file);

  return open(in);
}

BOOL PULSEreaderPLS::open(istream& stream)
{
  // create input
  ByteStreamIn* in;
  if (IS_LITTLE_ENDIAN())
    in = new ByteStreamInIstreamLE(stream);
  else
    in = new ByteStreamInIstreamBE(stream);

  return open(in);
}

BOOL PULSEreaderPLS::open(ByteStreamIn* stream)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: ByteStreamIn* pointer is zero\n");
    return FALSE;
  }

  pulse_stream = stream;

  // load the header

  if (!header.load(stream))
  {
    fprintf(stderr,"ERROR: loading header\n");
    return FALSE;
  }

/*
  // check the compressor state

  if (header.pulsezip)
  {
    if (!header.pulsezip->check())
    {
      fprintf(stderr,"ERROR: %s\n", header.pulsezip->get_error());
      fprintf(stderr,"       please upgrade to the latest release of PULSEtools and if all\n");
      fprintf(stderr,"       fails contact 'martin.isenburg@rapidlasso.com' for assistance.\n");
      return FALSE;
    }
  }
*/

  // create the pulse reader

  pulse_reader = new PULSEreadPulse();

  // initialize pulse and the pulse_reader

  if (header.pulsezip)
  {
    if (!pulse.init(&header, header.pulsezip->num_items, header.pulsezip->items)) return FALSE;
    if (!pulse_reader->setup(header.pulsezip->num_items, header.pulsezip->items, header.pulsezip)) return FALSE;
  }
  else
  {
    if (!pulse.init(&header)) return FALSE;
    if (!pulse_reader->setup(pulse.num_items, pulse.items)) return FALSE;
  }
  if (!pulse_reader->init(stream)) return FALSE;

  npulses = header.number_of_pulses;
  p_count = 0;

  return TRUE;
}

BOOL PULSEreaderPLS::open_waves()
{
  if (file_name == 0)
  {
    return FALSE;
  }

  CHAR* temp_file_name = strdup(file_name);
  I32 len = strlen(temp_file_name);
  temp_file_name[len-3] = 'w';
  temp_file_name[len-2] = 'v';
  temp_file_name[len-1] = 'z';

  if (waves_file)
  {
    fclose(waves_file);
  }

  // try open compressed version
  waves_file = fopen(temp_file_name, "rb");
  if (waves_file == 0)
  {
    // try open uncompressed version
    temp_file_name[len-1] = 's';
    waves_file = fopen(temp_file_name, "rb");
    if (waves_file == 0)
    {
      fprintf(stderr, "ERROR: cannot open file '%s'\n", temp_file_name);
      free(temp_file_name);
      return FALSE;
    }
    else
    {
      decompress_waves = FALSE;
    }
  }
  else
  {
    decompress_waves = TRUE;
  }
  free(temp_file_name);

  if (waves_stream)
  {
    delete waves_stream;
  }

  if (IS_LITTLE_ENDIAN())
    waves_stream = new ByteStreamInFileLE(waves_file);
  else
    waves_stream = new ByteStreamInFileBE(waves_file);

  if (waves_stream == 0)
  {
    fprintf(stderr,"ERROR: waves_stream is zero\n");
    return FALSE;
  }

  if (decompress_waves)
  {
    waves_reader = new PULSEreadWaves_compressed();
  }
  else
  {
    waves_reader = new PULSEreadWaves_raw();
  }

  if (waves_reader == 0)
  {
    fprintf(stderr,"ERROR: allocating waves_reader\n");
    return FALSE;
  }

  if (!waves_reader->init(waves_stream))
  {
    fprintf(stderr,"ERROR: initializing waves_reader\n");
    return FALSE;
  }

  return TRUE;
}

I32 PULSEreaderPLS::get_format() const
{
  return PULSEWAVES_FORMAT_PLS;
}

BOOL PULSEreaderPLS::seek(const I64 p_index)
{
  if (p_index < npulses)
  {
    if (pulse_reader->seek((U32)p_count, (U32)p_index))
    {
      p_count = p_index;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL PULSEreaderPLS::read_pulse_default()
{
  if (p_count < npulses)
  {
    if (pulse_reader->read(pulse.pulse) == FALSE)
    {
      fprintf(stderr,"WARNING: end-of-file after %u of %u pulses\n", (U32)p_count, (U32)npulses);
      return FALSE;
    }
    p_count++;
    return TRUE;
  }
  return FALSE;
}

BOOL PULSEreaderPLS::read_waves()
{
  // maybe we need to open the waves file
  if (waves_stream == 0)
  {
    if (!open_waves())
    {
      return FALSE;
    }
  }
  waves_stream->seek(pulse.offset);
  if (waves_map[pulse.descriptor_index] == 0)
  {
    waves_map[pulse.descriptor_index] = new WAVESwaves();
    if (waves_map[pulse.descriptor_index] == 0)
    {
      return FALSE;
    }
    if (!waves_map[pulse.descriptor_index]->init(header.get_descriptor(pulse.descriptor_index)))
    {
      return FALSE;
    }
  }
  waves = waves_map[pulse.descriptor_index];
  if (!waves_reader->read(waves))
  {
#ifdef _WIN32
    fprintf(stderr,"ERROR: reading waves of pulse %I64d\n", p_count);
#else
    fprintf(stderr,"ERROR: reading waves of pulse %lld\n", p_count);
#endif
    return FALSE;
  }
  return TRUE;
}

ByteStreamIn* PULSEreaderPLS::get_pulse_stream() const
{
  return pulse_stream;
}

ByteStreamIn* PULSEreaderPLS::get_waves_stream() const
{
  return waves_stream;
}

void PULSEreaderPLS::close(BOOL close_streams)
{
  if (pulse_reader) 
  {
    pulse_reader->done();
    delete pulse_reader;
    pulse_reader = 0;
  }
  if (waves_reader) 
  {
    delete waves_reader;
    waves_reader = 0;
  }
  if (close_streams)
  {
    if (pulse_stream)
    {
      delete pulse_stream;
      pulse_stream = 0;
      if (pulse_file)
      {
        fclose(pulse_file);
        pulse_file = 0;
      }
    }
    if (waves_stream)
    {
      delete waves_stream;
      waves_stream = 0;
      if (waves_file)
      {
        fclose(waves_file);
        waves_file = 0;
      }
    }
  }
  for (I32 i = 0; i < 256; i++)
  {
    if (waves_map[i])
    {
      delete waves_map[i];
      waves_map[i] = 0;
    }
  }
}

PULSEreaderPLS::PULSEreaderPLS()
{
  decompress_waves = FALSE;
  file_name = 0;
  pulse_file = 0;
  waves_file = 0;
  pulse_stream = 0;
  waves_stream = 0;
  pulse_reader = 0;
  waves_reader = 0;
  memset(waves_map, 0, sizeof(WAVESwaves*)*256);
}

PULSEreaderPLS::~PULSEreaderPLS()
{
  if (file_name) free(file_name);
  if (pulse_reader || waves_reader || pulse_stream || waves_stream) close();
}
