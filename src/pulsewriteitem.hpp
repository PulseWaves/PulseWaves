/*
===============================================================================

  FILE:  pulsewriteitem.hpp
  
  CONTENTS:
  
    Common interface for all classes that write the items that compose a pulse.

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
  
    22 February 2012 -- created at Mamio Verde 500 meters above a lit up valley
  
===============================================================================
*/
#ifndef PULSE_WRITE_ITEM_HPP
#define PULSE_WRITE_ITEM_HPP

#include "pulseitem.hpp"

class ByteStreamOut;

class PULSEwriteItem
{
public:
  virtual void write(const U8* item)=0;

  virtual ~PULSEwriteItem(){};
};

class PULSEwriteItemRaw : public PULSEwriteItem
{
public:
  PULSEwriteItemRaw()
  {
    outstream = 0;
  };
  BOOL init(ByteStreamOut* outstream)
  {
    if (!outstream) return FALSE;
    this->outstream = outstream;
    return TRUE;
  };
  virtual ~PULSEwriteItemRaw(){};
protected:
  ByteStreamOut* outstream;
};

class PULSEwriteItemCompressed : public PULSEwriteItem
{
public:
  virtual BOOL init(const U8* item)=0;

  virtual ~PULSEwriteItemCompressed(){};
};

#endif
