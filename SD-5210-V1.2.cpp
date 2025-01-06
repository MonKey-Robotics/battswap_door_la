#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#ifdef __GLIBC__
#  include <sys/perm.h>
#endif

#define INDEX_IO ((short)0x4E)
#define DATA_IO ((short)0x4F)

unsigned int SMBus_Base = 0xF040;
unsigned int SMBus_SlaveAddress = 0x94;
unsigned int SMBus_POE_SlaveAddress = 0x40;
unsigned int SMBus_POE2_SlaveAddress = 0x42;
unsigned int Firmware_version1, Firmware_version2, Firmware_version3, Firmware_version4, Firmware_version5; //MCU Firmware Version
unsigned int IGN_Hour, IGN_Min, IGN_Sec; //IGN Value
unsigned int UPS_Hour, UPS_Min, UPS_Sec; //UPS Vlaue
unsigned int SystemInputVoltage_H, SystemInputVoltage_L;
unsigned int DI_Value;
unsigned int DO_Value;
unsigned int IgnitionStatus;
unsigned int POE_Current;
unsigned int POE_Voltage;
char Get_Set_Check;
char Get_Check;
char Get_UPS_IGN_Check;
char Set_Check;
char Set_UPS_IGN_Check;
unsigned int UPS_H_Value;
unsigned int UPS_L_Value;
double UPS_Result = 0;
unsigned int UPS_Result2 = 0;

//
#pragma region root
int get_io_permission(void) //Admin
{
	if (iopl(3)) {
		fprintf(stderr, "iopl(): %s\n", strerror(errno));
		return errno;
	}

	return 0;
}
#pragma endregion
#pragma region SMBus R/W Function
unsigned int SMB_read(int PORT, int DEVICE, int REG_INDEX) //SMB Read
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

unsigned int SMB_write(int PORT, int DEVICE, int REG_INDEX, int REG_DATA) //SMB Write
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
//

