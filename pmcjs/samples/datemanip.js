
use('Perl');
Perl.Use('Date::Manip');
var datemanip = Perl.Functions['Date::Manip'];

var dt = datemanip.ParseDate('today');
print(datemanip.UnixDate(dt, "It is now %T on %b %e, %Y."));

