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

#include "embot_prot_can_analog_polling.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_core_binary.h"

#include <cstring>

#include "embot_hw.h"

#include <algorithm>

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace prot { namespace can { namespace analog {
    
    float deciDeg_export(const deciDeg d) { return static_cast<float>(d) * 0.1f; } 
    deciDeg deciDeg_import(const float f) { return static_cast<deciDeg>(f*10.0f); } 
    
}}}}

namespace embot { namespace prot { namespace can { namespace analog { namespace polling {

    bool supported(std::uint8_t cmd)
    {
        return (CMD::none != convert(cmd))? true : false;
    }
        
    CMD convert(std::uint8_t cmd)
    {
        constexpr std::uint64_t aspollmask256[4] = 
        {
            // bits 0-63
            (1ULL << static_cast<std::uint8_t>(CMD::SET_BOARD_ADX))                 | 
            (1ULL << static_cast<std::uint8_t>(CMD::GET_FIRMWARE_VERSION))          | 
            (1ULL << static_cast<std::uint8_t>(CMD::SET_TXMODE))                    | 
            (1ULL << static_cast<std::uint8_t>(CMD::SET_CANDATARATE))               | 
            (1ULL << static_cast<std::uint8_t>(CMD::GET_FULL_SCALES))               | 
            (1ULL << static_cast<std::uint8_t>(CMD::SET_FULL_SCALES))               | 
            (1ULL << static_cast<std::uint8_t>(CMD::SAVE2EE))                       |
            (1ULL << static_cast<std::uint8_t>(CMD::GET_EEPROM_STATUS))             | 
            (1ULL << static_cast<std::uint8_t>(CMD::SET_SERIAL_NO))                 |
            (1ULL << static_cast<std::uint8_t>(CMD::GET_SERIAL_NO))                 | 
            (1ULL << static_cast<std::uint8_t>(CMD::GET_CH_ADC))                    |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_CH_DAC))                    | 
            (1ULL << static_cast<std::uint8_t>(CMD::GET_CH_DAC))                    |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_MATRIX_RC))                 | 
            (1ULL << static_cast<std::uint8_t>(CMD::GET_MATRIX_RC))                 |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_MATRIX_G))                  | 
            (1ULL << static_cast<std::uint8_t>(CMD::GET_MATRIX_G))                  |                
            (1ULL << static_cast<std::uint8_t>(CMD::SET_CALIB_TARE))                | 
            (1ULL << static_cast<std::uint8_t>(CMD::GET_CALIB_TARE))                |
            (1ULL << static_cast<std::uint8_t>(CMD::SET_CURR_TARE))                 | 
            (1ULL << static_cast<std::uint8_t>(CMD::GET_CURR_TARE))                 |
                
            (1ULL << static_cast<std::uint8_t>(CMD::AMPLIFIER_RESET))               | 
            (1ULL << static_cast<std::uint8_t>(CMD::AMPLIFIER_PGA308_CFG1_GET))     |
            (1ULL << static_cast<std::uint8_t>(CMD::AMPLIFIER_PGA308_CFG1_SET))     |
            (1ULL << static_cast<std::uint8_t>(CMD::AMPLIFIER_RANGE_OF_GAIN_GET))   |
            (1ULL << static_cast<std::uint8_t>(CMD::AMPLIFIER_RANGE_OF_OFFSET_GET)) |
            (1ULL << static_cast<std::uint8_t>(CMD::AMPLIFIER_GAINOFFSET_GET))      |
            (1ULL << static_cast<std::uint8_t>(CMD::AMPLIFIER_GAINOFFSET_SET))      |
            (1ULL << static_cast<std::uint8_t>(CMD::AMPLIFIER_OFFSET_AUTOCALIB))    |
            
            
                
            (1ULL << static_cast<std::uint8_t>(CMD::IMU_CONFIG_GET))                |
            (1ULL << static_cast<std::uint8_t>(CMD::IMU_CONFIG_SET))                |
            (1ULL << static_cast<std::uint8_t>(CMD::IMU_TRANSMIT))                  |
                
