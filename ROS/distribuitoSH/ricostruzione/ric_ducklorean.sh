cd ~/catkin_ws
source devel/setup.bash
export ROS_HOSTNAME=ubuntu.local
roslaunch distribuito ricostruzione_launch.launch nome_db:=ducklorean x:=-1.5 y:=0.9 theta:=0.0
