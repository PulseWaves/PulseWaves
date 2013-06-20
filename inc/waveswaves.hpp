/*
===============================================================================

  FILE:  waveswaves.hpp
  
  CONTENTS:
  
    Describes the waves records.

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
  
    07 March 2012 -- created while watching Bayer loose 7:1 against Barcelona
  
===============================================================================
*/
#ifndef WAVES_WAVES_HPP
#define WAVES_WAVES_HPP

#include "mydefs.hpp"

#include <stdio.h>

class ByteStreamIn;
class ByteStreamOut;
class PULSEpulse;
class PULSEsampling;
class PULSEdescriptor;

class WAVESheader
{
public:
  CHAR file_signature[16];
  U32 compression;
  CHAR unused[44];

  BOOL load(ByteStreamIn* stream);
  BOOL save(ByteStreamOut* stream) const;

  BOOL check() const;

  WAVESheader();
};

class WAVESsampling
{
public:

  // access parameters copied or derived from the pulse descriptor
  inline U8 get_type() const { return type; };
  inline U8 get_channel() const { return channel; };
  inline virtual U8 get_bits_for_duration_from_anchor() const { return bits_for_duration_from_anchor; };
  inline virtual F32 get_scale_for_duration_from_anchor() const { return scale_for_duration_from_anchor; };
  inline virtual F32 get_offset_for_duration_from_anchor() const { return offset_for_duration_from_anchor; };
  inline virtual U8 get_bits_for_number_of_segments() const { return (U8)bits_for_number_of_segments; };
  inline virtual U8 get_bits_for_number_of_samples() const { return (U8)bits_for_number_of_samples; };
  inline virtual I32 get_number_of_segments() const { return number_of_segments; };
  inline virtual I32 get_number_of_samples() const { return number_of_samples; };
  inline virtual I32 get_bits_per_sample() const { return bits_per_sample; };
  inline virtual I32 get_bytes_per_sample() const { return ((get_bits_per_sample()%8) ? 1 + (get_bits_per_sample() / 8) : (get_bits_per_sample() / 8)); };

  inline virtual BOOL set_bits_for_number_of_segments(U8 bits_for_number_of_segments) { this->bits_for_number_of_segments = bits_for_number_of_segments; return TRUE; };
  inline virtual BOOL set_bits_for_duration_from_anchor(U8 bits_for_duration_from_anchor) { this->bits_for_duration_from_anchor = bits_for_duration_from_anchor; return TRUE; };
  inline virtual BOOL set_number_of_segments(I32 number_of_segments) = 0;

  // controls on which segment the functions below operate
  virtual BOOL set_active_segment(U16 segment_idx) = 0;
  virtual U16 get_active_segment() const = 0;

  // set or get parameters that read from or write to the waves
  virtual BOOL set_duration_from_anchor_for_segment(F32 duration) = 0;
  virtual F32 get_duration_from_anchor_for_segment() const = 0;
  virtual BOOL set_quantized_duration_from_anchor_for_segment(I32 quantized_duration) = 0;
  virtual I32 get_quantized_duration_from_anchor_for_segment() const = 0;
  virtual BOOL set_number_of_samples_for_segment(I32 num_samples) = 0;
  virtual I32 get_number_of_samples_for_segment() const = 0;

  // access memory and size for reading and writing the waves 
  virtual U32 size() const = 0;
  virtual U8* get_samples() const = 0;

  // compute value and xyz location for an individual sample
  virtual I32 get_sample(I32 sample_idx) = 0;
  virtual BOOL get_sample_xyz(const PULSEpulse* pulse, I32 sample_idx) = 0;

  // access computed value and computed xyz location for an individual sample
  inline virtual I32 get_sample() { return sample; };
  inline virtual const F64* get_xyz() { return (const F64*)xyz; };

  // read an entire array of sample values or sample xyz locations
//  virtual I32 get_samples(U8* values) const {};
//  virtual I32 get_samples_xyz(U8* values, const PULSEpulse* pulse) const {};

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  void clean();
  WAVESsampling();
  virtual ~WAVESsampling();

protected:
  F64 xyz[3];
  I32 sample;

