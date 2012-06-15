
local EXE = CLR.GetExecutingAssembly();
local Core = CLR.GetCore();
print("EA: " .. EXE.FullName);
print("Core: " .. Core.FullName);
print("CORE Location: " .. Core.Location);

local Forms = CLR.Load("System.Windows.Forms");
print(Forms.Location);

local asname = Core.GetName();
print(asname.GetType());
local kp = asname.KeyPair;
if kp == null then
  print("kp is null");
end
-- print(kp.PublicKey());

-- var Forms = CLR.Load("System.Windows.Forms");
-- print(Forms.FullName());

