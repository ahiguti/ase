
var cb = {
  'Quit' : function() { print("QUIT"); }
};
var word = COM.CreateObject("Word.Application");
COM.Connect(word, cb);
word.Visible = true;
var i = 0;
while (true) {
  COM.DispatchMessage(1000);
  print("sleep " + i);
  i += 1
}
print("DONE");

