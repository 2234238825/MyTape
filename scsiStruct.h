//
// Created by Admin on 2024/10/12.
//

#ifndef UNTITLED13_SCSISTRUCT_H
#define UNTITLED13_SCSISTRUCT_H


//--------------------------------
// Structures For Read Element Status
//
// Element Status Data
#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <cstring>
#include <vector>

#define DEVICE_PATH             260
#define VENDOR_ID_LENGTH        8
#define PRODUCT_ID_LENGTH       16
#define FIRMWARE_LENGTH         4

#define MAX_SERIAL_NUM_LENGTH	64
#define TAPENAME_LEN            256
#define OWNER_NAME_LEN          64
#define NODE_ID_LEN             64

#define MOVE_FROM_SLOT_TO_DRIVE 0
#define MOVE_FROM_DRIVE_TO_SLOT 1

struct ELEMENT_STATUS_HEADER
{
    char  szFirstElementReported[2];
    char  szNumberOfElementAvailable[2];
    char  chReserved;
    char  szByteCountReportAvailable[3];
} ;

struct   ELEMENT_STATUS_PAGE{
    char  chElementType;
    //CHAR  chReserved;
    char	PVoltag;
    char  szElementDescriptLength[2];
    char  chReserved2;
    char  szByteCountDescript[3];
} ;

struct   ELEMENT_DESCRIPTOR
{
    char  szElementAddress[2];
    char  chERFByte;        //Except: Bit 2, Reserved:Bit 1, Full:Bit 0
    char  chReserved;
    char  chAscode;         //additional sense code
    char  chAscq;           //additional sense qualifier
    char  chNRILByte;       // These two bytes only used in Data Trans Element
    char  chSCSIAddress;
    char  chReserved2;
    char  chSIByte;
    char  szSrcStEleAddr[2];//Source storage element address
    char	 szVolTag[36];		//Primary Volume Tag information
    char  szReserved3[4];
    char  szReserved4[2];
} ;

//Request Sense Buffer offsets
#define  ERROR_BIT      0
#define  SENSE_KEY      2
#define  ASL            7
#define  ASC            12
#define  ASCQ           13

//mode page sizes
#define  SIX_BYTE_MODE_HEADER       4  //bytes
#define  TEN_BYTE_MODE_HEADER       8  //bytes
#define  BLOCK_DESCRIPTOR_LENGTH    8  //bytes
#define  COMPRESSION_PAGE_LENGTH    16 //bytes
#define  DEVICE_CONFIG_PAGE_LENGTH  16 //bytes


//ModePage 0x00 offsets and bits
#define  WP_BIT_OFFSET        2
#define  WP_BIT               0x80  // Mode Sense Write Protect Bit

//ModePage 0f Compression offsets and Bits
#define  DCE_BIT_OFFSET 2
#define  DCC_BIT_OFFSET 2
#define  DCE_BIT        0x80 //hex
#define  DCC_BIT        0x40 //hex


//ModePage 1d Buffer offsets
#define  MT_ADDRESS1    6
#define  MT_ADDRESS2    7
#define  MT_COUNT1      8
#define  MT_COUNT2      9
#define  SE_ADDRESS1    10
#define  SE_ADDRESS2    11
#define  SE_COUNT1      12
#define  SE_COUNT2      13
#define  IE_ADDRESS1    14
#define  IE_ADDRESS2    15
#define  IE_COUNT1      16
#define  IE_COUNT2      17
#define  DT_ADDRESS1    18
#define  DT_ADDRESS2    19
#define  DT_COUNT1      20
#define  DT_COUNT2      21


//ModeSense Parameter List Header offsets
#define  MODE_SENSE_MEDIUM_TYPE     1
#define  MODE_SENSE_WP_BYTE         2
#define  MODE_SENSE_WP_BIT          0x80
#define  MODE_SENSE_BUFF_MODE_BYTE  2
#define  MODE_SENSE_BUFF_MODE_BIT   0x10

