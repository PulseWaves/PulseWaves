/*
===============================================================================

  FILE:  pulsewriter.cpp
  
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
#include "pulsewriter.hpp"

#include "pulsewavesdefinitions.hpp"

#include "pulsewriter_pls.hpp"
#ifndef PULSEWAVES_DLL
#include "pulsewriter_txt.hpp"
#endif // PULSEWAVES_DLL

#include <stdlib.h>
#include <string.h>

PULSEwriter* PULSEwriteOpener::open(PULSEheader* header)
{
  if (use_nil)
  {
    PULSEwriterPLS* pulsewriterpls = new PULSEwriterPLS();
    if (!pulsewriterpls->open(header, (format == PULSEWAVES_FORMAT_PLZ)))
    {
      fprintf(stderr,"ERROR: cannot open pulsewriterpls to NULL\n");
      delete pulsewriterpls;
      return 0;
    }
    return pulsewriterpls;
  }
  else if (file_name)
  {
    if (format <= PULSEWAVES_FORMAT_PLZ)
    {
      PULSEwriterPLS* pulsewriterpls = new PULSEwriterPLS();
      if (!pulsewriterpls->open(file_name, header, (format == PULSEWAVES_FORMAT_PLZ)))
      {
        fprintf(stderr,"ERROR: cannot open pulsewriterpls with file name '%s'\n", file_name);
        delete pulsewriterpls;
        return 0;
      }
      return pulsewriterpls;
    }
#ifndef PULSEWAVES_DLL
    else if (format == PULSEWAVES_FORMAT_TXT)
    {
      PULSEwriterTXT* pulsewritertxt = new PULSEwriterTXT();
      if (!pulsewritertxt->open(file_name, file_name, header))
      {
        fprintf(stderr,"ERROR: cannot open pulsewritertxt with file name '%s'\n", file_name);
        delete pulsewritertxt;
        return 0;
      }
      return pulsewritertxt;
    }
#endif // PULSEWAVES_DLL
    else
    {
      fprintf(stderr,"ERROR: unknown format %d\n", format);
      return 0;
    }
  }
  else if (use_stdout || use_stderr)
  {
    if (format <= PULSEWAVES_FORMAT_PLZ)
    {
      PULSEwriterPLS* pulsewriterpls = new PULSEwriterPLS();
      if (!pulsewriterpls->open((use_stdout ? stdout : stderr), header, (format == PULSEWAVES_FORMAT_PLZ)))
      {
        fprintf(stderr,"ERROR: cannot open pulsewriterpls to stdout\n");
        delete pulsewriterpls;
        return 0;
      }
      return pulsewriterpls;
    }
#ifndef PULSEWAVES_DLL
    else if (format == PULSEWAVES_FORMAT_TXT)
    {
      PULSEwriterTXT* pulsewritertxt = new PULSEwriterTXT();
      if (!pulsewritertxt->open((use_stdout ? stdout : stderr), stdout, header))
      {
        fprintf(stderr,"ERROR: cannot open pulsewritertxt to stdout\n");
        delete pulsewritertxt;
        return 0;
      }
      return pulsewritertxt;
    }
#endif // PULSEWAVES_DLL
    else
    {
      fprintf(stderr,"ERROR: unknown format %d\n", format);
      return 0;
    }
  }
  else
  {
    fprintf(stderr,"ERROR: no pulsewriter output specified\n");
    return 0;
  }
}

BOOL PULSEwriteOpener::open_waves(PULSEwriter* reader)
{
  if (use_nil)
  {
    PULSEwriterPLS* pulsewriterpls = (PULSEwriterPLS*)reader;
    if (pulsewriterpls->open_waves())
    {
      return TRUE;
    }
    fprintf(stderr,"WARNING: cannot open_waves pulsewriterpls to NULL\n");
  }
  else if (file_name)
  {
    if (format <= PULSEWAVES_FORMAT_PLZ)
    {
      PULSEwriterPLS* pulsewriterpls = (PULSEwriterPLS*)reader;
      if (pulsewriterpls->open_waves())
      {
        return TRUE;
      }
      fprintf(stderr,"ERROR: cannot open waves for pulsewriterpls with file name '%s'\n", file_name);
    }
#ifndef PULSEWAVES_DLL
    else if (format == PULSEWAVES_FORMAT_TXT)
    {
      PULSEwriterTXT* pulsewritertxt = (PULSEwriterTXT*)reader;
      if (pulsewritertxt->open_waves())
      {
        return TRUE;
      }
      fprintf(stderr,"ERROR: cannot open waves for pulsewritertxt with file name '%s'\n", file_name);
    }
#endif // PULSEWAVES_DLL
    else
    {
      fprintf(stderr,"ERROR: unknown format %d\n", format);
      return FALSE;
    }
  }
  else
  {
    fprintf(stderr,"ERROR: no output specified\n");
  }
  return FALSE;
}

void PULSEwriteOpener::usage() const
{
  fprintf(stderr,"Supported PULSE Outputs\n");
  fprintf(stderr,"  -o pulse.pls\n");
  fprintf(stderr,"  -o pulse.pls -owvz\n");
#ifndef PULSEWAVES_DLL
  fprintf(stderr,"  -o human_readable.txt\n");
#endif // PULSEWAVES_DLL
//  fprintf(stderr,"  -odir C:\\data\\flight1 (specify output directory)\n");
//  fprintf(stderr,"  -odix _processed (specify file name appendix)\n");
//  fprintf(stderr,"  -opls (specify format)\n");
//  fprintf(stderr,"  -stdout (pipe to stdout)\n");
//  fprintf(stderr,"  -nil    (pipe to NULL)\n");
}

BOOL PULSEwriteOpener::parse(int argc, char* argv[])
{
  int i;
  for (i = 1; i < argc; i++)
  {
    if (argv[i][0] == '\0')
    {
      continue;
    }
    else if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"-help") == 0)
    {
      usage();
      return TRUE;
    }
    else if (strcmp(argv[i],"-o") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: file_name\n", argv[i]);
        return FALSE;
      }
      set_file_name(argv[i+1]);
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-odir") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: directory\n", argv[i]);
        return FALSE;
      }
      set_directory(argv[i+1]);
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-odix") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: appendix\n", argv[i]);
        return FALSE;
      }
      set_appendix(argv[i+1]);
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-opls") == 0)
    {
      format = PULSEWAVES_FORMAT_PLS;
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-oplz") == 0)
    {
      format = PULSEWAVES_FORMAT_PLZ;
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-otxt") == 0)
    {
      format = PULSEWAVES_FORMAT_TXT;
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-stdout") == 0)
    {
      use_stdout = TRUE;
      use_nil = FALSE;
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-nil") == 0)
    {
      use_nil = TRUE;
      use_stdout = FALSE;
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-oparse") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: string\n", argv[i]);
        return FALSE;
      }
      set_parse_string(argv[i+1]);
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-sep") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: separator\n", argv[i]);
        return FALSE;
      }
      set_separator(argv[i+1]);
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
  }
  return TRUE;
}

void PULSEwriteOpener::set_file_name(const char* file_name)
{
  if (this->file_name) free(this->file_name);
  if (file_name)
  {
    this->file_name = strdup(file_name);
    int end = strlen(file_name) - 4;
    if (strstr(file_name+end, ".plz") || strstr(file_name+end, ".PLZ"))
    {
      format = PULSEWAVES_FORMAT_PLZ;
    }
    else if (strstr(file_name+end, ".pls") || strstr(file_name+end, ".PLS"))
    {
      format = PULSEWAVES_FORMAT_PLS;
    }
    else // assume ascii output
    {
      format = PULSEWAVES_FORMAT_TXT;
    }
  }
  else
  {
    this->file_name = 0;
  }
}

void PULSEwriteOpener::set_directory(const char* directory)
{
  if (this->directory) free(this->directory);
  if (directory)
  {
    this->directory = strdup(directory);
  }
  else
  {
    this->directory = 0;
  }
}

void PULSEwriteOpener::set_appendix(const char* appendix)
{
  if (this->appendix) free(this->appendix);
  if (appendix)
  {
    this->appendix = strdup(appendix);
  }
  else
  {
    this->appendix = 0;
  }
}

void PULSEwriteOpener::set_format(const char* format)
{
  if (format)
  {
    if (strstr(format, "plz") || strstr(format, "PLZ"))
    {
      set_format(PULSEWAVES_FORMAT_PLZ);
    }
    else if (strstr(format, "pls") || strstr(format, "PLS"))
    {
      set_format(PULSEWAVES_FORMAT_PLS);
    }
    else // assume ascii output
    {
      set_format(PULSEWAVES_FORMAT_TXT);
    }
  }
  else
  {
    set_format(PULSEWAVES_FORMAT_DEFAULT);
  }
}

void PULSEwriteOpener::set_format(I32 format)
{
  this->format = format;
}

void PULSEwriteOpener::set_parse_string(const char* parse_string)
{
  if (this->parse_string) free(this->parse_string);
  if (parse_string)
  {
    this->parse_string = strdup(parse_string);
  }
  else
  {
    this->parse_string = 0;
  }
}

void PULSEwriteOpener::set_separator(const char* separator)
{
  if (this->separator) free(this->separator);
  if (separator)
  {
    this->separator = strdup(separator);
  }
  else
  {
    this->separator = 0;
  }
}

void PULSEwriteOpener::make_numbered_file_name(const char* file_name, I32 digits)
{
  int len;
  if (file_name)
  {
    if (this->file_name != 0) free(this->file_name);
    len = strlen(file_name);
    this->file_name = (char*)malloc(len + digits + 2);
    strcpy(this->file_name, file_name);
  }
  else
  {
    if (this->file_name == 0) this->file_name = strdup("output.xxx");
    len = strlen(this->file_name);
    this->file_name = (char*)realloc(this->file_name, len + digits + 2);
  }
  while (len > 0 && this->file_name[len] != '.')
  {
    len--;
  }
  len++;
  while (digits > 0)
  {
    this->file_name[len] = '0';
    digits--;
    len++;
  }
  this->file_name[len] = '.';
  len++;
  this->file_name[len] = 'x';
  this->file_name[len+1] = 'x';
  this->file_name[len+2] = 'x';
  this->file_name[len+3] = '\0';
}

void PULSEwriteOpener::make_file_name(const char* file_name, I32 file_number)
{
  int len;
  if (file_number > -1)
  {
    if (file_name)
    {
      if (this->file_name) free(this->file_name);
      len = strlen(file_name);
      this->file_name = (char*)malloc(len+10);
      strcpy(this->file_name, file_name);
    }
    else
    {
      if (this->file_name == 0)
      {
        this->file_name = strdup("output.000000.xxx");
      }
      len = strlen(this->file_name);
    }
    while (len > 0 && this->file_name[len] != '.') len--;
    len++;
    int num = len - 2;
    while (num > 0 && this->file_name[num] >= '0' && this->file_name[num] <= '9')
    {
      this->file_name[num] = '0' + (file_number%10);
      file_number = file_number/10;
      num--;
    }
  }
  else
  {
    if (this->file_name) free(this->file_name);
    if (file_name)
    {
      len = strlen(file_name);
      this->file_name = (char*)malloc(len + (appendix ? strlen(appendix) + 5 : 10));
      strcpy(this->file_name, file_name);
      while (len > 0 && this->file_name[len] != '.') len--;
      if (file_number == -1)
      {
        if (appendix)
        {
          strcpy(&(this->file_name[len]), appendix);
          len += strlen(appendix);
        }
        else
        {
          this->file_name[len] = '_';
          this->file_name[len+1] = '1';
          len += 2;
        }
        this->file_name[len] = '.';
      }
      len++;
    }
    else
    {
      len = 7;
      this->file_name = strdup("output.xxx");
    }
  }
  if (format <= PULSEWAVES_FORMAT_PLS)
  {
    this->file_name[len] = 'p';
    this->file_name[len+1] = 'l';
    this->file_name[len+2] = 's';
  }
  else if (format == PULSEWAVES_FORMAT_PLZ)
  {
    this->file_name[len] = 'p';
    this->file_name[len+1] = 'l';
    this->file_name[len+2] = 'z';
  }
  else // if (format == PULSEWAVES_FORMAT_TXT)
  {
    this->file_name[len] = 't';
    this->file_name[len+1] = 'x';
    this->file_name[len+2] = 't';
  }
  this->file_name[len+3] = '\0';

  if (directory)
  {
    len = strlen(this->file_name);
    while (len > 0 && this->file_name[len] != '\\') len--;
    char* temp_file_name = (char*)malloc(strlen(directory) + strlen(&(this->file_name[len])) + 5);
    sprintf(temp_file_name, "%s\\%s", directory, &(this->file_name[len]));
    free(this->file_name);
    this->file_name = temp_file_name;
  }
}

const char* PULSEwriteOpener::get_directory() const
{
  return directory;
}

const char* PULSEwriteOpener::get_file_name() const
{
  return file_name;
}

const char* PULSEwriteOpener::get_appendix() const
{
  return appendix;
}

BOOL PULSEwriteOpener::format_was_specified() const
{
  return (format != PULSEWAVES_FORMAT_DEFAULT);
}

static const char* PULSEWAVES_FORMAT_NAMES[5] = { "pls", "plz", "las", "laz", "txt" };

const char* PULSEwriteOpener::get_format_name() const
{
  return PULSEWAVES_FORMAT_NAMES[format];
}

I32 PULSEwriteOpener::get_format() const
{
  return format;
}

BOOL PULSEwriteOpener::is_piped() const
{
  return ((file_name == 0) && use_stdout);
}

BOOL PULSEwriteOpener::active() const
{
  return (file_name != 0 || use_stdout || use_stderr || use_nil);
}

PULSEwriteOpener::PULSEwriteOpener()
{
  directory = 0;
  file_name = 0;
  appendix = 0;
  parse_string = 0;
  separator = 0;
  format = PULSEWAVES_FORMAT_DEFAULT;
  use_stdout = FALSE;
  use_stderr = FALSE;
  use_nil = FALSE;
}

PULSEwriteOpener::~PULSEwriteOpener()
{
  if (directory) free(directory);
  if (file_name) free(file_name);
  if (appendix) free(appendix);
  if (parse_string) free(parse_string);
  if (separator) free(separator);
}
