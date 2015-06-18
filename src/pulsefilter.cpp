/*
===============================================================================

  FILE:  pulsefilter.cpp
  
  CONTENTS:
  
    see corresponding header file
  
  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2007-2015, martin isenburg, rapidlasso - fast tools to catch reality

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

/*

class PULSEcriterionKeepTile : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_tile"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g ", name(), ll_x, ll_y, tile_size); };
  inline BOOL filter(const PULSEpulse* pulse) { return (!pulse->inside_tile(ll_x, ll_y, ur_x, ur_y)); };
  PULSEcriterionKeepTile(F32 ll_x, F32 ll_y, F32 tile_size) { this->ll_x = ll_x; this->ll_y = ll_y; this->ur_x = ll_x+tile_size; this->ur_y = ll_y+tile_size; this->tile_size = tile_size; };
private:
  F32 ll_x, ll_y, ur_x, ur_y, tile_size;
};

class PULSEcriterionKeepCircle : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_circle"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g ", name(), center_x, center_y, radius); };
  inline BOOL filter(const PULSEpulse* pulse) { return (!pulse->inside_circle(center_x, center_y, radius_squared)); };
  PULSEcriterionKeepCircle(F64 x, F64 y, F64 radius) { this->center_x = x; this->center_y = y; this->radius = radius; this->radius_squared = radius*radius; };
private:
  F64 center_x, center_y, radius, radius_squared;
};

class PULSEcriterionKeepBox : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_box"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g %g %g ", name(), min_x, min_y, min_z, max_x, max_y, max_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (!pulse->inside_box(min_x, min_y, min_z, max_x, max_y, max_z)); };
  PULSEcriterionKeepBox(F64 min_x, F64 min_y, F64 min_z, F64 max_x, F64 max_y, F64 max_z) { this->min_x = min_x; this->min_y = min_y; this->min_z = min_x; this->max_x = max_x; this->max_y = max_y; this->max_z = max_x; };
private:
  F64 min_x, min_y, min_z, max_x, max_y, max_z;
};

class PULSEcriterionKeepXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g ", name(), below_x, below_y, above_x, above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (!pulse->inside_rectangle(below_x, below_y, above_x, above_y)); };
  PULSEcriterionKeepXY(F64 below_x, F64 below_y, F64 above_x, F64 above_y) { this->below_x = below_x; this->below_y = below_y; this->above_x = above_x; this->above_y = above_y; };
private:
  F64 below_x, below_y, above_x, above_y;
};

class PULSEcriterionKeepZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_z, above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 z = pulse->get_anchor_z(); return (z < below_z) || (z > above_z); };
  PULSEcriterionKeepZ(F64 below_z, F64 above_z) { this->below_z = below_z; this->above_z = above_z; };
private:
  F64 below_z, above_z;
};

class PULSEcriterionDropXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_x() < below_x); };
  PULSEcriterionDropXBelow(F64 below_x) { this->below_x = below_x; };
private:
  F64 below_x;
};

class PULSEcriterionDropXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_x() > above_x); };
  PULSEcriterionDropXAbove(F64 above_x) { this->above_x = above_x; };
private:
  F64 above_x;
};

class PULSEcriterionDropYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_y() < below_y); };
  PULSEcriterionDropYBelow(F64 below_y) { this->below_y = below_y; };
private:
  F64 below_y;
};

class PULSEcriterionDropYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_y() > above_y); };
  PULSEcriterionDropYAbove(F64 above_y) { this->above_y = above_y; };
private:
  F64 above_y;
};

class PULSEcriterionDropZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_z() < below_z); };
  PULSEcriterionDropZBelow(F64 below_z) { this->below_z = below_z; };
private:
  F64 below_z;
};

class PULSEcriterionDropZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_z() > above_z); };
  PULSEcriterionDropZAbove(F64 above_z) { this->above_z = above_z; };
private:
  F64 above_z;
};

*/

class PULSEcriterionKeepFirstXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_first"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g ", name(), below_x, below_y, above_x, above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 x = pulse->compute_and_get_first_x(); if ((x < below_x) || (x >= above_x)) return TRUE; F64 y = pulse->compute_and_get_first_y(); return ((y < below_y) || (y >= above_y)); };
  PULSEcriterionKeepFirstXY(F64 below_x, F64 below_y, F64 above_x, F64 above_y) { this->below_x = below_x; this->below_y = below_y; this->above_x = above_x; this->above_y = above_y; };