//
#pragma region Set IGN Delay Times Function
unsigned int Set_IGN_Delay_Time()
{
	char IGN_Delay;
	unsigned int IGN_Set_Hour;
	unsigned int IGN_Set_Min;
	unsigned int IGN_Set_Sec;

	printf("'H':Hour\n'M':Min\n'S':Sec\n");
	printf("Enter: ");
	scanf(" %c", &IGN_Delay);
	switch (IGN_Delay)
	{
	case 'H':
		printf("Set IGN Hour 0~23\n");
		printf("Enter: ");
		scanf(" %u", &IGN_Set_Hour);
		if (IGN_Set_Hour > 23)
		{
			printf(" 'Error' Hour times is 0~23\n");
		}
		else
		{
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x68, IGN_Set_Hour);
		}
		break;
	case 'M':
		printf("Set IGN Min 0~59\n");
		printf("Enter: ");
		scanf(" %u", &IGN_Set_Min);
		if (IGN_Set_Min > 59)
		{
			printf(" 'Error' Hour times is 0~59\n");
		}
		else
		{
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x67, IGN_Set_Min);
		}
		break;

		break;
	case 'S':
		printf("Set IGN Sec 0~59\n");
		printf("Enter: ");
		scanf(" %u", &IGN_Set_Sec);
		if (IGN_Set_Sec > 59)
		{
			printf(" 'Error' Hour times is 0~59\n");
		}
		else
		{
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x66, IGN_Set_Sec);
		}
		break;
		break;

	default:
		break;
	}
}
#pragma endregion
#pragma region Set UPS Delay Times Function
unsigned int Set_UPS_Delay_Time()
{
	char UPS_Delay;
	unsigned int UPS_Set_Hour;
	unsigned int UPS_Set_Min;
	unsigned int UPS_Set_Sec;

	printf("'H':Hour\n'M':Min\n'S':Sec\n");
	printf("Enter: ");
	scanf(" %c", &UPS_Delay);
	switch (UPS_Delay)
	{
	case 'H':
		printf("Set UPS Hour 0~23\n");
		printf("Enter: ");
		scanf(" %u", &UPS_Set_Hour);
		if (UPS_Set_Hour > 23)
		{
			printf(" 'Error' Hour times is 0~23\n");
		}
		else
		{
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x74, UPS_Set_Hour);
		}
		break;
	case 'M':
		printf("Set UPS Min 0~59\n");
		printf("Enter: ");
		scanf(" %u", &UPS_Set_Min);
		if (UPS_Set_Min > 59)
		{
			printf(" 'Error' Hour times is 0~59\n");
		}
		else
		{
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x73, UPS_Set_Min);
		}
		break;

		break;
	case 'S':
		printf("Set UPS Sec 0~59\n");
		printf("Enter: ");
		scanf(" %u", &UPS_Set_Sec);
		if (UPS_Set_Sec > 59)
		{
			printf(" 'Error' Hour times is 0~59\n");
		}
		else
		{
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x72, UPS_Set_Sec);
		}
		break;
		break;

	default:
		break;
	}
}
#pragma endregion
#pragma region Set DO Function
unsigned int Set_DO()
{
	char DO;
	char DO1_Set;
	char DO2_Set;
	char DO3_Set;
	char DO4_Set;
	unsigned int DO_Value;
	unsigned int D1_Value;
	unsigned int D2_Value;
	unsigned int D3_Value;
	unsigned int D4_Value;
	DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);
	printf("'1':DO1\n'2':DO2\n'3':DO3\n'4':DO4\n");
	printf("Enter: ");
	scanf(" %c", &DO);
	switch (DO)
	{
	case '1':
		printf("Set 'H' or 'L' \n");
		printf("Enter: ");
		scanf(" %c", &DO1_Set);
		if (DO1_Set == 'H')
		{
			D1_Value = DO_Value | 0x01;
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
			printf("DO1 set to High\n");
		}
		else if (DO1_Set == 'L')
		{
			D1_Value = DO_Value & 0xFE;
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D1_Value);
			printf("DO1 set to Low\n");
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '2':
		printf("Set 'H' or 'L' \n");
		printf("Enter: ");
		scanf(" %c", &DO2_Set);
		if (DO2_Set == 'H')
		{
			D2_Value = DO_Value | 0x02;
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
			printf("DO2 set to High\n");
		}
		else if (DO2_Set == 'L')
		{
			D2_Value = DO_Value & 0xFD;
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D2_Value);
			printf("DO2 set to Low\n");
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '3':
		printf("Set 'H' or 'L' \n");
		printf("Enter: ");
		scanf(" %c", &DO3_Set);
		if (DO3_Set == 'H')
		{
			D3_Value = DO_Value | 0x04;
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D3_Value);
			printf("DO3 set to High\n");
		}
		else if (DO3_Set == 'L')
		{
			D3_Value = DO_Value & 0xFB;
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D3_Value);
			printf("DO3 set to Low\n");
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '4':
		printf("Set 'H' or 'L' \n");
		printf("Enter: ");
		scanf(" %c", &DO4_Set);
		if (DO4_Set == 'H')
		{
			D4_Value = DO_Value | 0x08;
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D4_Value);
			printf("DO4 set to High\n");
		}
		else if (DO4_Set == 'L')
		{
			D4_Value = DO_Value & 0xF7;
			SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x31, D4_Value);
			printf("DO4 set to Low\n");
		}
		else
		{
			printf("Enter Error\n");
		}
		break;

	default:
		break;
	}
}
#pragma endregion
#pragma region Set POE ON & OFF Function
unsigned int Set_POE()
{
	char POE;
	char POE1;
	char POE2;
	char POE3;
	char POE4;
	char POE5;
	char POE6;
	char POE7;
	char POE8;
	unsigned int POE1_On_Off, POE1_Detect;
	unsigned int POE2_On_Off, POE2_Detect;
	unsigned int POE3_On_Off, POE3_Detect;
	unsigned int POE4_On_Off, POE4_Detect;
	unsigned int POE5_On_Off, POE5_Detect;
	unsigned int POE6_On_Off, POE6_Detect;
	unsigned int POE7_On_Off, POE7_Detect;
	unsigned int POE8_On_Off, POE8_Detect;
	printf("'1':LAN3/POE\n'2':LAN4/POE\n'3':LAN5/POE\n'4':LAN6/POE\n'5':LAN7/POE\n'6':LAN8/POE\n'7':LAN9/POE\n'8':LAN10/POE\n");
	printf("Enter: ");
	scanf(" %c", &POE);
	switch (POE)
	{
	case '1':
		printf("Set 'N': LAN3/POE is ON \nSet 'F': LAN3/POE is OFF \n");
		printf("Enter: ");
		scanf(" %c", &POE1);
		if (POE1 == 'N')
		{
			POE1_Detect = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x14);
			POE1_Detect = (POE1_Detect & 0xFE) | 0x01;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x14, POE1_Detect);
			POE1_On_Off = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x19);
			POE1_On_Off = (POE1_On_Off & 0xEE) | 0x01;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x19, POE1_On_Off);
		}
		else if (POE1 == 'F')
		{
			POE1_On_Off = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x19);
			POE1_On_Off = (POE1_On_Off & 0xEE) | 0x10;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x19, POE1_On_Off);
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '2':
		printf("Set 'N': LAN4/POE is ON \nSet 'F': LAN4/POE is OFF \n");
		printf("Enter: ");
		scanf(" %c", &POE2);
		if (POE2 == 'N')
		{
			POE2_Detect = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x14);
			POE2_Detect = (POE2_Detect & 0xFD) | 0x02;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x14, POE2_Detect);
			POE2_On_Off = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x19);
			POE2_On_Off = (POE2_On_Off & 0xDD) | 0x02;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x19, POE2_On_Off);
		}
		else if (POE2 == 'F')
		{
			POE2_On_Off = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x19);
			POE2_On_Off = (POE2_On_Off & 0xDD) | 0x20;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x19, POE2_On_Off);
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '3':
		printf("Set 'N': LAN5/POE is ON \nSet 'F': LAN5/POE is OFF \n");
		printf("Enter: ");
		scanf(" %c", &POE3);
		if (POE3 == 'N')
		{
			POE3_Detect = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x14);
			POE3_Detect = (POE3_Detect & 0xFB) | 0x04;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x14, POE3_Detect);
			POE3_On_Off = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x19);
			POE3_On_Off = (POE3_On_Off & 0xBB) | 0x04;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x19, POE3_On_Off);
		}
		else if (POE3 == 'F')
		{
			POE3_On_Off = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x19);
			POE3_On_Off = (POE3_On_Off & 0xBB) | 0x40;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x19, POE3_On_Off);
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '4':
		printf("Set 'N': LAN6/POE is ON \nSet 'F': LAN6/POE is OFF \n");
		printf("Enter: ");
		scanf(" %c", &POE4);
		if (POE4 == 'N')
		{
			POE4_Detect = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x14);
			POE4_Detect = (POE4_Detect & 0xF7) | 0x08;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x14, POE4_Detect);
			POE4_On_Off = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x19);
			POE4_On_Off = (POE4_On_Off & 0x77) | 0x08;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x19, POE4_On_Off);
		}
		else if (POE4 == 'F')
		{
			POE4_On_Off = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, 0x19);
			POE4_On_Off = (POE4_On_Off & 0x77) | 0x80;
			SMB_write(SMBus_Base, SMBus_POE_SlaveAddress, 0x19, POE4_On_Off);
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '5':
		printf("Set 'N': LAN7/POE is ON \nSet 'F': LAN7/POE is OFF \n");
		printf("Enter: ");
		scanf(" %c", &POE5);
		if (POE5 == 'N')
		{
			POE5_Detect = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x14);
			POE5_Detect = (POE5_Detect & 0xFE) | 0x01;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x14, POE5_Detect);
			POE5_On_Off = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19);
			POE5_On_Off = (POE5_On_Off & 0xEE) | 0x01;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19, POE5_On_Off);
		}
		else if (POE5 == 'F')
		{
			POE5_On_Off = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19);
			POE5_On_Off = (POE5_On_Off & 0xEE) | 0x10;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19, POE5_On_Off);
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '6':
		printf("Set 'N': LAN8/POE is ON \nSet 'F': LAN8/POE is OFF \n");
		printf("Enter: ");
		scanf(" %c", &POE6);
		if (POE6 == 'N')
		{
			POE6_Detect = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x14);
			POE6_Detect = (POE6_Detect & 0xFD) | 0x02;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x14, POE6_Detect);
			POE6_On_Off = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19);
			POE6_On_Off = (POE6_On_Off & 0xDD) | 0x02;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19, POE6_On_Off);
		}
		else if (POE6 == 'F')
		{
			POE6_On_Off = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19);
			POE6_On_Off = (POE6_On_Off & 0xDD) | 0x20;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19, POE6_On_Off);
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '7':
		printf("Set 'N': LAN9/POE is ON \nSet 'F': LAN9/POE is OFF \n");
		printf("Enter: ");
		scanf(" %c", &POE7);
		if (POE7 == 'N')
		{
			POE7_Detect = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x14);
			POE7_Detect = (POE7_Detect & 0xFB) | 0x04;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x14, POE7_Detect);
			POE7_On_Off = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19);
			POE7_On_Off = (POE7_On_Off & 0xBB) | 0x04;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19, POE7_On_Off);
		}
		else if (POE7 == 'F')
		{
			POE7_On_Off = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19);
			POE7_On_Off = (POE7_On_Off & 0xBB) | 0x40;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19, POE7_On_Off);
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	case '8':
		printf("Set 'N': LAN10/POE is ON \nSet 'F': LAN10/POE is OFF \n");
		printf("Enter: ");
		scanf(" %c", &POE8);
		if (POE8 == 'N')
		{
			POE8_Detect = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x14);
			POE8_Detect = (POE8_Detect & 0xF7) | 0x08;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x14, POE8_Detect);
			POE8_On_Off = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19);
			POE8_On_Off = (POE8_On_Off & 0x77) | 0x08;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19, POE8_On_Off);
		}
		else if (POE8 == 'F')
		{
			POE8_On_Off = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19);
			POE8_On_Off = (POE8_On_Off & 0x77) | 0x80;
			SMB_write(SMBus_Base, SMBus_POE2_SlaveAddress, 0x19, POE8_On_Off);
		}
		else
		{
			printf("Enter Error\n");
		}
		break;
	default:
		break;
	}
}
#pragma endregion
#pragma region 3G/LTE Function
unsigned int Modem()
{
	char Modem;
	unsigned int Reset_3G, SIM1, SIM2;
	printf("'R':Reset 3G/LTE Module \n'1':Change to SIM1\n'2':Change to SIM2\n");
	printf("Enter: ");
	scanf(" %c", &Modem);
	switch (Modem)
	{
	case 'R':
		Reset_3G = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x39);
		Reset_3G = Reset_3G & 0xFD;
		SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x39, Reset_3G);
		break;
	case '1':
		//change to SIM1
		SIM1 = SMB_read(0xF040, 0x94, 0x39);
		SIM1 = (SIM1 & 0xFE) | 0x01;
		SMB_write(0xF040, 0x94, 0x39, SIM1);
		sleep(2000);
		//change to SIM1

		//module reset
		Reset_3G = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x39);
		Reset_3G = Reset_3G & 0xFD;
		SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x39, Reset_3G);
		//module reset
		break;
	case '2':
		//change to SIM2
		SIM2 = SMB_read(0xF040, 0x94, 0x39);
		SIM2 = SIM2 & 0xFE;
		SMB_write(0xF040, 0x94, 0x39, SIM2);
		sleep(2000);
		//change to SIM1

		//module reset
		Reset_3G = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x39);
		Reset_3G = Reset_3G & 0xFD;
		SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x39, Reset_3G);
		//module reset
		break;
	default:
		break;
	}
}
#pragma endregion
#pragma region Save setting to MCU Function
unsigned int Save_MCU()
{
	SMB_write(SMBus_Base, SMBus_SlaveAddress, 0x28, 0x02);
	printf("Save Finish\n");
}
#pragma endregion
//

