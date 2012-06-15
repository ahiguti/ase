
use utf8;
package JSObjTest;

sub jsobj_getprop {
  my $jsobj = shift;
  my $key = shift;
  return $jsobj->GetProperty($key);
}

sub jsobj_setprop {
  my $jsobj = shift;
  my $key = shift;
  my $val = shift;
  $jsobj->SetProperty($key, $val);
}

sub jsobj_getprop_smart {
  my $jsobj = shift;
  my $key = shift;
  return $jsobj->{$key};
}

sub jsobj_setprop_smart {
  my $jsobj = shift;
  my $key = shift;
  my $val = shift;
  $jsobj->{$key} = $val;
}

sub jsobj_get_プロパティ {
  my $jsobj = shift;
  return $jsobj->GetProperty('プロパティ');
}

sub jsobj_set_プロパティ {
  my $jsobj = shift;
  my $val = shift;
  $jsobj->SetProperty('プロパティ', $val);
}

sub jsobj_get_プロパティ_smart {
  my $jsobj = shift;
  return $jsobj->{'プロパティ'};
}

sub jsobj_set_プロパティ_smart {
  my $jsobj = shift;
  my $val = shift;
  $jsobj->{'プロパティ'} = $val;
}

sub jsobj_invoke_foo {
  my $jsobj = shift;
  return $jsobj->Invoke('foo', @_);
}

sub jsobj_invoke_foo_smart {
  my $jsobj = shift;
  return $jsobj->foo(@_);
}

sub jsobj_invoke_手続き {
  my $jsobj = shift;
  return $jsobj->Invoke('手続き', @_);
}

sub jsobj_invoke_手続き_smart {
  my $jsobj = shift;
  return $jsobj->手続き(@_);
}

1;

