
print("LUA Loaded")

local obj = JS.obj;
for k in obj.Keys() do
  print(k .. " = " .. obj[k])
end
