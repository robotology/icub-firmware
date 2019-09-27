#include "EOMDiagnosticRopMsg.h"

EOMDiagnosticRopMsg::EOMDiagnosticRopMsg(void* data,uint8_t size)
{
	if(size>sizeof(Info))
	{
		//TODO
		return;
	}
		
	memcpy(&data_,data,sizeof(Info));
}

uint8_t* EOMDiagnosticRopMsg::data() const
{
	return (uint8_t*)(&data_); 
}