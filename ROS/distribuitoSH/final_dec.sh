#!/bin/bash

terminator -m -T MASTER_DISCOVERY -e "bash -c '~/Desktop/distribuitoSH/master/master_db1.sh'" &
sleep 5
xdotool key ctrl+shift+e
sleep 1
xdotool key Alt+Left 
xdotool key ctrl+shift+e
sleep 1

xdotool type "~/Desktop/distribuitoSH/master/master_db2.sh"
xdotool key Return
sleep 5
xdotool key Alt+Right 
xdotool key ctrl+shift+e
sleep 1
xdotool key Alt+Left
sleep 1

xdotool type "~/Desktop/distribuitoSH/master/master_ducklorean.sh"
xdotool key Return
sleep 5
xdotool key Alt+Right 
sleep 1

xdotool type "~/Desktop/distribuitoSH/master/master_duckicha.sh"
xdotool key Return
sleep 5

xdotool key Alt+Left 
xdotool key Alt+Left 
xdotool key Alt+Left
xdotool key ctrl+shift+o

# ROSCORE

xdotool type "~/Desktop/centralizzatoSH/script3.sh"
xdotool key Return
sleep 5
xdotool key Alt+Right
xdotool key ctrl+shift+o 
sleep 1

xdotool type "~/Desktop/centralizzatoSH/script4.sh"
xdotool key Return
sleep 5
xdotool key Alt+Right
xdotool key ctrl+shift+o 
sleep 1

xdotool type "~/Desktop/centralizzatoSH/script5.sh"
xdotool key Return
sleep 5


