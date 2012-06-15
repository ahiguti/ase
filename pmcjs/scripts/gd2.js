
use('Perl');

Perl.Use("GD");
var P = Perl.Invoke;
var im = P("GD::Image","new",100,100);
var white = P(im,"colorAllocate",255,255,255);
var black = P(im,"colorAllocate",0,0,0);
var red = P(im,"colorAllocate",255,0,0);
var blue = P(im,"colorAllocate",0,0,255);
P(im,"transparent",white);
P(im,"interlaced",'true');
P(im,"rectangle",0,0,99,99,black);
P(im,"arc",50,50,95,75,0,360,blue);
P(im,"fill",50,50,red);
var s = P(im,"png");
//print_nonl(s);

