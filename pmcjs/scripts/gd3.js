
use('Perl');

Perl.Use("GD");
Perl.Use("FileHandle");
var im = new Perl("GD::Image",100,100);
var white = im.colorAllocate(255,255,255);
var black = im.colorAllocate(0,0,0);
var red = im.colorAllocate(255,0,0);
var blue = im.colorAllocate(0,0,255);
im.transparent(white);
im.interlaced('true');
im.rectangle(0,0,99,99,black);
im.arc(50,50,95,75,0,360,blue);
im.fill(50,50,red);
var s = im.png();
var fh = new Perl("FileHandle", "gdout.png", "w");
fh.print(s);
fh.close();
//print_nonl(s);

