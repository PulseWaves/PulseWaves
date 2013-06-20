/*
===============================================================================

  FILE:  pulsereaditem.hpp
  
  CONTENTS:
  
    Common interface for all classes that read the items that compose a pulse.

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
#ifndef PULSE_READ_ITEM_HPP
#define PULSE_READ_ITEM_HPP

#include "pulseitem.hpp"

class ByteStreamIn;

class PULSEreadItem
{
public:
  virtual void read(U8* item)=0;

  virtual ~PULSEreadItem(){};
};

class PULSEreadItemRaw : public PULSEreadItem
{
public:
  PULSEreadItemRaw()
  {
    instream = 0;
  };
  BOOL init(ByteStreamIn* instream)
  {
    if (!instream) return FALSE;
    this->instream = instream;
    return TRUE;
  };
  virtual ~PULSEreadItemRaw(){};
protected:
  ByteStreamIn* instream;
};

class PULSEreadItemCompressed : public PULSEreadItem
{
public:
  virtual BOOL init(const U8* item)=0;

  virtual ~PULSEreadItemCompressed(){};
};

#endif
