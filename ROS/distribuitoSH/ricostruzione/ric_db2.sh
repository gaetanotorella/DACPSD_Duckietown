cd ~/catkin_ws
source devel/setup.bash
export ROS_HOSTNAME=ubuntu.local
roslaunch distribuito ricostruzione_launch.launch nome_db:=db2 x:=-1.0 y:=-0.9 theta:=0.0
