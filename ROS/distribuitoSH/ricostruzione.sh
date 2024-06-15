#!/bin/bash

terminator -m -T RICOSTRUZIONE -e "bash -c '~/Desktop/distribuitoSH/ricostruzione/ric_db1.sh'" &
sleep 2
xdotool key ctrl+shift+o
sleep 2

xdotool type "~/Desktop/distribuitoSH/ricostruzione/ric_db2.sh"
xdotool key Return
xdotool key Alt+Up
xdotool key ctrl+shift+e 
sleep 2

xdotool type "~/Desktop/distribuitoSH/ricostruzione/ric_ducklorean.sh"
xdotool key Return
xdotool key Alt+Down
xdotool key ctrl+shift+e 
sleep 2

xdotool type "~/Desktop/distribuitoSH/ricostruzione/ric_duckicha.sh"
xdotool key Return




