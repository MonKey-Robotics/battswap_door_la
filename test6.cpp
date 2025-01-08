#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <thread>
#ifdef __GLIBC__
#include <sys/perm.h>
#endif

#define INDEX_IO ((short)0x4E)
#define DATA_IO ((short)0x4F)

unsigned int SMBus_Base = 0xF040;
unsigned int SMBus_SlaveAddress = 0x94;
// unsigned int SMBus_POE_SlaveAddress = 0x40;
// unsigned int SMBus_POE2_SlaveAddress = 0x42;
// unsigned int Firmware_version1, Firmware_version2, Firmware_version3,
//     Firmware_version4, Firmware_version5;  // MCU Firmware Version
// unsigned int IGN_Hour, IGN_Min, IGN_Sec;   // IGN Value
// unsigned int UPS_Hour, UPS_Min, UPS_Sec;   // UPS Vlaue
// unsigned int SystemInputVoltage_H, SystemInputVoltage_L;
unsigned int DI_Value;
unsigned int DO_Value;
// unsigned int IgnitionStatus;
// unsigned int POE_Current;
// unsigned int POE_Voltage;
char Get_Set_Check;
char Get_Check;
char Get_UPS_IGN_Check;
char Set_Check;
// char Set_UPS_IGN_Check;
// unsigned int UPS_H_Value;
// unsigned int UPS_L_Value;
// double UPS_Result = 0;
// unsigned int UPS_Result2 = 0;

//
#pragma region root
int get_io_permission(void)  // Admin
{
  if (iopl(3)) {
    fprintf(stderr, "iopl(): %s\n", strerror(errno));
    return errno;
  }

  return 0;
}
#pragma endregion
#pragma region SMBus R/W Function
unsigned int SMB_read(int PORT, int DEVICE, int REG_INDEX)  // SMB Read
{
  unsigned int SMB_Value;

  outb(0x00, PORT + 02);
  outb(0xff, PORT + 00);
  usleep(10000);
  outb(DEVICE + 1, PORT + 04);
  usleep(10000);
  outb(REG_INDEX, PORT + 03);
  usleep(10000);
  outb(0x48, PORT + 02);
  usleep(10000);
  SMB_Value = inb(PORT + 05);
  return SMB_Value;
}

unsigned int SMB_write(int PORT, int DEVICE, int REG_INDEX,
                       int REG_DATA)  // SMB Write
{
  outb(0x00, PORT + 02);
  outb(0xFF, PORT + 00);
  usleep(10000);
  outb(DEVICE, PORT + 04);
  usleep(10000);
  outb(REG_INDEX, PORT + 03);
  usleep(10000);
  outb(REG_DATA, PORT + 05);
  usleep(10000);
  outb(0x48, PORT + 02);
  usleep(10000);
}
#pragma endregion

#pragma region Linear Actuator Extend
void LA_Ext() {
  unsigned int DO_Value;
  unsigned int D1_Value;
  unsigned int D2_Value;

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D2_Value = DO_Value | 0x02;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
  printf("DO2 set to High\n");

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D1_Value = DO_Value & 0xFE;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
  printf("DO1 set to Low\n");
}
#pragma endregion

#pragma region Linear Actuator Off
void LA_Off() {
  unsigned int DO_Value;
  unsigned int D1_Value;
  unsigned int D2_Value;

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D1_Value = DO_Value & 0xFE;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
  printf("DO1 set to Low\n");

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D2_Value = DO_Value & 0xFD;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
  printf("DO2 set to Low\n");
}
#pragma endregion

#pragma region Linear Actuator Retract
void LA_Ret() {
  unsigned int DO_Value;
  unsigned int D1_Value;
  unsigned int D2_Value;

  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D2_Value = DO_Value | 0x02;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
  printf("DO2 set to High\n");

  sleep(0.1);
  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

  D1_Value = DO_Value | 0x01;
  SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
  printf("DO1 set to High\n");
}
#pragma endregion

