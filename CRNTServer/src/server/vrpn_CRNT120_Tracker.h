#pragma once

#ifndef VRPN_BINOCULAR_CAMERA
#define VRPN_BINOCULAR_CAMERA

#include "vrpn_Tracker.h"
#include "vrpn_Connection.h"
#include "vrpn_Types.h"
#include "UDPMulticastClient.h"
#include <vector>
#include <memory>

#define RIGIDBODY_COUNT 256
#define Header_ID		0
#define WAND_ID			1

struct vrpn_CRNT120_Rigidbody_Data
{
	vrpn_uint8		rigidbody_id;
	vrpn_float64	rigidbody_position[3];
	vrpn_float64	rigidbody_quaternion[4];
};

struct vrpn_CRNT120_Data
{
	vrpn_uint32									time_stamp;
	vrpn_uint8									rigidbody_num;
	std::vector<vrpn_CRNT120_Rigidbody_Data>	rigidbody_data;
};

class vrpn_CRNT120_Tracker : public vrpn_Tracker_Server
{
public:
	vrpn_CRNT120_Tracker(const char* name, vrpn_Connection* cxn);

	~vrpn_CRNT120_Tracker();

	virtual void mainloop() override;
	
protected:
	vrpn_CRNT120_Data vrpn_Get_CRNT120_Data() const;

	struct timeval timestamp;

	UDPMulticastClient udp_client_;

	std::shared_ptr<char> data_buf_;

private:
	vrpn_CRNT120_Tracker(const vrpn_CRNT120_Tracker&);

	vrpn_CRNT120_Tracker& operator=(const vrpn_CRNT120_Tracker&);

	static vrpn_CRNT120_Data vrpn_Data_Conversion(char* orgin_data);

	std::string tracker_name_;
};

#endif