/*
===============================================================================

  FILE:  pulsefilter.cpp
  
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
#include "pulsefilter.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class PULSEcriterionClipTile : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_tile"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g ", name(), ll_x, ll_y, tile_size); };
  inline BOOL filter(const PULSEpulse* pulse) { return (!pulse->inside_tile(ll_x, ll_y, ur_x, ur_y)); };
  PULSEcriterionClipTile(F32 ll_x, F32 ll_y, F32 tile_size) { this->ll_x = ll_x; this->ll_y = ll_y; this->ur_x = ll_x+tile_size; this->ur_y = ll_y+tile_size; this->tile_size = tile_size; };
private:
  F32 ll_x, ll_y, ur_x, ur_y, tile_size;
};

class PULSEcriterionClipCircle : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_circle"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g ", name(), center_x, center_y, radius); };
  inline BOOL filter(const PULSEpulse* pulse) { return (!pulse->inside_circle(center_x, center_y, radius_squared)); };
  PULSEcriterionClipCircle(F64 x, F64 y, F64 radius) { this->center_x = x; this->center_y = y; this->radius = radius; this->radius_squared = radius*radius; };
private:
  F64 center_x, center_y, radius, radius_squared;
};

class PULSEcriterionClipBox : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_box"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g %g %g ", name(), min_x, min_y, min_z, max_x, max_y, max_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (!pulse->inside_box(min_x, min_y, min_z, max_x, max_y, max_z)); };
  PULSEcriterionClipBox(F64 min_x, F64 min_y, F64 min_z, F64 max_x, F64 max_y, F64 max_z) { this->min_x = min_x; this->min_y = min_y; this->min_z = min_x; this->max_x = max_x; this->max_y = max_y; this->max_z = max_x; };
private:
  F64 min_x, min_y, min_z, max_x, max_y, max_z;
};

class PULSEcriterionClipXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g ", name(), below_x, below_y, above_x, above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (!pulse->inside_rectangle(below_x, below_y, above_x, above_y)); };
  PULSEcriterionClipXY(F64 below_x, F64 below_y, F64 above_x, F64 above_y) { this->below_x = below_x; this->below_y = below_y; this->above_x = above_x; this->above_y = above_y; };
private:
  F64 below_x, below_y, above_x, above_y;
};

class PULSEcriterionClipZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_z, above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 z = pulse->get_anchor_z(); return (z < below_z) || (z > above_z); };
  PULSEcriterionClipZ(F64 below_z, F64 above_z) { this->below_z = below_z; this->above_z = above_z; };
private:
  F64 below_z, above_z;
};

class PULSEcriterionClipXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_x() < below_x); };
  PULSEcriterionClipXBelow(F64 below_x) { this->below_x = below_x; };
private:
  F64 below_x;
};

class PULSEcriterionClipXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_x() > above_x); };
  PULSEcriterionClipXAbove(F64 above_x) { this->above_x = above_x; };
private:
  F64 above_x;
};

class PULSEcriterionClipYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_y() < below_y); };
  PULSEcriterionClipYBelow(F64 below_y) { this->below_y = below_y; };
private:
  F64 below_y;
};

class PULSEcriterionClipYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_y() > above_y); };
  PULSEcriterionClipYAbove(F64 above_y) { this->above_y = above_y; };
private:
  F64 above_y;
};

class PULSEcriterionClipZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_z() < below_z); };
  PULSEcriterionClipZBelow(F64 below_z) { this->below_z = below_z; };
private:
  F64 below_z;
};

class PULSEcriterionClipZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_z() > above_z); };
  PULSEcriterionClipZAbove(F64 above_z) { this->above_z = above_z; };
private:
  F64 above_z;
};

class PULSEcriterionClipFirstXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_first"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g ", name(), below_x, below_y, above_x, above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 x = pulse->compute_and_get_first_x(); if ((x < below_x) || (x >= above_x)) return TRUE; F64 y = pulse->compute_and_get_first_y(); return ((y < below_y) || (y >= above_y)); };
  PULSEcriterionClipFirstXY(F64 below_x, F64 below_y, F64 above_x, F64 above_y) { this->below_x = below_x; this->below_y = below_y; this->above_x = above_x; this->above_y = above_y; };
private:
  F64 below_x, below_y, above_x, above_y;
};

class PULSEcriterionClipFirstZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_first_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_z, above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 z = pulse->compute_and_get_first_z(); return ((z < below_z) || (z >= above_z)); };
  PULSEcriterionClipFirstZ(F64 below_z, F64 above_z) { this->below_z = below_z; this->above_z = above_z; };
private:
  F64 below_z, above_z;
};

class PULSEcriterionClipFirstXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_first_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_x() < below_x); };
  PULSEcriterionClipFirstXBelow(F64 below_x) { this->below_x = below_x; };
private:
  F64 below_x;
};

class PULSEcriterionClipFirstXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_first_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_x() >= above_x); };
  PULSEcriterionClipFirstXAbove(F64 above_x) { this->above_x = above_x; };
private:
  F64 above_x;
};

class PULSEcriterionClipFirstYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_first_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_y() < below_y); };
  PULSEcriterionClipFirstYBelow(F64 below_y) { this->below_y = below_y; };
private:
  F64 below_y;
};

class PULSEcriterionClipFirstYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_first_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_y() >= above_y); };
  PULSEcriterionClipFirstYAbove(F64 above_y) { this->above_y = above_y; };
private:
  F64 above_y;
};

class PULSEcriterionClipFirstZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_first_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_z() < below_z); };
  PULSEcriterionClipFirstZBelow(F64 below_z) { this->below_z = below_z; };
private:
  F64 below_z;
};

class PULSEcriterionClipFirstZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_first_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_z() >= above_z); };
  PULSEcriterionClipFirstZAbove(F64 above_z) { this->above_z = above_z; };
private:
  F64 above_z;
};

class PULSEcriterionClipLastXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_last"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g ", name(), below_x, below_y, above_x, above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 x = pulse->compute_and_get_last_x(); if ((x < below_x) || (x >= above_x)) return TRUE; F64 y = pulse->compute_and_get_last_y(); return ((y < below_y) || (y >= above_y)); };
  PULSEcriterionClipLastXY(F64 below_x, F64 below_y, F64 above_x, F64 above_y) { this->below_x = below_x; this->below_y = below_y; this->above_x = above_x; this->above_y = above_y; };
private:
  F64 below_x, below_y, above_x, above_y;
};

class PULSEcriterionClipLastZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_last_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_z, above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 z = pulse->compute_and_get_last_z(); return ((z < below_z) || (z >= above_z)); };
  PULSEcriterionClipLastZ(F64 below_z, F64 above_z) { this->below_z = below_z; this->above_z = above_z; };
private:
  F64 below_z, above_z;
};

class PULSEcriterionClipLastXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_last_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_x() < below_x); };
  PULSEcriterionClipLastXBelow(F64 below_x) { this->below_x = below_x; };
private:
  F64 below_x;
};

class PULSEcriterionClipLastXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_last_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_x() >= above_x); };
  PULSEcriterionClipLastXAbove(F64 above_x) { this->above_x = above_x; };
private:
  F64 above_x;
};

class PULSEcriterionClipLastYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_last_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_y() < below_y); };
  PULSEcriterionClipLastYBelow(F64 below_y) { this->below_y = below_y; };
private:
  F64 below_y;
};

class PULSEcriterionClipLastYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_last_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_y() >= above_y); };
  PULSEcriterionClipLastYAbove(F64 above_y) { this->above_y = above_y; };
private:
  F64 above_y;
};

class PULSEcriterionClipLastZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_last_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_z() < below_z); };
  PULSEcriterionClipLastZBelow(F64 below_z) { this->below_z = below_z; };
private:
  F64 below_z;
};

class PULSEcriterionClipLastZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_last_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_z() >= above_z); };
  PULSEcriterionClipLastZAbove(F64 above_z) { this->above_z = above_z; };
private:
  F64 above_z;
};

class PULSEcriterionClipAnchorXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g ", name(), below_x, below_y, above_x, above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 x = pulse->get_anchor_x(); if ((x < below_x) || (x >= above_x)) return TRUE; F64 y = pulse->get_anchor_y(); return ((y < below_y) || (y >= above_y)); };
  PULSEcriterionClipAnchorXY(F64 below_x, F64 below_y, F64 above_x, F64 above_y) { this->below_x = below_x; this->below_y = below_y; this->above_x = above_x; this->above_y = above_y; };
private:
  F64 below_x, below_y, above_x, above_y;
};

class PULSEcriterionClipAnchorZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_z, above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 z = pulse->get_anchor_z(); return ((z < below_z) || (z >= above_z)); };
  PULSEcriterionClipAnchorZ(F64 below_z, F64 above_z) { this->below_z = below_z; this->above_z = above_z; };
private:
  F64 below_z, above_z;
};

class PULSEcriterionClipAnchorXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_x() < below_x); };
  PULSEcriterionClipAnchorXBelow(F64 below_x) { this->below_x = below_x; };
private:
  F64 below_x;
};

class PULSEcriterionClipAnchorXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_x() >= above_x); };
  PULSEcriterionClipAnchorXAbove(F64 above_x) { this->above_x = above_x; };
private:
  F64 above_x;
};

class PULSEcriterionClipAnchorYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_y() < below_y); };
  PULSEcriterionClipAnchorYBelow(F64 below_y) { this->below_y = below_y; };
private:
  F64 below_y;
};

class PULSEcriterionClipAnchorYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_y() >= above_y); };
  PULSEcriterionClipAnchorYAbove(F64 above_y) { this->above_y = above_y; };
private:
  F64 above_y;
};

class PULSEcriterionClipAnchorZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_z() < below_z); };
  PULSEcriterionClipAnchorZBelow(F64 below_z) { this->below_z = below_z; };
private:
  F64 below_z;
};

class PULSEcriterionClipAnchorZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_z() >= above_z); };
  PULSEcriterionClipAnchorZAbove(F64 above_z) { this->above_z = above_z; };
private:
  F64 above_z;
};

class PULSEcriterionClipAnchorRawXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_raw_xy"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d %d %d ", name(), below_X, below_Y, above_X, above_Y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_X < below_X) || (pulse->anchor_X <= below_X) || (pulse->anchor_Y >= above_Y) || (pulse->anchor_Y >= above_Y); };
  PULSEcriterionClipAnchorRawXY(I32 below_X, I32 below_Y, I32 above_X, I32 above_Y) { this->below_X = below_X; this->below_Y = below_Y; this->above_X = above_X; this->above_Y = above_Y; };
private:
  I32 below_X, below_Y, above_X, above_Y;
};

class PULSEcriterionClipAnchorRawZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_raw_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d ", name(), below_Z, above_Z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Z < below_Z) || (pulse->anchor_Z > above_Z); };
  PULSEcriterionClipAnchorRawZ(I32 below_Z, I32 above_Z) { this->below_Z = below_Z; this->above_Z = above_Z; };
private:
  I32 below_Z, above_Z;
};

class PULSEcriterionClipAnchorRawXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_raw_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), below_X); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_X < below_X); };
  PULSEcriterionClipAnchorRawXBelow(I32 below_X) { this->below_X = below_X; };
private:
  I32 below_X;
};

class PULSEcriterionClipAnchorRawXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_raw_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), above_X); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_X > above_X); };
  PULSEcriterionClipAnchorRawXAbove(I32 above_X) { this->above_X = above_X; };
private:
  I32 above_X;
};

class PULSEcriterionClipAnchorRawYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_raw_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), below_Y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Y < below_Y); };
  PULSEcriterionClipAnchorRawYBelow(I32 below_Y) { this->below_Y = below_Y; };
private:
  I32 below_Y;
};

class PULSEcriterionClipAnchorRawYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_raw_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), above_Y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Y > above_Y); };
  PULSEcriterionClipAnchorRawYAbove(I32 above_Y) { this->above_Y = above_Y; };
private:
  I32 above_Y;
};

class PULSEcriterionClipAnchorRawZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_raw_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), below_Z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Z < below_Z); };
  PULSEcriterionClipAnchorRawZBelow(I32 below_Z) { this->below_Z = below_Z; };
private:
  I32 below_Z;
};

class PULSEcriterionClipAnchorRawZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "clip_anchor_raw_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), above_Z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Z > above_Z); };
  PULSEcriterionClipAnchorRawZAbove(I32 above_Z) { this->above_Z = above_Z; };
private:
  I32 above_Z;
};

class PULSEcriterionDropScanDirection : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_scan_direction"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), scan_direction); };
  inline BOOL filter(const PULSEpulse* pulse) { return (scan_direction == pulse->scan_direction); };
  PULSEcriterionDropScanDirection(I32 scan_direction) { this->scan_direction = scan_direction; };
private:
  I32 scan_direction;
};

class PULSEcriterionScanDirectionChangeOnly : public PULSEcriterion
{
public:
  inline const char* name() const { return "scan_direction_change_only"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s ", name()); };
  inline BOOL filter(const PULSEpulse* pulse) { if (scan_direction == pulse->scan_direction) return TRUE; I32 s = scan_direction; scan_direction = pulse->scan_direction; return s == -1; };
  void reset() { scan_direction = -1; };
  PULSEcriterionScanDirectionChangeOnly() { reset(); };
private:
  I32 scan_direction;
};

class PULSEcriterionEdgeOfScanLineOnly : public PULSEcriterion
{
public:
  inline const char* name() const { return "edge_of_scan_line_only"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s ", name()); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->edge_of_scan_line == 0); };
};

class PULSEcriterionKeepScanAngle : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_scan_angle"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d ", name(), below_scan, above_scan); };
  inline BOOL filter(const PULSEpulse* pulse) { return FALSE; }; // (pulse->scan_angle_rank < below_scan) || (above_scan < pulse->scan_angle_rank); };
  PULSEcriterionKeepScanAngle(I32 below_scan, I32 above_scan) { if (above_scan < below_scan) { this->below_scan = above_scan; this->above_scan = below_scan; } else { this->below_scan = below_scan; this->above_scan = above_scan; } };
private:
  I32 below_scan, above_scan;
};

class PULSEcriterionDropScanAngleBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_scan_angle_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), below_scan); };
  inline BOOL filter(const PULSEpulse* pulse) { return FALSE; }; // { return (pulse->scan_angle_rank < below_scan); };
  PULSEcriterionDropScanAngleBelow(I32 below_scan) { this->below_scan = below_scan; };
private:
  I32 below_scan;
};

class PULSEcriterionDropScanAngleAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_scan_angle_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), above_scan); };
  inline BOOL filter(const PULSEpulse* pulse) { return FALSE; }; // { return (pulse->scan_angle_rank > above_scan); };
  PULSEcriterionDropScanAngleAbove(I32 above_scan) { this->above_scan = above_scan; };
private:
  I32 above_scan;
};

class PULSEcriterionDropScanAngleBetween : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_scan_angle_between"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d ", name(), below_scan, above_scan); };
  inline BOOL filter(const PULSEpulse* pulse) { return FALSE; }; // { return (below_scan <= pulse->scan_angle_rank) && (pulse->scan_angle_rank <= above_scan); };
  PULSEcriterionDropScanAngleBetween(I32 below_scan, I32 above_scan) { if (above_scan < below_scan) { this->below_scan = above_scan; this->above_scan = below_scan; } else { this->below_scan = below_scan; this->above_scan = above_scan; } };
private:
  I32 below_scan, above_scan;
};

class PULSEcriterionKeepIntensity : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_intensity"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d ", name(), below_intensity, above_intensity); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->intensity < below_intensity) || (pulse->intensity > above_intensity); };
  PULSEcriterionKeepIntensity(I32 below_intensity, I32 above_intensity) { this->below_intensity = below_intensity; this->above_intensity = above_intensity; };
private:
  I32 below_intensity, above_intensity;
};

class PULSEcriterionDropIntensityBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_intensity_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), below_intensity); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->intensity < below_intensity); };
  PULSEcriterionDropIntensityBelow(I32 below_intensity) { this->below_intensity = below_intensity; };
private:
  I32 below_intensity;
};

class PULSEcriterionDropIntensityAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_intensity_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), above_intensity); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->intensity > above_intensity); };
  PULSEcriterionDropIntensityAbove(I32 above_intensity) { this->above_intensity = above_intensity; };
private:
  I32 above_intensity;
};

class PULSEcriterionDropIntensityBetween : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_intensity_between"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d ", name(), below_intensity, above_intensity); };
  inline BOOL filter(const PULSEpulse* pulse) { return (below_intensity <= pulse->intensity) && (pulse->intensity <= above_intensity); };
  PULSEcriterionDropIntensityBetween(I32 below_intensity, I32 above_intensity) { this->below_intensity = below_intensity; this->above_intensity = above_intensity; };
private:
  I32 below_intensity, above_intensity;
};

class PULSEcriterionKeepClassifications : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_classification_mask"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %u ", name(), ~drop_classification_mask); };
  inline BOOL filter(const PULSEpulse* pulse) { return ((1 << pulse->classification) & drop_classification_mask); };
  PULSEcriterionKeepClassifications(U32 keep_classification_mask) { drop_classification_mask = ~keep_classification_mask; };
private:
  U32 drop_classification_mask;
};

class PULSEcriterionKeepTime : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_time"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_time, above_time); };
  inline BOOL filter(const PULSEpulse* pulse) { return (((pulse->T < below_time) || (pulse->T > above_time))); };
  PULSEcriterionKeepTime(F64 below_time, F64 above_time) { this->below_time = below_time; this->above_time = above_time; };
private:
  F64 below_time, above_time;
};

class PULSEcriterionDropTimeBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_time_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_time); };
  inline BOOL filter(const PULSEpulse* pulse) { return ((pulse->T < below_time)); };
  PULSEcriterionDropTimeBelow(F64 below_time) { this->below_time = below_time; };
private:
  F64 below_time;
};

class PULSEcriterionDropTimeAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_time_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_time); };
  inline BOOL filter(const PULSEpulse* pulse) { return ((pulse->T > above_time)); };
  PULSEcriterionDropTimeAbove(F64 above_time) { this->above_time = above_time; };
private:
  F64 above_time;
};

class PULSEcriterionDropTimeBetween : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_time_between"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_time, above_time); };
  inline BOOL filter(const PULSEpulse* pulse) { return (((below_time <= pulse->T) && (pulse->T <= above_time))); };
  PULSEcriterionDropTimeBetween(F64 below_time, F64 above_time) { this->below_time = below_time; this->above_time = above_time; };
private:
  F64 below_time, above_time;
};

class PULSEcriterionKeepDescriptor : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_descriptor_mask"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %u ", name(), ~drop_descriptor_mask); };
  inline BOOL filter(const PULSEpulse* pulse) { return ((1 << pulse->descriptor_index) & drop_descriptor_mask); };
  PULSEcriterionKeepDescriptor(U32 keep_descriptor_mask) { drop_descriptor_mask = ~keep_descriptor_mask; };
private:
  U32 drop_descriptor_mask;
};

class PULSEcriterionKeepFacet : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_facet_mask"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %u ", name(), ~drop_facet_mask); };
  inline BOOL filter(const PULSEpulse* pulse) { return ((1 << pulse->mirror_facet) & drop_facet_mask); };
  PULSEcriterionKeepFacet(U32 keep_facet_mask) { drop_facet_mask = ~keep_facet_mask; };
private:
  U32 drop_facet_mask;
};

class PULSEcriterionKeepEveryNth : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_every_nth"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), every); };
  inline BOOL filter(const PULSEpulse* pulse) { if (counter == every) { counter = 1; return FALSE; } else { counter++; return TRUE; } };
  PULSEcriterionKeepEveryNth(I32 every) { this->every = every; counter = 1; };
private:
  I32 counter;
  I32 every;
};

class PULSEcriterionKeepRandomFraction : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_random_fraction"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), fraction); };
  inline BOOL filter(const PULSEpulse* pulse) { F32 f = (F32)rand()/(F32)RAND_MAX; return f > fraction; };
  PULSEcriterionKeepRandomFraction(F32 fraction) { this->fraction = fraction; };
private:
  F32 fraction;
};

class PULSEcriterionThinWithGrid : public PULSEcriterion
{
public:
  inline const char* name() const { return "thin_with_grid"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), (grid_spacing > 0 ? grid_spacing : -grid_spacing)); };
  inline BOOL filter(const PULSEpulse* pulse)
  { 
    if (grid_spacing < 0)
    {
      grid_spacing = -grid_spacing;
      anker = I32_FLOOR(pulse->get_anchor_y() / grid_spacing);
    }
    I32 pos_x = I32_FLOOR(pulse->get_anchor_x() / grid_spacing);
    I32 pos_y = I32_FLOOR(pulse->get_anchor_y() / grid_spacing) - anker;
    BOOL no_x_anker = FALSE;
    U32* array_size;
    I32** ankers;
    U32*** array;
    U16** array_sizes;
    if (pos_y < 0)
    {
      pos_y = -pos_y - 1;
      ankers = &minus_ankers;
      if ((U32)pos_y < minus_plus_size && minus_plus_sizes[pos_y])
      {
        pos_x -= minus_ankers[pos_y];
        if (pos_x < 0)
        {
          pos_x = -pos_x - 1;
          array_size = &minus_minus_size;
          array = &minus_minus;
          array_sizes = &minus_minus_sizes;
        }
        else
        {
          array_size = &minus_plus_size;
          array = &minus_plus;
          array_sizes = &minus_plus_sizes;
        }
      }
      else
      {
        no_x_anker = TRUE;
        array_size = &minus_plus_size;
        array = &minus_plus;
        array_sizes = &minus_plus_sizes;
      }
    }
    else
    {
      ankers = &plus_ankers;
      if ((U32)pos_y < plus_plus_size && plus_plus_sizes[pos_y])
      {
        pos_x -= plus_ankers[pos_y];
        if (pos_x < 0)
        {
          pos_x = -pos_x - 1;
          array_size = &plus_minus_size;
          array = &plus_minus;
          array_sizes = &plus_minus_sizes;
        }
        else
        {
          array_size = &plus_plus_size;
          array = &plus_plus;
          array_sizes = &plus_plus_sizes;
        }
      }
      else
      {
        no_x_anker = TRUE;
        array_size = &plus_plus_size;
        array = &plus_plus;
        array_sizes = &plus_plus_sizes;
      }
    }
    // maybe grow banded grid in y direction
    if ((U32)pos_y >= *array_size)
    {
      U32 array_size_new = ((pos_y/1024)+1)*1024;
      if (*array_size)
      {
        if (array == &minus_plus || array == &plus_plus) *ankers = (I32*)realloc(*ankers, array_size_new*sizeof(I32));
        *array = (U32**)realloc(*array, array_size_new*sizeof(U32*));
        *array_sizes = (U16*)realloc(*array_sizes, array_size_new*sizeof(U16));
      }
      else
      {
        if (array == &minus_plus || array == &plus_plus) *ankers = (I32*)malloc(array_size_new*sizeof(I32));
        *array = (U32**)malloc(array_size_new*sizeof(U32*));
        *array_sizes = (U16*)malloc(array_size_new*sizeof(U16));
      }
      for (U32 i = *array_size; i < array_size_new; i++)
      {
        (*array)[i] = 0;
        (*array_sizes)[i] = 0;
      }
      *array_size = array_size_new;
    }
    // is this the first x anker for this y pos?
    if (no_x_anker)
    {
      (*ankers)[pos_y] = pos_x;
      pos_x = 0;
    }
    // maybe grow banded grid in x direction
    U32 pos_x_pos = pos_x/32;
    if (pos_x_pos >= (*array_sizes)[pos_y])
    {
      U32 array_sizes_new = ((pos_x_pos/256)+1)*256;
      if ((*array_sizes)[pos_y])
      {
        (*array)[pos_y] = (U32*)realloc((*array)[pos_y], array_sizes_new*sizeof(U32));
      }
      else
      {
        (*array)[pos_y] = (U32*)malloc(array_sizes_new*sizeof(U32));
      }
      for (U16 i = (*array_sizes)[pos_y]; i < array_sizes_new; i++)
      {
        (*array)[pos_y][i] = 0;
      }
      (*array_sizes)[pos_y] = array_sizes_new;
    }
    U32 pos_x_bit = 1 << (pos_x%32);
    if ((*array)[pos_y][pos_x_pos] & pos_x_bit) return TRUE;
    (*array)[pos_y][pos_x_pos] |= pos_x_bit;
    return FALSE;
  }
  void reset()
  {
    if (grid_spacing > 0) grid_spacing = -grid_spacing;
    if (minus_minus_size)
    {
      for (U32 i = 0; i < minus_minus_size; i++) if (minus_minus[i]) free(minus_minus[i]);
      free(minus_minus);
      minus_minus = 0;
      free(minus_minus_sizes);
      minus_minus_sizes = 0;
      minus_minus_size = 0;
    }
    if (minus_plus_size)
    {
      free(minus_ankers);
      minus_ankers = 0;
      for (U32 i = 0; i < minus_plus_size; i++) if (minus_plus[i]) free(minus_plus[i]);
      free(minus_plus);
      minus_plus = 0;
      free(minus_plus_sizes);
      minus_plus_sizes = 0;
      minus_plus_size = 0;
    }
    if (plus_minus_size)
    {
      for (U32 i = 0; i < plus_minus_size; i++) if (plus_minus[i]) free(plus_minus[i]);
      free(plus_minus);
      plus_minus = 0;
      free(plus_minus_sizes);
      plus_minus_sizes = 0;
      plus_minus_size = 0;
    }
    if (plus_plus_size)
    {
      free(plus_ankers);
      plus_ankers = 0;
      for (U32 i = 0; i < plus_plus_size; i++) if (plus_plus[i]) free(plus_plus[i]);
      free(plus_plus);
      plus_plus = 0;
      free(plus_plus_sizes);
      plus_plus_sizes = 0;
      plus_plus_size = 0;
    }
  };
  PULSEcriterionThinWithGrid(F32 grid_spacing)
  {
    this->grid_spacing = -grid_spacing;
    minus_ankers = 0;
    minus_minus_size = 0;
    minus_minus = 0;
    minus_minus_sizes = 0;
    minus_plus_size = 0;
    minus_plus = 0;
    minus_plus_sizes = 0;
    plus_ankers = 0;
    plus_minus_size = 0;
    plus_minus = 0;
    plus_minus_sizes = 0;
    plus_plus_size = 0;
    plus_plus = 0;
    plus_plus_sizes = 0;
  };
  ~PULSEcriterionThinWithGrid() { reset(); };
private:
  F32 grid_spacing;
  I32 anker;
  I32* minus_ankers;
  U32 minus_minus_size;
  U32** minus_minus;
  U16* minus_minus_sizes;
  U32 minus_plus_size;
  U32** minus_plus;
  U16* minus_plus_sizes;
  I32* plus_ankers;
  U32 plus_minus_size;
  U32** plus_minus;
  U16* plus_minus_sizes;
  U32 plus_plus_size;
  U32** plus_plus;
  U16* plus_plus_sizes;
};

void PULSEfilter::clean()
{
  U32 i;
  for (i = 0; i < num_criteria; i++)
  {
    delete criteria[i];
  }
  if (criteria) delete [] criteria;
  if (counters) delete [] counters;
  alloc_criteria = 0;
  num_criteria = 0;
  criteria = 0;
  counters = 0;
}

void PULSEfilter::usage() const
{
  fprintf(stderr,"Filter pulses based on first and last sample.\n");
  fprintf(stderr,"  -clip_tile 631000 4834000 1000 (ll_x ll_y size)\n");
  fprintf(stderr,"  -clip_circle 630250.00 4834750.00 100 (x y radius)\n");
  fprintf(stderr,"  -clip_box 620000 4830000 100 621000 4831000 200 (min_x min_y min_z max_x max_y max_z)\n");
  fprintf(stderr,"  -clip 630000 4834000 631000 4836000 (min_x min_y max_x max_y)\n");
  fprintf(stderr,"  -clip_x_below 630000.50 (min_x)\n");
  fprintf(stderr,"  -clip_y_below 4834500.25 (min_y)\n");
  fprintf(stderr,"  -clip_x_above 630500.50 (max_x)\n");
  fprintf(stderr,"  -clip_y_above 4836000.75 (max_y)\n");
  fprintf(stderr,"  -clip_z 11.125 130.725 (min_z, max_z)\n");
  fprintf(stderr,"  -clip_z_below 11.125 (min_z)\n");
  fprintf(stderr,"  -clip_z_above 130.725 (max_z)\n");
  fprintf(stderr,"Filter pulses based on first sample.\n");
  fprintf(stderr,"  -clip_first 630000 4834000 631000 4836000 (min_x min_y max_x max_y)\n");
  fprintf(stderr,"  -clip_first_x_below 630000.50 (min_x)\n");
  fprintf(stderr,"  -clip_first_y_below 4834500.25 (min_y)\n");
  fprintf(stderr,"  -clip_first_x_above 630500.50 (max_x)\n");
  fprintf(stderr,"  -clip_first_y_above 4836000.75 (max_y)\n");
  fprintf(stderr,"  -clip_first_z 11.125 130.725 (min_z, max_z)\n");
  fprintf(stderr,"  -clip_first_z_below 11.125 (min_z)\n");
  fprintf(stderr,"  -clip_first_z_above 130.725 (max_z)\n");
  fprintf(stderr,"Filter pulses based on last sample.\n");
  fprintf(stderr,"  -clip_last 630000 4834000 631000 4836000 (min_x min_y max_x max_y)\n");
  fprintf(stderr,"  -clip_last_x_below 630000.50 (min_x)\n");
  fprintf(stderr,"  -clip_last_y_below 4834500.25 (min_y)\n");
  fprintf(stderr,"  -clip_last_x_above 630500.50 (max_x)\n");
  fprintf(stderr,"  -clip_last_y_above 4836000.75 (max_y)\n");
  fprintf(stderr,"  -clip_last_z 11.125 130.725 (min_z, max_z)\n");
  fprintf(stderr,"  -clip_last_z_below 11.125 (min_z)\n");
  fprintf(stderr,"  -clip_last_z_above 130.725 (max_z)\n");
  fprintf(stderr,"Filter pulses based on anchor point.\n");
  fprintf(stderr,"  -clip_anchor 630000 4834000 631000 4836000 (min_x min_y max_x max_y)\n");
  fprintf(stderr,"  -clip_anchor_x_below 630000.50 (min_x)\n");
  fprintf(stderr,"  -clip_anchor_y_below 4834500.25 (min_y)\n");
  fprintf(stderr,"  -clip_anchor_x_above 630500.50 (max_x)\n");
  fprintf(stderr,"  -clip_anchor_y_above 4836000.75 (max_y)\n");
  fprintf(stderr,"  -clip_anchor_z 11.125 130.725 (min_z, max_z)\n");
  fprintf(stderr,"  -clip_anchor_z_below 11.125 (min_z)\n");
  fprintf(stderr,"  -clip_anchor_z_above 130.725 (max_z)\n");
  fprintf(stderr,"Filter pulses based on the scanline flags.\n");
  fprintf(stderr,"  -drop_scan_direction 0\n");
  fprintf(stderr,"  -scan_direction_change_only\n");
  fprintf(stderr,"  -edge_of_scan_line_only\n");
/*
  fprintf(stderr,"Filter pulses based on their intensity.\n");
  fprintf(stderr,"  -keep_intensity 20 100\n");
  fprintf(stderr,"  -drop_intensity_below 20\n");
  fprintf(stderr,"  -drop_intensity_above 220\n");
  fprintf(stderr,"  -drop_intensity_between 10 100\n");
  fprintf(stderr,"Filter pulses based on their classification.\n");
  fprintf(stderr,"  -keep_class 1 3 7\n");
  fprintf(stderr,"  -drop_class 4 2\n");
  fprintf(stderr,"Filter pulses based on their scan angle.\n");
  fprintf(stderr,"  -keep_scan_angle -15 15\n");
  fprintf(stderr,"  -drop_scan_angle_below -15\n");
  fprintf(stderr,"  -drop_scan_angle_above 15\n");
  fprintf(stderr,"  -drop_scan_angle_between -25 -23\n");
*/
  fprintf(stderr,"Filter pulses based on their time stamp.\n");
  fprintf(stderr,"  -keep_time 11.125 130.725\n");
  fprintf(stderr,"  -drop_time_below 11.125\n");
  fprintf(stderr,"  -drop_time_above 130.725\n");
  fprintf(stderr,"  -drop_time_between 22.0 48.0\n");
  fprintf(stderr,"Filter pulses based on their pulse descriptor.\n");
  fprintf(stderr,"  -keep_descriptor 1 2\n");
  fprintf(stderr,"  -drop_descriptor 0\n");
  fprintf(stderr,"Filter pulses based on scan direction or mirror facet.\n");
  fprintf(stderr,"  -keep_scan_direction 1\n");
  fprintf(stderr,"  -drop_scan_direction 0\n");
  fprintf(stderr,"  -keep_facet 1 3\n");
  fprintf(stderr,"  -drop_facet 0\n");
  fprintf(stderr,"Filter pulses with simple thinning.\n");
  fprintf(stderr,"  -keep_every_nth 2\n");
  fprintf(stderr,"  -keep_random_fraction 0.1\n");
  fprintf(stderr,"  -thin_with_grid 1.0\n");
}

