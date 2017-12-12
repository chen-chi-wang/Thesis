/** \brief The constants and configuration of algorithm.
*/

#include <stdbool.h>
// Constants
const int SMALL;
const int BIG;
const int SIZE_SMALL;
const int SIZE_BIG;
const int MAX_NUM_OF_LOW_FPS;
const int MAX_NUM_OF_HIGH_FPS;
const int RLC_HEADER[];
const int PDCP_HEADER;

// Configuration
const int HIGH_FPS_UE;
const int LOW_FPS_UE;
const int TEMPLATE_VERSION;
const int CQI_THRESHOLD;
const int UNIT_COUNTER;
const int CQI_CENTER;//9=(3+15)/2
const int IDT;
const int CQI_FIXED;

// Variables
int CHANGE_UE_FLAG;
int DROP_FLAG;
int SENT_FLAG;
int SENT_FLAG_1;
int SENT_FLAG_2;
int THE_UE;
int TEST;
int PKT_COUNTER_1;
int PKT_COUNTER_2;
int CQI_PASS;
int CTIME;
int ARR_CTIME_PASS_1[100];
int ARR_CTIME_PASS_2[100];
int RB_PASS_FLAG;//Miracle
int PRE_RB_PASS;//Miracle
// fn adjustTBS(int TBS, int template_version, int size_version);
/** \brief                    Calculate the TB size to indicate to RLC.
 *
 * \param  TBS :              Total TB size that can be used.
 * \param  template_version : The configuration that user chose.
 * \param  size_version :     The RLC SDU size should be.
 * \param  packet_count :     The counter records the number of packet until now.
 * \return integer :          The suitable TB size such that packet does not be segmented.
 *
 */
int adjustTBS(int TBS, int size_version, int packet_count);

int chooseSizeVersion(int UE_id);

int calcTrashSize(int drop_count, int max_pkt_num, int size_version);

int calcCQI(int CQI_CENTER);


