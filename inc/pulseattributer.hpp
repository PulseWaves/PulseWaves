/*
===============================================================================

  FILE:  pulseattributer.hpp
  
  CONTENTS:
  
    A mechanism to add/delete/maintain "extra attributes".

  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2007-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    16 February 2012 -- created in the cafe where it smelled like 1917 Chestnut
  
===============================================================================
*/
#ifndef PULSE_ATTRIBUTER_HPP
#define PULSE_ATTRIBUTER_HPP

#include "pulseextraattribute.hpp"

class PULSEattributer
{
public:
  I32 number_extra_attributes;
  PULSEattribute* extra_attributes;
  I32* extra_attribute_array_offsets;
  I32* extra_attribute_sizes;

  PULSEattributer()
  {
    number_extra_attributes = 0;
    extra_attributes = 0;
    extra_attribute_array_offsets = 0;
    extra_attribute_sizes = 0;
  }

  ~PULSEattributer()
  {
    clean_extra_attributes();
  }

  void clean_extra_attributes()
  {
    if (number_extra_attributes)
    {
      number_extra_attributes = 0;
      free(extra_attributes); extra_attributes = 0;
      free(extra_attribute_array_offsets); extra_attribute_array_offsets = 0;
      free(extra_attribute_sizes); extra_attribute_sizes = 0;
    }
  }

  BOOL init_extra_attributes(U32 number_extra_attributes, PULSEattribute* extra_attributes)
  {
    U32 i;
    clean_extra_attributes();
    this->number_extra_attributes = number_extra_attributes;
    this->extra_attributes = (PULSEattribute*)malloc(sizeof(PULSEattribute)*number_extra_attributes);
    memcpy(this->extra_attributes, extra_attributes, sizeof(PULSEattribute)*number_extra_attributes);
    extra_attribute_array_offsets = (I32*)malloc(sizeof(I32)*number_extra_attributes);
    extra_attribute_sizes = (I32*)malloc(sizeof(I32)*number_extra_attributes);
    extra_attribute_array_offsets[0] = 0;
    extra_attribute_sizes[0] = extra_attributes[0].get_size();
    for (i = 1; i < number_extra_attributes; i++)
    {
      extra_attribute_array_offsets[i] = extra_attribute_array_offsets[i-1] + extra_attribute_sizes[i-1];
      extra_attribute_sizes[i] = extra_attributes[i].get_size();
    }
    return TRUE;
  }

  I32 add_extra_attribute(const PULSEattribute extra_attribute)
  {
    if (extra_attribute.get_size())
    {
      if (extra_attributes)
      {
        number_extra_attributes++;
        extra_attributes = (PULSEattribute*)realloc(extra_attributes, sizeof(PULSEattribute)*number_extra_attributes);
        extra_attribute_array_offsets = (I32*)realloc(extra_attribute_array_offsets, sizeof(I32)*number_extra_attributes);
        extra_attribute_sizes = (I32*)realloc(extra_attribute_sizes, sizeof(I32)*number_extra_attributes);
        extra_attributes[number_extra_attributes-1] = extra_attribute;
        extra_attribute_array_offsets[number_extra_attributes-1] = extra_attribute_array_offsets[number_extra_attributes-2] + extra_attribute_sizes[number_extra_attributes-2];
        extra_attribute_sizes[number_extra_attributes-1] = extra_attributes[number_extra_attributes-1].get_size();
      }
      else
      {
        number_extra_attributes = 1;
        extra_attributes = (PULSEattribute*)malloc(sizeof(PULSEattribute));
        extra_attribute_array_offsets = (I32*)malloc(sizeof(I32));
        extra_attribute_sizes = (I32*)malloc(sizeof(I32));
        extra_attributes[0] = extra_attribute;
        extra_attribute_array_offsets[0] = 0;
        extra_attribute_sizes[0] = extra_attributes[0].get_size();
      }
      return number_extra_attributes-1;
    }
    return -1;
  };

  inline I16 get_total_extra_attributes_size() const
  {
    return (extra_attributes ? extra_attribute_array_offsets[number_extra_attributes-1] + extra_attribute_sizes[number_extra_attributes-1] : 0);
  }

  I32 get_extra_attribute_index(const char* name) const
  {
    I32 i;
    for (i = 0; i < number_extra_attributes; i++)
    {
      if (strcmp(extra_attributes[i].name, name) == 0)
      {
        return i;
      }
    }
    return -1;
  }

  I32 get_extra_attribute_array_offset(const char* name) const
  {
    I32 i;
    for (i = 0; i < number_extra_attributes; i++)
    {
      if (strcmp(extra_attributes[i].name, name) == 0)
      {
        return extra_attribute_array_offsets[i];
      }
    }
    return -1;
  }

  I32 get_extra_attribute_array_offset(I32 index) const
  {
    if (index < number_extra_attributes)
    {
      return extra_attribute_array_offsets[index];
    }
    return -1;
  }

  BOOL remove_extra_attribute(I32 index)
  {
    if (index < 0 || index >= number_extra_attributes)
    {
      return FALSE;
    }
    for (index = index + 1; index < number_extra_attributes; index++)
    {
      extra_attributes[index-1] = extra_attributes[index];
      if (index > 1)
      {
        extra_attribute_array_offsets[index-1] = extra_attribute_array_offsets[index-2] + extra_attribute_sizes[index-2];
      }
      else
      {
        extra_attribute_array_offsets[index-1] = 0;
      }
      extra_attribute_sizes[index-1] = extra_attribute_sizes[index];
    }
    number_extra_attributes--;
    if (number_extra_attributes)
    {
      extra_attributes = (PULSEattribute*)realloc(extra_attributes, sizeof(PULSEattribute)*number_extra_attributes);
      extra_attribute_array_offsets = (I32*)realloc(extra_attribute_array_offsets, sizeof(I32)*number_extra_attributes);
      extra_attribute_sizes = (I32*)realloc(extra_attribute_sizes, sizeof(I32)*number_extra_attributes);
    }
    else
    {
      free(extra_attributes); extra_attributes = 0;
      free(extra_attribute_array_offsets); extra_attribute_array_offsets = 0;
      free(extra_attribute_sizes); extra_attribute_sizes = 0;
    }
  }

  BOOL remove_extra_attribute(const char* name)
  {
    I32 index = get_extra_attribute_index(name);
    if (index != -1)
    { 
      return remove_extra_attribute(index);
    }
    return FALSE;
  }
};

#endif
