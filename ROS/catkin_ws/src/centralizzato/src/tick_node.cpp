#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include "duckietown_msgs/WheelEncoderStamped.h"


int  left_tick = 0;
int  right_tick = 0;


int main(int argc, char* argv[])
{ 	
	ros::init(argc, argv, "tick_node");
	ros::NodeHandle nodeHandle;

	ros::Rate loopRate(10);

	ros::Publisher pub_left = nodeHandle.advertise<duckietown_msgs::WheelEncoderStamped>("/db1/left_wheel_encoder_node/tick", 1);
	ros::Publisher pub_right= nodeHandle.advertise<duckietown_msgs::WheelEncoderStamped>("/db1/right_wheel_encoder_node/tick", 1);


	while (ros::ok()) {

        duckietown_msgs::WheelEncoderStamped msg_left;

        msg_left.data = left_tick;
        msg_left.header.stamp = ros::Time::now();

        left_tick = left_tick + 45; 
        
	    ROS_INFO_STREAM(msg_left.data);

        pub_left.publish(msg_left);
        
        ros::spinOnce();
        loopRate.sleep();
	} 


	return 0;
}