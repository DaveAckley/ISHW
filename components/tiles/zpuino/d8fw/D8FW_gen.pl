#!/usr/bin/perl -Tw

use Carp;

my %knownFiles = (
    'debug.log' => \&dumpAll,
    'd8fw.png' => \&makeBoardImage,
    'D8FWPin2Pin.csv' => \&printP2PCSV,
    'D8FWByPin.csv' => \&printNetsByPin,
    'D8FWByFunc.csv' => \&printNetsByFunc,
    'ishw_papilio_pro_top.vhd' => \&generateTopVHD,
    );

use Cwd "abs_path";  # for abs_path
use File::Basename;  # for dirname
my $scriptpath = abs_path($0);
my $scriptdir = dirname($scriptpath);
my $timestamp = scalar(localtime());
my $whoami = getlogin || getpwuid($<) || "unknown";

my %allNames;
sub name {
    my $newName = shift;
    croak "name '$newName' already defined" if isDefined($newName);
    defineName($newName);
}

sub isDefined {
    my $name = shift;
    croak unless defined $name;
    return defined $allNames{$name};
}

sub list {
    my ($name,@args) = @_;
    croak "list(undef,".join(",",@args).")" unless defined $name;
    croak "list($name(not a name),".join(",",@args).")" unless defined $allNames{$name};
    croak "list(".join(",",$name,@args).") already defined" if defined $allNames{$name}->{_LIST};
    $allNames{$name}->{_LIST} = \@args;
}

sub getList {
    my ($name) = shift;
    croak "getList(undef)" unless defined $name;
    croak "list($name(not a name))" unless defined $allNames{$name};
    croak "list($name) undefined" unless defined $allNames{$name}->{_LIST};
    return @{$allNames{$name}->{_LIST}};
}

sub pushList {
    my ($name, $pushVal) = @_;
    pushProp($name,'_LIST',$pushVal);
}

sub props {
    my ($name,@args) = @_;
    croak "props(undef,".join(",",@args).")" unless defined $name;
    croak "props($name(not a name),".join(",",@args).")" unless defined $allNames{$name};
    while (scalar(@args)>1) {
        my $key = shift @args;
        my $val = shift @args;
        if (defined($allNames{$name}->{$key})) {
            if ($val eq $allNames{$name}->{$key}) {
                warn "Duplicate assignment of '$val' as '$key' of '$name'";
            } else {
                croak "Conflicting assignments ('$val' now, '$allNames{$name}->{$key}' before) for '$key' of '$name'";
            }
        } else {
            $allNames{$name}->{$key} = $val;
        }
    }
    warn "Excess argument '$args[0]'" if scalar(@args) > 0;
}


sub defineName {
    my $newName = shift;
    my $d = '$';
    my $a = '@';
    my $b = '\\';
    eval "sub $newName { return expand($newName,${a}_); } ${d}allNames{$newName} = { NAME => ${b}&$newName }";
}

sub expand {
    my ($name,@args) = @_;
    my $argCount = scalar(@args);
    if ($argCount==0) { return $name; }
    if ($argCount==1) {
        my $key = shift @args;
        my $href = $allNames{$name};
        croak "No '$key' in $name" unless defined $href->{$key};
        return $href->{$key};
    }
    croak "UNIMPO expand ".join(",",@args);
}

sub alias {
    my $oldName = shift;
    croak "alias(undef, ..) illegal" unless defined $oldName;
    croak "alias(old, ..) '$oldName' undefined" unless defined $allNames{$oldName};

    my $newName = shift;
    croak "alias(.., undef) illegal" unless defined $newName;

    my $redef = shift;
    croak "alias($oldName,$newName) '$newName' has an alias" 
        if defined $allNames{$newName}->{ALIAS};

    defineName($newName) unless defined $allNames{$newName};
    $allNames{$newName}->{ALIAS} = $oldName;
    pushProp($oldName,'ALIASES',$newName);
}

sub getAliasMatching {
    my ($name,$pattern) = @_;
    my $aliases = getProp($name,'ALIASES');
    return undef unless defined($aliases);
    for my $alias (@{$aliases}) {
        return $alias if $alias =~ /^$pattern$/;
    }
    return undef;
}

sub pushProp {
    my ($name, $prop, $pushVal) = @_;
    push @{$allNames{$name}->{$prop}}, $pushVal;
}


