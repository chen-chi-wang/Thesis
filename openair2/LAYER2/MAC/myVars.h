#include "utility.h"
// Constants
extern const int SMALL = 1;
extern const int BIG = 2;
extern const int SIZE_SMALL = 150;//normal:110
extern const int SIZE_BIG = 150;//idt(small,big)=(2,1) 220
extern const int MAX_NUM_OF_LOW_FPS = 45;//359 360 45
extern const int MAX_NUM_OF_HIGH_FPS = 90;//352 720 90
extern const int PDCP_HEADER = 2;
extern const int RLC_HEADER[] = {0, 2, 4, 5, 7, 8};
//(MAX_NUM_OF_LOW_FPS,MAX_NUM_OF_HIGH_FPS) = (360,720) ==> UNIT_COUNTER = 4
//'s time length is equal to
//(MAX_NUM_OF_LOW_FPS,MAX_NUM_OF_HIGH_FPS) = (45,90) ==> UNIT_COUNTER = 32


// Configuration
extern const int HIGH_FPS_UE = 1;// 
extern const int LOW_FPS_UE = 1;//
extern const int TEMPLATE_VERSION = 41;
extern const int CQI_THRESHOLD = 7;//default:9 last:7
extern const int UNIT_COUNTER = 64;//
extern const int CQI_CENTER = 6;//inter_DL_cqi     9=(3+15)/2
extern const int IDT = 2;//time_dist
extern const int CQI_FIXED = 15;//inter_DL_cqi
//Time Alignment = Y;//new_random_string
//CQI union = Y;//inter_DL_cqi 50%
//CQI fixed = N;

// Variables
extern int CHANGE_UE_FLAG = 1;//
extern int DROP_FLAG = 0;
extern int SENT_FLAG = 0;
extern int SENT_FLAG_1 = 0;
extern int SENT_FLAG_2 = 0;
extern int THE_UE = 0;
extern int TEST = 5;
extern int PKT_COUNTER_1 = 0;
extern int PKT_COUNTER_2 = 0;
extern int CQI_PASS = 0;
extern int CTIME;
extern int ARR_CTIME_PASS_1[100] = {0};
extern int ARR_CTIME_PASS_2[100] = {0};
extern int RB_PASS_FLAG=0;//Miracle
extern int PRE_RB_PASS=0;//Miracle
