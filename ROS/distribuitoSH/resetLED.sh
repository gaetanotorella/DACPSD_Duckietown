source ~/catkin_ws/devel/setup.bash
export ROS_HOSTNAME=ubuntu.local

rosservice call /db1/led_emitter_node/set_pattern "{pattern_name: {data: 'WHITE'}}"
rosservice call /db2/led_emitter_node/set_pattern "{pattern_name: {data: 'WHITE'}}"
rosservice call /ducklorean/led_emitter_node/set_pattern "{pattern_name: {data: 'WHITE'}}"
rosservice call /duckicha/led_emitter_node/set_pattern "{pattern_name: {data: 'WHITE'}}"