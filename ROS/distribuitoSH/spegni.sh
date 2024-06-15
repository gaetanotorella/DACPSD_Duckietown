terminator -e "bash -c 'ssh duckie@db1.local'" &
sleep 10
xdotool type "quackquack"
xdotool key Return
xdotool type "sudo shutdown -h 0"
xdotool key Return
sleep 5

terminator -e "bash -c 'ssh duckie@db2.local'" &
sleep 10
xdotool type "quackquack"
xdotool key Return
xdotool type "sudo shutdown -h 0"
xdotool key Return
sleep 5

terminator -e "bash -c 'ssh duckie@ducklorean.local'" &
sleep 10
xdotool type "quackquack"
xdotool key Return
xdotool type "sudo shutdown -h 0"
xdotool key Return
sleep 5

terminator -e "bash -c 'ssh duckie@duckicha.local'" &
sleep 10
xdotool type "quackquack"
xdotool key Return
xdotool type "sudo shutdown -h 0"
xdotool key Return