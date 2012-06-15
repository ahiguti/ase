
my $System = $CLR->GetCore();
my $x = $CLR->CreateWrapper($System->{System}->{Boolean}, "true");
print("$x->{Value}\n");
$x->{Value} = "false";
print("$x->{Value}\n");

