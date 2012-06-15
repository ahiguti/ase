
call "%VS80COMNTOOLS%vsvars32.bat"
devenv vc8/ASETestNative.sln /build Release

..\..\aserun testint.lua
..\..\aserun testintfast.lua
..\..\aserun teststr.lua
..\..\aserun teststrfast.lua
..\..\aserun testcbint.lua
..\..\aserun testcbstr.lua

