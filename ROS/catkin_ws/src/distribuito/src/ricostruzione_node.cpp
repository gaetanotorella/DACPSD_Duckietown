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
#include <duckietown_msgs/Pose2DStamped.h>


double Ts = 0.1; 

int left_tick = 0;
int right_tick = 0;
double raggio = 0.0318;
double interasse = 0.105;
double v = 0;				// m/s	
double omega = 0;			// rad/s
double x = 0;				// m
double y = 0;				// m
double theta = 0;			// rad
double tempo_medio = 0;		// s



struct encoder{
	int tick;
	int temp[2];
	int diff;
	double velocity;  // giri/sec
    int sec;
    int nsec;
    int sec_diff;
	int nsec_diff;
    int sec_temp[2];
    int nsec_temp[2];
	double intervallo;
};

encoder enc_r;
encoder enc_l;

void compute_vel(encoder& enc){

	if (enc.temp[0]==0 && enc.temp[1]==0) {
		enc.temp[0]=enc.tick;
		enc.temp[1]=enc.tick;
	} else enc.temp[1]=enc.tick;
	enc.diff=enc.temp[1]-enc.temp[0];

	if (enc.sec_temp[0]==0 && enc.sec_temp[1]==0) {
		enc.sec_temp[0]=enc.sec;
		enc.sec_temp[1]=enc.sec;
	} else enc.sec_temp[1]=enc.sec;
	enc.sec_diff=enc.sec_temp[1]-enc.sec_temp[0];

    if (enc.nsec_temp[0]==0 && enc.nsec_temp[1]==0) {
		enc.nsec_temp[0]=enc.nsec;
		enc.nsec_temp[1]=enc.nsec;
	} else enc.nsec_temp[1]=enc.nsec;
	enc.nsec_diff=enc.nsec_temp[1]-enc.nsec_temp[0];
	

enc.intervallo = static_cast<double>(enc.sec_diff) + (static_cast<double>(enc.nsec_diff)/1000000000);
enc.velocity = static_cast<double>(enc.diff)/135 / enc.intervallo; 

    enc.sec_temp[0]=enc.sec_temp[1];
    enc.nsec_temp[0]=enc.nsec_temp[1];
    enc.temp[0]=enc.temp[1];

}

void sub_left(const duckietown_msgs::WheelEncoderStamped& msg)
{
	// ROS_INFO("TICK: [%d] left", msg.data);
    enc_l.tick=msg.data;
    
    enc_l.sec=msg.header.stamp.sec;
    enc_l.nsec=msg.header.stamp.nsec;

    compute_vel(enc_l);

}

void sub_right(const duckietown_msgs::WheelEncoderStamped& msg)
{
	// ROS_INFO("TICK: [%d] right", msg.data);
    enc_r.tick=msg.data;

    enc_r.sec=msg.header.stamp.sec;
    enc_r.nsec=msg.header.stamp.nsec;
    
    compute_vel(enc_r);

}

void compute_v_omega(encoder& enc_r, encoder& enc_l){

	v = raggio/2*(enc_r.velocity*(2*M_PI) + enc_l.velocity*(2*M_PI));
	omega = raggio/interasse*(enc_r.velocity*(2*M_PI) - enc_l.velocity*(2*M_PI));
	tempo_medio = (enc_r.intervallo + enc_l.intervallo)/2;

}

void runge_kutta(double v, double omega){

	double Ts_rk = tempo_medio;
	double x_p = x;
	double y_p = y;
	double theta_p = theta;

	if(std::isnan(v)){
		v=0;
	}
	if(std::isnan(omega)){
		omega = 0;
	}

	x = x_p + v * Ts_rk * cos(theta_p + 1/2*omega*Ts_rk);
	y = y_p + v * Ts_rk * sin(theta_p + 1/2*omega*Ts_rk);
	theta = theta_p + omega*Ts_rk;

}

void pubblica_tf(std::string nome_macchina, tf2_ros::TransformBroadcaster& tf_broadcaster){

	geometry_msgs::TransformStamped transformStamped;
	transformStamped.header.stamp = ros::Time::now();
	transformStamped.header.frame_id = "map"; 
	transformStamped.child_frame_id = "/" + nome_macchina + "/footprint"; 
	transformStamped.transform.translation.x = x;
	transformStamped.transform.translation.y = y;
	transformStamped.transform.translation.z = 0.0; 
	transformStamped.transform.rotation = tf2::toMsg(tf2::Quaternion(0, 0, theta));

	// Pubblicare la trasformata
	tf_broadcaster.sendTransform(transformStamped);

}

