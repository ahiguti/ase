
print("LUA Loaded")

function luafunc()
  local obj = { abc = 120, def = 250, foo = 999 }
  obj[3] = 300
  obj[4] = 400
  obj.bar = 'barval'
  obj["4"] = "774val"
  return obj
end

