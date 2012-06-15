
local quit_flag = false;
local cb = {
  Quit = function()
    print("QUIT")
    quit_flag = true
  end,
};
local word = ASE.COM.CreateObject("Word.Application");
ASE.COM.Connect(word, cb);
word.Visible = true;
while (not quit_flag) do
  ASE.COM.DispatchMessage(1000);
  print("WAIT");
end
print("DONE");

