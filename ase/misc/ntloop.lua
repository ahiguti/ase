
local Math = Java['java.lang.Math']
local Thread = Native.Util.Thread

function Worker(id)
  return function()
    local r = Math.abs(-99)
    -- print(r)
  end
end

local loop = 100000
for i = 0, loop do
  local thr = Thread(Worker(i))
  thr.start()
  thr.join()
end

