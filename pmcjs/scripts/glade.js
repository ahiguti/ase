
/* SKIP_CHECKMEM */

use('Perl');
Perl.Use('Gtk2');
Perl.Use('Gtk2::GladeXML');
Perl.Use('IO::File');

var gtk = Perl.Methods['Gtk2'];
gtk.init();
var gtkclipboard = Perl.Methods['Gtk2::Clipboard'];
var clipboard_atom = Perl.Methods['Gtk2::Gdk::Atom'].intern('CLIPBOARD');
var gladexml = new Perl('Gtk2::GladeXML', 'pmcnotepad.glade');
var mainwin = gladexml.get_widget('mainwindow');
var display = mainwin.get_display();
var clipboard = gtkclipboard.get_for_display(display, clipboard_atom);
var textview = gladexml.get_widget('textview');
var textbuf = textview.get_buffer();

var current_filename = null;
function select_file(title) {
  var fs = new Perl('Gtk2::FileSelection', title);
  if (current_filename) {
    fs.set_filename(current_filename);
  }
  var response = fs.run();
  var r = null;
  if (response == 'ok') {
    current_filename = fs.get_filename();
    r = current_filename;
  }
  fs.destroy();
  return r;
}
function read_file(filename) {
  var s = "";
  var fh = new Perl('IO::File', filename, 'r');
  if (!fh) {
    return s;
  }
  var x = 0;
  while (x = fh.getline()) {
    s += x;
  }
  fh.close();
  return s;
}
function write_file(filename, str) {
  var fh = new Perl('IO::File', filename, 'w');
  if (!fh) {
    return 1;
  }
  fh.write(str, str.length);
  return fh.close();
}
mainwin.signal_connect('delete_event', function() { gtk.main_quit(); });
var callbacks = {
  on_quit_activate : function() { gtk.main_quit(); },
  on_new_activate : function() {
    textbuf.delete_interactive(textbuf.get_start_iter(),
      textbuf.get_end_iter(), 1);
    current_filename = null;
  },
  on_open_activate : function() {
    if (select_file('open')) {
      s = read_file(current_filename);
      if (s) {
	textbuf.set_text(s);
      }
    }
  },
  on_save_activate : function() {
    if (!current_filename) {
      if (!select_file('save')) {
	return;
      }
    }
    var s = textbuf.get_text(textbuf.get_start_iter(),
      textbuf.get_end_iter(), 0);
    write_file(current_filename, s);
  },
  on_save_as_activate : function() {
    if (!select_file('save')) {
      return;
    }
    var s = textbuf.get_text(textbuf.get_start_iter(),
      textbuf.get_end_iter(), 0);
    write_file(current_filename, s);
  },
  on_cut_activate : function() { textbuf.cut_clipboard(clipboard, 1); },
  on_copy_activate : function() { textbuf.copy_clipboard(clipboard); },
  on_paste_activate : function() {
    textbuf.paste_clipboard(clipboard, undefined, 1); },
  on_delete_activate : function() { textbuf.delete_selection(1, 1); }
};
for (var i in callbacks) {
  gladexml.signal_autoconnect_all(i, callbacks[i]);
}
gtk.main();