  // parameters copied from the pulse descriptor
  U8 type;                                     // 0 - undefined, 1 - outgoing, 2 - returning
  U8 channel;
  U8 unused;                                   // must be zero
  U8 bits_for_duration_from_anchor;            // 0, 8, 16, or 32
  F32 scale_for_duration_from_anchor;          // default is 1.0f
  F32 offset_for_duration_from_anchor;         // default is 0.0f
  U8 bits_for_number_of_segments;              // 0 or 8 or 16
  U8 bits_for_number_of_samples;               // 0 or 8 or 16
  U16 number_of_segments;
  U32 number_of_samples;
  U16 bits_per_sample;                         // 8 or 16
  U16 lookup_table_index;                      // must be zero (for now)
  F32 sample_units;                            // [nanoseconds]
  U32 compression;                             // must be zero

  inline BOOL fits_in_bits(I32 number, U8 bits) { return (bits == 32 ? TRUE : (bits == 16 ? I16_FITS_IN_RANGE(number) : (bits == 8 ? I8_FITS_IN_RANGE(number) : FALSE))); }; 
};

// ----------------- specialized for one segment ------------------

class WAVESsamplingOneSegment : public virtual WAVESsampling
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_segments() const { return 0; };
  inline I32 get_number_of_segments() const { return 1; };
  inline I32 get_number_of_samples() const { return num_samples; };

  inline BOOL set_bits_for_number_of_segments(U8 bits_for_number_of_segments) { if (bits_for_number_of_segments != 0) return FALSE;  return TRUE; };
  inline BOOL set_number_of_segments(I32 number_of_segments) { if (number_of_segments != 1) return FALSE; return TRUE; };

  // controls on which segment the functions below operate
  BOOL set_active_segment(U16 segment_idx) { if (segment_idx == 0) return TRUE; return FALSE; };
  U16 get_active_segment() const { return 0; };

  // set or get parameters that read from or write to the waves
  inline BOOL set_duration_from_anchor_for_segment(F32 duration) { quantized_duration = I32_QUANTIZE((duration - offset_for_duration_from_anchor)/scale_for_duration_from_anchor); set_quantized_duration_from_anchor_for_segment(quantized_duration); return fits_in_bits(quantized_duration, bits_for_duration_from_anchor); };
  inline F32 get_duration_from_anchor_for_segment() const { return duration; };
  inline BOOL set_quantized_duration_from_anchor_for_segment(I32 quantized_duration) { this->quantized_duration = quantized_duration; duration = scale_for_duration_from_anchor*quantized_duration + offset_for_duration_from_anchor; return TRUE; };
  inline I32 get_quantized_duration_from_anchor_for_segment() const { return quantized_duration; };
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (this->num_samples != num_samples) return FALSE; return TRUE; };
  inline I32 get_number_of_samples_for_segment() const { return num_samples; };

  // access memory and size for reading and writing the waves 
  inline U8* get_samples() const { return samples; };
 
  // compute value and xyz location for an individual sample
  BOOL get_sample_xyz(const PULSEpulse* pulse, I32 sample_idx);

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  void clean();
  WAVESsamplingOneSegment();
  virtual ~WAVESsamplingOneSegment();

protected:
  U8* samples;
  I32 num_samples;
  I32 quantized_duration;
  F32 duration;
};

class WAVESsamplingOneSegment8bit : public virtual WAVESsamplingOneSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 8; };
  inline I32 get_bytes_per_sample() const { return 1; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return num_samples; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples) { sample = samples[sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingOneSegment8bit();
  virtual ~WAVESsamplingOneSegment8bit();
};

class WAVESsamplingOneSegment16bit : public virtual WAVESsamplingOneSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 16; };
  inline I32 get_bytes_per_sample() const { return 2; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return 2*num_samples; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples) { sample = ((U16*)samples)[sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingOneSegment16bit();
  virtual ~WAVESsamplingOneSegment16bit();
};


class WAVESsamplingOneSegment8bitFixed : public virtual WAVESsamplingOneSegment8bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingOneSegment8bitFixed();
  ~WAVESsamplingOneSegment8bitFixed();
};

