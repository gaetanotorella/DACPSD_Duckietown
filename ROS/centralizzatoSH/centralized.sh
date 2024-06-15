#!/bin/bash

# Avvia un terminale con tre terminali all'interno, ognuno eseguendo uno script diverso

terminator -e "bash -c './script1.sh'" &
sleep 5
xdotool key Super_L+Up
xdotool key ctrl+shift+o 
sleep 1

xdotool type "~/Desktop/centralizzatoSH/script2.sh"
xdotool key Return
sleep 5
xdotool key Alt+Up
xdotool key ctrl+shift+e 
sleep 1

xdotool type "~/Desktop/centralizzatoSH/script3.sh"
xdotool key Return
sleep 5
xdotool key Alt+Down
xdotool key ctrl+shift+e 
sleep 1

xdotool type "~/Desktop/centralizzatoSH/script4.sh"
xdotool key Return
sleep 5
xdotool key ctrl+shift+e 
sleep 1

xdotool type "~/Desktop/centralizzatoSH/script5.sh"
xdotool key Return
sleep 5


