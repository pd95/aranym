#!/usr/bin/perl
# Read link*.h to generate ptr*.h

$file = @ARGV[0];

if ( ! defined(open(FILE, $file)) ) {
	warn "Couldn't open $file: $!\n";
	exit;
}

print "/* Generated by pointers-gen.pl from $file */\n\n";

$linecount=0;
while ($ligne = <FILE>) {
	# Ignore commented commands
	if ($ligne =~ m=.*/\*.*=) {
		while ($ligne !~ m=.*\*/.*=) {
			$ligne = <FILE>;
		}
		$ligne = <FILE>;
	}

	if ($ligne =~ /\)\;$/ ) {
		# Remove start tabulation and quote
		$ligne =~ s/\t\"//;
		# Add (xxxx) around function name
		$ligne =~ s/(gl\w+) *\(/\(\*$1\)\(/;
		# Add (xxxx) around function name
		$ligne =~ s/(OSMesa\w+) *\(/\(\*$1\)\(/;	
		# Remove context parameter
		$ligne =~ s/Uint32 ctx *, *//;	
		$ligne =~ s/Uint32 ctx//;	
		# Add void if necessary
		$ligne =~ s/\(\)/(void)/;	
		# Replace many spaces by one
		$ligne =~ s/ +/ /g;	

		print $ligne;
		$linecount++;
	}
}
close(FILE);
print "/* Functions generated : $linecount */\n";