
use('Perl');
Perl.Use('HTML::Template');
var tmpl = new Perl('HTML::Template', 'filename', 'test.tmpl')
var env = Perl.Hashes[''].ENV;
tmpl.param('HOME', env.HOME);
tmpl.param('PATH', env.PATH);
print('Content-Type: text/html');
print('');
print(tmpl.output());

