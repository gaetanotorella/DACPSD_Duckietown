cd
cd catkin_ws
source devel/setup.bash
export ROS_HOSTNAME=ubuntu.local   #sostituisci il nome del pc
export ROS_MASTER_URI=http://db2.local:11311
rosrun fkie_master_sync master_sync _check_host:=false