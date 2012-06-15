
/* SKIP_CHECKMEM */

use('Perl');
Perl.Use('IO::Socket::INET');
Perl.Use('IO::Poll');
Perl.Use('POSIX');

try {
  var sock = new Perl('IO::Socket::INET',
    'Listen', 5,
    'LocalAddr', 'localhost',
    'LocalPort', 9999,
    'ReuseAddr', 1,
    'Blocking', 1,
    'Proto', 'tcp');
} catch (e) {
  if (e.error == 'PerlNewFailed') {
    var en = Perl.ERRNO;
    var estr = "(errno=" + Perl.Functions['POSIX'].strerror(en) + ")";
    e.description += " " + estr;
  }
  throw e;
}
print("listening on localhost 9999. do 'telnet localhost 9999' to connect.");

var stdin = Perl.Handles[''].STDIN;
var stdout = Perl.Handles[''].STDOUT;

var pollc = Perl.Constants['IO::Poll'];

var sd;
var buf = Perl.NewScalar();
while (sd = sock.accept()) {
  print('ACCEPTED');
  var po = new Perl('IO::Poll');
  po.mask(sd, pollc.POLLIN);
  po.mask(stdin, pollc.POLLIN);
  var done = false;
  while (!done) {
    po.poll();
    var hl = po.InvokeLC('handles', pollc.POLLIN);
    for (i in hl) {
      var h = hl[i];
      /* we must compare using fileno() because h and sd can be different
	objects wrapping the same perl object. */
      if (h.fileno() == sd.fileno()) {
	/* read from client */
	if (h.sysread(buf, 1024) == 0) {
	  done = true;
	  break;
	}
	stdout.syswrite(buf);
      } else if (h.fileno() == stdin.fileno()) {
	/* read from stdin */
	if (h.sysread(buf, 1024) == 0) {
	  done = true;
	  break;
	}
	sd.syswrite(buf);
      } else {
	print(h);
      }
    }
  }
  sd.close();
  print('CLOSED');
}

