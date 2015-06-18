/*
===============================================================================

  FILE:  pulsetransform.cpp
  
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
#include "pulsetransform.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

class PULSEoperationTranslateX : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_x"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), offset_x); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_anchor_x(pulse->compute_and_get_anchor_x() + offset_x);
    pulse->set_target_x(pulse->compute_and_get_target_x() + offset_x);
  };
  PULSEoperationTranslateX(F64 offset_x) { this->offset_x = offset_x; };
private:
  F64 offset_x;
};

class PULSEoperationTranslateY : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_y"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), offset_y); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_anchor_y(pulse->compute_and_get_anchor_y() + offset_y);
    pulse->set_target_y(pulse->compute_and_get_target_y() + offset_y);
  };
  PULSEoperationTranslateY(F64 offset_y) { this->offset_y = offset_y; };
private:
  F64 offset_y;
};

class PULSEoperationTranslateZ : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), offset_z); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_anchor_z(pulse->compute_and_get_anchor_z() + offset_z);
    pulse->set_target_z(pulse->compute_and_get_target_z() + offset_z);
  };
  PULSEoperationTranslateZ(F64 offset_z) { this->offset_z = offset_z; };
private:
  F64 offset_z;
};

class PULSEoperationTranslateXYZ : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_xyz"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g ", name(), offset[0], offset[1], offset[2]); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_anchor_x(pulse->compute_and_get_anchor_x() + offset[0]);
    pulse->set_anchor_y(pulse->compute_and_get_anchor_y() + offset[1]);
    pulse->set_anchor_z(pulse->compute_and_get_anchor_z() + offset[2]);
    pulse->set_target_x(pulse->compute_and_get_target_x() + offset[0]);
    pulse->set_target_y(pulse->compute_and_get_target_y() + offset[1]);
    pulse->set_target_z(pulse->compute_and_get_target_z() + offset[2]);
  };
  PULSEoperationTranslateXYZ(F64 offset_x, F64 offset_y, F64 offset_z) { this->offset[0] = offset_x; this->offset[1] = offset_y; this->offset[2] = offset_z; };
private:
  F64 offset[3];
};

class PULSEoperationSwitchXY : public PULSEoperation
{
public:
  inline const char* name() const { return "switch_x_y"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s ", name()); };
  inline void transform(PULSEpulse* pulse) const { I32 temp = pulse->anchor_X; pulse->anchor_X = pulse->anchor_Y; pulse->anchor_Y = temp; temp = pulse->target_X; pulse->target_X = pulse->target_Y; pulse->target_Y = temp; };
};

class PULSEoperationSwitchXZ : public PULSEoperation
{
public:
  inline const char* name() const { return "switch_x_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s ", name()); };
  inline void transform(PULSEpulse* pulse) const { I32 temp = pulse->anchor_X; pulse->anchor_X = pulse->anchor_Z; pulse->anchor_Z = temp; temp = pulse->target_X; pulse->target_X = pulse->target_Z; pulse->target_Z = temp; };
};

class PULSEoperationSwitchYZ : public PULSEoperation
{
public:
  inline const char* name() const { return "switch_y_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s ", name()); };
  inline void transform(PULSEpulse* pulse) const { I32 temp = pulse->anchor_Y; pulse->anchor_Y = pulse->anchor_Z; pulse->anchor_Z = temp; temp = pulse->target_Y; pulse->target_Y = pulse->target_Z; pulse->target_Z = temp; };
};

/*
class PULSEoperationScaleX : public PULSEoperation
{
public:
  inline const char* name() const { return "scale_x"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), scale); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_x(pulse->get_x() * scale);
  };
  PULSEoperationScaleX(F64 scale) { this->scale = scale; };
private:
  F64 scale;
};

class PULSEoperationScaleY : public PULSEoperation
{
public:
  inline const char* name() const { return "scale_y"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), scale); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_y(pulse->get_y() * scale);
  };
  PULSEoperationScaleY(F64 scale) { this->scale = scale; };
private:
  F64 scale;
};

class PULSEoperationScaleZ : public PULSEoperation
{
public:
  inline const char* name() const { return "scale_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), scale); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_z(pulse->get_z() * scale);
  };
  PULSEoperationScaleZ(F64 scale) { this->scale = scale; };
private:
  F64 scale;
};

class PULSEoperationScaleXYZ : public PULSEoperation
{
public:
  inline const char* name() const { return "scale_xyz"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g ", name(), scale[0], scale[1], scale[2]); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_x(pulse->get_x() * scale[0]);
    pulse->set_y(pulse->get_y() * scale[1]);
    pulse->set_z(pulse->get_z() * scale[2]);
  };
  PULSEoperationScaleXYZ(F64 x_scale, F64 y_scale, F64 z_scale) { this->scale[0] = x_scale; this->scale[1] = y_scale; this->scale[2] = z_scale; };
private:
  F64 scale[3];
};

class PULSEoperationTranslateThenScaleX : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_then_scale_x"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), offset, scale); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_x((pulse->get_x()+offset)*scale);
  };
  PULSEoperationTranslateThenScaleX(F64 offset, F64 scale_factor) { this->offset = offset; this->scale = scale; };
private:
  F64 offset;
  F64 scale;
};

class PULSEoperationTranslateThenScaleY : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_then_scale_y"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), offset, scale); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_y((pulse->get_y()+offset)*scale);
  };
  PULSEoperationTranslateThenScaleY(F64 offset, F64 scale) { this->offset = offset; this->scale = scale; };
private:
  F64 offset;
  F64 scale;
};

class PULSEoperationTranslateThenScaleZ : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_then_scale_z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), offset, scale); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->set_z((pulse->get_z()+offset)*scale);
  };
  PULSEoperationTranslateThenScaleZ(F64 offset, F64 scale) { this->offset = offset; this->scale = scale; };
private:
  F64 offset;
  F64 scale;
};

class PULSEoperationRotateXY : public PULSEoperation
{
public:
  inline const char* name() const { return "rotate_xy"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g %g ", name(), angle, x_offset, y_offset); };
  inline void transform(PULSEpulse* pulse) const {
    F64 x = pulse->get_x() - x_offset;
    F64 y = pulse->get_y() - y_offset;
    pulse->set_x(cos_angle*x - sin_angle*y + x_offset);
    pulse->set_y(cos_angle*y + sin_angle*x + y_offset);
  };
  PULSEoperationRotateXY(F64 angle, F64 x_offset, F64 y_offset) { this->angle = angle; this->x_offset = x_offset; this->y_offset = y_offset; cos_angle = cos(3.141592653589793238462643383279502884197169/180*angle); sin_angle = sin(3.141592653589793238462643383279502884197169/180*angle); };
private:
  F64 angle;
  F64 x_offset, y_offset;
  F64 cos_angle, sin_angle;
};
*/

