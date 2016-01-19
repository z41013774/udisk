#ifndef __SCSI_H__
#define __SCSI_H__
#include "s3c2440.h"
/*at least 36byte*/
#define	RESP_INQURIY_COUNT		36



/*
 *      SCSI opcodes
 */

#define TEST_UNIT_READY       		0x00
#define REZERO_UNIT           		0x01
#define REQUEST_SENSE         		0x03
#define FORMAT_UNIT           		0x04
#define READ_BLOCK_LIMITS     		0x05
#define REASSIGN_BLOCKS       		0x07
#define INITIALIZE_ELEMENT_STATUS 	0x07
#define READ_6                		0x08
#define WRITE_6               		0x0a
#define SEEK_6                		0x0b
#define READ_REVERSE          		0x0f
#define WRITE_FILEMARKS       		0x10
#define SPACE                 		0x11
#define INQUIRY               		0x12
#define RECOVER_BUFFERED_DATA 		0x14
#define MODE_SELECT           		0x15
#define RESERVE               		0x16
#define RELEASE               		0x17
#define COPY                  		0x18
#define ERASE                 		0x19
#define MODE_SENSE            		0x1a
#define START_STOP            		0x1b
#define RECEIVE_DIAGNOSTIC    		0x1c
#define SEND_DIAGNOSTIC       		0x1d
#define ALLOW_MEDIUM_REMOVAL  		0x1e

#define READ_FORMAT_CAPACITY		0x23
#define SET_WINDOW            		0x24
#define READ_CAPACITY         		0x25
#define READ_10               		0x28
#define WRITE_10              		0x2a
#define SEEK_10               		0x2b
#define POSITION_TO_ELEMENT   		0x2b
#define WRITE_VERIFY          		0x2e
#define VERIFY                		0x2f
#define SEARCH_HIGH           		0x30
#define SEARCH_EQUAL          		0x31
#define SEARCH_LOW            		0x32
#define SET_LIMITS            		0x33
#define PRE_FETCH             		0x34
#define READ_POSITION         		0x34
#define SYNCHRONIZE_CACHE     		0x35
#define LOCK_UNLOCK_CACHE     		0x36
#define READ_DEFECT_DATA      		0x37
#define MEDIUM_SCAN           		0x38
#define COMPARE               		0x39
#define COPY_VERIFY           		0x3a
#define WRITE_BUFFER          		0x3b
#define READ_BUFFER           		0x3c
#define UPDATE_BLOCK          		0x3d
#define READ_LONG             		0x3e
#define WRITE_LONG            		0x3f
#define CHANGE_DEFINITION     		0x40
#define WRITE_SAME            		0x41
#define READ_TOC              		0x43
#define LOG_SELECT            		0x4c
#define LOG_SENSE             		0x4d
#define XDWRITEREAD_10        		0x53
#define MODE_SELECT_10        		0x55
#define RESERVE_10            		0x56
#define RELEASE_10            		0x57
#define MODE_SENSE_10         		0x5a
#define PERSISTENT_RESERVE_IN 		0x5e
#define PERSISTENT_RESERVE_OUT 		0x5f
#define VARIABLE_LENGTH_CMD   		0x7f
#define REPORT_LUNS           		0xa0
#define MAINTENANCE_IN        		0xa3
#define MAINTENANCE_OUT       		0xa4
#define MOVE_MEDIUM           		0xa5
#define EXCHANGE_MEDIUM       		0xa6
#define READ_12               		0xa8
#define WRITE_12              		0xaa
#define WRITE_VERIFY_12       		0xae
#define SEARCH_HIGH_12        		0xb0
#define SEARCH_EQUAL_12       		0xb1
#define SEARCH_LOW_12         		0xb2
#define READ_ELEMENT_STATUS   		0xb8
#define SEND_VOLUME_TAG       		0xb6
#define WRITE_LONG_2          		0xea
#define READ_16               		0x88
#define WRITE_16              		0x8a
#define VERIFY_16	      			0x8f
#define SERVICE_ACTION_IN     0x9e
/* values for service action in */
#define	SAI_READ_CAPACITY_16  0x10
/* values for maintenance in */
#define MI_REPORT_TARGET_PGS  0x0a
/* values for maintenance out */
#define MO_SET_TARGET_PGS     0x0a
/* values for variable length command */
#define READ_32		      0x09
#define WRITE_32	      0x0b
void resp_inquiry(void);
void resp_readcap(void);
void resp_mode_select(void);
void resp_test_uint_ready();
void resp_requset_sense();
void resp_allow_memium_removal();
void resp_read_format_capacity();
void resp_read_capacity();
void resp_read10();
void resp_write10();
void resp_verify();
void resp_mode_sense10();

static void usb_process_scsi_cmd(void);
extern const uint8_t scsi_inquiry[RESP_INQURIY_COUNT];
extern const uint8_t scsi_datareadformatcapacities[12];
extern const uint8_t scsidata_readcapacity[8];
extern const uint8_t scsidata_requestsense[18];
extern  const uint8_t scsidata_sense[48];



#endif
