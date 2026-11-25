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

#include "embot_prot_can_motor_periodic.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_core_binary.h"

#include <cstring>
#include <algorithm>


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------
  
    
    
namespace embot { namespace prot { namespace can { namespace motor { namespace periodic {

    namespace {
        // little endian helpers, independent from the endianess of the platform
        constexpr std::uint16_t le2u16(const std::uint8_t *d)
        {
            return static_cast<std::uint16_t>(d[0]) | (static_cast<std::uint16_t>(d[1]) << 8);
        }

        constexpr std::uint32_t le2u32(const std::uint8_t *d)
        {
            return  static_cast<std::uint32_t>(d[0])        | (static_cast<std::uint32_t>(d[1]) << 8) |
                   (static_cast<std::uint32_t>(d[2]) << 16) | (static_cast<std::uint32_t>(d[3]) << 24);
        }

        constexpr void u16tole(std::uint16_t v, std::uint8_t *d)
        {
            d[0] = static_cast<std::uint8_t>(v & 0x00ff);
            d[1] = static_cast<std::uint8_t>((v & 0xff00) >> 8);
        }
        
        constexpr void u32tole(std::uint32_t v, std::uint8_t *d)
        {
            d[0] = static_cast<std::uint8_t>(v & 0x000000ff);
            d[1] = static_cast<std::uint8_t>((v & 0x0000ff00) >> 8);
            d[2] = static_cast<std::uint8_t>((v & 0x00ff0000) >> 16);
            d[3] = static_cast<std::uint8_t>((v & 0xff000000) >> 24);
        }
        
        // it forms a frame of class periodicMotorControl: cmd goes in the ID, data[] holds only the payload 
        bool formperiodic(embot::prot::can::Frame &outframe, embot::prot::can::Address sender, CMD cmd, const std::uint8_t *data, std::uint8_t size)
        {
            outframe.clear();
            frame_set_sender(outframe, sender);
            frame_set_clascmddestinationdata(outframe, Clas::periodicMotorControl, convert(cmd), 0xf, data, size);
            frame_set_size(outframe, size);
            return true;
        }
        
        // it tells if a frame is of class periodicMotorControl w/ the given cmd and at least minsize bytes of payload
        bool isperiodic(const embot::prot::can::Frame &inframe, CMD cmd, std::uint8_t minsize)
        {
            return (Clas::periodicMotorControl == frame2clas(inframe)) && 
                   (convert(cmd) == frame2cmd(inframe)) && 
                   (frame2datasize(inframe) >= minsize);
        }
    }

    bool supported(std::uint8_t cmd)
    {
        return (CMD::none != convert(cmd))? true : false;
    }
        
    CMD convert(std::uint8_t cmd)
    {
        constexpr std::uint16_t mcpermask16 =   (1 << static_cast<std::uint8_t>(CMD::FOC))                              |
                                                (1 << static_cast<std::uint8_t>(CMD::STATUS))                           |
                                                (1 << static_cast<std::uint8_t>(CMD::PRINT))                            |
                                                (1 << static_cast<std::uint8_t>(CMD::DEBUG))                            |
                                                (1 << static_cast<std::uint8_t>(CMD::ADDITIONAL_STATUS))                |    
                                                (1 << static_cast<std::uint8_t>(CMD::EMSTO2FOC_DESIRED_CURRENT))        ;

        if(cmd > 15)
        {
            return CMD::none;
        }

        if(true == embot::core::binary::bit::check(mcpermask16, cmd))
        {
            return static_cast<CMD>(cmd);          
        }
        
        return CMD::none;           
    } 

    std::uint8_t convert(CMD cmd)
    {
        return static_cast<std::uint8_t>(cmd);
    }      
    
    
    std::uint8_t Message_PRINT::textIDmod4 = 0;
    
