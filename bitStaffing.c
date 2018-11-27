#include "bitStaffing.h"

uint32_t MajorVersion = 1;
uint32_t MinorVersion = 0;

uint32_t MaxDataLength = MAX_DATA_LENGTH;   //������������ ������ ������. ��� ����� ��������� � ���������
uint32_t MaxFrameLength = MAX_FRAME_LENGTH;  //������������ ������ ������. (� ������ ��������� � ���������)

uint8_t FrameStart = FRAME_START;  //������� ������ ������
uint8_t FrameEnd = FRAME_END;    //������� ����� ������
uint8_t StuffSign = STUFF_SIGN;   //������� ������� ������� (���� ��������� ���� �������� �� �������� �� ���������)
uint8_t StuffFactor = STUFF_FACTOR; //��������� ��� ������������� �������

//---------------------------------------------------------------------------
int32_t StuffData(void *ptrRawData, uint32_t rawDataLen, void *ptrDest, uint32_t destSize, uint32_t *resultLen)
{
  int32_t ret_code     = BUS_FAIL;
  uint8_t *work_ptr    = NULL;
  uint32_t result_len  = 0;
  uint8_t tempBuffer[MAX_DATA_LENGTH];
  uint8_t *ptrTempBuffer = tempBuffer;

  do
  {
    if( ptrRawData == NULL || rawDataLen == 0 || rawDataLen > MaxFrameLength ||
        ptrDest == NULL || destSize < 3 || destSize < rawDataLen )
    {
      ret_code = BUS_WRONG_PARAMS;
      break;
    }
    //
    work_ptr    = (uint8_t*)ptrRawData;
    memset(ptrTempBuffer, 0, MaxDataLength);
    ret_code = BUS_OK;
    //
    //��������� ������� ������ ������
    ptrTempBuffer[0] = FrameStart;
    result_len     = 1;
    //
    do
    {
      // ���� �������� ���������� ����� ������ ��������� �� ���������� ���������,
      // n�� ���������� �������������� ������ � ������ ��������������� �������
      // �����, ���� ���������� �� ����, ������ ��������� ��������� ���� � �����
      if(*work_ptr == FrameStart || *work_ptr == FrameEnd || *work_ptr == StuffSign)
      {
        ptrTempBuffer[result_len] = StuffSign;
        result_len++;
        ptrTempBuffer[result_len] = *work_ptr ^ StuffFactor;
      }
      else
      {
        ptrTempBuffer[result_len] = *work_ptr;
      }
      work_ptr++;
      result_len++;

      if(result_len > MaxDataLength)
      {
        ret_code = BUS_EXCEED_FRAME_SIZE;
        break;
      }
    } while(work_ptr < ((uint8_t*)ptrRawData + rawDataLen));
    //
    //���� ��� ������ ������, �� ��������� ������� ����� ������
    //� ��������� ������ � �� ������ � ������
    if(ret_code == BUS_OK)
    {
      if(result_len > MaxFrameLength)
      {
        ret_code = BUS_EXCEED_FRAME_SIZE;
        break;
      }

      if(result_len > destSize)
      {
        ret_code = BUS_OUT_OF_MEMORY;
        break;
      }

      ptrTempBuffer[result_len] = FrameEnd;
      result_len++;
      memcpy(ptrDest, ptrTempBuffer, result_len);
      *resultLen = result_len;
    }
  } while(0);

//  free(ptrTempBuffer);
  ptrTempBuffer = NULL;

  return ret_code;
}
//---------------------------------------------------------------------------
int32_t UnstuffData(void *ptrRawData, uint32_t rawDataLen, void *ptrDest, uint32_t destSize, uint32_t *resultLen)
{
  int32_t ret_code     = BUS_NO_FRAMES;
  uint8_t *work_ptr    = NULL;
  uint8_t pTempBuffer[BUS_MAX_ARRAY_LEN];
//  uint8_t *pTempBuffer = TempBuffer;
  uint32_t result_len  = 0;
  uint32_t frame_unstuffing = 0;  //������� ��������� ������
                                  //������� ������ ������ ��� ��� ���������

  do //while(0);
  {
    if(ptrRawData == NULL || rawDataLen < 3 || ptrDest == NULL || destSize == 0)
    {
      ret_code = BUS_WRONG_PARAMS;
      break;
    }

    memset(pTempBuffer, 0, BUS_MAX_ARRAY_LEN);
    work_ptr = (uint8_t*)ptrRawData;

    do //while(work_ptr < ((uint8_t)ptrRawData + rawDataLen));
    {
      //���� ������� ���� - ������� ������ ������...
      if(*work_ptr == FrameStart)
      {
        //"��������" ���������� ������������ �����, �.�. � ����
        //������ ���������� �� ���������� ��������� ���� �� ������!
        frame_unstuffing = 1;
        result_len       = 0;
        work_ptr++;
      }
      //
      //���� ������� ���� - ������� ����� ������
      if(*work_ptr == FrameEnd)
      {
        *resultLen = result_len;
        memcpy(ptrDest, pTempBuffer, result_len);
        ret_code = BUS_OK;
        break;  //����� �������� ���������. ������� �� �����.
      }
      //���� ��� ���� ������ ������
      if(frame_unstuffing)
      {
        //�������� �� ������������ ������ ����������
        if(result_len > destSize)
        {
          ret_code = BUS_OUT_OF_MEMORY;
          break;
        }
        //���� ����� ������� ��������������, ����������� �������
        if(*work_ptr == StuffSign)
        {
          work_ptr++;
          pTempBuffer[result_len] = *work_ptr ^ StuffFactor;
        }
       //�����, ��� ������ ��������� ����, ���������� ��� � �����...
        else
        {
          pTempBuffer[result_len] = *work_ptr;
        }
        result_len++;
      }
      //...� ��������� � ����������.
      work_ptr++;
    } while(work_ptr < ((uint8_t*)ptrRawData + rawDataLen));
  } while(0);

  return ret_code;
}
//---------------------------------------------------------------------------
//������� ������������� BitStaffing'�
void workWithBitStaffing(void)
{
  uint8_t arrayForTx[10] = {0x21, 0x22, 0xdd, 0x10, 0x24, 0xca, 0xfe, 0x11, 0xcc, 0x98};

  uint8_t arrayTx[20] = {};

  uint32_t rezLen, i;

  StuffData((uint8_t*)&arrayForTx[0], 10, (uint8_t*)&arrayTx[0], 20, &rezLen);

  printf("rezLen = %d\n", rezLen);
  for(i=0;i<rezLen;i++)
  {
    printf("%x\t", arrayTx[i]);
  }

  UnstuffData((uint8_t*)&arrayTx[0], rezLen, (uint8_t*)&arrayForTx[0], 10, &rezLen);

  printf("rezLen = %d\n", rezLen);
  for(i=0;i<rezLen;i++)
  {
    printf("%x\t", arrayTx[i]);
  }

}
