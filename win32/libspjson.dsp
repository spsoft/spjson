# Microsoft Developer Studio Project File - Name="libspjson" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libspjson - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libspjson.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libspjson.mak" CFG="libspjson - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libspjson - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libspjson - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libspjson - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libspjson - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libspjson - Win32 Release"
# Name "libspjson - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\spjson\spjsoncodec.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsondomparser.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonevent.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonhandle.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsoniterator.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonnode.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonparser.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonreader.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonrpc.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonutils.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\sppbcodec.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\sppbfield.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\sppbrpc.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\sppbtree.cpp
# End Source File
# Begin Source File

SOURCE=..\spjson\testpbcomm.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\spjson\spjsoncodec.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsondomparser.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonevent.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonhandle.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsoniterator.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonnode.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonparser.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonport.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonreader.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonrpc.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\spjsonutils.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\sppbcodec.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\sppbfield.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\sppbrpc.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\sppbtree.hpp
# End Source File
# Begin Source File

SOURCE=..\spjson\testpbcomm.hpp
# End Source File
# End Group
# End Target
# End Project
