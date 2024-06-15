#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include "duckietown_msgs/WheelEncoderStamped.h"
#include "duckietown_msgs/WheelsCmdStamped.h"
#include <math.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

#include <geometry_msgs/Pose2D.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseArray.h>

double Ts = 0.1; 

struct macchina{
	
	geometry_msgs::Pose2D pose;
	geometry_msgs::Pose poseStmp;
	std::string nome;
	geometry_msgs::PoseArray poseArray;

};

macchina duck_db1;
macchina duck_db2;
macchina duck_ducklorean;
macchina duck_duckicha;

void compute_path(macchina& duck){
	duck.poseStmp.position.x = duck.pose.x;
	duck.poseStmp.position.y = duck.pose.y;
	duck.poseStmp.position.z = 0.0;

    tf2::Quaternion quaternione;
    quaternione.setRPY(0.0, 0.0, duck.pose.theta);
	duck.poseStmp.orientation = tf2::toMsg(quaternione);

	duck.poseArray.header.stamp = ros::Time::now();
    duck.poseArray.header.frame_id = "map";

    duck.poseArray.poses.push_back(duck.poseStmp);
}

void sub_1(const geometry_msgs::Pose2D& msg){
	duck_db1.pose.x = msg.x;
	duck_db1.pose.y = msg.y;
	duck_db1.pose.theta = msg.theta;
	compute_path(duck_db1);
}

void sub_2(const geometry_msgs::Pose2D& msg){
	duck_db2.pose.x = msg.x;
	duck_db2.pose.y = msg.y;
	duck_db2.pose.theta = msg.theta;
	compute_path(duck_db2);

}

void sub_3(const geometry_msgs::Pose2D& msg){
	duck_ducklorean.pose.x = msg.x;
	duck_ducklorean.pose.y = msg.y;
	duck_ducklorean.pose.theta = msg.theta;
	compute_path(duck_ducklorean);
}

void sub_4(const geometry_msgs::Pose2D& msg){
	duck_duckicha.pose.x = msg.x;
	duck_duckicha.pose.y = msg.y;
	duck_duckicha.pose.theta = msg.theta;
	compute_path(duck_duckicha);
}



int main(int argc, char* argv[])
{ 	
	
	ros::init(argc, argv, "mappa_node");
	ros::NodeHandle nodeHandle;
	ros::Rate loopRate(1/Ts);

	duck_db1.nome = "db1";
	duck_db2.nome = "db2";
	duck_ducklorean.nome = "ducklorean";
	duck_duckicha.nome = "duckicha";
	
	ros::Subscriber sub_db1			= nodeHandle.subscribe("/DUCKS/db1/pose",10,sub_1);
	ros::Subscriber sub_db2			= nodeHandle.subscribe("/DUCKS/db2/pose",10,sub_2);
	ros::Subscriber sub_ducklorean	= nodeHandle.subscribe("/DUCKS/ducklorean/pose",10,sub_3);
	ros::Subscriber sub_duckicha	= nodeHandle.subscribe("/DUCKS/duckicha/pose",10,sub_4);

	ros::Publisher 	pub_db1			= nodeHandle.advertise<geometry_msgs::PoseArray>("/DUCKS/" + duck_db1.nome + "/path", 1);
	ros::Publisher	pub_db2			= nodeHandle.advertise<geometry_msgs::PoseArray>("/DUCKS/" + duck_db2.nome + "/path", 1);
	ros::Publisher	pub_ducklorean	= nodeHandle.advertise<geometry_msgs::PoseArray>("/DUCKS/" + duck_ducklorean.nome + "/path", 1);		
	ros::Publisher	pub_duckicha	= nodeHandle.advertise<geometry_msgs::PoseArray>("/DUCKS/" + duck_duckicha.nome + "/path", 1);		
	
	while (ros::ok()) {	

    pub_db1.publish(duck_db1.poseArray);
    pub_db2.publish(duck_db2.poseArray);
    pub_ducklorean.publish(duck_ducklorean.poseArray);
    pub_duckicha.publish(duck_duckicha.poseArray);

 
	// finale ros
	ros::spinOnce();
	loopRate.sleep();

	} 

	return 0;
}

// rostopic pub /DUCKS/db1/pose geometry_msgs/Pose2D "{'x':0 , 'y':0, 'theta':0}" -r 10
