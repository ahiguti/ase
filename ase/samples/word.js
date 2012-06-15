
var quit_flag = false;
var cb = {
  'Quit' : function() {
    print("QUIT");
    quit_flag = true;
  }
};
var word = ASE.COM.CreateObject("Word.Application");
ASE.COM.Connect(word, cb);
word.Visible = true;
while (!quit_flag) {
  ASE.COM.DispatchMessage(1000);
  print("WAIT");
}
print("DONE");

