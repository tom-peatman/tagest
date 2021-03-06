#!/usr/bin/gawk -f
# ARGV[1] par file name
# ARGV[2] ipar number to change
# ARGV[3] new value of ipar
BEGIN{
# print "# ",ARGV[0],ARGV[1],ARGV[2],ARGV[3],ARGV[4]
  flags=0
  fline = 0
  file = ARGV[1]
  ipar = ARGV[2]
  value = ARGV[3]
  line = int(ipar/10) + 1
  field = ipar - 10*(line-1)
#  print "# Looking for ipar "ipar" in file " file" in field "field" of ipar line "line

  # to prevent awk from interpreting other args as files to process
  ARGC = 2
# exit
}
{
  if ($2 == "flags")
  {
     flags = 1
  }
  if (flags == 1 && $1 != "#" && fline < 10)
  {
     fline = fline + 1
     if (fline == line)
     {
     #  print(fline": "$0)
        $field = value
     #  print(fline": "$0)
     }
  }
  print $0
}

## oldversion did not work for different number of rows in gridmap
## ARGV[1] par file name
## ARGV[2] line number of ipar to change
## ARGV[3] field number of ipar to change
## ARGV[4] new value of ipar
#BEGIN{
## print "# ",ARGV[0],ARGV[1],ARGV[2],ARGV[3],ARGV[4]
#  line = ARGV[2]
#  field = ARGV[3]
#  value = ARGV[4]
#  # to prevent awk from interpreting other args as files to process
#  ARGC = 2
#}
#
#{
##  if (NR == 3)
##  {
##     print "# file generated by",ARGV[0],ARGV[1],ARGV[2],ARGV[3],ARGV[4]
##  }
#   if (NR == line) #ARGV[2])
#   {
#   #   $ARGV[3] = ARGV[4]
#       $field = value
#   }
#   print $0
#} 
#
##END{
#  #print "in END"
##}
