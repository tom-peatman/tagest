#!/bin/bash
# plots eez numbers in 1 degree cells using the gmt_eezmap.txt
# generated by eezmap
#
if [ $# -lt 1 ]; then
   echo "Usage: eezmap.sh prnfile"
   echo "       for example, eezmap.sh grid16"
   exit 0
fi

REGION=-R115/195/-20/15
echo $REGION
PROJECTION=-JM9i
echo $PROJECTION
MAP=$1
echo $MAP
EPS=$MAP.eps
echo $EPS
rm -v $EPS
rm -v $MAP.png

EEZ_COLOR=1P/0/0/255
WET_COLOR=194/241/253
DRY_COLOR=237/214/151
SHORELINE=154/139/98
REG_COLOR=0/128/0
DOM_COLOR=$REG_COLOR

gmtset ANOT_FONT_SIZE_PRIMARY 12 BASEMAP_TYPE fancy PAGE_ORIENTATION landscape HEADER_FONT_SIZE 16 HEADER_FONT 1 MEASURE_UNIT inch PLOT_DEGREE_FORMAT dddF

# draw fancy base map with messy 1 deg grid
psbasemap -B10f5g1:.$1: $REGION $PROJECTION -V    -K > $EPS
# draw fancy base map with messy 1/2 deg grid
# psbasemap -B10f5g0.5:.$1: $REGION $PROJECTION -V    -K > $EPS

#draw coastlines with wet and dry areas filled
#gmt pscoast $REGION $PROJECTION -Di -G$DRY_COLOR -W$SHORELINE -V -O    >> $EPS
pscoast $REGION $PROJECTION -Di -G$DRY_COLOR  -Wfaint,$SHORELINE -V -O -K >> $EPS

#draw EEZs
# beware of hard coded path to eez.dat
gawk '{if (NF != 2){print ">"}else{print $0}}' ../eez.dat | psxy -m $REGION $PROJECTION -A -W$EEZ_COLOR -V -O -K >> $EPS

#
# draw EEZ numbers
pstext gmt_eezmap.txt $REGION $PROJECTION $REG_COLOR -V -O    >> $EPS

#
#rm  -v .gmt*
gv $EPS &
ps2raster -V -A -Tg -E600 $EPS
eog $MAP.png &
