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

/*! \file otg_tx.c
 * \brief function containing the OTG TX traffic generation functions
 * \author N. Nikaein and A. Hafsaoui
 * \date 2011
 * \version 0.1
 * \company Eurecom
 * \email: navid.nikaein@eurecom.fr
 * \note
 * \warning
 */

#include "UTIL/MATH/oml.h"
#include "otg_tx.h"
#include "otg_externs.h"//EDITED
//#include "OCG_extern.h"//EDITED
//#include "OCG.h"//EDITED
//-------------------------------------------
#include "../../LAYER2/MAC/utility.h"//EDITED
extern const int SIZE_SMALL;
extern const int SIZE_BIG;
extern const int MAX_NUM_OF_LOW_FPS;
extern const int MAX_NUM_OF_HIGH_FPS;
extern int PKT_COUNTER_1;
extern int PKT_COUNTER_2;
extern const int UNIT_COUNTER;
extern const int HIGH_FPS_UE;
extern const int LOW_FPS_UE; 
extern const int IDT;
extern int CTIME;
extern int ARR_CTIME_PASS_1[100];//UE_id=0 
extern int ARR_CTIME_PASS_2[100];//UE_id=1


//------------------------------------------
#include <stdbool.h>//EDITED
#include <stdio.h>//EDITED
#include <stdlib.h>//EDITED
#include <unistd.h>//EDITED
#include <errno.h>//EDITED

extern unsigned char NB_eNB_INST;
extern unsigned char NB_UE_INST;




static int c=1;
// char string for payload gen
const char HEADER_STRING[] =
  "048717272261061594909177115977673656394812939088509638561159848103044447631759621785741859753883189837913087874397230808338408049820398430984093477437879374893649639479575023646394693665969273071047";
const char PAYLOAD_STRING[] =
  "UVk5miARQfZGDFKf1wS0dt57kHigd0fXNrUZCjIhpyOS4pZWMHOP1GPdgXmlPtarLUjd3Rmkg05bhUZWtDDmdhrl5EzMZz6DkhIg0Uq7NlaU8ZGrt9EzgVLdr9SiBOLLXiTN3aMInMrlDYFYZ8n5WYbfZTnpz13lbMY4OBE4eWfIMLvBLLyzzzEqjUGILBVMfKGVccPi0VSCyg28RqAiR3z1P6zryk4FWFp0G78AUT1hZWhGcGOTDcKj9bCzny592m1Dj123KWczIm5KVLupO7AP83flqamimfLz6GtHrz5ZN2BAEVQjUhYSc35s5jDhofIlL2U4qPT3Ilsd7amTjaCl5zE0L89ZeIcPCWKSEuNdH5gG8sojuSvph1hU0gG4QOLhCk15IE8eCeMCz2LTL68U0hEQqeM6UmgmA9j7Eid7oPzQHbzj8A30HzGXGhWpt4CT3MSwWVvcCWSbYjkYGgOhHj5csTsONWyGAh5l3qquf8v3jGRSRu0nGXqYILCkw1SX9Na46qodrN6BnPl49djH2AuAaYKAStoR9oL7I1aZG6rVLFPMIZiAqF1tuDVcX9VWnyTVpTMXR6GtBp5bgfDyKuT4ZE9MDUASikGA5hoMfX5Gf2Ml7eLGBtEqZF4rouczHI0DRfgX4ev967n6dYFFkaXbFTvWdykN5bfMinzcrWeqVrmZhTvtUkvq3Rc9enM9qTNz6cDo0HHM0VD8EYtpaPH3yG2CYGDgogHlkaCcHaOyViyq8RH8wf4WQWoHuTNG1kWdkpgTrWic5Gv5p24O9YAPMOn6A1IsdvwpOF85qj8nPvj4nfIo385HOjGfadzfBXueruaKEa0lvbhLgS1bQWKv5fE7k2cMPzQ8USIpUyBhBGUHsLKaykvsr1qDTueAUWAGH8VqyozZZkyhWahjmFEEwU6hhcK1Z9wv9jOAAeqopQvbQFm4aQzzBwGIAhBqhZMiarIBwYPOFdPmK1hKHIa94GGtQbMZ0n83IGt6w8K3dqfOhmpQWqSRZscFwPuo4uhC0ByoC9hFpnizCBfoRZ7Gj9cGOzVLT2eMtD0XC8rUnDiR3p7Ke4ho6lWMLHmtCr7VWYIpY19dtiWoyU0FQ7VMlHoWeBhIUfuG54WVVX0h5Mvvvo0cnLQzh4knysVhAfQw9EhXq94mLrI9GydUaTihOvydQikfq2CrvLeNK81msBlYYoK0aT7OewTUI51YYufj7kYGkPVDf7t5n3VnMV3ShMERKwFyTNHQZyo9ccFibYdoT1FyMAfVeMDO89bUMKAD7RFaT9kUZpaIiH5W7dIbPcPPdSBSr1krKPtuQEeHVgf4OcQLfpOWtsEya4ftXpNw76RPCXmp4rjKt1mCh0pBiTYkQ5GDnj2khLZMzb1uua6R1ika8ACglrs1n0vDbnNjZEVpIMK4OGLFOXIOn9UBserI4Pa63PhUl49TGLNjiqQWdnAsolTKrcjnSklN1swcmyVU8B5gTO4Y3vhkG2U2";
const char FIXED_STRING[]=
  "ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD";
//may be put in vars
//packet_t *packet=NULL;
int type_header=0;
unsigned int state=OFF_STATE; // default traffic state
unsigned int application=0;
int ptime=0;
bool content_flag=false;//EDITED
int content_counter=0;
// Time Distribution function to distribute the inter-departure time using the required distribution

//unsigned char *new_random_string(const int src_instance,const int dst_instance);//EDITED

char *new_random_string(const int size,const int src_instance,const int dst_instance);//EDITED
int time_dist(
  const int src_instance,
  const int dst_instance,
  const int application,
  const int state)
{

  int idt=0;

  switch (g_otg->idt_dist[src_instance][dst_instance][application][state]) {
  case  UNIFORM:printf("\ntime UNIFORM\n");// %%EDITED
    idt =  ceil((uniform_dist(g_otg->idt_min[src_instance][dst_instance][application][state], g_otg->idt_max[src_instance][dst_instance][application][state])));
    break;

  case GAUSSIAN:printf("\ntime GAUSSIAN\n");// %%EDITED
    idt =  ceil((gaussian_dist((g_otg->idt_max[src_instance][dst_instance][application][state] + g_otg->idt_min[src_instance][dst_instance][application][state])/2 ,
                               g_otg->idt_std_dev[src_instance][dst_instance][application][state])));
    break;

  case EXPONENTIAL :printf("\ntime EXPONENTIAL\n");// %%EDITED
    idt=  ceil((exponential_dist(g_otg->idt_lambda[src_instance][dst_instance][application][state])));
    break;

  case  POISSON:printf("\ntime POISSON\n");// %%EDITED
    idt =  ceil((poisson_dist(g_otg->idt_lambda[src_instance][dst_instance][application][state])));
    break;

  case FIXED :printf("\ntime FIXED\n");// %%EDITED
    // idt = ceil((g_otg->idt_min[src_instance][dst_instance][application][state])); //IDT_TH *
//    idt = IDT;//5 8 9 safe for c41

			  if(HIGH_FPS_UE==1 && LOW_FPS_UE==0){
					  idt = IDT/2;
			  }else{
					  if(dst_instance==1){
							  idt = IDT;//IDT=2

					  }else{
							  idt = IDT/2;
					  }
			  }	
    break;

  case WEIBULL :printf("\ntime WEIBULL\n");// %%EDITED
    idt =ceil(weibull_dist(g_otg->idt_scale[src_instance][dst_instance][application][state],g_otg->idt_shape[src_instance][dst_instance][application][state] ));
    break;

  case PARETO :printf("\ntime PARETO\n");// %%EDITED
    idt =ceil(pareto_dist(g_otg->idt_scale[src_instance][dst_instance][application][state],g_otg->idt_shape[src_instance][dst_instance][application][state] ));
    break;

  case GAMMA :printf("\ntime GAMMA\n");// %%EDITED
    idt =ceil(gamma_dist(g_otg->idt_scale[src_instance][dst_instance][application][state],g_otg->idt_shape[src_instance][dst_instance][application][state] ));
    break;

  case CAUCHY :printf("\ntime CAUCHY\n");// %%EDITED
    idt =ceil(cauchy_dist(g_otg->idt_scale[src_instance][dst_instance][application][state],g_otg->idt_shape[src_instance][dst_instance][application][state] ));
    break;

  case LOG_NORMAL :printf("\ntime LOG_NORMAL\n");// %%EDITED
    idt =ceil((lognormal_dist((g_otg->idt_max[src_instance][dst_instance][application][state] + g_otg->idt_min[src_instance][dst_instance][application][state])/2 ,
                              g_otg->idt_std_dev[src_instance][dst_instance][application][state])));
    break;

  case TARMA :printf("\ntime TARMA\n");// %%EDITED
    idt=ceil(tarmaCalculateSample(otg_info->tarma_stream[src_instance][dst_instance][application]->tarma_input_samples,
                                  &(otg_info->tarma_stream[src_instance][dst_instance][application]->tarma_idt)));
    break;

  case VIDEO :printf("\ntime VIDEO\n");// %%EDITED
    idt = ceil((g_otg->idt_min[src_instance][dst_instance][application][state])); //IDT_TH *
    break;

  case BACKGROUND_DIST :printf("\ntime BACKGROUND_DIST\n");// %%EDITED
    idt = ceil((g_otg->idt_min[src_instance][dst_instance][application][state])); //IDT_TH *
    break;

  default :
    idt =0;
    LOG_W(OTG, "IDT distribution unknown, set to 0 for (src %d, dst %d, app %d, state %d)\n",
          src_instance, dst_instance, application, state );
  }

  LOG_D(OTG,"IDT :: Inter Departure Time Distribution= %d , val= %d\n", g_otg->idt_dist[src_instance][dst_instance][application][state],idt);
  
  // %EDITED%
  printf("IDT TIME: %d\n", idt);

  return idt;
}


// otg_params.size Distribution Function to distribute the packet otg_params.size using the required distribution


int size_dist(
  const int src_instance,
  const int dst_instance,
  const int application,
  const int state)
{

  int size_data=0;

  if (state==PU_STATE)
    size_data=g_otg->pu_size_pkts[src_instance][dst_instance][application];

  else if (state==ED_STATE)
    size_data=g_otg->ed_size_pkts[src_instance][dst_instance][application];
  else {
    LOG_D(OTG,"Size Distribution idx= %d \n", g_otg->size_dist[src_instance][dst_instance][application][state]);

    switch  (g_otg->size_dist[src_instance][dst_instance][application][state]) {
    case UNIFORM :printf("\nsize UNIFORM\n");// %%EDITED
      size_data = ceil(uniform_dist(g_otg->size_min[src_instance][dst_instance][application][state], g_otg->size_max[src_instance][dst_instance][application][state]));
      break;

    case GAUSSIAN :printf("\nsize GAUSSIAN\n");// %%EDITED
      size_data = ceil(gaussian_dist((g_otg->size_max[src_instance][dst_instance][application][state] + g_otg->size_min[src_instance][dst_instance][application][state])/2 ,
                                     g_otg->size_std_dev[src_instance][dst_instance][application][state]));
      break;

    case EXPONENTIAL :printf("\nsize EXPONENTIAL\n");// %%EDITED
      size_data= ceil(exponential_dist(g_otg->size_lambda[src_instance][dst_instance][application][state])); //SIZE_COEF *
      break;

    case POISSON :printf("\nsize POISSON\n");// %%EDITED
      size_data =ceil(poisson_dist(g_otg->size_lambda[src_instance][dst_instance][application][state]));
      break;

    case FIXED :printf("\nsize FIXED\n");// %%EDITED
	  // %EDITED%
	  // Change data size
      // size_data=ceil(g_otg->size_min[src_instance][dst_instance][application][state]);
//	   size_data = 220;//safe:180 530? 110 220; unsafe:90 100 360_for_c41_ind
//	   printf("\nIn size_dist, src_instance=%d dst_instance=%d\n",src_instance,dst_instance);//EDITED

//	   if((src_instance==0)&&(dst_instance==1)){//assume only data plane will choose FIXED mode, otherwise back to the last version
//			   if(HIGH_FPS_UE && LOW_FPS_UE){	
//					   size_data = 110;
//			   } else if (LOW_FPS_UE) {
//					   size_data = 110;
//			   } else {
//					   size_data = 220;
//			   }
//	   }else if((src_instance==0)&&(dst_instance==2)){
////			if(bug_counter==1){
////				size_data = 227;
////				bug_counter=0;
////			}else{
//				size_data = 220;
////			}
//		}

//				if(HIGH_FPS_UE && LOW_FPS_UE){
//						if(dst_instance==1){
//								size_data = 110;
//						}else{
//								size_data = 220;
//						}
//				} else if (LOW_FPS_UE){
//						size_data = 110;
//				} else {
//						size_data = 220;
//				}

				if(HIGH_FPS_UE==1 && LOW_FPS_UE==0){
						size_data = SIZE_BIG;

				} else {
						if(dst_instance==1){
								size_data = SIZE_SMALL;
						}else{
								size_data = SIZE_BIG;
						}

				}
				printf("\ndata_size choose as %d\n",size_data);
      break;

    case WEIBULL :printf("\nsize WEIBULL\n");// %%EDITED
      size_data =ceil(weibull_dist(g_otg->size_scale[src_instance][dst_instance][application][state],g_otg->size_shape[src_instance][dst_instance][application][state] ));
      break;

    case PARETO :printf("\nsize PARETO\n");// %%EDITED
      size_data =ceil(pareto_dist(g_otg->size_scale[src_instance][dst_instance][application][state],g_otg->size_shape[src_instance][dst_instance][application][state] ));
      break;

    case GAMMA :printf("\nsize GAMMA\n");// %%EDITED
      size_data =ceil(gamma_dist(g_otg->size_scale[src_instance][dst_instance][application][state],g_otg->size_shape[src_instance][dst_instance][application][state] ));
      break;

    case CAUCHY :printf("\nsize CAUCHY\n");// %%EDITED
      size_data =ceil(cauchy_dist(g_otg->size_scale[src_instance][dst_instance][application][state],g_otg->size_shape[src_instance][dst_instance][application][state] ));
      break;

    case LOG_NORMAL :printf("\nsize LOG_NORMAL\n");// %%EDITED
      size_data =ceil((lognormal_dist((g_otg->size_max[src_instance][dst_instance][application][state] + g_otg->size_min[src_instance][dst_instance][application][state])/2 ,
                                      g_otg->size_std_dev[src_instance][dst_instance][application][state])));
      break;

    case TARMA :printf("\nsize TARMA\n");// %%EDITED
      size_data=ceil(tarmaCalculateSample(otg_info->tarma_stream[src_instance][dst_instance][application]->tarma_input_samples,
                                          &(otg_info->tarma_stream[src_instance][dst_instance][application]->tarma_size)));
      break;

    case VIDEO :printf("\nsize VIDEO\n");// %%EDITED
      size_data=ceil(tarmaCalculateVideoSample (otg_info->tarma_video[src_instance][dst_instance][application]));
      break;

    case BACKGROUND_DIST :printf("\nsize BACKGROUND_DIST\n");// %%EDITED
      size_data = ceil(backgroundCalculateSize(otg_info->background_stream[src_instance][dst_instance][application],
                       otg_info->ctime, otg_info->idt[src_instance][dst_instance][application]));
      break;

    default:
      LOG_E(OTG, "PKT Size Distribution unknown (src %d, dst %d, app %d, state %d)\n",
            src_instance, dst_instance, application, state );
    }

  }

  //Case when size overfill min and max values
  size_data=adjust_size(size_data);
  LOG_D(OTG,"[src %d] [dst %d] [application %d] [state %d]Packet :: Size=%d  Distribution= %d \n", src_instance, dst_instance, application, state, size_data,
        g_otg->size_dist[src_instance][dst_instance][application][state]);

  return size_data;
}

