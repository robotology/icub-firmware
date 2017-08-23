/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_canprotocol_analog_polling.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include <cstring>

#include "embot_hw.h"



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



namespace embot { namespace app { namespace canprotocol { namespace analog { namespace polling {

    bool supported(std::uint8_t cmd)
    {
        return (CMD::none != convert(cmd))? true : false;
    }
        
    CMD convert(std::uint8_t cmd)
    {
        static const std::uint64_t aspollmask256[4] = 
        {
            // bits 0-63
            (1ULL << static_cast<std::uint8_t>(CMD::SET_BOARD_ADX))             | (1ULL << static_cast<std::uint8_t>(CMD::GET_FIRMWARE_VERSION))  | 
            (1ULL << static_cast<std::uint8_t>(CMD::SET_TXMODE))                | (1ULL << static_cast<std::uint8_t>(CMD::SET_CANDATARATE))       | 
            (1ULL << static_cast<std::uint8_t>(CMD::GET_FULL_SCALES))           | 
            (1ULL << static_cast<std::uint8_t>(CMD::SET_FULL_SCALES))           | (1ULL << static_cast<std::uint8_t>(CMD::SAVE2EE))               |
            (1ULL << static_cast<std::uint8_t>(CMD::GET_EEPROM_STATUS))         | (1ULL << static_cast<std::uint8_t>(CMD::SET_SERIAL_NO))         |
            (1ULL << static_cast<std::uint8_t>(CMD::GET_SERIAL_NO))             | (1ULL << static_cast<std::uint8_t>(CMD::GET_AMP_GAIN))          |    
            (1ULL << static_cast<std::uint8_t>(CMD::SET_AMP_GAIN))              | (1ULL << static_cast<std::uint8_t>(CMD::GET_CH_ADC))            |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_CH_DAC))                | (1ULL << static_cast<std::uint8_t>(CMD::GET_CH_DAC))            |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_MATRIX_RC))             | (1ULL << static_cast<std::uint8_t>(CMD::GET_MATRIX_RC))         |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_MATRIX_G))              | (1ULL << static_cast<std::uint8_t>(CMD::GET_MATRIX_G))          |                
            (1ULL << static_cast<std::uint8_t>(CMD::SET_CALIB_TARE))            | (1ULL << static_cast<std::uint8_t>(CMD::GET_CALIB_TARE))        |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_CURR_TARE))             | (1ULL << static_cast<std::uint8_t>(CMD::GET_CURR_TARE))         ,
            // bits 64-127
            (1ULL << (static_cast<std::uint8_t>(CMD::SKIN_SET_BRD_CFG)-64))     | (1ULL << (static_cast<std::uint8_t>(CMD::ACC_GYRO_SETUP)-64))   |
            (1ULL << (static_cast<std::uint8_t>(CMD::SKIN_SET_TRIANG_CFG)-64)),
            // bits 128-191    
            0, 
            // bits 191-255
            0 
        }; 

        std::uint8_t ind = cmd >> 6;
        std::uint8_t pos = cmd & 0x3f;
        if(true == embot::common::bit::check(aspollmask256[ind], pos))
        {
            return static_cast<CMD>(cmd);            
        }
        
        return CMD::none;         
    } 

    std::uint8_t convert(CMD cmd)
    {
        return static_cast<std::uint8_t>(cmd);
    }      
    
    
    bool Message_SET_TXMODE::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_TXMODE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        switch(board)
        {
            case Board::mtb:
            case Board::mtb4:
            {
                // according to protocol, we may have 0 (icubCanProto_as_sigmode_signal) or 1 (icubCanProto_as_sigmode_dontsignal)
                info.transmit = (0 == candata.datainframe[0]) ? true : false;
                info.strainmode = StrainMode::none;
            } break;
            
            case Board::strain:
            case Board::strain2:
            {
                // according to protocol, we may have 0 (eoas_strainmode_txcalibrateddatacontinuously), 1 (eoas_strainmode_acquirebutdonttx),
                // 3 (eoas_strainmode_txuncalibrateddatacontinuously), or 4 (eoas_strainmode_txalldatacontinuously)
                if(0 == candata.datainframe[0])    
                {
                    info.transmit = true;                        
                    info.strainmode = StrainMode::txCalibrated;
                }
                else if(1 == candata.datainframe[0])       
                {
                    info.transmit = false;
                    info.strainmode = StrainMode::acquireOnly;
                }
                else if(3 == candata.datainframe[0])       
                {
                    info.transmit = true;  
                    info.strainmode = StrainMode::txUncalibrated;
                }
                else if(4 == candata.datainframe[0])       
                {
                    info.transmit = true;  
                    info.strainmode = StrainMode::txAll;
                }
                else                            
                {       
                    info.transmit = false;  
                    info.strainmode = StrainMode::none;  
                }                        
            } break;   

            default:
            {
                info.transmit = false;  
                info.strainmode = StrainMode::none;                      
            } break;
        }
      
        return true;         
    }                    
        
    bool Message_SET_TXMODE::reply()
    {
        return false;
    } 
    

    bool Message_SKIN_SET_BRD_CFG::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SKIN_SET_BRD_CFG) != frame2cmd(inframe))
        {
            return false; 
        }
        
        switch(candata.datainframe[0])
        {
            case static_cast<std::uint8_t>(SkinType::withTemperatureCompensation):
            {
                info.skintype = SkinType::withTemperatureCompensation;
            } break;
            
            case static_cast<std::uint8_t>(SkinType::palmFingerTip):
            {
                info.skintype = SkinType::palmFingerTip;
            } break;
            
            case static_cast<std::uint8_t>(SkinType::withoutTempCompensation):
            {
                info.skintype = SkinType::withoutTempCompensation;
            } break;
            
            case static_cast<std::uint8_t>(SkinType::testmodeRAW):
            {
                info.skintype = SkinType::testmodeRAW;
            } break;
            
            default:
            {
                info.skintype = SkinType::none;
            } break;                                
        }
        
        info.txperiod = 1000*candata.datainframe[1]; // transform from msec into usec
        info.noload = candata.datainframe[2];
      
        return true;         
    } 
    
        
    bool Message_SKIN_SET_BRD_CFG::reply()
    {
        return false;
    }  


    bool Message_SKIN_SET_TRIANG_CFG::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SKIN_SET_TRIANG_CFG) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.trgStart = candata.datainframe[0];
        info.trgEnd= candata.datainframe[1];
        info.shift = candata.datainframe[2];
        info.enabled = embot::common::bit::check(candata.datainframe[3], 0);
        info.cdcOffset = candata.datainframe[4] | static_cast<std::uint16_t>(candata.datainframe[5]) << 8;
     
        return true;         
    } 
    
        
    bool Message_SKIN_SET_TRIANG_CFG::reply()
    {
        return false;
    }   



    bool Message_SET_CANDATARATE::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CANDATARATE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        std::uint8_t rate = candata.datainframe[0];
        info.txperiod = rate * embot::common::time1millisec;
      
        return true;         
    }                    
        
    bool Message_SET_CANDATARATE::reply()
    {
        return false;
    } 
    
    bool Message_SET_FULL_SCALES::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_FULL_SCALES) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];
        info.fullscale = static_cast<std::uint16_t>(candata.datainframe[1]) << 8 + static_cast<std::uint16_t>(candata.datainframe[2]);
      
        return true;         
    }                    
        
    bool Message_SET_FULL_SCALES::reply()
    {
        return false;
    } 
    
    
    bool Message_SET_AMP_GAIN::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_AMP_GAIN) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];
        info.gain1 = static_cast<std::uint16_t>(candata.datainframe[1]) << 8 + static_cast<std::uint16_t>(candata.datainframe[2]);
        info.gain2 = static_cast<std::uint16_t>(candata.datainframe[3]) << 8 + static_cast<std::uint16_t>(candata.datainframe[4]);
      
        return true;         
    }                    
        
    bool Message_SET_AMP_GAIN::reply()
    {
        return false;
    }         
    
    
    bool Message_SET_CH_DAC::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CH_DAC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];
        info.offset = static_cast<std::uint16_t>(candata.datainframe[1]) << 8 + static_cast<std::uint16_t>(candata.datainframe[2]);
      
        return true;         
    }                    
        
    bool Message_SET_CH_DAC::reply()
    {
        return false;
    }       
    
    bool Message_SET_MATRIX_RC::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_MATRIX_RC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.row = candata.datainframe[0];
        info.col = candata.datainframe[1];
        info.value = static_cast<std::uint16_t>(candata.datainframe[2]) << 8 + static_cast<std::uint16_t>(candata.datainframe[3]);
      
        return true;         
    }                    
        
    bool Message_SET_MATRIX_RC::reply()
    {
        return false;
    }   


    bool Message_SET_CALIB_TARE::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CALIB_TARE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        // now ... i look at the mode. not even at the length.
        
        switch(candata.datainframe[0])
        {
            case 0:
            {
                info.mode = Mode::everychannelreset;
            } break;
            
            case 1:
            {
                info.mode = Mode::everychannelnegativeofadc;
            } break;  

            case 2:
            {
                info.mode = Mode::setchannelwithvalue;
            } break;

            default:
            {
                info.mode = Mode::unknown;
            } break;                   
        }
        
        if(Mode::unknown == info.mode)
        {
            return false;
        }
        
        if(Mode::setchannelwithvalue == info.mode)
        {
            info.channel = candata.datainframe[1];
            info.value = static_cast<std::uint16_t>(candata.datainframe[2]) << 8 + static_cast<std::uint16_t>(candata.datainframe[3]);
        }
        
        return true;         
    }                    
        
    bool Message_SET_CALIB_TARE::reply()
    {
        return false;
    } 


    bool Message_GET_CALIB_TARE::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CALIB_TARE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];

        return true;
    }  

    bool Message_GET_CALIB_TARE::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = replyinfo.channel;
        dd[1] = (replyinfo.value >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[2] = replyinfo.value & 0xff;             
                    
        std::uint8_t datalen = 3;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_CALIB_TARE), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }            



    bool Message_SET_CURR_TARE::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CURR_TARE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        // now ... i look at the mode. not even at the length.
        
        switch(candata.datainframe[0])
        {
            case 0:
            {
                info.mode = Mode::everychannelreset;
            } break;
            
            case 1:
            {
                info.mode = Mode::everychannelnegativeoftorque;
            } break;  

            case 2:
            {
                info.mode = Mode::setchannelwithvalue;
            } break;

            default:
            {
                info.mode = Mode::unknown;
            } break;                   
        }
        
        if(Mode::unknown == info.mode)
        {
            return false;
        }
        
        if(Mode::setchannelwithvalue == info.mode)
        {
            info.channel = candata.datainframe[1];
            info.value = static_cast<std::uint16_t>(candata.datainframe[2]) << 8 + static_cast<std::uint16_t>(candata.datainframe[3]);
        }
        
        return true;         
    }                    
        
    bool Message_SET_CURR_TARE::reply()
    {
        return false;
    }   


    bool Message_GET_CURR_TARE::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CURR_TARE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];

        return true;
    }  

    bool Message_GET_CURR_TARE::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = replyinfo.channel;
        dd[1] = (replyinfo.value >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[2] = replyinfo.value & 0xff;             
                    
        std::uint8_t datalen = 3;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_CURR_TARE), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }          
    
    bool Message_SET_MATRIX_G::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_MATRIX_G) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.gain = candata.datainframe[0];
      
        return true;         
    }                    
        
    bool Message_SET_MATRIX_G::reply()
    {
        return false;
    }    
    
    
    bool Message_SET_SERIAL_NO::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_SERIAL_NO) != frame2cmd(inframe))
        {
            return false; 
        }
        
        std::memmove(info.serial, &candata.datainframe[0], sizeof(info.serial));
      
        return true;         
    }                    
        
    bool Message_SET_SERIAL_NO::reply()
    {
        return false;
    } 


    bool Message_GET_SERIAL_NO::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_SERIAL_NO) != frame2cmd(inframe))
        {
            return false; 
        }

        return true;
    }  

    bool Message_GET_SERIAL_NO::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        std::memmove(dd, replyinfo.serial, sizeof(replyinfo.serial));

        std::uint8_t datalen = sizeof(replyinfo.serial);
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_SERIAL_NO), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  



    bool Message_GET_FULL_SCALES::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_FULL_SCALES) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];

        return true;
    }  

    bool Message_GET_FULL_SCALES::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = replyinfo.channel;
        dd[1] = (replyinfo.fullscale >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[2] = replyinfo.fullscale & 0xff;             // both embobjStrain and teh ems use a wrong conversion ... they both exchange the bytes .... so at teh end the fullscale is correct 
                    
        std::uint8_t datalen = 3;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_FULL_SCALES), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }   

    bool Message_GET_AMP_GAIN::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_AMP_GAIN) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];

        return true;
    }  

    bool Message_GET_AMP_GAIN::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = replyinfo.channel;
        dd[1] = (replyinfo.gain1 >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[2] = replyinfo.gain1 & 0xff;             
        dd[3] = (replyinfo.gain2 >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[4] = replyinfo.gain2 & 0xff;             
        
        std::uint8_t datalen = 5;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_AMP_GAIN), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    
    
    
    bool Message_GET_CH_DAC::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CH_DAC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];

        return true;
    }  

    bool Message_GET_CH_DAC::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = replyinfo.channel;
        dd[1] = (replyinfo.offset >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[2] = replyinfo.offset & 0xff;                   
        
        std::uint8_t datalen = 3;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_CH_DAC), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    

    bool Message_GET_MATRIX_RC::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_MATRIX_RC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.row = candata.datainframe[0];
        info.col = candata.datainframe[1];

        return true;
    }  

    bool Message_GET_MATRIX_RC::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = replyinfo.row;
        dd[1] = replyinfo.col;
        dd[2] = (replyinfo.value >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[3] = replyinfo.value & 0xff;                   
        
        std::uint8_t datalen = 4;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_MATRIX_RC), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    
    bool Message_GET_MATRIX_G::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_MATRIX_G) != frame2cmd(inframe))
        {
            return false; 
        }

        return true;
    }  

    bool Message_GET_MATRIX_G::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = replyinfo.gain;      
        
        std::uint8_t datalen = 1;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_MATRIX_G), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    
    
    bool Message_GET_CH_ADC::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CH_ADC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel =  candata.datainframe[0];
        info.getcalibrated = (0 == candata.datainframe[1]) ? false : true;

        return true;
    }  

    bool Message_GET_CH_ADC::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = replyinfo.channel;
        dd[1] = (true == replyinfo.valueiscalibrated) ? (1) : (0);
        dd[2] = (replyinfo.adcvalue >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[3] = replyinfo.adcvalue & 0xff;             

        
        std::uint8_t datalen = 4;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_CH_ADC), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }        

    bool Message_GET_EEPROM_STATUS::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_EEPROM_STATUS) != frame2cmd(inframe))
        {
            return false; 
        }            

        return true;
    }  

    bool Message_GET_EEPROM_STATUS::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = (true == replyinfo.saved) ? 1 : 0;
                    
        std::uint8_t datalen = 1;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_EEPROM_STATUS), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  


    bool Message_SAVE2EE::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::SAVE2EE) != frame2cmd(inframe))
        {
            return false; 
        }

        return true;
    }  

    bool Message_SAVE2EE::reply(embot::hw::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = (true == replyinfo.ok) ? 1 : 0;
                    
        std::uint8_t datalen = 1;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::SAVE2EE), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }             
    
    
    bool Message_ACC_GYRO_SETUP::load(const embot::hw::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::ACC_GYRO_SETUP) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.maskoftypes = candata.datainframe[0];
        info.txperiod = 1000*candata.datainframe[1]; // transform from msec into usec
      
        return true;         
    } 
    
        
    bool Message_ACC_GYRO_SETUP::reply()
    {
        return false;
    }     
    
        
}}}}} // namespace embot { namespace app { namespace canprotocol { namespace analog { namespace polling {
    
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


