/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Luca Tricerri
 * email:   luca.tricerri@iit.it
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


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOMDIGNOSTICUDPMSG_H_
#define _EOMDIGNOSTICUDPMSG_H_

/** @file     EOMDiagnosticUdpMsg.h
	@brief      This header file implements public interface to the EMS diagnostic UDP msg.
 	@author     luca.tricerri@iit.it
	@date       09/2019
 **/


#include "EoCommon.h"

#include <array>

#include "EOMDiagnosticRopMsg.h"
#include "EOMDiagnosticUdpHeader.h"
#include "EOMDiagnosticUdpHeader.h"

class EOMDiagnosticUdpMsg
{
	public:
		struct Info
		{
			EOMDiagnosticUdpHeader* header_;
			std::array<EOMDiagnosticRopMsg,1> body_;
			EOMDiagnosticUdpHeader* footer_;
		};
		
		EOMDiagnosticUdpMsg(const Info& data):data_(data){};
		EOMDiagnosticUdpMsg(){};
			
		uint8_t* data() const
		{
				return (uint8_t*)(&data_); 
		}
		constexpr static uint16_t getSize()
		{
				return sizeof(Info);
		}			
		
	private:
		Info data_;	
};

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
