/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
    included in this distribution in the file called "COPYING". If not,
    see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@lists.eurecom.fr

  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

*******************************************************************************/

/*! \file eNB_scheduler_dlsch.c
 * \brief procedures related to eNB for the DLSCH transport channel
 * \author  Navid Nikaein and Raymond Knopp
 * \date 2010 - 2014
 * \email: navid.nikaein@eurecom.fr
 * \version 1.0
 * @ingroup _mac

 */

// %EDITED%
#include "utility.h"
#include "myVars.h"

#include "assertions.h"
#include "PHY/defs.h"
#include "PHY/extern.h"

#include "SCHED/defs.h"
#include "SCHED/extern.h"

#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/proto.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#include "UTIL/OPT/opt.h"
#include "OCG.h"
#include "OCG_extern.h"
#include "RRC/LITE/extern.h"
#include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"

//#include "LAYER2/MAC/pre_processor.c"
#include "pdcp.h"

#include "SIMULATION/TOOLS/defs.h" // for taus

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

#define ENABLE_MAC_PAYLOAD_DEBUG
//#define DEBUG_eNB_SCHEDULER 1

//extern variables--------------------------------------------------------------
// Constants
extern const int SMALL;
extern const int BIG;
extern const int SIZE_SMALL;
extern const int SIZE_BIG;
extern const int MAX_NUM_OF_LOW_FPS;
extern const int MAX_NUM_OF_HIGH_FPS;
extern const int RLC_HEADER[];
extern const int PDCP_HEADER;

// Configuration
extern const int HIGH_FPS_UE;
extern const int LOW_FPS_UE;
extern const int TEMPLATE_VERSION;
extern const int CQI_THRESHOLD;

// Variables
extern int CHANGE_UE_FLAG;
extern int DROP_FLAG;
extern int SENT_FLAG;
extern int SENT_FLAG_1;
extern int SENT_FLAG_2;
extern int THE_UE;
extern int PKT_COUTNER_1;
extern int CTIME;
extern int ARR_CTIME_PASS_1[100];//UE_id=0
extern int ARR_CTIME_PASS_2[100];//UE_id=1
extern int RB_PASS_FLAG;//Miracle
extern int PRE_RB_PASS;

int ue_1_sent_counter=0;
int ue_2_sent_counter=0;
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void
add_ue_dlsch_info(
  module_id_t module_idP,
  int CC_id,
  int UE_id,
  sub_frame_t subframeP,
  UE_DLSCH_STATUS status
)
//------------------------------------------------------------------------------
{

  eNB_dlsch_info[module_idP][CC_id][UE_id].rnti             = UE_RNTI(module_idP,UE_id);
  //  eNB_dlsch_info[module_idP][CC_id][ue_mod_idP].weight           = weight;
  eNB_dlsch_info[module_idP][CC_id][UE_id].subframe         = subframeP;
  eNB_dlsch_info[module_idP][CC_id][UE_id].status           = status;

  eNB_dlsch_info[module_idP][CC_id][UE_id].serving_num++;

}

//------------------------------------------------------------------------------
int
schedule_next_dlue(
  module_id_t module_idP,
  int CC_id,
  sub_frame_t subframeP
)
//------------------------------------------------------------------------------
{

  int next_ue;
  UE_list_t *UE_list=&eNB_mac_inst[module_idP].UE_list;

  for (next_ue=UE_list->head; next_ue>=0; next_ue=UE_list->next[next_ue] ) {
    if  (eNB_dlsch_info[module_idP][CC_id][next_ue].status == S_DL_WAITING) {
      return next_ue;
    }
  }

  for (next_ue=UE_list->head; next_ue>=0; next_ue=UE_list->next[next_ue] ) {
    if  (eNB_dlsch_info[module_idP][CC_id][next_ue].status == S_DL_BUFFERED) {
      eNB_dlsch_info[module_idP][CC_id][next_ue].status = S_DL_WAITING;
    }
  }

  return(-1);//next_ue;

}

//------------------------------------------------------------------------------
unsigned char
generate_dlsch_header(
  unsigned char* mac_header,
  unsigned char num_sdus,
  unsigned short *sdu_lengths,
  unsigned char *sdu_lcids,
  unsigned char drx_cmd,
  short timing_advance_cmd,
  unsigned char *ue_cont_res_id,
  unsigned char short_padding,
  unsigned short post_padding
)
//------------------------------------------------------------------------------
{

  SCH_SUBHEADER_FIXED *mac_header_ptr = (SCH_SUBHEADER_FIXED *)mac_header;
  uint8_t first_element=0,last_size=0,i;
  uint8_t mac_header_control_elements[16],*ce_ptr;

  ce_ptr = &mac_header_control_elements[0];

  // compute header components

  if ((short_padding == 1) || (short_padding == 2)) {
    mac_header_ptr->R    = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = SHORT_PADDING;
    first_element=1;
    last_size=1;
  }

  if (short_padding == 2) {
    mac_header_ptr->E = 1;
    mac_header_ptr++;
    mac_header_ptr->R = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = SHORT_PADDING;
    last_size=1;
  }

  if (drx_cmd != 255) {
    if (first_element>0) {
      mac_header_ptr->E = 1;
      mac_header_ptr++;
    } else {
      first_element=1;
    }

    mac_header_ptr->R = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = DRX_CMD;
    last_size=1;
  }

  if (timing_advance_cmd != 0) {
    if (first_element>0) {
      mac_header_ptr->E = 1;
      mac_header_ptr++;
    } else {
      first_element=1;
    }

    mac_header_ptr->R = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = TIMING_ADV_CMD;
    last_size=1;
    //    msg("last_size %d,mac_header_ptr %p\n",last_size,mac_header_ptr);
    ((TIMING_ADVANCE_CMD *)ce_ptr)->R=0;
    ((TIMING_ADVANCE_CMD *)ce_ptr)->TA=(timing_advance_cmd+31)&0x3f;
    LOG_D(MAC,"timing advance =%d (%d)\n",timing_advance_cmd,((TIMING_ADVANCE_CMD *)ce_ptr)->TA);
    ce_ptr+=sizeof(TIMING_ADVANCE_CMD);
    //msg("offset %d\n",ce_ptr-mac_header_control_elements);
  }

  if (ue_cont_res_id) {
    if (first_element>0) {
      mac_header_ptr->E = 1;
      /*
      printf("[eNB][MAC] last subheader : %x (R%d,E%d,LCID%d)\n",*(unsigned char*)mac_header_ptr,
      ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->R,
      ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->E,
      ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->LCID);
      */
      mac_header_ptr++;
    } else {
      first_element=1;
    }

    mac_header_ptr->R = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = UE_CONT_RES;
    last_size=1;

    LOG_T(MAC,"[eNB ][RAPROC] Generate contention resolution msg: %x.%x.%x.%x.%x.%x\n",
          ue_cont_res_id[0],
          ue_cont_res_id[1],
          ue_cont_res_id[2],
          ue_cont_res_id[3],
          ue_cont_res_id[4],
          ue_cont_res_id[5]);

    memcpy(ce_ptr,ue_cont_res_id,6);
    ce_ptr+=6;
    // msg("(cont_res) : offset %d\n",ce_ptr-mac_header_control_elements);
  }

  //msg("last_size %d,mac_header_ptr %p\n",last_size,mac_header_ptr);

  for (i=0; i<num_sdus; i++) {
    LOG_T(MAC,"[eNB] Generate DLSCH header num sdu %d len sdu %d\n",num_sdus, sdu_lengths[i]);

    if (first_element>0) {
      mac_header_ptr->E = 1;
      /*msg("last subheader : %x (R%d,E%d,LCID%d)\n",*(unsigned char*)mac_header_ptr,
      ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->R,
      ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->E,
      ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->LCID);
      */
      mac_header_ptr+=last_size;
      //msg("last_size %d,mac_header_ptr %p\n",last_size,mac_header_ptr);
    } else {
      first_element=1;
    }

    if (sdu_lengths[i] < 128) {
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->R    = 0;
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->E    = 0;
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->F    = 0;
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->LCID = sdu_lcids[i];
      ((SCH_SUBHEADER_SHORT *)mac_header_ptr)->L    = (unsigned char)sdu_lengths[i];
      last_size=2;
    } else {
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->R    = 0;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->E    = 0;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->F    = 1;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->LCID = sdu_lcids[i];
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_MSB    = ((unsigned short) sdu_lengths[i]>>8)&0x7f;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_LSB    = (unsigned short) sdu_lengths[i]&0xff;
      ((SCH_SUBHEADER_LONG *)mac_header_ptr)->padding   = 0x00;
      last_size=3;
#ifdef DEBUG_HEADER_PARSING
      LOG_D(MAC,"[eNB] generate long sdu, size %x (MSB %x, LSB %x)\n",
            sdu_lengths[i],
            ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_MSB,
            ((SCH_SUBHEADER_LONG *)mac_header_ptr)->L_LSB);
#endif
    }
  }

  /*

    printf("last_size %d,mac_header_ptr %p\n",last_size,mac_header_ptr);

    printf("last subheader : %x (R%d,E%d,LCID%d)\n",*(unsigned char*)mac_header_ptr,
    ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->R,
    ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->E,
    ((SCH_SUBHEADER_FIXED *)mac_header_ptr)->LCID);


    if (((SCH_SUBHEADER_FIXED*)mac_header_ptr)->LCID < UE_CONT_RES) {
    if (((SCH_SUBHEADER_SHORT*)mac_header_ptr)->F == 0)
    printf("F = 0, sdu len (L field) %d\n",(((SCH_SUBHEADER_SHORT*)mac_header_ptr)->L));
    else
    printf("F = 1, sdu len (L field) %d\n",(((SCH_SUBHEADER_LONG*)mac_header_ptr)->L));
    }
  */
  if (post_padding>0) {// we have lots of padding at the end of the packet
    mac_header_ptr->E = 1;
    mac_header_ptr+=last_size;
    // add a padding element
    mac_header_ptr->R    = 0;
    mac_header_ptr->E    = 0;
    mac_header_ptr->LCID = SHORT_PADDING;
    mac_header_ptr++;
  } else { // no end of packet padding
    // last SDU subhead is of fixed type (sdu length implicitly to be computed at UE)
    mac_header_ptr++;
  }

  //msg("After subheaders %d\n",(uint8_t*)mac_header_ptr - mac_header);

  if ((ce_ptr-mac_header_control_elements) > 0) {
    // printf("Copying %d bytes for control elements\n",ce_ptr-mac_header_control_elements);
    memcpy((void*)mac_header_ptr,mac_header_control_elements,ce_ptr-mac_header_control_elements);
    mac_header_ptr+=(unsigned char)(ce_ptr-mac_header_control_elements);
  }

  //msg("After CEs %d\n",(uint8_t*)mac_header_ptr - mac_header);

  return((unsigned char*)mac_header_ptr - mac_header);

}