class WAVESsamplingOneSegment16bitFixed : public virtual WAVESsamplingOneSegment16bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingOneSegment16bitFixed();
  ~WAVESsamplingOneSegment16bitFixed();
};

class WAVESsamplingOneSegment8bitVariable : public virtual WAVESsamplingOneSegment8bit
{
public:
  // set or get parameters that read from or write to the waves
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (num_samples > alloc_samples) { if (samples) { delete [] samples; } samples = new U8[num_samples]; if (samples == 0) return FALSE; alloc_samples = num_samples; } this->num_samples = num_samples; return TRUE; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingOneSegment8bitVariable();
  ~WAVESsamplingOneSegment8bitVariable();

protected:
  I32 alloc_samples;
};

class WAVESsamplingOneSegment16bitVariable : public virtual WAVESsamplingOneSegment16bit
{
public:
  // set or get parameters that read from or write to the waves
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (num_samples > alloc_samples) { if (samples) { delete [] samples; } samples = new U8[2*num_samples]; if (samples == 0) return FALSE; alloc_samples = num_samples; } this->num_samples = num_samples; return TRUE; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingOneSegment16bitVariable();
  ~WAVESsamplingOneSegment16bitVariable();

protected:
  I32 alloc_samples;
};

// ------------------- specialized for two segments ------------------

class WAVESsamplingTwoSegment : public virtual WAVESsampling
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_segments() const { return 0; };
  inline I32 get_number_of_segments() const { return 2; };
  inline I32 get_number_of_samples() const { return num_samples[active]; };

  inline BOOL set_bits_for_number_of_segments(U8 bits_for_number_of_segments) { if (bits_for_number_of_segments != 0) return FALSE;  return TRUE; };
  inline BOOL set_number_of_segments(I32 number_of_segments) { if (number_of_segments != 2) return FALSE; return TRUE; };

  // controls on which segment the functions below operate
  BOOL set_active_segment(U16 segment_idx) { if (segment_idx == 0) active = 0; else if (segment_idx == 1) active = 1; else return FALSE; return TRUE; };
  U16 get_active_segment() const { return active; };

  // set or get parameters that read from or write to the waves
  inline BOOL set_duration_from_anchor_for_segment(F32 duration) { quantized_duration[active] = I32_QUANTIZE((duration - offset_for_duration_from_anchor)/scale_for_duration_from_anchor); set_quantized_duration_from_anchor_for_segment(quantized_duration[active]); return fits_in_bits(quantized_duration[active], bits_for_duration_from_anchor); };
  inline F32 get_duration_from_anchor_for_segment() const { return duration[active]; };
  inline BOOL set_quantized_duration_from_anchor_for_segment(I32 quantized_duration) { this->quantized_duration[active] = quantized_duration; duration[active] = scale_for_duration_from_anchor*quantized_duration + offset_for_duration_from_anchor; return TRUE; };
  inline I32 get_quantized_duration_from_anchor_for_segment() const { return quantized_duration[active]; };
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (this->num_samples[active] != num_samples) return FALSE; return TRUE; };
  inline I32 get_number_of_samples_for_segment() const { return num_samples[active]; };

  // access memory and size for reading and writing the waves 
  inline U8* get_samples() const { return samples[active]; };
 
  // compute value and xyz location for an individual sample
  BOOL get_sample_xyz(const PULSEpulse* pulse, I32 sample_idx);

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  void clean();
  WAVESsamplingTwoSegment();
  virtual ~WAVESsamplingTwoSegment();

protected:
  U16 active;
  U8* samples[2];
  I32 num_samples[2];
  I32 quantized_duration[2];
  F32 duration[2];
};

