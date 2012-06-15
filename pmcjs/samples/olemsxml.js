
var url = arguments[0];
if (!url) {
  throw "Usage: olemsxml.js URL";
}

use('Perl');
Perl.Use('Win32::OLE');
Perl.Methods['Win32::OLE'].Option('CP', Perl.Constants['Win32::OLE'].CP_UTF8);

var req = new Perl('Win32::OLE', 'Microsoft.XMLHTTP');
req.Open('GET', url, false);
req.Send;
print('status = ' + req.Status);
print(req.GetAllResponseHeaders);
print(req.ResponseText);