//Mode Page Block Descriptor
#define  MODE_PAGE_DENSITY_CODE     0
#define  MODE_PAGE_NO_BLOCKS_1      1
#define  MODE_PAGE_NO_BLOCKS_2      2
#define  MODE_PAGE_NO_BLOCKS_3      3
#define  MODE_PAGE_BLOCK_SIZE_1     5
#define  MODE_PAGE_BLOCK_SIZE_2     6
#define  MODE_PAGE_BLOCK_SIZE_3     7

//Mode Page Device Configuration
#define  MODE_PAGE_DELAY_TIME_1     7
#define  MODE_PAGE_DELAY_TIME_2     8


//SCSI Command Direction
#define  SCSI_NODATA                   0x00
#define  SCSI_DATAIN                   0x02
#define  SCSI_DATAOUT                  0x04
#define  RESET_DEVICE                  0x10

#define  MAX_CDB_LENGTH                16
#define  MAX_CDB_BUFFER                (8 * 1024)
#define  MAX_BUFFER_SIZE               ((1024*1024)*8)  //8 meg

//Pre-Qual Error Codes
#define  SCSI_TOOL_ERROR_EXIT                     10
#define  SCSI_TOOL_ERROR_CONTINUE                 20
#define  SCSI_TOOL_ERROR_CHANGE_DEVICE            30
#define  SCSI_TOOL_ERROR_INVALID_STREAM           40
#define  SCSI_TOOL_ERROR_BAD_CDB_LENGTH           50
#define  SCSI_TOOL_ERROR_SUCCESS                  60
#define  SCSI_TOOL_INVALID_DIRECTION              70
#define  SCSI_TOOL_MALLOC_FAILED                  80
#define  SCSI_TOOL_FAILED_HEX_CONVERT             256

//CDB Lengths
#define  SIX_BYTE_CDB         6
#define  TEN_BYTE_CDB         10
#define  TWELVE_BYTE_CDB      12


//Buffer sizes
#define  TEMP_BUFFER_LENGTH            80
#define  REQUEST_SENSE_BUFFER_LENGTH   60
#define  MODE_SENSE_BUFFER_LENGTH      80
#define  INQUIRY_BUFFER_LENGTH         80
#define  TEST_UNIT_READY_BUFFER_LENGTH 00

//SCSI_DATAIN
#define 	SCSI_READ                         0x08      // Read blocks
#define 	SCSI_INQUIRY                      0x12      // Inquiry
#define 	SCSI_REQUEST_SENSE                0x03      // Request sense
#define 	SCSI_READ_ELEMENT_STATUS          0xB8      // Read Element Status
#define 	SCSI_LOG_SENSE                    0x4D      // LOG Sense
#define 	SCSI_READ_POS                     0x34      // WANGTEK Read Position
#define 	SCSI_READ_BLK_ADDR                0x02      // Get block address
#define 	SCSI_READ_BUFFER                  0x3c      // Read Buffer
#define 	SCSI_MODE_SENSE                   0x1a      // Mode sense
#define     SCSI_READ_BLOCK_LIMIT             0x05      // Read Block Limit
#define     SCSI_DENSITY_SUPPORT              0x44      // Get Density Support

//SCSI_DATAOUT
#define 	SCSI_MODE_SELECT                  0x15      // Mode select
#define 	SCSI_WRITE                        0x0a      // Write blocks
#define 	SCSI_WRITE_BUFFER                 0x3b      // Write Buffer
#define 	SCSI_ERASE                        0x19      // Erase

//SCSI_NODATA
#define 	SCSI_TUR                          0x00      // Test unit ready
#define 	SCSI_REWIND                       0x01      // Rewind
#define 	SCSI_MOVE_MEDIUM                  0xA5      // Move Medium
#define 	SCSI_LOAD                         0x1b      // Load
#define 	SCSI_INITIALIZE_ELEMENT_STATUS    0x07		  // Initialize Element Status
#define 	SCSI_INITIALIZE_ELEMENT_RANGE     0xE7		  // Initialize Element with Range
#define 	SCSI_SEEK_BLOCK                   0x0c      // Seek block
#define 	SCSI_PARTITION                    0x0d      // WANGTEK Partition
#define 	SCSI_WRITE_FILEMARK               0x10      // Write filemark
#define 	SCSI_SPACE                        0x11      // Space command
#define 	SCSI_RECOVER_DATA                 0x14      // Recover buffered data
#define 	SCSI_PREVENT_ALLOW                0x1e      // Prevent/Allow
#define 	SCSI_SEEK_POS                     0x2b      // WANGTEK Seek Position
#define     SCSI_POS_TO_ELEMENT               0x2b      // Position to Element in Library
#define 	SCSI_RESERVE                      0x16
#define 	SCSI_RELEASE                      0x17
#define 	SCSI_LOG_SELECT                   0x4C      // LOG Select

