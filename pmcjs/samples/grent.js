
use('Perl');
Perl.Use('User::grent');
var grent = Perl.Functions['User::grent'];

function show(gr) {
  print("group id==" + gr.gid() + " name=" + gr.name());
  var grmem = gr.members();
  for (var i = 0; i < grmem.length; ++i) {
    print(" member " + grmem[i]);
  }
}

show(grent.getgrgid(0));
show(grent.getgr('daemon'));