private:
  F64 below_x, below_y, above_x, above_y;
};

class PULSEcriterionKeepFirstZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_first_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_z, above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 z = pulse->compute_and_get_first_z(); return ((z < below_z) || (z >= above_z)); };
  PULSEcriterionKeepFirstZ(F64 below_z, F64 above_z) { this->below_z = below_z; this->above_z = above_z; };
private:
  F64 below_z, above_z;
};

class PULSEcriterionDropFirstXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_first_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_x() < below_x); };
  PULSEcriterionDropFirstXBelow(F64 below_x) { this->below_x = below_x; };
private:
  F64 below_x;
};

class PULSEcriterionDropFirstXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_first_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_x() >= above_x); };
  PULSEcriterionDropFirstXAbove(F64 above_x) { this->above_x = above_x; };
private:
  F64 above_x;
};

class PULSEcriterionDropFirstYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_first_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_y() < below_y); };
  PULSEcriterionDropFirstYBelow(F64 below_y) { this->below_y = below_y; };
private:
  F64 below_y;
};

class PULSEcriterionDropFirstYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_first_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_y() >= above_y); };
  PULSEcriterionDropFirstYAbove(F64 above_y) { this->above_y = above_y; };
private:
  F64 above_y;
};

class PULSEcriterionDropFirstZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_first_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_z() < below_z); };
  PULSEcriterionDropFirstZBelow(F64 below_z) { this->below_z = below_z; };
private:
  F64 below_z;
};

class PULSEcriterionDropFirstZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_first_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_first_z() >= above_z); };
  PULSEcriterionDropFirstZAbove(F64 above_z) { this->above_z = above_z; };
private:
  F64 above_z;
};

class PULSEcriterionKeepLastXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_last"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g ", name(), below_x, below_y, above_x, above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 x = pulse->compute_and_get_last_x(); if ((x < below_x) || (x >= above_x)) return TRUE; F64 y = pulse->compute_and_get_last_y(); return ((y < below_y) || (y >= above_y)); };
  PULSEcriterionKeepLastXY(F64 below_x, F64 below_y, F64 above_x, F64 above_y) { this->below_x = below_x; this->below_y = below_y; this->above_x = above_x; this->above_y = above_y; };
private:
  F64 below_x, below_y, above_x, above_y;
};

class PULSEcriterionKeepLastZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_last_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_z, above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 z = pulse->compute_and_get_last_z(); return ((z < below_z) || (z >= above_z)); };
  PULSEcriterionKeepLastZ(F64 below_z, F64 above_z) { this->below_z = below_z; this->above_z = above_z; };
private:
  F64 below_z, above_z;
};

class PULSEcriterionDropLastXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_last_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_x() < below_x); };
  PULSEcriterionDropLastXBelow(F64 below_x) { this->below_x = below_x; };
private:
  F64 below_x;
};

class PULSEcriterionDropLastXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_last_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_x() >= above_x); };
  PULSEcriterionDropLastXAbove(F64 above_x) { this->above_x = above_x; };
private:
  F64 above_x;
};

class PULSEcriterionDropLastYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_last_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_y() < below_y); };
  PULSEcriterionDropLastYBelow(F64 below_y) { this->below_y = below_y; };
private:
  F64 below_y;
};

class PULSEcriterionDropLastYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_last_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_y() >= above_y); };
  PULSEcriterionDropLastYAbove(F64 above_y) { this->above_y = above_y; };
private:
  F64 above_y;
};

class PULSEcriterionDropLastZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_last_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_z() < below_z); };
  PULSEcriterionDropLastZBelow(F64 below_z) { this->below_z = below_z; };
private:
  F64 below_z;
};

class PULSEcriterionDropLastZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_last_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->compute_and_get_last_z() >= above_z); };
  PULSEcriterionDropLastZAbove(F64 above_z) { this->above_z = above_z; };
private:
  F64 above_z;
};

class PULSEcriterionKeepAnchorXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_anchor"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g %g ", name(), below_x, below_y, above_x, above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 x = pulse->get_anchor_x(); if ((x < below_x) || (x >= above_x)) return TRUE; F64 y = pulse->get_anchor_y(); return ((y < below_y) || (y >= above_y)); };
  PULSEcriterionKeepAnchorXY(F64 below_x, F64 below_y, F64 above_x, F64 above_y) { this->below_x = below_x; this->below_y = below_y; this->above_x = above_x; this->above_y = above_y; };
