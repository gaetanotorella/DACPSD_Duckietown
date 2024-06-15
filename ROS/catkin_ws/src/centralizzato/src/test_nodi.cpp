#include <ros/ros.h>
#include <ros/master.h>
// #include <ros/datatypes.h>

#include <std_msgs/String.h>
#include <std_msgs/Float64.h>


bool check_node(std::string nome_topic){
	std::vector<std::string> nodes;
    ros::master::getNodes(nodes); 	
	bool trovato = false;

	for (const auto& node : nodes) {
		if (node == nome_topic){
			trovato = true;
		}
	}
    nodes.clear();
	
	return trovato;
}

int main(int argc, char* argv[])
{ 	
	ros::init(argc, argv, "test_nodi");
	ros::NodeHandle nodeHandle;
	ros::Rate loopRate(10);


	while (ros::ok()) {	
    
	if (check_node("/db1/left_wheel_encoder_node")){
		std::cout<<"trovato"<<std::endl;
	}
	else {
		std::cout<<"nisba"<<std::endl;
	}


	// finale ros
	ros::spinOnce();
	loopRate.sleep();

	} 


	return 0;
}