class WAVESsamplingTwoSegment8bit : public virtual WAVESsamplingTwoSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 8; };
  inline I32 get_bytes_per_sample() const { return 1; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return num_samples[active]; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples[active]) { sample = samples[active][sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingTwoSegment8bit();
  virtual ~WAVESsamplingTwoSegment8bit();
};

class WAVESsamplingTwoSegment16bit : public virtual WAVESsamplingTwoSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 16; };
  inline I32 get_bytes_per_sample() const { return 2; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return 2*num_samples[active]; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples[active]) { sample = ((U16*)samples[active])[sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingTwoSegment16bit();
  virtual ~WAVESsamplingTwoSegment16bit();
};


class WAVESsamplingTwoSegment8bitFixed : public virtual WAVESsamplingTwoSegment8bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingTwoSegment8bitFixed();
  ~WAVESsamplingTwoSegment8bitFixed();
};

class WAVESsamplingTwoSegment16bitFixed : public virtual WAVESsamplingTwoSegment16bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingTwoSegment16bitFixed();
  ~WAVESsamplingTwoSegment16bitFixed();
};

class WAVESsamplingTwoSegment8bitVariable : public virtual WAVESsamplingTwoSegment8bit
{
public:
  // set or get parameters that read from or write to the waves
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (num_samples > alloc_samples[active]) { if (samples[active]) { delete [] samples[active]; } samples[active] = new U8[num_samples]; if (samples[active] == 0) return FALSE; alloc_samples[active] = num_samples; } this->num_samples[active] = num_samples; return TRUE; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingTwoSegment8bitVariable();
  ~WAVESsamplingTwoSegment8bitVariable();

protected:
  I32 alloc_samples[2];
};

class WAVESsamplingTwoSegment16bitVariable : public virtual WAVESsamplingTwoSegment16bit
{
public:
  // set or get parameters that read from or write to the waves
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (num_samples > alloc_samples[active]) { if (samples[active]) { delete [] samples[active]; } samples[active] = new U8[2*num_samples]; if (samples[active] == 0) return FALSE; alloc_samples[active] = num_samples; } this->num_samples[active] = num_samples; return TRUE; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingTwoSegment16bitVariable();
  ~WAVESsamplingTwoSegment16bitVariable();

protected:
  I32 alloc_samples[2];
};

// ------------------- specialized for three segments ------------------

class WAVESsamplingThreeSegment : public virtual WAVESsampling
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_segments() const { return 0; };
  inline I32 get_number_of_segments() const { return 2; };
  inline I32 get_number_of_samples() const { return num_samples[active]; };

  inline BOOL set_bits_for_number_of_segments(U8 bits_for_number_of_segments) { if (bits_for_number_of_segments != 0) return FALSE;  return TRUE; };
  inline BOOL set_number_of_segments(I32 number_of_segments) { if (number_of_segments != 3) return FALSE; return TRUE; };

  // controls on which segment the functions below operate
  BOOL set_active_segment(U16 segment_idx) { if (segment_idx == 0) active = 0; else if (segment_idx == 1) active = 1; else return FALSE; return TRUE; };
  U16 get_active_segment() const { return active; };

  // set or get parameters that read from or write to the waves
  inline BOOL set_duration_from_anchor_for_segment(F32 duration) { quantized_duration[active] = I32_QUANTIZE((duration - offset_for_duration_from_anchor)/scale_for_duration_from_anchor); set_quantized_duration_from_anchor_for_segment(quantized_duration[active]); return fits_in_bits(quantized_duration[active], bits_for_duration_from_anchor); };
  inline F32 get_duration_from_anchor_for_segment() const { return duration[active]; };
  inline BOOL set_quantized_duration_from_anchor_for_segment(I32 quantized_duration) { this->quantized_duration[active] = quantized_duration; duration[active] = scale_for_duration_from_anchor*quantized_duration + offset_for_duration_from_anchor; return TRUE; };
  inline I32 get_quantized_duration_from_anchor_for_segment() const { return quantized_duration[active]; };
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (this->num_samples[active] != num_samples) return FALSE; return TRUE; };
  inline I32 get_number_of_samples_for_segment() const { return num_samples[active]; };

  // access memory and size for reading and writing the waves 
  inline U8* get_samples() const { return samples[active]; };
 
  // compute value and xyz location for an individual sample
  BOOL get_sample_xyz(const PULSEpulse* pulse, I32 sample_idx);

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  void clean();
  WAVESsamplingThreeSegment();
  virtual ~WAVESsamplingThreeSegment();

