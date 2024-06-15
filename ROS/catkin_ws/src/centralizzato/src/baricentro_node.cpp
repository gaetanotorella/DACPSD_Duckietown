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

double Ts = 0.1;

double raggio = 0.0318;
double interasse = 0.1232;

double x_b;
double y_b;
double off_y = 0.6;
double saturation = 0.9;

double K1 = 0.2;
double K2 = 0.5;
double K3 = 0.3;


struct macchina{
	std::string nome;
	double x;				// m
	double y;				// m
	double theta = 0;			// rad
	double rk_rho = 0;
	double rk_gamma = 0;
	double rk_delta = 0;
	double v = 0;				// m/s	
	double omega = 0;			// rad/s
	double comm_wr = 0;				// rad/s	
	double comm_wl = 0;				// rad/s
	double db_wl=0; //letta
	double db_wr=0; //letta
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



void ref_generator(macchina& mach){

	mach.rk_rho = sqrt ((mach.x*mach.x)+(mach.y*mach.y));
	mach.rk_gamma = atan2 (mach.y,mach.x) - mach.theta + M_PI;


	// if (mach.rk_rho <= 0.5){

	// 	double v_mis = raggio/2*(mach.db_wr*(2*M_PI) + mach.db_wl*(2*M_PI));

	// 	if (v_mis <= 0.1){
	// 		std::cout<<"fermato....."<<std::endl;

	// 		mach.comm_wr = mach.comm_wr + 0.25/raggio;
	// 		mach.comm_wl = mach.comm_wl + 0.25/raggio;

	// 	}

	// }
	// else{
		if (mach.rk_gamma > M_PI) {
			mach.rk_gamma = mach.rk_gamma - 2*M_PI;
		}
		if (mach.rk_gamma < -M_PI) {
			mach.rk_gamma = mach.rk_gamma + 2*M_PI;
		}

		mach.rk_delta = mach.rk_gamma + mach.theta;

	// caloclo v e omega
		mach.v = K1 * mach.rk_rho * cos(mach.rk_gamma);
		mach.omega = K2 * mach.rk_gamma + K1*sin(mach.rk_gamma)*cos(mach.rk_gamma)*(1+K3*mach.rk_delta/mach.rk_gamma);

	// calcolo wr wl

		mach.comm_wr = (2*mach.v + mach.omega*interasse)/(2*raggio);
		mach.comm_wl = (2*mach.v - mach.omega*interasse)/(2*raggio);

	// }

}

void calcolo_V_omega(macchina& appoggio, macchina &duck, duckietown_msgs::WheelsCmdStamped& msg_comm, double& offset){
	appoggio.x = duck.x + (duck.x - x_b);						// m
	appoggio.y = duck.y + (duck.y - y_b) + offset;				// m
	appoggio.theta = duck.theta;		
	appoggio.nome = duck.nome;	
	appoggio.rk_rho = 0;
	appoggio.rk_gamma = 0;
	appoggio.rk_delta = 0;
	appoggio.v = 0;						// m/s	
	appoggio.omega = 0;					// rad/s
	appoggio.comm_wr = 0;				// rad/s	
	appoggio.comm_wl = 0;				// rad/s

	ref_generator(appoggio);

	msg_comm.vel_left = appoggio.comm_wl*raggio;
	msg_comm.vel_right = appoggio.comm_wr*raggio;

	if (msg_comm.vel_left >= saturation){
		msg_comm.vel_left = saturation;
	}

	if (msg_comm.vel_right >= saturation){
		msg_comm.vel_right = saturation;
	}


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

	// Pubblicare la trasformata
	tf_broadcaster.sendTransform(transformStamped);

}

int main(int argc, char* argv[])
{ 	
	ros::init(argc, argv, "baricentro_node");
	ros::NodeHandle nodeHandle;
	ros::Rate loopRate(1/Ts);

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

	// calcolo velocità per db1
	double off_db1 = off_y;
	calcolo_V_omega(appoggio, duck_1, msg_comm1, off_db1);

	// calcolo velocità per db2
	double off_db2 = -off_y;
	calcolo_V_omega(appoggio, duck_2, msg_comm2, off_db2);

	// pubblico le velocità sul topic
    pub_comm_velocity1.publish(msg_comm1);
    pub_comm_velocity2.publish(msg_comm2);

	
	pubblica_tf("baricentro",tf_broadcaster);

	double bari_rho = sqrt ((x_b*x_b)+(y_b*y_b));
		
		std::cout<<"x: "<<x_b<<" y: "<<y_b<<" rho: "<<bari_rho<<std::endl;

		if (bari_rho <= 0.1 && bari_rho > 0){

			std::cout<<"BARICENTROOOO"<<std::endl;

			msg_comm1.vel_left = 0.0;
			msg_comm2.vel_left = 0.0;
			msg_comm1.vel_right = 0.0;
			msg_comm2.vel_right = 0.0;

			pub_comm_velocity1.publish(msg_comm1);
			pub_comm_velocity2.publish(msg_comm2);

			ros::shutdown();
			
		}



	// finale ros
	ros::spinOnce();
	loopRate.sleep();

	} 


	return 0;
}