sub prefix {
    my $pref = quotemeta(shift);
    my @results;
    for my $name (sort keys %allNames) {
        push @results, $name if $name =~ /^$pref/;
    }
    return @results;
}
sub pushPropIfNew {
    my ($name, $prop, $pushVal) = @_;
    my $ret = !onListProp($name,$prop,$pushVal);
    if ($ret) {
        push @{$allNames{$name}->{$prop}}, $pushVal
    }
    return $ret;
}

sub onListProp {
    my ($name, $prop, $valToFind) = @_;
    for my $pushval (@{$allNames{$name}->{$prop}}) {
        return 1 if ($pushval eq $valToFind);
    }
    return 0;
}

sub isDefinedProp {
    my ($name,$prop) = @_;
    return defined($allNames{$name}) && defined($allNames{$name}->{$prop});
}
sub getProp {
    my ($name,$prop) = @_;
    my $val = undef;
    $val = $allNames{$name}->{$prop} if isDefinedProp($name,$prop);
    return $val;
}
sub putProp {
    my ($name,$prop,$val) = @_;
    $allNames{$name}->{$prop} = $val;
}

sub assertProp {
    my ($name,$mustHaveProp) = @_;
    # Insist that either $name, or that something reachable via alias
    # of $name, have $mustHaveProp
    my $oname = $name;
    while (1) {
        return if (isDefinedProp($name,$mustHaveProp));
        next if defined($name = getProp($name,'ALIAS'));
        croak "assertProp($oname,$mustHaveProp) failed";
    }
}

sub getAliasesProp {
    my ($oname,$prop) = @_;
    # Find value of $prop in $name or anything reachable via alias of $name
    my @slist = ($oname);
    while (my $name = shift @slist) {
        if (isDefinedProp($name,$prop)) {
            return getProp($name,$prop);
        }
        my $more = getProp($name,'ALIASES');
        if (defined $more) {
            push @slist,@{$more};
        }
    }
    return undef;
}

sub mapColumnSegment {
    my ($column, $topRow, $rowCount, $ppid, $wingid, $firstWingPin, $wingPinIncr) = @_;

    for (my $i = 0; $i < $rowCount; ++$i) {
        my $x = $column;
        my $y = $topRow+$i;
        my $wpin = $firstWingPin+$i*$wingPinIncr;
        my $iname = sprintf("I%X%X",$x,$y);
        my $wpid = sprintf("WING_%s%02d",$wingid,$wpin);
        my $aname = sprintf("%s_%s",$ppid, $wpid);
        alias($iname, $aname);
        props($aname,'PPRO',$ppid,'WING',$wingid,'WINGPIN',$wpid);
    }
}

my $netIds = 0;
my %allNets;
my %allNetsRev;
sub net {
    my @pins = @_;
    croak "net()" if scalar(@pins)==0;
    my $new = 0;
    my %allNetPins;

#    print "\nXXX0 ".join(",",@pins)."\n";

    # assemble all pins now in this net
    for my $p1 (@pins) {
        croak "net(..,$p1(undef),..)" unless isDefined($p1);
        $allNetPins{$p1}++;
#        print "XXX1 ".join(",",sort keys %allNetPins)."\n";
        my $nid = getProp($p1,'_NETLIST');
        next unless defined($nid);
        for my $p2 (@{$allNetsRev{$nid}}) {
            $allNetPins{$p2}++;
        }
#        print "XXX2 ".join(",",sort keys %allNetPins)."\n";
    }
    
    # find or define that net
    my @pinsInNet = sort keys %allNetPins;
    my $nkey = join(",",@pinsInNet);
    my $nid = $allNets{$nkey};
    if (!defined($nid)) {
        my $num = $netIds++;
        $nid = "N".length($num).$num;
        $allNets{$nkey} = $nid;
        $allNetsRev{$nid} = \@pinsInNet;

#        print "XXX3 $nid $nkey\n";
    }

    # smear that netid over all pins
    for my $pin (sort keys %allNetPins) {
        putProp($pin,'_NETLIST',$nid);
    }
}

sub removeUnusedNets {
    my %usedNets;
    for my $name (sort keys %allNames) {
        my $nid = getProp($name,'_NETLIST');
        $usedNets{$nid}++ if defined $nid;
    }
    my @allnids = keys %allNetsRev;
    for my $nid (@allnids) {
        if (!defined($usedNets{$nid})) {
            delete $allNetsRev{$nid};
        }
    }
}

