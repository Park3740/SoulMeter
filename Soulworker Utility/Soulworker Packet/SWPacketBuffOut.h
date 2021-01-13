#pragma once
#include ".\Soulworker Packet\SWPacket.h"

typedef struct _SWPACKETBUFFOUT {
	UINT32 _playerID;
	USHORT _buffID;
	BYTE _unknwon01;
	UINT32 _giverID;
}SWPACKETBUFFOUT;

class SWPacketBuffOut : public SWPacket {
protected:
	SWPacketBuffOut() {}

public:
	SWPacketBuffOut(SWHEADER* swheader, BYTE* data);
	~SWPacketBuffOut() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};