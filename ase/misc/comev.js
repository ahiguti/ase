
function cb_Quit() {
  WScript.Echo("QUIT");
  WScript.Quit();
};
var word = WScript.CreateObject("Word.Application");
WScript.ConnectObject(word, "cb_");
word.Visible = true;

while (true) {
  WScript.Sleep(1000);
}