private:
  F64 below_x, below_y, above_x, above_y;
};

class PULSEcriterionKeepAnchorZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_anchor_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_z, above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { F64 z = pulse->get_anchor_z(); return ((z < below_z) || (z >= above_z)); };
  PULSEcriterionKeepAnchorZ(F64 below_z, F64 above_z) { this->below_z = below_z; this->above_z = above_z; };
private:
  F64 below_z, above_z;
};

class PULSEcriterionDropAnchorXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_x() < below_x); };
  PULSEcriterionDropAnchorXBelow(F64 below_x) { this->below_x = below_x; };
private:
  F64 below_x;
};

class PULSEcriterionDropAnchorXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_x); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_x() >= above_x); };
  PULSEcriterionDropAnchorXAbove(F64 above_x) { this->above_x = above_x; };
private:
  F64 above_x;
};

class PULSEcriterionDropAnchorYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_y() < below_y); };
  PULSEcriterionDropAnchorYBelow(F64 below_y) { this->below_y = below_y; };
private:
  F64 below_y;
};

class PULSEcriterionDropAnchorYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_y() >= above_y); };
  PULSEcriterionDropAnchorYAbove(F64 above_y) { this->above_y = above_y; };
private:
  F64 above_y;
};

class PULSEcriterionDropAnchorZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_z() < below_z); };
  PULSEcriterionDropAnchorZBelow(F64 below_z) { this->below_z = below_z; };
private:
  F64 below_z;
};

class PULSEcriterionDropAnchorZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->get_anchor_z() >= above_z); };
  PULSEcriterionDropAnchorZAbove(F64 above_z) { this->above_z = above_z; };
private:
  F64 above_z;
};

class PULSEcriterionKeepAnchorRawXY : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_anchor_raw_xy"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d %d %d ", name(), below_X, below_Y, above_X, above_Y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_X < below_X) || (pulse->anchor_X <= below_X) || (pulse->anchor_Y >= above_Y) || (pulse->anchor_Y >= above_Y); };
  PULSEcriterionKeepAnchorRawXY(I32 below_X, I32 below_Y, I32 above_X, I32 above_Y) { this->below_X = below_X; this->below_Y = below_Y; this->above_X = above_X; this->above_Y = above_Y; };
private:
  I32 below_X, below_Y, above_X, above_Y;
};

class PULSEcriterionKeepAnchorRawZ : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_anchor_raw_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d ", name(), below_Z, above_Z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Z < below_Z) || (pulse->anchor_Z > above_Z); };
  PULSEcriterionKeepAnchorRawZ(I32 below_Z, I32 above_Z) { this->below_Z = below_Z; this->above_Z = above_Z; };
private:
  I32 below_Z, above_Z;
};

class PULSEcriterionDropAnchorRawXBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_raw_x_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), below_X); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_X < below_X); };
  PULSEcriterionDropAnchorRawXBelow(I32 below_X) { this->below_X = below_X; };
private:
  I32 below_X;
};

class PULSEcriterionDropAnchorRawXAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_raw_x_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), above_X); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_X > above_X); };
  PULSEcriterionDropAnchorRawXAbove(I32 above_X) { this->above_X = above_X; };
private:
  I32 above_X;
};

class PULSEcriterionDropAnchorRawYBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_raw_y_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), below_Y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Y < below_Y); };
  PULSEcriterionDropAnchorRawYBelow(I32 below_Y) { this->below_Y = below_Y; };
private:
  I32 below_Y;
};

class PULSEcriterionDropAnchorRawYAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_raw_y_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), above_Y); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Y > above_Y); };
  PULSEcriterionDropAnchorRawYAbove(I32 above_Y) { this->above_Y = above_Y; };
private:
  I32 above_Y;
};

class PULSEcriterionDropAnchorRawZBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_raw_z_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), below_Z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Z < below_Z); };
  PULSEcriterionDropAnchorRawZBelow(I32 below_Z) { this->below_Z = below_Z; };
private:
  I32 below_Z;
};

class PULSEcriterionDropAnchorRawZAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_anchor_raw_z_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), above_Z); };
  inline BOOL filter(const PULSEpulse* pulse) { return (pulse->anchor_Z > above_Z); };
  PULSEcriterionDropAnchorRawZAbove(I32 above_Z) { this->above_Z = above_Z; };
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
  inline BOOL filter(const PULSEpulse* pulse) { return (((pulse->get_t() < below_time) || (pulse->get_t() > above_time))); };
  PULSEcriterionKeepTime(F64 below_time, F64 above_time) { this->below_time = below_time; this->above_time = above_time; };
