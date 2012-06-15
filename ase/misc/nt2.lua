
local Thread = Native.Util.Thread;

function Worker(id)
  return function()
    for j = 1, 10000 do
      print("thread " .. id .. " j=" .. j)
      -- Thread.noop()
      Thread.yield()
    end
  end
end

local num = 5
local thr = {}
local i, ok, errstr
ok, errstr = pcall(
  function()
    for i = 1, num do
      thr[i] = Thread(Worker(i))
    end
    for i = 1, num do
      thr[i].start()
    end
  end
)
for i = 1, num do
  --thr[i].join()
  --print("id=" .. i .. " join done");
end
if not ok then
  error(errstr)
end

