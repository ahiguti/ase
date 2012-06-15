
/* the following is a verbatim translation of a sample code in the XML::RSS
   manual page. */

use('Perl');
Perl.Use('XML::RSS');
var rss = new Perl('XML::RSS', 'version', '1.0');
/* because rss has a property named 'channel', we need to call method
   'channel' using rss.Invoke(...). */
rss.Invoke('channel',
  'title', 'freshmeat.net',
  'link', 'http://freshmeat.net',
  'description', 'the one-stop-shop for all your Linux software needs',
  'dc', {
    date: '2000-08-23T07:00+00:00',
    subject: 'Linux Software',
    creator: '...',
    publisher: '...',
    rights: 'Copyright 1999, Freshmeat.net',
    language: 'en-us'
  },
  'syn', {
    updatePeriod: 'hourly',
    updateFrequency: '1',
    updateBase: '1901-01-01T00:00+00:00'
  },
  'taxo', [
   'http://dmoz.org/Computers/Internet',
   'http://dmoz.org/Computers/PC'
  ]);
rss.Invoke('image',
  'title', 'freshmeat.net',
  'url', 'http://freshmeat.net/images/fm.mini.jpg',
  'link', 'http://freshmeat.net',
  'dc', {
    creator: '...',
  });
rss.Invoke('add_item',
  'title', 'GTKeyboard 0.85',
  'link', 'http://freshmeat.net/news/1999/06/21/930003829.html',
  'description', 'GTKeyboard is a graphical keyboard that ...',
  'dc', {
    subject: 'X11/Utilities',
    creator: '...',
  },
  'taxo', [
    'http://dmoz.org/Computers/Internet',
    'http://dmoz.org/Computers/PC'
  ]);

print(rss.as_string());