            (1ULL << static_cast<std::uint8_t>(CMD::THERMOMETER_CONFIG_GET))        |
            (1ULL << static_cast<std::uint8_t>(CMD::THERMOMETER_CONFIG_SET))        |
            (1ULL << static_cast<std::uint8_t>(CMD::THERMOMETER_TRANSMIT))          | 
                
            (1ULL << static_cast<std::uint8_t>(CMD::REGULATIONSET_SET))             |  
            (1ULL << static_cast<std::uint8_t>(CMD::REGULATIONSET_GET))             ,                  

            // bits 64-127
            (1ULL << (static_cast<std::uint8_t>(CMD::SKIN_OBSOLETE_TACT_SETUP)-64)) | 
            (1ULL << (static_cast<std::uint8_t>(CMD::SKIN_SET_BRD_CFG)-64))         | 
            (1ULL << (static_cast<std::uint8_t>(CMD::ACC_GYRO_SETUP)-64))           |
            (1ULL << (static_cast<std::uint8_t>(CMD::SKIN_SET_TRIANG_CFG)-64))      |      
            (1ULL << (static_cast<std::uint8_t>(CMD::POS_CONFIG_GET)-64))           |
            (1ULL << (static_cast<std::uint8_t>(CMD::POS_CONFIG_SET)-64))           |
            (1ULL << (static_cast<std::uint8_t>(CMD::POS_TRANSMIT)-64))             ,     

