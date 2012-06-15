
use('Perl');
Perl.Use('DateTime');
var dt = new Perl('DateTime',
  'year', 2003,
  'month', 4,
  'day', 5,
  'hour', 2,
  'minute', 30,
  'second', 15,
  'nanosecond', 500000000,
  'time_zone', 'America/Chicago');
dt.set_time_zone('UTC');
print(dt.datetime());

