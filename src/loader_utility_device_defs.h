/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_utility_device_defs.h
 * - Description
 *   : This defines macros and structures for Utility::getDiskSerial(..)
 */

#include <windows.h>

#define DFP_GET_VERSION         0x00074080
#define DFP_SEND_DRIVE_COMMAND  0x0007c084
#define DFP_RECEIVE_DRIVE_DATA  0x0007c088
#define SERIAL_NUMBER_LENGTH    20

#pragma pack(1)
typedef struct
{
	char Features;
	char SectorCount;
	char SectorNumber;
	char CylinderLow;
	char CylinderHight;
	char DriveHead;
	char Command;
	char Reserved;
} IDEREGS;

typedef struct
{
	int BufferSize;
	IDEREGS DriveRegs;
	char DriveNumber;
	char reserved[3];
	int reserved2[4];
} SENDCMDINPARAMS;

typedef struct
{
	char DriveError;
	char IDEStatus;
	char Reserved1[2];
	int Reserverd2[2];
} DRIVERSTATUS;

typedef struct
{
	short GenConfig;
	short NumberCylinders;
	short Reserved;
	short NumberHeads;
	short BytesPerTrack;
	short BytesPerSector;
	short SectorsPerTrack;
	short VendorUnique[3];
	char SerialNumber[20];
	short BufferClass;
	short BufferSize;
	short ECCSize;
	char FirmwareRevision[8];
	char ModelNumber[40];
	short MoreVendorUnique;
	short DoubleWordIO;
	short Capabilities;
	short Reserved1;
	short PIOTiming;
	short DMATiming;
	short BS;
	short NumberCurrentCyls;
	short NumberCurrentHeads;
	short NumberCurrentSectorsPerTrack;
	int CurrentSectorCapacity;
	short MultipleSectorCapacity;
	short MultipleSectorStuff;
	int TotalAddressableSectors;
	short SingleWordDMA;
	short MultiWordDMA;
	char Reserved2[382];
} IDSECTOR;

typedef struct
{
	int BufferSize;
	DRIVERSTATUS Status;
	IDSECTOR IDS;
} SENDCMDOUTPARAMS;
#pragma pack()