protected:
  U16 active;
  U8* samples[3];
  I32 num_samples[3];
  I32 quantized_duration[3];
  F32 duration[3];
};

class WAVESsamplingThreeSegment8bit : public virtual WAVESsamplingThreeSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 8; };
  inline I32 get_bytes_per_sample() const { return 1; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return num_samples[active]; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples[active]) { sample = samples[active][sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingThreeSegment8bit();
  virtual ~WAVESsamplingThreeSegment8bit();
};

class WAVESsamplingThreeSegment16bit : public virtual WAVESsamplingThreeSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 16; };
  inline I32 get_bytes_per_sample() const { return 2; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return 2*num_samples[active]; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples[active]) { sample = ((U16*)samples[active])[sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingThreeSegment16bit();
  virtual ~WAVESsamplingThreeSegment16bit();
};


class WAVESsamplingThreeSegment8bitFixed : public virtual WAVESsamplingThreeSegment8bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingThreeSegment8bitFixed();
  ~WAVESsamplingThreeSegment8bitFixed();
};

class WAVESsamplingThreeSegment16bitFixed : public virtual WAVESsamplingThreeSegment16bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingThreeSegment16bitFixed();
  ~WAVESsamplingThreeSegment16bitFixed();
};

class WAVESsamplingThreeSegment8bitVariable : public virtual WAVESsamplingThreeSegment8bit
{
public:
  // set or get parameters that read from or write to the waves
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (num_samples > alloc_samples[active]) { if (samples[active]) { delete [] samples[active]; } samples[active] = new U8[num_samples]; if (samples[active] == 0) return FALSE; alloc_samples[active] = num_samples; } this->num_samples[active] = num_samples; return TRUE; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingThreeSegment8bitVariable();
  ~WAVESsamplingThreeSegment8bitVariable();

protected:
  I32 alloc_samples[3];
};

class WAVESsamplingThreeSegment16bitVariable : public virtual WAVESsamplingThreeSegment16bit
{
public:
  // set or get parameters that read from or write to the waves
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (num_samples > alloc_samples[active]) { if (samples[active]) { delete [] samples[active]; } samples[active] = new U8[2*num_samples]; if (samples[active] == 0) return FALSE; alloc_samples[active] = num_samples; } this->num_samples[active] = num_samples; return TRUE; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingThreeSegment16bitVariable();
  ~WAVESsamplingThreeSegment16bitVariable();

protected:
  I32 alloc_samples[3];
};

// ----------- specialized for fixed number of multiple segments ----------------

class WAVESsamplingFixedMultiSegment : public virtual WAVESsampling
{
public:
  // override access parameters copied or derived from the pulse descriptor
  virtual inline U8 get_bits_for_number_of_segments() const { return bits_for_number_of_segments; };
  inline I32 get_number_of_segments() const { return number_of_segments; };
  inline I32 get_number_of_samples() const { return num_samples[active]; };

  virtual inline BOOL set_bits_for_number_of_segments(U8 bits_for_number_of_segments) { if (bits_for_number_of_segments != 0) return FALSE;  return TRUE; };
  virtual inline BOOL set_number_of_segments(I32 number_of_segments) { if (number_of_segments != this->number_of_segments) return FALSE; return TRUE; };

  // controls on which segment the functions below operate
  BOOL set_active_segment(U16 segment_idx) { if (segment_idx >= number_of_segments) return FALSE; active = segment_idx; return TRUE; };
  U16 get_active_segment() const { return active; };

