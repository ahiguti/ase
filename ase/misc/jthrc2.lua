
local Thread = Native.Util.Thread;
local System = Java['java.lang.System'];

function newWorkerThread(id)
  local obj
  obj = function()
    System.out.println("thr " .. id)
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
      thr = Thread(newWorkerThread(i))
      thr.start()
    end
  )
  thr.join()
  if not ok then
    error(errstr)
  end
  if (i % 10 == 0) then
    System.gc()
  end
end