private:
  F64 below_time, above_time;
};

class PULSEcriterionDropTimeBelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_time_below"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), below_time); };
  inline BOOL filter(const PULSEpulse* pulse) { return ((pulse->get_t() < below_time)); };
  PULSEcriterionDropTimeBelow(F64 below_time) { this->below_time = below_time; };
private:
  F64 below_time;
};

class PULSEcriterionDropTimeAbove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_time_above"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), above_time); };
  inline BOOL filter(const PULSEpulse* pulse) { return ((pulse->get_t() > above_time)); };
  PULSEcriterionDropTimeAbove(F64 above_time) { this->above_time = above_time; };
private:
  F64 above_time;
};

class PULSEcriterionDropTimeBetween : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_time_between"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), below_time, above_time); };
  inline BOOL filter(const PULSEpulse* pulse) { return (((below_time <= pulse->get_t()) && (pulse->get_t() <= above_time))); };
  PULSEcriterionDropTimeBetween(F64 below_time, F64 above_time) { this->below_time = below_time; this->above_time = above_time; };
private:
  F64 below_time, above_time;
};

class PULSEcriterionKeepT : public PULSEcriterion
{
public:
  inline const char* name() const { return "keep_T"; };
#ifdef _WIN32
  inline int get_command(char* string) const { return sprintf(string, "-%s %I64d %I64d ", name(), below_T, above_T); };
#else
  inline int get_command(char* string) const { return sprintf(string, "-%s %lld %lld ", name(), below_T, above_T); };
#endif
  inline BOOL filter(const PULSEpulse* pulse) { return (((pulse->T < below_T) || (pulse->T > above_T))); };
  PULSEcriterionKeepT(I64 below_T, I64 above_T) { this->below_T = below_T; this->above_T = above_T; };
private:
  I64 below_T, above_T;
};

class PULSEcriterionDropTbelow : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_T_below"; };
#ifdef _WIN32
  inline int get_command(char* string) const { return sprintf(string, "-%s %I64d ", name(), below_T); };
#else
  inline int get_command(char* string) const { return sprintf(string, "-%s %lld ", name(), below_T); };
#endif
  inline BOOL filter(const PULSEpulse* pulse) { return ((pulse->T < below_T)); };
  PULSEcriterionDropTbelow(I64 below_T) { this->below_T = below_T; };
private:
  I64 below_T;
};

class PULSEcriterionDropTabove : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_T_above"; };
#ifdef _WIN32
  inline int get_command(char* string) const { return sprintf(string, "-%s %I64d ", name(), above_T); };
#else
  inline int get_command(char* string) const { return sprintf(string, "-%s %lld ", name(), above_T); };
#endif
  inline BOOL filter(const PULSEpulse* pulse) { return ((pulse->T > above_T)); };
  PULSEcriterionDropTabove(I64 above_T) { this->above_T = above_T; };
private:
  I64 above_T;
};