//SmartHexDump Types
#define  NORMAL_HEX_DUMP                  0x10
#define  REQUEST_SENSE_HEX_DUMP           0x20
#define  SERIAL_NUMBER_HEX_DUMP           0x30
#define  MODE_SELECT_HEX_DUMP             0x40
#define  LOGSENSE_HEX_DUMP                0x60
#define  READ_HEX_DUMP                    0x80
#define  READ_NO_HEX_DUMP                 0x100


#define  RESIDUE_BYTES_VALID     			0x80
#define  SENSEKEY_MASK           			0x0F


//Inquiry Offsets
#define  VENDOR_ID_BIT           8
#define  PRODUCT_ID_BIT          16
#define  PERSONALITY_BIT         41



//
// SCSI Sense keys.
//
#define  SCSI_NO_SENSE                    0x00
#define  SCSI_RECOVERED_ERROR             0x01
#define  SCSI_NOT_READY                   0x02
#define  SCSI_MEDIUM_ERROR                0x03
#define  SCSI_HARDWARE_ERROR              0x04
#define  SCSI_ILLEGAL_REQUEST             0x05
#define  SCSI_UNIT_ATTENTION              0x06
#define  SCSI_WRITE_PROTECT               0x07
#define  SCSI_BLANK_CHECK                 0x08
#define  SCSI_COPY_ABORTED                0x0a
#define  SCSI_ABORTED_COMMAND             0x0b
#define  SCSI_VOLUME_OVERFLOW             0x0d
#define  SCSI_FILEMARK_DETECTED           0x80
#define  SCSI_EOM_DETECTED                0x40
#define  SCSI_ILLEGAL_LENGTH_DETECTED     0x20

#define  VENDOR_ID_MAX_LENGTH    8
#define  PRODUCT_ID_MAX_LENGTH   16

#define  BARCODE_LENGTH	      36
#define  SERIAL_NO_LENGTH     20



//ReadElement Offsets

//Element Status data
#define ESD_FIRST_ELEMENT_1      0
#define ESD_FIRST_ELEMENT_2      1
#define ESD_NUMBER_ELEMENTS_1    2
#define ESD_NUMBER_ELEMENTS_2    3
#define ESD_PAGE_SIZE            8 //bytes

//Element Status Page
#define ESP_ELEMENT_TYPE         0
#define ESP_PVOLTAG_BYTE         1
#define ESP_AVOLTAG_BYTE         1
#define ESP_ESD_LENGTH_1         2
#define ESP_ESD_LENGTH_2         3
#define ESP_TOTAL_ESD_LENGTH_1   5
#define ESP_TOTAL_ESD_LENGTH_2   6
#define ESP_TOTAL_ESD_LENGTH_3   7
#define ESP_PAGE_SIZE            8 //bytes

//Storage Element Descriptor
#define SED_ELEMENT_ADDRESS_1       0
#define SED_ELEMENT_ADDRESS_2       1
#define SED_SLOT_FULL_BYTE          2
#define SED_ACESS_BYTE              2
#define SED_ASC                     4
#define SED_ASCQ                    5
#define SED_SVALID                  9
#define SED_INVERT                  9
#define SED_SOURCE_ELEMENT_1        10
#define SED_SOURCE_ELEMENT_2        11
#define SED_PRIMARY_VOLTAG          12
#define SED_PRIMARY_VOLTAG_LENGTH   37
#define SED_PAGE_SIZE               52

