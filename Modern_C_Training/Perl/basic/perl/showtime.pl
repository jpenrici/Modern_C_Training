#!/ usr / bin / perl
# perl/showtime.pl
#Reference : https://perldoc.perl.org/perlembed

use strict;
use warnings;

sub showtime {

    my ( $sec, $min, $hour, $day, $mon, $year ) = localtime(time);
    printf( "Now: %02d/%02d/%04d %02d:%02d:%02d\n",
        $day, $mon, $year, $hour, $min, $sec );
}