class PULSEcriterionDropTbetween : public PULSEcriterion
{
public:
  inline const char* name() const { return "drop_T_between"; };
#ifdef _WIN32
  inline int get_command(char* string) const { return sprintf(string, "-%s %I64d %I64d ", name(), below_T, above_T); };
#else
  inline int get_command(char* string) const { return sprintf(string, "-%s %lld %lld ", name(), below_T, above_T); };
#endif
  inline BOOL filter(const PULSEpulse* pulse) { return (((below_T <= pulse->T) && (pulse->T <= above_T))); };
  PULSEcriterionDropTbetween(I64 below_T, I64 above_T) { this->below_T = below_T; this->above_T = above_T; };
private:
  I64 below_T, above_T;
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
  fprintf(stderr,"Filter pulses based on their first waveform sample.\n");
  fprintf(stderr,"  -keep_first 630000 4834000 631000 4836000 (min_x min_y max_x max_y)\n");
  fprintf(stderr,"  -drop_first_x_below 630000.50 (min_x)\n");
  fprintf(stderr,"  -drop_first_y_below 4834500.25 (min_y)\n");
  fprintf(stderr,"  -drop_first_x_above 630500.50 (max_x)\n");
  fprintf(stderr,"  -drop_first_y_above 4836000.75 (max_y)\n");
  fprintf(stderr,"  -keep_first_z 11.125 130.725 (min_z, max_z)\n");
  fprintf(stderr,"  -drop_first_z_below 11.125 (min_z)\n");
  fprintf(stderr,"  -drop_first_z_above 130.725 (max_z)\n");
  fprintf(stderr,"Filter pulses based on their last waveform sample.\n");
  fprintf(stderr,"  -keep_last 630000 4834000 631000 4836000 (min_x min_y max_x max_y)\n");
  fprintf(stderr,"  -drop_last_x_below 630000.50 (min_x)\n");
  fprintf(stderr,"  -drop_last_y_below 4834500.25 (min_y)\n");
  fprintf(stderr,"  -drop_last_x_above 630500.50 (max_x)\n");
  fprintf(stderr,"  -drop_last_y_above 4836000.75 (max_y)\n");
  fprintf(stderr,"  -keep_last_z 11.125 130.725 (min_z, max_z)\n");
  fprintf(stderr,"  -drop_last_z_below 11.125 (min_z)\n");
  fprintf(stderr,"  -drop_last_z_above 130.725 (max_z)\n");
  fprintf(stderr,"Filter pulses based on anchor point.\n");
  fprintf(stderr,"  -keep_anchor 630000 4834000 631000 4836000 (min_x min_y max_x max_y)\n");
  fprintf(stderr,"  -drop_anchor_x_below 630000.50 (min_x)\n");
  fprintf(stderr,"  -drop_anchor_y_below 4834500.25 (min_y)\n");
  fprintf(stderr,"  -drop_anchor_x_above 630500.50 (max_x)\n");
  fprintf(stderr,"  -drop_anchor_y_above 4836000.75 (max_y)\n");
  fprintf(stderr,"  -keep_anchor_z 11.125 130.725 (min_z, max_z)\n");
  fprintf(stderr,"  -drop_anchor_z_below 11.125 (min_z)\n");
  fprintf(stderr,"  -drop_anchor_z_above 130.725 (max_z)\n");
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
  fprintf(stderr,"Filter pulses based on the internally stored integer time stamp.\n");
  fprintf(stderr,"  -keep_T 401003726000 401003728000\n");
  fprintf(stderr,"  -drop_T_below 401003726000\n");
  fprintf(stderr,"  -drop_T_above 401003728000\n");
  fprintf(stderr,"  -drop_T_between 401003726000 401003728000\n");
  fprintf(stderr,"Filter pulses based on a double-precision float GPS time stamp.\n");
  fprintf(stderr,"  -keep_time 401003.726 401003.728\n");
  fprintf(stderr,"  -drop_time_below 401003.726\n");
  fprintf(stderr,"  -drop_time_above 401003.728\n");
  fprintf(stderr,"  -drop_time_between 401003.726 401003.728\n");
  fprintf(stderr,"Filter pulses based on their pulse descriptor.\n");
  fprintf(stderr,"  -keep_descriptor 1 2\n");
  fprintf(stderr,"  -drop_descriptor 0\n");
  fprintf(stderr,"Filter pulses based on scan direction or mirror facet.\n");
  fprintf(stderr,"  -scan_direction_change_only\n");
  fprintf(stderr,"  -edge_of_scan_line_only\n");
  fprintf(stderr,"  -keep_scan_direction 1\n");
  fprintf(stderr,"  -drop_scan_direction 0\n");
  fprintf(stderr,"  -keep_facet 1 3\n");
  fprintf(stderr,"  -drop_facet 0\n");
  fprintf(stderr,"Filter pulses with simple thinning.\n");
  fprintf(stderr,"  -keep_every_nth 2\n");
  fprintf(stderr,"  -keep_random_fraction 0.1\n");
/*
  fprintf(stderr,"  -thin_with_grid 1.0\n");
*/
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
/*
    else if (strcmp(argv[i],"-keep_tile") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: llx lly size\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepTile((F32)atof(argv[i+1]), (F32)atof(argv[i+2]), (F32)atof(argv[i+3])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3; 
    }
    else if (strcmp(argv[i],"-keep_circle") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: center_x center_y radius\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepCircle(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3;
    }
    else if (strcmp(argv[i],"-keep") == 0 || strcmp(argv[i],"-keep_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x min_y max_x max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepXY(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-keep_box") == 0 || strcmp(argv[i],"-keep_xyz") == 0)
    {
      if ((i+6) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 6 arguments: min_x min_y min_z max_x max_y max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepBox(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4]), atof(argv[i+5]), atof(argv[i+6])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; *argv[i+5]='\0'; *argv[i+6]='\0'; i+=6; 
    }
    else if (strcmp(argv[i],"-keep_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_z max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepZ(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-keep_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropXBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropYBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropZBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropXAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropYAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropZAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
*/
    else if (strcmp(argv[i],"-keep_first") == 0 || strcmp(argv[i],"-keep_first_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x min_y max_x max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepFirstXY(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-keep_first_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_z max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepFirstZ(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-drop_first_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropFirstXBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_first_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropFirstYBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_first_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropFirstZBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_first_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropFirstXAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_first_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropFirstYAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_first_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropFirstZAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-keep_last") == 0 || strcmp(argv[i],"-keep_last_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x min_y max_x max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepLastXY(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-drop_last_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_z max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepLastZ(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-drop_last_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropLastXBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_last_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropLastYBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_last_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropLastZBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_last_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropLastXAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_last_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropLastYAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_last_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropLastZAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-keep_anchor") == 0 || strcmp(argv[i],"-keep_anchor_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_x min_y max_x max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepAnchorXY(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]), atof(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-keep_anchor_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_z max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepAnchorZ(atof(argv[i+1]), atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-drop_anchor_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorXBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorYBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorZBelow(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorXAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorYAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorZAbove(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-keep_anchor_raw") == 0 || strcmp(argv[i],"-keep_anchor_raw_xy") == 0)
    {
      if ((i+4) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 4 arguments: min_raw_x min_raw_y max_raw_x max_raw_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepAnchorRawXY(atoi(argv[i+1]), atoi(argv[i+2]), atoi(argv[i+3]), atoi(argv[i+4])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; *argv[i+4]='\0'; i+=4; 
    }
    else if (strcmp(argv[i],"-keep_anchor_raw_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min_raw_z max_raw_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionKeepAnchorRawZ(atoi(argv[i+1]), atoi(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-drop_anchor_raw_x_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_raw_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorRawXBelow(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_raw_y_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_raw_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorRawYBelow(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_raw_z_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: min_raw_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorRawZBelow(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_raw_x_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_raw_x\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorRawXAbove(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_raw_y_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_raw_y\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorRawYAbove(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_anchor_raw_z_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_raw_z\n", argv[i]);
        return FALSE;
      }
      add_criterion(new PULSEcriterionDropAnchorRawZAbove(atoi(argv[i+1])));
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
    else if (strcmp(argv[i],"-keep_T") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min max\n", argv[i]);
        return FALSE;
      }
#ifdef _WIN32
      add_criterion(new PULSEcriterionKeepT(_atoi64(argv[i+1]), _atoi64(argv[i+2])));
#else
      add_criterion(new PULSEcriterionKeepT(atoill(argv[i+1]), atoill(argv[i+2])));
#endif
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
    }
    else if (strcmp(argv[i],"-drop_T_below") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_T\n", argv[i]);
        return FALSE;
      }
#ifdef _WIN32
      add_criterion(new PULSEcriterionDropTbelow(_atoi64(argv[i+1])));
#else
      add_criterion(new PULSEcriterionDropTbelow(atoll(argv[i+1])));
#endif
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_T_above") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: max_T\n", argv[i]);
        return FALSE;
      }
#ifdef _WIN32
      add_criterion(new PULSEcriterionDropTabove(_atoi64(argv[i+1])));
#else
      add_criterion(new PULSEcriterionDropTabove(atoll(argv[i+1])));
#endif
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
    else if (strcmp(argv[i],"-drop_T_between") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: min max\n", argv[i]);
        return FALSE;
      }
#ifdef _WIN32
      add_criterion(new PULSEcriterionDropTbetween(_atoi64(argv[i+1]), _atoi64(argv[i+2])));
#else
      add_criterion(new PULSEcriterionDropTbetween(atoill(argv[i+1]), atoill(argv[i+2])));
#endif
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2;
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
/*
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
*/
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

void PULSEfilter::addKeepCircle(F64 x, F64 y, F64 radius)
{
//  add_criterion(new PULSEcriterionKeepCircle(x, y, radius));
}

void PULSEfilter::addKeepBox(F64 min_x, F64 min_y, F64 min_z, F64 max_x, F64 max_y, F64 max_z)
{
//  add_criterion(new PULSEcriterionKeepBox(min_x, min_y, min_z, max_x, max_y, max_z));
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