//------------------------------------------------------------------------------
void
set_ul_DAI(
  int module_idP,
  int UE_idP,
  int CC_idP,
  int frameP,
  int subframeP,
  LTE_DL_FRAME_PARMS*  frame_parms[MAX_NUM_CCs]
)
//------------------------------------------------------------------------------
{

  eNB_MAC_INST         *eNB      = &eNB_mac_inst[module_idP];
  UE_list_t            *UE_list  = &eNB->UE_list;
  unsigned char         DAI;

  if (frame_parms[CC_idP]->frame_type == TDD) {
    DAI = (UE_list->UE_template[CC_idP][UE_idP].DAI-1)&3;
    LOG_D(MAC,"[eNB %d] CC_id %d Frame %d, subframe %d: DAI %d for UE %d\n",module_idP,CC_idP,frameP,subframeP,DAI,UE_idP);
    // Save DAI for Format 0 DCI

    switch (frame_parms[CC_idP]->tdd_config) {
    case 0:
      //      if ((subframeP==0)||(subframeP==1)||(subframeP==5)||(subframeP==6))
      break;

    case 1:
      switch (subframeP) {
      case 1:
        UE_list->UE_template[CC_idP][UE_idP].DAI_ul[7] = DAI;
        break;

      case 4:
        UE_list->UE_template[CC_idP][UE_idP].DAI_ul[8] = DAI;
        break;

      case 6:
        UE_list->UE_template[CC_idP][UE_idP].DAI_ul[2] = DAI;
        break;

      case 9:
        UE_list->UE_template[CC_idP][UE_idP].DAI_ul[3] = DAI;
        break;
      }

    case 2:
      //      if ((subframeP==3)||(subframeP==8))
      //  UE_list->UE_template[CC_idP][UE_idP].DAI_ul = DAI;
      break;

    case 3:

      //if ((subframeP==6)||(subframeP==8)||(subframeP==0)) {
      //  LOG_D(MAC,"schedule_ue_spec: setting UL DAI to %d for subframeP %d => %d\n",DAI,subframeP, ((subframeP+8)%10)>>1);
      //  UE_list->UE_template[CC_idP][UE_idP].DAI_ul[((subframeP+8)%10)>>1] = DAI;
      //}
      switch (subframeP) {
      case 5:
      case 6:
      case 1:
        UE_list->UE_template[CC_idP][UE_idP].DAI_ul[2] = DAI;
        break;

      case 7:
      case 8:
        UE_list->UE_template[CC_idP][UE_idP].DAI_ul[3] = DAI;
        break;

      case 9:
      case 0:
        UE_list->UE_template[CC_idP][UE_idP].DAI_ul[4] = DAI;
        break;

      default:
        break;
      }

      break;

    case 4:
      //      if ((subframeP==8)||(subframeP==9))
      //  UE_list->UE_template[CC_idP][UE_idP].DAI_ul = DAI;
      break;

    case 5:
      //      if (subframeP==8)
      //  UE_list->UE_template[CC_idP][UE_idP].DAI_ul = DAI;
      break;

    case 6:
      //      if ((subframeP==1)||(subframeP==4)||(subframeP==6)||(subframeP==9))
      //  UE_list->UE_template[CC_idP][UE_idP].DAI_ul = DAI;
      break;

    default:
      break;
    }
  }
}
int retran_counter=0;//EDITED
//Global Variables
int packet_count = 0;
int drop_count = 0;
int packet_count_1 = 0;//single UE or small UE of Compare group
int packet_count_2 = 0;
int packet_count_debug = 0;
int lock = 1;
int unit_time_counter_1_1 = 0;//UE_id=0
int unit_time_counter_1_2 = 0;//UE_id=1
int unit_time_counter_1_3 = 0;//THE_UE
int unit_time_counter_1_4 = 0;//!THE_UE
int arr_harq_1[8];
int arr_harq_2[8];
//Global Variables
//------------------------------------------------------------------------------
void
schedule_ue_spec(
  module_id_t   module_idP,
  frame_t       frameP,
  sub_frame_t   subframeP,
  int*          mbsfn_flag
)
//------------------------------------------------------------------------------
{

  uint8_t               CC_id;
  int                   UE_id;
  int                   N_RBG[MAX_NUM_CCs];
  unsigned char         aggregation;
  mac_rlc_status_resp_t rlc_status;
  unsigned char         header_len_dcch=0, header_len_dcch_tmp=0,header_len_dtch=0,header_len_dtch_tmp=0, ta_len=0;
  unsigned char         sdu_lcids[11],offset,num_sdus=0;
  uint16_t              nb_rb,nb_rb_temp,total_nb_available_rb[MAX_NUM_CCs],nb_available_rb;
  uint16_t              TBS,j,sdu_lengths[11],rnti,padding=0,post_padding=0;
  unsigned char         dlsch_buffer[MAX_DLSCH_PAYLOAD_BYTES];
  unsigned char         round            = 0;
  unsigned char         harq_pid         = 0;
  void                 *DLSCH_dci        = NULL;
  LTE_eNB_UE_stats     *eNB_UE_stats     = NULL;
  uint16_t              sdu_length_total = 0;
  //  uint8_t               dl_pow_off[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
  //  unsigned char         rballoc_sub_UE[MAX_NUM_CCs][NUMBER_OF_UE_MAX][N_RBG_MAX];
  //  uint16_t              pre_nb_available_rbs[MAX_NUM_CCs][NUMBER_OF_UE_MAX];
  int                   mcs;
  uint16_t              min_rb_unit[MAX_NUM_CCs];
  eNB_MAC_INST         *eNB      = &eNB_mac_inst[module_idP];
  UE_list_t            *UE_list  = &eNB->UE_list;
  LTE_DL_FRAME_PARMS   *frame_parms[MAX_NUM_CCs];
  int                   continue_flag=0;
  int32_t                 normalized_rx_power, target_rx_power;
  int32_t                 tpc=1;
  static int32_t          tpc_accumulated=0;
  UE_sched_ctrl           *ue_sched_ctl;
  int i;
  int DTCH_ind_SIZE=0;

  if (UE_list->head==-1) {
    return;
  }

  start_meas(&eNB->schedule_dlsch);
  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_SCHEDULE_DLSCH,VCD_FUNCTION_IN);

  //weight = get_ue_weight(module_idP,UE_id);
  aggregation = 1; // set to the maximum aggregation level

  for (CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
    min_rb_unit[CC_id]=get_min_rb_unit(module_idP,CC_id);
    frame_parms[CC_id] = mac_xface->get_lte_frame_parms(module_idP,CC_id);
    // get number of PRBs less those used by common channels
    total_nb_available_rb[CC_id] = frame_parms[CC_id]->N_RB_DL;
    for (i=0;i<frame_parms[CC_id]->N_RB_DL;i++)
      if (eNB->common_channels[CC_id].vrb_map[i]!=0)
	total_nb_available_rb[CC_id]--;

    N_RBG[CC_id] = frame_parms[CC_id]->N_RBG;

    // store the global enb stats:
    eNB->eNB_stats[CC_id].num_dlactive_UEs =  UE_list->num_UEs;printf("UE_list length=%d\n",UE_list->num_UEs);//EDITED
    eNB->eNB_stats[CC_id].available_prbs =  total_nb_available_rb[CC_id];
    eNB->eNB_stats[CC_id].total_available_prbs +=  total_nb_available_rb[CC_id];
    eNB->eNB_stats[CC_id].dlsch_bytes_tx=0;
    eNB->eNB_stats[CC_id].dlsch_pdus_tx=0;
  }

  /// CALLING Pre_Processor for downlink scheduling (Returns estimation of RBs required by each UE and the allocation on sub-band)

  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_PREPROCESSOR,VCD_FUNCTION_IN);
  start_meas(&eNB->schedule_dlsch_preprocessor);
  //TODO
  dlsch_scheduler_pre_processor(module_idP,
                                frameP,
                                subframeP,
                                N_RBG,
                                mbsfn_flag);
  stop_meas(&eNB->schedule_dlsch_preprocessor);
   
  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_DLSCH_PREPROCESSOR,VCD_FUNCTION_OUT);


  for (CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
    LOG_D(MAC, "doing schedule_ue_spec for CC_id %d\n",CC_id);

    if (mbsfn_flag[CC_id]>0)
      continue;

    for (UE_id=UE_list->head; UE_id>=0; UE_id=UE_list->next[UE_id]) {
      continue_flag=0; // reset the flag to allow allocation for the remaining UEs
      rnti = UE_RNTI(module_idP,UE_id);
      eNB_UE_stats = mac_xface->get_eNB_UE_stats(module_idP,CC_id,rnti);
      ue_sched_ctl = &UE_list->UE_sched_ctrl[UE_id];//for line no. 1438 767

      if (rnti==NOT_A_RNTI) {
        LOG_I(MAC,"Cannot find rnti for UE_id %d (num_UEs %d)\n",UE_id,UE_list->num_UEs);
        // mac_xface->macphy_exit("Cannot find rnti for UE_id");
        continue_flag=1;
      }

      if (eNB_UE_stats==NULL) {
        LOG_I(MAC,"[eNB] Cannot find eNB_UE_stats\n");
        //  mac_xface->macphy_exit("[MAC][eNB] Cannot find eNB_UE_stats\n");
        continue_flag=1;
      }
     
//	  printf("RB debug, in dlsch, pre_nb_available_rbs=%d CCE_allocation_feasible=%d\n",ue_sched_ctl->pre_nb_available_rbs[CC_id],!CCE_allocation_infeasible(module_idP,CC_id,0,subframeP,aggregation,rnti));//Scary... do not use...
      if ((ue_sched_ctl->pre_nb_available_rbs[CC_id] == 0) ||  // no RBs allocated, pre_nb_available_rbs=nb_available_rb 
	  CCE_allocation_infeasible(module_idP,CC_id,0,subframeP,aggregation,rnti)
	  ) {//if(ue_sched_ctl->pre_nb_available_rbs[CC_id] != 0){puts("CCE has problem!!!");}//CCE will never has problem.
		//printf("\nno RB allocated hit\n");
        LOG_I(MAC,"[eNB %d] Frame %d : no RB allocated for UE_instance %d on CC_id %d: continue \n",
              module_idP, frameP, UE_id+1, CC_id);
        
        if(HIGH_FPS_UE&&LOW_FPS_UE&&UE_id==!THE_UE&&RB_PASS_FLAG==1){//Miracle
            //do nothing
        }else{
            //original code 

            //if(mac_xface->get_transmission_mode(module_idP,rnti)==5)
            continue_flag=1; //to next user (there might be rbs availiable for other UEs in TM5
            // else
            //  break;
        }

      }else{
		//printf("\nyes RB allocated hit\n");
	  }

      if (frame_parms[CC_id]->frame_type == TDD)  {
        set_ue_dai (subframeP,
                    frame_parms[CC_id]->tdd_config,
                    UE_id,
                    CC_id,
                    UE_list);
        // update UL DAI after DLSCH scheduling
        set_ul_DAI(module_idP,UE_id,CC_id,frameP,subframeP,frame_parms);
      }
		
		printf("continue flag : %d, UE_instance : %d\n", continue_flag, UE_id+1);//before Retran and new SDU

      if (continue_flag == 1 ) {
        add_ue_dlsch_info(module_idP,
                          CC_id,
                          UE_id,
                          subframeP,
                          S_DL_NONE);
        continue;
      }
      nb_available_rb = ue_sched_ctl->pre_nb_available_rbs[CC_id];
      //printf("RB debug, in dlsch, nb_available_rb=pre_nb_available_rbs=%d\n",nb_available_rb);
      harq_pid = ue_sched_ctl->harq_pid[CC_id];printf("RB debug HARQ debug dlsch UE %d harq_pid=%d\n",UE_id+1,harq_pid);
      round = ue_sched_ctl->round[CC_id];
      UE_list->eNB_UE_stats[CC_id][UE_id].crnti= rnti;
      UE_list->eNB_UE_stats[CC_id][UE_id].rrc_status=mac_eNB_get_rrc_status(module_idP,rnti);
      UE_list->eNB_UE_stats[CC_id][UE_id].harq_pid = harq_pid; 
      UE_list->eNB_UE_stats[CC_id][UE_id].harq_round = round;

      sdu_length_total=0;
      num_sdus=0;

      /*
      DevCheck(((eNB_UE_stats->DL_cqi[0] < MIN_CQI_VALUE) || (eNB_UE_stats->DL_cqi[0] > MAX_CQI_VALUE)),
      eNB_UE_stats->DL_cqi[0], MIN_CQI_VALUE, MAX_CQI_VALUE);
      */
      eNB_UE_stats->dlsch_mcs1 = cqi_to_mcs[eNB_UE_stats->DL_cqi[0]];

      eNB_UE_stats->dlsch_mcs1 = cmin(eNB_UE_stats->dlsch_mcs1, openair_daq_vars.target_ue_dl_mcs);


#ifdef EXMIMO

      if (mac_xface->get_transmission_mode(module_idP,CC_id, rnti)==5) {
        eNB_UE_stats->dlsch_mcs1 = cmin(eNB_UE_stats->dlsch_mcs1,16);
      }

#endif

      // store stats
      UE_list->eNB_UE_stats[CC_id][UE_id].dl_cqi= eNB_UE_stats->DL_cqi[0];

      // initializing the rb allocation indicator for each UE
      for(j=0; j<frame_parms[CC_id]->N_RBG; j++) {
        UE_list->UE_template[CC_id][UE_id].rballoc_subband[harq_pid][j] = 0;
      }

      LOG_D(MAC,"[eNB %d] Frame %d: Scheduling UE %d on CC_id %d (rnti %x, harq_pid %d, round %d, rb %d, cqi %d, mcs %d, rrc %d)\n",
            module_idP, frameP, UE_id,CC_id,rnti,harq_pid, round,nb_available_rb,
            eNB_UE_stats->DL_cqi[0], eNB_UE_stats->dlsch_mcs1,
	    UE_list->eNB_UE_stats[CC_id][UE_id].rrc_status);


      // Note this code is for a specific DCI format
      DLSCH_dci = (void *)UE_list->UE_template[CC_id][UE_id].DLSCH_DCI[harq_pid];


      /* process retransmission  */

      if (round > 0) {

		// %EDITED%
		printf("Retransmission! UE_instance : %d\n", UE_id+1);
//		if(PKT_COUNTER_1>=1 && PKT_COUNTER_1<=1408){
//			printf("retran_counter=%d\n",++retran_counter);
//		}


        if (frame_parms[CC_id]->frame_type == TDD) {//hit
          UE_list->UE_template[CC_id][UE_id].DAI++;
          update_ul_dci(module_idP,CC_id,rnti,UE_list->UE_template[CC_id][UE_id].DAI);
          LOG_D(MAC,"DAI update: CC_id %d subframeP %d: UE %d, DAI %d\n",
                CC_id,subframeP,UE_id,UE_list->UE_template[CC_id][UE_id].DAI);
        }

        // get freq_allocation
        nb_rb = UE_list->UE_template[CC_id][UE_id].nb_rb[harq_pid];
		printf("RB debug HARQ debug Retransmission! UE=%d\t harq_pid=%d nb_rb=%d nb_available_rb=%d ",UE_id+1,harq_pid,nb_rb,nb_available_rb);
        if (nb_rb <= nb_available_rb) {
		  //printf("RB debug nb_rb=%d pre_nb_available_rbs=%d\n",nb_rb,ue_sched_ctl->pre_nb_available_rbs[CC_id]);
          if(nb_rb == ue_sched_ctl->pre_nb_available_rbs[CC_id]) {//hit
            for(j=0; j<frame_parms[CC_id]->N_RBG; j++) { // for indicating the rballoc for each sub-band
              UE_list->UE_template[CC_id][UE_id].rballoc_subband[harq_pid][j] = ue_sched_ctl->rballoc_sub_UE[CC_id][j];
            }
          } else {//not hit
            nb_rb_temp = nb_rb;
            j = 0;

            while((nb_rb_temp > 0) && (j<frame_parms[CC_id]->N_RBG)) {
              if(ue_sched_ctl->rballoc_sub_UE[CC_id][j] == 1) {
                UE_list->UE_template[CC_id][UE_id].rballoc_subband[harq_pid][j] = ue_sched_ctl->rballoc_sub_UE[CC_id][j];

                if((j == frame_parms[CC_id]->N_RBG-1) &&
                    ((frame_parms[CC_id]->N_RB_DL == 25)||
                     (frame_parms[CC_id]->N_RB_DL == 50))) {
                  nb_rb_temp = nb_rb_temp - min_rb_unit[CC_id]+1;
                } else {
				  printf("RB debug temp=%d\n",nb_rb_temp);
                  nb_rb_temp = nb_rb_temp - min_rb_unit[CC_id];
                  printf("RB debug temp-=%d\n",nb_rb_temp);
                }
              }

              j = j+1;
            }
          }

          nb_available_rb -= nb_rb;
          aggregation = process_ue_cqi(module_idP,UE_id);

          PHY_vars_eNB_g[module_idP][CC_id]->mu_mimo_mode[UE_id].pre_nb_available_rbs = nb_rb;
          PHY_vars_eNB_g[module_idP][CC_id]->mu_mimo_mode[UE_id].dl_pow_off = ue_sched_ctl->dl_pow_off[CC_id];

          for(j=0; j<frame_parms[CC_id]->N_RBG; j++) {
            PHY_vars_eNB_g[module_idP][CC_id]->mu_mimo_mode[UE_id].rballoc_sub[j] = UE_list->UE_template[CC_id][UE_id].rballoc_subband[harq_pid][j];
          }

          switch (mac_xface->get_transmission_mode(module_idP,CC_id,rnti)) {
          case 1:
          case 2:
          default:
            switch (frame_parms[CC_id]->N_RB_DL) {
            case 6:
              if (frame_parms[CC_id]->frame_type == TDD) {
                //        ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
                ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
                ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
                LOG_D(MAC,"[eNB %d] Retransmission CC_id %d : harq_pid %d, round %d, dai %d, mcs %d\n",
                      module_idP,CC_id,harq_pid,round,(UE_list->UE_template[CC_id][UE_id].DAI-1),
                      ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->mcs);
              } else {
                //        ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
                ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
                LOG_D(MAC,"[eNB %d] Retransmission CC_id %d : harq_pid %d, round %d, mcs %d\n",
                      module_idP,CC_id,harq_pid,round,((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->mcs);

              }

              break;

            case 25://hit
              if (frame_parms[CC_id]->frame_type == TDD) {
                //        ((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
				if(UE_id==0){
						arr_harq_1[harq_pid]++;
						printf("harq retransmission %d times\n",arr_harq_1[harq_pid]);//7 at most
				}else{
						arr_harq_2[harq_pid]++;
						printf("harq retransmission %d times\n",arr_harq_2[harq_pid]);//7 at most
				}
//				printf("harq retransmission %d times\n",++arr_harq[harq_pid]);//7 at most
                LOG_I(MAC,"[eNB %d] RB debug HARQ debug Retransmission CC_id %d : harq_pid %d, round %d, dai %d, mcs %d\n",
                      module_idP,CC_id,harq_pid,round,(UE_list->UE_template[CC_id][UE_id].DAI-1),
                      ((DCI1_5MHz_TDD_t*)DLSCH_dci)->mcs);//this round is correct
              } else {
                //        ((DCI1_5MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
                LOG_I(MAC,"[eNB %d] Retransmission CC_id %d : harq_pid %d, round %d, mcs %d\n",
                      module_idP,CC_id,harq_pid,round,((DCI1_5MHz_FDD_t*)DLSCH_dci)->mcs);

              }

              break;

            case 50:
              if (frame_parms[CC_id]->frame_type == TDD) {
                //        ((DCI1_10MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
                ((DCI1_10MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
                ((DCI1_10MHz_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
                LOG_I(MAC,"[eNB %d] Retransmission CC_id %d : harq_pid %d, round %d, dai %d, mcs %d\n",
                      module_idP,CC_id,harq_pid,round,(UE_list->UE_template[CC_id][UE_id].DAI-1),
                      ((DCI1_10MHz_TDD_t*)DLSCH_dci)->mcs);
              } else {
                //        ((DCI1_10MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
                ((DCI1_10MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
                LOG_I(MAC,"[eNB %d] Retransmission CC_id %d : harq_pid %d, round %d, mcs %d\n",
                      module_idP,CC_id,harq_pid,round,((DCI1_10MHz_FDD_t*)DLSCH_dci)->mcs);

              }

              break;

            case 100:
              if (frame_parms[CC_id]->frame_type == TDD) {
                //        ((DCI1_20MHz_TDD_t*)DLSCH_dci)->ndi      = 0;
                ((DCI1_20MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rv       = round&3;
                ((DCI1_20MHz_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
                LOG_D(MAC,"[eNB %d] Retransmission CC_id %d : harq_pid %d, round %d, dai %d, mcs %d\n",
                      module_idP,CC_id,harq_pid,round,(UE_list->UE_template[CC_id][UE_id].DAI-1),
                      ((DCI1_20MHz_TDD_t*)DLSCH_dci)->mcs);
              } else {
                //        ((DCI1_20MHz_FDD_t*)DLSCH_dci)->ndi      = 0;
                ((DCI1_20MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rv       = round&3;
                LOG_D(MAC,"[eNB %d] Retransmission CC_id %d : harq_pid %d, round %d, mcs %d\n",
                      module_idP,CC_id,harq_pid,round,((DCI1_20MHz_FDD_t*)DLSCH_dci)->mcs);

              }

              break;
            }

            break;

          case 4:
            //    if (nb_rb>10) {
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->ndi1 = 0;
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->dai = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
            // }
            //else {
            //  ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->ndi1 = 0;
            // ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
            // ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->rv1 = round&3;
            // ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->dai = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
            // }
            break;

          case 5:
            // if(nb_rb>10){
            //((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->mcs = eNB_UE_stats->DL_cqi[0]<<1;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 0;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;

            if(ue_sched_ctl->dl_pow_off[CC_id] == 2) {
              ue_sched_ctl->dl_pow_off[CC_id] = 1;
            }

            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = ue_sched_ctl->dl_pow_off[CC_id];
            // }
            break;

          case 6:
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 0;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = 1;//dl_pow_off[UE_id];
            break;
          }

          add_ue_dlsch_info(module_idP,
                            CC_id,
                            UE_id,
                            subframeP,
                            S_DL_SCHEDULED);

          //eNB_UE_stats->dlsch_trials[round]++;
          UE_list->eNB_UE_stats[CC_id][UE_id].num_retransmission+=1;
          UE_list->eNB_UE_stats[CC_id][UE_id].rbs_used_retx=nb_rb;
          UE_list->eNB_UE_stats[CC_id][UE_id].total_rbs_used_retx+=nb_rb;
          UE_list->eNB_UE_stats[CC_id][UE_id].dlsch_mcs1=eNB_UE_stats->dlsch_mcs1;
          UE_list->eNB_UE_stats[CC_id][UE_id].dlsch_mcs2=eNB_UE_stats->dlsch_mcs1;
        } else {
	
          LOG_I(MAC,"[eNB %d] Frame %d CC_id %d : don't schedule UE %d, its retransmission takes more resources than we have\nnb_rb=%d > nb_available_rb=%d\n",module_idP, frameP, CC_id, UE_id+1,nb_rb, nb_available_rb);
        }
      } else { /* This is a potentially new SDU opportunity */

        rlc_status.bytes_in_buffer = 0;
        // Now check RLC information to compute number of required RBs
        // get maximum TBS size for RLC request
        //TBS = mac_xface->get_TBS(eNB_UE_stats->DL_cqi[0]<<1,nb_available_rb);



		// %EDITED%
		//TBS = 530;
        if(HIGH_FPS_UE&&LOW_FPS_UE&&UE_id==!THE_UE&&RB_PASS_FLAG==1){//Miracle
            //TBS = mac_xface->get_TBS_DL(eNB_UE_stats->dlsch_mcs1,PRE_RB_PASS);
            TBS = mac_xface->get_TBS_DL(16,PRE_RB_PASS);
        }else{ 
            TBS = mac_xface->get_TBS_DL(eNB_UE_stats->dlsch_mcs1,nb_available_rb);//LC shared rb
        }
		printf("\nNew SDU, CQI=%d, dlsch_mcs1=%d, nb_available_rb=pre_nb_available_rbs=%d, TBS=%d\n",eNB_UE_stats->DL_cqi[0],eNB_UE_stats->dlsch_mcs1,nb_available_rb,TBS);

        // check first for RLC data on DCCH
        // add the length for  all the control elements (timing adv, drx, etc) : header + payload

        ta_len = (ue_sched_ctl->ta_update!=0) ? 2 : 0;

        header_len_dcch = 2; // 2 bytes DCCH SDU subheader

        if ( TBS-ta_len-header_len_dcch > 0 ) {
          rlc_status = mac_rlc_status_ind(
                         module_idP,
                         rnti,
			 module_idP,
                         frameP,
                         ENB_FLAG_YES,
                         MBMS_FLAG_NO,
                         DCCH,
                         (TBS-ta_len-header_len_dcch)); // transport block set size
		  printf("RB debug Bytes in DCCH RLC buffer : %d\n", rlc_status.bytes_in_buffer);
// 		  printf("\nTBS=%d, ta_len=%d, header_len_dcch=%d",TBS, ta_len, header_len_dcch); printf("\npayload request DCCH = %d\n",(TBS-ta_len-header_len_dcch));
          sdu_lengths[0]=0;

          if (rlc_status.bytes_in_buffer > 0) {  // There is DCCH to transmit
            LOG_D(MAC,"[eNB %d] Frame %d, DL-DCCH->DLSCH CC_id %d, Requesting %d bytes from RLC (RRC message)\n",
                  module_idP,frameP,CC_id,TBS-header_len_dcch);
            sdu_lengths[0] += mac_rlc_data_req(
                                module_idP,
                                rnti,
				module_idP,
                                frameP,
                                ENB_FLAG_YES,
                                MBMS_FLAG_NO,
                                DCCH,
                                (char *)&dlsch_buffer[sdu_lengths[0]]);
            printf("\nOriginally, MAC's dlsch buffer has been occupied by %d bytes\n",0);printf("\nRB debug RLC send %d bytes for DCCH\n",sdu_lengths[0]);
            LOG_D(MAC,"[eNB %d][DCCH] CC_id %d Got %d bytes from RLC\n",module_idP,CC_id,sdu_lengths[0]);
            sdu_length_total = sdu_lengths[0];printf("\nsdu_length_total=%d\n",sdu_length_total);
            sdu_lcids[0] = DCCH;
            UE_list->eNB_UE_stats[CC_id][UE_id].num_pdu_tx[DCCH]+=1;
            UE_list->eNB_UE_stats[CC_id][UE_id].num_bytes_tx[DCCH]+=sdu_lengths[0];
            num_sdus = 1;
#ifdef DEBUG_eNB_SCHEDULER
            LOG_T(MAC,"[eNB %d][DCCH] CC_id %d Got %d bytes :",module_idP,CC_id,sdu_lengths[0]);

            for (j=0; j<sdu_lengths[0]; j++) {
              LOG_T(MAC,"%x ",dlsch_buffer[j]);
            }

            LOG_T(MAC,"\n");
#endif
          } else {
            header_len_dcch = 0;
            sdu_length_total = 0;
          }
        }

        // check for DCCH1 and update header information (assume 2 byte sub-header)
        if (TBS-ta_len-header_len_dcch-sdu_length_total > 0 ) {
          rlc_status = mac_rlc_status_ind(
                         module_idP,
                         rnti,
			 module_idP,
                         frameP,
                         ENB_FLAG_YES,
                         MBMS_FLAG_NO,
                         DCCH+1,
                         (TBS-ta_len-header_len_dcch-sdu_length_total)); // transport block set size less allocations for timing advance and
		 printf("RB debug Bytes in DCCH1 RLC buffer : %d\n", rlc_status.bytes_in_buffer); 	
          // DCCH SDU
//          printf("\nTBS=%d, ta_len=%d, header_len_dcch=%d, sdu_length_total=%d",TBS, ta_len, header_len_dcch, sdu_length_total);printf("\npayload request DCCH1 = %d\n\n",(TBS-ta_len-header_len_dcch-sdu_length_total));
          if (rlc_status.bytes_in_buffer > 0) {
            LOG_D(MAC,"[eNB %d], Frame %d, DCCH1->DLSCH, CC_id %d, Requesting %d bytes from RLC (RRC message)\n",
                  module_idP,frameP,CC_id,TBS-header_len_dcch-sdu_length_total);
            sdu_lengths[num_sdus] += mac_rlc_data_req(
                                       module_idP,
                                       rnti,
				       module_idP,
                                       frameP,
                                       ENB_FLAG_YES,
                                       MBMS_FLAG_NO,
                                       DCCH+1,
                                       (char *)&dlsch_buffer[sdu_lengths[0]]);
            printf("\nOriginally, MAC's dlsch buffer has been occupied by %d bytes\n",sdu_length_total);printf("\nRB debug RLC send %d bytes for DCCH1\n",sdu_lengths[num_sdus]);
            sdu_lcids[num_sdus] = DCCH1;
            sdu_length_total += sdu_lengths[num_sdus];printf("\nsdu_length_total=%d\n",sdu_length_total);
            header_len_dcch += 2;
            UE_list->eNB_UE_stats[CC_id][UE_id].num_pdu_tx[DCCH1]+=1;
            UE_list->eNB_UE_stats[CC_id][UE_id].num_bytes_tx[DCCH1]+=sdu_lengths[num_sdus];
            num_sdus++;
            LOG_D(MAC,"[eNB %d] CC_id %d Got %d bytes for DCCH from RLC\n",module_idP,CC_id,sdu_lengths[0]);
          }
        }


		// check for DTCH and update header information
		// here we should loop over all possible DTCH

		// %EDITED%
		// TODO

		header_len_dtch = 3; // 3 bytes DTCH SDU subheader

		LOG_D(MAC,"[eNB %d], Frame %d, DTCH->DLSCH, CC_id %d, Checking RLC status (rab %d, tbs %d, len %d)\n",
						module_idP,frameP,CC_id,DTCH,TBS,
						TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch);
		printf("Before DTCH : available bytes for payload=%d\n", TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch);
		if (TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch > 0 ) {

				int size_version = 0;
				int switch_flag=0;				
				size_version = chooseSizeVersion(UE_id);//1 for SMALL, 2 for BIG
				if(HIGH_FPS_UE && LOW_FPS_UE){
						printf("THE_UE : %d ; NOW_UE : %d ; SENT_FLAG=%d DROP_FLAG=%d\n", THE_UE+1, UE_id+1,SENT_FLAG,DROP_FLAG);
				}else{	
						printf("NOW_UE : %d ; SENT_FLAG=%d DROP_FLAG=%d\n",UE_id+1,SENT_FLAG,DROP_FLAG);//
						printf("SENT_FLAG_1=%d SENT_FLAG_2=%d\n",SENT_FLAG_1,SENT_FLAG_2);//
				}
				if(HIGH_FPS_UE && LOW_FPS_UE){
						printf("Packet count before adjustTBS : %d\n", packet_count);
				}else{
						if(UE_id==0){
								printf("Packet count before adjustTBS : %d\n", packet_count_1);
						}else{//UE_id==1
								printf("Packet count before adjustTBS : %d\n", packet_count_2);
						}
				}
				rlc_status = mac_rlc_status_ind(
								module_idP,
								rnti,
								module_idP,
								frameP,
								ENB_FLAG_YES,
								MBMS_FLAG_NO,
								DTCH,
								TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch);
				printf("RB debug Bytes in DTCH RLC buffer : %d\n", rlc_status.bytes_in_buffer);

				if(HIGH_FPS_UE && LOW_FPS_UE) {
						// Count TB size
						if(UE_id == THE_UE) {
								DTCH_ind_SIZE = adjustTBS(
												TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch,
												size_version,
												packet_count
												);
						} else {
								//								DTCH_ind_SIZE = 2;//no use
								DTCH_ind_SIZE = adjustTBS(
												TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch,
												size_version,
												drop_count
												);//

								header_len_dtch = 0;
						}
				} else {
						// TODO
						if(UE_id==0){//single UE or small UE of Compare group
								DTCH_ind_SIZE = adjustTBS(
												TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch,
												size_version,
												packet_count_1
												);
						}
						else{
								DTCH_ind_SIZE = adjustTBS(
												TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch,
												size_version,
												packet_count_2
												);
						}
						THE_UE = UE_id;
						SENT_FLAG = 1;
				}

				//				if(CTIME>=727&&lock==1){//
				//						printf("Enter setting flag 1.\n");
				//						CHANGE_UE_FLAG = 1;//
				//						DROP_FLAG = 1;						
				//						SENT_FLAG = 1;
				//						lock=0;
				//				}
				int temp = !(LOW_FPS_UE && HIGH_FPS_UE && CHANGE_UE_FLAG);//CHANGE_UE_FLAG is always 0 for except test group
				printf("Temp=%d\n",temp);
				if(temp){	
						// Drop packet
						if(UE_id != THE_UE) {
								printf("DROP_FLAG=%d, UE_list length=%d\n",DROP_FLAG,UE_list->num_UEs);
								if(DROP_FLAG && LOW_FPS_UE && HIGH_FPS_UE){
										printf("Enter drop ");//\n

										rlc_status = mac_rlc_status_ind(
														module_idP,
														rnti,
														module_idP,
														frameP,
														ENB_FLAG_YES,
														MBMS_FLAG_NO,
														DTCH,
														DTCH_ind_SIZE);

										if (rlc_status.bytes_in_buffer > 0) {

												// Choose size version and maximum number of packets
												int max_pkt_num = 0;
												size_version = chooseSizeVersion(UE_id);

												// Start dropping packets
												unsigned char trash[2000];


												int sdu_size = mac_rlc_data_req(module_idP,
																rnti,
																module_idP,
																frameP,
																ENB_FLAG_YES,
																MBMS_FLAG_NO,
																DTCH,
																(char *)&trash[0]);
												int add_unit;
												if(size_version == SMALL) {
														add_unit = (sdu_size / (SIZE_SMALL + 57));
														max_pkt_num = MAX_NUM_OF_LOW_FPS;//
												} else if(size_version == BIG) {
														add_unit = (sdu_size / (SIZE_BIG + 57));
														max_pkt_num = MAX_NUM_OF_HIGH_FPS;//
												}	
												drop_count+=add_unit;
												printf("Drop count : %d, drop content : %x\n", drop_count, trash[100]);

												if(drop_count == max_pkt_num && add_unit!=0) {	
														drop_count = 0;
														DROP_FLAG = 0;
														printf("Drop finish.\n");

														//debug region
														unit_time_counter_1_4++;							
														if(UE_id==0) {
																printf("CTIME=%d !THE_UE=%d ARR_CTIME_PASS_1[%d]=%d\n",CTIME,((!THE_UE)+1),unit_time_counter_1_4,ARR_CTIME_PASS_1[unit_time_counter_1_4]);
																printf("UE %d : unit time %d takes %d time to drop.\n",UE_id+1,unit_time_counter_1_4,CTIME-ARR_CTIME_PASS_1[unit_time_counter_1_4]);
														}else{
																printf("CTIME=%d !THE_UE=%d ARR_CTIME_PASS_2[%d]=%d\n",CTIME,((!THE_UE)+1),unit_time_counter_1_4,ARR_CTIME_PASS_2[unit_time_counter_1_4]);
																printf("UE %d : unit time %d takes %d time to drop.\n",UE_id+1,unit_time_counter_1_4,CTIME-ARR_CTIME_PASS_2[unit_time_counter_1_4]);	
														}



												}
										}
								}
						} else if (SENT_FLAG) {
								switch_flag = 1;//
								printf("Real indication\n");//

								rlc_status = mac_rlc_status_ind(
												module_idP,
												rnti,
												module_idP,
												frameP,
												ENB_FLAG_YES,
												MBMS_FLAG_NO,
												DTCH,
												DTCH_ind_SIZE);

								printf("\nTBS=%d, ta_len=%d, header_len_dcch=%d, sdu_length_total=%d, header_len_dtch=%d",TBS, ta_len, header_len_dcch, sdu_length_total, header_len_dtch);
								printf("\npayload request DTCH = %d\n",TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch);
								printf("DTCH_ind_SIZE=%d\n",DTCH_ind_SIZE);
								printf("bytes in DTCH RLC buffer : %d\n", rlc_status.bytes_in_buffer);

								if (rlc_status.bytes_in_buffer > 0) {
										LOG_D(MAC,"[eNB %d][USER-PLANE DEFAULT DRB], Frame %d, DTCH->DLSCH, CC_id %d, Requesting %d bytes from RLC (hdr len dtch %d)\n",module_idP,frameP,CC_id,TBS-header_len_dcch-sdu_length_total-header_len_dtch,header_len_dtch);

									
									int switch_flag_2=1;				
									if(LOW_FPS_UE==0 && HIGH_FPS_UE==0){//for two sent alignment
											if((UE_id==0 && SENT_FLAG_1==1)||(UE_id==1 && SENT_FLAG_2==1)){

													sdu_lengths[num_sdus] = mac_rlc_data_req(
																	module_idP,
																	rnti,
																	module_idP,
																	frameP,
																	ENB_FLAG_YES,
																	MBMS_FLAG_NO,
																	DTCH,
																	(char*)&dlsch_buffer[sdu_length_total]);//put data from sdu_length_total index//

													printf("\nOriginally, MAC's dlsch buffer has been occupied by %d bytes\n",sdu_length_total);
													printf("RB debug RLC send %d bytes for DTCH\n",sdu_lengths[num_sdus]);
													LOG_D(MAC,"[eNB %d][USER-PLANE DEFAULT DRB] CC_id %d Got %d bytes for DTCH %d \n",
																	module_idP,CC_id,sdu_lengths[num_sdus],DTCH);
													if(sdu_lengths[num_sdus]!=0){
													sdu_lcids[num_sdus] = DTCH;//
													sdu_length_total += sdu_lengths[num_sdus];//
													UE_list->eNB_UE_stats[CC_id][UE_id].num_pdu_tx[DTCH]+=1;//
													UE_list->eNB_UE_stats[CC_id][UE_id].num_bytes_tx[DTCH]+=sdu_lengths[num_sdus];//

													if (sdu_lengths[num_sdus] < 128) {
															header_len_dtch=2;
													}//

													num_sdus++;//For next//
													}else{
															switch_flag_2=0;	
															header_len_dtch = 0;
		
													}
													printf("After that, MAC's dlsch buffer been occupied by %d bytes\n",sdu_length_total);
													printf("sdu_length_total=%d\n\n",sdu_length_total);


											}else{
													switch_flag_2=0;	
													header_len_dtch = 0;
											}	
									}else{
													sdu_lengths[num_sdus] = mac_rlc_data_req(
																	module_idP,
																	rnti,
																	module_idP,
																	frameP,
																	ENB_FLAG_YES,
																	MBMS_FLAG_NO,
																	DTCH,
																	(char*)&dlsch_buffer[sdu_length_total]);//put data from sdu_length_total index//

													printf("\nOriginally, MAC's dlsch buffer has been occupied by %d bytes\n",sdu_length_total);
													printf("RB debug RLC send %d bytes for DTCH\n",sdu_lengths[num_sdus]);
													LOG_D(MAC,"[eNB %d][USER-PLANE DEFAULT DRB] CC_id %d Got %d bytes for DTCH %d \n",
																	module_idP,CC_id,sdu_lengths[num_sdus],DTCH);
													if(sdu_lengths[num_sdus]!=0){
													sdu_lcids[num_sdus] = DTCH;//
													sdu_length_total += sdu_lengths[num_sdus];//
													UE_list->eNB_UE_stats[CC_id][UE_id].num_pdu_tx[DTCH]+=1;//
													UE_list->eNB_UE_stats[CC_id][UE_id].num_bytes_tx[DTCH]+=sdu_lengths[num_sdus];//

													if (sdu_lengths[num_sdus] < 128) {
															header_len_dtch=2;
													}//

													num_sdus++;//For next//
													}else{
														switch_flag=0;
														switch_flag_2=0;	
													}
													printf("After that, MAC's dlsch buffer been occupied by %d bytes\n",sdu_length_total);
													printf("sdu_length_total=%d\n\n",sdu_length_total);


									}									



										// Count number of packets that have been sent
										size_version = chooseSizeVersion(UE_id);
										int add_unit;
										int max_pkt_num = 0;// Choose size version and maximum number of packets
										if(size_version == SMALL) {
												if(switch_flag_2){
														add_unit = (sdu_lengths[--num_sdus] / (SIZE_SMALL + 57));++num_sdus;
												}else{
														add_unit = (sdu_lengths[num_sdus] / (SIZE_SMALL + 57));
												}
												printf("sdu_lengths=%d SIZE=%d\n",sdu_lengths[--num_sdus],(SIZE_SMALL + 57));++num_sdus;
												max_pkt_num = MAX_NUM_OF_LOW_FPS;
										} else if(size_version == BIG) {
												if(switch_flag_2){
														add_unit = (sdu_lengths[--num_sdus] / (SIZE_BIG + 57));++num_sdus;
												}else{
														add_unit = (sdu_lengths[num_sdus] / (SIZE_BIG + 57));
												}
												printf("sdu_lengths=%d SIZE=%d\n",sdu_lengths[--num_sdus],(SIZE_BIG + 57));++num_sdus;
												max_pkt_num = MAX_NUM_OF_HIGH_FPS;
										}

										printf("add unit=%d\n",add_unit);

										if(LOW_FPS_UE && HIGH_FPS_UE) {
												packet_count+=add_unit;
												printf("Sent count : %d, ",packet_count);//max
												printf("sent content : %x\n", dlsch_buffer[sdu_length_total-10]);
												packet_count %= max_pkt_num;
												printf("packet_count after request : %d\n", packet_count);//%
										} else {
												// compare group
												packet_count = 1;
												if(UE_id==0) {//single UE or small UE of Compare group
														packet_count_1+=add_unit;
														printf("Sent count : %d, ",packet_count_1);//max
														printf("sent content : %x\n", dlsch_buffer[sdu_length_total-10]);
														packet_count_1 %= max_pkt_num;
														printf("packet_count after request : %d\n", packet_count_1);//%
														if(packet_count_1==0&&add_unit!=0){//add unit problem
																SENT_FLAG_1=0;
																printf("Sent finish.\n");

																//debug region
																unit_time_counter_1_1++;
																printf("CTIME=%d ARR_CTIME_PASS_1[%d]=%d\n",CTIME,unit_time_counter_1_1,ARR_CTIME_PASS_1[unit_time_counter_1_1]);
																printf("UE %d : unit time %d takes %d time to sent.\n",UE_id+1,unit_time_counter_1_1,CTIME-ARR_CTIME_PASS_1[unit_time_counter_1_1]);

														}
												} else {//UE_id==1
														packet_count_2+=add_unit;
														printf("Sent count : %d, ",packet_count_2);//max
														printf("sent content : %x\n", dlsch_buffer[sdu_length_total-10]);
														packet_count_2 %= max_pkt_num;
														printf("packet_count after request : %d\n", packet_count_2);//%
														if(packet_count_2==0&&add_unit!=0){
																SENT_FLAG_2=0;
																printf("Sent finish.\n");	

																//debug region
																unit_time_counter_1_2++;
																printf("CTIME=%d ARR_CTIME_PASS_2[%d]=%d\n",CTIME,unit_time_counter_1_2,ARR_CTIME_PASS_2[unit_time_counter_1_2]);
																printf("UE %d : unit time %d takes %d time to sent.\n",UE_id+1,unit_time_counter_1_2,CTIME-ARR_CTIME_PASS_2[unit_time_counter_1_2]);

														}

												}
										}



										if((add_unit!=0)&&(!packet_count)&& LOW_FPS_UE && HIGH_FPS_UE){
												SENT_FLAG=0;
												printf("Sent finish.\n");

												//debug region
												unit_time_counter_1_3++;
												if(UE_id==0) {
														printf("CTIME=%d THE_UE=%d ARR_CTIME_PASS_1[%d]=%d\n",CTIME,THE_UE+1,unit_time_counter_1_3,ARR_CTIME_PASS_1[unit_time_counter_1_3]);
														printf("UE %d : unit time %d takes %d time to sent. ac=%d\n",UE_id+1,unit_time_counter_1_3,CTIME-ARR_CTIME_PASS_1[unit_time_counter_1_3],++ue_1_sent_counter);
												}
												else{
														printf("CTIME=%d THE_UE=%d ARR_CTIME_PASS_2[%d]=%d\n",CTIME,THE_UE+1,unit_time_counter_1_3,ARR_CTIME_PASS_2[unit_time_counter_1_3]);
														printf("UE %d : unit time %d takes %d time to sent. ac=%d\n",UE_id+1,unit_time_counter_1_3,CTIME-ARR_CTIME_PASS_2[unit_time_counter_1_3],++ue_2_sent_counter);
												}
										}




								} else {
										header_len_dtch = 0;
								}

						}
						//
				}
				if(LOW_FPS_UE && HIGH_FPS_UE){
					printf("SENT_FLAG=%d DROP_FLAG=%d\n",SENT_FLAG,DROP_FLAG);//
				}else{

					printf("SENT_FLAG_1=%d SENT_FLAG_2=%d\n",SENT_FLAG_1,SENT_FLAG_2);//
				}
				puts("*******************************************");
				//Enter setting flag
				if(LOW_FPS_UE && HIGH_FPS_UE){			
						//					if(lock==0)	{									//after flow_start_time
						//
						if((!SENT_FLAG)&&(!DROP_FLAG)){
								printf("Enter setting flag.\n");
								CHANGE_UE_FLAG = 1;
								DROP_FLAG = 1;
								SENT_FLAG = 1;
						}

						//					}

				}
				if(LOW_FPS_UE==0 && HIGH_FPS_UE==0){
						if((!SENT_FLAG_1)&&(!SENT_FLAG_2)){
								printf("Enter setting flag two sent.\n");
								SENT_FLAG_1 = 1;
								SENT_FLAG_2 = 1;
						}
				}
				//Handle THE_UE’s DTCH Hdr when no enter Real indication				
				if(switch_flag==0){
						header_len_dtch = 0;
				}
				printf("RB debug header_len_dtch=%d\n",header_len_dtch);
		}

        // there is a payload
        if (((sdu_length_total + header_len_dcch + header_len_dtch )> 0)) {

          // Now compute number of required RBs for total sdu length
          // Assume RAH format 2
          // adjust  header lengths
          header_len_dcch_tmp = header_len_dcch;
          header_len_dtch_tmp = header_len_dtch;

          if (header_len_dtch==0) {
            header_len_dcch = (header_len_dcch >0) ? 1 : header_len_dcch;  // remove length field
			printf("RB debug header_len_dcch=%d\n",header_len_dcch);
          } else {
            header_len_dtch = (header_len_dtch > 0) ? 1 :header_len_dtch;     // remove length field for the last SDU
			printf("RB debug header_len_dtch=%d\n",header_len_dtch);
          }


          mcs = eNB_UE_stats->dlsch_mcs1;

          if (mcs==0) {
            nb_rb = 4;  // don't let the TBS get too small
            printf("RB debug nb_rb mcs==0 =%d\n",nb_rb);
          } else {
            nb_rb=min_rb_unit[CC_id];
			//printf("RB debug nb_rb=min_rb_unit=%d\n",nb_rb);
          }
		  printf("RB debug TBS=%d NEW SDU=%d\n",TBS,(sdu_length_total + header_len_dcch + header_len_dtch + ta_len));
          printf("RB debug sdu_length_total=%d header_len_dcch=%d header_len_dtch=%d ta_len=%d\n",sdu_length_total,header_len_dcch,header_len_dtch,ta_len);
          TBS = mac_xface->get_TBS_DL(mcs,nb_rb);
         // printf("RB debug smallest TBS=%d\n\n",TBS);
		  
          while (TBS < (sdu_length_total + header_len_dcch + header_len_dtch + ta_len))  {
		  //printf("RB debug TBS=%d NEW SDU=%d\n",TBS,(sdu_length_total + header_len_dcch + header_len_dtch + ta_len));
            nb_rb += min_rb_unit[CC_id];  //
			//printf("RB debug nb_rb+=%d\n",nb_rb);
            if (nb_rb>nb_available_rb) { // if we've gone beyond the maximum number of RBs
              // (can happen if N_RB_DL is odd)
			  //printf("RB debug nb_rb=%d > nb_available_rb=%d\n",nb_rb,nb_available_rb);
              TBS = mac_xface->get_TBS_DL(eNB_UE_stats->dlsch_mcs1,nb_available_rb);
              nb_rb = nb_available_rb;
		      //printf("RB debug nb_rb=nb_available_rb=%d\n",nb_rb);
              break;
            }

            TBS = mac_xface->get_TBS_DL(eNB_UE_stats->dlsch_mcs1,nb_rb);
            //printf("RB debug TBS=%d\n",TBS);
          }
		  printf("RB debug HARQ debug UE=%d ADJUST TBS=%d\t nb_rb=%d ",UE_id+1,TBS,nb_rb);

          if(nb_rb == ue_sched_ctl->pre_nb_available_rbs[CC_id]) {//if don't have adjustTBS will always hit
            for(j=0; j<frame_parms[CC_id]->N_RBG; j++) { // for indicating the rballoc for each sub-band
              UE_list->UE_template[CC_id][UE_id].rballoc_subband[harq_pid][j] = ue_sched_ctl->rballoc_sub_UE[CC_id][j];
            }
          } else {//if don't have adjustTBS will always not hit
            nb_rb_temp = nb_rb;
            j = 0;

            while((nb_rb_temp > 0) && (j<frame_parms[CC_id]->N_RBG)) {
              if(ue_sched_ctl->rballoc_sub_UE[CC_id][j] == 1) {
                UE_list->UE_template[CC_id][UE_id].rballoc_subband[harq_pid][j] = ue_sched_ctl->rballoc_sub_UE[CC_id][j];

                if ((j == frame_parms[CC_id]->N_RBG-1) &&
                    ((frame_parms[CC_id]->N_RB_DL == 25)||
                     (frame_parms[CC_id]->N_RB_DL == 50))) {
                  nb_rb_temp = nb_rb_temp - min_rb_unit[CC_id]+1;
                } else {
                  nb_rb_temp = nb_rb_temp - min_rb_unit[CC_id];
                }
              }

              j = j+1;
            }
          }

          PHY_vars_eNB_g[module_idP][CC_id]->mu_mimo_mode[UE_id].pre_nb_available_rbs = nb_rb;
          PHY_vars_eNB_g[module_idP][CC_id]->mu_mimo_mode[UE_id].dl_pow_off = ue_sched_ctl->dl_pow_off[CC_id];

          for(j=0; j<frame_parms[CC_id]->N_RBG; j++) {
            PHY_vars_eNB_g[module_idP][CC_id]->mu_mimo_mode[UE_id].rballoc_sub[j] = UE_list->UE_template[CC_id][UE_id].rballoc_subband[harq_pid][j];

          }

          // decrease mcs until TBS falls below required length
          while ((TBS > (sdu_length_total + header_len_dcch + header_len_dtch + ta_len)) && (mcs>0)) {
            mcs--;
            TBS = mac_xface->get_TBS_DL(mcs,nb_rb);
          }

          // if we have decreased too much or we don't have enough RBs, increase MCS
          while ((TBS < (sdu_length_total + header_len_dcch + header_len_dtch + ta_len)) && ((( ue_sched_ctl->dl_pow_off[CC_id]>0) && (mcs<28))
                 || ( (ue_sched_ctl->dl_pow_off[CC_id]==0) && (mcs<=15)))) {
            mcs++;
            TBS = mac_xface->get_TBS_DL(mcs,nb_rb);
          }

          LOG_D(MAC,"dlsch_mcs before and after the rate matching = (%d, %d)\n",eNB_UE_stats->dlsch_mcs1, mcs);

#ifdef DEBUG_eNB_SCHEDULER
          LOG_D(MAC,"[eNB %d] CC_id %d Generated DLSCH header (mcs %d, TBS %d, nb_rb %d)\n",
                module_idP,CC_id,mcs,TBS,nb_rb);
          // msg("[MAC][eNB ] Reminder of DLSCH with random data %d %d %d %d \n",
          //  TBS, sdu_length_total, offset, TBS-sdu_length_total-offset);
#endif

          if ((TBS - header_len_dcch - header_len_dtch - sdu_length_total - ta_len) <= 2) {
            padding = (TBS - header_len_dcch - header_len_dtch - sdu_length_total - ta_len);
            post_padding = 0;
          } else {
            padding = 0;

            // adjust the header len
            if (header_len_dtch==0) {
              header_len_dcch = header_len_dcch_tmp;
            } else { //if (( header_len_dcch==0)&&((header_len_dtch==1)||(header_len_dtch==2)))
              header_len_dtch = header_len_dtch_tmp;
            }

            post_padding = TBS - sdu_length_total - header_len_dcch - header_len_dtch - ta_len ; // 1 is for the postpadding header
          }


          offset = generate_dlsch_header((unsigned char*)UE_list->DLSCH_pdu[CC_id][0][UE_id].payload[0],
                                         // offset = generate_dlsch_header((unsigned char*)eNB_mac_inst[0].DLSCH_pdu[0][0].payload[0],
                                         num_sdus,              //num_sdus
                                         sdu_lengths,  //
                                         sdu_lcids,
                                         255,                                   // no drx
                                         ue_sched_ctl->ta_update, // timing advance
                                         NULL,                                  // contention res id
                                         padding,
                                         post_padding);

          //#ifdef DEBUG_eNB_SCHEDULER
          if (ue_sched_ctl->ta_update) {
            LOG_I(MAC,
                  "[eNB %d][DLSCH] Frame %d Generate header for UE_id %d on CC_id %d: sdu_length_total %d, num_sdus %d, sdu_lengths[0] %d, sdu_lcids[0] %d => payload offset %d,timing advance value : %d, padding %d,post_padding %d,(mcs %d, TBS %d, nb_rb %d),header_dcch %d, header_dtch %d\n",
                  module_idP,frameP, UE_id, CC_id, sdu_length_total,num_sdus,sdu_lengths[0],sdu_lcids[0],offset,
                  ue_sched_ctl->ta_update,padding,post_padding,mcs,TBS,nb_rb,header_len_dcch,header_len_dtch);
          }

          //#endif
#ifdef DEBUG_eNB_SCHEDULER
          LOG_T(MAC,"[eNB %d] First 16 bytes of DLSCH : \n");

          for (i=0; i<16; i++) {
            LOG_T(MAC,"%x.",dlsch_buffer[i]);
          }

          LOG_T(MAC,"\n");
#endif
          // cycle through SDUs and place in dlsch_buffer
          memcpy(&UE_list->DLSCH_pdu[CC_id][0][UE_id].payload[0][offset],dlsch_buffer,sdu_length_total);
          // memcpy(&eNB_mac_inst[0].DLSCH_pdu[0][0].payload[0][offset],dcch_buffer,sdu_lengths[0]);

          // fill remainder of DLSCH with random data
          for (j=0; j<(TBS-sdu_length_total-offset); j++) {
            UE_list->DLSCH_pdu[CC_id][0][UE_id].payload[0][offset+sdu_length_total+j] = (char)(taus()&0xff);
          }

          //eNB_mac_inst[0].DLSCH_pdu[0][0].payload[0][offset+sdu_lengths[0]+j] = (char)(taus()&0xff);
          if (opt_enabled == 1) {
            trace_pdu(1, (uint8_t *)UE_list->DLSCH_pdu[CC_id][0][UE_id].payload[0],
                      TBS, module_idP, 3, UE_RNTI(module_idP,UE_id),
                      eNB->subframe,0,0);
            LOG_D(OPT,"[eNB %d][DLSCH] CC_id %d Frame %d  rnti %x  with size %d\n",
                  module_idP, CC_id, frameP, UE_RNTI(module_idP,UE_id), TBS);
          }

          aggregation = process_ue_cqi(module_idP,UE_id);
          UE_list->UE_template[CC_id][UE_id].nb_rb[harq_pid] = nb_rb;//assign in case of retransmission?
		  printf("harq_pid=%d\n\n\n\n",harq_pid);
		  if(UE_id==0){
			arr_harq_1[harq_pid]=0;
 		  }else{
			arr_harq_2[harq_pid]=0;

          }	
		//	arr_harq[harq_pid]=0;
          add_ue_dlsch_info(module_idP,
                            CC_id,
                            UE_id,
                            subframeP,
                            S_DL_SCHEDULED);
          // store stats
          eNB->eNB_stats[CC_id].dlsch_bytes_tx+=sdu_length_total;
          eNB->eNB_stats[CC_id].dlsch_pdus_tx+=1;

          UE_list->eNB_UE_stats[CC_id][UE_id].rbs_used = nb_rb;
          UE_list->eNB_UE_stats[CC_id][UE_id].total_rbs_used += nb_rb;
          UE_list->eNB_UE_stats[CC_id][UE_id].dlsch_mcs1=eNB_UE_stats->dlsch_mcs1;
          UE_list->eNB_UE_stats[CC_id][UE_id].dlsch_mcs2=mcs;
          UE_list->eNB_UE_stats[CC_id][UE_id].TBS = TBS;

          UE_list->eNB_UE_stats[CC_id][UE_id].overhead_bytes= TBS- sdu_length_total;
          UE_list->eNB_UE_stats[CC_id][UE_id].total_sdu_bytes+= sdu_length_total;
          UE_list->eNB_UE_stats[CC_id][UE_id].total_pdu_bytes+= TBS;
          UE_list->eNB_UE_stats[CC_id][UE_id].total_num_pdus+=1;

          if (frame_parms[CC_id]->frame_type == TDD) {
            UE_list->UE_template[CC_id][UE_id].DAI++;
            //  printf("DAI update: subframeP %d: UE %d, DAI %d\n",subframeP,UE_id,UE_list->UE_template[CC_id][UE_id].DAI);
//#warning only for 5MHz channel
            update_ul_dci(module_idP,CC_id,rnti,UE_list->UE_template[CC_id][UE_id].DAI);
          }

	  // do PUCCH power control
          // this is the normalized RX power
	  eNB_UE_stats =  mac_xface->get_eNB_UE_stats(module_idP,CC_id,rnti);
	  normalized_rx_power = eNB_UE_stats->Po_PUCCH_dBm; 
	  target_rx_power = mac_xface->get_target_pucch_rx_power(module_idP,CC_id) + 20;
	    
          // this assumes accumulated tpc
	  // make sure that we are only sending a tpc update once a frame, otherwise the control loop will freak out
	  int32_t framex10psubframe = UE_list->UE_template[CC_id][UE_id].pucch_tpc_tx_frame*10+UE_list->UE_template[CC_id][UE_id].pucch_tpc_tx_subframe;
          if (((framex10psubframe+10)<=(frameP*10+subframeP)) || //normal case
	      ((framex10psubframe>(frameP*10+subframeP)) && (((10240-framex10psubframe+frameP*10+subframeP)>=10)))) //frame wrap-around
	    if (eNB_UE_stats->Po_PUCCH_update == 1) { 
	      eNB_UE_stats->Po_PUCCH_update = 0;

	      UE_list->UE_template[CC_id][UE_id].pucch_tpc_tx_frame=frameP;
	      UE_list->UE_template[CC_id][UE_id].pucch_tpc_tx_subframe=subframeP;
	      
	      if (normalized_rx_power>(target_rx_power+1)) {
		tpc = 0; //-1
		tpc_accumulated--;
	      } else if (normalized_rx_power<(target_rx_power-1)) {
		tpc = 2; //+1
		tpc_accumulated++;
	      } else {
		tpc = 1; //0
	      }
	      /*	      
	      LOG_I(MAC,"[eNB %d] DLSCH scheduler: frame %d, subframe %d, harq_pid %d, tpc %d, accumulated %d, normalized/target rx power %d/%d\n",
		    module_idP,frameP, subframeP,harq_pid,tpc,
		    tpc_accumulated,normalized_rx_power,target_rx_power);*/

	    } // Po_PUCCH has been updated 
	    else {
	      tpc = 1; //0
	    } // time to do TPC update 
	  else {
	    tpc = 1; //0
	  }

          switch (mac_xface->get_transmission_mode(module_idP,CC_id,rnti)) {
          case 1:
          case 2:
          default:
            if (frame_parms[CC_id]->frame_type == TDD) {
              switch (frame_parms[CC_id]->N_RB_DL) {
              case 6:
                ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->TPC = tpc;
                ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
                break;

              case 25:
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->TPC = tpc;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
                LOG_D(MAC,"Format1 DCI: harq_pid %d, ndi %d\n",harq_pid,((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi);
                break;

              case 50:
                ((DCI1_10MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_10MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_10MHz_TDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_10MHz_TDD_t*)DLSCH_dci)->TPC = tpc;
                ((DCI1_10MHz_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
                break;

              case 100:
                ((DCI1_20MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_20MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_20MHz_TDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_20MHz_TDD_t*)DLSCH_dci)->TPC = tpc;
                ((DCI1_20MHz_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
                break;

              default:
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->TPC = tpc;
                ((DCI1_5MHz_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
                break;
              }
            } else {
              switch (frame_parms[CC_id]->N_RB_DL) {
              case 6:
                ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->TPC = tpc;
                break;

              case 25:
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->TPC = tpc;
                break;

              case 50:
                ((DCI1_10MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_10MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_10MHz_FDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_10MHz_FDD_t*)DLSCH_dci)->TPC = tpc;
                break;

              case 100:
                ((DCI1_20MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_20MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_20MHz_FDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_20MHz_FDD_t*)DLSCH_dci)->TPC = tpc;
                break;

              default:
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->mcs = mcs;
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->ndi = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rv = 0;
                ((DCI1_5MHz_FDD_t*)DLSCH_dci)->TPC = tpc;
                break;
              }
            }

            break;

          case 3:
            if (frame_parms[CC_id]->frame_type == TDD) {
              switch (frame_parms[CC_id]->N_RB_DL) {
              case 6:
                ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->rv1 = 0;

                // deactivate TB2
                ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->rv2 = 1;

                ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
                ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->TPC      = tpc;
                break;

              case 25:
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->rv1 = 0;
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
		((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->TPC = tpc;

                // deactivate TB2
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->rv2 = 1;

                LOG_D(MAC,"Format1 DCI: harq_pid %d, ndi %d\n",harq_pid,((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->ndi1);
                break;

              case 50:
                ((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->rv1 = 0;
                ((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
		((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->TPC = tpc;

                // deactivate TB2
                ((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->rv2 = 1;
                break;

              case 100:
                ((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->rv1 = 0;
                ((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
		((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->TPC = tpc;

                // deactivate TB2
                ((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->rv2 = 1;
                break;

              default:
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->rv1 = 0;
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->dai      = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
		((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->TPC = tpc;

                // deactivate TB2
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->rv2 = 1;
                break;
              }
            } else {
              switch (frame_parms[CC_id]->N_RB_DL) {
              case 6:
                ((DCI2A_1_5MHz_2A_FDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_1_5MHz_2A_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_1_5MHz_2A_FDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_1_5MHz_2A_FDD_t*)DLSCH_dci)->rv1 = 0;
		((DCI2A_1_5MHz_2A_FDD_t*)DLSCH_dci)->TPC = tpc;

                // deactivate TB2
                ((DCI2A_1_5MHz_2A_FDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_1_5MHz_2A_FDD_t*)DLSCH_dci)->rv2 = 1;
                break;

              case 25:
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->rv1 = 0;
		((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->TPC = tpc;

                // deactivate TB2
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->rv2 = 1;
                break;

              case 50:
                ((DCI2A_10MHz_2A_FDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_10MHz_2A_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_10MHz_2A_FDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_10MHz_2A_FDD_t*)DLSCH_dci)->rv1 = 0;
		((DCI2A_10MHz_2A_FDD_t*)DLSCH_dci)->TPC = tpc;
                // deactivate TB2
                ((DCI2A_10MHz_2A_FDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_10MHz_2A_FDD_t*)DLSCH_dci)->rv2 = 1;
                break;

              case 100:
                ((DCI2A_20MHz_2A_FDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_20MHz_2A_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_20MHz_2A_FDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_20MHz_2A_FDD_t*)DLSCH_dci)->rv1 = 0;
		((DCI2A_20MHz_2A_FDD_t*)DLSCH_dci)->TPC = tpc;

                // deactivate TB2
                ((DCI2A_20MHz_2A_FDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_20MHz_2A_FDD_t*)DLSCH_dci)->rv2 = 1;
                break;

              default:
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->mcs1 = mcs;
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->harq_pid = harq_pid;
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->ndi1 = 1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->rv1 = 0;
		((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->TPC = tpc;

                // deactivate TB2
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->mcs2 = 0;
                ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->rv2 = 1;
                break;
              }
            }

            break;

          case 4:
            //  if (nb_rb>10) {
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->mcs1 = mcs;
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->ndi1 = 1;
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->rv1 = round&3;
            ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->dai = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
	    ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->TPC = tpc;

            //}
            /* else {
               ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->mcs1 = eNB_UE_stats->DL_cqi[0];
               ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
               ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->ndi1 = 1;
               ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->rv1 = round&3;
               ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->tpmi = 5;
               ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->dai = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
               }*/
            break;

          case 5:

            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->mcs = mcs;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 1;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
	    ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->TPC = tpc;

            if(ue_sched_ctl->dl_pow_off[CC_id] == 2) {
              ue_sched_ctl->dl_pow_off[CC_id] = 1;
            }

            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = ue_sched_ctl->dl_pow_off[CC_id];
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->tpmi = 5;
            break;

          case 6:
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->mcs = mcs;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->harq_pid = harq_pid;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->ndi = 1;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rv = round&3;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dai = (UE_list->UE_template[CC_id][UE_id].DAI-1)&3;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->dl_power_off = 1;
            ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->tpmi = 5;
	    ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->TPC = tpc;

            break;
          }

          // Toggle NDI for next time
          LOG_D(MAC,"CC_id %d Frame %d, subframeP %d: Toggling Format1 NDI for UE %d (rnti %x/%d) oldNDI %d\n",
                CC_id, frameP,subframeP,UE_id,
                UE_list->UE_template[CC_id][UE_id].rnti,harq_pid,UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid]);
          UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid]=1-UE_list->UE_template[CC_id][UE_id].oldNDI[harq_pid];
        } else {  // There is no data from RLC or MAC header, so don't schedule

        }
      }

      if (frame_parms[CC_id]->frame_type == TDD) {
        set_ul_DAI(module_idP,UE_id,CC_id,frameP,subframeP,frame_parms);
      }

    } // UE_id loop
  }  // CC_id loop


  stop_meas(&eNB->schedule_dlsch);
  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_SCHEDULE_DLSCH,VCD_FUNCTION_OUT);

}

//------------------------------------------------------------------------------
void
fill_DLSCH_dci(
	       module_id_t module_idP,
	       frame_t frameP,
	       sub_frame_t subframeP,
	       int* mbsfn_flagP
	       )
//------------------------------------------------------------------------------
{

  // loop over all allocated UEs and compute frequency allocations for PDSCH
  int   UE_id = -1;
  uint8_t            /* first_rb, */ nb_rb=3;
  rnti_t        rnti;
  //unsigned char *vrb_map;
  uint8_t            rballoc_sub[25];
  //uint8_t number_of_subbands=13;

  //unsigned char round;
  unsigned char harq_pid;
  void         *DLSCH_dci=NULL;
  DCI_PDU      *DCI_pdu;
  int           i;
  //void         *BCCH_alloc_pdu;
  int           size_bits,size_bytes;
  int CC_id;
  eNB_MAC_INST *eNB  =&eNB_mac_inst[module_idP];
  UE_list_t    *UE_list = &eNB->UE_list;
  //RA_TEMPLATE  *RA_template;

  start_meas(&eNB->fill_DLSCH_dci);
  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_FILL_DLSCH_DCI,VCD_FUNCTION_IN);

  for (CC_id=0; CC_id<MAX_NUM_CCs; CC_id++) {
    LOG_D(MAC,"Doing fill DCI for CC_id %d\n",CC_id);

    if (mbsfn_flagP[CC_id]>0)
      continue;

    DCI_pdu         = &eNB->common_channels[CC_id].DCI_pdu;
    

    // UE specific DCIs
    for (UE_id=UE_list->head; UE_id>=0; UE_id=UE_list->next[UE_id]) {
      LOG_T(MAC,"CC_id %d, UE_id: %d => status %d\n",CC_id,UE_id,eNB_dlsch_info[module_idP][CC_id][UE_id].status);

      if (eNB_dlsch_info[module_idP][CC_id][UE_id].status == S_DL_SCHEDULED) {

        // clear scheduling flag
        eNB_dlsch_info[module_idP][CC_id][UE_id].status = S_DL_WAITING;
        rnti = UE_RNTI(module_idP,UE_id);
	//        mac_xface->get_ue_active_harq_pid(module_idP,CC_id,rnti,frameP,subframeP,&harq_pid,&round,0);
	harq_pid = UE_list->UE_sched_ctrl[UE_id].harq_pid[CC_id];
        nb_rb = UE_list->UE_template[CC_id][UE_id].nb_rb[harq_pid];

        DLSCH_dci = (void *)UE_list->UE_template[CC_id][UE_id].DLSCH_DCI[harq_pid];


        /// Synchronizing rballoc with rballoc_sub
        for(i=0; i<PHY_vars_eNB_g[module_idP][CC_id]->lte_frame_parms.N_RBG; i++) {
          rballoc_sub[i] = UE_list->UE_template[CC_id][UE_id].rballoc_subband[harq_pid][i];
        }

        switch(mac_xface->get_transmission_mode(module_idP,CC_id,rnti)) {
        default:

        case 1:

        case 2:
          LOG_D(MAC,"[eNB %d] CC_id %d Adding UE %d spec DCI for %d PRBS \n",module_idP, CC_id, UE_id, nb_rb);

          if (PHY_vars_eNB_g[module_idP][CC_id]->lte_frame_parms.frame_type == TDD) {
            switch (PHY_vars_eNB_g[module_idP][CC_id]->lte_frame_parms.N_RB_DL) {
            case 6:
              ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_1_5MHz_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI1_1_5MHz_TDD_t);
              size_bits  = sizeof_DCI1_1_5MHz_TDD_t;
              break;

            case 25:
              ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI1_5MHz_TDD_t);
              size_bits  = sizeof_DCI1_5MHz_TDD_t;
              break;

            case 50:
              ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_10MHz_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI1_10MHz_TDD_t);
              size_bits  = sizeof_DCI1_10MHz_TDD_t;
              break;

            case 100:
              ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_20MHz_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI1_20MHz_TDD_t);
              size_bits  = sizeof_DCI1_20MHz_TDD_t;
              break;

            default:
              ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_5MHz_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI1_5MHz_TDD_t);
              size_bits  = sizeof_DCI1_5MHz_TDD_t;
              break;
            }


          } else {
            switch(PHY_vars_eNB_g[module_idP][CC_id]->lte_frame_parms.N_RB_DL) {
            case 6:
              ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_1_5MHz_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI1_1_5MHz_FDD_t);
              size_bits=sizeof_DCI1_1_5MHz_FDD_t;
              break;

            case 25:
              ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI1_5MHz_FDD_t);
              size_bits=sizeof_DCI1_5MHz_FDD_t;
              break;

            case 50:
              ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_10MHz_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI1_10MHz_FDD_t);
              size_bits=sizeof_DCI1_10MHz_FDD_t;
              break;

            case 100:
              ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_20MHz_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI1_20MHz_FDD_t);
              size_bits=sizeof_DCI1_20MHz_FDD_t;
              break;

            default:
              ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI1_5MHz_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI1_5MHz_FDD_t);
              size_bits=sizeof_DCI1_5MHz_FDD_t;
              break;
            }
          }

          add_ue_spec_dci(DCI_pdu,
                          DLSCH_dci,
                          rnti,
                          size_bytes,
                          process_ue_cqi (module_idP,UE_id),//aggregation,
                          size_bits,
                          format1,
                          0);

          break;

        case 3:
          LOG_D(MAC,"[eNB %d] CC_id %d Adding Format 2A UE %d spec DCI for %d PRBS (rb alloc: %x) \n",
                module_idP, CC_id, UE_id, nb_rb);

          if (PHY_vars_eNB_g[module_idP][CC_id]->lte_frame_parms.frame_type == TDD) {
            switch (PHY_vars_eNB_g[module_idP][CC_id]->lte_frame_parms.N_RB_DL) {
            case 6:
              ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_1_5MHz_2A_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI2A_1_5MHz_2A_TDD_t);
              size_bits  = sizeof_DCI2A_1_5MHz_2A_TDD_t;
              break;

            case 25:
              ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI2A_5MHz_2A_TDD_t);
              size_bits  = sizeof_DCI2A_5MHz_2A_TDD_t;
              break;

            case 50:
              ((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_10MHz_2A_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI2A_10MHz_2A_TDD_t);
              size_bits  = sizeof_DCI2A_10MHz_2A_TDD_t;
              break;

            case 100:
              ((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_20MHz_2A_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI2A_20MHz_2A_TDD_t);
              size_bits  = sizeof_DCI2A_20MHz_2A_TDD_t;
              break;

            default:
              ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_5MHz_2A_TDD_t*)DLSCH_dci)->rah = 0;
              size_bytes = sizeof(DCI2A_5MHz_2A_TDD_t);
              size_bits  = sizeof_DCI2A_5MHz_2A_TDD_t;
              break;
            }


          } else {
            switch(PHY_vars_eNB_g[module_idP][CC_id]->lte_frame_parms.N_RB_DL) {
            case 6:
              ((DCI2A_1_5MHz_2A_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_1_5MHz_2A_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI2A_1_5MHz_2A_FDD_t);
              size_bits=sizeof_DCI2A_1_5MHz_2A_FDD_t;
              break;

            case 25:
              ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI2A_5MHz_2A_FDD_t);
              size_bits=sizeof_DCI2A_5MHz_2A_FDD_t;
              break;

            case 50:
              ((DCI2A_10MHz_2A_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_10MHz_2A_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI2A_10MHz_2A_FDD_t);
              size_bits=sizeof_DCI2A_10MHz_2A_FDD_t;
              break;

            case 100:
              ((DCI2A_20MHz_2A_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_20MHz_2A_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI2A_20MHz_2A_FDD_t);
              size_bits=sizeof_DCI2A_20MHz_2A_FDD_t;
              break;

            default:
              ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
              ((DCI2A_5MHz_2A_FDD_t*)DLSCH_dci)->rah = 0;
              size_bytes=sizeof(DCI2A_5MHz_2A_FDD_t);
              size_bits=sizeof_DCI2A_5MHz_2A_FDD_t;
              break;
            }
          }

          add_ue_spec_dci(DCI_pdu,
                          DLSCH_dci,
                          rnti,
                          size_bytes,
                          process_ue_cqi (module_idP,UE_id),//aggregation,
                          size_bits,
                          format2A,
                          0);

          break;

        case 4:

          //if (nb_rb>10) {
          // DCI format 2_2A
          ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
          ((DCI2_5MHz_2A_TDD_t*)DLSCH_dci)->rah = 0;
          add_ue_spec_dci(DCI_pdu,
                          DLSCH_dci,
                          rnti,
                          sizeof(DCI2_5MHz_2A_TDD_t),
                          process_ue_cqi (module_idP,UE_id),//aggregation,
                          sizeof_DCI2_5MHz_2A_TDD_t,
                          format2,
                          0);
          /*}
            else {
            ((DCI2_5MHz_2A_L10PRB_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
            add_ue_spec_dci(DCI_pdu,
            DLSCH_dci,
            rnti,
            sizeof(DCI2_5MHz_2A_L10PRB_TDD_t),
            2,//aggregation,
            sizeof_DCI2_5MHz_2A_L10PRB_TDD_t,
            format2_2A_L10PRB);
            }*/
          break;

        case 5:
          ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
          ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rah = 0;

          add_ue_spec_dci(DCI_pdu,
                          DLSCH_dci,
                          rnti,
                          sizeof(DCI1E_5MHz_2A_M10PRB_TDD_t),
                          process_ue_cqi (module_idP,UE_id),//aggregation,
                          sizeof_DCI1E_5MHz_2A_M10PRB_TDD_t,
                          format1E_2A_M10PRB,
                          0);
          break;

        case 6:
          ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rballoc = allocate_prbs_sub(nb_rb,rballoc_sub);
          ((DCI1E_5MHz_2A_M10PRB_TDD_t*)DLSCH_dci)->rah = 0;

          add_ue_spec_dci(DCI_pdu,
                          DLSCH_dci,
                          rnti,
                          sizeof(DCI1E_5MHz_2A_M10PRB_TDD_t),
                          process_ue_cqi (module_idP,UE_id),//aggregation
                          sizeof_DCI1E_5MHz_2A_M10PRB_TDD_t,
                          format1E_2A_M10PRB,
                          0);
          break;

        }
      }
    }

  }

  stop_meas(&eNB->fill_DLSCH_dci);
  VCD_SIGNAL_DUMPER_DUMP_FUNCTION_BY_NAME(VCD_SIGNAL_DUMPER_FUNCTIONS_FILL_DLSCH_DCI,VCD_FUNCTION_OUT);
}

//------------------------------------------------------------------------------
unsigned char*
get_dlsch_sdu(
  module_id_t module_idP,
  int CC_id,
  frame_t frameP,
  rnti_t rntiP,
  uint8_t TBindex
)
//------------------------------------------------------------------------------
{

  int UE_id;
  eNB_MAC_INST *eNB=&eNB_mac_inst[module_idP];

  if (rntiP==SI_RNTI) {
    LOG_D(MAC,"[eNB %d] CC_id %d Frame %d Get DLSCH sdu for BCCH \n", module_idP, CC_id, frameP);

    return((unsigned char *)&eNB->common_channels[CC_id].BCCH_pdu.payload[0]);
  }

  UE_id = find_UE_id(module_idP,rntiP);

  if (UE_id != -1) {
    LOG_D(MAC,"[eNB %d] Frame %d:  CC_id %d Get DLSCH sdu for rnti %x => UE_id %d\n",module_idP,frameP,CC_id,rntiP,UE_id);
    return((unsigned char *)&eNB->UE_list.DLSCH_pdu[CC_id][TBindex][UE_id].payload[0]);
  } else {
    LOG_E(MAC,"[eNB %d] Frame %d: CC_id %d UE with RNTI %x does not exist\n", module_idP,frameP,CC_id,rntiP);
    return NULL;
  }

}


//------------------------------------------------------------------------------
void
update_ul_dci(
  module_id_t module_idP,
  uint8_t CC_id,
  rnti_t rnti,
  uint8_t dai)
//------------------------------------------------------------------------------
{

  DCI_PDU             *DCI_pdu   = &eNB_mac_inst[module_idP].common_channels[CC_id].DCI_pdu;
  int                  i;
  DCI0_5MHz_TDD_1_6_t *ULSCH_dci = NULL;;

  if (mac_xface->lte_frame_parms->frame_type == TDD) {
    for (i=0; i<DCI_pdu->Num_common_dci+DCI_pdu->Num_ue_spec_dci; i++) {
      ULSCH_dci = (DCI0_5MHz_TDD_1_6_t *)DCI_pdu->dci_alloc[i].dci_pdu;

      if ((DCI_pdu->dci_alloc[i].format == format0) && (DCI_pdu->dci_alloc[i].rnti == rnti)) {
        ULSCH_dci->dai = (dai-1)&3;
      }
    }
  }

  //  printf("Update UL DCI: DAI %d\n",dai);
}


//------------------------------------------------------------------------------
void set_ue_dai(
  sub_frame_t   subframeP,
  uint8_t       tdd_config,
  int           UE_id,
  uint8_t       CC_id,
  UE_list_t*     UE_list
)
//------------------------------------------------------------------------------
{

  switch (tdd_config) {
  case 0:
    if ((subframeP==0)||(subframeP==1)||(subframeP==3)||(subframeP==5)||(subframeP==6)||(subframeP==8)) {
      UE_list->UE_template[CC_id][UE_id].DAI = 0;
    }

    break;

  case 1:
    if ((subframeP==0)||(subframeP==4)||(subframeP==5)||(subframeP==9)) {
      UE_list->UE_template[CC_id][UE_id].DAI = 0;
    }

    break;

  case 2:
    if ((subframeP==4)||(subframeP==5)) {
      UE_list->UE_template[CC_id][UE_id].DAI = 0;
    }

    break;

  case 3:
    if ((subframeP==5)||(subframeP==7)||(subframeP==9)) {
      UE_list->UE_template[CC_id][UE_id].DAI = 0;
    }

    break;

  case 4:
    if ((subframeP==0)||(subframeP==6)) {
      UE_list->UE_template[CC_id][UE_id].DAI = 0;
    }

    break;

  case 5:
    if (subframeP==9) {
      UE_list->UE_template[CC_id][UE_id].DAI = 0;
    }

    break;

  case 6:
    if ((subframeP==0)||(subframeP==1)||(subframeP==5)||(subframeP==6)||(subframeP==9)) {
      UE_list->UE_template[CC_id][UE_id].DAI = 0;
    }

    break;

  default:
    UE_list->UE_template[CC_id][UE_id].DAI = 0;
    LOG_N(MAC,"unknow TDD config %d\n",tdd_config);
    break;
  }
}
/*			
				enum{small=1,big=2};
				int size_small = 110;           //here adjust
				int size_big = 220;             //here adjust
				int x;
				int OTG_size;
				int template_version = 41;//33  //here adjust
				int size_version = big;//big    //here adjust
//				printf("\nIn dlsch, UE_id=%d\n",UE_id);
			    if(UE_id==0){
					size_version = small;
				}else if(UE_id==1){
					size_version = big;
				}
				switch(template_version){
						case 41:OTG_size=57;
								switch(size_version){
										case small:x=size_small+OTG_size+2;
										break;
										case big:x=size_big+OTG_size+2;
										break;
								}
								break;
						case 33:OTG_size=69;
								switch(size_version){
										case small:x=size_small+OTG_size+2;
										break;
										case big:x=size_big+OTG_size+2;
										break;
								}
								break;
				}
				if((floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-8)/x))>=5){
						DTCH_ind_SIZE=(floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-8)/x))*x+8;
				}else if((floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-7)/x))>=4){
						DTCH_ind_SIZE=(floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-7)/x))*x+7;
				}else if((floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-5)/x))>=3){
						DTCH_ind_SIZE=(floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-5)/x))*x+5;
				}else if( (floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-4)/x))>=2){ 
					DTCH_ind_SIZE=(floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-4)/x))*x+4;
				}else{
                    DTCH_ind_SIZE=(floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-2)/x))*x+2;
				}

          //DTCH_ind_SIZE=(floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-2)/239))*239+2;//180:for c41 239=57+2+180, for c33 251=69+2+180 360:for c41 419=57+2+360, for c33 431=69+2+360
          //DTCH_ind_SIZE=(floor((double)((TBS-ta_len-header_len_dcch-sdu_length_total-header_len_dtch)-2)/239))*239+2;//100:for c41 159=57+2+100, for c33 171=69+2+100 200:for c41 259=57+2+200, for c33 271=69+2+200
*/
// %EDITED%
int adjustTBS(int TBS, int size_version, int packet_count)
{
    int max_concat_num = 5;
    int max_packet_num = 0;
    int packet_size = 0;
    int OTG_size = 0;
    int DTCH_ind_size = 2;

    switch(TEMPLATE_VERSION)
    {
    case 41:
        OTG_size = 57;
        break;
    case 33:
        OTG_size = 69;
        break;
    default:
        OTG_size = -1;
    }

    // SMALL = 1, BIG = 2
    // Choose payload size and,
    // ensure MAC SDU does not contain data of two unit-times.
    switch(size_version)
    {
    case 1:
        // 110 + 57 + 2 = 169
        packet_size = SIZE_SMALL + OTG_size + PDCP_HEADER;//SIZE_SMALL=110
        max_packet_num = MAX_NUM_OF_LOW_FPS;//359

        break;
    case 2:
        // 220 + 57 + 2 = 279
        packet_size = SIZE_BIG + OTG_size + PDCP_HEADER;
		max_packet_num = MAX_NUM_OF_HIGH_FPS;

        break;
    }

    if(max_packet_num - packet_count < max_concat_num)
    {
        max_concat_num = max_packet_num - packet_count;
    }


    int i = 0;
    for(i = max_concat_num; i > 0; i--)
    {
        int tmp = TBS - RLC_HEADER[i];
        if(tmp/packet_size >= i)
        {
            DTCH_ind_size = packet_size*i + RLC_HEADER[i];
			printf("adjustTBS : TBS:%d, concat_num:%d, DTCH_ind_size:%d\n", TBS, i, DTCH_ind_size);
            break;
        }
    }

    return DTCH_ind_size;
}

int chooseSizeVersion(int UE_id){
	int size_version = 0;

    // Choose packet size version
//	if(HIGH_FPS_UE && LOW_FPS_UE) {
//			if(UE_id == 0) {
//					size_version = SMALL;
//			} else {
//					size_version = BIG;
//			}
//	} else if (LOW_FPS_UE) {
//			size_version = SMALL;
//	} else if (HIGH_FPS_UE){
//			size_version = BIG;
//	} else {
//			if(UE_id == 0) {
//					size_version = SMALL;
//			} else {
//					size_version = BIG;
//			}  
//	}
	if(HIGH_FPS_UE && !LOW_FPS_UE){
			size_version = BIG;
	}else{
			if(UE_id==0){  
					size_version = SMALL;
			}else{
					size_version = BIG;
			}

	}


	return size_version;
}

int calcTrashSize(int drop_count, int max_pkt_num, int size_version) {
	int max_concat_num = 5;
	int trash_size = 0;
	int OTG_size = 0;
	int pkt_size = 0;

	if(max_pkt_num - drop_count < max_concat_num) {
		max_concat_num = max_pkt_num - drop_count;
	}
	
	switch(TEMPLATE_VERSION)
    {
    case 41:
        OTG_size = 57;
        break;
    case 33:
        OTG_size = 69;
        break;
    default:
        OTG_size = -1;
    }
	
	if(size_version == SMALL) {
		pkt_size += SIZE_SMALL;
	} else {
		pkt_size += SIZE_BIG;
	}
	pkt_size += (OTG_size + PDCP_HEADER);
	
	trash_size = pkt_size*max_concat_num + RLC_HEADER[max_concat_num];

	return trash_size;

}

/*

int countSeqNum(int concat_num, char *buffer){
    int seqIdx = 0;
    int OTG_size = 0;

    if(!concat_num)
        return 0;

    switch(TEMPLATE_VERSION)
    {
    case 41:
        OTG_size = 57;
        break;
    case 33:
        OTG_size = 69;
        break;
    default:
        OTG_size = -1;
    }

    seqIdx = OTG_size + RLC_HEADER[concat_num] + PDCP_HEADER + 1;

    char q = buffer[seqIdx];
    char r = buffer[seqIdx+1];

    printf("%d, %d\n", q, r);

    int seqNum = (q << 2) + (r & 0xFF);

    return seqNum;
}

*/
