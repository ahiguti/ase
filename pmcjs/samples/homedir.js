
use('Perl');
Perl.Use('File::HomeDir');
var homedir = Perl.Methods['File::HomeDir'];
print(homedir.my_home());
print(homedir.my_documents());
print(homedir.my_data());