  // set or get parameters that read from or write to the waves
  inline BOOL set_duration_from_anchor_for_segment(F32 duration) { quantized_duration[active] = I32_QUANTIZE((duration - offset_for_duration_from_anchor)/scale_for_duration_from_anchor); set_quantized_duration_from_anchor_for_segment(quantized_duration[active]); return fits_in_bits(quantized_duration[active], bits_for_duration_from_anchor); };
  inline F32 get_duration_from_anchor_for_segment() const { return duration[active]; };
  inline BOOL set_quantized_duration_from_anchor_for_segment(I32 quantized_duration) { this->quantized_duration[active] = quantized_duration; duration[active] = scale_for_duration_from_anchor*quantized_duration + offset_for_duration_from_anchor; return TRUE; };
  inline I32 get_quantized_duration_from_anchor_for_segment() const { return quantized_duration[active]; };
  inline virtual BOOL set_number_of_samples_for_segment(I32 num_samples) { if (this->num_samples[active] != num_samples) return FALSE; return TRUE; };
  inline I32 get_number_of_samples_for_segment() const { return num_samples[active]; };

  // access memory and size for reading and writing the waves 
  inline U8* get_samples() const { return samples[active]; };
 
  // compute value and xyz location for an individual sample
  BOOL get_sample_xyz(const PULSEpulse* pulse, I32 sample_idx);

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  void clean();
  WAVESsamplingFixedMultiSegment();
  virtual ~WAVESsamplingFixedMultiSegment();

protected:
  U16 active;
  U16 alloc_segments;
  U8** samples;
  I32* num_samples;
  I32* alloc_samples;
  I32* quantized_duration;
  F32* duration;
};

class WAVESsamplingFixedMultiSegment8bit : public virtual WAVESsamplingFixedMultiSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 8; };
  inline I32 get_bytes_per_sample() const { return 1; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return num_samples[active]; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples[active]) { sample = samples[active][sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingFixedMultiSegment8bit();
  virtual ~WAVESsamplingFixedMultiSegment8bit();
};

class WAVESsamplingFixedMultiSegment16bit : public virtual WAVESsamplingFixedMultiSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 16; };
  inline I32 get_bytes_per_sample() const { return 2; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return 2*num_samples[active]; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples[active]) { sample = ((U16*)samples[active])[sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingFixedMultiSegment16bit();
  virtual ~WAVESsamplingFixedMultiSegment16bit();
};


class WAVESsamplingFixedMultiSegment8bitFixed : public virtual WAVESsamplingFixedMultiSegment8bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingFixedMultiSegment8bitFixed();
  ~WAVESsamplingFixedMultiSegment8bitFixed();
};

class WAVESsamplingFixedMultiSegment16bitFixed : public virtual WAVESsamplingFixedMultiSegment16bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingFixedMultiSegment16bitFixed();
  ~WAVESsamplingFixedMultiSegment16bitFixed();
};

class WAVESsamplingFixedMultiSegment8bitVariable : public virtual WAVESsamplingFixedMultiSegment8bit
{
public:
  // set or get parameters that read from or write to the waves
  BOOL set_number_of_samples_for_segment(I32 num_samples);

  // housekeeping
  WAVESsamplingFixedMultiSegment8bitVariable();
  ~WAVESsamplingFixedMultiSegment8bitVariable();
};

class WAVESsamplingFixedMultiSegment16bitVariable : public virtual WAVESsamplingFixedMultiSegment16bit
{
public:
  // set or get parameters that read from or write to the waves
  BOOL set_number_of_samples_for_segment(I32 num_samples);

  // housekeeping
  WAVESsamplingFixedMultiSegment16bitVariable();
  ~WAVESsamplingFixedMultiSegment16bitVariable();
};

// ----------- specialized for varying number of multiple segments ----------------