class PULSEoperationTranslateRawX : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_X"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), offset_X); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->anchor_X += offset_X;
    pulse->target_X += offset_X;
  };
  PULSEoperationTranslateRawX(I32 offset_X) { this->offset_X = offset_X; };
private:
  I32 offset_X;
};

class PULSEoperationTranslateRawY : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_Y"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), offset_Y); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->anchor_Y += offset_Y;
    pulse->target_Y += offset_Y;
  };
  PULSEoperationTranslateRawY(I32 offset_Y) { this->offset_Y = offset_Y; };
private:
  I32 offset_Y;
};

class PULSEoperationTranslateRawZ : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_Z"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), offset_Z); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->anchor_Z += offset_Z;
    pulse->target_Z += offset_Z;
  };
  PULSEoperationTranslateRawZ(I32 offset_Z) { this->offset_Z = offset_Z; };
private:
  I32 offset_Z;
};

class PULSEoperationTranslateRawXYZ : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_XYZ"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d %d ", name(), offset[0], offset[1], offset[2]); };
  inline void transform(PULSEpulse* pulse) const {
    pulse->anchor_X += offset[0];
    pulse->anchor_Y += offset[1];
    pulse->anchor_Z += offset[2];
    pulse->target_X += offset[0];
    pulse->target_Y += offset[1];
    pulse->target_Z += offset[2];
  };
  PULSEoperationTranslateRawXYZ(I32 offset_X, I32 offset_Y, I32 offset_Z) { this->offset[0] = offset_X; this->offset[1] = offset_Y; this->offset[2] = offset_Z; };
