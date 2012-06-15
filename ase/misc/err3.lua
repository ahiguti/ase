
local ok, err = pcall(
  function()
    local Thread = Java['java.lang.Thread']
  end
)
if not ok then
  print(err)
end
