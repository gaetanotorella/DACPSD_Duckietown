cd
cd catkin_ws
source devel/setup.bash
export ROS_HOSTNAME=ubuntu.local   #sostituisci il nome del pc
rosrun fkie_master_discovery master_discovery _rpc_port:=11613