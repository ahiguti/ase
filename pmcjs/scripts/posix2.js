
use('Perl');

Perl.Use("POSIX");
var f = Perl.Functions["POSIX"];
var c = Perl.Constants["POSIX"];
var fd = f.open("foobar.out", c.O_CREAT | c.O_WRONLY, 0644);
var wrlen = f.write(fd, "hello", 5);
f.close(fd);
print("wrote foobar.out");

