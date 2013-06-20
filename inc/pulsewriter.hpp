/*
===============================================================================

  FILE:  pulsewriter.hpp
  
  CONTENTS:
  
    Interface class to write Pulses (and their Waves) in the PulseWaves format.

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
#ifndef PULSE_WRITER_HPP
#define PULSE_WRITER_HPP

#include "pulseheader.hpp"
#include "pulsepulse.hpp"
#include "waveswaves.hpp"
#include "pulseutility.hpp"

class PULSEwriter
{
public:
  PULSEquantizer quantizer;
  I64 npulses;
  I64 p_count;
  PULSEinventory inventory;

  virtual BOOL write_pulse(const PULSEpulse* pulse) = 0;
  virtual BOOL write_waves(const WAVESwaves* waves) = 0;

  void update_inventory(const PULSEpulse* pulse, BOOL only_count=FALSE) { inventory.add(pulse, only_count); };
  virtual I64 get_current_offset() const = 0;
  virtual BOOL update_header(const PULSEheader* header, BOOL use_inventory=TRUE, BOOL update_extra_bytes=FALSE) = 0;
  virtual I64 close(BOOL update_npulses=TRUE) = 0;

  PULSEwriter() { npulses = 0; p_count = 0; };
  virtual ~PULSEwriter() {};
};

class PULSEwriteOpener
{
public:

  void usage() const;

  BOOL parse(int argc, char* argv[]);

  void set_directory(const char* directory);
  void set_file_name(const char* file_name);
  void set_appendix(const char* appendix);
  void set_format(const char* format);
  void set_parse_string(const char* parse_string);
  void set_separator(const char* separator);
  void make_numbered_file_name(const char* file_name, I32 digits);
  void make_file_name(const char* file_name, I32 file_number=-1);

  PULSEwriter* open(PULSEheader* header);
  BOOL open_waves(PULSEwriter* writer);

  BOOL active() const;
  const char* get_directory() const;
  const char* get_file_name() const;
  const char* get_appendix() const;
  BOOL format_was_specified() const;
  I32 get_format() const;
  const char* get_format_name() const;

  PULSEwriteOpener();
  ~PULSEwriteOpener();

private:
  char* file_name;
  char* appendix;
  char* directory;
  char* parse_string;
  char* separator;
  U32 format;
  BOOL compress_waves;
  BOOL use_stdout;
  BOOL use_stderr;
  BOOL use_nil;
};

#endif