int adjust_size(int size)
{

  if (size<PAYLOAD_MIN) {
    LOG_W(OTG,"Packet Size=%d out of range, size=%d \n", size, PAYLOAD_MIN);
    size=PAYLOAD_MIN;
  } else if  (size>PAYLOAD_MAX) {
    LOG_W(OTG,"Packet Size=%d out of range, size=%d \n", size, PAYLOAD_MAX);
    size=PAYLOAD_MAX;
  }

  return(size);
}



//int size_counter_0=0;//EDITED
unsigned char *packet_gen(
  const int src_instance,
  const int dst_instance,
  const int app,
  const int ctime,
  unsigned int * const pkt_size)
{
  // when pdcp, ctime = frame cnt

  //unsigned char *packet=NULL;
  unsigned int size=0;
  unsigned int buffer_size =0;
  unsigned char flow;
  unsigned int flow_id_background=1;
  unsigned int seq_num;
  unsigned int flag;
  char *payload=NULL;
  char *header=NULL;
  int header_size = 0;


  // check if the app is configured
  if (app >= g_otg->application_idx[src_instance][dst_instance]) {
    return NULL;
  }

  LOG_T(OTG,"[src %d] [dst %d ][APP %d] current time  %d\n",src_instance,  dst_instance, app, ctime);

  // %EDITED%
  printf("\nEnter packet_gen. Source : %d, Destination : %d\n", src_instance, dst_instance);//0 1

  *pkt_size=0;
  init_packet_gen(src_instance, dst_instance,ctime);
  size=check_data_transmit(src_instance,dst_instance,app,ctime);
  /*
  Send Packets when:
  - there is data to send
  - time to transmit background traffic
  - when (emutime- ctime)>20m ==> to avoid the fact that TX transmit and RX
  can't received due to the end of the emulation. ctime=current time.
   */
  if (((size>0) || (otg_info->traffic_type_background[src_instance][dst_instance]==1)) &&
      (((g_otg->max_nb_frames*10)-ctime)>20))   {

    /* No aggregation for background traffic   */
    if (otg_info->traffic_type_background[src_instance][dst_instance]==0) {//hit
      if (otg_info->header_size[src_instance][dst_instance] <= 0)
        otg_info->header_size[src_instance][dst_instance]=1;

      header = random_string(otg_info->header_size[src_instance][dst_instance], g_otg->packet_gen_type, HEADER_ALPHABET);//printf("\notg_info->header_size[src_instance][dst_instance] = %d\n\ng_otg->packet_gen_type = %d\n\nHEADER_ALPHABET = %d\n",otg_info->header_size[src_instance][dst_instance],g_otg->packet_gen_type,HEADER_ALPHABET);printf("header = %s\n",header);
      header_size = (header != NULL)? strlen(header) : 0;

	  // %EDITEn%
	  // Change data content
      // payload = random_string(size, 3010, PAYLOAD_ALPHABET);//size = payload size
     //  printf("\nsize_counter_0=%d\n",++size_counter_0);//total DL pkt

//	  if(dst_instance==2){
//          payload = random_string(size, RANDOM_STRING, PAYLOAD_ALPHABET);
//	  }else{
	      payload = new_random_string(size,src_instance,dst_instance);//0 1
//	  }

//	   if((size==110)||(size==220)){//EDITED
//
//	   		payload = new_random_string(src_instance,dst_instance);//0 1
//		}

	 //  if((src_instance==0)&&(dst_instance==1)){if(size>0)size=110;}else if((src_instance==0)&&(dst_instance==2)){if(size>0)size=220;}

	   if ((payload == NULL)){ puts("\ndata is NULL");//EDITED
			return NULL;
   
	   }else{
			 //  printf("\nsize=%d strlen(payload)=%d\n",size,strlen(payload));
	   }

      flag=0xffff;
      flow=otg_info->flow_id[src_instance][dst_instance];
      seq_num=otg_info->seq_num[src_instance][dst_instance][otg_info->traffic_type[src_instance][dst_instance]];
      otg_info->header_type[src_instance][dst_instance]=type_header;
      otg_info->seq_num[src_instance][dst_instance][otg_info->traffic_type[src_instance][dst_instance]]+=1;
      otg_info->tx_num_bytes[src_instance][dst_instance][otg_info->traffic_type[src_instance][dst_instance]]+=  otg_hdr_size(src_instance,dst_instance) + header_size + strlen(payload) ;
      otg_info->tx_num_pkt[src_instance][dst_instance][otg_info->traffic_type[src_instance][dst_instance]]+=1;
	  
      if (size!=strlen(payload)){
        LOG_E(OTG,"[%d][%d] [0x %x] The expected packet size does not match the payload size hit : size %d, strlen %d, seq_num %d packet: |%s|%s| \n",
              src_instance, dst_instance, flag,   size, strlen(payload), seq_num, header, payload);
		printf("\nsize=%d strlen(payload)=%d\n",size,strlen(payload));
		puts("data is discarded");
		return NULL;
      }else
        LOG_D(OTG,"[%d][%d] 0x %x][m2m Aggre %d][Flow %d][Type %d/%s] TX INFO pkt at time %d Size= [payload %d] [Total %d] with seq num %d, state=%d : |%s|%s| \n",
              src_instance, dst_instance, flag,
              otg_info->m2m_aggregation[src_instance][dst_instance],
              otg_info->flow_id[src_instance][dst_instance],
              otg_info->traffic_type[src_instance][dst_instance],
              map_int_to_str(otg_app_type_names,otg_info->traffic_type[src_instance][dst_instance]),
              ctime,  size, header_size+strlen(payload), seq_num,state, header, payload);

      LOG_D(OTG, "[%d]MY_SEQ %d \n", otg_info->traffic_type[src_instance][dst_instance], otg_info->seq_num[src_instance][dst_instance][otg_info->traffic_type[src_instance][dst_instance]] );
    } else {
      if ((g_otg->aggregation_level[src_instance][dst_instance][application]*otg_info->size_background[src_instance][dst_instance])<=PAYLOAD_MAX)
        otg_info->size_background[src_instance][dst_instance]=g_otg->aggregation_level[src_instance][dst_instance][application]*otg_info->size_background[src_instance][dst_instance];
      else {
        //otg_info->size_background[src][dst_instance]=PAYLOAD_MAX;
        LOG_E(OTG,"[BACKGROUND] Aggregated packet larger than PAYLOAD_MAX, payload is limited to %d\n", PAYLOAD_MAX);
      }

      header =random_string(header_size_gen_background(src_instance,dst_instance),  g_otg->packet_gen_type, HEADER_ALPHABET);
      payload = random_string(otg_info->size_background[src_instance][dst_instance],  RANDOM_STRING, PAYLOAD_ALPHABET);
      flag=0xbbbb;
      flow=flow_id_background;
      seq_num=otg_info->seq_num_background[src_instance][dst_instance];
      otg_info->tx_num_bytes_background[src_instance][dst_instance]+= otg_hdr_size(src_instance,dst_instance) + header_size + strlen(payload) ;
      otg_info->tx_num_pkt_background[src_instance][dst_instance]+=1;
      otg_info->seq_num_background[src_instance][dst_instance]+=1;

      if (otg_info->size_background[src_instance][dst_instance]!=strlen(payload))
        LOG_E(OTG,"[%d][%d] [0x %x] The expected packet size does not match the payload size : size %d, strlen %d, seq num %d, packet |%s|%s| \n",
              src_instance,
              dst_instance,
              flag,
              otg_info->size_background[src_instance][dst_instance],
              strlen(payload),
              seq_num,
              header,
              payload);
      else
        LOG_D(OTG,"[%d][%d][%s][0x %x] TX INFO pkt at time %d size is %d with seq num %d, state=%d : |%s|%s| \n",
              src_instance,
              dst_instance,
              flag,
              ctime,
              map_int_to_str(otg_app_type_names,otg_info->traffic_type[src_instance][dst_instance]),
              otg_info->size_background[src_instance][dst_instance],
              seq_num,
              state,
              header,
              payload);
    }
    buffer_size = otg_hdr_size(src_instance,dst_instance) + header_size + strlen(payload);
    *pkt_size = buffer_size;

    if (src_instance<NB_eNB_INST)
      otg_info->tx_total_bytes_dl+=buffer_size;
    else
      otg_info->tx_total_bytes_ul+=buffer_size;

    if (otg_info->traffic_type[src_instance][dst_instance] > MAX_NUM_APPLICATION) {
      LOG_W(OTG,"application type out of range %d for the pair of (src %d, dst %d) \n",
            otg_info->traffic_type[src_instance][dst_instance], src_instance, dst_instance);
      otg_info->traffic_type[src_instance][dst_instance]=0;
    }
	
	// %EDITED%
	printf("\nExit packet_gen.\n");

    return serialize_buffer(header,
                            payload,
                            buffer_size,
                            otg_info->traffic_type[src_instance][dst_instance],
                            flag,
                            flow,
                            ctime,
                            seq_num,
                            otg_info->header_type_app[src_instance][dst_instance][flow],
                            state,
                            otg_info->m2m_aggregation[src_instance][dst_instance],
                            src_instance,
                            dst_instance);
  } else {

	// %EDITED%
	printf("Exit packet_gen, but generate nothing.\n");

    return NULL;
  }
}



unsigned char *packet_gen_multicast(
  const int src_instance,
  const int dst_instance,
  const int ctime,
  unsigned int * const pkt_size)
{

  *pkt_size =0;
  unsigned int size=0;
  unsigned int buffer_size =0;
  char *payload=NULL;
  char *header=NULL;
  unsigned int flag;
  int app,seq_num=0;
  int otg_hdr_size= sizeof(otg_hdr_info_t) + sizeof(otg_hdr_t);

  set_ctime(ctime); // fixme: this should be done separetly from packet_gen and packet_gen_multicast

  //for (app=0; app<MAX_NUM_APPLICATION; app++){
  for (app=0; app<1; app++) {

    if ( (g_otg_multicast->idt_dist[src_instance][dst_instance][app]> 0) &&
         ((ctime - otg_multicast_info->ptime[src_instance][dst_instance][app]) >= otg_multicast_info->idt[src_instance][dst_instance][app]) ) {

      //Duy add
      LOG_I(OTG,"multicast gen: entering generating\n");
      //end Duy add

      //otg_info->idt[src_instance][dst_instance][app]= time_dist(src_instance, dst_instance, app, -1);
      otg_multicast_info->idt[src_instance][dst_instance][app]=ceil(uniform_dist(g_otg_multicast->idt_min[src_instance][dst_instance][app],
          g_otg_multicast->idt_max[src_instance][dst_instance][app]));
      size = ceil(uniform_dist(g_otg_multicast->size_min[src_instance][dst_instance][app],
                               g_otg_multicast->size_max[src_instance][dst_instance][app]));
      LOG_D(OTG, "ptime %d, ctime %d idt %d (min %d, max %d) size %d (min %d, max %d)\n",
            otg_multicast_info->ptime[src_instance][dst_instance][application],
            ctime,
            otg_multicast_info->idt[src_instance][dst_instance][app],
            g_otg_multicast->idt_min[src_instance][dst_instance][app],
            g_otg_multicast->idt_max[src_instance][dst_instance][app],
            size,g_otg_multicast->size_min[src_instance][dst_instance][app],
            g_otg_multicast->size_max[src_instance][dst_instance][app]);

      otg_multicast_info->ptime[src_instance][dst_instance][application]=ctime;

      if (size == 0)
        size = 1;

      if (otg_multicast_info->header_size_app[src_instance][dst_instance][app]==0) {
        otg_multicast_info->header_size_app[src_instance][dst_instance][app]=1;
        LOG_W(OTG,"header type not defined, set to 1\n");
      }

      header = random_string(otg_multicast_info->header_size_app[src_instance][dst_instance][app],
                             g_otg->packet_gen_type,
                             HEADER_ALPHABET);
      payload = random_string(size, RANDOM_STRING, PAYLOAD_ALPHABET);
      flag = 0x1000;
      seq_num=otg_multicast_info->tx_sn[src_instance][dst_instance][app]++;
      otg_multicast_info->tx_num_pkt[src_instance][dst_instance][app]+=1;
      otg_multicast_info->tx_num_bytes[src_instance][dst_instance][app]+= strlen(header) + strlen(payload) + otg_hdr_size;
      LOG_D(OTG,"otg_multicast_info->tx_num_bytes[%d][%d][%d] = %d \n",
            src_instance,
            dst_instance,
            app,
            otg_multicast_info->tx_num_bytes[src_instance][dst_instance][app]);

      if (size!=strlen(payload))
        LOG_E(OTG,"[src %d][dst %d] The expected packet size does not match the payload size : size %d, strlen %d \n",
              src_instance, dst_instance, size, strlen(payload));
      else {
        LOG_I(OTG,"[src %d][dst %d]TX INFO pkt at time %d Size= [payload %d] [Total %d] with seq num %d: |%s|%s| \n",
              src_instance,
              dst_instance,
              ctime,
              size,
              strlen(header)+strlen(payload)+otg_hdr_size,
              seq_num,
              header,
              payload);
        LOG_D(OTG,"\n");
      }

      buffer_size = otg_hdr_size + strlen(header) + strlen(payload);
      *pkt_size = buffer_size;
      break;
    } else {
      //LOG_D(OTG,"no packet is generated \n");
    }
  }

  if (buffer_size) {
    return serialize_buffer(
             header,
             payload,
             buffer_size,
             0/*g_otg_multicast->application_type[src_instance][dst][app]*/,
             flag, 0, ctime,
             seq_num,
             0,
             HDR_IP_v4_MIN+HDR_UDP,
             1,
             src_instance,
             dst_instance);
  }

  // application_types is MSCBR = 1 is set in g_otg_multicast init, but 0 is need in otg_rx for coherence with index of otg_multicast_info
  return NULL;
}



