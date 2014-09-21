/*
===============================================================================

  FILE:  pulsewriter_pls.cpp
  
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
#include "pulsewriter_pls.hpp"

#include "pulsezip.hpp"
#include "bytestreamout_nil.hpp"
#include "bytestreamout_file.hpp"
#include "bytestreamout_ostream.hpp"
#include "pulsewritepulse.hpp"
#include "pulsewritewaves_raw.hpp"
#include "pulsewritewaves_compressed.hpp"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>

BOOL PULSEwriterPLS::refile_pulse(FILE* pulse_file)
{
  if (pulse_stream == 0) return FALSE;
  if (this->pulse_file) this->pulse_file = pulse_file;
  return ((ByteStreamOutFile*)pulse_stream)->refile(pulse_file);
}

BOOL PULSEwriterPLS::refile_waves(FILE* waves_file)
{
  if (waves_stream == 0) return FALSE;
  if (this->waves_file) this->waves_file = waves_file;
  return ((ByteStreamOutFile*)waves_stream)->refile(waves_file);
}

BOOL PULSEwriterPLS::open(PULSEheader* header, U32 compress)
{
  ByteStreamOut* out = new ByteStreamOutNil();
  return open(out, header, compress);
}

BOOL PULSEwriterPLS::open(const char* file_name, PULSEheader* header, U32 compress, U32 io_buffer_size)
{
  if (file_name == 0)
  {
    fprintf(stderr,"ERROR: file name pointer is zero\n");
    return FALSE;
  }

  pulse_file = fopen(file_name, "wb");
  if (pulse_file == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", file_name);
    return FALSE;
  }

  if (setvbuf(pulse_file, NULL, _IOFBF, io_buffer_size) != 0)
  {
    fprintf(stderr, "WARNING: setvbuf() failed with buffer size %u\n", io_buffer_size);
  }

  ByteStreamOut* out;
  if (IS_LITTLE_ENDIAN())
    out = new ByteStreamOutFileLE(pulse_file);
  else
    out = new ByteStreamOutFileBE(pulse_file);

  if (this->file_name) free(this->file_name);
  this->file_name = strdup(file_name);

  return open(out, header, compress);
}

BOOL PULSEwriterPLS::open(FILE* file, PULSEheader* header, U32 compress)
{
  if (file == 0)
  {
    fprintf(stderr,"ERROR: file pointer is zero\n");
    return FALSE;
  }

#ifdef _WIN32
  if (file == stdout)
  {
    if(_setmode( _fileno( stdout ), _O_BINARY ) == -1 )
    {
      fprintf(stderr, "ERROR: cannot set stdout to binary (untranslated) mode\n");
    }
  }
#endif

  ByteStreamOut* out;
  if (IS_LITTLE_ENDIAN())
    out = new ByteStreamOutFileLE(file);
  else
    out = new ByteStreamOutFileBE(file);

  return open(out, header, compress);
}

BOOL PULSEwriterPLS::open(ostream& stream, PULSEheader* header, U32 compress)
{
  ByteStreamOut* out;
  if (IS_LITTLE_ENDIAN())
    out = new ByteStreamOutOstreamLE(stream);
  else
    out = new ByteStreamOutOstreamBE(stream);

  return open(out, header, compress);
}

BOOL PULSEwriterPLS::open(ByteStreamOut* stream, PULSEheader* header, U32 compress)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: ByteStreamOut pointer is zero\n");
    return FALSE;
  }

  this->pulse_stream = stream;

  if (header == 0)
  {
    fprintf(stderr,"ERROR: PULSEheader pointer is zero\n");
    return FALSE;
  }

  // check header contents

  if (!header->check()) return FALSE;

  // copy scale_and_offset

  quantizer.x_scale_factor = header->x_scale_factor;
  quantizer.y_scale_factor = header->y_scale_factor;
  quantizer.z_scale_factor = header->z_scale_factor;
  quantizer.x_offset = header->x_offset;
  quantizer.y_offset = header->y_offset;
  quantizer.z_offset = header->z_offset;

  // check if the requested pulse type is supported

  BOOL pulse_is_standard = TRUE;
  U32 pulse_format;
  U32 pulse_attributes;
  U32 pulse_size;
  PULSEpulse pulse;
  if (header->pulsezip)
  {
    if (!pulse.init(&quantizer, header->pulsezip->num_items, header->pulsezip->items, header)) return FALSE;
    pulse_is_standard = header->pulsezip->is_standard(&pulse_format, &pulse_attributes, &pulse_size);
    delete header->pulsezip;
    header->pulsezip = 0;
  }
  else
  {
    if (!pulse.init(header, header)) return FALSE;
    pulse_format = header->pulse_format;
    pulse_attributes = header->pulse_attributes;
    pulse_size = header->pulse_size;
  }

  // do we need a new pulsezip VLR (because we compress or use non-standard pulses?) 

  if (compress || pulse_is_standard == FALSE)
  {
    header->pulsezip = new PULSEzip();
    header->pulsezip->setup(pulse.num_items, pulse.items, compress);
//    if (chunk_size > -1) header->pulsezip->set_chunk_size((U32)chunk_size);
    if (compress == PULSEZIP_COMPRESSOR_NONE)
    {
      header->pulsezip->request_version(0);
    }
    else
    {
      header->pulsezip->request_version(1);
    }
  }

  // create and setup the pulse writer

  pulse_writer = new PULSEwritePulse();
  if (header->pulsezip)
  {
    if (!pulse_writer->setup(header->pulsezip->num_items, header->pulsezip->items, header->pulsezip)) return FALSE;
  }
  else
  {
    if (!pulse_writer->setup(pulse.num_items, pulse.items)) return FALSE;
  }

  // write the header

  if (!header->save(stream))
  {
    fprintf(stderr,"ERROR: saving the header\n");
    return FALSE;
  }

  // initialize the pulse writer

  if (!pulse_writer->init(stream))
  {
    fprintf(stderr,"ERROR: initializing the pulse writer\n");
    return FALSE;
  }

  this->compress = compress;

  npulses = header->number_of_pulses;
  p_count = 0;

  return TRUE;
}

BOOL PULSEwriterPLS::open_waves()
{
  if (file_name == 0)
  {
    return FALSE;
  }

  CHAR* temp_file_name = strdup(file_name);
  I32 len = strlen(temp_file_name);
  temp_file_name[len-3] = 'w';
  temp_file_name[len-2] = 'v';
  temp_file_name[len-1] = (compress ? 'z' : 's');

  if (waves_file)
  {
    fclose(waves_file);
  }

  waves_file = fopen(temp_file_name, "wb");
  if (waves_file == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", temp_file_name);
    free(temp_file_name);
    return FALSE;
  }
  free(temp_file_name);

  if (waves_stream)
  {
    delete waves_stream;
  }

  if (IS_LITTLE_ENDIAN())
    waves_stream = new ByteStreamOutFileLE(waves_file);
  else
    waves_stream = new ByteStreamOutFileBE(waves_file);

  if (waves_stream == 0)
  {
    fprintf(stderr,"ERROR: allocating waves_stream\n");
    return FALSE;
  }

  WAVESheader waves_header;

  if (compress)
  {
    waves_header.compression = 1;
  }
  else
  {
    waves_header.compression = 0;
  }

  if (!waves_header.save(waves_stream))
  {
    fprintf(stderr,"ERROR: saving WAVESheader\n");
    return FALSE;
  }

  if (compress)
  {
    waves_writer = new PULSEwriteWaves_compressed();
  }
  else
  {
    waves_writer = new PULSEwriteWaves_raw();
  }

  if (waves_writer == 0)
  {
    fprintf(stderr,"ERROR: allocating waves_writer\n");
    return FALSE;
  }

  if (!waves_writer->init(waves_stream))
  {
    fprintf(stderr,"ERROR: initializing waves_writer\n");
    return FALSE;
  }

  return TRUE;
}

BOOL PULSEwriterPLS::write_pulse(const PULSEpulse* pulse)
{
  p_count++;
  return pulse_writer->write(pulse->pulse);
}

BOOL PULSEwriterPLS::write_waves(const WAVESwaves* waves)
{
  return waves_writer->write(waves);
}

I64 PULSEwriterPLS::get_current_offset() const
{
  if (waves_writer == 0) 
  {
    return 0;
  }
  return waves_writer->get_current_offset();
}

BOOL PULSEwriterPLS::update_header(const PULSEheader* header, BOOL use_inventory, BOOL update_extra_bytes)
{
  if (pulse_stream == 0)
  {
    fprintf(stderr,"ERROR: pulse_stream pointer is zero\n");
    return FALSE;
  }
  if (!pulse_stream->isSeekable())
  {
    fprintf(stderr,"ERROR: pulse_stream is not seekable\n");
    return FALSE;
  }

  if (use_inventory && inventory.active())
  {
    pulse_stream->seek(184);
    try { pulse_stream->put64bitsLE((U8*)&(inventory.number_of_pulses)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating inventory.number_of_pulses\n");
      return FALSE;
    }
    npulses = inventory.number_of_pulses;
    pulse_stream->seek(240);
    try { pulse_stream->put64bitsLE((U8*)&(inventory.min_T)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->min_T\n");
      return FALSE;
    }
    try { pulse_stream->put64bitsLE((U8*)&(inventory.max_T)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->max_T\n");
      return FALSE;
    }
    pulse_stream->seek(304);
    F64 value;
    value = quantizer.get_x(quantizer.get_X(inventory.min_x)-1);
    try { pulse_stream->put64bitsLE((U8*)&value); } catch(...)
    {
      fprintf(stderr,"ERROR: updating inventory.min_x\n");
      return FALSE;
    }
    value = quantizer.get_x(quantizer.get_X(inventory.max_x)+1);
    try { pulse_stream->put64bitsLE((U8*)&value); } catch(...)
    {
      fprintf(stderr,"ERROR: updating inventory.max_x\n");
      return FALSE;
    }
    value = quantizer.get_y(quantizer.get_Y(inventory.min_y)-1);
    try { pulse_stream->put64bitsLE((U8*)&value); } catch(...)
    {
      fprintf(stderr,"ERROR: updating inventory.min_y\n");
      return FALSE;
    }
    value = quantizer.get_y(quantizer.get_Y(inventory.max_y)+1);
    try { pulse_stream->put64bitsLE((U8*)&value); } catch(...)
    {
      fprintf(stderr,"ERROR: updating inventory.max_y\n");
      return FALSE;
    }
    value = quantizer.get_z(quantizer.get_Z(inventory.min_z)-1);
    try { pulse_stream->put64bitsLE((U8*)&value); } catch(...)
    {
      fprintf(stderr,"ERROR: updating inventory.min_z\n");
      return FALSE;
    }
    value = quantizer.get_z(quantizer.get_Z(inventory.max_z)+1);
    try { pulse_stream->put64bitsLE((U8*)&value); } catch(...)
    {
      fprintf(stderr,"ERROR: updating inventory.max_z\n");
      return FALSE;
    }
  }
  else
  {
    if (header == 0)
    {
      fprintf(stderr,"ERROR: header pointer is zero\n");
      return FALSE;
    }
    pulse_stream->seek(184);
    try { pulse_stream->put64bitsLE((U8*)&(header->number_of_pulses)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->number_of_pulses\n");
      return FALSE;
    }
    pulse_stream->seek(240);
    try { pulse_stream->put64bitsLE((U8*)&(header->min_T)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->min_T\n");
      return FALSE;
    }
    try { pulse_stream->put64bitsLE((U8*)&(header->max_T)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->max_T\n");
      return FALSE;
    }
    pulse_stream->seek(304);
    try { pulse_stream->put64bitsLE((U8*)&(header->min_x)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->min_x\n");
      return FALSE;
    }
    try { pulse_stream->put64bitsLE((U8*)&(header->max_x)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->max_x\n");
      return FALSE;
    }
    try { pulse_stream->put64bitsLE((U8*)&(header->min_y)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->min_y\n");
      return FALSE;
    }
    try { pulse_stream->put64bitsLE((U8*)&(header->max_y)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->max_y\n");
      return FALSE;
    }
    try { pulse_stream->put64bitsLE((U8*)&(header->min_z)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->min_z\n");
      return FALSE;
    }
    try { pulse_stream->put64bitsLE((U8*)&(header->max_z)); } catch(...)
    {
      fprintf(stderr,"ERROR: updating header->max_z\n");
      return FALSE;
    }
  }
  pulse_stream->seekEnd();
  /*
  if (update_extra_bytes)
  {
    if (header == 0)
    {
      fprintf(stderr,"ERROR: header pointer is zero\n");
      return FALSE;
    }
    if (header->number_extra_attributes)
    {
      I64 start = header->header_size;
      for (i = 0; i < (I32)header->number_of_variable_length_records; i++)
      {
        start += 54;
        if ((header->vlrs[i].record_id == 4) && (strcmp(header->vlrs[i].user_id, "LASF_Spec") == 0))
        {
          break;
        }
        else
        {
          start += header->vlrs[i].record_length_after_header;
        }
      }
      if (i == (I32)header->number_of_variable_length_records)
      {
        fprintf(stderr,"WARNING: could not find extra bytes VLR for update\n");
      }
      else
      {
        pulse_stream->seek(start);
        try { pulse_stream->putBytes((U8*)header->vlrs[i].data, header->vlrs[i].record_length_after_header))
        {
          fprintf(stderr,"ERROR: writing %d bytes of data from header->vlrs[%d].data\n", header->vlrs[i].record_length_after_header, i);
          return FALSE;
        }
      }
    }
    pulse_stream->seekEnd();
  }
  */
  return TRUE;
}