int main(int argc, char* argv[])
{ 	
	
	ros::init(argc, argv, "ricostruzione_node");
	ros::NodeHandle nodeHandle;
	ros::Rate loopRate(1/Ts);

	std::string macchina;

	if (!nodeHandle.getParam("nome_macchina", macchina)){
		ROS_ERROR("Impossibile ottenere il nome della macchina! Imposto automaticamente db1\n");
		macchina = "db1";
	}

	if (!nodeHandle.getParam("x_init", x)){
		ROS_ERROR("Impossibile ottenere la X iniziale");
	}

	if (!nodeHandle.getParam("y_init", y)){
		ROS_ERROR("Impossibile ottenere la Y iniziale");
	}

		if (!nodeHandle.getParam("theta_init", theta)){
		ROS_ERROR("Impossibile ottenere la THETA iniziale");
	}
	

	ros::Subscriber subscriber_left = nodeHandle.subscribe("/"+ macchina +"/left_wheel_encoder_node/tick",10,sub_left);
	ros::Subscriber subscriber_right= nodeHandle.subscribe("/"+ macchina +"/right_wheel_encoder_node/tick",10,sub_right);

	ros::Publisher pub_left 	= nodeHandle.advertise<std_msgs::Float64>("/DUCKS/"+ macchina +"/left_vel", 1);
	ros::Publisher pub_right 	= nodeHandle.advertise<std_msgs::Float64>("/DUCKS/"+ macchina +"/right_vel", 1);
	ros::Publisher pub_v 		= nodeHandle.advertise<std_msgs::Float64>("/DUCKS/"+ macchina +"/v_velocity", 1);
	ros::Publisher pub_omega 	= nodeHandle.advertise<std_msgs::Float64>("/DUCKS/"+ macchina +"/omega_velocity", 1);

	ros::Publisher pub_pose 	= nodeHandle.advertise<geometry_msgs::Pose2D>("/DUCKS/"+ macchina +"/pose",1);
	
	ros::Publisher pub_duck_pose 	= nodeHandle.advertise<duckietown_msgs::Pose2DStamped>("/"+ macchina +"/velocity_to_pose_node/pose",1);

	// ros::Publisher pub_x 		= nodeHandle.advertise<std_msgs::Float64>("/"+ macchina +"/DUCKS/traj_x", 1);
	// ros::Publisher pub_y 		= nodeHandle.advertise<std_msgs::Float64>("/"+ macchina +"/DUCKS/traj_y", 1);
	// ros::Publisher pub_theta 	= nodeHandle.advertise<std_msgs::Float64>("/"+ macchina +"/DUCKS/traj_theta", 1);

    tf2_ros::TransformBroadcaster tf_broadcaster;

	
	while (ros::ok()) {	

// DICHIARAZIONE MESSAGGI
	std_msgs::Float64 msg_left_vel;
	std_msgs::Float64 msg_right_vel;
	std_msgs::Float64 msg_v;
	std_msgs::Float64 msg_omega;

	geometry_msgs::Pose2D msg_pose;

	duckietown_msgs::Pose2DStamped msg_duck_pose;

	// std_msgs::Float64 msg_x;
	// std_msgs::Float64 msg_y;
	// std_msgs::Float64 msg_theta;

// PUBBLICO VELOCITA' SINGOLE RUOTE
	msg_left_vel.data = enc_l.velocity*(2*M_PI); // rad/s
	msg_right_vel.data = enc_r.velocity*(2*M_PI); // rad/s
	pub_left.publish(msg_left_vel);
	pub_right.publish(msg_right_vel);

	// std::cout<<"[L] velocita'"<<enc_l.velocity*(2*M_PI)<<" rad/s ";
	// std::cout<<"[R] velocita'"<<enc_r.velocity*(2*M_PI)<<" rad/s"<<std::endl;

// CALCOLO E PUBBLICO VELOCITA' V E OMEGA
	compute_v_omega(enc_r, enc_l);
		std::cout<<"[V] = "<<v<<" m/sec ";
		std::cout<<"[W] = "<<omega<<" rad/sec"<<std::endl;

	msg_v.data = v; 			// m/s
	msg_omega.data = omega; 	// rad/s
	pub_v.publish(msg_v);
	pub_omega.publish(msg_omega);

// CALCOLO E PUBBLICO TRAIETTORIA X Y THETA
	runge_kutta(v,omega);
		std::cout<<"[X] = "<<x<<"  ";
		std::cout<<"[Y] = "<<y<<"  ";
		std::cout<<"[T] = "<<theta*180/M_PI<<"°  ";
		std::cout<<"[T] = "<<theta<<"rad  ";

	// msg_x.data = x;
	// msg_y.data = y;
	// msg_theta.data = theta; 	// rad
	// pub_x.publish(msg_x);
	// pub_y.publish(msg_y);
	// pub_theta.publish(msg_theta);

	msg_pose.x = x;
	msg_pose.y = y;
	msg_pose.theta = theta; 	// rad
	pub_pose.publish(msg_pose);

	msg_duck_pose.x = x; 
	msg_duck_pose.y = y; 
	msg_duck_pose.theta = theta; 
	pub_duck_pose.publish(msg_duck_pose);


	pubblica_tf(macchina,tf_broadcaster);

 
// finale ros
	ros::spinOnce();
	loopRate.sleep();

	} 

	return 0;
}