int otg_hdr_size(
  const int src_instance,
  const int dst_instance)
{
  //static int c=1;
  if (otg_info->hdr_size[src_instance][dst_instance]==0)
    otg_info->hdr_size[src_instance][dst_instance]=sizeof(otg_hdr_info_t) + sizeof(otg_hdr_t);

  return otg_info->hdr_size[src_instance][dst_instance];
}



void init_packet_gen(
  const int src_instance,
  const int dst_instance,
  const int ctime)
{
  check_ctime(ctime);
  set_ctime(ctime);
  otg_info->m2m_aggregation[src_instance][dst_instance]=0;
  otg_info->flow_id[src_instance][dst_instance]=0;
  otg_info->traffic_type[src_instance][dst_instance]=0;
  otg_info->traffic_type_background[src_instance][dst_instance]=0;
  /* init background traffic*/
  /*if (otg_info->idt_background[src_instance][dst_instance]==0){
       otg_info->idt_background[src_instance][dst_instance]= exponential_dist(0.025);
       otg_info->background_stream[src_instance][dst_instance][0]=backgroundStreamInit(0,1);
  } */
}



void check_ctime(int ctime)
{
  if (ptime>ctime)
    LOG_W(OTG, "ERROR ctime: current time [%d] less than previous time [%d] \n",ctime,ptime);

  ptime=ctime;
}


int tx_counter_1=0;
int tx_counter_2=0;
int check_data_transmit(
  const int src_instance,
  const int dst_instance,
  const int app,
  const int ctime)
{

  unsigned int size=0;

  // for (application=0; application<g_otg->application_idx[src_instance][dst_instance]; application++){
  otg_info->gen_pkts=0;

  LOG_D(OTG,"FLOW_INFO [src %d][dst %d] [IDX %d] [APPLICATION TYPE %d] MAX %d [M2M %d ]\n",
        src_instance, dst_instance, app,
        g_otg->application_type[src_instance][dst_instance][app],
        g_otg->application_idx[src_instance][dst_instance],
        g_otg->m2m[src_instance][dst_instance][app]);
  g_otg->flow_duration[src_instance][dst_instance][app]=35000;//EDITED
  //EDITED
//  if(dst_instance==2){//for bug UE
//		  g_otg->flow_start[src_instance][dst_instance][app]=692;//EDITED 657dead
//  }else{
//		  g_otg->flow_start[src_instance][dst_instance][app]=727;//EDITED 727original
//      
//  }

  // do not generate packet for this pair of src_instance, dst_instance : no app type and/or no idt are defined
  if (g_otg->flow_start[src_instance][dst_instance][app] > ctime ) {
    //g_ otg->flow_start_flag[src_instance][dst_instance][app]=1;
    LOG_I(OTG,"Duration_reach_1 Flow start time not reached : do not generate packet for this pair of src=%d, dst=%d, start %d > ctime %d \n",
          src_instance,
          dst_instance,g_otg->flow_start[src_instance][dst_instance][app],
          ctime);
    size=0;
  } else if (g_otg->flow_duration[src_instance][dst_instance][app] + g_otg->flow_start[src_instance][dst_instance][app] < ctime ) {
//    LOG_I(OTG,"Duration_reach_2 Flow duration reached: do not generate packet for this pair of src=%d, dst =%d, duration %d < ctime %d + start %d\n",
//          src_instance, dst_instance,
//          g_otg->flow_duration[src_instance][dst_instance][app],
//          ctime,
//          g_otg->flow_start[src_instance][dst_instance][app]);
		  LOG_I(OTG,"Duration_reach_2 Flow duration reached: do not generate packet for this pair of src=%d, dst=%d, start %d + duration %d < ctime %d\n",src_instance, dst_instance, g_otg->flow_start[src_instance][dst_instance][app],g_otg->flow_duration[src_instance][dst_instance][app],ctime);

    size=0;
  } else if ((g_otg->application_type[src_instance][dst_instance][app]==0)&&(g_otg->idt_dist[src_instance][dst_instance][app][PE_STATE]==0)) {
    LOG_I(OTG,"Do not generate packet for this pair of src=%d, dst =%d, IDT zero and app %d not specificed\n",
          src_instance,
          dst_instance, app);
    size=0;
  }

  else if ((g_otg->application_type[src_instance][dst_instance][app] >0) || (g_otg->idt_dist[src_instance][dst_instance][app][PE_STATE] > 0)) {
    state = get_application_state(src_instance, dst_instance, app, ctime);

#ifdef STANDALONE

    //pre-config for the standalone
    if (ctime<otg_info->ptime[src_instance][dst_instance][app]) //it happends when the emulation was finished
      otg_info->ptime[src_instance][dst_instance][app]=ctime;

    if (ctime==0)
      otg_info->idt[src_instance][dst_instance][app]=0; //for the standalone mode: the emulation is run several times, we need to initialise the idt to 0 when ctime=0

    //end pre-config
#endif
   // LOG_I(OTG,"MY_STATE %d\n", state);//EDITED

    if (state!=OFF_STATE) {

      if (((state==PU_STATE)||(state==ED_STATE))|| (otg_info->idt[src_instance][dst_instance][app]==0)
          || (( (ctime-otg_info->ptime[src_instance][dst_instance][app]) >= otg_info->idt[src_instance][dst_instance][app] ) )) {
		//EDITED	
//		switch(state){
//			case PU_STATE:
//				puts("state=PU_STATE");
//				break;
//			case ED_STATE:
//				puts("state=PU_STATE");
//				break;
//			default:
//				puts("other STATE");
//				break;
//		}
//        printf("\nstate=%d ")
        LOG_I(OTG,"[TX] OTG packet: Time To Transmit::OK (Source = %d, Destination = %d, Application %d, State = %d), (IDT = %d ,ctime = %d, ptime = %d)\n",
              src_instance,
              dst_instance ,
              app,
              state,
              otg_info->idt[src_instance][dst_instance][app],
              ctime,
              otg_info->ptime[src_instance][dst_instance][app]);// %EDITED%

        if(dst_instance==1)printf("\ntx_counter_1=%d\n",++tx_counter_1);//EDITED
        if(dst_instance==2)printf("\ntx_counter_2=%d\n",++tx_counter_2);//EDITED
        otg_info->ptime[src_instance][dst_instance][app]=ctime;

        if (state==PE_STATE)  //compute the IDT only for PE STATE
          tarmaUpdateInputSample(otg_info->tarma_stream[src_instance][dst_instance][app]);

        otg_info->idt[src_instance][dst_instance][app]=time_dist(src_instance, dst_instance, app,state);
        otg_info->gen_pkts=1;
        header_size_gen(src_instance,dst_instance, app);
        //for(i=1;i<=g_otg->aggregation_level[src_instance][dst_instance][application];i++)
        /*    if   (g_otg->m2m[src_instance][dst_instance][application]==M2M){ //TO FIX FOR M2M
        size+=size_dist(src_instance, dst_instance, application,state);
        if (otg_info->header_size_app[src_instance][dst_instance][application] > otg_info->header_size[src_instance][dst_instance]) //adapt the header to the application (increment the header if the the new header size is      largest that the already computed)
        otg_info->header_size[src_instance][dst_instance]+=otg_info->header_size_app[src_instance][dst_instance][application];
        otg_info->m2m_aggregation[src_instance][dst_instance]++;
        otg_info->flow_id[src_instance][dst_instance]=application;
        otg_info->traffic_type[src_instance][dst_instance]=g_otg->application_type[src_instance][dst_instance][application] //M2M;
        }
        else{ */
        /* For the case of non M2M traffic: when more than one flows transmit data in the same time
        --> the second flow transmit  (because of non data aggragation)  */
        size=size_dist(src_instance, dst_instance, app,state);
        otg_info->header_size[src_instance][dst_instance]=otg_info->header_size_app[src_instance][dst_instance][app];
        otg_info->flow_id[src_instance][dst_instance]=app;
        otg_info->traffic_type[src_instance][dst_instance]=app;//g_otg->application_type[src_instance][dst_instance][app];
        /*} */


        /* if the aggregated size is less than PAYLOAD_MAX the traffic is aggregated, otherwise size=PAYLOAD_MAX */
        if (size>=(PAYLOAD_MAX-(sizeof(otg_hdr_info_t) + sizeof(otg_hdr_t) + otg_info->header_size[src_instance][dst_instance]))) {
          //size=PAYLOAD_MAX- (sizeof(otg_hdr_info_t) + sizeof(otg_hdr_t) + otg_info->header_size[src_instance][dst_instance]);
          LOG_E(OTG,"Aggregated packet larger than PAYLOAD_MAX, payload is limited to %d \n", PAYLOAD_MAX );
        }

      }  //check if there is background traffic to generate
      else if ((otg_info->gen_pkts==0) && (g_otg->background[src_instance][dst_instance][app]==1)&&(background_gen(src_instance, dst_instance, ctime)!=0)) { // the gen_pkts condition could be relaxed here
        otg_info->traffic_type_background[src_instance][dst_instance]=1;

        if   (g_otg->m2m[src_instance][dst_instance][app]==M2M)
          otg_info->traffic_type[src_instance][dst_instance]=app;//g_otg->application_idx[src_instance][dst_instance];//M2M;

        LOG_D(OTG,"[BACKGROUND=%d] Time To Transmit [SRC %d][DST %d][APPLI %d] \n", otg_info->traffic_type_background[src_instance][dst_instance], src_instance, dst_instance, app);
      }

    }

  }

  // }

  return size;
}



unsigned int get_application_state(
  const int src_instance,
  const int dst_instance,
  const int application,
  const int ctime)
{

  switch (g_otg->application_type[src_instance][dst_instance][application]) {
  case VOIP_G711:
  case VOIP_G729:
    voip_traffic(src_instance, dst_instance, application, ctime);
    return otg_info->voip_state[src_instance][dst_instance][application];
    break;

  default:
    state_management(src_instance,dst_instance,application, ctime);
    return otg_info->state[src_instance][dst_instance][application];
    break;
  }
}



void header_size_gen(
  const int src_instance,
  const int dst_instance,
  const int application)
{

  unsigned int size_header=0;

  if (otg_info->header_size_app[src_instance][dst_instance][application]==0) {
    if (g_otg->ip_v[src_instance][dst_instance][application]==1) {
      size_header+=HDR_IP_v4_MIN;
      otg_info->header_type_app[src_instance][dst_instance][application]+=0;
    } else if  (g_otg->ip_v[src_instance][dst_instance][application]==2) {
      size_header+=HDR_IP_v6;
      otg_info->header_type_app[src_instance][dst_instance][application]+=2;

    }

    if (g_otg->trans_proto[src_instance][dst_instance][application]==1) {
      size_header+= HDR_UDP ;
      otg_info->header_type_app[src_instance][dst_instance][application]+=1;
    } else if (g_otg->trans_proto[src_instance][dst_instance][application]==2) {
      size_header+= HDR_TCP;
      otg_info->header_type_app[src_instance][dst_instance][application]+=2;
    }

    if ((g_otg->application_type[src_instance][dst_instance][application]==VOIP_G711)||(g_otg->application_type[src_instance][dst_instance][application]==VOIP_G729))
      size_header+=RTP_HEADER;

    LOG_D(OTG,"Header size is %d [IP V=%d][PROTO=%d]\n",  size_header, g_otg->ip_v[src_instance][dst_instance][application],g_otg->trans_proto[src_instance][dst_instance][application]);
    otg_info->header_size_app[src_instance][dst_instance][application]=size_header;
  }
}



