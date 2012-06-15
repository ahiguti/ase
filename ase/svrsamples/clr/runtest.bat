
call "%VS80COMNTOOLS%vsvars32.bat"
devenv ASETestCLR/ASETestCLR.sln /build Release
copy ASETestCLR\bin\Release\ASETestCLR.dll .

..\..\aserun testint.lua
..\..\aserun teststr.lua
..\..\aserun testcbint.lua
..\..\aserun testcbstr.lua

