#include "EOMDiagnosticRopMsg.h"

#include <sstream>
#include <iostream>

EOMDiagnosticRopMsg::EOMDiagnosticRopMsg(void* data,uint8_t size)
{
	if(size>sizeof(Info))
	{
		//TODO
		return;
	}
		
	std::memcpy(&data_,data,sizeof(Info));
}

EOMDiagnosticRopMsg::EOMDiagnosticRopMsg(const std::array<uint8_t,sizeof(Info)>& data)
{
	std::memcpy(&data_,data.data(),sizeof(Info));
}

uint8_t* EOMDiagnosticRopMsg::data() const
{
	return (uint8_t*)(&data_); 
}

void EOMDiagnosticRopMsg::rawdump() const
{
	std::stringstream ss;
	std::array<uint8_t,getSize()>* tmp;
	tmp=(std::array<uint8_t,getSize()>*)&data_;
	
	std::copy(tmp->begin(),tmp->end(),std::ostream_iterator<int>(ss," "));
	std::cout<<std::hex<<ss.str()<<std::endl;
}