private:
  I32 offset[3];
};

class PULSEoperationScaleIntensity : public PULSEoperation
{
public:
  inline const char* name() const { return "scale_intensity"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), scale); };
  inline void transform(PULSEpulse* pulse) const {
    F32 intensity = scale*pulse->intensity;
    pulse->intensity = U8_CLAMP(I32_QUANTIZE(intensity));
  };
  PULSEoperationScaleIntensity(F32 scale) { this->scale = scale; };
private:
  F32 scale;
};

class PULSEoperationTranslateIntensity : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_intensity"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), offset); };
  inline void transform(PULSEpulse* pulse) const {
    F32 intensity = offset+pulse->intensity;
    pulse->intensity = U8_CLAMP(I32_QUANTIZE(intensity));
  };
  PULSEoperationTranslateIntensity(F32 offset) { this->offset = offset; };
private:
  F32 offset;
};

class PULSEoperationTranslateThenScaleIntensity : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_then_scale_intensity"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g %g ", name(), offset, scale); };
  inline void transform(PULSEpulse* pulse) const {
    F32 intensity = (offset+pulse->intensity)*scale;
    pulse->intensity = U8_CLAMP(I32_QUANTIZE(intensity));
  };
  PULSEoperationTranslateThenScaleIntensity(F32 offset, F32 scale) { this->offset = offset; this->scale = scale; };
private:
  F32 offset;
  F32 scale;
};

class PULSEoperationSetMirrorFacet : public PULSEoperation
{
public:
  inline const char* name() const { return "set_mirror_facet"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), facet); };
  inline void transform(PULSEpulse* pulse) const { pulse->mirror_facet = facet; };
  PULSEoperationSetMirrorFacet(U8 facet) { this->facet = (facet & 3); };
private:
  U8 facet;
};

class PULSEoperationChangeMirrorFacetFromTo : public PULSEoperation
{
public:
  inline const char* name() const { return "change_mirror_facet_from_to"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d ", name(), facet_from, facet_to); };
  inline void transform(PULSEpulse* pulse) const { if (pulse->mirror_facet == facet_from) pulse->mirror_facet = facet_to; };
  PULSEoperationChangeMirrorFacetFromTo(U8 facet_from, U8 facet_to) { this->facet_from = (facet_from & 3); this->facet_to = (facet_to & 3); };
private:
  U8 facet_from;
  U8 facet_to;
};

class PULSEoperationSetClassification : public PULSEoperation
{
public:
  inline const char* name() const { return "set_classification"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), classification); };
  inline void transform(PULSEpulse* pulse) const { pulse->classification = classification; };
  PULSEoperationSetClassification(U8 classification) { this->classification = classification; };
private:
  U8 classification;
};

class PULSEoperationChangeClassificationFromTo : public PULSEoperation
{
public:
  inline const char* name() const { return "change_classification_from_to"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d ", name(), class_from, class_to); };
  inline void transform(PULSEpulse* pulse) const { if (pulse->classification == class_from) pulse->classification = class_to; };
  PULSEoperationChangeClassificationFromTo(U8 class_from, U8 class_to) { this->class_from = class_from; this->class_to = class_to; };
private:
  U8 class_from;
  U8 class_to;
};

class PULSEoperationSetPointSource : public PULSEoperation
{
public:
  inline const char* name() const { return "set_pulse_source_ID"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), psid); };
  inline void transform(PULSEpulse* pulse) const { pulse->pulse_source_ID = psid; };
  PULSEoperationSetPointSource(U16 psid) { this->psid = psid; };
private:
  U16 psid;
};

