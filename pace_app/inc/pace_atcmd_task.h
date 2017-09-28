#ifndef __PACE_ATCMD_TASK_H__
#define __PACE_ATCMD_TASK_H__


typedef enum
{
	AT_OK = 0,
    AT_ERROR,
	AT_MAX
} at_cmd_func_status;
typedef struct
{
	char *cmd_string;
	at_cmd_func_status (*cmd_func)(char *func_para);
} at_cmd_func;



#endif /* __PACE_ATCMD_TASK_H__ */

