/*
===============================================================================

  FILE:  pulsefilter.hpp
  
  CONTENTS:
  
    Filters LiDAR pulses based on certain criteria being true (or not).

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
#ifndef PULSE_FILTER_HPP
#define PULSE_FILTER_HPP

#include "pulsepulse.hpp"

class PULSEcriterion
{
public:
  virtual const char * name() const = 0;
  virtual int get_command(char* string) const = 0;
  virtual BOOL filter(const PULSEpulse* pulse) = 0;
  virtual void reset(){};
  virtual ~PULSEcriterion(){};
};

class PULSEfilter
{
public:

  void usage() const;
  void clean();
  BOOL parse(int argc, char* argv[]);
  I32 unparse(char* string) const;
  inline BOOL active() const { return (num_criteria != 0); };

  BOOL filter(const PULSEpulse* pulse);
  void reset();

  void addKeepCircle(F64 x, F64 y, F64 radius);
  void addKeepBox(F64 min_x, F64 min_y, F64 min_z, F64 max_x, F64 max_y, F64 max_z);

  PULSEfilter();
  ~PULSEfilter();

private:

  void add_criterion(PULSEcriterion* criterion);
  U32 num_criteria;
  U32 alloc_criteria;
  PULSEcriterion** criteria;
  int* counters;
};

#endif