BOOL PULSEfilter::parse(int argc, char* argv[])
{
  int i;

  U32 keep_descriptor_mask = 0;
  U32 drop_descriptor_mask = 0;

  U32 keep_facet_mask = 0;
  U32 drop_facet_mask = 0;

  U32 keep_classification_mask = 0;
  U32 drop_classification_mask = 0;


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
    else if (strcmp(argv[i],"-clip_tile") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: llx lly size\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipTile((F32)atof(argv[i+1]), (F32)atof(argv[i+2]), (F32)atof(argv[i+3])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3; 
    }
    else if (strcmp(argv[i],"-clip_circle") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: center_x center_y radius\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipCircle(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
    }
    else if (strcmp(argv[i],"-clip") == 0 || strcmp(argv[i],"-clip_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x min_y max_x max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipXY(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-clip_box") == 0 || strcmp(argv[i],"-clip_xyz") == 0)
    {
      if ((i+6) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 6 arguments: min_x min_y min_z max_x max_y max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipBox(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4]), atof(argv[i+5]), atof(argv[i+6])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; *argv[i+5]='\0'; *argv[i+6]='\0'; i+=6; 
    }
    else if (strcmp(argv[i],"-clip_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_z max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipZ(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-clip_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipXBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipYBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipZBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipXAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipYAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipZAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_first") == 0 || strcmp(argv[i],"-clip_first_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x min_y max_x max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipFirstXY(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-clip_first_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_z max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipFirstZ(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-clip_first_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipFirstXBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_first_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipFirstYBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_first_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipFirstZBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_first_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipFirstXAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_first_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipFirstYAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_first_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipFirstZAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_last") == 0 || strcmp(argv[i],"-clip_last_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x min_y max_x max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipLastXY(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-clip_last_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_z max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipLastZ(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-clip_last_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipLastXBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_last_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipLastYBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_last_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipLastZBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_last_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipLastXAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_last_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipLastYAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_last_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipLastZAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor") == 0 || strcmp(argv[i],"-clip_anchor_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x min_y max_x max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorXY(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-clip_anchor_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_z max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorZ(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-clip_anchor_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorXBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorYBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorZBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorXAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorYAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorZAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    
    
    else if (strcmp(argv[i],"-clip_anchor_raw") == 0 || strcmp(argv[i],"-clip_anchor_raw_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_raw_x min_raw_y max_raw_x max_raw_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorRawXY(atoi(argv[i+1]), atoi(argv[i+2]), atoi(argv[i+3]), atoi(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-clip_anchor_raw_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_raw_z max_raw_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorRawZ(atoi(argv[i+1]), atoi(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-clip_anchor_raw_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_raw_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorRawXBelow(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_raw_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_raw_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorRawYBelow(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_raw_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_raw_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorRawZBelow(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_raw_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_raw_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorRawXAbove(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_raw_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_raw_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorRawYAbove(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-clip_anchor_raw_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_raw_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionClipAnchorRawZAbove(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-keep_scan_direction") == 0)
    {
      add_criterion(new PULSEcriterionDropScanDirection((atoi(argv[i+1]) ? 0 : 1)));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_scan_direction") == 0)
    {
      add_criterion(new PULSEcriterionDropScanDirection(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-scan_direction_change_only") == 0 || strcmp(argv[i],"-scan_direction_change") == 0)
    {
      add_criterion(new PULSEcriterionScanDirectionChangeOnly());
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-edge_of_scan_line_only") == 0 || strcmp(argv[i],"-edge_of_scan_line") == 0)
    {
      add_criterion(new PULSEcriterionEdgeOfScanLineOnly());
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-keep_intensity") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min max\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepIntensity(atoi(argv[i+1]), atoi(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-drop_intensity_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropIntensityAbove(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_intensity_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropIntensityBelow(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_intensity_between") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min max\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropIntensityBetween(atoi(argv[i+1]), atoi(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-keep_scan_angle") == 0 || strcmp(argv[i],"-keep_scan") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min max\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepScanAngle(atoi(argv[i+1]), atoi(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-drop_scan_angle_above") == 0 || strcmp(argv[i],"-drop_scan_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropScanAngleAbove(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_scan_angle_below") == 0 || strcmp(argv[i],"-drop_scan_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropScanAngleBelow(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }    
    else if (strcmp(argv[i],"-drop_scan_angle_between") == 0 || strcmp(argv[i],"-drop_scan_between") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min max\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropScanAngleBetween(atoi(argv[i+1]), atoi(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-keep_classification") == 0 || strcmp(argv[i],"-keep_class") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 at least argument: classification\n", argv[i]);
        return FALSE;
      }
      *argv[i]='\0';
      i+=1;
      do
      {
        keep_classification_mask |= (1 << atoi(argv[i]));
        *argv[i]='\0';
        i+=1;
      } while ((i < argc) && ('0' <= *argv[i]) && (*argv[i] <= '9'));
      i-=1;
    }
    else if (strcmp(argv[i],"-keep_classification_mask") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: classifications_mask\n", argv[i]);
        return FALSE;
      }
      keep_classification_mask = atoi(argv[i+1]);
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_classification") == 0 || strcmp(argv[i],"-drop_class") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs at least 1 argument: classification\n", argv[i]);
        return FALSE;
      }
      *argv[i]='\0';
      i+=1;
      do
      {
        drop_classification_mask |= (1 << atoi(argv[i]));
        *argv[i]='\0';
        i+=1;
      } while ((i < argc) && ('0' <= *argv[i]) && (*argv[i] <= '9'));
      i-=1;
    }
    else if (strcmp(argv[i],"-keep_descriptor") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 at least argument: index\n", argv[i]);
        return FALSE;
      }
      *argv[i]='\0';
      i+=1;
      do
      {
        keep_descriptor_mask |= (1 << atoi(argv[i]));
        *argv[i]='\0';
        i+=1;
      } while ((i < argc) && ('0' <= *argv[i]) && (*argv[i] <= '9'));
      i-=1;
    }
    else if (strcmp(argv[i],"-drop_descriptor") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs at least 1 argument: index\n", argv[i]);
        return FALSE;
      }
      *argv[i]='\0';
      i+=1;
      do
      {
        drop_descriptor_mask |= (1 << atoi(argv[i]));
        *argv[i]='\0';
        i+=1;
      } while ((i < argc) && ('0' <= *argv[i]) && (*argv[i] <= '9'));
      i-=1;
    }
    else if (strcmp(argv[i],"-keep_facet") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 at least argument: index\n", argv[i]);
        return FALSE;
      }
      *argv[i]='\0';
      i+=1;
      do
      {
        keep_facet_mask |= (1 << atoi(argv[i]));
        *argv[i]='\0';
        i+=1;
      } while ((i < argc) && ('0' <= *argv[i]) && (*argv[i] <= '9'));
      i-=1;
    }
    else if (strcmp(argv[i],"-drop_facet") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs at least 1 argument: index\n", argv[i]);
        return FALSE;
      }
      *argv[i]='\0';
      i+=1;
      do
      {
        drop_facet_mask |= (1 << atoi(argv[i]));
        *argv[i]='\0';
        i+=1;
      } while ((i < argc) && ('0' <= *argv[i]) && (*argv[i] <= '9'));
      i-=1;
    }
    else if (strcmp(argv[i],"-keep_time") == 0 || strcmp(argv[i],"-keep_gps_time") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min max\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepTime(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-drop_time_above") == 0 || strcmp(argv[i],"-drop_gps_time_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_time\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropTimeAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_time_below") == 0 || strcmp(argv[i],"-drop_gps_time_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_time\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropTimeBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_time_between") == 0 || strcmp(argv[i],"-drop_gps_time_between") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min max\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropTimeBetween(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-keep_every_nth") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: nth\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepEveryNth((I32)atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-keep_random_fraction") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: fraction\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepRandomFraction((F32)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-thin_with_grid") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: grid_spacing\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionThinWithGrid((F32)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
  }

  if (drop_descriptor_mask)
  {
    if (keep_descriptor_mask == 0) keep_descriptor_mask = ~drop_descriptor_mask;
  }
  if (keep_descriptor_mask) add_criterion(new PULSEcriterionKeepDescriptor(keep_descriptor_mask));

  if (drop_facet_mask)
  {
    if (keep_facet_mask == 0) keep_facet_mask = ~drop_facet_mask;
  }
  if (keep_facet_mask) add_criterion(new PULSEcriterionKeepFacet(keep_facet_mask));

  if (drop_classification_mask)
  {
    if (keep_classification_mask == 0) keep_classification_mask = ~drop_classification_mask;
  }
  if (keep_classification_mask) add_criterion(new PULSEcriterionKeepClassifications(keep_classification_mask));

  return TRUE;
}

I32 PULSEfilter::unparse(char* string) const
{
  U32 i;
  I32 n = 0;
  for (i = 0; i < num_criteria; i++)
  {
    n += criteria[i]->get_command(&string[n]);
  }
  return n;
}

void PULSEfilter::addClipCircle(F64 x, F64 y, F64 radius)
{
  add_criterion(new PULSEcriterionClipCircle(x, y, radius));
}

void PULSEfilter::addClipBox(F64 min_x, F64 min_y, F64 min_z, F64 max_x, F64 max_y, F64 max_z)
{
  add_criterion(new PULSEcriterionClipBox(min_x, min_y, min_z, max_x, max_y, max_z));
}

BOOL PULSEfilter::filter(const PULSEpulse* pulse)
{
  U32 i;

  for (i = 0; i < num_criteria; i++)
  {
    if (criteria[i]->filter(pulse))
    {
      counters[i]++;
      return TRUE; // pulse was filtered
    }
  }
  return FALSE; // pulse survived
}

void PULSEfilter::reset()
{
  U32 i;
  for (i = 0; i < num_criteria; i++)
  {
    criteria[i]->reset();
  }
}

PULSEfilter::PULSEfilter()
{
  alloc_criteria = 0;
  num_criteria = 0;
  criteria = 0;
  counters = 0;
}

PULSEfilter::~PULSEfilter()
{
  if (criteria) clean();
}

void PULSEfilter::add_criterion(PULSEcriterion* filter_criterion)
{
  if (num_criteria == alloc_criteria)
  {
    U32 i;
    alloc_criteria += 16;
    PULSEcriterion** temp_criteria = new PULSEcriterion*[alloc_criteria];
    int* temp_counters = new int[alloc_criteria];
    if (criteria)
    {
      for (i = 0; i < num_criteria; i++)
      {
        temp_criteria[i] = criteria[i];
        temp_counters[i] = counters[i];
      }
      delete [] criteria;
      delete [] counters;
    }
    criteria = temp_criteria;
    counters = temp_counters;
  }
  criteria[num_criteria] = filter_criterion;
  counters[num_criteria] = 0;
  num_criteria++;
}