    bool Message_PRINT::load(const Info& inf)
    {
        info = inf;  
        nchars = std::strlen(info.text);
        nframes = (std::strlen(info.text) + 5) / 6;   
        framecounter = 0;  
        textIDmod4++;
        textIDmod4 %= 4;
        
        return (nframes>0) ? true : false;
    }
    
    std::uint8_t Message_PRINT::numberofframes()
    {       
        return nframes;
    }
        
    bool Message_PRINT::get(embot::prot::can::Frame &outframe)
    {
        if(false == Sender::form(outframe, info, framecounter, textIDmod4, info.canaddress))
        {
            return false;
        }
        framecounter ++;
        Message::set(outframe);
        return true;
    } 


    bool Message_DEBUG::load(const Info& inf)
    {
        info = inf;  
        return true;
    }
            
    bool Message_DEBUG::get(embot::prot::can::Frame &outframe)
    {
        Sender::form(outframe, info, info.canaddress);
        Message::set(outframe);
        return true;
    } 
    
    bool Message_EMSTO2FOC_DESIRED_CURRENT::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        return Receiver::parse(info, inframe);
    }

    bool Message_EMSTO2FOC_DESIRED_CURRENT::load(const Info& inf)
    {
        info = inf;
        return true;
    }

    bool Message_EMSTO2FOC_DESIRED_CURRENT::get(embot::prot::can::Frame &outframe)
    {
        Sender::form(outframe, info, info.canaddress);
        Message::set(outframe);
        return true;
    }


    bool Message_FOC::load(const Info& inf)
    {
        info = inf;  
        return true;
    }
    

        
    bool Message_FOC::get(embot::prot::can::Frame &outframe)
    {
        Sender::form(outframe, info, info.canaddress);
        Message::set(outframe);
        return true;
    }

    bool Message_FOC::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        return Receiver::parse(info, inframe);
    }
 
    
    bool Message_STATUS::load(const Info& inf)
    {
        info = inf;  
        return true;
    }
            
    bool Message_STATUS::get(embot::prot::can::Frame &outframe)
    {
        Sender::form(outframe, info, info.canaddress);
        Message::set(outframe);
        return true;
    }

    bool Message_STATUS::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        return Receiver::parse(info, inframe);
    }

    bool Message_ADDITIONAL_STATUS::load(const Info& inf)
    {
        info = inf;  
        return true;
    }
            
    bool Message_ADDITIONAL_STATUS::get(embot::prot::can::Frame &outframe)
    {
        Sender::form(outframe, info, info.canaddress);
        Message::set(outframe);
        return true;
    }

    bool Message_ADDITIONAL_STATUS::load(const embot::prot::can::Frame &inframe)
    {
        Message::set(inframe);  
        return Receiver::parse(info, inframe);
    }
    

    

    // - the Sender / Receiver of Message_DEBUG
    //   the payload is the 8 bytes of the uint64_t in the memory layout of the platform (as the legacy get() does)
    
    bool Message_DEBUG::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address sender)
    {
        constexpr std::uint8_t size {8};
        std::uint8_t data08[size] = {0};
        std::memmove(data08, &info.payload, size);
        return formperiodic(outframe, sender, CMD::DEBUG, data08, size);
    }
    