void header_size_gen_multicast(
  const int src_instance,
  const int dst_instance,
  const int application)
{

  unsigned int size_header=0;

  if (otg_multicast_info->header_size_app[src_instance][dst_instance][application]==0) {

    if (g_otg_multicast->ip_v[src_instance][dst_instance][application]==1) {
      size_header+=HDR_IP_v4_MIN;
    } else if  (g_otg_multicast->ip_v[src_instance][dst_instance][application]==2) {
      size_header+=HDR_IP_v6;
    }

    if (g_otg_multicast->trans_proto[src_instance][dst_instance][application]==1) {
      size_header+= HDR_UDP ;
    } else if (g_otg_multicast->trans_proto[src_instance][dst_instance][application]==2) {
      size_header+= HDR_TCP;
    }

    //   LOG_I(OTG,"multicast header is set to %d\n", size_header);
    otg_multicast_info->header_size_app[src_instance][dst_instance][application]=size_header;
  }
}
//EDITED
/*
int new_content_counter_1=0;
char new_content_counter_2='@';
FILE *fPtr_read_1;
FILE *fPtr_read_2;
int pkt_counter_1=0;//one frame:ceil(38016/106)=359
int pkt_counter_2=0;//one frame:ceil(38016/216)=176
const int small_size=110;
const int big_size=220;
unsigned char byte_v[1];
unsigned char byte_u[1];
unsigned char byte_s[2];
unsigned char byte_p1_1[106];
unsigned char byte_p1_2[216];
unsigned char byte_p2_1[68];
unsigned char byte_pr_1[38]= {'\0'};
unsigned char *new_random_string(const int src_instance,const int dst_instance){
		unsigned char *data=NULL;

		if((src_instance==0)&&(dst_instance==1)){
				if(pkt_counter_1==0){
						printf("\npkt_counter_1=%d\n",pkt_counter_1);//
						fPtr_read_1 = fopen("/home/enbuser/oaisim_test/openair2/UTIL/OTG/dec_Base_176x144_3.75_size_110_one_frame_product.yuv","rb");//one frame
				}
				if(pkt_counter_1<359){//one frame=359
						if(pkt_counter_1!=0){
							printf("\npkt_counter_1=%d\n",pkt_counter_1);//
						}
						if(fPtr_read_1){
								data = (char*) malloc((small_size+1) * sizeof(char));
								fread(byte_v,sizeof(byte_v),1,fPtr_read_1);
								data[0]=byte_v[0];
								fread(byte_u,sizeof(byte_u),1,fPtr_read_1);
								data[1]=byte_u[0];
								fread(byte_s,sizeof(byte_s),1,fPtr_read_1);
								data[2]=byte_s[0];
								data[3]=byte_s[1];
								fread(byte_p1_1,sizeof(byte_p1_1),1,fPtr_read_1);//TODO

								//debug-purposed payload
								int i=4;//109-4+1=106,106=110-4
								for (i; i < small_size; i++)
								{
										data[i] =new_content_counter_1+'0';
										if((i%small_size)==(small_size-1)){
												new_content_counter_1++;
												if((new_content_counter_1%10)==0){
														new_content_counter_1=0;
												}
										}
								}
								data[small_size] = '\0';
								pkt_counter_1++;
						}else if(fPtr_read_1==NULL){
								perror("\nread file 1 err\n");
						}
				}
		}
		if((src_instance==0)&&(dst_instance==2)){
				if(pkt_counter_2==0){
						printf("\npkt_counter_2=%d\n",pkt_counter_2);//
						fPtr_read_2 = fopen("/home/enbuser/oaisim_test/openair2/UTIL/OTG/dec_Base_176x144_7.5_size_220_one_frame_product.yuv","rb");//one frame
				}	
				if(pkt_counter_2<176){//one frame=176
						if(pkt_counter_2!=0){
							printf("\npkt_counter_2=%d\n",pkt_counter_2);
						}
						if(fPtr_read_2){
								data = (char*) malloc((big_size+1) * sizeof(char));
								fread(byte_v,sizeof(byte_v),1,fPtr_read_2);
								data[0]=byte_v[0];
								fread(byte_u,sizeof(byte_u),1,fPtr_read_2);
								data[1]=byte_u[0];
								fread(byte_s,sizeof(byte_s),1,fPtr_read_2);
								data[2]=byte_s[0];
								data[3]=byte_s[1];
								fread(byte_p1_2,sizeof(byte_p1_2),1,fPtr_read_2);//TODO
								//debug-purposed payload
								int i=4;//219-4+1=216,216=220-4
								for (i; i < big_size; i++)
								{
										data[i] =new_content_counter_2;
										if((i%big_size)==(big_size-1)){
												new_content_counter_2++;
												if((new_content_counter_2/74)==1){
														new_content_counter_2='@';
												}
										}
								}
								data[big_size] = '\0';
								pkt_counter_2++;
						}else if(fPtr_read_2==NULL){
								perror("\nread file 2 err\n");
						}
				}
		}
		return data;
}*/
/*
int new_content_counter_1=0;
char new_content_counter_2='@';
char *new_random_string(const int size,const int src_instance,const int dst_instance){//EDITED

//		char cwd[1024];
//        if (getcwd(cwd, sizeof(cwd)) != NULL)
//                fprintf(stdout, "Current working dir: %s\n", cwd);
//        else
//                perror("getcwd() error");

		FILE *fPtr_read_1;
		fPtr_read_1 = fopen("/home/enbuser/oaisim_test/openair2/UTIL/OTG/dec_Base_176x144_3.75_size_110_one_frame_product.yuv","rb");
		if(fPtr_read_1){
			puts("read success");
		}else if(fPtr_read_1==NULL){
			perror("Error");
		}
        unsigned char *data=NULL;
		data = (char*) malloc((size+1) * sizeof(char));
        if((src_instance==0)&&(dst_instance==1)){
                int i=0;
                for (i; i < size; i++)
                {
                        data[i] =new_content_counter_1+'0';
//                        printf("%02x ", data[i]);//
                        if((i%220)==219){
                                new_content_counter_1++;
                                if((new_content_counter_1%10)==0){
                                        new_content_counter_1=0;
                                }
                        }
//                        if(((i%10)==9)){//
//                                puts("");//
//                        }//
                }
//                puts("");//
                data[size] = '\0';
        }
        if((src_instance==0)&&(dst_instance==2)){
                int i=0;
                for (i; i < size; i++)
                {
                        data[i] =new_content_counter_2;
//                        printf("%02x ", data[i]);//
                        if((i%220)==219){
                                new_content_counter_2++;
                                if((new_content_counter_2/74)==1){
                                        new_content_counter_2='@';
                                }
                        }
//                        if(((i%10)==9)){//
//                                puts("");//
//                        }//
                }
//                puts("");//
                data[size] = '\0';
        }
		 return data;
}
*/


//int size_counter=0;
//int pkt_counter_1=0;//one frame:ceil(38016/106)=359 --> extern 
//int pkt_counter_2=0;//one frame:ceil(38016/216)=176 -- > extern
FILE *fPtr_read_1;
FILE *fPtr_read_2;
unsigned char byte_v[1];
unsigned char byte_u[1];
unsigned char byte_s[2];
unsigned char byte_p1_1[106];
unsigned char byte_p1_2[216];
unsigned char byte_fake_1[110]={'a'};//SIZE_SMALL
unsigned char byte_fake_2[220]={'a'};//SIZE_BIG idt related
int new_content_counter_1=0;
char new_content_counter_2='@';
//int free_counter_1=1;
//int free_counter_2=1;
int initial_counter=1;
int fake_counter_1=0;
int fake_counter_2=0;
int FAKE_COUNTER_1;
int back_count=0;
int unit_time_counter_0_1 = 0;//UE_id=0
int unit_time_counter_0_2 = 0;//UE_id=1

char *new_random_string(const int size,const int src_instance,const int dst_instance){//EDITED
	
		//for time backoff at 1128 purpose
        if((HIGH_FPS_UE^LOW_FPS_UE)==0){//two UEs
				if(initial_counter==1){
						switch(IDT){
								case 5:
										FAKE_COUNTER_1=81;
										break;
								case 4:
										FAKE_COUNTER_1=101;
										break;
								case 2:
										FAKE_COUNTER_1=201;
										break;
								case 1:
										FAKE_COUNTER_1=401;
										break;
						}
						initial_counter=0;			
				}
		}else{//single UE
				if(initial_counter==1){

						if(HIGH_FPS_UE==1){
								FAKE_COUNTER_1=401;
						}else{//LOW_FPS_UE=1
								FAKE_COUNTER_1=201;
						}

						initial_counter=0;			
				}

		}
		
        unsigned char *data=NULL;
        if((dst_instance==1)){
			if(back_count<FAKE_COUNTER_1){		//TODO
				back_count++;
				printf("back_count=%d\n",back_count);
			}
			else{
			    //dst_instance==1:one UE, or the first UE			
				if(HIGH_FPS_UE==1 && LOW_FPS_UE==0)	{
						if(PKT_COUNTER_1<MAX_NUM_OF_HIGH_FPS*UNIT_COUNTER){
								data = (char*) malloc((size+1) * sizeof(char));
								//debug-purposed payload
								//int i=4;
								int i=0;
								for (i; i < size; i++)
								{
										data[i] =new_content_counter_2;
										if((i%SIZE_BIG)==(SIZE_BIG-1)){
												new_content_counter_2++;
												if((new_content_counter_2/74)==1){
														new_content_counter_2='@';
												}
										}
								}
								data[size] = '\0';				
								printf("\npkt_counter_1=%d\n",++PKT_COUNTER_1);	
								//debug region
								if((PKT_COUNTER_1%MAX_NUM_OF_HIGH_FPS)==1){//first pkt of a unit time
									unit_time_counter_0_1++;	
									printf("UE %d : unit time %d begin, CTIME=%d\n",dst_instance,unit_time_counter_0_1,CTIME);
									ARR_CTIME_PASS_1[unit_time_counter_0_1]=CTIME;
									printf("ARR_CTIME_PASS_1[%d]=%d\n",unit_time_counter_0_1,ARR_CTIME_PASS_1[unit_time_counter_0_1]);

								}

					
						}
				}
				else{
						if(PKT_COUNTER_1<MAX_NUM_OF_LOW_FPS*UNIT_COUNTER){
								data = (char*) malloc((size+1) * sizeof(char));
								//debug-purposed payload
								//int i=4;
								int i=0;
								for (i; i < size; i++)
								{
										data[i] =new_content_counter_1+'0';
										if((i%SIZE_SMALL)==(SIZE_SMALL-1)){
												new_content_counter_1++;
												if((new_content_counter_1%10)==0){
														new_content_counter_1=0;
												}
										}
								}
								data[size] = '\0';				
								printf("\npkt_counter_1=%d\n",++PKT_COUNTER_1);	
								//debug region
								if((PKT_COUNTER_1%MAX_NUM_OF_LOW_FPS)==1){//first pkt of a unit time

									unit_time_counter_0_1++;		
									printf("UE %d : unit time %d begin, CTIME=%d\n",dst_instance,unit_time_counter_0_1,CTIME);
									ARR_CTIME_PASS_1[unit_time_counter_0_1]=CTIME;								
									printf("ARR_CTIME_PASS_1[%d]=%d\n",unit_time_counter_0_1,ARR_CTIME_PASS_1[unit_time_counter_0_1]);
								}
						}
					
				}
			}//back count
     	}
        if((dst_instance==2)){//UE_list length = 2
            if(PKT_COUNTER_2<MAX_NUM_OF_HIGH_FPS*UNIT_COUNTER+7){//+7 for bug padding
                    data = (char*) malloc((size+1) * sizeof(char));

					if(fake_counter_2<7){//+7 for bug padding
						int i=0;
						for(i;i<size;i++){
								data[i]=byte_fake_2[i];
						}
						data[size] = '\0';
						fake_counter_2++;
					}else{
							//debug-purposed payload
							//int i=4;
							int i=0;
							for (i; i < size; i++)
							{
									data[i] =new_content_counter_2;
									if((i%SIZE_BIG)==(SIZE_BIG-1)){
											new_content_counter_2++;
											if((new_content_counter_2/74)==1){
													new_content_counter_2='@';
											}
									}
							}
							data[size] = '\0';
					}
//					printf("\npkt_counter_2=%d\n",++PKT_COUNTER_2);       
                    int shift;
					PKT_COUNTER_2++;
					shift=PKT_COUNTER_2-7; 
					printf("\npkt_counter_2=%d\n",shift);//shift for bug padding.       
					if(shift>=1){
					//debug region
					if((shift%MAX_NUM_OF_HIGH_FPS)==1){//first pkt of a unit time

							unit_time_counter_0_2++;
							printf("UE %d : unit time %d begin, CTIME=%d\n",dst_instance,unit_time_counter_0_2,CTIME);
							ARR_CTIME_PASS_2[unit_time_counter_0_2]=CTIME+1;
							printf("ARR_CTIME_PASS_2[%d]=%d\n",unit_time_counter_0_2,ARR_CTIME_PASS_2[unit_time_counter_0_2]);	

					}
					}

			}
		}
		 return data;
}

// Generate a random string[size]
char *random_string(
  const int size,//40, for header
  const ALPHABET_GEN mode,//header mode=1, SUBSTRACT_STRING case
  const ALPHABET_TYPE data_type)//header data_type=0
{
  unsigned char *data=NULL;//EDITED char --> unsigned char
  int i=0,start=0;

  switch (mode) {
  case REPEAT_STRING:
    start = uniform_dist (0, abs(strlen(FIXED_STRING)- size - 1));
    return str_sub (FIXED_STRING, start, start + size -1);
    break;

  case SUBSTRACT_STRING://header hit, SUBSTRACT_STRING=1 

    //data=strndup(data_string + (strlen(data_string) - size), strlen(data_string));
    //data=strndup(data_string + (strlen(data_string) - size), size);
    if (data_type == HEADER_ALPHABET) {//hit, HEADER_ALPHABET = 0
      start = uniform_dist (0, abs(strlen(HEADER_STRING)- size - 1));
      return str_sub (HEADER_STRING, start, start + size -1);
    } else if (data_type == PAYLOAD_ALPHABET) {
      start = uniform_dist (0, abs(strlen(PAYLOAD_STRING)- size - 1));
      return str_sub (PAYLOAD_STRING, start, start+size - 1 );
    } else
      LOG_D(OTG, "unsupported alphabet data type \n");

    break;

  case RANDOM_POSITION:

    data=(char*)malloc((size+1)*sizeof(char));
	
    for(i=0; i<=size; i++) {
      if (data_type == HEADER_ALPHABET)
        data[i]=HEADER_STRING [(int)uniform_dist(0, 128)];
      else if (data_type == PAYLOAD_ALPHABET)
        data[i]=PAYLOAD_STRING [(int)uniform_dist(0, 1500)];
      else
        LOG_E(OTG,"unsupported alphabet data type \n");

      //pos = rand()%(strlen(data_string));
      //data[i]=data_string[pos];
      //data[i]= taus(OTG) % (126 - 33 + 1) + 33;
    }

    data[size] = '\0';
    break;

  case RANDOM_STRING://original payload hit


    data=(char*)malloc((size+1)*sizeof(char));

    for(i=0; i<=size; i++) {
      if (data_type == HEADER_ALPHABET)
        data[i]= (char) uniform_dist(48,57); // take only the numbers
      else if (data_type == PAYLOAD_ALPHABET)
        data[i]= (char) uniform_dist(48,126); // take all the ascii chars
      else
        LOG_E(OTG,"unsupported alphabet data type \n");
    }

    data[size] = '\0';
    break;

  // %EDITED%
  // Change data content
  case 3010:
	data = (char*) malloc((size+1) * sizeof(char));
	

	for (int i = 0; i < size; i++) {
			data[i] = content_counter + '0';
			if((i%220)==219){
					content_counter++;
					if((content_counter%10)==0){
							content_counter=0;
					}
			}
	} 


    
    data[size] = '\0';
    printf("DATA SIZE : %d\n",size);
	break;    




  default:
    LOG_E(OTG,"not supported string generation");
    break;
  }

  return data;
}



unsigned int header_size(const int hdr_size)
{

  int size = hdr_size;

  if (hdr_size>(sizeof(otg_hdr_info_t) + sizeof(otg_hdr_t)))
    size-=(sizeof(otg_hdr_info_t) + sizeof(otg_hdr_t));
  else
    LOG_W(OTG,"OTG Header not included inside packet header (OTG header:%d, Header%d)\n", hdr_size, sizeof(otg_hdr_info_t) + sizeof(otg_hdr_t) );

  return(size);

}



