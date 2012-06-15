
-- SKIP_CHECKMEM

Perl.Use('Gtk2')
Perl.Use('Gtk2::GladeXML')
Perl.Use('IO::File')

local gtk = Perl.Methods['Gtk2']
gtk.init()
local gtkclipboard = Perl.Methods['Gtk2::Clipboard']
local clipboard_atom = Perl.Methods['Gtk2::Gdk::Atom'].intern('CLIPBOARD')
local gladexml = Perl.NewInstance('Gtk2::GladeXML', 'pmcnotepad.glade')
local mainwin = gladexml.get_widget('mainwindow')
local display = mainwin.get_display()
local clipboard = gtkclipboard.get_for_display(display, clipboard_atom)
local textview = gladexml.get_widget('textview')
local textbuf = textview.get_buffer()

local current_filename = nil
function select_file(title)
  local fs = Perl.NewInstance('Gtk2::FileSelection', title)
  if (current_filename) then
    fs.set_filename(current_filename)
  end
  local response = fs.run()
  local r = nil
  if (response == 'ok') then
    current_filename = fs.get_filename()
    r = current_filename
  end
  fs.destroy()
  return r
end
function read_file(filename)
  local s = ""
  local fh = Perl.NewInstance('IO::File', filename, 'r')
  if (not fh) then
    return s
  end
  local x = 0
  while true do
    local x = fh.getline()
    if (not x) then
      break
    end
    s = s .. x
  end
  fh.close()
  return s
end
function write_file(filename, str)
  local fh = Perl.NewInstance('IO::File', filename, 'w')
  if (not fh) then
    return 1
  end
  fh.write(str, str.length)
  return fh.close()
end
mainwin.signal_connect('delete_event', function() gtk.main_quit(); end);
local callbacks = {
  on_quit_activate = function() gtk.main_quit(); end,
  on_new_activate = function() 
    textbuf.delete_interactive(textbuf.get_start_iter(),
      textbuf.get_end_iter(), 1);
    current_filename = null
  end,
  on_open_activate = function()
    if (select_file('open')) then
      s = read_file(current_filename)
      if (s) then
	textbuf.set_text(s)
      end
    end
  end,
  on_save_activate = function()
    if (not current_filename) then
      if (not select_file('save')) then
	return;
      end
    end
    local s = textbuf.get_text(textbuf.get_start_iter(),
      textbuf.get_end_iter(), 0)
    write_file(current_filename, s)
  end,
  on_save_as_activate = function()
    if (not select_file('save')) then
      return
    end
    local s = textbuf.get_text(textbuf.get_start_iter(),
      textbuf.get_end_iter(), 0)
    write_file(current_filename, s)
  end,
  on_cut_activate = function() textbuf.cut_clipboard(clipboard, 1); end,
  on_copy_activate = function() textbuf.copy_clipboard(clipboard); end,
  on_paste_activate = function()
    textbuf.paste_clipboard(clipboard, undefined, 1); end,
  on_delete_activate = function() textbuf.delete_selection(1, 1); end
}
local v
for i,v in pairs(callbacks) do
  gladexml.signal_autoconnect_all(i, callbacks[i])
end
gtk.main()

