cd ~/catkin_ws
source devel/setup.bash
export ROS_MASTER_URI=http://duckicha.local:11311
rostopic list
export ROS_HOSTNAME=ubuntu.local
rosrun fkie_master_discovery master_discovery _rpc_port:=11616