#include "pch.h"
#include ".\Soulworker Packet\SWPacketBuffIn.h"
#include ".\Buff Meter\Buff Meter.h"
#include ".\Damage Meter\Damage Meter.h"

SWPacketBuffIn::SWPacketBuffIn(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketBuffIn::Do() {

	BUFFMETER.GetLock();
	{
		SWPACKETBUFFIN* buff = (SWPACKETBUFFIN*)(_data + sizeof(SWHEADER));

		if(DAMAGEMETER.CheckPlayer(buff->_playerID))
			BUFFMETER.AddBuff(buff->_playerID, buff->_buffID, buff->_stack);
	}
	BUFFMETER.FreeLock();
}

VOID SWPacketBuffIn::Log() {

}

VOID SWPacketBuffIn::Debug() {
	SWPACKETBUFFIN* buff = (SWPACKETBUFFIN*)(_data + sizeof(SWHEADER));

	Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [BUFF IN] [PLAYER ID = %08x] [BUFF ID = %d] [BUFF STACK = %d]")), buff->_playerID, buff->_buffID, buff->_stack);
}