unsigned char * serialize_buffer(
  char* const header,
  char* const payload,
  const unsigned int buffer_size,
  const unsigned int traffic_type,
  const int flag,
  const int flow_id,
  const int ctime,
  const int seq_num,
  const int hdr_type,
  const int state,
  const unsigned int aggregation_level,
  const int src_instance,
  const int dst_instance)
{

  unsigned char *tx_buffer=NULL;
  otg_hdr_info_t *otg_hdr_info_p=NULL;
  otg_hdr_t *otg_hdr_p=NULL;
  unsigned int  byte_tx_count=0;

  if (header == NULL || payload == NULL)
    return NULL;

  // allocate memory for the tx_buffer
  tx_buffer= (unsigned char*)malloc(buffer_size);
  // add otg control information first for decoding and computing the statistics
  otg_hdr_info_p = (otg_hdr_info_t *) (&tx_buffer[byte_tx_count]);
  otg_hdr_info_p->size = buffer_size;
  otg_hdr_info_p->flag = flag; // (background_ok==0)? 0xffff : 0xbbbb;
  byte_tx_count = sizeof(otg_hdr_info_t);
  otg_hdr_p = (otg_hdr_t *) (&tx_buffer[byte_tx_count]);
  otg_hdr_p->flow_id =flow_id;
  otg_hdr_p->time =ctime;
  otg_hdr_p->seq_num =seq_num;
  otg_hdr_p->hdr_type=hdr_type;
  otg_hdr_p->src_instance = src_instance;
  otg_hdr_p->dst_instance = dst_instance;
  otg_hdr_p->state = state;
  otg_hdr_p->aggregation_level=aggregation_level;
  otg_hdr_p->traffic_type=traffic_type;
  byte_tx_count += sizeof(otg_hdr_t);

  //LOG_I(OTG,"traffic type %d\n\n",otg_hdr_p->traffic_type);//EDITED
  // copy the header first
  memcpy(&tx_buffer[byte_tx_count], header, strlen(header));
  byte_tx_count += strlen(header);
  free(header);
  // now append the payload
  memcpy(&tx_buffer[byte_tx_count], payload, strlen(payload));
  byte_tx_count +=strlen(payload);
  free(payload);

  return tx_buffer;
}

