
var font = '/usr/share/fonts/japanese/TrueType/sazanami-gothic.ttf';
// var font = 'c:/windows/fonts/MSUIgothic.ttf';

use('Perl');
Perl.Use("GD");
var im = new Perl("GD::Image",100,100);
var gdc = Perl.Constants['GD'];
var white = im.colorAllocate(255,255,255);
var black = im.colorAllocate(0,0,0);
var red = im.colorAllocate(255,0,0);
var blue = im.colorAllocate(0,0,255);
im.transparent(white);
im.interlaced('true');
im.rectangle(0,0,99,99,black);
im.arc(50,50,95,75,0,360,blue);
im.fill(50,50,red);
im.string(gdc.gdSmallFont, 2, 10, 'ABCabc', black);
im.stringFT(black, font, 10, 0, 5, 95, 'あいうえお');
var s = im.png();
Perl.Use("FileHandle");
var fh = new Perl("FileHandle", "gducout.png", "w");
fh.binmode();
fh.print(s);
fh.close();
print("wrote gducout.png");

