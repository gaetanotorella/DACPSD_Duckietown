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


double raggio = 0.0318;
double interasse = 0.1232;

double x_b;
double y_b;
double off_y = 0.2;

double ref = 0.12; 

struct macchina{
	std::string nome;
	double x = 0;				// m
	double y = 0;				// m
	double theta = 0;			// rad
	double v = 0;				// m/s	
	double omega = 0;			// rad/s
	double comm_wr = ref;			// rad/s	
	double comm_wl = ref;			// rad/s
	double db_wl = 0; 			//letta
	double db_wr = 0; 			//letta
};

macchina duck_1;
macchina duck_2;


void x1_Callback(const std_msgs::Float64& msg) {
	duck_1.x = msg.data;
	duck_1.nome = "db1";
}
void y1_Callback(const std_msgs::Float64& msg) {
	duck_1.y = msg.data;
}
void theta1_Callback(const std_msgs::Float64& msg) {
	duck_1.theta = msg.data;
}

void x2_Callback(const std_msgs::Float64& msg) {
	duck_2.x = msg.data;
	duck_2.nome = "db2";
}
void y2_Callback(const std_msgs::Float64& msg) {
	duck_2.y = msg.data;
}
void theta2_Callback(const std_msgs::Float64& msg) {
	duck_2.theta = msg.data;
}

void wl1_Callback(const std_msgs::Float64& msg) {
	duck_1.db_wl = msg.data;
}
void wr1_Callback(const std_msgs::Float64& msg) {
	duck_1.db_wr = msg.data;
}
void wl2_Callback(const std_msgs::Float64& msg) {
	duck_2.db_wl = msg.data;
}
void wr2_Callback(const std_msgs::Float64& msg) {
	duck_2.db_wr = msg.data;
}

void control(macchina& duck, duckietown_msgs::WheelsCmdStamped& msg_comm, double offset){

	double err_wl = ref - duck.db_wl*raggio;
	double err_wr = ref - duck.db_wr*raggio;;

	if (err_wl > 0.05){
		duck.comm_wl = duck.comm_wl + 0.01;
	}
	else if (err_wl < -0.05){
		duck.comm_wl = duck.comm_wl - 0.01;
	}

	if (err_wr > 0.05){
		duck.comm_wr = duck.comm_wr + 0.01;
	}
	else if (err_wr < -0.05){
		duck.comm_wr = duck.comm_wr - 0.01;
	}

	// if (duck.nome == "db1"){
	// 	if ((duck.y - y_b) > offset){
	// 		duck.comm_wl = duck.comm_wl + 0.01;
	// 	}
	// 	else if ((duck.y - y_b) < offset){
	// 		duck.comm_wr = duck.comm_wr + 0.01;
	// 	}
	// }

	// else if (duck.nome == "db2"){
	// 	if ((duck.y - y_b) < offset){
	// 		duck.comm_wr = duck.comm_wr + 0.01;
	// 	}
	// 	else if ((duck.y - y_b) > offset){
	// 		duck.comm_wl = duck.comm_wl + 0.01;
	// 	}

	// }

	if ((duck.y - y_b) > (offset + 0.1)){
		duck.comm_wl = duck.comm_wl + 0.01;
	}
	else if ((duck.y - y_b) < (offset - 0.1)){
		duck.comm_wr = duck.comm_wr + 0.01;
	}


	msg_comm.vel_left = duck.comm_wl;
	msg_comm.vel_right = duck.comm_wr;
}



void pubblica_tf(std::string nome_macchina, tf2_ros::TransformBroadcaster& tf_broadcaster){

	geometry_msgs::TransformStamped transformStamped;
	transformStamped.header.stamp = ros::Time::now();
	transformStamped.header.frame_id = "map"; 
	transformStamped.child_frame_id = nome_macchina; 
	transformStamped.transform.translation.x = x_b;
	transformStamped.transform.translation.y = y_b;
	transformStamped.transform.translation.z = 0.0; 
	transformStamped.transform.rotation = tf2::toMsg(tf2::Quaternion(0, 0, 0));
	tf_broadcaster.sendTransform(transformStamped);

}

int main(int argc, char* argv[])
{ 	
	ros::init(argc, argv, "bari_control_node");
	ros::NodeHandle nodeHandle;
	ros::Rate loopRate(10);

	ros::Subscriber wl_db1 			= nodeHandle.subscribe("/db1/DUCKS/left_vel", 10, wl1_Callback);
	ros::Subscriber wr_db1 			= nodeHandle.subscribe("/db1/DUCKS/right_vel", 10, wr1_Callback);
	ros::Subscriber wl_db2 			= nodeHandle.subscribe("/db2/DUCKS/left_vel", 10, wl2_Callback);
	ros::Subscriber wr_db2 			= nodeHandle.subscribe("/db2/DUCKS/right_vel", 10, wr2_Callback);

	ros::Subscriber sub_x1 			= nodeHandle.subscribe("/db1/DUCKS/traj_x", 10, x1_Callback);
	ros::Subscriber sub_y1 			= nodeHandle.subscribe("/db1/DUCKS/traj_y", 10, y1_Callback);
	ros::Subscriber sub_theta1 		= nodeHandle.subscribe("/db1/DUCKS/traj_theta", 10, theta1_Callback);
	ros::Publisher pub_comm_velocity1 	= nodeHandle.advertise<duckietown_msgs::WheelsCmdStamped>("/db1/wheels_driver_node/wheels_cmd", 1);

	ros::Subscriber sub_x2 			= nodeHandle.subscribe("/db2/DUCKS/traj_x", 10, x2_Callback);
	ros::Subscriber sub_y2 			= nodeHandle.subscribe("/db2/DUCKS/traj_y", 10, y2_Callback);
	ros::Subscriber sub_theta2 		= nodeHandle.subscribe("/db2/DUCKS/traj_theta", 10, theta2_Callback);
	ros::Publisher pub_comm_velocity2 	= nodeHandle.advertise<duckietown_msgs::WheelsCmdStamped>("/db2/wheels_driver_node/wheels_cmd", 1);

	duckietown_msgs::WheelsCmdStamped msg_comm1;
	duckietown_msgs::WheelsCmdStamped msg_comm2;
    tf2_ros::TransformBroadcaster tf_broadcaster;



	while (ros::ok()) {	

	// calcolo baricentro
	x_b = (duck_1.x + duck_2.x)/2;
	y_b = (duck_1.y + duck_2.y)/2;

	macchina appoggio;

	// calcolo velocità per db1 SOPRAAAA
	off_y = abs(off_y);
	control(duck_1, msg_comm1, off_y);
   	pub_comm_velocity1.publish(msg_comm1);


	// calcolo velocità per db2 SOTTOOOO
	off_y = -off_y;
	control(duck_2, msg_comm2, off_y);
    pub_comm_velocity2.publish(msg_comm2);


 


	pubblica_tf("baricentro",tf_broadcaster);

	// finale ros
	ros::spinOnce();
	loopRate.sleep();

	} 


	return 0;
}


