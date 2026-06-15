#!/usr/bin/perl

use strict;
use warnings;

# ---------------------------------------------------------------------------
# counter.pl — Word Frequency Counter
#
# Called by the embedded Perl interpreter inside main.c.
# Receives the file path via @ARGV (injected from C before running this script).
# Sets $main::output with the formatted report string.
# ---------------------------------------------------------------------------

my $filepath = $ARGV[0] or die "Error: no file path in \@ARGV.\n";

die "Error: file '$filepath' not found.\n" unless -f $filepath;

open( my $fh, '<', $filepath ) or die "Error: cannot open '$filepath': $!\n";
my %freq;
while ( my $line = <$fh> ) {
    chomp $line;

    # Normalize: lowercase, strip punctuation, split on whitespace
    my @words = split /\s+/, lc( $line =~ s/[^a-z0-9\s]//gr );

    for my $word (@words) {
        next unless length($word);
        $freq{$word}++;
    }
}
close($fh);

# Build result string sorted by frequency (desc), then alphabetically
my $total_words  = 0;
my $unique_words = scalar keys %freq;
$total_words += $_ for values %freq;

my $result = "=== Word Frequency Report ===\n";
$result .= sprintf( "File        : %s\n", $filepath );
$result .= sprintf( "Total words : %d\n", $total_words );
$result .= sprintf( "Unique words: %d\n", $unique_words );
$result .= "-" x 35 . "\n";
$result .= sprintf( "%-20s %s\n", "WORD", "COUNT" );
$result .= "-" x 35 . "\n";

for my $word ( sort { $freq{$b} <=> $freq{$a} || $a cmp $b } keys %freq ) {
    $result .= sprintf( "%-20s %d\n", $word, $freq{$word} );
}

$result .= "=" x 35 . "\n";

# Return the result to C via $main::output
$main::output = $result;
