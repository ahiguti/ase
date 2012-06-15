
use('Perl');
Perl.Use('Term::ANSIColor');
var ac = Perl.Constants['Term::ANSIColor'];
print(ac.BOLD + ac.BLUE + "bold blue" + ac.RESET);
print(ac.RED + "red" + ac.RESET);
print(ac.BLINK + ac.YELLOW + "blink yellow" + ac.RESET);
print("normal");
