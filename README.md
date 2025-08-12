# Winaudit
WinAudit is an inventory utility for Windows computers. It creates a comprehensive report on a machine's configuration, hardware and software. WinAudit is free, open source and can be used or distributed by anyone. It is used by IT experts in academia, government, industry as well as security conscious p... See : http://parmavex.co.uk/winaudit.html

BUILDING WINAUDIT
=================

You do not need to build WinAudit, an executable is available for download at 
www.parmavex.co.uk/winaudit.html . 

Prequisites
------------

1. Visual Studio 2022 or newer with x86/x64 spectre mitigation libraries installed.
The debug build requires the address sanitizer (ASAN) libraries installed.

2. Microsoft SDK 10 or newer.

Build
-----

1. Start Visual Studio. Browse to WinAudit-3.4.3-Source/WinAudit/VS2022 then open the solution file WinAudit.sln

2. If the WinAudit project is not in bold then "Set as StartUp Project".

3. Verify in "Project Dependencies" that WinAudit depends on PxsBase.

4. Select a "Release" build configuration and "x86" platform.

5. Finally, "Build Solution". The output is at \WinAudit\VS2022\Win32\Release\WinAudit.exe

Distribution
------------

Before distributing the executable to others it is recommended that:

1. Test it on a reasonable representation of the machines to verify expected behaviour.
Use the logger to verify there are no unexpected errors.

2. Make a note of the exectuable's MD5 so you can verify it has not been altered.

3. You can build an x64 version for distribution however, avoid this if you intend to export data
to Microsoft Access Database. Some of the database's components are 32-bit so are incompatible with
a 64-bit process.
  


Parmavex
Birmingham
England
