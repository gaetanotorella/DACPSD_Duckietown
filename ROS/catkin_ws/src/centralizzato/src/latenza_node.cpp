#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int64.h>
#include "duckietown_msgs/WheelEncoderStamped.h"
#include "duckietown_msgs/WheelsCmdStamped.h"
#include <math.h>
#include <ros/time.h>


struct encoder{
	int tick;
    int sec;
    int nsec;
    ros::Time invio_db;
};

encoder enc_r;
encoder enc_l;

ros::Time ricevo_db;
ros::Time invio_pc;


void sub_left(const duckietown_msgs::WheelEncoderStamped& msg)
{
    enc_l.tick=msg.data;  
    enc_l.invio_db = msg.header.stamp;
    
    ricevo_db = ros::Time::now();

    enc_l.sec=msg.header.stamp.sec;
    enc_l.nsec=msg.header.stamp.nsec;
}

void sub_right(const duckietown_msgs::WheelEncoderStamped& msg)
{
    enc_r.tick=msg.data;
    enc_r.invio_db = msg.header.stamp;

    enc_r.sec=msg.header.stamp.sec;
    enc_r.nsec=msg.header.stamp.nsec;
}


int main(int argc, char* argv[])
{ 	
	
	ros::init(argc, argv, "latenza_node");
	ros::NodeHandle nodeHandle;
	ros::Rate loopRate(10);

	std::string macchina;

	if (!nodeHandle.getParam("nome_macchina", macchina)){
		ROS_ERROR("Impossibile ottenere il nome della macchina! Imposto automaticamente db1\n");
		macchina = "db1";
	}


	ros::Subscriber subscriber_left = nodeHandle.subscribe("/"+ macchina +"/left_wheel_encoder_node/tick",10,sub_left);
	ros::Subscriber subscriber_right= nodeHandle.subscribe("/"+ macchina +"/right_wheel_encoder_node/tick",10,sub_right);

	ros::Publisher pub_comm_velocity= nodeHandle.advertise<duckietown_msgs::WheelsCmdStamped>("/"+ macchina +"/wheels_driver_node/wheels_cmd", 1);
    duckietown_msgs::WheelsCmdStamped msg_comm;

    ros::Publisher pub_latency= nodeHandle.advertise<std_msgs::Int64>("/"+ macchina +"/DUCKS/latency", 1);
    std_msgs::Int64 latency_msg;

    int appoggio_l = enc_l.tick;
    int appoggio_r = enc_r.tick;

	while (ros::ok()) {	

    std::cout<<"avvio...."<<std::endl;

    std::cout<<"ricevo_db: "<<ricevo_db<<std::endl;
    std::cout<<"invio_pc:  "<<invio_pc<<std::endl;

    std::cout<<"ricevo-invio: "<<(ricevo_db-invio_pc)<<std::endl;
    pub_latency.publish(latency_msg);

    msg_comm.vel_left = 0.2;
    msg_comm.vel_right = 0.2;
    pub_comm_velocity.publish(msg_comm);
    invio_pc = ros::Time::now();


 
    // finale ros
	ros::spinOnce();
	loopRate.sleep();

	} 

	return 0;

}