class PULSEoperationChangePointSourceIDFromTo : public PULSEoperation
{
public:
  inline const char* name() const { return "change_pulse_source_ID_from_to"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d %d ", name(), psid_from, psid_to); };
  inline void transform(PULSEpulse* pulse) const { if (pulse->pulse_source_ID == psid_from) pulse->pulse_source_ID = psid_to; };
  PULSEoperationChangePointSourceIDFromTo(U16 psid_from, U16 psid_to) { this->psid_from = psid_from; this->psid_to = psid_to; };
private:
  U16 psid_from;
  U16 psid_to;
};

class PULSEoperationTranslateGpsTime : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_time"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %g ", name(), offset); };
  inline void transform(PULSEpulse* pulse) const { pulse->set_T(pulse->get_t() + offset); };
  PULSEoperationTranslateGpsTime(F64 offset) { this->offset = offset; };
private:
  F64 offset;
};

class PULSEoperationTranslateT : public PULSEoperation
{
public:
  inline const char* name() const { return "translate_T"; };
#ifdef _WIN32
  inline int get_command(char* string) const { return sprintf(string, "-%s %I64d ", name(), offset); };
#else
  inline int get_command(char* string) const { return sprintf(string, "-%s %lld ", name(), offset); };
#endif
  inline void transform(PULSEpulse* pulse) const { pulse->set_T(pulse->get_T() + offset); };
  PULSEoperationTranslateT(I64 offset) { this->offset = offset; };
private:
  I64 offset;
};

/*
class PULSEoperationConvertAdjustedGpsToWeek : public PULSEoperation
{
public:
  inline const char* name() const { return "adjusted_to_week"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s ", name()); };
  inline void transform(PULSEpulse* pulse) const
  {
    I32 week = (I32)(pulse->gps_time/604800.0 + 1653.4391534391534391534391534392);
    I32 secs = week*604800 - 1000000000;
    pulse->gps_time -= secs;
  };
  PULSEoperationConvertAdjustedGpsToWeek(){};
};

class PULSEoperationConvertWeekToAdjustedGps : public PULSEoperation
{
public:
  inline const char* name() const { return "week_to_adjusted"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s %d ", name(), week); };
  inline void transform(PULSEpulse* pulse) const { pulse->gps_time += delta_secs; }
  PULSEoperationConvertWeekToAdjustedGps(I32 week) { this->week = week; delta_secs = week; delta_secs *= 604800; delta_secs -= 1000000000; };
private:
  I32 week;
  I64 delta_secs;
};

class PULSEoperationScaleRGBdown : public PULSEoperation
{
public:
  inline const char* name() const { return "scale_rgb_down"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s ", name()); };
  inline void transform(PULSEpulse* pulse) const { pulse->rgb[0] = pulse->rgb[0]/256; pulse->rgb[1] = pulse->rgb[1]/256; pulse->rgb[2] = pulse->rgb[2]/256; };
};

class PULSEoperationScaleRGBup : public PULSEoperation
{
public:
  inline const char* name() const { return "scale_rgb_up"; };
  inline int get_command(char* string) const { return sprintf(string, "-%s ", name()); };
  inline void transform(PULSEpulse* pulse) const { pulse->rgb[0] = pulse->rgb[0]*256; pulse->rgb[1] = pulse->rgb[1]*256; pulse->rgb[2] = pulse->rgb[2]*256; };
};
*/

void PULSEtransform::clean()
{
  U32 i;
  for (i = 0; i < num_operations; i++)
  {
    delete operations[i];
  }
  if (operations) delete [] operations;
  change_coordinates = FALSE;
  alloc_operations = 0;
  num_operations = 0;
  operations = 0;
}

