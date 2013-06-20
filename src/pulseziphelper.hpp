/*
===============================================================================

  FILE:  pulseziphelper.hpp
  
  CONTENTS:
  
    Implementation of functionality used for compression and decompression

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
  
    8 April 2013 -- created at Glenelg Beach Backpackers after two cans of tuna

===============================================================================
*/
#ifndef PULSE_ZIP_HELPER_HPP
#define PULSE_ZIP_HELPER_HPP

#include "pulsezip.hpp"

class PULSEzipContextMapper8to7orLess
{
public:
  inline void init()
  {
    memset(contexts, 0xFF, sizeof(U8)*U8_MAX);
    count = 0;
  };

  inline U32 getContext(U8 value)
  {
    if (contexts[value] == 0xFF)
    {
      contexts[value] = (U8)(count & mask);
      count++;
    }
    return contexts[value];
  };

  inline U32 getCount() const
  {
    return count;
  };

  inline BOOL setup(U32 nbits)
  {
    if (nbits > 7)
    {
      fprintf(stderr, "ERROR: nbits %d exceeds limit of 7 for context mapper 8 to 7.\n", nbits);
      return FALSE;
    }
    mask = (1u << nbits) - 1;
    init();
    return TRUE;
  };

  PULSEzipContextMapper8to7orLess() {};
  ~PULSEzipContextMapper8to7orLess() {};

private:
  U32 mask;
  U32 count;
  U8 contexts[U8_MAX];
};

class PULSEzipContextMapper16to7orLess
{
public:
  inline void init()
  {
    memset(contexts, 0xFF, sizeof(U8)*U16_MAX);
    count = 0;
  };

  inline U32 getContext(U16 value)
  {
    if (contexts[value] == 0xFF)
    {
      contexts[value] = (U8)(count & mask);
      count++;
    }
    return contexts[value];
  };

  inline U32 getCount() const
  {
    return count;
  };

  inline BOOL setup(U32 nbits)
  {
    if (nbits > 7)
    {
      fprintf(stderr, "ERROR: nbits %d exceeds limit of 7 for context mapper 16 to 7.\n", nbits);
      return FALSE;
    }
    mask = (1u << nbits) - 1;
    init();
    return TRUE;
  };

  PULSEzipContextMapper16to7orLess() {};
  ~PULSEzipContextMapper16to7orLess() {};

private:
  U32 mask;
  U32 count;
  U8 contexts[U16_MAX];
};

class PULSEzipContextMapper16to15orLess
{
public:
  inline void init()
  {
    memset(contexts, 0xFF, sizeof(U16)*U16_MAX);
    count = 0;
  };

  inline U32 getContext(U16 value)
  {
    if (contexts[value] == 0xFFFF)
    {
      contexts[value] = (U16)(count & mask);
      count++;
    }
    return contexts[value];
  };

  inline U32 getCount() const
  {
    return count;
  };

  BOOL setup(U32 nbits)
  {
    if (nbits > 15)
    {
      fprintf(stderr, "ERROR: nbits %d exceeds limit of 15 for context mapper 16 to 15.\n", nbits);
      return FALSE;
    }
    mask = (1u << nbits) - 1;
    init();
    return TRUE;
  };

  PULSEzipContextMapper16to15orLess() {};
  ~PULSEzipContextMapper16to15orLess() {};

private:
  U32 mask;
  U32 count;
  U16 contexts[U16_MAX];
};

#endif
