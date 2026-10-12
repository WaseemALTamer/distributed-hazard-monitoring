#include <Arduino.h>


#include "protocol_event_handler.h"
#include "protocol.h"

#include "sequence_boot.h"
#include "sequence_normal.h"
#include "sequence_danger.h"



namespace DeviceStatusManager{



    void set_status(SetDeviceStatus content, IPAddress senderIP, uint16_t senderPort){

        switch (content.status){
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
            
            default:
                break;
        }
    }


    void init(){
        ProtocalEventHandler::register_set_device_status_callback(set_status);
    }
}