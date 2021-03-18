#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include "wnf.h"

const char hexes[] = "0123456789ABCDEF";

void HexDump(unsigned char *From, unsigned int Len)
{
 unsigned int i;
 int j,k;
 char buffer[256];
 char *ptr;

     for(i=0;i<Len;)
     {
          ptr = buffer;
          sprintf(ptr, "%08X ",i);
          ptr += 9;
          for(j=0;j<16 && i<Len;j++,i++)
          {
             *ptr++ = j && !(j%4)?(!(j%8)?'|':'-'):' ';
             *ptr++ = hexes[From[i] >> 4];
             *ptr++ = hexes[From[i] & 0xF];
          }
          for(k=16-j;k!=0;k--)
          {
            ptr[0] = ptr[1] = ptr[2] = ' ';
            ptr += 3;

          }
          ptr[0] = ptr[1] = ' ';
          ptr += 2;
          for(;j!=0;j--)
          {
               if(From[i-j]>=0x20)
                    *ptr = From[i-j];
               else
                    *ptr = '.';
               ptr++;
          }
          *ptr = 0;
          printf("%s\n", buffer);
     }
     printf("\n");
}

NTSTATUS NTAPI wer_wnf_cb(
    _In_     WNF_STATE_NAME   StateName,
    _In_     WNF_CHANGE_STAMP ChangeStamp,
    _In_opt_ PWNF_TYPE_ID     TypeId,
    _In_opt_ PVOID            CallbackContext,
    _In_     PVOID            Buffer,
    _In_     ULONG            BufferSize)
{
  printf("WNF_STATE_NAME %X %X buf %p len %d ctx %p\n", StateName.Data[0], StateName.Data[1], Buffer, BufferSize, CallbackContext);
  if ( Buffer != NULL && BufferSize )
    HexDump((PBYTE)Buffer, BufferSize);
  return 0;
}

extern "C"
NTSYSCALLAPI
NTSTATUS NTAPI RtlQueryWnfStateData(uint32_t*,
      WNF_STATE_NAME,
      PWNF_USER_CALLBACK,
      size_t,
      size_t);

struct wnf_id_names
{
  DWORD id1;
  DWORD id2;
  const char *const name;
} const wnf_name[] = {
#include "wnf.inc"
};


int main(int argc, char **argv)
{
    for ( size_t i = 0; i < _countof(wnf_name); i++ )
    {
      uint32_t hz = 0;
      uint32_t hz2 = 0;
      WNF_STATE_NAME state = { wnf_name[i].id1, wnf_name[i].id2 };
      NTSTATUS res = RtlQueryWnfStateData(&hz,
                                          state,
                                          wer_wnf_cb,
                                          0, 0);
      if ( res )
      {
        if ( res != 0xC0000034 ) // STATUS_OBJECT_NAME_NOT_FOUND
          printf("RtlQueryWnfStateData %s failed: %X\n", wnf_name[i].name, res);
        continue;
      }
      res = RtlSubscribeWnfStateChangeNotification(
                &hz2,
                state,
                hz,
                wer_wnf_cb,
                (PVOID)11, 0, 0, 1);
      printf("RtlSubscribeWnfStateChangeNotification %s res: %X\n", wnf_name[i].name, res);
    }
  for (;;) Sleep(100);
}