//
#pragma region Get MCU Firmware Version Function
unsigned int Get_MCU_FW()
{
	Firmware_version1 = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x01);
	Firmware_version2 = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x02);
	Firmware_version3 = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x03);
	Firmware_version4 = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x04);
	Firmware_version5 = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x05);
	printf("MCU Version: V%X.%X.%X.%X.%X\n", Firmware_version1, Firmware_version2, Firmware_version3, Firmware_version4, Firmware_version5);
}
#pragma endregion
#pragma region Get POE Voltage & Current Function
void Get_POE_Current(unsigned int iplsb, unsigned int ipmsb)
{
	unsigned int Current_Value, Value_LSB, Value_MSB;
	int X_value;
	Value_LSB = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, iplsb);
	usleep(1000);
	Value_MSB = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, ipmsb);
	usleep(1000);
	Current_Value = (Value_MSB << 8) + Value_LSB;
	X_value = Current_Value;
	X_value = (X_value * 122070) / 1000000;
	POE_Current = X_value;
}

void Get_POE_Voltage(unsigned int vplsb, unsigned int vpmsb)
{
	unsigned int Voltage_Value, Value_LSB, Value_MSB;
	int X_value;
	Value_LSB = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, vplsb);
	usleep(1000);
	Value_MSB = SMB_read(SMBus_Base, SMBus_POE_SlaveAddress, vpmsb);
	usleep(1000);
	Voltage_Value = (Value_MSB << 8) + Value_LSB;
	X_value = Voltage_Value;
	X_value = (X_value * 5835) / 1000000;
	POE_Voltage = X_value;
}
void Get_POE2_Current(unsigned int iplsb, unsigned int ipmsb)
{
	unsigned int Current_Value, Value_LSB, Value_MSB;
	int X_value;
	Value_LSB = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, iplsb);
	usleep(1000);
	Value_MSB = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, ipmsb);
	usleep(1000);
	Current_Value = (Value_MSB << 8) + Value_LSB;
	X_value = Current_Value;
	X_value = (X_value * 122070) / 1000000;
	POE_Current = X_value;
}