//IE Element Descriptor
#define IEED_ELEMENT_ADDRESS_1      0
#define IEED_ELEMENT_ADDRESS_2      1
#define IEED_ACESS_BYTE             2  //retracted-notRetracted
#define IEED_EXENAB_BYTE            2
#define IEED_INENAB_BYTE            2
#define IEED_IMPEXP_BYTE            2  //set if user inserted - 0 if pt there by library
#define IEED_SLOT_FULL_BYTE         2
#define IEED_ASC                    4
#define IEED_ASCQ                   5
#define IEED_SVALID                 9
#define IEED_INVERT                 9
#define IEED_SOURCE_ELEMENT_1       10
#define IEED_SOURCE_ELEMENT_2       11
#define IEED_PRIMARY_VOLTAG         12
#define IEED_PRIMARY_VOLTAG_LENGTH  37
#define IEED_PAGE_SIZE              52

//Data Transfer Element Descriptor
#define DTED_ELEMENT_ADDRESS_1      0
#define DTED_ELEMENT_ADDRESS_2      1
#define DTED_ACESS_BYTE             2  //ejected-notejected
#define DTED_DRIVE_FULL_BYTE        2
#define DTED_ASC                    4
#define DTED_ASCQ                   5
#define DTED_IDVALID                6
#define DTED_LUVALID                6
#define DTED_LOGICAL_UNIT           6
#define DTED_SCSI_ID                7
#define DTED_SVALID                 9
#define DTED_INVERT                 9
#define DTED_SOURCE_ELEMENT_1       10
#define DTED_SOURCE_ELEMENT_2       11
#define DTED_PAGE_SIZE              63
#define DTED_SER_NUM_OFFSET         52
#define DTED_SER_NUM_SIZE           10

//Read Element Bits
#define RE_PVOLTAG_BIT           0x80
#define RE_AVOLTAG_BIT           0x40
#define RE_SVALID_BIT            0x80
#define RE_INVERT                0x40
#define RE_ACESS_BIT             0x08
#define RE_EXENAB_BIT            0x10
#define RE_INENAB_BIT            0x20
#define RE_IMPEXP_BIT            0x02
#define RE_ELEMENT_FULL_BIT      0x01
#define RE_IDVALID_BIT           0x20
#define RE_LUVALID_BIT           0x10
#define RE_VOLTAG_BIT            0x10

//Element Types
#define RE_ALL_TYPES             0x00
#define RE_MED_TRANS             0x01
#define RE_STORAGE_ELEMENT       0x02
#define RE_IE_ELEMENT            0x03
#define RE_DT_ELEMENT            0x04

///ShowBarCodesInSlots Flags
#define SHOW_EMPTY_SLOTS         0x01
#define SHOW_FULL_SLOTS          0x02
#define SHOW_IE_SLOTS            0x04
#define SHOW_TAPE_IN_DRIVE       0x08

#define  RMB                        	0x80     // device uses removeable media
#define  DIRECT_ACCESS              	0x00     // disks
#define  SEQUENTIAL_ACCESS          	0x01     // tapes
#define  PRINTER_DEVICE             	0x02     // printers
#define  PROCESSOR_DEVICE           	0x03     // scanners, printers, etc
#define  WORM_DEVICE                	0x04     // worms
#define  CDROM_DEVICE               	0x05     // cdroms
#define  SCANNER_DEVICE             	0x06     // scanners
#define  OPTICAL_DEVICE             	0x07     // optical disks
#define  SCSI_CHANGER_DEVICE        	0x08     // jukebox
#define  COMMUNICATION_DEVICE       	0x09     // network

//Inquiry Offsets
#define  VENDOR_ID_OFFSET        8
#define  PRODUCT_ID_OFFSET       16
#define  FIRMWARE_REV_OFFSET     32
#define  PERSONALITY_OFFSET      41

#define 	SCSI_COMPAQ_INQUIRY_PAGE       0xCC
#define 	SCSI_DELL_INQUIRY_PAGE         0xDC
#define 	COMPAQ_INQUIRY_BIT			    0x0C
#define 	DELL_INQUIRY_BIT			       0x17

#endif //UNTITLED13_SCSISTRUCT_H
