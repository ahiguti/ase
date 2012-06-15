
function LoadDll(fname, cname)
  local cwd = CLR.GetCore().System.IO.Directory.GetCurrentDirectory()
  local fname = cwd .. '\\' .. fname
  print("Loading " .. fname);
  return CLR.Load(fname, cname)
end

