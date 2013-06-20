/*
===============================================================================

  FILE:  pulsequantizer.hpp
  
  CONTENTS:
  
    The mechanism to quantize pulse locations (and direction vectors).

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
  
    15 June 2012 -- added scale and offset for GPS times
    16 February 2012 -- created in the cafe where it smelled like 1917 Chestnut
  
===============================================================================
*/
#ifndef PULSE_QUANTIZER_HPP
#define PULSE_QUANTIZER_HPP

#include "mydefs.hpp"

class PULSEquantizer
{
public:
  F64 t_scale_factor;
  F64 t_offset;
  F64 x_scale_factor;
  F64 y_scale_factor;
  F64 z_scale_factor;
  F64 x_offset;
  F64 y_offset;
  F64 z_offset;

  inline I64 get_T(const F64 t) const { if (t >= t_offset) return (I64)(((t-t_offset)/t_scale_factor)+0.5); else return (I64)(((t-t_offset)/t_scale_factor)-0.5); };

  inline I64 get_T_msec(const I64 T) const { return (I64)((t_scale_factor/0.001)*T);};
  inline I64 get_T_usec(const I64 T) const { return (I64)((t_scale_factor/0.000001)*T);};
  inline I64 get_T_nsec(const I64 T) const { return (I64)((t_scale_factor/0.000000001)*T);};
  inline I64 get_T_psec(const I64 T) const { return (I64)((t_scale_factor/0.000000000001)*T);};

  inline F64 get_t(const I64 T) const { return t_scale_factor*T+t_offset; };
  inline F64 get_t_msec(const I64 T) const { return (t_scale_factor/0.001)*(t_scale_factor*T+t_offset);};
  inline F64 get_t_usec(const I64 T) const { return (t_scale_factor/0.000001)*(t_scale_factor*T+t_offset);};
  inline F64 get_t_nsec(const I64 T) const { return (t_scale_factor/0.000000001)*(t_scale_factor*T+t_offset);};
  inline F64 get_t_psec(const I64 T) const { return (t_scale_factor/0.000000000001)*(t_scale_factor*T+t_offset);};

  inline F64 get_x(const I32 X) const { return x_scale_factor*X+x_offset; };
  inline F64 get_y(const I32 Y) const { return y_scale_factor*Y+y_offset; };
  inline F64 get_z(const I32 Z) const { return z_scale_factor*Z+z_offset; };

  inline I32 get_X(const F64 x) const { if (x >= x_offset) return (I32)(((x-x_offset)/x_scale_factor)+0.5); else return (I32)(((x-x_offset)/x_scale_factor)-0.5); };
  inline I32 get_Y(const F64 y) const { if (y >= y_offset) return (I32)(((y-y_offset)/y_scale_factor)+0.5); else return (I32)(((y-y_offset)/y_scale_factor)-0.5); };
  inline I32 get_Z(const F64 z) const { if (z >= z_offset) return (I32)(((z-z_offset)/z_scale_factor)+0.5); else return (I32)(((z-z_offset)/z_scale_factor)-0.5); };


  PULSEquantizer()
  {
    t_scale_factor = 0.000001;
    t_offset = 0; 
    x_scale_factor = 0.01;
    y_scale_factor = 0.01;
    z_scale_factor = 0.01;
    x_offset = 0.0;
    y_offset = 0.0;
    z_offset = 0.0;
  };

  PULSEquantizer & operator=(const PULSEquantizer & quantizer)
  {
    this->t_scale_factor = quantizer.t_scale_factor;
    this->t_offset = quantizer.t_offset;
    this->x_scale_factor = quantizer.x_scale_factor;
    this->y_scale_factor = quantizer.y_scale_factor;
    this->z_scale_factor = quantizer.z_scale_factor;
    this->x_offset = quantizer.x_offset;
    this->y_offset = quantizer.y_offset;
    this->z_offset = quantizer.z_offset;
    return *this;
  };
};

#endif