sub dumpNets {
    for my $n (sort keys %allNetsRev) {
        print "$n: ".join(",",@{$allNetsRev{$n}})."\n";
    }
}

sub printNetsByPin {
    printNetsCSV(shift,getList('_IPINS'));
}

sub printNetsByFunc {
    printNetsCSV(shift, sort { 
        my $fa = getAliasMatching($a,'PP.._.._FACE_....') || "\177$a";  # Urgh, make unused..
        my $fb = getAliasMatching($b,'PP.._.._FACE_....') || "\177$b";  # ..pins sort last
        $fa cmp $fb;
    } getList("_IPINS"));
}

sub printNetsCSV {
    my ($handle,@pins) = @_;
    print $handle toCSV('PinId','XWingId','FuncId','NetId','Connections');
    for my $ip (@pins) {
        my $xwid = getAliasMatching($ip,'PP.._WING_...');
        croak "Unwinged pin '$ip'" unless defined $xwid;
        my $fid = getAliasMatching($ip,'PP.._.._FACE_....');
        my $netid;
        my @net;

        if (!defined($fid)) {
            $fid = "/";
            $netid = "/";
        } else {
            $netid = getProp($fid,'_NETLIST');
            @net = @{$allNetsRev{$netid}};
            # Remove ourselves from the connections
            for (my $i = 0; $i < scalar(@net); ++$i) {
                if ($net[$i] eq $fid) {
                    splice(@net, $i, 1);
                    last;
                }
            }
            # Ensure we found ourselves?
        }
        print $handle toCSV($ip,$xwid,$fid,$netid,join(" ",@net));
    }
}

sub printP2PCSV {
    my ($handle) = @_;
    print $handle toCSV('Pin1Id','Pin2Id','LEDId','Purpose');
    for my $ip1 (getList("_IPINS")) {
        my $fid1 = getAliasMatching($ip1,'PP.._.._FACE_....');
        my $ip2;
        my $fid2;
        my $led;
        my $purp;

        if (!defined($fid1)) {
            warn "Warning: '$ip1' unconnected";
            $fid1 = "/";
            $ip2 = "/";
            $fid2 = "/";
            $led = "/";
            $purp = "unconnected";
        } else {
            my $netid = getProp($fid1,'_NETLIST');
            my @net = @{$allNetsRev{$netid}};

            for my $i (@net) {
                if ($i =~ /^LED/) {
                    croak "Two LEDs" if defined $led;
                    $led = $i;
                    next;
                }
                next if ($i =~ /^$fid1$/);
                croak "Two not us ($i)" if defined $fid2;
                $fid2 = $i;
            }
            $led = "/" unless defined $led;
            $ip2 = getProp($fid2,'ALIAS');
            croak "No alias $fid2" unless defined $ip2;
            $purp = "$fid1 <=> $fid2";
        }
        print $handle toCSV($ip1,$ip2,$led,$purp);
    }
}