void Get_POE2_Voltage(unsigned int vplsb, unsigned int vpmsb)
{
	unsigned int Voltage_Value, Value_LSB, Value_MSB;
	int X_value;
	Value_LSB = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, vplsb);
	usleep(1000);
	Value_MSB = SMB_read(SMBus_Base, SMBus_POE2_SlaveAddress, vpmsb);
	usleep(1000);
	Voltage_Value = (Value_MSB << 8) + Value_LSB;
	X_value = Voltage_Value;
	X_value = (X_value * 5835) / 1000000;
	POE_Voltage = X_value;
}
#pragma endregion
#pragma region Get DIO Status Function
unsigned int Get_DIO_Status()
{
	DI_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x30);
	DO_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x31);

#pragma region Get DI1~DI8
	if ((DI_Value & 0x01) == 1)
	{
		printf("DI 1 = HIGH \n");
	}
	else
	{
		printf("DI 1 = LOW \n");
	}
	if (((DI_Value >> 1) & 0x01) == 1)
	{
		printf("DI 2 = HIGH \n");
	}
	else
	{
		printf("DI 2 = LOW \n");
	}
	if (((DI_Value >> 2) & 0x01) == 1)
	{
		printf("DI 3 = HIGH \n");
	}
	else
	{
		printf("DI 3 = LOW \n");
	}
	if (((DI_Value >> 3) & 0x01) == 1)
	{
		printf("DI 4 = HIGH \n");
	}
	else
	{
		printf("DI 4 = LOW \n");
	}
	if (((DI_Value >> 4) & 0x01) == 1)
	{
		printf("DI 5 = HIGH \n");
	}
	else
	{
		printf("DI 5 = LOW \n");
	}
	if (((DI_Value >> 5) & 0x01) == 1)
	{
		printf("DI 6 = HIGH \n");
	}
	else
	{
		printf("DI 6 = LOW \n");
	}
	if (((DI_Value >> 6) & 0x01) == 1)
	{
		printf("DI 7 = HIGH \n");
	}
	else
	{
		printf("DI 7 = LOW \n");
	}
	if (((DI_Value >> 7) & 0x01) == 1)
	{
		printf("DI 8 = HIGH \n");
	}
	else
	{
		printf("DI 8 = LOW \n");
	}