            // bits 128-191    
            0, 
            // bits 191-255
            0 
        }; 

        std::uint8_t ind = cmd >> 6;
        std::uint8_t pos = cmd & 0x3f;
        if(true == embot::core::binary::bit::check(aspollmask256[ind], pos))
        {
            return static_cast<CMD>(cmd);            
        }
        
        return CMD::none;         
    } 

    std::uint8_t convert(CMD cmd)
    {
        return static_cast<std::uint8_t>(cmd);
    }      
    
    
    bool Message_SET_TXMODE::load(const embot::prot::can::Frame &inframe)
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
    
       

    bool Message_SKIN_OBSOLETE_TACT_SETUP::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SKIN_OBSOLETE_TACT_SETUP) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.txperiod = 40*embot::core::time1millisec;
        info.cdcOffset = 0x2200; //static_cast<std::uint16_t>(candata.datainframe[3]) | (static_cast<std::uint16_t>(candata.datainframe[4]) << 8);
      
        return true;         
    } 
    
        
    bool Message_SKIN_OBSOLETE_TACT_SETUP::reply()
    {
        return false;
    }  
    

    
    

    bool Message_SKIN_SET_BRD_CFG::load(const embot::prot::can::Frame &inframe)
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
 
            case static_cast<std::uint8_t>(SkinType::withTemperatureCompensationV2):
            {
                info.skintype = SkinType::withTemperatureCompensationV2;
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


    bool Message_SKIN_SET_TRIANG_CFG::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SKIN_SET_TRIANG_CFG) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.trgStart = candata.datainframe[0];
        info.trgEnd= candata.datainframe[1];
        info.shift = candata.datainframe[2];
        info.enabled = embot::core::binary::bit::check(candata.datainframe[3], 0);
        // little endian ...
        info.cdcOffset = static_cast<std::uint16_t>(candata.datainframe[4]) | (static_cast<std::uint16_t>(candata.datainframe[5]) << 8);
     
        return true;         
    } 
    
        
    bool Message_SKIN_SET_TRIANG_CFG::reply()
    {
        return false;
    }   



    bool Message_SET_CANDATARATE::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CANDATARATE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        std::uint8_t rate = candata.datainframe[0];
        info.txperiod = rate * embot::core::time1millisec;
      
        return true;         
    }                    
        
    bool Message_SET_CANDATARATE::reply()
    {
        return false;
    } 
    
    bool Message_SET_FULL_SCALES::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_FULL_SCALES) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);

        // big endian ...
        info.fullscale = (static_cast<std::uint16_t>(candata.datainframe[1]) << 8) | static_cast<std::uint16_t>(candata.datainframe[2]);
      
        return true;         
    }                    
        
    bool Message_SET_FULL_SCALES::reply()
    {
        return false;
    } 
    
        
    bool Message_SET_CH_DAC::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_CH_DAC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);
        
        // big endian ...
        info.offset = (static_cast<std::uint16_t>(candata.datainframe[1]) << 8) | static_cast<std::uint16_t>(candata.datainframe[2]);
      
        return true;         
    }                    
        
    bool Message_SET_CH_DAC::reply()
    {
        return false;
    }       
    
    bool Message_SET_MATRIX_RC::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_MATRIX_RC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.row = (candata.datainframe[0] & 0x0F);
        info.col = candata.datainframe[1];
        // big endian ...
        info.value = (static_cast<std::uint16_t>(candata.datainframe[2]) << 8) | static_cast<std::uint16_t>(candata.datainframe[3]);
      
        return true;         
    }                    
        
    bool Message_SET_MATRIX_RC::reply()
    {
        return false;
    }   


    bool Message_SET_CALIB_TARE::load(const embot::prot::can::Frame &inframe)
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
            info.regulationset = (candata.datainframe[1] & 0xF0) >> 4;
            info.channel = (candata.datainframe[1] & 0x0F);
            // big endian ...
            info.value = (static_cast<std::uint16_t>(candata.datainframe[2]) << 8) | static_cast<std::uint16_t>(candata.datainframe[3]);
        }
        else
        {   // use the default ones. 
            info.regulationset = static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse);
            info.channel = 0;
            info.value = 0;
        }
        
        return true;         
    }                    
        
    bool Message_SET_CALIB_TARE::reply()
    {
        return false;
    } 


    bool Message_GET_CALIB_TARE::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CALIB_TARE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);

        return true;
    }  

    bool Message_GET_CALIB_TARE::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.channel & 0x0F); 
        dd[1] = (replyinfo.value >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[2] = replyinfo.value & 0xff;             
                    
        std::uint8_t datalen = 3;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_CALIB_TARE), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }            



    bool Message_SET_CURR_TARE::load(const embot::prot::can::Frame &inframe)
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
                info.mode = Mode::everychannelnegativeofforcetorque;
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
            // big endian ...
            info.value = (static_cast<std::uint16_t>(candata.datainframe[2]) << 8) | static_cast<std::uint16_t>(candata.datainframe[3]);
        }
        
        return true;         
    }                    
        
    bool Message_SET_CURR_TARE::reply()
    {
        return false;
    }   


    bool Message_GET_CURR_TARE::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CURR_TARE) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];

        return true;
    }  

    bool Message_GET_CURR_TARE::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
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
    
    bool Message_SET_MATRIX_G::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::SET_MATRIX_G) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse);
        info.gain = candata.datainframe[0];
      
        return true;         
    }                    
        
    bool Message_SET_MATRIX_G::reply()
    {
        return false;
    }    
    
    
    bool Message_SET_SERIAL_NO::load(const embot::prot::can::Frame &inframe)
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


    bool Message_GET_SERIAL_NO::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_SERIAL_NO) != frame2cmd(inframe))
        {
            return false; 
        }

        return true;
    }  

    bool Message_GET_SERIAL_NO::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        std::memmove(dd, replyinfo.serial, sizeof(replyinfo.serial));

        std::uint8_t datalen = sizeof(replyinfo.serial);
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_SERIAL_NO), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  



    bool Message_GET_FULL_SCALES::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_FULL_SCALES) != frame2cmd(inframe))
        {
            return false; 
        }

        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);

        return true;
    }  

    bool Message_GET_FULL_SCALES::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.channel & 0x0F); 
        dd[1] = (replyinfo.fullscale >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[2] = replyinfo.fullscale & 0xff;             // both embobjStrain and the ems use a wrong conversion ... they both exchange the bytes .... so at teh end the fullscale is correct 
                    
        std::uint8_t datalen = 3;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_FULL_SCALES), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }   

    
    bool Message_GET_CH_DAC::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CH_DAC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);

        return true;
    }  

    bool Message_GET_CH_DAC::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.channel & 0x0F); 
        dd[1] = (replyinfo.offset >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[2] = replyinfo.offset & 0xff;                   
        
        std::uint8_t datalen = 3;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_CH_DAC), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    

    bool Message_GET_MATRIX_RC::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_MATRIX_RC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.row = (candata.datainframe[0] & 0x0F);
        info.col = candata.datainframe[1];

        return true;
    }  

    bool Message_GET_MATRIX_RC::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.row & 0x0F); 
        dd[1] = replyinfo.col;
        dd[2] = (replyinfo.value >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[3] = replyinfo.value & 0xff;                   
        
        std::uint8_t datalen = 4;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_MATRIX_RC), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    
    bool Message_GET_MATRIX_G::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_MATRIX_G) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = static_cast<std::uint8_t>(StrainRegulationSet::theoneinuse);

        return true;
    }  

    bool Message_GET_MATRIX_G::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = replyinfo.gain;      
        
        std::uint8_t datalen = 1;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_MATRIX_G), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    
    
    bool Message_GET_CH_ADC::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_CH_ADC) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.channel = candata.datainframe[0];
        info.getcalibrated = (0 == candata.datainframe[1]) ? false : true;

        return true;
    }  

    bool Message_GET_CH_ADC::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        uint8_t valueindex = std::min(static_cast<uint8_t>(ReplyInfo::adcvaluesmaxnumber-1), static_cast<uint8_t>(replyinfo.valueindex));
        std::uint8_t dd[7] = {0};
        dd[0] = (replyinfo.channel != embot::core::tointegral(StrainChannel::all)) ? replyinfo.channel : valueindex;
        dd[1] = (true == replyinfo.valueiscalibrated) ? (1) : (0);
        dd[2] = (replyinfo.adcvalues[valueindex] >> 8) & 0xff;      // important note: the strain uses big endianess ... 
        dd[3] = replyinfo.adcvalues[valueindex] & 0xff;             

        
        std::uint8_t datalen = 4;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_CH_ADC), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }        

    bool Message_GET_EEPROM_STATUS::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::GET_EEPROM_STATUS) != frame2cmd(inframe))
        {
            return false; 
        }            

        return true;
    }  

    bool Message_GET_EEPROM_STATUS::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = (true == replyinfo.saved) ? 1 : 0;
                    
        std::uint8_t datalen = 1;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::GET_EEPROM_STATUS), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  


    bool Message_SAVE2EE::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::SAVE2EE) != frame2cmd(inframe))
        {
            return false; 
        }

        return true;
    }  

    bool Message_SAVE2EE::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        dd[0] = (true == replyinfo.ok) ? 1 : 0;
                    
        std::uint8_t datalen = 1;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::SAVE2EE), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }             
    
    
    bool Message_ACC_GYRO_SETUP::load(const embot::prot::can::Frame &inframe)
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


    bool Message_AMPLIFIER_RESET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::AMPLIFIER_RESET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);
        
        return true;         
    }                    
        
    bool Message_AMPLIFIER_RESET::reply()
    {
        return false;
    } 
    
    
    bool Message_AMPLIFIER_PGA308_CFG1_SET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::AMPLIFIER_PGA308_CFG1_SET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);
        
        // little endian
        info.cfg1.GD = static_cast<std::uint16_t>(candata.datainframe[1]) | (static_cast<std::uint16_t>(candata.datainframe[2]) << 8);  
        info.cfg1.GI = (candata.datainframe[3] & 0xF0) >> 4;
        info.cfg1.S = (candata.datainframe[3] & 0x08) >> 3;
        info.cfg1.GO = (candata.datainframe[3] & 0x07);
        info.cfg1.Voffsetcoarse = candata.datainframe[4];
        // little endian
        info.cfg1.Vzerodac = static_cast<std::uint16_t>(candata.datainframe[5]) | (static_cast<std::uint16_t>(candata.datainframe[6]) << 8);
        
        return true;         
    }                    
        
    bool Message_AMPLIFIER_PGA308_CFG1_SET::reply()
    {
        return false;
    } 



    bool Message_AMPLIFIER_PGA308_CFG1_GET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::AMPLIFIER_PGA308_CFG1_GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);

        return true;
    }  

    bool Message_AMPLIFIER_PGA308_CFG1_GET::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};

        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.channel & 0x0F);                  
        dd[1] = (replyinfo.cfg1.GD & 0xff);                  // important note: in here we use little endian order ...
        dd[2] = (replyinfo.cfg1.GD >> 8) & 0xff;  
        dd[3] = (static_cast<std::uint8_t>(replyinfo.cfg1.GI) << 4) | (static_cast<std::uint8_t>(replyinfo.cfg1.S) << 3) | (static_cast<std::uint8_t>(replyinfo.cfg1.GO) & 0x7);
        dd[4] = replyinfo.cfg1.Voffsetcoarse;
        dd[5] = (replyinfo.cfg1.Vzerodac & 0xff);            // important note: in here we use little endian order ...
        dd[6] = (replyinfo.cfg1.Vzerodac >> 8) & 0xff;  
        
        std::uint8_t datalen = 7;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::AMPLIFIER_PGA308_CFG1_GET), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    
    
    bool Message_AMPLIFIER_OFFSET_AUTOCALIB::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::AMPLIFIER_OFFSET_AUTOCALIB) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);
        info.mode = Mode::oneshot; // it would be candata.datainframe[1]
        info.target = static_cast<std::uint16_t>(candata.datainframe[2]) | (static_cast<std::uint16_t>(candata.datainframe[3]) << 8);
        info.tolerance = static_cast<std::uint16_t>(candata.datainframe[4]) | (static_cast<std::uint16_t>(candata.datainframe[5]) << 8);
        info.samples2average = candata.datainframe[6];

        return true;
    }  

    bool Message_AMPLIFIER_OFFSET_AUTOCALIB::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};

        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.channel & 0x0F);  
        dd[1] = replyinfo.noisychannelmask;
        dd[2] = replyinfo.algorithmOKmask;        
        dd[3] = replyinfo.finalmeasureOKmask;
        dd[4] = 0;
        dd[5] = (replyinfo.mae & 0x000000ff);  
        dd[6] = (replyinfo.mae & 0x0000ff00) >> 8;
          
        
        std::uint8_t datalen = 7;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::AMPLIFIER_OFFSET_AUTOCALIB), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  


    bool Message_AMPLIFIER_GAINOFFSET_SET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::AMPLIFIER_GAINOFFSET_SET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);        
        info.mode = candata.datainframe[1];
        // little endian
        info.gain = static_cast<std::uint16_t>(candata.datainframe[2]) | (static_cast<std::uint16_t>(candata.datainframe[3]) << 8);  
        // little endian
        info.offset = static_cast<std::uint16_t>(candata.datainframe[4]) | (static_cast<std::uint16_t>(candata.datainframe[5]) << 8);
        
        return true;         
    }                    
        
    bool Message_AMPLIFIER_GAINOFFSET_SET::reply()
    {
        return false;
    } 



    bool Message_AMPLIFIER_GAINOFFSET_GET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::AMPLIFIER_GAINOFFSET_GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);

        return true;
    }  

    bool Message_AMPLIFIER_GAINOFFSET_GET::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};

        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.channel & 0x0F);                  
        dd[1] = static_cast<std::uint8_t>(replyinfo.gain & 0xff);               // important note: in here we use little endian order ...
        dd[2] = static_cast<std::uint8_t>((replyinfo.gain >> 8) & 0xff);        // important note: in here we use little endian order ... 
        dd[3] = static_cast<std::uint8_t>(replyinfo.offset & 0xff);             // important note: in here we use little endian order ...
        dd[4] = static_cast<std::uint8_t>((replyinfo.offset >> 8) & 0xff);      // important note: in here we use little endian order ... 
        
        std::uint8_t datalen = 5;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::AMPLIFIER_GAINOFFSET_GET), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }  
    
    
    
    bool Message_REGULATIONSET_GET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::REGULATIONSET_GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.mode = (candata.datainframe[0] & 0x0F);

        return true;
    }  

    bool Message_REGULATIONSET_GET::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};

        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.mode & 0x0F);                  
        
        std::uint8_t datalen = 1;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::REGULATIONSET_GET), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }     
    
    bool Message_REGULATIONSET_SET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::REGULATIONSET_SET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.mode = (candata.datainframe[0] & 0x0F);        
        
        return true;         
    }                    
        
    bool Message_REGULATIONSET_SET::reply()
    {
        return false;
    } 



    bool Message_AMPLIFIER_RANGE_OF_GAIN_GET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::AMPLIFIER_RANGE_OF_GAIN_GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);

        return true;
    }  

    bool Message_AMPLIFIER_RANGE_OF_GAIN_GET::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};

        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.channel & 0x0F);                  
        dd[1] = static_cast<std::uint8_t>(replyinfo.lowest & 0xff);                 // important note: in here we use little endian order ...
        dd[2] = static_cast<std::uint8_t>((replyinfo.lowest >> 8) & 0xff);          // important note: in here we use little endian order ... 
        dd[3] = static_cast<std::uint8_t>(replyinfo.highest & 0xff);                // important note: in here we use little endian order ...
        dd[4] = static_cast<std::uint8_t>((replyinfo.highest >> 8) & 0xff);         // important note: in here we use little endian order ... 
        
        std::uint8_t datalen = 5;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::AMPLIFIER_RANGE_OF_GAIN_GET), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }     


    bool Message_AMPLIFIER_RANGE_OF_OFFSET_GET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::AMPLIFIER_RANGE_OF_OFFSET_GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.regulationset = (candata.datainframe[0] & 0xF0) >> 4;
        info.channel = (candata.datainframe[0] & 0x0F);

        return true;
    }  

    bool Message_AMPLIFIER_RANGE_OF_OFFSET_GET::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};

        dd[0] = (static_cast<std::uint8_t>(replyinfo.regulationset) << 4) | (replyinfo.channel & 0x0F);                  
        dd[1] = static_cast<std::uint8_t>(replyinfo.lowest & 0xff);                 // important note: in here we use little endian order ...
        dd[2] = static_cast<std::uint8_t>((replyinfo.lowest >> 8) & 0xff);          // important note: in here we use little endian order ... 
        dd[3] = static_cast<std::uint8_t>(replyinfo.highest & 0xff);                // important note: in here we use little endian order ...
        dd[4] = static_cast<std::uint8_t>((replyinfo.highest >> 8) & 0xff);         // important note: in here we use little endian order ... 
        
        std::uint8_t datalen = 5;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::AMPLIFIER_RANGE_OF_OFFSET_GET), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }     
    
    
    bool Message_IMU_CONFIG_SET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::IMU_CONFIG_SET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        // little endian
        info.sensormask = static_cast<std::uint16_t>(candata.datainframe[0]) | (static_cast<std::uint16_t>(candata.datainframe[1]) << 8);
        info.fusion = imuFusion::enabled;
        if(candata.datainframe[2] == static_cast<std::uint8_t>(imuFusion::none))
        {
            info.fusion = imuFusion::none;
        }
        // little endian
        std::memmove(&info.ffu_ranges_measureunits, &candata.datainframe[3], 4);
                
        return true;         
    }                    
        
    bool Message_IMU_CONFIG_SET::reply()
    {
        return false;
    }   


    bool Message_IMU_CONFIG_GET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::IMU_CONFIG_GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        return true;
    }  

    bool Message_IMU_CONFIG_GET::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};

        dd[0] = (replyinfo.sensormask & 0xff);              // important note: in here we use little endian order ...                                
        dd[1] = (replyinfo.sensormask >> 8) & 0xff; 
        dd[2] = static_cast<std::uint8_t>(replyinfo.fusion);  
        std::memmove(&dd[3], &replyinfo.ffu_ranges_measureunits, 4);  
        
        std::uint8_t datalen = 7;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::IMU_CONFIG_GET), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }


    bool Message_IMU_TRANSMIT::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::IMU_TRANSMIT) != frame2cmd(inframe))
        {
            return false; 
        }
        
        // just one byte.
        info.transmit = (0 == candata.datainframe[0]) ? false : true;
        info.txperiod = embot::core::time1millisec * static_cast<embot::core::relTime>(candata.datainframe[0]);
        return true;         
    }                    
        
    bool Message_IMU_TRANSMIT::reply()
    {
        return false;
    }       
    


    bool Message_THERMOMETER_CONFIG_SET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::THERMOMETER_CONFIG_SET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        // little endian
        info.sensormask = candata.datainframe[0];

        return true;         
    }                    
        
    bool Message_THERMOMETER_CONFIG_SET::reply()
    {
        return false;
    }   


    bool Message_THERMOMETER_CONFIG_GET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::THERMOMETER_CONFIG_GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        return true;
    }  

    bool Message_THERMOMETER_CONFIG_GET::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};

        dd[0] = replyinfo.sensormask;                                          

        std::uint8_t datalen = 1;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::THERMOMETER_CONFIG_GET), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }


    bool Message_THERMOMETER_TRANSMIT::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::THERMOMETER_TRANSMIT) != frame2cmd(inframe))
        {
            return false; 
        }
        
        // just one byte.
        info.transmit = (0 == candata.datainframe[0]) ? false : true;
        info.txperiod = embot::core::time1second * static_cast<embot::core::relTime>(candata.datainframe[0]);
        return true;         
    }                    
        
    bool Message_THERMOMETER_TRANSMIT::reply()
    {
        return false;
    } 



    
    bool Message_POS_CONFIG_SET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::POS_CONFIG_SET) != frame2cmd(inframe))
        {
            return false; 
        }
                
        uint8_t ty = candata.datainframe[0] & 0x0f;
        uint8_t ii = candata.datainframe[0] >> 4;
        
        if(ty < embot::core::tointegral(posTYPE::numberof))
        {
            info.type = static_cast<posTYPE>(ty);
        }
        else
        {
            info.type = (ty == static_cast<uint8_t>(posTYPE::none)) ? (posTYPE::none) : (posTYPE::unknown);
        }
        
        info.id = static_cast<posID>(ii);
  
        info.descriptor[0].load(&candata.datainframe[1]);
        info.descriptor[1].load(&candata.datainframe[4]);
                 
        return true;         
    }                    
        
    bool Message_POS_CONFIG_SET::reply()
    {
        return false;
    }   


    bool Message_POS_CONFIG_GET::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe); 
        
        if(static_cast<std::uint8_t>(CMD::POS_CONFIG_GET) != frame2cmd(inframe))
        {
            return false; 
        }
        
        info.type = (candata.datainframe[0] == static_cast<uint8_t>(posTYPE::angleDeciDeg)) ? (posTYPE::angleDeciDeg) : (posTYPE::unknown);
        
        return true;
    }  

    bool Message_POS_CONFIG_GET::reply(embot::prot::can::Frame &outframe, const std::uint8_t sender, const ReplyInfo &replyinfo)
    {
        std::uint8_t dd[7] = {0};
        
        dd[0] = static_cast<uint8_t>(replyinfo.type);
        replyinfo.descriptor[0].fill(&dd[1]);
        replyinfo.descriptor[1].fill(&dd[4]);

        std::uint8_t datalen = 7;
        
        frame_set_sender(outframe, sender);
        frame_set_clascmddestinationdata(outframe, Clas::pollingAnalogSensor, static_cast<std::uint8_t>(CMD::POS_CONFIG_GET), candata.from, dd, datalen);
        frame_set_size(outframe, datalen+1);
        return true;
    }


    bool Message_POS_TRANSMIT::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        
        if(static_cast<std::uint8_t>(CMD::POS_TRANSMIT) != frame2cmd(inframe))
        {
            return false; 
        }
        
        // just one byte.
        info.transmit = (0 == candata.datainframe[0]) ? false : true;
        info.txperiod = embot::core::time1millisec * static_cast<embot::core::relTime>(candata.datainframe[0]);
        return true;         
    }                    
        
    bool Message_POS_TRANSMIT::reply()
    {
        return false;
    }       
    
}}}}} // namespace embot { namespace prot { namespace can { namespace analog { namespace polling {
    
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