void PULSEtransform::usage() const
{
  fprintf(stderr,"Transform coordinates.\n");
  fprintf(stderr,"  -translate_x -2.5\n");
  fprintf(stderr,"  -translate_z 42.1\n");
  fprintf(stderr,"  -translate_xyz 0.5 0.5 0.01\n");
  fprintf(stderr,"  -switch_x_y\n");
  fprintf(stderr,"  -switch_y_z\n");
  fprintf(stderr,"  -switch_x_z\n");
//  fprintf(stderr,"  -scale_z 0.3048\n");
//  fprintf(stderr,"  -rotate_xy 15.0 620000 4100000 (angle + origin)\n");
//  fprintf(stderr,"  -translate_then_scale_y -0.5 1.001\n");
  fprintf(stderr,"Transform raw XYZ integers.\n");
  fprintf(stderr,"  -translate_Z 20\n");
  fprintf(stderr,"  -translate_XYZ 2 2 0\n");
  fprintf(stderr,"Transform intensity.\n");
  fprintf(stderr,"  -scale_intensity 2.5\n");
  fprintf(stderr,"  -translate_intensity 50\n");
  fprintf(stderr,"  -translate_then_scale_intensity 0.5 3.1\n");
  fprintf(stderr,"Modify the classification.\n");
  fprintf(stderr,"  -set_classification 2\n");
  fprintf(stderr,"  -change_classification_from_to 2 4\n");
  fprintf(stderr,"Modify the pulse source ID.\n");
  fprintf(stderr,"  -set_pulse_source_ID 500\n");
  fprintf(stderr,"  -change_pulse_source_ID_from_to 1023 1024\n");
  fprintf(stderr,"Transform the time stamps.\n");
  fprintf(stderr,"  -translate_T 40500000\n");
  fprintf(stderr,"  -translate_time 40.50\n");
  fprintf(stderr,"Transform flags and else.\n");
  fprintf(stderr,"  -set_mirror_facet 0\n");
  fprintf(stderr,"  -change_mirror_facet_from_to 1 2\n");
  
/*
  fprintf(stderr,"  -adjusted_to_week\n");
  fprintf(stderr,"  -week_to_adjusted 1671\n");
*/
}

