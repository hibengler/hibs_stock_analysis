

use Finance::Quote;
open(XDATE,'date +"%Y-%m-%d" |');
$dt = <XDATE>;
$dt =~ s/\n//g;
$dt = "$dt " . $ARGV[0] . ":00.0";
my $q = Finance::Quote->new();
$q->failover(1);
open(ALL_COMPANIES,"all_companies.dat");
$i = 0;
while ($symbol = <ALL_COMPANIES>) {
  $symbol =~ s/\n//g;
  $fetch[$i++] = $symbol;
  }
my %data = $q->fetch('usa',@fetch);


foreach $key ( @fetch ) {
   $price =  $data{$key,'price'};
   $vol = $data{$key,'volume'};
   if ($price ne "") {
      print "x,$key,$dt,$price,0,0,0,0,$vol\n";
      }
  }