I64 PULSEwriterPLS::close(BOOL update_header)
{
  I64 bytes = 0;

  if (p_count != npulses)
  {
#ifdef _WIN32
    fprintf(stderr,"WARNING: written %I64d pulses but expected %I64d pulses\n", p_count, npulses);
#else
    fprintf(stderr,"WARNING: written %lld pulses but expected %lld pulses\n", p_count, npulses);
#endif
  }

  if (pulse_writer) 
  {
    pulse_writer->done();
    delete pulse_writer;
    pulse_writer = 0;
  }

  if (waves_writer) 
  {
    delete waves_writer;
    waves_writer = 0;
  }

  if (pulse_stream)
  {
    // update pulse count if needed

    if (update_header && p_count != npulses)
    {
      if (!pulse_stream->isSeekable())
      {
#ifdef _WIN32
        fprintf(stderr, "ERROR: pulse_stream not seekable. cannot update header from %I64d to %I64d pulses.\n", npulses, p_count);
#else
        fprintf(stderr, "ERROR: pulse_stream not seekable. cannot update header from %lld to %lld pulses.\n", npulses, p_count);
#endif
      }
      else
      {
	      pulse_stream->seek(184);
	      pulse_stream->put64bitsLE((U8*)&p_count);
        pulse_stream->seekEnd();
      }
    }

    // write final AVLR

    PULSEavlr pulseavlr("PulseWaves_Spec", 0xFFFFFFFF, "end of reverse list of Appended Variable Length Records (AVLRs)");
    pulseavlr.save(pulse_stream);
 
    // how many bytes did we write

    bytes = pulse_stream->tell();
    delete pulse_stream;
    pulse_stream = 0;
  }

  if (pulse_file)
  {
    fclose(pulse_file);
    pulse_file = 0;
  }

  if (waves_stream)
  {
    bytes += waves_stream->tell();
    delete waves_stream;
    waves_stream = 0;
  }

  if (waves_file)
  {
    fclose(waves_file);
    waves_file = 0;
  }

  npulses = p_count;
  p_count = 0;

  return bytes;
}

PULSEwriterPLS::PULSEwriterPLS()
{
  compress = FALSE;
  file_name = 0;
  pulse_file = 0;
  waves_file = 0;
  pulse_stream = 0;
  waves_stream = 0;
  pulse_writer = 0;
  waves_writer = 0;
}

PULSEwriterPLS::~PULSEwriterPLS()
{
  if (pulse_stream || waves_stream || pulse_writer || waves_writer) close();
}
