/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Luca Tricerri
 * email:   luca.tricerri@iit.it
 * website: https://github.com/icub-tech-iit
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMDIGNOSTICROPMSG_H_
#define _EOMDIGNOSTICROPMSG_H_

#include "EoCommon.h"

#include <array>
#include <sstream>
#include <iostream>


/** 
  @file       EOMDiagnosticRopMsg.h
	@brief      This header file implements public interface to the EMS diagnostic message.
 	@author     luca.tricerri@iit.it
	@date       09/2019
 **/

class EOMDiagnosticRopMsg
{
	public:
		struct Info
		{
			uint16_t code_{0};
			uint16_t param1_{0};
			uint16_t param2_{0};
			uint16_t param3_{0};
			uint16_t param4_{0};
			uint16_t param5_{0};
			uint16_t dummy1_{0};
			uint16_t dummy2_{0};
			uint64_t param6_{0};
		}; EO_VERIFYsizeof(Info, 24)
		
		constexpr static uint16_t getSize(){return sizeof(Info);}
		
		EOMDiagnosticRopMsg(void* data,uint8_t size);
		EOMDiagnosticRopMsg(const std::array<uint8_t,sizeof(Info)>& data);
		EOMDiagnosticRopMsg(const Info& data):data_(data){};
		EOMDiagnosticRopMsg(){};
			
		uint8_t* data() const;	
		void rawdump() const;

	private:
		Info data_;	
};

inline EOMDiagnosticRopMsg::EOMDiagnosticRopMsg(void* data,uint8_t size)
{
	if(size>sizeof(Info))
	{
		//TODO
		return;
	}
		
	std::memcpy(&data_,data,sizeof(Info));
}

inline EOMDiagnosticRopMsg::EOMDiagnosticRopMsg(const std::array<uint8_t,sizeof(Info)>& data)
{
	std::memcpy(&data_,data.data(),sizeof(Info));
}

inline uint8_t* EOMDiagnosticRopMsg::data() const
{
	return (uint8_t*)(&data_); 
}

inline void EOMDiagnosticRopMsg::rawdump() const
{
	std::stringstream ss;
	std::array<uint8_t,getSize()>* tmp;
	tmp=(std::array<uint8_t,getSize()>*)&data_;
	
	std::copy(tmp->begin(),tmp->end(),std::ostream_iterator<int>(ss," "));
	std::cout<<std::hex<<ss.str()<<std::endl;
}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
