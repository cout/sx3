// File: sx3_utils.c
// Author: Paul Brannan
//

#include "sx3_utils.h"


const char * error_lookup(
        const struct Error_Message_Entry * msgs,
        int error_code) {
    for(; msgs->error_code != -1; ++msgs) {
        if(msgs->error_code == error_code) {
            return msgs->error_message;
        }
    }
    return "Unexpected error code";
}
