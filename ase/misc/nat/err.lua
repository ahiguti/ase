
local NTestMod = Native.Util.Meta.LoadFile("ntest.cpp")
local TestNative = NTestMod.TestNative
local obj = TestNative()
error("lua throws");
