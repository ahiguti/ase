
/* SKIP_CHECKMEM */

use('Perl');
Perl.Use('Gtk2');
var gtk = Perl.Methods['Gtk2'];
gtk.init();

var win = new Perl('Gtk2::Window', 'toplevel');
win.resize(100, 30);
var vbox = new Perl('Gtk2::VBox');
var button = new Perl('Gtk2::Button', 'Quit');
var txtw = new Perl('Gtk2::TextView');
txtw.set_size_request(300, 300);
win.add(vbox);
vbox.pack_start(button, 0, 0, 0);
vbox.pack_start(txtw, 1, 1, 0);
var quit_mainloop = function() { gtk.main_quit(); }
win.signal_connect('delete_event', quit_mainloop);
button.signal_connect('clicked', quit_mainloop);

win.show_all();
gtk.main();
print('done');