void init_predef_multicast_traffic(void)
{
  int i, j, k;

  for (i=0; i<2; i++) { // src //maxServiceCount
    for (j=1; j<3; j++) { // dst // maxSessionPerPMCH
      for (k=0; k<1/*MAX_NUM_APPLICATION*/; k++) {
        switch(g_otg_multicast->application_type[i][j][k]) {
        case  MSCBR :
          LOG_D(OTG, "configure MSCBR for eMBMS (service %d, session %d, app %d)\n", i, j, k);
          g_otg_multicast->trans_proto[i][j][k]= UDP;
          g_otg_multicast->ip_v[i][j][k]= IPV4;

          g_otg_multicast->idt_dist[i][j][k]= FIXED;
          g_otg_multicast->idt_min[i][j][k]= 40;// can modify here to increase the frequency of generate data
          g_otg_multicast->idt_max[i][j][k]= 40;

          g_otg_multicast->size_dist[i][j][k]= FIXED;
          g_otg_multicast->size_min[i][j][k]= 256;
          g_otg_multicast->size_max[i][j][k]=  256;//can not be greater than 1500 which is max_ip_packet_size in pdcp.c


          g_otg_multicast->flow_duration[i][j][k] = 1000; // the packet will be generated after duration
          header_size_gen_multicast(i,j,k);
          break;

        case  MMCBR :
          LOG_D(OTG, "configure MSCBR for eMBMS (service %d, session %d, app %d)\n", i, j, k);
          g_otg_multicast->trans_proto[i][j][k]= UDP;
          g_otg_multicast->ip_v[i][j][k]= IPV4;

          g_otg_multicast->idt_dist[i][j][k]= FIXED;
          g_otg_multicast->idt_min[i][j][k]= 30;// can modify here to increase the frequency of generate data
          g_otg_multicast->idt_max[i][j][k]= 40;

          g_otg_multicast->size_dist[i][j][k]= FIXED;
          g_otg_multicast->size_min[i][j][k]= 768;
          g_otg_multicast->size_max[i][j][k]= 768 ;//can not be greater than 1500 which is max_ip_packet_size in pdcp.c


          g_otg_multicast->flow_duration[i][j][k] = 1000; // the packet will be generated after duration
          header_size_gen_multicast(i,j,k);
          break;

        case  MBCBR :
          LOG_D(OTG, "configure MSCBR for eMBMS (service %d, session %d, app %d)\n", i, j, k);
          g_otg_multicast->trans_proto[i][j][k]= UDP;
          g_otg_multicast->ip_v[i][j][k]= IPV4;

          g_otg_multicast->idt_dist[i][j][k]=FIXED;
          g_otg_multicast->idt_min[i][j][k]= 20;// can modify here to increase the frequency of generate data
          g_otg_multicast->idt_max[i][j][k]= 20;

          g_otg_multicast->size_dist[i][j][k]= FIXED;
          g_otg_multicast->size_min[i][j][k]= 1400;
          g_otg_multicast->size_max[i][j][k]= 1400 ;//can not be greater than 1500 which is max_ip_packet_size in pdcp.c


          g_otg_multicast->flow_duration[i][j][k] = 1000; // the packet will be generated after duration
          header_size_gen_multicast(i,j,k);
          break;

        case  MSVBR:
          LOG_D(OTG, "configure MSCBR for eMBMS (service %d, session %d, app %d)\n", i, j, k);
          g_otg_multicast->trans_proto[i][j][k]= UDP;
          g_otg_multicast->ip_v[i][j][k]= IPV4;

          g_otg_multicast->idt_dist[i][j][k]=UNIFORM;
          g_otg_multicast->idt_min[i][j][k]= 20;// can modify here to increase the frequency of generate data
          g_otg_multicast->idt_max[i][j][k]= 40;

          g_otg_multicast->size_dist[i][j][k]= UNIFORM;
          g_otg_multicast->size_min[i][j][k]= 64;
          g_otg_multicast->size_max[i][j][k]= 512 ;//can not be greater than 1500 which is max_ip_packet_size in pdcp.c


          g_otg_multicast->flow_duration[i][j][k] = 1000; // the packet will be generated after duration
          header_size_gen_multicast(i,j,k);
          break;

        case  MMVBR:
          LOG_D(OTG, "configure MSCBR for eMBMS (service %d, session %d, app %d)\n", i, j, k);
          g_otg_multicast->trans_proto[i][j][k]= UDP;
          g_otg_multicast->ip_v[i][j][k]= IPV4;

          g_otg_multicast->idt_dist[i][j][k]=UNIFORM;
          g_otg_multicast->idt_min[i][j][k]= 15;// can modify here to increase the frequency of generate data
          g_otg_multicast->idt_max[i][j][k]= 30;

          g_otg_multicast->size_dist[i][j][k]= UNIFORM;
          g_otg_multicast->size_min[i][j][k]= 512;
          g_otg_multicast->size_max[i][j][k]= 1024 ;//can not be greater than 1500 which is max_ip_packet_size in pdcp.c


          g_otg_multicast->flow_duration[i][j][k] = 1000; // the packet will be generated after duration
          header_size_gen_multicast(i,j,k);
          break;

        case  MBVBR:
          LOG_D(OTG, "configure MSCBR for eMBMS (service %d, session %d, app %d)\n", i, j, k);
          g_otg_multicast->trans_proto[i][j][k]= UDP;
          g_otg_multicast->ip_v[i][j][k]= IPV4;

          g_otg_multicast->idt_dist[i][j][k]=UNIFORM;
          g_otg_multicast->idt_min[i][j][k]= 5;// can modify here to increase the frequency of generate data
          g_otg_multicast->idt_max[i][j][k]= 15;

          g_otg_multicast->size_dist[i][j][k]= UNIFORM;
          g_otg_multicast->size_min[i][j][k]= 1024;
          g_otg_multicast->size_max[i][j][k]= 1400 ;//can not be greater than 1500 which is max_ip_packet_size in pdcp.c


          g_otg_multicast->flow_duration[i][j][k] = 1000; // the packet will be generated after duration
          header_size_gen_multicast(i,j,k);
          break;

        case MVIDEO_VBR_4MBPS :

          LOG_D(OTG,"Configure MSCBR for eMBMS (service %d, session %d, app %d)\n", i, j, k);
          g_otg_multicast->trans_proto[i][j][k]= UDP;
          g_otg_multicast->ip_v[i][j][k]= IPV4;

          g_otg_multicast->idt_dist[i][j][k]= FIXED;
          g_otg_multicast->idt_min[i][j][k]= 40;// can modify here to increase the frequency of generate data
          g_otg_multicast->idt_max[i][j][k]= 40;

          /*the tarma initialization*/
          otg_multicast_info->mtarma_video[i][j][k]=tarmaInitVideo(0);
          tarmaSetupVideoGop12(otg_multicast_info->mtarma_video[i][j][k],2.5);
          LOG_I(OTG,"[CONFIG] Multicast Video VBR 4MBPS, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg_multicast->idt_dist[i][j][k]);

          //g_otg_multicast->duration[i][j][k] = 1000; // the packet will be generated after duration
          header_size_gen_multicast(i,j,k);
          break;

        default :
          LOG_W(OTG, "not supported model for multicast traffic\n");

        }

      }
    }
  }
}



void init_predef_traffic(
  const unsigned char nb_ue_local,
  const unsigned char nb_enb_local)
{
  int i,j, k;

 // LOG_I(OTG,"OTG_CONFIG num_node %d\n",  g_otg->num_nodes);
  //LOG_I(OTG,"MAX_NODES [MAX UE= %d] [MAX eNB= %d] \n",nb_ue_local,  nb_enb_local);

  for (i=0; i<g_otg->num_nodes; i++) { // src
    for (j=0; j<g_otg->num_nodes; j++) { // dst
      for (k=0; k<g_otg->application_idx[i][j]; k++) {

        switch  (g_otg->application_type[i][j][k]) {
        case NO_PREDEFINED_TRAFFIC :
          LOG_I(OTG, "[SRC %d][DST %d] No predefined Traffic \n", i, j);
          g_otg->trans_proto[i][j][k] = 0;
          g_otg->ip_v[i][j][k] = 0;
          g_otg->idt_dist[i][j][k][PE_STATE] = 0;
          g_otg->idt_min[i][j][k][PE_STATE] =  0;
          g_otg->idt_max[i][j][k][PE_STATE] =  0;
          g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->size_min[i][j][k][PE_STATE] =  0;
          g_otg->size_max[i][j][k][PE_STATE] = 0;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case  SCBR :
          g_otg->trans_proto[i][j][k] = UDP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =   20;//(int)round(uniform_dist((i+1)*30,(i+1)*100));// 500+(i+1)*10; //random idt among different UEs
          g_otg->idt_max[i][j][k][PE_STATE] =  20;
          g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->size_min[i][j][k][PE_STATE] =  128;
          g_otg->size_max[i][j][k][PE_STATE] =  128;
          LOG_I(OTG,"OTG_CONFIG SCBR, src = %d, dst = %d, traffic id %d, idt %d dist type for size = %d\n", i, j, k,
                g_otg->idt_min[i][j][k][PE_STATE], g_otg->size_min[i][j][k][PE_STATE]);
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case MCBR :
          g_otg->trans_proto[i][j][k] = UDP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =   20;//(int)round(uniform_dist((i+1)*30, (i+1)*100));// 250+(i+1)*10;
          g_otg->idt_max[i][j][k][PE_STATE] =  20;
          g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->size_min[i][j][k][PE_STATE] =  768;
          g_otg->size_max[i][j][k][PE_STATE] =  768;
          LOG_I(OTG,"OTG_CONFIG MCBR, src = %d, dst = %d,  traffic id %d, dist type for size = %d\n", i, j,k , g_otg->size_dist[i][j][k][PE_STATE]);
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case BCBR :
          g_otg->trans_proto[i][j][k] = UDP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;// main param in this mode
          g_otg->idt_min[i][j][k][PE_STATE] =  20;// (int)round(uniform_dist((i+1)*30,(i+1)*100)); //125+(i+1)*10;
          g_otg->idt_max[i][j][k][PE_STATE] =  20;
          g_otg->size_dist[i][j][k][PE_STATE] = FIXED; // main param in this mode
          g_otg->size_min[i][j][k][PE_STATE] =  1400;// main param in this mode
          g_otg->size_max[i][j][k][PE_STATE] =  1400;
          LOG_I(OTG,"OTG_CONFIG BCBR, src = %d, dst = %d, dist type for size = %d\n", i, j, g_otg->size_dist[i][j][k][PE_STATE]);
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case AUTO_PILOT :
          g_otg->trans_proto[i][j][k] = 2;
          g_otg->ip_v[i][j][k] = 1;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          LOG_I(OTG,"OTG_CONFIG AUTO PILOT, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          g_otg->idt_min[i][j][k][PE_STATE] =  100;
          g_otg->idt_max[i][j][k][PE_STATE] =  500;
          g_otg->idt_std_dev[i][j][k][PE_STATE] = 0;
          g_otg->idt_lambda[i][j][k][PE_STATE] = 0;
          g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->size_min[i][j][k][PE_STATE] =  800;
          g_otg->size_max[i][j][k][PE_STATE] =  800;
          g_otg->size_std_dev[i][j][k][PE_STATE] = 0;
          g_otg->size_lambda[i][j][k][PE_STATE] = 0;
#ifdef STANDALONE 
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case BICYCLE_RACE :
          g_otg->trans_proto[i][j][k] = 2;
          g_otg->ip_v[i][j][k] = 1;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          LOG_I(OTG,"OTG_CONFIG BICYCLE RACE, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          g_otg->idt_min[i][j][k][PE_STATE] =  100;
          g_otg->idt_max[i][j][k][PE_STATE] =  500;
          g_otg->idt_std_dev[i][j][k][PE_STATE] = 0;
          g_otg->idt_lambda[i][j][k][PE_STATE] = 0;
          g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->size_min[i][j][k][PE_STATE] =  800;
          g_otg->size_max[i][j][k][PE_STATE] =  800;
          g_otg->size_std_dev[i][j][k][PE_STATE] = 0;
          g_otg->size_lambda[i][j][k][PE_STATE] = 0;

#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

          /* case OPENARENA :
          g_otg->trans_proto[i][j][k] = 2;
          g_otg->ip_v[i][j][k] = 1;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          LOG_I(OTG,"OTG_CONFIG OPENARENA, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          g_otg->idt_min[i][j][k][PE_STATE] =  69;
          g_otg->idt_max[i][j][k][PE_STATE] =  103;
          g_otg->idt_std_dev[i][j][k][PE_STATE] = 0;
          g_otg->idt_lambda[i][j][k][PE_STATE] = 0;
          g_otg->size_dist[i][j][k][PE_STATE] = GAUSSIAN;
          g_otg->size_min[i][j][k][PE_STATE] =  5;
          g_otg->size_max[i][j][k][PE_STATE] =  43;
          g_otg->size_std_dev[i][j][k][PE_STATE] = 5;
          g_otg->size_lambda[i][j][k][PE_STATE] = 0;

          #ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
          #endif
          break;  */
        case TEAM_FORTRESS :
          g_otg->trans_proto[i][j][k] = 2;
          g_otg->ip_v[i][j][k] = 1;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          LOG_I(OTG,"OTG_CONFIG GAMING_TF, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          g_otg->idt_min[i][j][k][PE_STATE] =  31;
          g_otg->idt_max[i][j][k][PE_STATE] =  42;
          g_otg->idt_std_dev[i][j][k][PE_STATE] = 0;
          g_otg->idt_lambda[i][j][k][PE_STATE] = 0;
          g_otg->size_dist[i][j][k][PE_STATE] = GAUSSIAN;
          g_otg->size_min[i][j][k][PE_STATE] =  5;
          g_otg->size_max[i][j][k][PE_STATE] =  43;
          g_otg->size_std_dev[i][j][k][PE_STATE] = 5;
          g_otg->size_lambda[i][j][k][PE_STATE] = 0;

#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case M2M_TRAFFIC : /* example of M2M traffic  */
          LOG_I(OTG," M2M_TRAFFIC, src = %d, dst = %d \n", i, j, g_otg->application_type[i][j][k]);
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=50;
          g_otg->ed_size_pkts[i][j][k]=60;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          g_otg->idt_min[i][j][k][PE_STATE] =  1000;
          g_otg->idt_max[i][j][k][PE_STATE] =  15;
          g_otg->size_dist[i][j][k][PE_STATE] = UNIFORM;
          g_otg->size_min[i][j][k][PE_STATE] =  200;
          g_otg->size_max[i][j][k][PE_STATE] =  800;
          g_otg->prob_off_pu[i][j][k]=0.2;
          g_otg->prob_off_ed[i][j][k]=0.3;
          g_otg->prob_off_pe[i][j][k]=0.4;
          g_otg->prob_pu_ed[i][j][k]=0.1;
          g_otg->prob_pu_pe[i][j][k]=0.6;
          g_otg->prob_ed_pu[i][j][k]=0.3;
          g_otg->prob_ed_pe[i][j][k]=0.5;
          g_otg->holding_time_off_pu[i][j][k]=10;
          g_otg->holding_time_off_ed[i][j][k]=12;
          g_otg->holding_time_off_pe[i][j][k]=13;
          g_otg->holding_time_pe_off[i][j][k]=30;
          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif

        case AUTO_PILOT_L :
          /* Measurements from:
          Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */

          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->ed_size_pkts[i][j][k]=1000;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          g_otg->idt_min[i][j][k][PE_STATE] =  100;
          g_otg->idt_max[i][j][k][PE_STATE] =  500;

          if (i<nb_enb_local) { /*DL*/
            g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
            g_otg->size_min[i][j][k][PE_STATE] =  1000;
            g_otg->size_max[i][j][k][PE_STATE] =  1000;
            LOG_I(OTG,"AUTO PILOT LOW SPEEDS DL , src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          } else { /*UL*/
            g_otg->size_dist[j][i][k][PE_STATE] = UNIFORM;
            g_otg->size_min[j][i][k][PE_STATE] =  64;
            g_otg->size_max[j][i][k][PE_STATE] =  1000;
            LOG_I(OTG,"AUTO PILOT LOW SPEEDS UL, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          }

          g_otg->prob_off_pu[i][j][k]=0.2;
          g_otg->prob_off_ed[i][j][k]=0.3;
          g_otg->prob_off_pe[i][j][k]=0;
          g_otg->prob_pu_ed[i][j][k]=0.5;
          g_otg->prob_pu_pe[i][j][k]=0;
          g_otg->prob_ed_pu[i][j][k]=0;
          g_otg->prob_ed_pe[i][j][k]=1;
          g_otg->holding_time_off_pu[i][j][k]=100;
          g_otg->holding_time_off_ed[i][j][k]=10;
          g_otg->holding_time_pe_off[i][j][k]=1000;
          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j]= 303;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case AUTO_PILOT_M :
          /* Measurements from:
          Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */

          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->ed_size_pkts[i][j][k]=1000;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =  100;
          g_otg->idt_max[i][j][k][PE_STATE] =  100;

          if (i<nb_enb_local) { /*DL*/
            g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
            g_otg->size_min[i][j][k][PE_STATE] =  1000;
            g_otg->size_max[i][j][k][PE_STATE] =  1000;
            LOG_I(OTG,"AUTO PILOT MEDIUM SPEEDS DL, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          } else { /*UL*/
            g_otg->size_dist[j][i][k][PE_STATE] = UNIFORM;
            g_otg->size_min[j][i][k][PE_STATE] =  64;
            g_otg->size_max[j][i][k][PE_STATE] =  1000;
            LOG_I(OTG,"AUTO PILOT MEDIUM SPEEDS UL, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          }

          g_otg->prob_off_pu[i][j][k]=0.2;
          g_otg->prob_off_ed[i][j][k]=0.3;
          g_otg->prob_off_pe[i][j][k]=0;
          g_otg->prob_pu_ed[i][j][k]=0.5;
          g_otg->prob_pu_pe[i][j][k]=0;
          g_otg->prob_ed_pu[i][j][k]=0;
          g_otg->prob_ed_pe[i][j][k]=1;
          g_otg->holding_time_off_pu[i][j][k]=40;
          g_otg->holding_time_off_ed[i][j][k]=10;
          g_otg->holding_time_pe_off[i][j][k]=1000;
          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 303;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case AUTO_PILOT_H :
          /* Measurements from:
          Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */

          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->ed_size_pkts[i][j][k]=1000;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          g_otg->idt_min[i][j][k][PE_STATE] =  20;
          g_otg->idt_max[i][j][k][PE_STATE] =  25;

          if (i<nb_enb_local) { /*DL*/
            g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
            g_otg->size_min[i][j][k][PE_STATE] =  1000;
            g_otg->size_max[i][j][k][PE_STATE] =  1000;
            LOG_I(OTG,"AUTO PILOT HIGH SPEEDS DL, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          } else { /*UL*/
            g_otg->size_dist[j][i][k][PE_STATE] = UNIFORM;
            g_otg->size_min[j][i][k][PE_STATE] =  64;
            g_otg->size_max[j][i][k][PE_STATE] =  1000;
            LOG_I(OTG,"AUTO PILOT HIGH SPEEDS UL, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          }

          g_otg->prob_off_pu[i][j][k]=0.2;
          g_otg->prob_off_ed[i][j][k]=0.3;
          g_otg->prob_off_pe[i][j][k]=0;
          g_otg->prob_pu_ed[i][j][k]=0.5;
          g_otg->prob_pu_pe[i][j][k]=0;
          g_otg->prob_ed_pu[i][j][k]=0;
          g_otg->prob_ed_pe[i][j][k]=1;
          g_otg->holding_time_off_pu[i][j][k]=20;
          g_otg->holding_time_off_ed[i][j][k]=10;
          g_otg->holding_time_pe_off[i][j][k]=1000;
          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 303;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case AUTO_PILOT_E :
          /* Measurements from:
          Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */

          /* DL SCENARIO*/
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->ed_size_pkts[i][j][k]=1000;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          g_otg->idt_min[i][j][k][PE_STATE] =  20;
          g_otg->idt_max[i][j][k][PE_STATE] =  25;

          if (i<nb_enb_local) { /*DL*/
            g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
            g_otg->size_min[i][j][k][PE_STATE] =  1000;
            g_otg->size_max[i][j][k][PE_STATE] =  1000;
            LOG_I(OTG,"AUTO PILOT EMERGENCY DL, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          } else { /*UL*/
            g_otg->size_dist[j][i][k][PE_STATE] = UNIFORM;
            g_otg->size_min[j][i][k][PE_STATE] =  64;
            g_otg->size_max[j][i][k][PE_STATE] =  1000;
            LOG_I(OTG,"AUTO PILOT EMERGENCY UL, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          }

          g_otg->prob_off_pu[i][j][k]=0.2;
          g_otg->prob_off_ed[i][j][k]=0.3;
          g_otg->prob_off_pe[i][j][k]=0;
          g_otg->prob_pu_ed[i][j][k]=0.5;
          g_otg->prob_pu_pe[i][j][k]=0;
          g_otg->prob_ed_pu[i][j][k]=0;
          g_otg->prob_ed_pe[i][j][k]=1;
          g_otg->holding_time_off_pu[i][j][k]=10;
          g_otg->holding_time_off_ed[i][j][k]=10;
          g_otg->holding_time_pe_off[i][j][k]=1000;
          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 303;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case VIRTUAL_GAME_L :
          /* Measurements from:
          Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */
          LOG_I(OTG,"VIRTUAL GAME LOW SPEEDS, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->prob_off_pu[i][j][k]=1;

          if (i<nb_enb_local)/*DL*/
            g_otg->holding_time_off_pu[i][j][k]=1000;
          else /*UL*/
            g_otg->holding_time_off_pu[j][i][k]=500;

          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j][k] = 302;
          g_otg->flow_duration[i][j][k] = 1000;
#endif
          break;

        case VIRTUAL_GAME_M :
          /* Measurements from:
          Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */
          LOG_I(OTG,"VIRTUAL GAME MEDIUM SPEEDS, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          /* DL SCENARIO*/
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->prob_off_pu[i][j][k]=1;

          if (i<nb_enb_local)/*DL*/
            g_otg->holding_time_off_pu[i][j][k]=1000;
          else /*UL*/
            g_otg->holding_time_off_pu[j][i][k]=150;

          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case VIRTUAL_GAME_H :
          /* Measurements from:
          Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */
          LOG_I(OTG,"VIRTUAL GAME HIGH SPEEDS, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;

          if (i<nb_enb_local)/*DL*/
            g_otg->holding_time_off_pu[i][j][k]=1000;
          else /*UL*/
            g_otg->holding_time_off_pu[j][i][k]=100;

          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case VIRTUAL_GAME_F :
          /* Measurements from:
          Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */
          LOG_I(OTG,"VIRTUAL GAME FINISH, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          /* DL SCENARIO*/
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->prob_off_pu[i][j][k]=1;

          if (i<nb_enb_local)/*DL*/
            g_otg->holding_time_off_pu[i][j][k]=1000;
          else /*UL*/
            g_otg->holding_time_off_pu[j][i][k]=70;

          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case ALARM_HUMIDITY :
          /* Measurements from:
           *  Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */
          LOG_I(OTG,"ALARM HUMIDITY, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->ed_size_pkts[i][j][k]=2000;
          g_otg->prob_off_pu[i][j][k]=0.5;
          g_otg->prob_off_ed[i][j][k]=0.5;
          g_otg->prob_pu_ed[i][j][k]=0.5;
          g_otg->holding_time_off_pu[i][j][k]=1680000;    /* 28 minutes*/
          g_otg->holding_time_off_ed[i][j][k]=32400000;   /* 9 hours*/
          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 303;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case ALARM_SMOKE :
          /* Measurements from:
          Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */
          LOG_I(OTG,"ALARM SMOKE, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->ed_size_pkts[i][j][k]=2000;
          g_otg->prob_off_pu[i][j][k]=0.5;
          g_otg->prob_off_ed[i][j][k]=0.5;
          g_otg->prob_pu_ed[i][j][k]=0.5;
          g_otg->holding_time_off_pu[i][j][k]=60000;    /* 1 minute*/
          g_otg->holding_time_off_ed[i][j][k]=43200000;   /* 12 hours*/
          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j] = 303;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case ALARM_TEMPERATURE :
          /* Measurements from:
           *  Traffic Modeling Framework for Machine Type Communincation (Navid NiKaein, Markus Laner, Kajie Zhou, Philippe Svoboda, Dejan Drajic, Serjan Krco and Milica Popovic)
           */
          LOG_I(OTG,"ALARM TEMPERATURE, src = %d, dst = %d, application type = %d\n", i, j, g_otg->application_type[i][j][k]);
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->pu_size_pkts[i][j][k]=1000;
          g_otg->ed_size_pkts[i][j][k]=4000;
          g_otg->prob_off_pu[i][j][k]=0.5;
          g_otg->prob_off_ed[i][j][k]=0.5;
          g_otg->prob_pu_ed[i][j][k]=0.5;
          g_otg->holding_time_off_pu[i][j][k]=1680000;    /* 28 minute*/
          g_otg->holding_time_off_ed[i][j][k]=18000000;   /* 5 hours*/
          g_otg->m2m[i][j][k]=1;
#ifdef STANDALONE
          g_otg->dst_port[i][j]= 303;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case OPENARENA_DL :
        case OPENARENA_UL :
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->size_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->m2m[i][j][k]=1;

          if (i<nb_enb_local) { /*DL*/
            LOG_I(OTG,"OTG_CONFIG GAMING_OA_DL, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
            g_otg->idt_min[i][j][k][PE_STATE] =  40;
            g_otg->idt_max[i][j][k][PE_STATE] =  40;
            g_otg->size_min[i][j][k][PE_STATE] =  140;
            g_otg->size_max[i][j][k][PE_STATE] =  140;
          } else { /*UL*/
            LOG_I(OTG,"OTG_CONFIG GAMING_OA_UL, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
            g_otg->idt_min[i][j][k][PE_STATE] =  11;
            g_otg->idt_max[i][j][k][PE_STATE] =  11;
            g_otg->size_min[i][j][k][PE_STATE] =  42;
            g_otg->size_max[i][j][k][PE_STATE] =  42;
          }

#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case OPENARENA_DL_TARMA :
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->m2m[i][j][k]=1;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =  40;
          g_otg->idt_max[i][j][k][PE_STATE] =  40;
          g_otg->size_dist[i][j][k][PE_STATE] = TARMA;
          /*the tarma initialization*/
          otg_info->tarma_stream[i][j][k]=tarmaInitStream (0);
          tarmaSetupOpenarenaDownlink(otg_info->tarma_stream[i][j][k]);
          LOG_I(OTG,"OTG_CONFIG OPENARENA_DL_TARMA, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          break;


        case VIDEO_VBR_10MBPS :
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =  40;
          g_otg->idt_max[i][j][k][PE_STATE] =  40;
          g_otg->size_dist[i][j][k][PE_STATE] = VIDEO;
          /*the tarma initialization*/
          otg_info->tarma_video[i][j][k]=tarmaInitVideo(0);
          tarmaSetupVideoGop12(otg_info->tarma_video[i][j][k],1);
          LOG_I(OTG,"OTG_CONFIG OPENARENA_DL_TARMA, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          break;

        case VIDEO_VBR_4MBPS :
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =  40;
          g_otg->idt_max[i][j][k][PE_STATE] =  40;
          g_otg->size_dist[i][j][k][PE_STATE] = VIDEO;
          /*the tarma initialization*/
          otg_info->tarma_video[i][j][k]=tarmaInitVideo(0);
          tarmaSetupVideoGop12(otg_info->tarma_video[i][j][k],2.5);
          LOG_I(OTG,"OTG_CONFIG OPENARENA_DL_TARMA, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          break;

        case VIDEO_VBR_2MBPS :
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =  40;
          g_otg->idt_max[i][j][k][PE_STATE] =  40;
          g_otg->size_dist[i][j][k][PE_STATE] = VIDEO;
          /*the tarma initialization*/
          otg_info->tarma_video[i][j][k]=tarmaInitVideo(0);
          tarmaSetupVideoGop12(otg_info->tarma_video[i][j][k],5);
          LOG_I(OTG,"OTG_CONFIG OPENARENA_DL_TARMA, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          break;

        case VIDEO_VBR_768KBPS :
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =  40;
          g_otg->idt_max[i][j][k][PE_STATE] =  40;
          g_otg->size_dist[i][j][k][PE_STATE] = VIDEO;
          /*the tarma initialization*/
          otg_info->tarma_video[i][j][k]=tarmaInitVideo(0);
          tarmaSetupVideoGop12(otg_info->tarma_video[i][j][k],13);
          LOG_I(OTG,"OTG_CONFIG OPENARENA_DL_TARMA, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          break;

        case VIDEO_VBR_384KBPS :
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =  40;
          g_otg->idt_max[i][j][k][PE_STATE] =  40;
          g_otg->size_dist[i][j][k][PE_STATE] = VIDEO;
          /*the tarma initialization*/
          otg_info->tarma_video[i][j][k]=tarmaInitVideo(0);
          tarmaSetupVideoGop12(otg_info->tarma_video[i][j][k],26);
          LOG_I(OTG,"OTG_CONFIG OPENARENA_DL_TARMA, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          break;

        case VIDEO_VBR_192KBPS :
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = FIXED;
          g_otg->idt_min[i][j][k][PE_STATE] =  40;
          g_otg->idt_max[i][j][k][PE_STATE] =  40;
          g_otg->size_dist[i][j][k][PE_STATE] = VIDEO;
          /*the tarma initialization*/
          otg_info->tarma_video[i][j][k]=tarmaInitVideo(0);
          tarmaSetupVideoGop12(otg_info->tarma_video[i][j][k],52);
          LOG_I(OTG,"OTG_CONFIG OPENARENA_DL_TARMA, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          break;

        case BACKGROUND_USERS:
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          g_otg->idt_lambda[i][j][k][PE_STATE] = 1.0/40.0;
          g_otg->idt_min[i][j][k][PE_STATE] =  40;
          g_otg->idt_max[i][j][k][PE_STATE] =  80;
          g_otg->size_dist[i][j][k][PE_STATE] = BACKGROUND_DIST;
          /*the background initialization*/
          //  otg_info->background_stream[i][j][k]=backgroundStreamInit(0,2);
          break;

        case DUMMY :
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          /*the tarma initialization*/
          otg_info->tarma_video[i][j][k]=tarmaInitVideo(0);
          g_otg->idt_dist[i][j][k][PE_STATE] = VIDEO;
          g_otg->idt_min[i][j][k][PE_STATE] =  40;
          g_otg->idt_max[i][j][k][PE_STATE] =  40;
          g_otg->size_dist[i][j][k][PE_STATE] = VIDEO;
          otg_info->tarma_video[i][j][k]->tarma_size.inputWeight[0]=1;
          otg_info->tarma_video[i][j][k]->tarma_size.maWeight[0]=0.6;
          otg_info->tarma_video[i][j][k]->tarma_size.maWeight[1]=-1.04;
          otg_info->tarma_video[i][j][k]->tarma_size.maWeight[2]=0.44;
          otg_info->tarma_video[i][j][k]->tarma_size.arWeight[0]=1;
          otg_info->tarma_video[i][j][k]->tarma_size.arWeight[1]=-1.971;
          otg_info->tarma_video[i][j][k]->tarma_size.arWeight[2]=0.971;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[0]=0;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[1]=2;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[2]=2;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[3]=1;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[4]=2;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[5]=2;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[6]=1;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[7]=2;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[8]=2;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[9]=1;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[10]=2;
          otg_info->tarma_video[i][j][k]->tarmaVideoGopStructure[11]=2;
          otg_info->tarma_video[i][j][k]->polyWeightFrame[0][0]=300;
          otg_info->tarma_video[i][j][k]->polyWeightFrame[0][1]=30;
          otg_info->tarma_video[i][j][k]->polyWeightFrame[1][0]=200;
          otg_info->tarma_video[i][j][k]->polyWeightFrame[1][1]=20;
          otg_info->tarma_video[i][j][k]->polyWeightFrame[2][0]=100;
          otg_info->tarma_video[i][j][k]->polyWeightFrame[2][1]=10;
          tarmaPrintVideoInit(otg_info->tarma_video[i][j][k]);
          LOG_I(OTG,"OTG_CONFIG OPENARENA_DL_TARMA, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
          break;

        case VOIP_G711 :  /*http://www.computerweekly.com/feature/VoIP-bandwidth-fundamentals */
          /* Voice bit rate= 64 Kbps |  Sample time= 20 msec |  Voice payload=160 bytes */
          g_otg->trans_proto[i][j][k] = UDP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][SIMPLE_TALK] = FIXED;
          LOG_I(OTG,"OTG_CONFIG VOIP G711, src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][SIMPLE_TALK]);
          g_otg->idt_min[i][j][k][SIMPLE_TALK] =  20;
          g_otg->idt_max[i][j][k][SIMPLE_TALK] =  20;
          g_otg->size_dist[i][j][k][SIMPLE_TALK] = FIXED;
          g_otg->size_min[i][j][k][SIMPLE_TALK] =  160;
          g_otg->size_max[i][j][k][SIMPLE_TALK] =  160;

#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case VOIP_G729 :  /*http://www.computerweekly.com/feature/VoIP-bandwidth-fundamentals */
          /* Voice bit rate= 8 Kbps | Sample time= 30 msec |  Voice payload=30 bytes */
          g_otg->trans_proto[i][j][k] = UDP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][SIMPLE_TALK] = FIXED;
          LOG_I(OTG,"OTG_CONFIG  VOIP G729, src = %d, dst = %d, traffic id %d, dist IDT = %d\n", i, j, k, g_otg->idt_dist[i][j][k][SIMPLE_TALK]);
          g_otg->idt_min[i][j][k][SIMPLE_TALK] =  30;
          g_otg->idt_max[i][j][k][SIMPLE_TALK] =  30;
          g_otg->size_dist[i][j][k][SIMPLE_TALK] = FIXED;
          g_otg->size_min[i][j][k][SIMPLE_TALK] =  30;
          g_otg->size_max[i][j][k][SIMPLE_TALK] =  30;

#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif
          break;

        case IQSIM_MANGO : /*Realistic measurements: Eurecom with iQsim Gateway*/
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          g_otg->idt_min[i][j][k][PE_STATE] =  5000;
          g_otg->idt_max[i][j][k][PE_STATE] = 10000;
          g_otg->size_dist[i][j][k][PE_STATE] = FIXED;

          if (i<nb_enb_local) {
            LOG_I(OTG,"IQSIM_MANGO [DL], src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
            g_otg->size_min[i][j][k][PE_STATE] =  141;
            g_otg->size_max[i][j][k][PE_STATE] =  141;
          } else {
            LOG_I(OTG,"IQSIM_MANGO [UL], src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
            g_otg->size_min[i][j][k][PE_STATE] =  144;
            g_otg->size_max[i][j][k][PE_STATE] =  144;
          }

          g_otg->m2m[i][j][k]=1;

#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif

          break;

        case IQSIM_NEWSTEO :  /*Realistic measurements: Eurecom with iQsim Gateway*/
          g_otg->trans_proto[i][j][k] = TCP;
          g_otg->ip_v[i][j][k] = IPV4;
          g_otg->idt_dist[i][j][k][PE_STATE] = UNIFORM;
          g_otg->idt_min[i][j][k][PE_STATE] =  5000;
          g_otg->idt_max[i][j][k][PE_STATE] = 10000;
          g_otg->size_dist[i][j][k][PE_STATE] = FIXED;

          if (i<nb_enb_local) { /*DL*/
            LOG_I(OTG,"IQSIM_NEWSTEO [DL], src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
            g_otg->size_min[i][j][k][PE_STATE] =  467;
            g_otg->size_max[i][j][k][PE_STATE] =  467;
          } else { /*UL*/
            LOG_I(OTG,"IQSIM_NEWSTEO [UL], src = %d, dst = %d, dist IDT = %d\n", i, j, g_otg->idt_dist[i][j][k][PE_STATE]);
            g_otg->size_min[i][j][k][PE_STATE] =  251;
            g_otg->size_max[i][j][k][PE_STATE] =  251;
          }

          g_otg->m2m[i][j][k]=1;

#ifdef STANDALONE
          g_otg->dst_port[i][j] = 302;
          g_otg->flow_duration[i][j] = 1000;
#endif

          break;
	  
	case FULL_BUFFER:
	  LOG_E(OTG, "Full buffer is not yet supported\n");
	default:
          LOG_D(OTG, "[SRC %d][DST %d] Unknown traffic type\n", i, j);
          /* TO ADD: 3GPP TSG-RAN WG1 Meeting #71: Low-cost MTC Traffic Models and Characteristics */

	  
        }
      }
    }
  }
}



int background_gen(
  const int src_instance,
  const int dst_instance,
  const int ctime)
{

  int ptime_background;

  if (src_instance<NB_eNB_INST) // DL case
    ptime_background=otg_info->ptime_background_dl;
  else  //UL case
    ptime_background=otg_info->ptime_background_ul;


  /*check if it is time to transmit the background traffic
  - we have different distributions for packet size and idt for the UL and DL */
  if ((((ctime-ptime_background) >=  otg_info->idt_background[src_instance][dst_instance])) ||
      (otg_info->idt_background[src_instance][dst_instance]==0)) {

    LOG_D(OTG,"[SRC %d][DST %d] BACKGROUND :: OK (idt=%d, ctime=%d,ptime=%d ) !!\n",
          src_instance,
          dst_instance,
          otg_info->idt_background[src_instance][dst_instance],
          ctime, ptime_background);
    otg_info->size_background[src_instance][dst_instance]=adjust_size(
          ceil(backgroundCalculateSize(otg_info->background_stream[src_instance][dst_instance][0],
                                       ctime,
                                       otg_info->idt_background[src_instance][dst_instance])));

    if (src_instance<NB_eNB_INST) { // DL case
      otg_info->idt_background[src_instance][dst_instance]= exponential_dist(0.025);
      otg_info->ptime_background_dl=ctime;
    } else { //UL case
      otg_info->idt_background[src_instance][dst_instance]= uniform_dist(500,1000);
      otg_info->ptime_background_ul=ctime;
    }

    /*
    // Distinguish between the UL and DL case
    if (src_instance<NB_eNB_INST) // DL case
    otg_info->size_background[src_instance][dst_instance]=ceil(lognormal_dist(5.46,0.85)); //lognormal distribution for DL background packet
    else //UL case
    otg_info->size_background[src_instance][dst_instance]=ceil(lognormal_dist(3.03,0.5)); //lognormal distribution for DL background packet

    // adjust the packet size if needed
    if (otg_info->size_background[src_instance][dst_instance]>1400)
    otg_info->size_background[src_instance][dst_instance]=1400;
    if (otg_info->size_background[src_instance][dst_instance]<=10)
    otg_info->size_background[src_instance][dst_instance]=10;


    // Compute the corresponding IDT

    // Eq. (7) from "Users in Cells: a Data Traffic Analysis (Markus Laner, Philipp Svoboda, Stefan Schwarz, and Markus Rupp)" - Measured traffic consists of a mixture of many different types (e.g., web, video streaming, file download), gives an intuition for the encountered heavy-tails of. Most sessions are short with low data-volume, consisting of small downloads (e.g., e-mail, TCP-acknowledges), whereas some few sessions last very long and require high throughput (e.g., VoIP, video streaming, file-download).

    otg_info->idt_background[src_instance][dst_instance]=ceil(((otg_info->size_background[src_instance][dst_instance])*8000)/pow(10, lognormal_dist(1.3525, 0.1954)));
    //if(otg_info->idt_background[src_instance][dst_instance]==0)
    //otg_info->idt_background[src_instance][dst_instance]=10;
    otg_info->ptime_background=ctime;
     */
    LOG_D(OTG,"[BACKGROUND] TRAFFIC:: (src=%d, dst=%d) pkts size=%d idt=%d  \n",
          src_instance,
          dst_instance,
          otg_info->size_background[src_instance][dst_instance],
          otg_info->idt_background[src_instance][dst_instance]);

    return 1;
  } else {
    LOG_D(OTG,"[SRC %d][DST %d] [BACKGROUND] TRAFFIC:: not the time to transmit= (idt=%d, ctime=%d,ptime=%d ) size= %d \n",
          src_instance,
          dst_instance,
          otg_info->idt_background[src_instance][dst_instance],
          ctime, ptime_background,
          otg_info->size_background[src_instance][dst_instance]);
    return 0;
  }

}



int header_size_gen_background(
  const int src_instance,
  const int dst_instance)
{
  int size_header=0;

  if (otg_info->header_size_background[src_instance][dst_instance]==0) {

    if(g_otg->trans_proto_background[src_instance][dst_instance]==0)
      g_otg->trans_proto_background[src_instance][dst_instance]= rand() % (TCP_IPV6 - UDP_IPV4 + 1) + UDP_IPV4;

    switch (g_otg->trans_proto_background[src_instance][dst_instance]) {
    case  UDP_IPV4:
      size_header=HDR_IP_v4_MIN + HDR_UDP;
      break;

    case  UDP_IPV6:
      size_header=HDR_IP_v6 + HDR_UDP;
      break;

    case  TCP_IPV4:
      size_header=HDR_IP_v4_MIN + HDR_TCP;
      break;

    case  TCP_IPV6:
      size_header=HDR_IP_v6 + HDR_TCP;
      break;

    default :
      size_header=HDR_IP_v6 + HDR_TCP;
      break;
    }

    otg_info->header_size_background[src_instance][dst_instance]=size_header;
  }

  LOG_D(OTG," [SRC %d][DST %d]  BACKGROUND TRAFFIC:: size header %d \n", src_instance, dst_instance, otg_info->header_size_background[src_instance][dst_instance]);
  return otg_info->header_size_background[src_instance][dst_instance];
}



void state_management(
  const int src_instance,
  const int dst_instance,
  const int application,
  const int ctime)
{

  if ((g_otg->holding_time_off_pu[src_instance][dst_instance][application]==0) &&
      (g_otg->holding_time_off_ed[src_instance][dst_instance][application]==0) &&
      (g_otg->holding_time_off_pe[src_instance][dst_instance][application]==0))
    otg_info->state[src_instance][dst_instance][application]=PE_STATE;
  else {

    if (otg_info->state_transition_prob[src_instance][dst_instance][application]==0) {
      otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
      otg_info->state[src_instance][dst_instance][application]=OFF_STATE;
      LOG_D(OTG,"[%d][%d][Appli id %d] STATE:: OFF INIT \n", src_instance, dst_instance, application);
      otg_info->start_holding_time_off[src_instance][dst_instance][application]=0/*ctime*/;
    }

    //LOG_D(OTG,"[%d][[%d] HOLDING_TIMES OFF_PE: %d, OFF_PU: %d, OFF_ED %d, PE_OFF: %d \n", src_instance, dst_instance, g_otg->holding_time_off_pe[src_instance][dst_instance], g_otg->holding_time_off_pu[src_instance][dst_instance],g_otg->holding_time_off_ed[src_instance][dst_instance], g_otg->holding_time_pe_off[src_instance][dst_instance] );

    switch (otg_info->state[src_instance][dst_instance][application]) {

    case OFF_STATE:

      if (ctime>otg_info->start_holding_time_off[src_instance][dst_instance][application]) {
        otg_info->c_holding_time_off[src_instance][dst_instance][application]= ctime - otg_info->start_holding_time_off[src_instance][dst_instance][application];
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] STATE:: OFF Holding Time %d (%d, %d)\n", src_instance, dst_instance,application , g_otg->aggregation_level[src_instance][dst_instance][application],
              otg_info->c_holding_time_off[src_instance][dst_instance][application], ctime, otg_info->start_holding_time_off[src_instance][dst_instance][application]);
      }

      if ( ((otg_info->state_transition_prob[src_instance][dst_instance][application]>= 1-(g_otg->prob_off_pu[src_instance][dst_instance][application]
             +g_otg->prob_off_ed[src_instance][dst_instance][application]+g_otg->prob_off_pe[src_instance][dst_instance][application]))
            && (otg_info->state_transition_prob[src_instance][dst_instance][application]<1-(g_otg->prob_off_ed[src_instance][dst_instance][application]
                +g_otg->prob_off_pe[src_instance][dst_instance][application])))
           && (otg_info->c_holding_time_off[src_instance][dst_instance][application]>=g_otg->holding_time_off_pu[src_instance][dst_instance][application])) {
        otg_info->state[src_instance][dst_instance][application]=PU_STATE;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: OFF-->PU  %d  (ctime %d, start %d )\n", src_instance, dst_instance,application,
              g_otg->aggregation_level[src_instance][dst_instance][application], otg_info->c_holding_time_off[src_instance][dst_instance][application], ctime,
              otg_info->start_holding_time_off[src_instance][dst_instance][application]);
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);//EDITED turn off

      } else if ( ((otg_info->state_transition_prob[src_instance][dst_instance][application]>= 1-(g_otg->prob_off_ed[src_instance][dst_instance][application]
                    +g_otg->prob_off_pe[src_instance][dst_instance][application]))
                   && (otg_info->state_transition_prob[src_instance][dst_instance][application]< 1-g_otg->prob_off_pe[src_instance][dst_instance][application]))
                  && (otg_info->c_holding_time_off[src_instance][dst_instance][application]>=g_otg->holding_time_off_ed[src_instance][dst_instance][application])) {
        otg_info->state[src_instance][dst_instance][application]=ED_STATE;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: OFF-->ED \n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance][application]);
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
      }

      else if (((otg_info->state_transition_prob[src_instance][dst_instance][application]>=1-g_otg->prob_off_pe[src_instance][dst_instance][application])
                && (otg_info->state_transition_prob[src_instance][dst_instance][application]<=1))
               && (otg_info->c_holding_time_off[src_instance][dst_instance]>=g_otg->holding_time_off_pe[src_instance][dst_instance])) {
        otg_info->state[src_instance][dst_instance][application]=PE_STATE;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: OFF-->PE \n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance]);
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);//EDITED turn off

      } else {
        otg_info->c_holding_time_off[src_instance][dst_instance][application]= ctime - otg_info->start_holding_time_off[src_instance][dst_instance][application];
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] STATE:: OFF\n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance][application]);
      }

      break;

    case PU_STATE:
      if  (otg_info->state_transition_prob[src_instance][dst_instance][application]<=1-(g_otg->prob_pu_ed[src_instance][dst_instance][application]
           +g_otg->prob_pu_pe[src_instance][dst_instance][application])) {
        //otg_info->state[src_instance][dst_instance]=OFF_STATE;
        otg_info->state[src_instance][dst_instance][application]=OFF_STATE;
        otg_info->start_holding_time_off[src_instance][dst_instance][application]=ctime;
        otg_info->c_holding_time_off[src_instance][dst_instance][application]=0;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: PU-->OFF \n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance][application]);
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
      } else if  ((otg_info->state_transition_prob[src_instance][dst_instance][application]<=1-g_otg->prob_pu_pe[src_instance][dst_instance][application])
                  && (otg_info->state_transition_prob[src_instance][dst_instance][application]>1-(g_otg->prob_pu_ed[src_instance][dst_instance][application]
                      +g_otg->prob_pu_pe[src_instance][dst_instance][application]))) {
        //otg_info->state[src_instance][dst_instance]=ON_STATE;
        otg_info->state[src_instance][dst_instance][application]=ED_STATE;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: PU-->ED \n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance][application]);
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);//EDITED turn off
      } else { /*if (otg_info->state_transition_prob[src_instance][dst_instance]>=g_otg->prob_pu_ed)*/
        //otg_info->state[src_instance][dst_instance]=ON_STATE;
        otg_info->state[src_instance][dst_instance][application]=PE_STATE;
        otg_info->start_holding_time_pe_off[src_instance][dst_instance][application]=ctime;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: PU-->PE \n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance][application]);
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
      }

      break;

    case ED_STATE:
      if  (otg_info->state_transition_prob[src_instance][dst_instance][application]<1-(g_otg->prob_ed_pu[src_instance][dst_instance][application] +
           g_otg->prob_ed_pe[src_instance][dst_instance][application])) {
        //otg_info->state[src_instance][dst_instance]=OFF_STATE;
        otg_info->state[src_instance][dst_instance][application]=OFF_STATE;
        otg_info->start_holding_time_off[src_instance][dst_instance][application]=ctime;
        otg_info->c_holding_time_off[src_instance][dst_instance][application]=0;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: ED-->OFF \n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance][application]);
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
      } else if  ((otg_info->state_transition_prob[src_instance][dst_instance][application]>=1-(g_otg->prob_ed_pu[src_instance][dst_instance][application] +
                   g_otg->prob_ed_pe[src_instance][dst_instance][application] ))
                  && (otg_info->state_transition_prob[src_instance][dst_instance][application]<1-g_otg->prob_ed_pe[src_instance][dst_instance][application]))  {
        //otg_info->state[src_instance][dst_instance]=ON_STATE;
        otg_info->state[src_instance][dst_instance][application]=PE_STATE;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: ED-->PU \n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance][application]);
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
      } else { /*if ((otg_info->state_transition_prob[src_instance][dst_instance]>=1-g_otg->prob_ed_pe)&&(otg_info->state_transition_prob[src_instance][dst_instance]<=1)) */
        //otg_info->state[src_instance][dst_instance]=ON_STATE;
        otg_info->state[src_instance][dst_instance][application]=PE_STATE;
        otg_info->start_holding_time_pe_off[src_instance][dst_instance][application]=ctime;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: ED-->PE \n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance][application]);
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
      }

      break;

    case PE_STATE:
      if (g_otg->holding_time_pe_off[src_instance][dst_instance][application]<=otg_info->c_holding_time_pe_off[src_instance][dst_instance][application]) {
        //otg_info->state[src_instance][dst_instance]=OFF_STATE;
        otg_info->state[src_instance][dst_instance][application]=OFF_STATE;
        LOG_D(OTG,"[%d][%d][Appli id %d][Agg Level=%d] NEW STATE:: PE->OFF\n", src_instance, dst_instance,application, g_otg->aggregation_level[src_instance][dst_instance][application]);
        otg_info->c_holding_time_pe_off[src_instance][dst_instance][application]=0;
        otg_info->state_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
        otg_info->start_holding_time_off[src_instance][dst_instance][application]=ctime;
        otg_info->c_holding_time_off[src_instance][dst_instance][application]=0;
      } else { /* if (g_otg->holding_time_pe_off>otg_info->c_holding_time_pe_off[src_instance][dst_instance])*/
        if (ctime>otg_info->start_holding_time_pe_off[src_instance][dst_instance][application])
          otg_info->c_holding_time_pe_off[src_instance][dst_instance][application]=ctime-otg_info->start_holding_time_pe_off[src_instance][dst_instance][application];

        LOG_D(OTG,"[%d][%d][Appli id %d] STATE:: PE \n", src_instance, dst_instance,application);
      }

      break;

    default:
      LOG_W(OTG,"Unknown state(%d) \n", otg_info->state[src_instance][dst_instance][application]);
      otg_info->state[src_instance][dst_instance][application]= OFF_STATE; // switch to default state

      break;
    }
  }
}




void voip_traffic(
  const int src_instance,
  const int dst_instance,
  const int application,
  const int ctime)
{

  /*****************************************************************************************************
  RECOMMANDATION UIT-T P.59 (VOIX CONVERSATIONNELLE ARTIFICIELLE)
  Tst = − 0, 854 ln (1 − x1 ) //Tst (duration of single word)
  Tdt = − 0, 226 ln (1 − x2 ) //Tdt (duration of double-talk)
  Tms = − 0, 456 ln (1 − x3 ) //Tms (length of mutual silence)

  0 < x1, x2, x3 < 1: Random variables with uniform distribution
   *****************************************************************************************************/

  if (otg_info->silence_time[src_instance][dst_instance][application]==0) {
    //otg_info->voip_transition_prob[src_instance][dst_instance][application]=uniform_dist(0,1);
    otg_info->voip_state[src_instance][dst_instance][application]=SILENCE;
    LOG_D(OTG,"[%d][%d][Appli id %d] STATE:: SILENCE INIT \n", src_instance, dst_instance, application);//EDITED turn off
    otg_info->start_voip_silence[src_instance][dst_instance][application]=ctime /*ctime*/;
    otg_info->c_holding_time_talk[src_instance][dst_instance][application]=0;
    otg_info->c_holding_time_silence[src_instance][dst_instance][application]=0;
    otg_info->silence_time[src_instance][dst_instance][application]=ceil((-0.854*log(1-(uniform_dist(0,1))))*1000) + ceil((-0.226*log(1-(uniform_dist(0,1))))*1000);
    otg_info->simple_talk_time[src_instance][dst_instance][application]=ceil((-0.356*log(1-(uniform_dist(0,1))))*1000) ;

  }


  switch (otg_info->voip_state[src_instance][dst_instance][application]) {

  case SILENCE:

    if (ctime>otg_info->start_voip_silence[src_instance][dst_instance][application]) {
      if (ctime>otg_info->start_voip_silence[src_instance][dst_instance][application])
        otg_info->c_holding_time_silence[src_instance][dst_instance][application]= ctime - otg_info->start_voip_silence[src_instance][dst_instance][application];

      LOG_D(OTG,"[%d][%d][Appli id %d] VOIP STATE:: SILENCE %d (ctime=%d, start=%d)\n", src_instance, dst_instance,application,
            otg_info->c_holding_time_silence[src_instance][dst_instance][application], ctime, otg_info->start_voip_silence[src_instance][dst_instance][application]);
    }

    if (otg_info->c_holding_time_silence[src_instance][dst_instance][application]>=otg_info->silence_time[src_instance][dst_instance][application]) {
      otg_info->voip_state[src_instance][dst_instance][application]=SIMPLE_TALK;
      LOG_I(OTG,"[%d][%d][Appli id %d] NEW VOIP STATE :: SILENCE-->TALK  %d  (ctime=%d, start=%d )\n", src_instance, dst_instance,application ,
            otg_info->c_holding_time_silence[src_instance][dst_instance][application], ctime, otg_info->start_voip_silence[src_instance][dst_instance][application]);
      otg_info->start_voip_talk[src_instance][dst_instance][application]=ctime;
      otg_info->c_holding_time_talk[src_instance][dst_instance][application]=0;
      otg_info->simple_talk_time[src_instance][dst_instance][application]=ceil((-0.854*log(1-(uniform_dist(0,1))))*1000);
    } else {
      if (ctime>otg_info->start_voip_silence[src_instance][dst_instance][application])
        otg_info->c_holding_time_silence[src_instance][dst_instance][application]= ctime - otg_info->start_voip_silence[src_instance][dst_instance][application];

      LOG_D(OTG,"[%d][%d][Appli id %d] STATE:: SILENCE [timer:%d] \n", src_instance, dst_instance,application, otg_info->c_holding_time_silence[src_instance][dst_instance][application]);//EDITED turn off
    }

    break;

  case SIMPLE_TALK:
    if (otg_info->c_holding_time_talk[src_instance][dst_instance][application]>=otg_info->simple_talk_time[src_instance][dst_instance][application]) {
      otg_info->voip_state[src_instance][dst_instance][application]=SILENCE;
      LOG_D(OTG,"[%d][%d][Appli id %d] NEW VOIP STATE:: TALK-->SILENCE  %d  (ctime=%d, start=%d )\n", src_instance, dst_instance,application ,
            otg_info->c_holding_time_talk[src_instance][dst_instance][application], ctime, otg_info->start_voip_talk[src_instance][dst_instance][application]);//EDITED turn off
      otg_info->start_voip_silence[src_instance][dst_instance][application]=ctime;
      otg_info->c_holding_time_silence[src_instance][dst_instance][application]=0;
      otg_info->silence_time[src_instance][dst_instance][application]=ceil((-0.456*log(1-(uniform_dist(0,1))))*1000)+ceil((-0.226*log(1-(uniform_dist(0,1))))*1000);
    } else {
      if (ctime>otg_info->start_voip_talk[src_instance][dst_instance][application])
        otg_info->c_holding_time_talk[src_instance][dst_instance][application]= ctime - otg_info->start_voip_talk[src_instance][dst_instance][application];

      LOG_D(OTG,"[%d][%d][Appli id %d] VOIP STATE:: TALK [timer:%d]\n", src_instance, dst_instance,application, otg_info->c_holding_time_talk[src_instance][dst_instance][application]);//EDITED turn off
      LOG_D(OTG, "test_talk [ctime %d] [start talk %d] [%d] \n",ctime, otg_info->start_voip_talk[src_instance][dst_instance][application],
            otg_info->c_holding_time_talk[src_instance][dst_instance][application] );//EDITED turn off
    }

    break;

  default:
    LOG_W(OTG,"Unknown VOIP state(%d) \n", otg_info->voip_state[src_instance][dst_instance][application]);
    otg_info->voip_state[src_instance][dst_instance][application]= SILENCE; // switch to default state

    break;
  }


}


