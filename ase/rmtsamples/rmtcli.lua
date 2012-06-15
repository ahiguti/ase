
function usage()
  error("Usage: rmtcli.lua {-h HOST|-f FILENAME} [LOOPCOUNT] [SERIALIZER]");
end

local args = ASE.Host.Args
if args[1] == nil then
  usage();
end

local obj = nil;
local cnt = 10000;
if (args[0] == "-f") then
  obj = ASE.Host.Load(args[1]).Create();
  -- FIXME
  -- obj = ASE.Host.Load(args[1]).Create();
elseif (args[0] == "-h") then
  local a = { host = args[1], timeout = 30, serializer = "asn1der" }
  if args[3] ~= nil then
    a.serializer = args[3]
  end
  obj = ASE.CPP.Remote(a).Create();
else
  usage();
end
if not (args[2] == nil) then
  cnt = tonumber(args[2]);
end

-- obj.SetInt(100);
-- print(obj.GetInt());

local arr = ASE.Host.Array(1, 2, 3, 4, 5);
-- local arr = { 1, 2, 3, 4, 5 };
--
local x = 0;
local s = obj.Sum;
for i = 1, cnt do
  -- local arr = { 1, 2, 3, 4, 5 };
  x = s(arr);
  -- print(x)
  if not (x == 15) then
    error("wrong: " .. x);
  else
    -- print("ok: " .. x);
  end
end
print(x);
print("done cnt=" .. cnt);

