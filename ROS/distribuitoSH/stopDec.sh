cd ~/catkin_ws
source devel/setup.bash
export ROS_HOSTNAME=ubuntu.local



#ARRESTA TUTTE LE MACCHINE

echo 'Metto a 0 la velocità di [db1]'
rostopic pub /db1/wheels_driver_node/wheels_cmd duckietown_msgs/WheelsCmdStamped "{'header': {'seq': 0, 'stamp': { 'secs': 0, 'nsecs': 0}, 'frame_id': ''}, 'vel_left': 0.0, 'vel_right': 0.0}"

echo 'Metto a 0 la velocità di [db2]'
rostopic pub /db2/wheels_driver_node/wheels_cmd duckietown_msgs/WheelsCmdStamped "{'header': {'seq': 0, 'stamp': { 'secs': 0, 'nsecs': 0}, 'frame_id': ''}, 'vel_left': 0.0, 'vel_right': 0.0}"

echo 'Metto a 0 la velocità di [ducklorean]'
rostopic pub /ducklorean/wheels_driver_node/wheels_cmd duckietown_msgs/WheelsCmdStamped "{'header': {'seq': 0, 'stamp': { 'secs': 0, 'nsecs': 0}, 'frame_id': ''}, 'vel_left': 0.0, 'vel_right': 0.0}"

echo 'Metto a 0 la velocità di [duckicha]'
rostopic pub /duckicha/wheels_driver_node/wheels_cmd duckietown_msgs/WheelsCmdStamped "{'header': {'seq': 0, 'stamp': { 'secs': 0, 'nsecs': 0}, 'frame_id': ''}, 'vel_left': 0.0, 'vel_right': 0.0}"


