# Multi-Access Edge Computing-Enabled Channel-Aware Video Streaming for 4G LTE
## Table of Contents
- [Abstract](#abstract)
- [Introduction](#introduction)
    - [Objective](#objective)
- [LTE Scheduling over Radio Interface](#lte-scheduling-over-radio-interface)
    - [Our MEC Scenario](#our-mec-scenario)
- [Realization](#realization)
- [Implementation](#implementation)
- [Experiment Results](#experiment-results)
- [Related Code](#related-code)
- [Compilation and Execution](https://github.com/chen-chi-wang/Thesis/blob/master/RELEASE_NOTE.md)

## Abstract

- We investigate the ETSI MEC Intelligent Video Acceleration Service. 
- Hope to make LTE network more efficient. 
- That is, an MEC approach for the video transmission. 
- Our results indicates that the MEC approach can improve latency and throughput performance.


## Introduction

- Mobile Data Traffic Growth
    - Ever increasing numbers of users are surfing the net via an LTE access network
    - According to Cisco, 78% of the world’s mobile data traffic will be video-based by 2021

- Challenge for LTE Networks
    - Higher-bandwidth video content

      <img src="https://i.imgur.com/JXaU7ok.png" width="205">

    - End device proliferation

      <img src="https://i.imgur.com/mDcf0hQ.png" width="220">

    - Cost
        - All result in the increasing of infrastructural  capital expenditures

- Adaptive Bitrate Streaming
    - Video server maintains video content in many bitrate types
    - The browser adjusts the quality based on the bandwidth
    - MPEG-DASH is an HTTP-based protocol runs over TCP

- TCP Performance in Wireless Network
    - TCP’s performance can easily deteriorate in a wireless network
    - If the packet is lost for reasons other than congestion

- ETSI Multi-access Edge Computing (MEC)
    - Aim to reduce latency, ensure highly efficient network
    - The RAN edge offers a direct access to real-time radio information
    
- Intelligent Video Acceleration Scenario
    - MEC Server: Opens up network functionality
    - Video server: Get a near real-time indication on the throughput

### Objective

Implement and Verify the MEC approach for the video transmission.

<img src="https://i.imgur.com/YjOV6rO.png" width="570">
 

## LTE Scheduling over Radio Interface
In this section, we elaborate some concepts that will be used in the latter section.

### Logical Channels
- DCCH (Dedicated Control Channel): Dedicated Control Channel for transmitting signal data
- DTCH (Dedicated Traffic Channel): Dedicated Traffic Channel for transmitting user data

  <img src="https://i.imgur.com/uoDokad.png" width="405"> &nbsp;&nbsp;&nbsp;<img src="https://i.imgur.com/wJpLQ2g.png" width="280">

### CQI MAC Scheduling
- Apart from the wired network, channel condition can vary dramatically in a wireless network
- Therefore, the channel-aware solution is generally adopted

  <img src="https://i.imgur.com/RrbAkgQ.png" width="300">

- MAC Packet Scheduler schedules between UEs by allocating the radio resources based on many priorities
    - Control data
    - SDU creation time
    - Buffer size
    - Channel Quality Indicator (CQI)
- TTI
    - Transmission Time Interval
    - The time granularity of scheduling process, which is 1ms
    
### HARQ
- The MAC layer offers the Hybrid Automatic Repeat Request (HARQ) function for the error correction
- The retransmission data has the highest priority

  <img src="https://i.imgur.com/cBuSYvy.png" width="470">
  
### Video Quality and Synchronization
- The video player adjusts the video quality based on the speed of the Internet connection
- Decision Time Interval (DTI) is a concept indicates that the minimum amount that can be split in each video source
  
  <img src="https://i.imgur.com/U9NDflX.png" width="260"> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="https://i.imgur.com/CNPC3eR.png" width="380">
  
### Our MEC Scenario

<img src="https://i.imgur.com/qAJBnDG.png" width="535">

## Realization
### The Filter Module
- The filter is a module designed between the MAC layer and the RLC layer.

- Act as video server: To make a decision on selection of suitable bitrate type based on CQI.

- Control synchronization between different bitrate type.

  <img src="https://i.imgur.com/eUO1Qhk.png" width="320"> &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="https://i.imgur.com/1dko7zI.png" width="320">
  
### Resource Block (RB) Re-allocation

- EURECOM OpenAirInterface (OAI)
    - An LTE eNB software
- Problem
    - Through the traffic generator (OTG) interface, OAI provides one DTCH channel per UE 
    - Which is not compliant with 3GPP
- Trick
    - We turn to configure multiple UEs in order to accumulate different video sources

      <img src="https://i.imgur.com/u3kKmTg.png" width="300">
- Issues
    - Bandwidth is divided equally to every user by default
    - Some user level aspect 
        - Control data, retransmission data
    - Multi-user’s resource deadlock problem
- Solution
    - Determine the UE priority by ourselves 
      i.e., **Resource Block (RB) re-allocation**
      
## Implementation
### Scheduling Process

We modified and added some functions in OAI's scheduling procedure.

<img src="https://i.imgur.com/8ICEM2F.png" width="400">

### The Filter Operations

<img src="https://i.imgur.com/tKOPkFX.png" width="332">

### Sort UEs
In Sort_UEs procedure, we determined the UE priority by ourselves.

Noticed that UEs may have 
- Different retransmission times
- Control data
  
  <img src="https://i.imgur.com/YnCIIQq.png" width="375">

### RB Pass Mechanism

For the synchronization purpose, we need to pass a fake number of RBs as indication.

<img src="https://i.imgur.com/jFjBIBk.png" width="225"> <img src="https://i.imgur.com/CjKQTmS.png" width="280">

<img src="https://i.imgur.com/kHbXRt9.png" width="253">


## Experiment Results

### Experimental Environment
<img src="https://i.imgur.com/GIIHUKo.png" width="365"> <img src="https://i.imgur.com/7pU5ZEC.png" width="365">

### The Filter Module - Performance
By using constant-bitrate as our control groups, the testbed indicates that, consistent with our expectations, our mechanism can significantly benefit from lower latency with a reasonable throughput.

<img src="https://i.imgur.com/ePGQygb.png" width="180">

<img src="https://i.imgur.com/7jQNd45.png" width="380"> &nbsp;<img src="https://i.imgur.com/psSGtIi.png" width="375">

### RB Re-allocation - Performance
Compares with the original fairness algorithm, the RB re-allocation determined by ourselves can verify the effect of priority scheduling.

<img src="https://i.imgur.com/Hycrgpd.png" width="200">

<img src="https://i.imgur.com/5wGroDX.png" width="374"> &nbsp;<img src="https://i.imgur.com/zIRjb6g.png" height="257">

## Related Code
$OPENAIR_HOME/targets/SIMU/EXAMPLES/OSD/WEBXML/template_41.xml

$OPENAIR_HOME/openair2/UTIL/OTG/otg_tx.c

$OPENAIR_HOME/openair2/UTIL/OTG/otg_rx.c

$OPENAIR_HOME/openair2/LAYER2/MAC/pre_processor.c

$OPENAIR_HOME/openair2/LAYER2/MAC/eNB_scheduler_dlsch.c

$OPENAIR_HOME/openair2/LAYER2/MAC/myVars.h

$OPENAIR_HOME/openair2/LAYER2/MAC/utility.h

### [Compilation and Execution](https://github.com/chen-chi-wang/Thesis/blob/master/RELEASE_NOTE.md)






