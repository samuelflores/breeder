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
$SQLExecutable = " /usr/bin/mysql -h rembrandt.bmc.uu.se -u mmbadmin -D mmb -pMMB37xYstRhggY78432 --skip-column-names";
$database = "mmb";
# We are disregarding entries with wildTypeString = NULL because those are old ones, from outside the interface
@residueNumbers =  `echo \"select mutationString from results where jobName = \'$jobName\' and NOT ISNULL(wildTypeString) and NOT wildTypeString like \'%\.%\';\"  | $SQLExecutable $database | awk \'{print substr(\$1,3,length(\$1)-4);  }\' | sort -g | uniq `;
#print "check 1\n";
for (my $i = 0 ; $i < @residueNumbers ; $i++) {
    chomp($residueNumbers[$i]);
    #print $residueNumbers[$i], "\n";
}
my @wildTypeResidueNumbers;
for (my  $k = 0; $k < (@residueNumbers); $k++) {
    #system("echo \"select SUBSTRING(wildTypeString, CHAR_LENGTH(wildTypeString),1) from results where jobName = \'$jobName\' and mutationString like \'%-$residueNumbers[$k]-%\' order by wildTypeString desc  limit 1;\" ");
    #  NOT  mutationString like \'%\\\.%\' removes multiple mutants
    $wildTypeResidueTypes[$k] = `echo \"select SUBSTRING(wildTypeString, CHAR_LENGTH(wildTypeString),1) from results where jobName = \'$jobName\' and mutationString like \'%-$residueNumbers[$k]-%\' and NOT  mutationString like \'%\\\.%\'  order by wildTypeString desc  limit 1;\" | $SQLExecutable $database   `;
    #$wildTypeResidueTypes[$k] = `echo \"select wildTypeString from results where jobName = \'$jobName\' and mutationString like \'%-$residueNumbers[$k]-%\' order by wildTypeString desc  limit 1;\" | $SQLExecutable $database | awk \'{print substr(\$1,length(\$1),1) ; }\'  `;
    chomp( $wildTypeResidueTypes[$k]);
    #print "check 2\n";
}
$myIncrement =  0.0;
print  "Substitution\t";

for (my $i = 0; $i < @aminoAcids; $i++) {
    print "$aminoAcids[$i]\t";
}

for (my $j = 0; $j < @residueNumbers; $j++) {
    # print "check 3\n";
    print  "\n$wildTypeResidueTypes[$j]", ($residueNumbers[$j]-119), "\t";
    #print  "\n$wildTypeResidueTypes[$j]$residueNumbers[$j]"\t";
    for (my $i=0; $i <=19; $i++){
        $queryString = "select (foldx_energy - foldx_energy_wild_type + ($i *  $myIncrement))  as diff from results where mutationString like \'%-$residueNumbers[$j]\-$aminoAcids[$i]\'    and jobName = \'$jobName\'; ";
        #print $queryString,"\n";
        #$queryString = "select (foldx_energy - foldx_energy_wild_type + ($i *  $myIncrement))  as diff from results where mutationString like \'%-$residueNumbers[$j]\-$aminoAcids[$i]\'  and NOT mutationString like \'%\\.%\'  and jobName = \'$jobName\'; ";
        #print $queryString,"\n";
        $queryResult = `echo \"$queryString\"  | $SQLExecutable $database | tail -1`;

        chomp($queryResult);
        if (($queryResult eq "") || ($queryResult eq "NULL")) {$queryResult = '10000';} # for missing data, use an extremely large number.
        print "$queryResult\t";
    } print "";
}
