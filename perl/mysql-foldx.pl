#!/usr/bin/perl 
use Time::localtime;
print "syntax ./run-foldx.pl  <main directory>  <mutation string C:RRR:M where C = chain, RRR is residue ID (with insertion code), M is mutant residue type. concatenate mutations with '.', , e.g. C:130:L.C:149:T> \n";
#print "syntax ./predict-affinity.pl  <main directory>  <mutated sequence in single letter code> \n";
#if (! ($ARGV[0])) {exit (1);}
if (! ($ARGV[1])) {exit (1);}
if ( ($ARGV[2])) {exit (1);}

$firstResidue = 124;
$lastResidue = 167;
$includeRadius = 1.2;
$mainDirectory = $ARGV[0];
$mutationString = $ARGV[1];
chdir("$mainDirectory") || die ("predict-affinity.pl : couldn't chdir to $mainDirectory.  Perhaps the directory has not been created?\n");

$jobName = $mutationString; #// ""; #$mutationChain[0]."_".$mutationPosition[0]."_".$mutantResidueType[0];
#system("/bubo/home/h17/samuelf/local/FoldX.linux64 -runfile $mainDirectory/runfile.txt > $mainDirectory/$jobName/foldx.$jobName.out ");
$foldx_energy = `grep Total $mainDirectory/$jobName/foldx.$jobName.out | grep = | tail -1 | awk \'\{print \$3\}\' `;
chomp($foldx_energy);
if (length($foldx_energy) == 0) {
	print "foldx_energy has zero length!\n";
	exit(1);	
}
system("grep Total $mainDirectory/$jobName/foldx.$jobName.out | grep = | tail -1 | awk \'\{print \$3\}\' > $mainDirectory/$jobName/foldx_energy.$jobName");
#system("echo \"use samuelf; update results set foldx_energy = $foldx_energy, status = \\\"completed\\\" where mutationString = \\\"$mutationString\\\";\"   > $mainDirectory/$jobName/$jobName.mysql");
$tm=localtime;
my ($day,$month,$year)=($tm->mday,$tm->mon,$tm->year);
$year += 1900;
$month += 1;
$mysqlString = "echo \"use samuelf; update results set foldx_energy = $foldx_energy, status = \\\"completed\\\", jobName = \\\"$mainDirectory\\\" , date = \\\"$year-$month-$day\\\"  where mutationString = \\\"$mutationString\\\";\" | /usr/bin/mysql -h mysql.uppmax.uu.se -u samuelf -poirfy235k4ys &>  $mainDirectory/$jobName/$jobName.mysql ";
print $mysqlString,"\n";
system($mysqlString);
#system("echo \"use samuelf; update results set foldx_energy = $foldx_energy, status = \\\"completed\\\", jobName = \\\"$mainDirectory\\\" , date = \\\"$year-$month-$day\\\"  where mutationString = \\\"$mutationString\\\";\" | /usr/bin/mysql -h mysql.uppmax.uu.se -u samuelf -poirfy235k4ys ");
