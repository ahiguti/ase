
use('Perl');
Perl.Use('HTML::TreeBuilder');
var src =
  '<html><body><p>hello, world</p></body></html>';
var ht = new Perl('HTML::TreeBuilder');
ht.parse(src);
ht.dump();
print();

Perl.Use('HTML::FormatText');
var formatter = new Perl('HTML::FormatText',
  'leftmargin', 20,
  'rightmargin', 50);
print(formatter.format(ht));

