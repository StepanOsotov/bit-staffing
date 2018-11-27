#pragma once
#ifndef BUS_H
#define BUS_H

#include "main.h"

typedef enum
{
  BUS_PACKET_INCORRECT = -7,
  BUS_MEMALLOC_FAILED,
  BUS_OUT_OF_MEMORY,
  BUS_NO_FRAMES,
  BUS_FAIL,
  BUS_EXCEED_FRAME_SIZE,
  BUS_WRONG_PARAMS,
  BUS_OK
}bus_t;


#define MAX_DATA_LENGTH   0x80
#define MAX_FRAME_LENGTH  0x100

#define FRAME_START      0x21   //<-- ������� ������ ������
#define FRAME_END        0x24   //<-- ������� ����� ������
#define STUFF_SIGN       0x22   //<-- ������� ������� ���������� ����� (���� ��� ��������
                                //    ��������� � ����� �� ��������� ��������)
#define STUFF_FACTOR     0x55   //<-- �����������, � ������� ����������� XOR ������������ �����

#define BUS_MAX_ARRAY_LEN      0x80

extern uint32_t MajorVersion;
extern uint32_t MinorVersion;

extern uint32_t MaxDataLength;   //������������ ������ ������. ��� ����� ��������� � ���������
extern uint32_t MaxFrameLength;  //������������ ������ ������. (� ������ ��������� � ���������)

extern uint8_t FrameStart;  //������� ������ ������
extern uint8_t FrameEnd;    //������� ����� ������
extern uint8_t StuffSign;   //������� ������� ������� (���� ��������� ���� �������� �� �������� �� ���������)
extern uint8_t StuffFactor; //��������� ��� ������������� �������

int32_t StuffData(void *ptrRawData, uint32_t rawDataLen, void *ptrDest, uint32_t destSize, uint32_t *resultLen);
int32_t UnstuffData(void *pData, uint32_t DataLen, void *pDst, uint32_t DstSize, uint32_t *ResultLen);
void workWithBitStaffing(void);

#endif // BUS_H
