/*
===============================================================================

  FILE:  bytestreamout_ostream.hpp
  
  CONTENTS:
      
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
  
     1 October 2011 -- added 64 bit file support in MSVC 6.0 at McCafe at Hbf Linz
    10 January 2011 -- licensing change for LGPL release and liblas integration
    12 December 2010 -- created from ByteStreamOutFile after Howard got pushy (-;
  
===============================================================================
*/
#ifndef BYTE_STREAM_OUT_OSTREAM_H
#define BYTE_STREAM_OUT_OSTREAM_H

#include "bytestreamout.hpp"

#ifdef LZ_WIN32_VC6
#include <fstream.h>
#else
#include <istream>
#include <fstream>
using namespace std;
#endif

class ByteStreamOutOstream : public ByteStreamOut
{
public:
  ByteStreamOutOstream(ostream& stream);
/* write a single byte                                       */
  void putByte(U8 byte);
/* write an array of bytes                                   */
  void putBytes(const U8* bytes, U32 num_bytes);
/* is the stream seekable (e.g. standard out is not)         */
  BOOL isSeekable() const;
/* get current position of stream                            */
  I64 tell() const;
/* seek to this position in the stream                       */
  BOOL seek(const I64 position);
/* seek to the end of the file                               */
  BOOL seekEnd();
/* destructor                                                */
  ~ByteStreamOutOstream(){};
protected:
  ostream& stream;
};

class ByteStreamOutOstreamLE : public ByteStreamOutOstream
{
public:
  ByteStreamOutOstreamLE(ostream& stream);
/* write 16 bit low-endian field                             */
  void put16bitsLE(const U8* bytes);
/* write 32 bit low-endian field                             */
  void put32bitsLE(const U8* bytes);
/* write 64 bit low-endian field                             */
  void put64bitsLE(const U8* bytes);
/* write 16 bit big-endian field                             */
  void put16bitsBE(const U8* bytes);
/* write 32 bit big-endian field                             */
  void put32bitsBE(const U8* bytes);
/* write 64 bit big-endian field                             */
  void put64bitsBE(const U8* bytes);
private:
  U8 swapped[8];
};

class ByteStreamOutOstreamBE : public ByteStreamOutOstream
{
public:
  ByteStreamOutOstreamBE(ostream& stream);
/* write 16 bit low-endian field                             */
  void put16bitsLE(const U8* bytes);
/* write 32 bit low-endian field                             */
  void put32bitsLE(const U8* bytes);
/* write 64 bit low-endian field                             */
  void put64bitsLE(const U8* bytes);
/* write 16 bit big-endian field                             */
  void put16bitsBE(const U8* bytes);
/* write 32 bit big-endian field                             */
  void put32bitsBE(const U8* bytes);
/* write 64 bit big-endian field                             */
  void put64bitsBE(const U8* bytes);
private:
  U8 swapped[8];
};

inline ByteStreamOutOstream::ByteStreamOutOstream(ostream& stream_param) :
    stream(stream_param)
{
}

inline void ByteStreamOutOstream::putByte(U8 byte)
{
  stream.put(byte);
  if (!stream.good())
  {
    throw;
  }
}

inline void ByteStreamOutOstream::putBytes(const U8* bytes, U32 num_bytes)
{
  stream.write((const char*)bytes, num_bytes);
  if (!stream.good())
  {
    throw;
  }
}

inline BOOL ByteStreamOutOstream::isSeekable() const
{
  return !!(static_cast<ofstream&>(stream));
}

inline I64 ByteStreamOutOstream::tell() const
{
  return (I64)stream.tellp();
}

inline BOOL ByteStreamOutOstream::seek(I64 position)
{
  stream.seekp(static_cast<streamoff>(position));
  return stream.good();
}

inline BOOL ByteStreamOutOstream::seekEnd()
{
  stream.seekp(0, ios::end);
  return stream.good();
}

inline ByteStreamOutOstreamLE::ByteStreamOutOstreamLE(ostream& stream) : ByteStreamOutOstream(stream)
{
}

inline void ByteStreamOutOstreamLE::put16bitsLE(const U8* bytes)
{
  putBytes(bytes, 2);
}

inline void ByteStreamOutOstreamLE::put32bitsLE(const U8* bytes)
{
  putBytes(bytes, 4);
}

inline void ByteStreamOutOstreamLE::put64bitsLE(const U8* bytes)
{
  putBytes(bytes, 8);
}

inline void ByteStreamOutOstreamLE::put16bitsBE(const U8* bytes)
{
  swapped[0] = bytes[1];
  swapped[1] = bytes[0];
  putBytes(swapped, 2);
}

inline void ByteStreamOutOstreamLE::put32bitsBE(const U8* bytes)
{
  swapped[0] = bytes[3];
  swapped[1] = bytes[2];
  swapped[2] = bytes[1];
  swapped[3] = bytes[0];
  putBytes(swapped, 4);
}

inline void ByteStreamOutOstreamLE::put64bitsBE(const U8* bytes)
{
  swapped[0] = bytes[7];
  swapped[1] = bytes[6];
  swapped[2] = bytes[5];
  swapped[3] = bytes[4];
  swapped[4] = bytes[3];
  swapped[5] = bytes[2];
  swapped[6] = bytes[1];
  swapped[7] = bytes[0];
  putBytes(swapped, 8);
}

inline ByteStreamOutOstreamBE::ByteStreamOutOstreamBE(ostream& stream) : ByteStreamOutOstream(stream)
{
}

inline void ByteStreamOutOstreamBE::put16bitsLE(const U8* bytes)
{
  swapped[0] = bytes[1];
  swapped[1] = bytes[0];
  putBytes(swapped, 2);
}

inline void ByteStreamOutOstreamBE::put32bitsLE(const U8* bytes)
{
  swapped[0] = bytes[3];
  swapped[1] = bytes[2];
  swapped[2] = bytes[1];
  swapped[3] = bytes[0];
  putBytes(swapped, 4);
}

inline void ByteStreamOutOstreamBE::put64bitsLE(const U8* bytes)
{
  swapped[0] = bytes[7];
  swapped[1] = bytes[6];
  swapped[2] = bytes[5];
  swapped[3] = bytes[4];
  swapped[4] = bytes[3];
  swapped[5] = bytes[2];
  swapped[6] = bytes[1];
  swapped[7] = bytes[0];
  putBytes(swapped, 8);
}

inline void ByteStreamOutOstreamBE::put16bitsBE(const U8* bytes)
{
  putBytes(bytes, 2);
}

inline void ByteStreamOutOstreamBE::put32bitsBE(const U8* bytes)
{
  putBytes(bytes, 4);
}

inline void ByteStreamOutOstreamBE::put64bitsBE(const U8* bytes)
{
  putBytes(bytes, 8);
}

#endif
