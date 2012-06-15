
/* SKIP_CHECKMEM (fork() causes confusion) */

var core = Perl.Functions['CORE'];

core.system('echo abc');
core.system('echo', 'abcdef');

var pid = core.fork();
if (pid == 0) {
  print('child1');
  core.exit(3);
}
var k = core.waitpid(pid, 0);
var st = Perl.CHILD_ERROR >> 8;
print(st);

var pid = core.fork();
if (pid == 0) {
  core.exec('echo', 'child2');
}
var k = core.wait();
var st = Perl.CHILD_ERROR >> 8;
print(st);