BOOL PULSEtransform::parse(int argc, char* argv[])
{
  int i;

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
    else if (strcmp(argv[i],"-translate_x") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset_x\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateX((F64)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-translate_y") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset_y\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateY((F64)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-translate_z") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset_z\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateZ((F64)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-translate_xyz") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: offset_x offset_y offset_z\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateXYZ((F64)atof(argv[i+1]), (F64)atof(argv[i+2]), (F64)atof(argv[i+3])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3; 
    }
    else if (strcmp(argv[i],"-switch_x_y") == 0)
    {
      add_operation(new PULSEoperationSwitchXY());
      *argv[i]='\0'; 
    }
    else if (strcmp(argv[i],"-switch_x_z") == 0)
    {
      add_operation(new PULSEoperationSwitchXZ());
      *argv[i]='\0'; 
    }
    else if (strcmp(argv[i],"-switch_y_z") == 0)
    {
      add_operation(new PULSEoperationSwitchYZ());
      *argv[i]='\0'; 
    }
/*
    else if (strcmp(argv[i],"-scale_x") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: scale\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationScaleX((F64)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-scale_y") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: scale\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationScaleY((F64)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-scale_z") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: scale\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationScaleZ((F64)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-scale_xyz") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: scale_x scale_y scale_z\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationScaleXYZ((F64)atof(argv[i+1]), (F64)atof(argv[i+2]), (F64)atof(argv[i+3])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3; 
    }
    else if (strcmp(argv[i],"-translate_then_scale_x") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: offset scale\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateThenScaleX((F64)atof(argv[i+1]), (F64)atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-translate_then_scale_y") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: offset scale\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateThenScaleY((F64)atof(argv[i+1]), (F64)atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-translate_then_scale_z") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: offset scale\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateThenScaleZ((F64)atof(argv[i+1]), (F64)atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-rotate_xy") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: angle, x, y\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationRotateXY((F64)atof(argv[i+1]), (F64)atof(argv[i+2]), (F64)atof(argv[i+3])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3; 
    }
*/
    else if (strcmp(argv[i],"-translate_X") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset_X\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateRawX((I32)atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-translate_Y") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset_Y\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateRawY((I32)atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-translate_Z") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset_Z\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateRawZ((I32)atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-translate_XYZ") == 0)
    {
      if ((i+3) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 3 arguments: offset_X offset_Y offset_Z\n", argv[i]);
        return FALSE;
      }
      change_coordinates = TRUE;
      add_operation(new PULSEoperationTranslateRawXYZ((I32)atoi(argv[i+1]), (I32)atoi(argv[i+2]), (I32)atoi(argv[i+3])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; *argv[i+3]='\0'; i+=3; 
    }
    else if (strcmp(argv[i],"-scale_intensity") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: scale\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationScaleIntensity((F32)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-translate_intensity") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationTranslateIntensity((F32)atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-translate_then_scale_intensity") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: offset scale\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationTranslateThenScaleIntensity((F32)atof(argv[i+1]), (F32)atof(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-set_mirror_facet") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: facet\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationSetMirrorFacet((U8)atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-change_mirror_facet_from_to") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: from_facet to_facet\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationChangeMirrorFacetFromTo((U8)atoi(argv[i+1]), (U8)atoi(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-set_classification") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: classification\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationSetClassification((U8)atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-change_classification_from_to") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: from_class to_class\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationChangeClassificationFromTo((U8)atoi(argv[i+1]), (U8)atoi(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-set_pulse_source_ID") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' need 1 arguments: psid\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationSetPointSource((U16)atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-change_pulse_source_ID_from_to") == 0)
    {
      if ((i+2) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 2 arguments: from_psid to_psid\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationChangePointSourceIDFromTo((U16)atoi(argv[i+1]), (U16)atoi(argv[i+2])));
      *argv[i]='\0'; *argv[i+1]='\0'; *argv[i+2]='\0'; i+=2; 
    }
    else if (strcmp(argv[i],"-translate_T") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset\n", argv[i]);
        return FALSE;
      }
#ifdef _WIN32
      add_operation(new PULSEoperationTranslateT(_atoi64(argv[i+1])));
#else
      add_operation(new PULSEoperationTranslateT(atoill(argv[i+1])));
#endif
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-translate_time") == 0 || strcmp(argv[i],"-translate_gps_time") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: offset\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationTranslateGpsTime(atof(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
/*
    else if (strcmp(argv[i],"-adjusted_to_week") == 0)
    {
      add_operation(new PULSEoperationConvertAdjustedGpsToWeek());
      *argv[i]='\0'; 
    }
    else if (strcmp(argv[i],"-week_to_adjusted") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: week\n", argv[i]);
        return FALSE;
      }
      add_operation(new PULSEoperationConvertWeekToAdjustedGps(atoi(argv[i+1])));
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1; 
    }
    else if (strcmp(argv[i],"-scale_rgb_down") == 0 || strcmp(argv[i],"-scale_rbg_down") == 0)
    {
      add_operation(new PULSEoperationScaleRGBdown());
      *argv[i]='\0'; 
    }
    else if (strcmp(argv[i],"-scale_rgb_up") == 0 || strcmp(argv[i],"-scale_rbg_up") == 0)
    {
      add_operation(new PULSEoperationScaleRGBup());
      *argv[i]='\0'; 
    }
  */
  }
  return TRUE;
}

I32 PULSEtransform::unparse(char* string) const
{
  U32 i;
  I32 n = 0;
  for (i = 0; i < num_operations; i++)
  {
    n += operations[i]->get_command(&string[n]);
  }
  return n;
}

void PULSEtransform::transform(PULSEpulse* pulse) const
{
  U32 i;
  for (i = 0; i < num_operations; i++) operations[i]->transform(pulse);
}

PULSEtransform::PULSEtransform()
{
  change_coordinates = FALSE;
  alloc_operations = 0;
  num_operations = 0;
  operations = 0;
}

PULSEtransform::~PULSEtransform()
{
  if (operations) clean();
}

void PULSEtransform::add_operation(PULSEoperation* transform_operation)
{
  if (num_operations == alloc_operations)
  {
    U32 i;
    alloc_operations += 16;
    PULSEoperation** temp_operations = new PULSEoperation*[alloc_operations];
    if (operations)
    {
      for (i = 0; i < num_operations; i++)
      {
        temp_operations[i] = operations[i];
      }
      delete [] operations;
    }
    operations = temp_operations;
  }
  operations[num_operations] = transform_operation;
  num_operations++;
}
