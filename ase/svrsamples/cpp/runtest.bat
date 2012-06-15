
call "%VS90COMNTOOLS%vsvars32.bat"
devenv vc9/ASETestCPP.sln /build Release

..\..\aserun testint.lua
..\..\aserun testintfast.lua
..\..\aserun testobj.lua

