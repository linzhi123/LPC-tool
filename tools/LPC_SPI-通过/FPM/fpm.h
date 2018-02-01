#ifndef FPM_H
#define FPM_H



/********************************************** protocol **********************************************/
typedef struct {
    unsigned char addr_h;
    unsigned char addr_l;
    unsigned char comd;
}commandStruct;

typedef struct {
    unsigned char addr_h;
    unsigned char addr_l;
    unsigned char comd;
    unsigned char data_h;
    unsigned char data_l;
}writeRegisterStruct;

typedef struct {
    unsigned char isNeedFtrAes;
    unsigned char isNeedMatchAes;
    unsigned char reserved1;
    unsigned char ExtractAfterOneShot;
    unsigned char enrollNum;
    unsigned char rotationLimit;
    unsigned char singleFinger_l;
    unsigned char singleFinger_h;
}configureStruct;


typedef struct{
    unsigned char times;
    unsigned int result;
    unsigned int progress;
    unsigned int storage_id;
}ENROLL_PARA;

/***************** sys ******************/
#define SYS_STATUS_ADDR_H (0x1A)
#define SYS_STATUS_ADDR_L (0xE0)

#define SYS_CMD_ADDR_H    (0x1A)
#define SYS_CMD_ADDR_L    (0xE2)

#define SYS_RDY_FOR_CMD_BIT  (0)
#define SYS_ONE_SHOT_BIT     (2)
#define SYS_DEEP_SLEEP_BIT   (3)
#define SYS_FINGER_LEAVE_BIT (5)
#define SYS_GET_UID_BIT      (8)
/*************** sys end ****************/


/***************** Algo *****************/
#define ALGO_STATUS_ADDR_H (0x1A)
#define ALGO_STATUS_ADDR_L (0xE4)

#define ALGO_CMD_ADDR_H    (0x1A)
#define ALGO_CMD_ADDR_L    (0xE6)

#define ALGO_EXTRATION_BIT			  (0)
#define ALGO_MATCH_BIT                (1)
#define ALGO_ENROLLMENT_BIT           (2)
#define ALGO_ABORT_BIT                (3)
#define ALGO_AUTHENTICATION_BIT       (4)
#define ALGO_INITIAL_BIT              (5)
#define ALGO_FIRST_ENROLLMENT_BIT     (6)
#define ALGO_K_DATA_BIT               (7)
#define ALGO_WRITE_AES_KEY_BIT        (8)
#define ALGO_READ_AES_KEY_BIT         (9)
#define ALGO_CHECK_AES_KEY_EXIST_BIT  (11)
#define ALGO_AES_MATCHING_BIT         (12)
#define ALGO_UPDATE_BIT               (14)
#define ALGO_MERGE_FTR_BIT			  (15)

#define SYS_CONFIGURE_BIT       (9)

#define SYS_GET_BG_DATA_BIT     (10)
#define SYS_SET_BG_DATA_BIT     (11)

/*************** Algo end ***************/


/***************** COMD *****************/
#define READ_REGISTER_COMD  (0x00)
#define WRITE_REGISTER_COMD (0x80)

#define READ_K_DATA_COMD    (0x60)
#define WRITE_K_DATA_COMD   (0xC0)
/*************** COMD end ***************/

/************** return code *************/
#define FP_OK                   (0)
#define FP_READWRITE_ERROR      (-1)
#define FP_PARM_ERROR           (-2)
#define FP_TIME_OUT_ERROR       (-3)
#define FP_CHECK_SUM_ERROR      (-4)
#define FP_RESULT_ERROR         (-5)
#define FP_POLLING			    (-6)
#define FP_DATA_FLAG_ERROR      (-7)
#define FP_EXTRATION_ERROR		(-8)
#define FP_MERGE_FTR_ERROR		(-9)



#define FP_FINGER_DOWN			(-5)
#define FP_FINGER_UP            (-10)

/************ return code end ***********/


/************** Enrollment result code *************/
#define PR_ENROLL_OK                                    (0x02000000)
#define PR_ENROLL_FAIL                                  (0x02000001)
#define PR_ENROLL_LARGE_MOVE                            (0x02000002)
#define PR_ENROLL_ALMOST_NOMOVE                         (0x02000003)
#define PR_ENROLL_BUFFER_OVERFLOW                       (0x02000005)
#define PR_ENROLL_COMPLETE                              (0x02000006)
#define PR_ENROLL_EXCEEDED                              (0X02000007)
#define PR_ENROLL_LOW_IMAGE_QUALITY                     (0X02000008)
#define PR_ENROLL_LOW_COVERAGE                          (0X02000009)
#define PR_ENROLL_MEGER_FAIL                            (0X0200000A)
/************** Enrollment result code end *************/

#define SYS_BUTTON_SCAN_BIT  (1)

#define FPM_BIT(X) (1UL<<(X))
#define FPM_CHECK_BIT(arg,X) (((arg) & (FPM_BIT(X))) == 0 ? FP_RESULT_ERROR : FP_OK)

#define POLL_TIME_MS         (2) /*polling interval time */
#define DEFAULT_POLL_TIMES   (4000) /*polling times */

#define DUMMY_BYTE		        (0xFF)

#define ONE_FTR_MATCH			(0)
#define FIRST_FTR_TANSFORM		(1)
#define REMAIN_FTR_TANSFORM		(2)
#define QUICK_MATCH				(3)

#define BG_LENGTH		(160*160*2)
#define BG_SUB_TIMES	(10)
#define BG_SUB_LENGTH	(BG_LENGTH/BG_SUB_TIMES)

#define MATCH_MAX_FTR_SIZE	    (25 * 1024 - 8)

#define FTR_BUFFER_LENGTH	(5200)
#define FTR_COUNT_LENGTH	(8)

extern unsigned char g_enrollFtrBuf[];
extern unsigned char *g_pFtrBuf;
extern unsigned int g_maxEnrollNum;
extern unsigned int g_ftrLength;
extern ENROLL_PARA g_stEnrollPara;

int fpm_init(void);

int fpm_AfisInit(unsigned int enrollNum);

int fpm_configer(configureStruct * configure);

int fpm_buttonScan(void);

int fpm_readyForCommand(void);

int fpm_getUID(unsigned char *buffer);

int fpm_checkFingerLeave(void);

int fpm_enrollStart(unsigned int *result, unsigned int *progress);

int fpm_enrollContinue(unsigned int * result, unsigned int * progress);

int fpm_readFtr(unsigned char * ftr, unsigned int * ftrLen);

int fpm_matchStart(void);

int fpm_matchContinue(unsigned char * ftrPack, unsigned int length, unsigned short offset);

int fpm_matchEnd(unsigned char * ftrCount,unsigned char *result);

int fpm_decryptResult(unsigned char *result, unsigned char *pass_flag, unsigned short *score, unsigned int *index,unsigned char *update_flag);

int fpm_updateFtr(unsigned char *ftr,unsigned int *ftrlen);

int fpm_abortCommand(void);

int fpm_oneShot(void);

int fpm_getOneShot(unsigned char *readBuffer, unsigned int line);

int fpm_getBGData(unsigned char * bgData);

int fpm_setBGData(unsigned char * bgData);

int fpm_eraseBGData(void);

int fpm_isBGexist(void);

int fpm_mergeFtr(unsigned char *ftr, unsigned int * ftrLen);

int fpm_extration(int whitchPlace);

#endif
