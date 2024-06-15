#!/bin/bash
terminator -m -T CONTROLLO -e "bash -c '~/Desktop/distribuitoSH/controllo/controllo_db1.sh'" &
sleep 2
xdotool key ctrl+shift+o
sleep 2

xdotool type "~/Desktop/distribuitoSH/controllo/controllo_db2.sh"
xdotool key Return
xdotool key Alt+Up
xdotool key ctrl+shift+e 
sleep 2

xdotool type "~/Desktop/distribuitoSH/controllo/controllo_ducklorean.sh"
xdotool key Return
xdotool key Alt+Down
xdotool key ctrl+shift+e 
sleep 2

xdotool type "~/Desktop/distribuitoSH/controllo/controllo_duckicha.sh"
xdotool key Return