#pragma region Set DO Function
// unsigned int Set_DO()
// {
//   char DO;
//   char DO1_Set;
//   char DO2_Set;
//   char DO3_Set;
//   char DO4_Set;
//   unsigned int DO_Value;
//   unsigned int D1_Value;
//   unsigned int D2_Value;
//   unsigned int D3_Value;
//   unsigned int D4_Value;
//   DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);
//   printf("'1':DO1\n'2':DO2\n'3':DO3\n'4':DO4\n");
//   printf("Enter: ");
//   scanf(" %c", &DO);
//   switch (DO)
//   {
//   case '1':
//     printf("Set 'H' or 'L' \n");
//     printf("Enter: ");
//     scanf(" %c", &DO1_Set);
//     if (DO1_Set == 'H')
//     {
//       D1_Value = DO_Value | 0x01;
//       SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
//       printf("DO1 set to High\n");
//     }
//     else if (DO1_Set == 'L')
//     {
//       D1_Value = DO_Value & 0xFE;
//       SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
//       printf("DO1 set to Low\n");
//     }
//     else
//     {
//       printf("Enter Error\n");
//     }
//     break;
//   case '2':
//     printf("Set 'H' or 'L' \n");
//     printf("Enter: ");
//     scanf(" %c", &DO2_Set);
//     if (DO2_Set == 'H')
//     {
//       D2_Value = DO_Value | 0x02;
//       SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
//       printf("DO2 set to High\n");
//     }
//     else if (DO2_Set == 'L')
//     {
//       D2_Value = DO_Value & 0xFD;
//       SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
//       printf("DO2 set to Low\n");
//     }
//     else
//     {
//       printf("Enter Error\n");
//     }
//     break;
//   case '3':
//     printf("Set 'H' or 'L' \n");
//     printf("Enter: ");
//     scanf(" %c", &DO3_Set);
//     if (DO3_Set == 'H')
//     {
//       D3_Value = DO_Value | 0x04;
//       SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D3_Value);
//       printf("DO3 set to High\n");
//     }
//     else if (DO3_Set == 'L')
//     {
//       D3_Value = DO_Value & 0xFB;
//       SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D3_Value);
//       printf("DO3 set to Low\n");
//     }
//     else
//     {
//       printf("Enter Error\n");
//     }
//     break;
//   case '4':
//     printf("Set 'H' or 'L' \n");
//     printf("Enter: ");
//     scanf(" %c", &DO4_Set);
//     if (DO4_Set == 'H')
//     {
//       D4_Value = DO_Value | 0x08;
//       SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D4_Value);
//       printf("DO4 set to High\n");
//     }
//     else if (DO4_Set == 'L')
//     {
//       D4_Value = DO_Value & 0xF7;
//       SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D4_Value);
//       printf("DO4 set to Low\n");
//     }
//     else
//     {
//       printf("Enter Error\n");
//     }
//     break;

//   default:
//     break;
//   }
// }

#pragma endregion
#pragma region Get DIO Status Function
unsigned int Get_DIO_Status() {
  DI_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x30);
  DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

#pragma region Get DI1~DI8
  if ((DI_Value & 0x01) == 1) {
    printf("DI 1 = HIGH \n");
  } else {
    printf("DI 1 = LOW \n");
  }
  if (((DI_Value >> 1) & 0x01) == 1) {
    printf("DI 2 = HIGH \n");
  } else {
    printf("DI 2 = LOW \n");
  }
  if (((DI_Value >> 2) & 0x01) == 1) {
    printf("DI 3 = HIGH \n");
  } else {
    printf("DI 3 = LOW \n");
  }
  if (((DI_Value >> 3) & 0x01) == 1) {
    printf("DI 4 = HIGH \n");
  } else {
    printf("DI 4 = LOW \n");
  }
  if (((DI_Value >> 4) & 0x01) == 1) {
    printf("DI 5 = HIGH \n");
  } else {
    printf("DI 5 = LOW \n");
  }
  if (((DI_Value >> 5) & 0x01) == 1) {
    printf("DI 6 = HIGH \n");
  } else {
    printf("DI 6 = LOW \n");
  }
  if (((DI_Value >> 6) & 0x01) == 1) {
    printf("DI 7 = HIGH \n");
  } else {
    printf("DI 7 = LOW \n");
  }
  if (((DI_Value >> 7) & 0x01) == 1) {
    printf("DI 8 = HIGH \n");
  } else {
    printf("DI 8 = LOW \n");
  }
#pragma endregion
  printf("-------------\n");
#pragma region Get DO1~DO4
  if ((DO_Value & 0x01) == 1) {
    printf("DO 1 = HIGH \n");
  } else {
    printf("DO 1 = LOW \n");
  }
  if (((DO_Value >> 1) & 0x01) == 1) {
    printf("DO 2 = HIGH \n");
  } else {
    printf("DO 2 = LOW \n");
  }
  if (((DO_Value >> 2) & 0x01) == 1) {
    printf("DO 3 = HIGH \n");
  } else {
    printf("DO 3 = LOW \n");
  }
  if (((DO_Value >> 3) & 0x01) == 1) {
    printf("DO 4 = HIGH \n");
  } else {
    printf("DO 4 = LOW \n");
  }

#pragma endregion
}

#pragma region int main
int main(int argc, char **argv) {
  //
  if (get_io_permission())  // Root
    return -EPERM;

  // Off
  LA_Off();
  sleep(2);

  // Extend
  LA_Ext();
  sleep(8);

  // Off
  LA_Off();
  sleep(2);

  // Retract
  LA_Ret();
  sleep(8);

  LA_Off();
  sleep(2);
}

#pragma endregion