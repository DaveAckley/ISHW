#!/usr/bin/perl -Tw

# print an N-way arbiter to stdout

sub printUsage {
    my $msg = shift;
    print "\nUsage:\n\t$0 N\n\n";
    print "$msg\n" if defined $msg;
}

sub dieUsage {
    printUsage(shift);
    exit 1;
}

dieUsage if scalar(@ARGV) != 1;

dieUsage "Not a number" unless $ARGV[0] =~ /^([1-9][0-9]*)$/;

my $count = $1;
my $countm1 = $count-1;
my $timestamp = scalar(localtime());
my $whoami = getlogin || getpwuid($<) || "unknown";

print <<END_OF_PIECE;
-- 
-- DO NOT HAND EDIT!  This file was generated by ${whoami} on ${timestamp} via:
-- $0 $ARGV[0]
--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;
library work;
use work.zpu_config.all;

entity wbarb${count}_1 is
  generic (
    ADDRESS_HIGH: integer := maxIObit;
    ADDRESS_LOW: integer := minIObit
  );
  port (
    wb_clk_i: in std_logic;
	 	wb_rst_i: in std_logic;
END_OF_PIECE

for (my $i = 0; $i < $count; ++$i) {
    print <<END_OF_PIECE;

    -- Master $i signals

    m${i}_wb_dat_o: out std_logic_vector(31 downto 0);
    m${i}_wb_dat_i: in std_logic_vector(31 downto 0);
    m${i}_wb_adr_i: in std_logic_vector(ADDRESS_HIGH downto ADDRESS_LOW);
    m${i}_wb_sel_i: in std_logic_vector(3 downto 0);
    m${i}_wb_cti_i: in std_logic_vector(2 downto 0);
    m${i}_wb_we_i:  in std_logic;
    m${i}_wb_cyc_i: in std_logic;
    m${i}_wb_stb_i: in std_logic;
    m${i}_wb_stall_o: out std_logic;
    m${i}_wb_ack_o: out std_logic;
END_OF_PIECE
}
print <<END_OF_PIECE;

    -- Slave signals

    s0_wb_dat_i: in std_logic_vector(31 downto 0);
    s0_wb_dat_o: out std_logic_vector(31 downto 0);
    s0_wb_adr_o: out std_logic_vector(ADDRESS_HIGH downto ADDRESS_LOW);
    s0_wb_sel_o: out std_logic_vector(3 downto 0);
    s0_wb_cti_o: out std_logic_vector(2 downto 0);
    s0_wb_we_o:  out std_logic;
    s0_wb_cyc_o: out std_logic;
    s0_wb_stb_o: out std_logic;
    s0_wb_ack_i: in std_logic;
    s0_wb_stall_i: in std_logic
  );
end entity wbarb${count}_1;



architecture behave of wbarb${count}_1 is

signal current_master : integer range 0 to ${countm1};
signal next_master : integer range 0 to ${countm1};
begin

process(wb_clk_i)
begin
  if rising_edge(wb_clk_i) then
    if wb_rst_i='1' then
      current_master <= 0;
    else
      current_master <= next_master;
    end if;
  end if;
end process;

END_OF_PIECE
my $pargs = "";
for (my $i = 0; $i < $count; ++$i) {
    $pargs .= ", m${i}_wb_cyc_i";
}
print <<END_OF_PIECE;

process(current_master$pargs)
begin
  next_master <= current_master;

  case current_master is
END_OF_PIECE
for (my $i = 0; $i < $count; ++$i) {
    my $ip1 = ($i+1)<$count?$i+1:0;
print <<END_OF_PIECE;
    when $i =>
      if m${i}_wb_cyc_i='0' then
        if m${ip1}_wb_cyc_i='1' then
          next_master <= $ip1;
        end if;
      end if;
END_OF_PIECE
}
print <<END_OF_PIECE;
    when others =>
  end case;
end process;

-- Muxers for slave

process(current_master,
END_OF_PIECE
for (my $i = 0; $i < $count; ++$i) {
    print ",\n" if $i>0;
    print "        m${i}_wb_dat_i, m${i}_wb_adr_i, m${i}_wb_sel_i, m${i}_wb_cti_i, m${i}_wb_we_i, m${i}_wb_cyc_i, m${i}_wb_stb_i";
}
print ")\n";
print <<END_OF_PIECE;
begin
  case current_master is
END_OF_PIECE

for (my $i = 0; $i < $count; ++$i) {
    my $ip1 = ($i+1)<$count?$i+1:0;
    print <<END_OF_PIECE;
    when $i =>
      s0_wb_dat_o <= m${i}_wb_dat_i;
      s0_wb_adr_o <= m${i}_wb_adr_i;
      s0_wb_sel_o <= m${i}_wb_sel_i;
      s0_wb_cti_o <= m${i}_wb_cti_i;
      s0_wb_we_o  <= m${i}_wb_we_i;
      s0_wb_cyc_o <= m${i}_wb_cyc_i;
      s0_wb_stb_o <= m${i}_wb_stb_i;
END_OF_PIECE
}
print <<END_OF_PIECE;
    when others =>
      null;
  end case;
end process;

-- Muxers/sel for masters

END_OF_PIECE
for (my $i = 0; $i < $count; ++$i) {
    print <<END_OF_PIECE;
m${i}_wb_dat_o <= s0_wb_dat_i;
END_OF_PIECE
}
print <<END_OF_PIECE;

-- Ack

END_OF_PIECE
for (my $i = 0; $i < $count; ++$i) {
    my $ip1 = ($i+1)<$count?$i+1:0;
    print <<END_OF_PIECE;
m${i}_wb_ack_o <= s0_wb_ack_i when current_master=$i else '0';
m${i}_wb_stall_o <= s0_wb_stall_i when current_master=$i else '1';
END_OF_PIECE
        }
print <<END_OF_PIECE;

end behave;
END_OF_PIECE