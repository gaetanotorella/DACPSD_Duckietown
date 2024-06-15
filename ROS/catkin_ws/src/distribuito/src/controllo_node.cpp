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

#include <duckietown_msgs/ChangePattern.h>

double Ts = 0.1;

double raggio = 0.0318;
double interasse = 0.1232;

double x_b;
double y_b;
double off_y = 0;	 // preso dal launch	

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
};

macchina duck_main;
macchina duck_2;
macchina duck_3;

void Callback_main(const geometry_msgs::Pose2D& msg){
	duck_main.x = msg.x;
	duck_main.y = msg.y;
	duck_main.theta = msg.theta;
}

void Callback_2(const geometry_msgs::Pose2D& msg){
	duck_2.x = msg.x;
	duck_2.y = msg.y;
	duck_2.theta = msg.theta;
}

void Callback_3(const geometry_msgs::Pose2D& msg){
	duck_3.x = msg.x;
	duck_3.y = msg.y;
	duck_3.theta = msg.theta;
}

bool check_node(std::string nome_node){
	std::vector<std::string> nodes;
    ros::master::getNodes(nodes); 	
	bool trovato = false;

	for (const auto& node : nodes) {
		if (node == nome_node){
			trovato = true;
		}
	}
    nodes.clear();

	return trovato;
}


void posture_reg(macchina& mach){

	mach.rk_rho = sqrt ((mach.x*mach.x)+(mach.y*mach.y));
	mach.rk_gamma = atan2 (mach.y,mach.x) - mach.theta + M_PI;

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

}

void calcolo_V_omega(macchina& appoggio, macchina &duck, duckietown_msgs::WheelsCmdStamped& msg_vel, double& offset){
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

	posture_reg(appoggio);

	msg_vel.vel_left = appoggio.comm_wl*raggio;
	msg_vel.vel_right = appoggio.comm_wr*raggio;

}

void delta_update(double delta[], double delta_old[], double d_main, double d_2, double d_3){

	delta[0] = delta_old[0] - (d_main - d_2);
	delta[1] = delta_old[1] - (d_main - d_3);
	delta_old = delta;

}

void pubblica_tf(std::string nome, tf2_ros::TransformBroadcaster& tf_broadcaster){

	geometry_msgs::TransformStamped transformStamped;
	transformStamped.header.stamp = ros::Time::now();
	transformStamped.header.frame_id = "map"; 
	transformStamped.child_frame_id = nome; 
	transformStamped.transform.translation.x = x_b;
	transformStamped.transform.translation.y = y_b;
	transformStamped.transform.translation.z = 0.0; 
	transformStamped.transform.rotation = tf2::toMsg(tf2::Quaternion(0, 0, 0));

	// Pubblicare la trasformata
	tf_broadcaster.sendTransform(transformStamped);

}

int main(int argc, char* argv[])
{ 	
	ros::init(argc, argv, "controllo_node");
	ros::NodeHandle nodeHandle("~");
	ros::Rate loopRate(1/Ts);


	if (!nodeHandle.getParam("nome_macchina_main", duck_main.nome)){
		ROS_ERROR("Impossibile ottenere il nome della macchinaMain!\n");
	}
	if (!nodeHandle.getParam("nome_macchina_2", duck_2.nome)){
		ROS_ERROR("Impossibile ottenere il nome della macchina2!\n");
	}
	if (!nodeHandle.getParam("nome_macchina_3", duck_3.nome)){
		ROS_ERROR("Impossibile ottenere il nome della macchina3!\n");
	}
	if (!nodeHandle.getParam("offset", off_y)){
		ROS_ERROR("Impossibile ottenere il valore dell'offset!\n");
	}
	

	ros::Subscriber sub_pose_main 	= nodeHandle.subscribe("/DUCKS/"+ duck_main.nome +"/pose", 10, Callback_main);
	ros::Subscriber sub_pose_mach2 	= nodeHandle.subscribe("/DUCKS/"+ duck_2.nome +"/pose", 10, Callback_2);
	ros::Subscriber sub_pose_mach3 	= nodeHandle.subscribe("/DUCKS/"+ duck_3.nome +"/pose", 10, Callback_3);

	ros::Publisher pub_comm_vel		= nodeHandle.advertise<duckietown_msgs::WheelsCmdStamped>("/"+ duck_main.nome +"/wheels_driver_node/wheels_cmd", 1);
	duckietown_msgs::WheelsCmdStamped msg_vel;
 
 	duckietown_msgs::ChangePattern srv;
	ros::ServiceClient client = nodeHandle.serviceClient<duckietown_msgs::ChangePattern>("/"+ duck_main.nome +"/led_emitter_node/set_pattern");

	double x_delta[2] = {0, 0};
	double x_delta_old[2] = {0, 0};

	double y_delta[2] = {0, 0};
	double y_delta_old[2] = {0, 0};

	tf2_ros::TransformBroadcaster tf_broadcaster;


	while (ros::ok()) {	

		delta_update(x_delta, x_delta_old, duck_main.x, duck_2.x, duck_3.x);
		delta_update(y_delta, y_delta_old, duck_main.y, duck_2.y, duck_3.y);

		// calcolo consenso

	std::string topico2;
	std::string topico3;

	topico2 = "/ricostruzione_node_" + duck_2.nome + "";
	topico3 = "/ricostruzione_node_" + duck_3.nome + "";

	if (!check_node(topico2)){
		ROS_INFO("E' morta: %s", duck_2.nome.c_str());
		srv.request.pattern_name.data = "RED";
		client.call(srv);
		x_delta[0] = 0.0;
		y_delta[0] = 0.0;
	}
	else if (!check_node(topico3)){
		ROS_INFO("E' morta: %s", duck_3.nome.c_str());
		srv.request.pattern_name.data = "BLUE";
		client.call(srv);
		x_delta[1] = 0.0;
		y_delta[1] = 0.0;
	}
	else{
		ROS_INFO("E' morta: %s", duck_3.nome.c_str());
		srv.request.pattern_name.data = "WHITE";
	}


	x_b = (x_delta[0] + x_delta[1] + duck_main.x);
	y_b = (y_delta[0] + y_delta[1] + duck_main.y);


		// calcolo baricentro
		// x_b = (duck_main.x + duck_2.x + duck_3.x)/3;
		// y_b = (duck_main.y + duck_2.y + duck_3.y)/3;


		macchina appoggio;
		// calcolo velocità per MAIN
		calcolo_V_omega(appoggio, duck_main, msg_vel, off_y);


		// pubblico le velocità sul topic
		pub_comm_vel.publish(msg_vel);

		// pubblica baricentro
		std::string nome_frame = "baricentro"+duck_main.nome+"";
		pubblica_tf(nome_frame,tf_broadcaster);

		// finale ros
		ros::spinOnce();
		loopRate.sleep();

	} 


	return 0;
}