sub toCSV {
    map { croak "$_" if /"/; } @_;  # Easier than escaping correctly..
    return '"'.join('","',@_).'"'."\n";
}

sub loadConnections {
    doMe("./D8FW_connections.dat");
    removeUnusedNets();
}

sub dumpAll {
    my $fh = shift || STDOUT;
    for my $name (sort keys %allNames) {
        print $fh "$name:";
        my $pref = $allNames{$name};
        for my $prop (sort keys %$pref) {
            my $val = $pref->{$prop};
            next unless defined $val;
            my $type = ref $val;
            next if $type eq 'CODE';
            print $fh " $prop=";
            if (ref($val) eq 'ARRAY') {
                print $fh join(",",@{$val});
            } else {
                print $fh $val;
            }
            print $fh ";";
        }
        print $fh "\n";
    }
}

sub makeBoardImage {
    my $fh = shift;

    use constant PI => 4 * atan2(1, 1);

    require "GD.pm";  # Don't load it unless they want it

    my $titleFont = '/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-BI.ttf';
    my $bodyFont = '/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-B.ttf';

    my $boardWidthMM = 140;
    my $boardHeightMM = 82;

    my $pinSpacingMM = 2.54;

    my %columnToMM = (
        0 => 0,
        1 => 15,
        2 => 68,
        3 => 68+$pinSpacingMM,
        4 => 85,
        5 => $boardWidthMM-$pinSpacingMM
        );
    my %rowBlockToMM = (
        0 => 0,
        1 => 61
        );

    my $width = 1800;
    my $height = 1200;
    my $pixPerMM = 0.95*$width/$boardWidthMM;

    my ($bw,$bh) = ($boardWidthMM*$pixPerMM,$boardHeightMM*$pixPerMM);
    my ($bx,$by) = (($width-$bw)/2,($height-$bh)/2);

    my $im = new GD::Image($width,$height);

    $im->interlaced('true');

    my $white = $im->colorAllocate(255,255,255);
    my $ltgrey = $im->colorAllocate(240,240,240);
    my $dkgrey = $im->colorAllocate(100,100,100);
    my $black = $im->colorAllocate(0,0,0);
    my $red = $im->colorAllocate(255,0,0);
    my $blue = $im->colorAllocate(0,0,255);

    my $bgcolor = $ltgrey;

    # Draw background
    $im->filledRectangle(0,0,$width,$height,$bgcolor);

    # Draw board background
    $im->filledRectangle($bx,$by,$bx+$bw,$by+$bh,$black);

    # Draw finger holes
    my $offMM = 7;
    my @cxMM = ( 40, 110);
    my @cyMM = ( $offMM, $boardHeightMM-$offMM );
    my @rdMM = ( 30, 30);
    for (my $i = 0; $i < scalar(@cxMM); ++$i) {
        $im->filledEllipse($bx+$cxMM[$i]*$pixPerMM,$by+$cyMM[$i]*$pixPerMM,
                           $rdMM[$i]*$pixPerMM,$rdMM[$i]*$pixPerMM,
                           $bgcolor);
    }

    # Draw ipins
    for my $ip (getList("_IPINS")) {
        my $col = getProp($ip,'x');
        my $row = getProp($ip,'y');

        my $colMM = $columnToMM{$col};
        my $rowMM = $rowBlockToMM{int($row/8)}+($row%8)*$pinSpacingMM;
        
        $im->rectangle($bx+$colMM*$pixPerMM,
                       $by+$rowMM*$pixPerMM,
                       $bx+($colMM+$pinSpacingMM)*$pixPerMM,
                       $by+($rowMM+$pinSpacingMM)*$pixPerMM,
                       $dkgrey);
        
        $im->stringFT($white,$bodyFont,14,0,
                      $bx+($colMM+0.2)*$pixPerMM,
                      $by+($rowMM+0.8*$pinSpacingMM)*$pixPerMM,
                      $ip)
            or croak "$@";
    }

    # Draw power/ground blocks for reference
    my @pgCol = ( 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5);
    my @pgRow = ( 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8);
    my @pgYOf = ( 1,-1,-1, 1,-1,-1, 1,-1,-1, 1,-1,-1);
    my @pgXOf = ( 4, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0);

    for (my $i = 0; $i < scalar(@pgCol); ++$i) {
        for (my $box = 0; $box < 4; ++$box) {
            my $col = $pgCol[$i];
            my $row = $pgRow[$i]+$box;

            my $colMM = $columnToMM{$col}+$pgYOf[$i]*$pinSpacingMM;
            my $rowMM = $rowBlockToMM{int($row/8)}+($row%8+$pgXOf[$i])*$pinSpacingMM;

            $im->rectangle($bx+$colMM*$pixPerMM,
                           $by+$rowMM*$pixPerMM,
                           $bx+($colMM+$pinSpacingMM)*$pixPerMM,
                           $by+($rowMM+$pinSpacingMM)*$pixPerMM,
                           $dkgrey);
        }
    }

    # Draw title and timestamp
    $im->stringFT($black,$titleFont,50,0,10,60,"D8FW I pin positions")
        or croak "$@";
    $im->stringFT($black,$bodyFont,12,PI/2,$width-4,$height-4,"Generated $timestamp by $whoami")
        or croak "$@";


    # make sure we are writing to a binary stream
    binmode $fh;

    # Convert the image to PNG and print it on standard output
    print $fh $im->png;
}

sub printUsage {
    my $msg = shift;
    print "\nUsage:\n\t$0 [-h] [[optional/path/]knownfilename]\n\n";
    print "$msg\n" if defined $msg;
}

sub dieUsage {
    printUsage(shift);
    exit 1;
}

sub help {
    my $list = join("\n\t",sort keys %knownFiles);
    dieUsage("knownfilename=\n\t$list\n");
}

sub main {
    my @args = @_;


    my $havecon = 0;
    while (my $ofile = shift @args) {
        help() if $ofile =~ /-h|-help|--help/;
        
        if ($ofile !~ m!\A((.*)/)?([\w.]+)\z!) {
            croak "Bad char(s) in arg '$ofile'";
        }
        my ($optdir,$file) = ($2 || ".",$3);
        loadConnections() if !$havecon++;
        
        my $cref = $knownFiles{$file};
        dieUsage("'$file' unrecognized")
            unless defined $cref;

        my $filename = "$optdir/$file";
        open my $fh, '>', $filename
            or dieUsage("$filename: $!");
        $cref->($fh);
        close $fh or croak "$!";

        print "[Wrote $filename]\n";
    }

    help() unless $havecon;
}

##################
##  Generating top

my $topTemplate = "ishw_papilio_pro_top.vhd.dat";
my $loadedTopTemplate = 0;
my @topVHDPieces;
#my @ishwSlaveSlots = ('over');
#my @ishwMasterSlots = ('ride', 'me');
#my @ishwAllSlots;

sub addPiece {
    push @topVHDPieces, @_;
}

sub generateTopVHD {
    my ($handle) = @_;
    if (!$loadedTopTemplate) {
        doMe("./$topTemplate");
#        @ishwAllSlots = (@ishwSlaveSlots,@ishwMasterSlots);
#        croak "Bad slots: '".join(",",@ishwAllSlots)."'" unless scalar @ishwAllSlots == 8;
        $loadedTopTemplate = 1;
    }
    for my $piece (@topVHDPieces) {
        generateTopVHDPiece($handle, $piece);
    }
}

sub findISHW {
    my $path = shift;
    $path = $1 if $path =~ m!/(ISHW/.*?\z)!;
    return $path;
}

sub removeBlocks {
    my ($str,$kref) = (@_);
    $str =~ s'@\[([^@:]+):(.*?):\1\]@\n?'$kref->{$1}?"$2":""'ges;  # GAH'
    return $str;
}

sub mapCase {
    my ($toMap,$basedOn) = @_;
    $basedOn =~ s/@//g;

    # Mixed case values with ALL CAP keys are untouched
    if ($toMap =~ /[A-Z]/ && $toMap =~ /[a-z]/ && $basedOn =~ /^[A-Z]+$/) {
        return $toMap;
    }

    my @to = split(//,$toMap);
    my @from = split(//,$basedOn);
    for (my $t = 0; $t < scalar(@to); ++$t) {
        my $f = $t;
        $f = scalar(@from)-1 if $f >= scalar(@from);
        my $fch = $from[$f];
        if ($fch =~ /[a-z]/) {
            $to[$t] = lc($to[$t]);
        } elsif ($fch =~ /[A-Z]/) {
            $to[$t] = uc($to[$t]);
        }
    }
    return join("",@to);
}

sub generateTopVHDPins {
    my $ret = "";

    my $pc = -1;
    for my $wingpin (getList("_WINGPINS")) {
        ++$pc;
        my $padpc = (($pc<10)?"0":"").$pc;
        
        my $wing = getProp($wingpin,'wing');
        my $wpin = getProp($wingpin,'wpin');
        my $function = getAliasMatching($wingpin,'.._FACE_....');
        my $decl;
        if (!defined($function)) {
            $decl = "

  -- pin$padpc is GPIO
  pin$padpc: IOPAD port map(I => gpio_o($pc),O => gpio_i($pc),T => gpio_t($pc),C => sysclk,PAD => WING_$wing($wpin) );\n";
        } else {
            my $func = getProp($function,'facefunc');
            my $dir = getProp($func,'dir');
            my $sig = getProp($function,'signal');
            croak unless defined $sig;
            if ($dir eq 'input') {
                $decl = "\n  $sig <= WING_$wing($wpin);   -- pin$padpc: ISHW input; no IPAD sync";
            } else {
                $decl = "
  pin$padpc: OPAD port map(I => $sig,PAD => WING_$wing($wpin) );  -- ISHW output";
            }
        }
        $ret .= $decl;
    
    }
    return $ret;
}

sub getSignalName {
    my ($face, $func) = @_;
    my $id = uc($face."_FACE_".$func);
    my $sig = getProp($id,'signal');
    croak "No signal for ($face, $func)" unless defined $sig;
    return $sig;
}

sub substituteTopVHDVariable {
    my ($key,$face,$fcode,$slot,$isMaster) = @_;

    # Various large custom pieces
    return generateTopVHDPins() if uc($key) eq '@GENPADS@';

    # Various 'simple substitutions
    return mapCase($timestamp,$key) if uc($key) eq '@DATE@';
    return mapCase($whoami,$key) if uc($key) eq '@WHOAMI@';
    return $scriptpath if uc($key) eq '@FILENAME@';
    return findISHW("$scriptdir/$topTemplate") if uc($key) eq '@TEMPLATEPATH@';

    return "" if uc($key) eq '@BYFACE@';
    return "" if uc($key) eq '@BYSLOT@';

    # Remaining keys require being in an iteration

    if (defined($face)) {
        my $name = getProp($face,'name');
        return mapCase($face,$key) if uc($key) eq '@FACE@';
        return mapCase($name,$key) if uc($key) eq '@FACENAME@';

        return $fcode if uc($key) eq '@FACENUM@';
        return $slot if uc($key) eq '@SLOT@';

        return getSignalName($face,$1) if uc($key) =~ m'@FACESIG:([^@]+)@';
    }

    croak "Substitution '$key' unknown or used improperly (outside iteration perhaps?)";

    return $key;
}

sub substituteTopVHDVariables {
    my ($text,$face,$fcode,$slot,$isMaster) = @_;

    $text =~ s'(@[^@]+@)\n?'substituteTopVHDVariable($1,$face,$fcode,$slot,$isMaster)'ge; #GAH' 
    return $text;                                                  
}

my %faceMultiplierSubs = (
    '@FACE@' =>    0b11111111,
    '@FACENUM@' => 0b11111111,
    '@SFACE@' =>   0b00001111,
    '@SFACENUM@' =>0b00001111,
    '@MFACE@' =>   0b11110000,
    '@MFACENUM@' =>0b11110000,
    );
my $faceMultiplierRegex = '('.join("|",keys %faceMultiplierSubs).')';

sub expandFacePiece {
    my ($face,$tmpl) = @_;
    my @faceCodes = getList(_DIRS);
    my $code = $faceCodes[$face];

    my $isMaster = getProp($code,'isMaster');
    my $slot = getProp($code,'slot');
    my %keys;
    for (my $kf = 0; $kf < 8; ++$kf) {
        $keys{$code} = ($kf==$face);  # select on face
    }
    $keys{M} = $isMaster;
    $keys{S} = !$isMaster;

    my $subs = removeBlocks($tmpl,\%keys);
    $subs = substituteTopVHDVariables($subs,$code,$face,$slot,$isMaster);
    return $subs;
}

sub generateTopVHDPiece {
    my ($handle,$piece) = @_;

    # Just barf totally literal pieces and beat it
    return print $handle $piece if $piece !~ m'@';

    # Detect if we are doing a FACE or SLOT iteration
    if ($piece =~ m'@(BYFACE|BYSLOT)@') {
        my $type = $1;
        croak "Unimplemented $1" unless $type eq 'BYFACE';

        # We are
        my $tmpl = $piece;

        $piece = "";
        for (my $f = 0; $f < 8; ++$f) {
            $piece .= expandFacePiece($f,$tmpl);
        }
    }        

    # Finally, look for 'simple' substitutions
    $piece = substituteTopVHDVariables($piece);

    print $handle $piece;
}

##################
##  Load files late, so all 'global lexicals' are in scope

sub doMe {
    my $file = shift;
    my $content;
    open FILE, '<', $file
        or croak "$file: $!";
    { local $/ = undef; $content = <FILE>; }
    close FILE or croak "$file: $!";
    $content =~ /(^.*$)/s;
    $content = $1;              # Gah.

    unless (my $return = eval $content) {
        croak "couldn't parse $file: $@" if $@ ne "";
        croak "couldn't eval $file: $!"    unless defined $return;
        croak "couldn't run $file"       unless $return;
    }
    print STDERR "[Loaded $file]\n";
}

# Begin block to load names and aliases early, so we can later parse
# them without quotes.

BEGIN {
    my $file = "./D8FW_names.dat";
    doMe($file);
}

##################
##  Running the whole show

main(@ARGV);

exit 0;

