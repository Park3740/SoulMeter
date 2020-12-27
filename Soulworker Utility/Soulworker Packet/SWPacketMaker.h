#pragma once
#include ".\Soulworker Packet\PacketType.h"

#define SWPACKETMAKER SWPacketMaker::getInstance()

#define NO_SEGMENTATION 0
#define YES_SEGMENTATION 1
#define REASSAMBLY 2
#define NO_SWHEADER 3
#define NO_SWDATA 4
#define SEGMENTATION_SUCCESS 5
#define SEGMENTATION_FAILED 6

#define DEBUG_SEGMENTATION 0
#define DEBUG_CREATEPACKET 0
#define DEBUG_RESIZEPACKET 0

class SWPacketMaker : public Singleton<SWPacketMaker> {
private:
	class SegmentationPacket {
	private:
		BYTE* _data;
		SIZE_T _size;

		SIZE_T _curSize;
		SIZE_T _packetSize;

		IPv4Packet _packet;

	public:
		SegmentationPacket() {
			_data = nullptr;
			_size = 0;
			_curSize = 0;
			_packetSize = 0;
		}

		~SegmentationPacket() {

			if (_data != nullptr)
				delete[] _data;

			_data = nullptr;
		}

		VOID Init(SIZE_T size) {

			if (_data == nullptr) {
				_data = new BYTE[size];
				_size = size;
			}

			if (_size < size) {
				delete[] _data;
				_data = new BYTE[size];
				_size = size;
			}

			_curSize = 0;
			_packetSize = size;

#if DEBUG_SEGMENTATION == 1
			Log::WriteLogA(const_cast<CHAR*>("[segmentationPacket Init] [curSize = %d] [packetSize = %d]"), _curSize, _packetSize);
#endif

		}

		SIZE_T GetRemainSize() {
#if DEBUG_SEGMENTATION == 1
			Log::WriteLogA(const_cast<CHAR*>("[segmentationPacket RemainSize = %d]"), _packetSize - _curSize);
#endif

			return _packetSize - _curSize;
		}

		DWORD AppendData(BYTE* data, SIZE_T size) {

			memcpy(_data + _curSize, data, size);

			int temp_debug = _curSize;
			_curSize += size;

#if DEBUG_SEGMENTATION == 1
			Log::WriteLogA(const_cast<CHAR*>("[segmentationPacket AppendData Before curSize = %d After curSize = %d]"), temp_debug, _curSize);

			for (int i = temp_debug; i < _curSize; i++)
				printf("%02x ", _data[i]);
			printf("\n");
#endif

			if (_curSize == _packetSize)
				return SEGMENTATION_SUCCESS;
			else if (_curSize < _packetSize)
				return SEGMENTATION_FAILED;
		}

		IPv4Packet* GetData() {
			_packet._data = _data;
			_packet._datalength = _packetSize;

			return &_packet;
		}
	};

	VOID Decrypt(BYTE* data, const UINT size, const UINT start);
	CHAR _keyTable[3] = { 0x60, 0x3B, 0x0B };

	SWHEADER* GetSWHeader(IPv4Packet* packet);
	BYTE* GetSWData(IPv4Packet* packet);

	VOID ResizePacket(IPv4Packet* packet);
	VOID ResizePacket(SIZE_T remainsize, IPv4Packet* packet);

	DWORD CheckSegmentation(IPv4Packet* packet);
	BOOL _isSegmentation;

	VOID StartReassambly(IPv4Packet* packet);
	VOID Reassambly(IPv4Packet* packet);
	SegmentationPacket _segmentationPacket;

	VOID CreateSWPacket(IPv4Packet* packet);

	VOID CheckRemainPacket(IPv4Packet* packet);

public:
	SWPacketMaker();
	~SWPacketMaker();

	DWORD Parse(IPv4Packet* packet);
};