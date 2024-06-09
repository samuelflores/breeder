#!/usr/bin/perl
#print "Usage : ./multiScanning.pl <jobName>\n";
@aminoAcids = ("A","V","L","I","F","W","M","P","D","E","G","S","T","C","Y","N","Q","K","R","H");
$jobName = $ARGV[0];#"GH-high-aff-site-vs-GHR";
#print "You are asking to generate a data file for jobName = >$jobName<.\n";
if ((length $jobName ) == 0) {
    print " Zero length jobName! \n";
    exit;
}
$SQLExecutable = "/sw/apps/sqlite/3.8.5/tintin/bin/sqlite3";
$database = "/home/samuelf/svn/breeder/build/breeder.sqlite3";
@residueNumbers =  `echo \"select mutationString from results where jobName = \'$jobName\' ;\"  | $SQLExecutable $database | awk \'{print substr(\$1,3,length(\$1)-4);  }\' | sort -g | uniq `;
for (my $i = 0 ; $i < @residueNumbers ; $i++) {
    chomp($residueNumbers[$i]);
}
my @wildTypeResidueNumbers;
for (my  $k = 0; $k < (@residueNumbers); $k++) {
    $wildTypeResidueTypes[$k] = `echo \"select wildTypeString from results where jobName = \'$jobName\' and mutationString like \'%-$residueNumbers[$k]-%\' order by wildTypeString desc  limit 1;\" | $SQLExecutable $database | awk \'{print substr(\$1,length(\$i)) ; }\'  `;
}
$myIncrement =  0.0;
print  "Substitution\t";
for (my $i = 0 ; $i < @residueNumbers ; $i++) {
    chomp($wildTypeResidueTypes[$i]);
}

for (my $i = 0; $i < @aminoAcids; $i++) {
    print "$aminoAcids[$i]\t";
}

for (my $j = 0; $j < @residueNumbers; $j++) {
    print  "\n$wildTypeResidueTypes[$j]$residueNumbers[$j]\t";
    for (my $i=0; $i <=19; $i++){
        $queryString = "select (foldx_energy - foldx_energy_wild_type + ($i *  $myIncrement))  as diff from results where mutationString like \'%-$residueNumbers[$j]\-$aminoAcids[$i]\'  and jobName = \'$jobName\'; ";

        # If we're running with  PRAGMA journal_mode=WAL;, then no need to check for file locking.  database can always be opened for reading!
	$fileLocked = 0;
	while ($fileLocked) {
	    # If $database is locked for updating, then $fileLocked > 0.  Check this now: 
	    $fileLocked = `fuser -vm $database  2>&1 | grep F | wc -l`;
	    chomp($fileLocked);
	    # If $fileLocked > 0, then we will issue a warning and wait a bit before trying again.  Otherwise, we will run queryString and continue.
	    if ($fileLocked > 0) {
		print STDERR "run-foldx.pl : The database file $database appears to be locked. Waiting a bit before trying again.\n";
		print STDERR "output of fuser -vm $database :\n";
		print STDERR `fuser -vm $database`,"\n";;
		sleep(2); # number of seconds to sleep
	    } else {
		#print "run-foldx.pl : The database file $database appears to be ready for writing.  Writing now and then moving on..\n";
	    }
	    #print $queryString,"\n";
	    # system($queryString);
            $queryResult = `echo \"$queryString\"  | $SQLExecutable $database `;
	}

        $queryResult = `echo \"$queryString\"  | $SQLExecutable $database `;

        chomp($queryResult);
        if ($queryResult eq "") {$queryResult = '10000';} # for missing data, use an extremely large number.
        print "$queryResult\t";
    }
}
