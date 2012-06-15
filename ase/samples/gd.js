
Perl.Use("GD");
var im = Perl.NewInstance("GD::Image",100,100);
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
Perl.Use("FileHandle");
var fh = Perl.NewInstance("FileHandle", "gdout.png", "w");
fh.binmode();
fh.print(s);
fh.close();
print("wrote gdout.png");