#pragma endregion
	printf("-------------\n");
#pragma region Get DO1~DO4
	if ((DO_Value & 0x01) == 1)
	{
		printf("DO 1 = HIGH \n");
	}
	else
	{
		printf("DO 1 = LOW \n");
	}
	if (((DO_Value >> 1) & 0x01) == 1)
	{
		printf("DO 2 = HIGH \n");
	}
	else
	{
		printf("DO 2 = LOW \n");
	}
	if (((DO_Value >> 2) & 0x01) == 1)
	{
		printf("DO 3 = HIGH \n");
	}
	else
	{
		printf("DO 3 = LOW \n");
	}
	if (((DO_Value >> 3) & 0x01) == 1)
	{
		printf("DO 4 = HIGH \n");
	}
	else
	{
		printf("DO 4 = LOW \n");
	}

#pragma endregion
}
#pragma endregion
#pragma region Get IGN Delay Times Function
unsigned int Get_IGN_Delay_OFF_Times()
{
	IGN_Sec = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x66);
	IGN_Min = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x67);
	IGN_Hour = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x68);
	printf("IGN Hour:%u\nIGN Min:%u\nIGN Sec:%u\n", IGN_Hour, IGN_Min, IGN_Sec);
}
#pragma endregion
#pragma region Get UPS Delay Times Function
unsigned int Get_UPS_Delay_OFF_Times()
{
	UPS_Sec = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x72);
	UPS_Min = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x73);
	UPS_Hour = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x74);
	printf("UPS Hour:%u\nUPS Min:%u\nUPS Sec:%u\n", UPS_Hour, UPS_Min, UPS_Sec);
}
#pragma endregion
#pragma region Get IGN Status
unsigned int Get_IGN_Status()
{
	IgnitionStatus = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x12);
	if (IgnitionStatus == 0x01)
	{
		printf("Ignition ON\n");
	}
	else
	{
		printf("Ignition OFF\n");
	}
}
#pragma endregion
#pragma region Get System Voltage
unsigned int GetSystemInputVoltage()
{
	SystemInputVoltage_H = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x47);
	SystemInputVoltage_L = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0x46);
	printf("System Input Voltage:%u.%02uV\n", SystemInputVoltage_H, SystemInputVoltage_L);
}
#pragma endregion
#pragma region Get UPS Function



