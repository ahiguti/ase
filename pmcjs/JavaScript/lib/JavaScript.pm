
#  This file is part of pmcjs.
#  Copyright (C) 2006 A. Higuchi. All rights reserved.
#  See COPYRIGHT.txt for details.

package JavaScript;

use 5.008008;
use strict;
use warnings;

require Exporter;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use JavaScript ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '0.01';

require XSLoader;
XSLoader::load('JavaScript', $VERSION);

# Preloaded methods go here.

package JavaScript::Tie;
#require Tie::Hash;
#@ISA = (Tie::Hash);

sub TIEHASH {
  my $classname = shift;
  my $inner = shift;
  my $obj = bless \$inner, $classname;
  return $obj;
}

sub FETCH {
  my $obj = shift;
  my $key = shift;
  my $inner = $$obj;
  if ($key eq '') {
    return $inner;
  }
  return $inner->GetProperty($key);
}

sub STORE {
  my $obj = shift;
  my $key = shift;
  my $val = shift;
  if ($key eq '') {
    return;
  }
  my $inner = $$obj;
  $inner->SetProperty($key, $val);
}

sub FIRSTKEY {
  my $obj = shift;
  my $inner = $$obj;
  $inner->ResetIterator();
  return $inner->Iterate();
}

sub NEXTKEY {
  my $obj = shift;
  my $lastkey = shift;
  my $inner = $$obj;
  my $r = $inner->Iterate($lastkey);
  return $r;
}

#sub DELETE { }
#sub CLEAR { }
#sub EXISTS { }
#sub FIRSTKEY { }
#sub NEXTKEY { }
#sub SCALAR { }
#sub UNTIE { }
#sub DESTROY { }

package JavaScript;

sub AUTOLOAD {
  my $al = our $AUTOLOAD;
  return if $al =~ /::DESTROY$/;
  my $instance = shift;
  my $r = $instance->Invoke($al, @_);
  return $r;
}

package JavaScript::HTWrap;

sub AUTOLOAD {
  my $al = our $AUTOLOAD;
  return if $al =~ /::DESTROY$/;
  my $instance = shift;
  my $r = $instance->{''}->Invoke($al, @_);
  return $r;
}

sub new {
  my $class = shift;
  my $inner = shift;
  my %mh = ();
  tie %mh, 'JavaScript::Tie', $inner;
  my $instance = \%mh;
  bless $instance, $class;
  return $instance;
}

sub GetProperty {
  my $instance = shift;
  return $instance->{''}->GetProperty(@_);
}

sub SetProperty {
  my $instance = shift;
  return $instance->{''}->SetProperty(@_);
}

sub Invoke {
  my $instance = shift;
  return $instance->{''}->Invoke(@_);
}

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

JavaScript - Perl extension for blah blah blah

=head1 SYNOPSIS

  use JavaScript;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for JavaScript, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.



=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

a, E<lt>a@localdomainE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2006 by a

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.8 or,
at your option, any later version of Perl 5 you may have available.


=cut
