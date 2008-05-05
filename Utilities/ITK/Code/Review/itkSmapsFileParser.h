/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSmapsFileParser.h,v $
  Language:  C++
  Date:      $Date: 2008-04-10 12:33:37 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSmapsFileParser_h
#define __itkSmapsFileParser_h

#include "itkConfigure.h"
#include "itkWin32Header.h"
#include "itkExceptionObject.h"

#include <string>
#include <vector>
#include <map>
#include <istream>
#include <iostream>

namespace itk {


/** MAP RECORD **/


class ITKCommon_EXPORT MapRecord
{
public:
  typedef unsigned int  MemoryLoadType;

  /** Reset the record
  */
  void Reset(void);

  /** Optional record name
  */
  std::string  m_RecordName;

 /** Contains a list of token with the associated memory allocated, tokens 
   *  could be typically: Size, Rss, Shared_Clean, Shared_Dirty, Private_Clean, 
   *  Private_Dirty, Referenced.
  */
  std::map<std::string, MemoryLoadType> m_Tokens;

};

/** This struct contains an entry in a smaps file. 
 *  It is filled by operator>>(istream&,SmapsRecord&).
*/
class ITKCommon_EXPORT SmapsRecord : public MapRecord
{
    /** Input operator to fill a SmapsRecord 
   *  The format has to be the following:
   *  "address permissions offset device inode optional_name
   *   Token1:      number kB
   *   Token2:      number kB
   *   ...
   *   TokenN:      number kB"
   *  Example:
   *  00101000-0023b000 r-xp 00000000 fd:00 165671     /lib/libc-2.5.so
   *  Size:              1256 kB
   *  Rss:                640 kB
   *  Shared_Clean:       640 kB
   *  Shared_Dirty:         0 kB
   *  Private_Clean:        0 kB
   *  Private_Dirty:        0 kB
  */
  friend ITKCommon_EXPORT std::istream&  operator>>(std::istream &in, SmapsRecord &record);
};

/** This struct contains an entry in a smaps file. 
 *  It is filled by operator>>(istream&,SmapsRecord&).
*/
class ITKCommon_EXPORT VMMapRecord : public MapRecord
{

    /** Input operator to fill a SmapsRecord 
   *  The format has to be the following:
   *  "address permissions offset device inode optional_name
   *   Token1:      number kB
   *   Token2:      number kB
   *   ...
   *   TokenN:      number kB"
   *  Example:
   *  00101000-0023b000 r-xp 00000000 fd:00 165671     /lib/libc-2.5.so
   *  Size:              1256 kB
   *  Rss:                640 kB
   *  Shared_Clean:       640 kB
   *  Shared_Dirty:         0 kB
   *  Private_Clean:        0 kB
   *  Private_Dirty:        0 kB
  */
  friend ITKCommon_EXPORT std::istream&  operator>>(std::istream &in, VMMapRecord &record);
};


/** MAP DATA **/


/** Base class for the ?map data container. 
 *  Inherited classes must implement their own 
*/
template <class TMapRecord>
class ITK_EXPORT MapData{
public:
  /** need an unsigned long type to be able to accumulate the SmapsRecord */
  typedef unsigned long             MemoryLoadType;
  virtual ~MapData();

  /** Returns the heap usage in kB of the process */
  virtual MemoryLoadType GetHeapUsage() = 0;
  /** Returns the stack usage in kB of the process */
  virtual MemoryLoadType GetStackUsage() = 0;  
  /** Returns the total memory usage in kB of the process */
  virtual MemoryLoadType GetTotalMemoryUsage() = 0;
  /** Returns the memory usage in kB of a process segment */
  virtual MemoryLoadType GetMemoryUsage( const char * filter, const char * token ) =0;

  /** Returns true if the data has not been initialized yet */
  bool Empty();
protected:
  /** Clear the content of the container */
  void Reset(void);
protected: 
  typedef TMapRecord                    MapRecordType;
  typedef typename std::vector< MapRecordType >  MapRecordVectorType;

  /** contains all the segment records */
  MapRecordVectorType                   m_Records;
};

/** \class SmapsData_2_6 
 *  \brief Read a smaps stream and return the memory usage information.
 *  Smaps files have been added since the linux kernel 2.6 
*/
template <class TMapRecord>
class ITK_EXPORT SmapsData_2_6:public MapData<TMapRecord>
{
public:
  typedef typename MapData<TMapRecord>::MemoryLoadType MemoryLoadType;
  
  virtual ~SmapsData_2_6();

