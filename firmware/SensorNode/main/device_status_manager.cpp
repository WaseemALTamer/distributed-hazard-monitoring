#include <Arduino.h>


#include "network_wifi.h"


#include "protocol_event_handler.h"
#include "protocol.h"



#include "sequence_wifi_connecting.h"
#include "sequence_normal.h"
#include "sequence_danger.h"
#include "sequence_boot.h"



namespace DeviceStatusManager{

    // this module is a watch dog for the whole  system it will not modify anything
    // it will simply just listen apply sequences whlie also  holding devices state
    // note that all the sequences are not responsible in doing anything other than
    // playing led sequences all the  other tasks  like wifi  connect trasmit  data
    // every few seconds is a system job not a sequences job  


    int current_state = 0;





    void set_status(int status){

        if (status == current_state) return;

        switch (status){
            case DEVICE_STATUS_BOOTING:{
                SequenceBoot::init();
                break;
            }
            case DEVICE_STATUS_NORMAL:{
                SequenceNormal::init();
                break;
            }
            case DEVICE_STATUS_DANGER:{
                SequenceDanger::init();
                break;
            }
            case DEVICE_STATUS_NETWORK_ERROR:{
                SequenceWifiConnecting::init();
                break;
            }

            default:
                return;
        }

        current_state = status;

    }


    void network_set_status(SetDeviceStatus content, IPAddress senderIP, uint16_t senderPort){
        set_status(content.status);
    }


    void init(){
        ProtocalEventHandler::register_set_device_status_callback(network_set_status);
    }

    void loop(){
        // this loop orders the if statments in prority the if statmets
        // are goes like this first if statmet checks the system second
        // if statmet checks if we are already on the state

        // if we are on the boot sequence wait for it to end
        // the boot seqnece  shuold transition to the normal
        // state after it ends
        if (current_state == DEVICE_STATUS_BOOTING){
            return;
        }


        // check danger
        if(0){ 
            set_status(DEVICE_STATUS_DANGER);
            return; 
        }

        // check wifi 
        if (!NetworkWifi::is_connected){
            set_status(DEVICE_STATUS_NETWORK_ERROR);
            return;
        }


        // last check nothing all the other checks passed 
        // meaning we need to go to normal status
        if(1){
            set_status(DEVICE_STATUS_NORMAL);
            return; 
        }




    }
}