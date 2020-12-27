#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"

SWPacket::SWPacket(SWHEADER* swheader, BYTE* data) {
	_swheader = swheader;
	_data = data;
}

VOID SWPacket::Do() {
	return;
}

VOID SWPacket::Log() {
	return;
}

VOID SWPacket::Debug() {
	
	/*
	printf("OP : %04x\tsize : %04x\n", _swheader->_op, _swheader->_size);

	
	for (int i = 0; i < _swheader->_size; i++)
		printf("%02x ", _data[i]);
	printf("\n");
	*/
	
	return;
}