    bool Message_DEBUG::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(false == isperiodic(inframe, CMD::DEBUG, 8))
        {
            return false;
        }
        info.canaddress = frame2sender(inframe);
        std::memmove(&info.payload, inframe.data, sizeof(info.payload));
        return true;
    }
    
    // - the Sender / Receiver of Message_EMSTO2FOC_DESIRED_CURRENT (alias Message_TARGETS)
    
    bool Message_EMSTO2FOC_DESIRED_CURRENT::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address sender)
    {
        constexpr std::uint8_t size {8};
        std::uint8_t data08[size] = {0};
        // [target0:2][target1:2][target2:2][target3:2], little endian
        for(std::uint8_t i=0; i<Info::numberoftargets; i++)
        {
            u16tole(static_cast<std::uint16_t>(info.target[i]), &data08[2*i]);
        }
        return formperiodic(outframe, sender, CMD::EMSTO2FOC_DESIRED_CURRENT, data08, size);
    }
    
    bool Message_EMSTO2FOC_DESIRED_CURRENT::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(false == isperiodic(inframe, CMD::EMSTO2FOC_DESIRED_CURRENT, 8))
        {
            return false;
        }
        info.canaddress = frame2sender(inframe);
        for(std::uint8_t i=0; i<Info::numberoftargets; i++)
        {
            info.target[i] = static_cast<std::int16_t>(le2u16(&inframe.data[2*i]));
        }
        return true;
    }
    
    
    // - the Sender / Receiver of Message_FOC
    
    bool Message_FOC::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address sender)
    {
        constexpr std::uint8_t size {8};
        std::uint8_t data08[size] = {0};
        // [current:2][velocity:2][position:4], little endian
        u16tole(static_cast<std::uint16_t>(info.current), &data08[0]);
        u16tole(static_cast<std::uint16_t>(info.velocity), &data08[2]);
        u32tole(static_cast<std::uint32_t>(info.position), &data08[4]);
        return formperiodic(outframe, sender, CMD::FOC, data08, size);
    }
    
    bool Message_FOC::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(false == isperiodic(inframe, CMD::FOC, 8))
        {
            return false;
        }
        info.canaddress = frame2sender(inframe);
        info.current = static_cast<embot::prot::can::motor::Current>(le2u16(&inframe.data[0]));
        info.velocity = static_cast<embot::prot::can::motor::Velocity>(le2u16(&inframe.data[2]));
        info.position = static_cast<embot::prot::can::motor::PositionFull>(le2u32(&inframe.data[4]));
        return true;
    }
    
    
    // - the Sender / Receiver of Message_STATUS
    
    bool Message_STATUS::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address sender)
    {
        constexpr std::uint8_t size {8};
        std::uint8_t data08[size] = {0};
        // [controlmode:1][quadencoderstate:1][pwmfeedback:2][motorfaultstate:4], little endian
        data08[0] = info.controlmode;
        data08[1] = info.quadencoderstate;
        u16tole(static_cast<std::uint16_t>(info.pwmfeedback), &data08[2]);
        u32tole(info.motorfaultstate, &data08[4]);
        return formperiodic(outframe, sender, CMD::STATUS, data08, size);
    }
    
    bool Message_STATUS::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(false == isperiodic(inframe, CMD::STATUS, 8))
        {
            return false;
        }
        info.canaddress = frame2sender(inframe);
        info.controlmode = inframe.data[0];
        info.quadencoderstate = inframe.data[1];
        info.pwmfeedback = static_cast<std::int16_t>(le2u16(&inframe.data[2]));
        info.motorfaultstate = le2u32(&inframe.data[4]);
        return true;
    }
    
    
    // - the Sender / Receiver of Message_ADDITIONAL_STATUS
    
    bool Message_ADDITIONAL_STATUS::Sender::form(embot::prot::can::Frame &outframe, const Info &info, Address sender)
    {
        constexpr std::uint8_t size {4};
        std::uint8_t data08[size] = {0};
        // [tbd:2][temperature:2], little endian
        data08[0] = info.tbd[0];
        data08[1] = info.tbd[1];
        u16tole(static_cast<std::uint16_t>(info.temperature), &data08[2]);
        return formperiodic(outframe, sender, CMD::ADDITIONAL_STATUS, data08, size);
    }
    
    bool Message_ADDITIONAL_STATUS::Receiver::parse(Info &info, const embot::prot::can::Frame &inframe)
    {
        if(false == isperiodic(inframe, CMD::ADDITIONAL_STATUS, 4))
        {
            return false;
        }
        info.canaddress = frame2sender(inframe);
        info.tbd[0] = inframe.data[0];
        info.tbd[1] = inframe.data[1];
        info.temperature = static_cast<std::int16_t>(le2u16(&inframe.data[2]));
        return true;
    }
    
    
    // - the Sender / Receiver of Message_PRINT
    //   every frame is: [CMD::PRINT | 0x80 if last:1][textID << 4 | frameindex:1][chars:0-6]
    
    std::uint8_t Message_PRINT::Sender::numberofframes(const Info &info)
    {
        const std::uint8_t nchars = static_cast<std::uint8_t>(strnlen(info.text, maxchars));
        return (nchars + charsperframe - 1) / charsperframe;
    }
    
    bool Message_PRINT::Sender::form(embot::prot::can::Frame &outframe, const Info &info, std::uint8_t frameindex, std::uint8_t textID, Address sender)
    {
        const std::uint8_t nframes = numberofframes(info);
        if(frameindex >= nframes)
        {
            return false;
        }
        
        const std::uint8_t nchars = static_cast<std::uint8_t>(strnlen(info.text, maxchars));
        const bool last = ((frameindex+1) == nframes);
        const std::uint8_t offset = charsperframe*frameindex;
        const std::uint8_t charsinframe = std::min(static_cast<std::uint8_t>(nchars - offset), charsperframe);
        
        std::uint8_t data08[8] = {0};
        data08[0] = convert(CMD::PRINT) | (last ? 0x80 : 0x00);
        data08[1] = ((textID << 4) & 0xF0) | (frameindex & 0x0F);
        std::memmove(&data08[2], &info.text[offset], charsinframe);
        
        return formperiodic(outframe, sender, CMD::PRINT, data08, 2+charsinframe);
    }
    
    bool Message_PRINT::Receiver::parse(Fragment &fragment, const embot::prot::can::Frame &inframe)
    {
        if(false == isperiodic(inframe, CMD::PRINT, 2))
        {
            return false;
        }
        fragment.canaddress = frame2sender(inframe);
        fragment.last = (0x80 == (inframe.data[0] & 0x80));
        fragment.textID = (inframe.data[1] >> 4) & 0x0F;
        fragment.index = inframe.data[1] & 0x0F;
        fragment.nchars = std::min(static_cast<std::uint8_t>(frame2datasize(inframe) - 2), charsperframe);
        std::memset(fragment.chars, 0, sizeof(fragment.chars));
        std::memmove(fragment.chars, &inframe.data[2], fragment.nchars);
        return true;
    }
    
    void Message_PRINT::Receiver::Assembler::reset()
    {
        std::memset(buffer, 0, sizeof(buffer));
        length = 0;
        expectedindex = 0;
        textID = 0;
        canaddress = 0;
        active = false;
    }
    
    bool Message_PRINT::Receiver::Assembler::add(const Fragment &fragment, Info &info)
    {
        if((fragment.index >= maxframes) || (fragment.nchars > charsperframe))
        {
            reset();
            return false;
        }
        
        if(0 == fragment.index)
        {   // a new text begins and it discards any text in progress
            reset();
            active = true;
            textID = fragment.textID;
            canaddress = fragment.canaddress;
        }
        else if((!active) || (fragment.textID != textID) || (fragment.canaddress != canaddress) || (fragment.index != expectedindex))
        {   // out of sequence: we have lost at least one frame
            reset();
            return false;
        }
        
        const std::uint8_t offset = charsperframe*fragment.index;
        std::memmove(&buffer[offset], fragment.chars, fragment.nchars);
        length = offset + fragment.nchars;
        expectedindex = fragment.index + 1;
        
        if(fragment.last)
        {
            std::memset(info.text, 0, sizeof(info.text));
            std::memmove(info.text, buffer, length);
            info.canaddress = canaddress;
            reset();
            return true;
        }
        
        return false;
    }
    
}}}}} // namespace embot { namespace prot { namespace can { namespace motor { namespace periodic {
    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


