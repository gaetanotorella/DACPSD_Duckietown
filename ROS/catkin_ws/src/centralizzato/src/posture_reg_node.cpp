#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include "duckietown_msgs/WheelEncoderStamped.h"
#include "duckietown_msgs/WheelsCmdStamped.h"
#include <math.h>

double Ts = 0.1;

double raggio = 0.0318;
double interasse = 0.1232;
double v = 0;				// m/s	
double omega = 0;			// rad/s
double x = 0;				// m
double y = 0;				// m
double theta = 0;			// rad
double tempo_medio = 0;		// s
double rk_rho = 0;
double rk_gamma = 0;
double rk_delta = 0;
double K1 = 0.4;
double K2 = 0.4;
double K3 = 0.4;
double comm_v = 0;				// m/s	
double comm_omega = 0;			// rad/s
double comm_wr = 0;				// rad/s	
double comm_wl = 0;				// rad/s

// void v_Callback(const std_msgs::Float64& msg) {
// 	v = double(msg.data);
// }
// void omega_Callback(const std_msgs::Float64& msg) {
// 	v = double(msg.data);
// }

double approssima(double numero, int cifreDecimali) {
    double coefficiente = std::pow(10, cifreDecimali);
    return std::round(numero * coefficiente) / coefficiente;
}

void x_Callback(const std_msgs::Float64& msg) {
	x = msg.data;
}
void y_Callback(const std_msgs::Float64& msg) {
	y = msg.data;
}
void theta_Callback(const std_msgs::Float64& msg) {
	theta = msg.data;
}

void ref_generator(double& x, double& y, double& theta){

	rk_rho = sqrt ((x*x)+(y*y));
	rk_gamma = atan2 (y,x) - theta + M_PI;

	if (rk_gamma > M_PI) {
		rk_gamma = rk_gamma - 2*M_PI;
	}
	if (rk_gamma < -M_PI) {
		rk_gamma = rk_gamma + 2*M_PI;
	}

	rk_delta = rk_gamma + theta;

	comm_v = K1 * rk_rho * cos(rk_gamma);
	comm_omega = K2 * rk_gamma + K1*sin(rk_gamma)*cos(rk_gamma)*(1+K3*rk_delta/rk_gamma);

	comm_wr = (2*comm_v + comm_omega*interasse)/(2*raggio);
	comm_wl = (2*comm_v - comm_omega*interasse)/(2*raggio);


}



int main(int argc, char* argv[])
{ 	
	ros::init(argc, argv, "posture_reg_node");
	ros::NodeHandle nodeHandle;
	ros::Rate loopRate(1/Ts);

	std::string macchina;

	if (!nodeHandle.getParam("posture_reg_node/nome_macchina", macchina)){
		ROS_ERROR("Impossibile ottenere il nome della macchina! Imposto automaticamente db1\n");
		macchina = "db1";
	}

	ros::Subscriber sub_x 			= nodeHandle.subscribe("/"+ macchina +"/DUCKS/traj_x", 10, x_Callback);
	ros::Subscriber sub_y 			= nodeHandle.subscribe("/"+ macchina +"/DUCKS/traj_y", 10, y_Callback);
	ros::Subscriber sub_theta 		= nodeHandle.subscribe("/"+ macchina +"/DUCKS/traj_theta", 10, theta_Callback);
	ros::Publisher pub_comm_velocity 	= nodeHandle.advertise<duckietown_msgs::WheelsCmdStamped>("/"+ macchina +"/wheels_driver_node/wheels_cmd", 1);

	duckietown_msgs::WheelsCmdStamped msg_comm;

	while (ros::ok()) {	


		ref_generator(x,y,theta);
		std::cout<<" [c_v] "<<comm_v<<" [c_om] "<<comm_omega<<std::endl;
		std::cout<<" [w_r] "<<comm_wr*raggio<<" [w_l] "<<comm_wl*raggio<<std::endl;
		std::cout<<" [rho] "<<rk_rho<<" [gamma] "<<rk_gamma<<" [delta] "<<rk_delta<<std::endl;

		
		msg_comm.vel_left = comm_wl*raggio;
		msg_comm.vel_right = comm_wr*raggio;

        pub_comm_velocity.publish(msg_comm);



	// finale ros
	ros::spinOnce();
	loopRate.sleep();

	} 

		msg_comm.vel_left = 0;
		msg_comm.vel_right = 0;

        pub_comm_velocity.publish(msg_comm);

	return 0;
}