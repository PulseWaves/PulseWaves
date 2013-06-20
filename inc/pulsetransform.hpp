/*
===============================================================================

  FILE:  pulsetransform.hpp
  
  CONTENTS:
  
    Transforms LiDAR pulses with a number of different operations.

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
#ifndef PULSE_TRANSFORM_HPP
#define PULSE_TRANSFORM_HPP

#include "pulsepulse.hpp"

class PULSEoperation
{
public:
  virtual const char * name() const = 0;
  virtual int get_command(char* string) const = 0;
  virtual void transform(PULSEpulse* pulse) const = 0;
  virtual ~PULSEoperation(){};
};

class PULSEtransform
{
public:

  bool change_coordinates;

  void usage() const;
  void clean();
  BOOL parse(int argc, char* argv[]);
  I32 unparse(char* string) const;
  inline BOOL active() const { return (num_operations != 0); };

  void transform(PULSEpulse* pulse) const;

  PULSEtransform();
  ~PULSEtransform();

private:

  void add_operation(PULSEoperation* operation);
  U32 num_operations;
  U32 alloc_operations;
  PULSEoperation** operations;
};

#endif
