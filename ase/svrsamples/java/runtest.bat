
rem NOTE: modify JDKPATH if necessary
set JDKPATH="c:\Program Files\Java\jdk1.6.0_02\bin"

set PATH=%JDKPATH%;%PATH%
javac -g ASETestJava/TestObj.java
jar -cf ASETestJava.jar ASETestJava/*.class

..\..\aserun testint.lua
..\..\aserun teststr.lua
..\..\aserun testcbint.lua
..\..\aserun testcbstr.lua

