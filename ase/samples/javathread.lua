
local Thread = ASE.Java['java.lang.Thread']
local System = ASE.Java['java.lang.System']
local Runnable = ASE.Java['java.lang.Runnable']

function newWorkerThread(id)
  local obj = {}
  obj.run = function()
    local j
    for j = 1, 10000 do
      print("thread " .. id .. " j=" .. j)
      Thread.yield()
    end
  end
  return obj
end

local num = 5
local thr = {}
local i, ok, errstr
ok, errstr = pcall(
  function()
    for i = 1, num do
      thr[i] = Thread(Runnable(newWorkerThread(i)))
    end
    for i = 1, num do
      thr[i].start()
    end
  end
)
for i = 1, num do
  thr[i].join()
  print("id=" .. i .. " join done")
end
if not ok then
  error(errstr)
end

