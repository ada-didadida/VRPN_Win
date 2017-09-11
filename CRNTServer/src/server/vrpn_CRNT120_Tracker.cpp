#include "vrpn_CRNT120_Tracker.h"



vrpn_CRNT120_Tracker::vrpn_CRNT120_Tracker(const char* name, vrpn_Connection* cxn) :
	vrpn_Tracker_Server(name, cxn, RIGIDBODY_COUNT)
	,udp_client_(GROUP_IP, PORT)
	,data_buf_(new char[RECIEVE_SIZE])
	,tracker_name_(name)
{
	//不再输出"No response from server"
	this->shutup = true;
}

vrpn_CRNT120_Tracker::~vrpn_CRNT120_Tracker()
{
}

void vrpn_CRNT120_Tracker::mainloop()
{
	vrpn_gettimeofday(&timestamp, nullptr);

	auto crnt120_data = vrpn_Get_CRNT120_Data();

	auto first = crnt120_data.rigidbody_data.begin();

	if(this->tracker_name_ == "Tracker")
	{
		while (first != crnt120_data.rigidbody_data.end())
		{
			if ((*first).rigidbody_id == Header_ID)
			{
				vrpn_Tracker_Server::report_pose((*first).rigidbody_id, timestamp, ((*first).rigidbody_position), (*first).rigidbody_quaternion);
			}
			++first;
		}
	}
	if(this->tracker_name_ == "Wand")
	{
		while (first != crnt120_data.rigidbody_data.end())
		{
			if ((*first).rigidbody_id == WAND_ID)
			{
				vrpn_Tracker_Server::report_pose((*first).rigidbody_id, timestamp, ((*first).rigidbody_position), (*first).rigidbody_quaternion);
			}
			++first;
		}
		
	}
}

vrpn_CRNT120_Data vrpn_CRNT120_Tracker::vrpn_Get_CRNT120_Data() const
{
	auto recieve_data_len = udp_client_.GetUDPData(data_buf_.get());
	if(recieve_data_len > 0)
	{
		return vrpn_Data_Conversion(data_buf_.get());
	}
	return vrpn_CRNT120_Data();
}

vrpn_CRNT120_Data vrpn_CRNT120_Tracker::vrpn_Data_Conversion(char* orgin_data)
{
	vrpn_CRNT120_Data crnt120_data;

	//时间戳		
	memcpy(&(crnt120_data.time_stamp), orgin_data, 4);
	orgin_data += 4;
	//刚体数量
	memcpy(&(crnt120_data.rigidbody_num), orgin_data, 1);
	orgin_data += 1;
	//刚体信息
	for (auto i = 0; i < crnt120_data.rigidbody_num; ++i)
	{
		vrpn_CRNT120_Rigidbody_Data crnt120_rigidbody_data;
		//刚体id
		memcpy(&(crnt120_rigidbody_data.rigidbody_id), orgin_data, 1);
		orgin_data += 1;
		//刚体x轴位置信息
		vrpn_float32 rigidbody_x;
		memcpy(&rigidbody_x, orgin_data, 4);
		crnt120_rigidbody_data.rigidbody_position[0] = rigidbody_x;
		orgin_data += 4;
		//刚体y轴位置信息
		vrpn_float32 rigidbody_y;
		memcpy(&rigidbody_y, orgin_data, 4);
		crnt120_rigidbody_data.rigidbody_position[1] = rigidbody_y;
		orgin_data += 4;
		//刚体z轴位置信息
		vrpn_float32 rigidbody_z;
		memcpy(&rigidbody_z, orgin_data, 4);
		crnt120_rigidbody_data.rigidbody_position[2] = rigidbody_z;
		orgin_data += 4;
		//刚体四元数信息q1
		vrpn_float32 rigidbody_q1;
		memcpy(&rigidbody_q1, orgin_data, 4);
		crnt120_rigidbody_data.rigidbody_quaternion[0] = rigidbody_q1;
		orgin_data += 4;
		//刚体四元数信息q2
		vrpn_float32 rigidbody_q2;
		memcpy(&rigidbody_q2, orgin_data, 4);
		crnt120_rigidbody_data.rigidbody_quaternion[1] = rigidbody_q2;
		orgin_data += 4;
		//刚体四元数信息q3
		vrpn_float32 rigidbody_q3;
		memcpy(&rigidbody_q3, orgin_data, 4);
		crnt120_rigidbody_data.rigidbody_quaternion[2] = rigidbody_q3;
		orgin_data += 4;
		//刚体四元数信息q4
		vrpn_float32 rigidbody_q4;
		memcpy(&rigidbody_q4, orgin_data, 4);
		crnt120_rigidbody_data.rigidbody_quaternion[3] = rigidbody_q4;
		orgin_data += 4;

		crnt120_data.rigidbody_data.push_back(crnt120_rigidbody_data);
	}

	return crnt120_data;
}
