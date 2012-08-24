use strict;
use warnings;

use FileHandle;

use constant MAXGEN     => 10000;
use constant NONWORD    => '\n';

my $statetab = {};

sub build_table {
    my $fh          = shift;
    my $table_ref   = shift;

    my $w1          = NONWORD;
    my $w2          = NONWORD;
   
    while (<$fh>) {
        foreach my $suffix (split) {
            push @{$table_ref->{$w1}->{$w2}}, $suffix;
            ($w1, $w2) = ($w2, $suffix);
        }
    }
    push @{$table_ref->{$w1}->{$w2}}, NONWORD;
}

sub generate {
    my $table_ref   = shift;

    my $w1          = NONWORD;
    my $w2          = NONWORD;
    
    for (my $i = 0; $i < MAXGEN; $i++) {
        my $suffixes_ref    = $table_ref->{$w1}->{$w2};
        my $rand_position   = int(rand @$suffixes_ref);
        my $suffix          = $suffixes_ref->[$rand_position];
        
        if ($suffix eq NONWORD) {last;} 
        
        print "$suffix ";
        ($w1, $w2) = ($w2, $suffix);
    }
    print "\n";
}

sub dump_table {
    my $table_ref   = shift;

    foreach my $w1 (sort keys %$table_ref) {
        foreach my $w2 (sort keys %{$table_ref->{$w1}}) {
            print "[$w1][$w2] : @{$table_ref->{$w1}->{$w2}}\n";
        }
    }
}

#main
my $file_name   = shift;

my $fh          = FileHandle->new($file_name, "r");
build_table($fh, $statetab);
generate($statetab);

$fh->close;
exit(0);
