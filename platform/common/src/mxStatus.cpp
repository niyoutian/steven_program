#include "mxStatus.h"

//https://www.latelee.cn/my-library/get-error-code-using-linux-c.html

///////////// 推荐方式
static const char* gp_StatusCode[] = {
    [STATUS_SUCCESS] = "Success",
    [STATUS_FAILED] = "General Failed",
    [STATUS_CONTINUE] = "continue",
    // more...
};


string getStatusCodeString(u32_t status) 
{
    string str;

    if (status < STATUS_END) {
        str = gp_StatusCode[status];
    }

    return str;
}
