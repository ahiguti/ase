
ASE.Perl.Use("GD")
local im = Perl.NewInstance("GD::Image",100,100);
local white = im.colorAllocate(255,255,255);
local black = im.colorAllocate(0,0,0);
local red = im.colorAllocate(255,0,0);
local blue = im.colorAllocate(0,0,255);
im.transparent(white);
im.interlaced('true');
im.rectangle(0,0,99,99,black);
im.arc(50,50,95,75,0,360,blue);
im.fill(50,50,red);
local s = im.png();
Perl.Use("FileHandle");
local fh = Perl.NewInstance("FileHandle", "gdout.png", "w");
fh.binmode();
fh.print(s);
fh.close();
print("wrote gdout.png");

