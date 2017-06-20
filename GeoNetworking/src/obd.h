#ifndef OBD_H
#define OBD_H

#define OBD_DEV_PATH "/dev/ttyOBD"
#define OBD_ERROR	-1
#define VE 		90.0 	// volumetric efficiency 	UNIT:%
#define ED  		1.6 	// engine displacement  	UNIT:liters
#define MM 		28.97	// the average molecular mass of air	g/mole
#define R 		8.314   // J/K/mole
#define M2KM		1.6093  // 1 mile = 1.6093 km
#define M2M		1609.3  // 1 mile = 1609.3 meters
#define G2L		3.7854	// 1 gallon = 3.7854 liters
#define OHK		100.0	// one hundred km
#define MS2KMH		3.6	// 1m/s = 3.6Km/h			

typedef struct OBDInfo
{
	int ect;
	int rpm;
	int vss;
	int iat;
	int imap;
	int num_DTC;
	int fss; // fuel_system1_status
	float ltft;
	float fcphk;
	float ful_cspt;
	float kmeters;
	float accelt;

/*	int speed;
	int rpm;		//发动机转速
	int err_code;		//故障码
	bool is_healthy;	//是否状态良好
	float acceleration; 	//加速度
	float instan_fuel;  	//瞬时油耗
	float full_fuel;	//平均油耗
	float mileage		//里程数
*/
}OBDInfo;

extern OBDInfo obd_info;

void *thread_obd(void *m);

#endif

