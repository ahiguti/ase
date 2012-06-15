
use('Perl');
Perl.Use('Cairo');

var surface = Perl.Methods['Cairo::ImageSurface'].create('argb32', 100, 100);
var cr = Perl.Methods['Cairo::Context'].create(surface);

cr.rectangle(10, 10, 40, 40);
cr.set_source_rgb(0, 0, 0);
cr.fill();

cr.rectangle(50, 50, 40, 40);
cr.set_source_rgb(1, 1, 1);
cr.fill();

cr.move_to(50, 50);
cr.set_source_rgb(1, 0, 0);
cr.text_path('ABCあ漢');
cr.fill();

cr.show_page();

surface.write_to_png("cairo.png");
print("wrote cairo.png");

