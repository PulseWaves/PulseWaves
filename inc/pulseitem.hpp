/*
===============================================================================

  FILE:  pulseitem.hpp
  
  CONTENTS:
  
    Describes user-defineable "extra attributes" added to a pulse record.

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
#ifndef PULSE_ITEM_HPP
#define PULSE_ITEM_HPP

#include <string.h>
#include <stdlib.h>

#include "mydefs.hpp"
#include "pulsewavesdefinitions.hpp"

class PULSEitem
{
public:
  enum Type { EXTRABYTES = 1000, PULSE0, PULSESOURCEID16, PULSESOURCEID32 } type;
  U16 size;
  U16 version;
  BOOL is_type(PULSEitem::Type t) const;
  const CHAR* get_name() const;
};

#endif