class WAVESsamplingVaryingMultiSegment : public virtual WAVESsamplingFixedMultiSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_segments() const { return bits_for_number_of_segments; };

  inline BOOL set_bits_for_number_of_segments(U8 bits_for_number_of_segments) { this->bits_for_number_of_segments = bits_for_number_of_segments;  return TRUE; };
  BOOL set_number_of_segments(I32 number_of_segments);

  // housekeeping
  void clean();
  WAVESsamplingVaryingMultiSegment();
  virtual ~WAVESsamplingVaryingMultiSegment();
};

class WAVESsamplingVaryingMultiSegment8bit : public virtual WAVESsamplingVaryingMultiSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 8; };
  inline I32 get_bytes_per_sample() const { return 1; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return num_samples[active]; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples[active]) { sample = samples[active][sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingVaryingMultiSegment8bit();
  virtual ~WAVESsamplingVaryingMultiSegment8bit();
};

class WAVESsamplingVaryingMultiSegment16bit : public virtual WAVESsamplingVaryingMultiSegment
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline I32 get_bits_per_sample() const { return 16; };
  inline I32 get_bytes_per_sample() const { return 2; };

  // access memory and size for reading and writing the waves 
  inline U32 size() const { return 2*num_samples[active]; };

  // compute value and xyz location for an individual sample
  I32 get_sample(I32 sample_idx) { if (sample_idx < num_samples[active]) { sample = ((U16*)samples[active])[sample_idx]; return sample; } return 0; };

  // initialize waves sampling from a pulse sampling description
  virtual BOOL init(const PULSEsampling* sampling);

  // housekeeping
  WAVESsamplingVaryingMultiSegment16bit();
  virtual ~WAVESsamplingVaryingMultiSegment16bit();
};


class WAVESsamplingVaryingMultiSegment8bitFixed : public virtual WAVESsamplingVaryingMultiSegment8bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingVaryingMultiSegment8bitFixed();
  ~WAVESsamplingVaryingMultiSegment8bitFixed();
};

class WAVESsamplingVaryingMultiSegment16bitFixed : public virtual WAVESsamplingVaryingMultiSegment16bit
{
public:
  // override access parameters copied or derived from the pulse descriptor
  inline U8 get_bits_for_number_of_samples() const { return 0; };

  // housekeeping
  WAVESsamplingVaryingMultiSegment16bitFixed();
  ~WAVESsamplingVaryingMultiSegment16bitFixed();
};

class WAVESsamplingVaryingMultiSegment8bitVariable : public virtual WAVESsamplingVaryingMultiSegment8bit
{
public:
  // set or get parameters that read from or write to the waves
  BOOL set_number_of_samples_for_segment(I32 num_samples);

  // housekeeping
  WAVESsamplingVaryingMultiSegment8bitVariable();
  ~WAVESsamplingVaryingMultiSegment8bitVariable();
};

class WAVESsamplingVaryingMultiSegment16bitVariable : public virtual WAVESsamplingVaryingMultiSegment16bit
{
public:
  // set or get parameters that read from or write to the waves
  BOOL set_number_of_samples_for_segment(I32 num_samples);

  // housekeeping
  WAVESsamplingVaryingMultiSegment16bitVariable();
  ~WAVESsamplingVaryingMultiSegment16bitVariable();
};

class WAVESwaves
{
public:

  // access of samplings

  inline U16 get_number_of_samplings() const { return number_of_samplings; };
  inline WAVESsampling* get_sampling(U16 m) const { if (m < number_of_samplings) return samplings[m]; return 0; };

  // access of extra waves bytes

  inline U16 get_number_of_extra_bytes() const { return number_of_extra_bytes; };
  inline U8* get_extra_bytes() const { return extra_bytes; };

  // initialize waves from a pulse descriptor

  BOOL init(const PULSEdescriptor* descriptor);

  // convenient output to text for inspection

  BOOL save_to_txt(FILE* file) const;

  void clean();

  WAVESwaves();
  ~WAVESwaves();

protected:

  // actual data for storing the sampling of the waves

  U16 number_of_samplings;
  WAVESsampling** samplings;

  // actual data for storing the extra waves bytes

  U16 number_of_extra_bytes;
  U8* extra_bytes;
};

#endif
