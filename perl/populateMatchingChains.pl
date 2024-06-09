#! /usr/bin/perl
#
#
#
#
#
print " Usage: ./populateMatchingChains.pl [options] \n";
print " options include:\n";
print "     --sequence <sequence in single letter code> \n";
print "     --PDB <PDB ID> \n";
print "     --chain <chain ID> \n";
print ;
$sequence = "FPTIPLSRLFDNAMLRAHRLHQLAFDTYQEFEEAYIPKEQKYSFLQNPQTSLCFSESIPTPSNREETQQKSNLELLRISLLLIQSWLEPVQFLRSVFANSLVYGASDSNVYDLLKDLEEGIQTLMGRLEDGSPRTGQIFKQTYSKFDDALLKNYGLLYCFRKDMDKVETFLRIVQCRSVEGSCG";
$fastaExecutable = "./fasta_lwp.pl";
$inPDB = "1A22";
$inChain = "A";
#system("$fastaExecutable --email samuelfloresc@gmail.com --program ssearch --database pdb --stype protein --expupperlim 0.00000000001 --sequence $sequence ");

$resultFile = "./fasta-R20160916-111259-0494-72451549-oy.out.txt";
$parseResultCommand = "grep '>>PDB:' $resultFile | awk '{print \$1}' ";
#print $parseResultCommand,"\n";
#system($parseResultCommand);
@resultsArray = `$parseResultCommand`;
int i; $i = 0;
foreach(@resultsArray){
    #print "element: $_\r\n";
    @PDB[$i] = substr($_,6,4);
    @chain[$i] = substr($_,11,1);
    chomp($PDB[$i]);
    chomp($chain[$i]);
    #print $PDB[$i],",",$chain[$i],"\n";
    if ($inPDB eq $PDB[$i]) {print  "# ",$inPDB," ==  ",$PDB[$i]," .. Doing nothing. \n";}
    elsif ($inPDB lt $PDB[$i] ) {
        print "insert into matchingChains (PDB1, PDB2, chain1, chain2) VALUES ('$inPDB','$PDB[$i]','$inChain','$chain[$i]'); \n";
    } elsif ($inPDB gt $PDB[$i] ) {
        print "insert into matchingChains (PDB1, PDB2, chain1, chain2) VALUES ('$PDB[$i]','$inPDB','$chain[$i]','$inChain'); \n";
    } else {print "# Somethign is very wrong!\n"; exit ;}
    $i++;
}
