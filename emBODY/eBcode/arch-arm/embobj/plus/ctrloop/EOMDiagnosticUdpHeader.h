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
#ifndef _EOMDIGNOSTICUDPHEADER_H_
#define _EOMDIGNOSTICUDPHEADER_H_

/** @file     EOMDiagnosticUdpHeader.h
	@brief      This header file implements public interface to the EMS diagnostic UDP header.
 	@author     luca.tricerri@iit.it
	@date       09/2019
 **/

#include "EoCommon.h"

#include <limits>

class EOMDiagnosticUdpHeader
{
	public:
		struct Info
		{
			uint32_t startCode_{33};
			uint16_t sizeOfBody_{0};
			uint16_t numberOfRops_{0};
			uint64_t sequenceNum_;
			uint64_t ageOfFrame_{0};
		};
		
		EOMDiagnosticUdpHeader(const Info& data):data_(data)
		{
				data_.sequenceNum_=sequenceNumber_;
			  increaseSequenceNumber();
		};
		EOMDiagnosticUdpHeader(){};
			
		uint8_t* data() const
		{
				return (uint8_t*)(&data_); 
		}
			
		constexpr static uint16_t getSize()
		{
				return sizeof(Info);
		}
		
		void updateHeader(uint16_t sizeOfBody,uint16_t numberOfRops,uint64_t ageOfFrame)
		{
			data_.sizeOfBody_=sizeOfBody;
			data_.numberOfRops_=numberOfRops;
			data_.ageOfFrame_=ageOfFrame;
		}
		
	private:
		Info data_;	
	  static uint64_t sequenceNumber_;
		void increaseSequenceNumber()
		{
			if(sequenceNumber_>std::numeric_limits<uint64_t>::max())
				sequenceNumber_=0;
			else
				sequenceNumber_++;
		}
};

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
