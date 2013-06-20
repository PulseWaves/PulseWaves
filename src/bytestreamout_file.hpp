/*
===============================================================================

  FILE:  bytestreamout_file.hpp
  
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
#ifndef BYTE_STREAM_OUT_FILE_H
#define BYTE_STREAM_OUT_FILE_H

#include "bytestreamout.hpp"

#include <stdio.h>

#if defined(_MSC_VER) && (_MSC_VER < 1300)
extern "C" int _cdecl _fseeki64(FILE*, __int64, int);
extern "C" __int64 _cdecl _ftelli64(FILE*);
#endif

class ByteStreamOutFile : public ByteStreamOut
{
public:
  ByteStreamOutFile(FILE* file);
/* replace a closed FILE* with a reopened FILE* in "ab" mode */
  BOOL refile(FILE* file);
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
  ~ByteStreamOutFile(){};
protected:
  FILE* file;
};

class ByteStreamOutFileLE : public ByteStreamOutFile
{
public:
  ByteStreamOutFileLE(FILE* file);
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

class ByteStreamOutFileBE : public ByteStreamOutFile
{
public:
  ByteStreamOutFileBE(FILE* file);
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

inline ByteStreamOutFile::ByteStreamOutFile(FILE* file)
{
  this->file = file;
}

inline BOOL ByteStreamOutFile::refile(FILE* file)
{
  if (file == 0) return FALSE;
  this->file = file;
  return TRUE;
}

inline void ByteStreamOutFile::putByte(U8 byte)
{
  if (fputc(byte, file) != byte)
  {
    throw;
  }
}

inline void ByteStreamOutFile::putBytes(const U8* bytes, U32 num_bytes)
{
  if (fwrite(bytes, 1, num_bytes, file) != num_bytes)
  {
    throw;
  }
}

inline BOOL ByteStreamOutFile::isSeekable() const
{
  return (file != stdout);
}

inline I64 ByteStreamOutFile::tell() const
{
#ifdef _WIN32
  return _ftelli64(file);
#else
  return (I64)ftello(file);
#endif
}

inline BOOL ByteStreamOutFile::seek(I64 position)
{
#ifdef _WIN32
  return !(_fseeki64(file, position, SEEK_SET));
#else
  return !(fseeko(file, (off_t)position, SEEK_SET));
#endif
}

inline BOOL ByteStreamOutFile::seekEnd()
{
#ifdef _WIN32
  return !(_fseeki64(file, 0, SEEK_END));
#else
  return !(fseeko(file, (off_t)0, SEEK_END));
#endif
}

inline ByteStreamOutFileLE::ByteStreamOutFileLE(FILE* file) : ByteStreamOutFile(file)
{
}

inline void ByteStreamOutFileLE::put16bitsLE(const U8* bytes)
{
  putBytes(bytes, 2);
}

inline void ByteStreamOutFileLE::put32bitsLE(const U8* bytes)
{
  putBytes(bytes, 4);
}

inline void ByteStreamOutFileLE::put64bitsLE(const U8* bytes)
{
  putBytes(bytes, 8);
}

inline void ByteStreamOutFileLE::put16bitsBE(const U8* bytes)
{
  swapped[0] = bytes[1];
  swapped[1] = bytes[0];
  putBytes(swapped, 2);
}

inline void ByteStreamOutFileLE::put32bitsBE(const U8* bytes)
{
  swapped[0] = bytes[3];
  swapped[1] = bytes[2];
  swapped[2] = bytes[1];
  swapped[3] = bytes[0];
  putBytes(swapped, 4);
}

inline void ByteStreamOutFileLE::put64bitsBE(const U8* bytes)
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

inline ByteStreamOutFileBE::ByteStreamOutFileBE(FILE* file) : ByteStreamOutFile(file)
{
}

inline void ByteStreamOutFileBE::put16bitsLE(const U8* bytes)
{
  swapped[0] = bytes[1];
  swapped[1] = bytes[0];
  putBytes(swapped, 2);
}

inline void ByteStreamOutFileBE::put32bitsLE(const U8* bytes)
{
  swapped[0] = bytes[3];
  swapped[1] = bytes[2];
  swapped[2] = bytes[1];
  swapped[3] = bytes[0];
  putBytes(swapped, 4);
}

inline void ByteStreamOutFileBE::put64bitsLE(const U8* bytes)
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

inline void ByteStreamOutFileBE::put16bitsBE(const U8* bytes)
{
  putBytes(bytes, 2);
}

inline void ByteStreamOutFileBE::put32bitsBE(const U8* bytes)
{
  putBytes(bytes, 4);
}

inline void ByteStreamOutFileBE::put64bitsBE(const U8* bytes)
{
  putBytes(bytes, 8);
}

#endif