  /** Returns the heap usage in kB of the process */
  virtual MemoryLoadType GetHeapUsage();
  /** Returns the stack usage in kB of the process */
  virtual MemoryLoadType GetStackUsage();
  /** Returns the total memory usage in kB of the process */
  virtual MemoryLoadType GetTotalMemoryUsage();
  /** Returns the memory usage in kB of a process segment. 
   *  Typically, token == "Size" 
  */
  virtual MemoryLoadType GetMemoryUsage( const char * filter, const char * token );

  /** fill the smaps data */
  template<class TSmapsRecordType> 
  friend ITK_EXPORT std::istream& operator>>( std::istream &smapsStream,
                                              SmapsData_2_6<TSmapsRecordType>&data);

protected:
  bool                m_HeapRecordFound;  
};

/**  
 *  Apparently the vmmap command exists since at least Mac OS X v10.2
 *  On Tiger, /usr/bin/vmmap used to be installed by the Essentials.pkg, 
 *  On Panther, /usr/bin/vmmap used to be installed by the DevTools.pkg, 
*/
template <class TMapRecord>
class ITK_EXPORT VMMapData_10_2:public MapData<TMapRecord>
{
public:
  typedef typename MapData<TMapRecord>::MemoryLoadType MemoryLoadType;

  virtual ~VMMapData_10_2();

  /** Returns the heap usage in kB of the process */
  virtual MemoryLoadType GetHeapUsage();
  /** Returns the stack usage in kB of the process */
  virtual MemoryLoadType GetStackUsage();  
  /** Returns the total memory usage in kB of the process */
  virtual MemoryLoadType GetTotalMemoryUsage();
  /** Returns the memory usage in kB of a process segment */
  virtual MemoryLoadType GetMemoryUsage( const char * filter, const char * token );

  /** fill the smaps data */
  template<class TVMMapRecordType> 
  friend ITK_EXPORT std::istream& operator>>( std::istream &stream,
                                              VMMapData_10_2<TVMMapRecordType>&data);
};


/** MAP FILE PARSER **/


template<class TMapData>
class ITK_EXPORT MapFileParser
{
public: 
  typedef typename TMapData::MemoryLoadType MemoryLoadType;

  virtual ~MapFileParser();
  /** Load and parse a Map file pointed by mapFileLocation.
   *  If mapFileLocation is empty, load the default file  
   *  Throw an exception is the file can't be opened.
   */
  virtual void ReadFile( const std::string &mapFileLocation = "") = 0;
  /** ReRead the last parsed file to refresh the memory usage.
   *  Returns true if read from the default location "".
  */
  bool Update(void);

  /** Returns the heap usage in kB of the process. 
   *  If no file has been loaded yet, load a default file.
  */
  MemoryLoadType GetHeapUsage();
  /** Returns the heap usage in kB of the process. 
   *  If no file has been loaded yet, load a default file.
  */
  MemoryLoadType GetStackUsage();
  /** Returns the total memory usage in kB of a process. 
   *  If no file has been loaded yet, load a default file.
  */
  MemoryLoadType GetTotalMemoryUsage();
  /** Returns the memory usage in kB of a process segment. 
   *  If no file has been loaded yet, load a default file.
  */
  MemoryLoadType GetMemoryUsage( const char* filter , const char * token = "Size" );

protected: 
  std::string   m_MapFilePath;  //< location of the last loaded Map file
  TMapData      m_MapData;      //< data of the loaded smap file
};

/** \class SmapsFileParser 
 *  \brief Read a smap file (typically located in /proc/PID/smaps) and extract the 
 *  memory usage information. Any smaps data reader can be used in template as 
 *  long as they implement a operator>>(istream&) and have GetXXXUsage() methods.
*/
template<class TSmapsDataType>
class ITK_EXPORT SmapsFileParser: public MapFileParser<TSmapsDataType>
{
public:
  virtual ~SmapsFileParser();
  /** Load and parse the smaps file pointed by smapsFileLocation.
   *  If smapsFileLocation is empty, load the file located at 
   *  "/proc/" + PID + "/smaps" 
   *  Throw an exception is the file can't be opened.
   */
  virtual void ReadFile( const std::string &mapFileLocation = "");

};

/** \class VMMapFileParser 
 *  \brief Read the output of a vmmap command and extract the 
 *  memory usage information. Used for MAC OS X machines.
*/
template<class TVMMapDataType>
class ITK_EXPORT VMMapFileParser: public MapFileParser<TVMMapDataType>
{
public:
  virtual ~VMMapFileParser();
  /** If no vmmap file, create one using "vmmap pid" command 
   *  Throw an exception is the file can't be created/opened.
   */
  virtual void ReadFile( const std::string &mapFileLocation = "");

};

}  // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSmapsFileParser.txx"
#endif

#endif // __itkSmapsFileParser_h
