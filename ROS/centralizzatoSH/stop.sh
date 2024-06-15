#METTI VELOCITA' A 0.2 PER ENTRAMBI

cd ~/catkin_ws
source devel/setup.bash
export ROS_HOSTNAME=ubuntu.local


#echo 'Metto a 0.2 la velocità di [db1]'
#rostopic pub /db1/wheels_driver_node/wheels_cmd duckietown_msgs/WheelsCmdStamped "{'header': {'seq': 0, 'stamp': { 'secs': 0, 'nsecs': 0}, 'frame_id': ''}, 'vel_left': 0.2, 'vel_right': 0.2}"

#echo 'Metto a 0.2 la velocità di [db2]'
#rostopic pub /db2/wheels_driver_node/wheels_cmd duckietown_msgs/WheelsCmdStamped "{'header': {'seq': 0, 'stamp': { 'secs': 0, 'nsecs': 0}, 'frame_id': ''}, 'vel_left': 0.2, 'vel_right': 0.2}"


#ARRESTA ENTRAMBI

echo 'Metto a 0 la velocità di [db1]'
rostopic pub /db1/wheels_driver_node/wheels_cmd duckietown_msgs/WheelsCmdStamped "{'header': {'seq': 0, 'stamp': { 'secs': 0, 'nsecs': 0}, 'frame_id': ''}, 'vel_left': 0.0, 'vel_right': 0.0}"

echo 'Metto a 0 la velocità di [db2]'
rostopic pub /db2/wheels_driver_node/wheels_cmd duckietown_msgs/WheelsCmdStamped "{'header': {'seq': 0, 'stamp': { 'secs': 0, 'nsecs': 0}, 'frame_id': ''}, 'vel_left': 0.0, 'vel_right': 0.0}"


