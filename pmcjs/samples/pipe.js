
use('Perl');
Perl.Use('IO::Pipe');
var pipe = new Perl('IO::Pipe');
pipe.reader('echo abc');
var l = 0;
while (l = pipe.getline()) {
  print(l);
}

