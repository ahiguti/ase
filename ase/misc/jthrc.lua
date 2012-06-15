
local Thread = Java['java.lang.Thread']
local System = Java['java.lang.System']
local Runnable = Java['java.lang.Runnable']

function newWorkerThread(id)
  local obj = {}
  obj.run = function()
    print(id)
  end
  return obj
end

local num = 100000
local i, ok, errstr
local thr
for i = 1, num do
  ok, errstr = pcall(
    function()
      print(i);
      thr = Thread(Runnable(newWorkerThread(i)))
      thr.start()
    end
  )
  thr.join()
  if not ok then
    error(errstr)
  end
end

