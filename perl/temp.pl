#!/usr/bin/perl 
$database = "/home/samuelf/svn/breeder/build/breeder.sqlite3";

$fileLocked = 1;
while ($fileLocked) {
    # If $database is locked for updating, then $fileLocked > 0.  Check this now: 
    $fileLocked = `fuser -vm $database  2>&1 | grep F | wc -l`;
    chomp($fileLocked);
    # If $fileLocked > 0, then we will issue a warning and wait a bit before trying again.  Otherwise, we will run mysqlString and continue.
    if ($fileLocked > 0) {
        print "run-foldx.pl : The database file $database appears to be locked by ", `fuser -vm $database  2>&1 | grep F | wc -l`, " process. Waiting a bit before trying again.\n";
        print "output of fuser -vm $database :\n";
        print `fuser -vm $database`,"\n";;
        sleep(2); # number of seconds to sleep
    } else {
        print "run-foldx.pl : The database file $database appears to be ready for writing.  Writing now and then moving on..\n";
    }
    print $mysqlString,"\n";
    #system($mysqlString);
}
