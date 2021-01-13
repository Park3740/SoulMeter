#pragma once
#include ".\Soulworker Packet\SWPacket.h"

typedef struct _SWPACKETBUFFIN {
	UINT32 _playerID;
	USHORT _buffID;
	BYTE _unknown01[4];
	BYTE _stack;
	UINT32 _giverID;
}SWPACKETBUFFIN;

class SWPacketBuffIn : public SWPacket {
protected:
	SWPacketBuffIn() {}

public:
	SWPacketBuffIn(SWHEADER* swheader, BYTE* data);
	~SWPacketBuffIn() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};