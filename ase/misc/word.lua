
local quit_flag = false;
local cb = {
  Quit = function() print("QUIT"); quit_flag = true; end,
};
local word = COM.CreateObject("Word.Application");
COM.Connect(word, cb);
word.Visible = true;
local i = 0;
while (not quit_flag) do
  COM.DispatchMessage(1000);
  print("sleep " .. i);
  i = i + 1;
end
print("DONE");