void Get_UPS_PerCent(void)
{
	UPS_H_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0xA7);
    UPS_L_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0xA6);
    UPS_Result2 = UPS_H_Value+UPS_L_Value;
    printf("UPS PerCent: %d%% \n",UPS_Result2);

}

void Get_UPS_Current(void)
{
	UPS_H_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0xA5);
    UPS_L_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0xA4);
    UPS_Result = ((UPS_H_Value * 256) + UPS_L_Value)/1000;
    
    printf("UPS Current: %f A\n",UPS_Result );

}



void Get_UPS_Voltage(void)
{
	UPS_H_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0xA3);
    UPS_L_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0xA2);
    UPS_Result = UPS_H_Value + (UPS_L_Value * 0.01);
    printf("UPS Voltage: %f V\n",UPS_Result );

}

void Get_UPS_Temperature(void)
{
	UPS_H_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0xA1);
    UPS_L_Value = SMB_read(SMBus_Base, SMBus_SlaveAddress, 0xA0);

            UPS_Result = UPS_H_Value * 256;
            UPS_Result = UPS_Result + UPS_L_Value;
            UPS_Result2 = (UPS_Result / 10) - 273;


    
    printf("UPS Temperature: %d C\n",UPS_Result2 );

}

#pragma endregion
//

int main(int argc, char** argv)
{
	//
	if (get_io_permission())//Root
		return -EPERM;
	//

	printf("---------------------------------------------------------------------\n");
	printf("ABOX-5210-GetUPS-V1.2 \n");
	printf("'G':Get \n");
	printf("'S':Set \n");
	printf("Enter: ");
	scanf(" %c", &Get_Set_Check);
	printf("-------------------------------------------------------\n");
	switch (Get_Set_Check)
	{
	case 'G':
		printf("'F':Get MCU Firmware Version \n");
		printf("'I':Get IGN Status \n");
		printf("'T':Get IGN & UPS Delay OFF Time \n");
		printf("'D':Get DI & DO Value \n");
		printf("'P':Get POE Voltage & Current \n");
		printf("'V':Get System Input Voltage\n");
		printf("'U':Get UPS INFO \n");
		printf("Enter: ");
		scanf(" %c", &Get_Check);
		printf("-------------------------------------------------------\n");
		switch (Get_Check)
		{
		case 'F':
			Get_MCU_FW();
			break;

		case 'I':
			Get_IGN_Status();
			break;

		case 'T':
			printf("'I':Get IGN Delay OFF Time \n");
			printf("'U':Get UPS Delay OFF Time \n");
			printf("Enter: ");
			scanf(" %c", &Get_UPS_IGN_Check);
			printf("-------------------------------------------------------\n");
			switch (Get_UPS_IGN_Check)
			{
			case 'I':
				Get_IGN_Delay_OFF_Times();
				break;
			case 'U':
				Get_UPS_Delay_OFF_Times();
				break;
			default:
				break;
			}
			break;

		case 'D':
			Get_DIO_Status();
			break;
		case 'P':
			Get_POE_Voltage(0x32, 0x33);
			Get_POE_Current(0x30, 0x31);
			printf("LAN3/POE: %uV, %umA\n", POE_Voltage, POE_Current);
			usleep(1000);
			Get_POE_Voltage(0x36, 0x37);
			Get_POE_Current(0x34, 0x35);
			printf("LAN4/POE: %uV, %umA\n", POE_Voltage, POE_Current);
			usleep(1000);
			Get_POE_Voltage(0x3A, 0x3B);
			Get_POE_Current(0x38, 0x39);
			printf("LAN5/POE: %uV, %umA\n", POE_Voltage, POE_Current);
			usleep(1000);
			Get_POE_Voltage(0x3E, 0x3F);
			Get_POE_Current(0x3C, 0x3D);
			printf("LAN6/POE: %uV, %umA\n", POE_Voltage, POE_Current);
			Get_POE2_Voltage(0x32, 0x33);
			Get_POE2_Current(0x30, 0x31);
			printf("LAN7/POE: %uV, %umA\n", POE_Voltage, POE_Current);
			usleep(1000);
			Get_POE2_Voltage(0x36, 0x37);
			Get_POE2_Current(0x34, 0x35);
			printf("LAN8/POE: %uV, %umA\n", POE_Voltage, POE_Current);
			usleep(1000);
			Get_POE2_Voltage(0x3A, 0x3B);
			Get_POE2_Current(0x38, 0x39);
			printf("LAN9/POE: %uV, %umA\n", POE_Voltage, POE_Current);
			usleep(1000);
			Get_POE2_Voltage(0x3E, 0x3F);
			Get_POE2_Current(0x3C, 0x3D);
			printf("LAN10/POE: %uV, %umA\n", POE_Voltage, POE_Current);
			break;
		case 'V':
			GetSystemInputVoltage();
			break;
		case 'U':
			Get_UPS_Voltage();
            Get_UPS_Current();
            Get_UPS_PerCent();
            Get_UPS_Temperature();
			break;

		default:
			break;
		}
		break;
	case 'S':
		printf("'T':Set IGN & UPS Delay OFF Time \n");
		printf("'D':Set DO Value \n");
		printf("'P':Set POE ON or OFF \n");
		printf("'M':Set SIM card or 3G/LTE module reset\n");
		printf("'S':Save setting to MCU \n");
		printf("Enter: ");
		scanf(" %c", &Set_Check);
		printf("-------------------------------------------------------\n");
		switch (Set_Check)
		{
		case 'T':
			printf("'I':Set IGN Delay OFF Time \n");
			printf("'U':Set UPS Delay OFF Time \n");
			printf("Enter: ");
			scanf(" %c", &Set_UPS_IGN_Check);
			printf("-------------------------------------------------------\n");
			switch (Set_UPS_IGN_Check)
			{
			case 'I':
				Set_IGN_Delay_Time();
				break;
			case 'U':
				Set_UPS_Delay_Time();
				break;
			default:
				break;
			}
			break;
		case 'D':
			Set_DO();
			break;
		case 'P':
			Set_POE();
			break;
		case 'M':
			Modem();
			break;
		case 'S':
			Save_MCU();
			break;

		default:
			break;
		}
		break;
	default:
		break;
	}
}