#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "checks.h"

#define uCOMMS_CONTEXT_BUFFER_SIZE 64

typedef enum {
    START_BYTE_FLAG  = 0,
    LENGTH_BYTE_FLAG = 1,
    STOP_BYTE_FLAG   = 2,
    POST_CMD_FLAG    = 3,
    GET_CMD_FLAG     = 4,
    ESC_CMD_FLAG     = 5,
} COMMUNICATION_FLAGS;


typedef enum {
    START_BYTE = 0x02,
    STOP_BYTE  = 0x03,
    ESC_BYTE   = 0x10,
} uCOMMUNICATION_FLAGS;


typedef struct {
    char    comms_buf[uCOMMS_CONTEXT_BUFFER_SIZE];    /// ? Buffer that holds the current command.
    uint8_t comms_flags;                              /// ? Stores the current set comms flags.
    uint8_t cmd_len;                                  /// ? So validation, i think :).
    uint8_t curr_cmd_len;    /// ? Holds the current len command, will be  used to validate later.
} uCOMMS_CONTEXT;


// Resets the parsed comms context.
void reset_comms_context(uCOMMS_CONTEXT *ctx) {
    CHECK_PTR(ctx);
    memset(ctx, 0, sizeof(*ctx));
}

// Compare two uCOMMS_CONTEXT instances
// Returns 1 if equal, 0 if different
int compare_contexts(const uCOMMS_CONTEXT *ctx1, const uCOMMS_CONTEXT *ctx2) {
    CHECK_PTR(ctx1);
    CHECK_PTR(ctx2);
    
    // Field-by-field comparison for reliability
    if (ctx1->comms_flags  != ctx2->comms_flags)  return 0;
    if (ctx1->cmd_len      != ctx2->cmd_len)      return 0;
    if (ctx1->curr_cmd_len != ctx2->curr_cmd_len) return 0;
    
    // Compare buffer contents
    CHECK((sizeof(ctx1->comms_buf) == sizeof(ctx2->comms_buf)), "sizeof context buffers do not match");
    return memcmp(ctx1->comms_buf, ctx2->comms_buf, sizeof(ctx1->comms_buf)) == 0;
}


void interprete_cmd(uCOMMS_CONTEXT *ctx) {
    CHECK_PTR(ctx);
    return;
}

/// * {START_BYTE, MSG_LEN, COMMAND, STOP_BYTE}
void parse_cmd(uCOMMS_CONTEXT *ctx, char data) {
    CHECK_PTR(ctx);          /// ? Passed a valid comms context.

    /// ? Now the actual parsing begins.
    switch (data) {
        /// * START BYTE => start of a new message.
        case START_BYTE: {
            reset_comms_context(ctx);
            ctx->comms_flags  |= 1 << START_BYTE_FLAG;
            break;
        }

        /// * STOP BYTE => end of a transmission.
        case STOP_BYTE: {
            // Check if we're in valid state for STOP
            CHECK((ctx->comms_flags & (1 << START_BYTE_FLAG)), "STOP without START");
            CHECK((ctx->comms_flags & (1 << LENGTH_BYTE_FLAG)), "STOP without length");
            // Check length matches
            CHECK((ctx->curr_cmd_len == ctx->cmd_len), "Length mismatch");
            // Check buffer bounds before adding null terminator
            CHECK((ctx->curr_cmd_len < uCOMMS_CONTEXT_BUFFER_SIZE - 1), "Buffer overflow");
            ctx->comms_flags |= 1 << STOP_BYTE_FLAG;
            ctx->comms_buf[ctx->curr_cmd_len] = '\0';
            interprete_cmd(ctx);
            break;
        }

        /// * DATA BYTE => msg payload
        /// * The total size of the expected msg is also part of the payload
        /// * we need to also detect that, we can achieve that goal with the
        /// * LENGTH BYTE FLAG.
        default : {
            /// ? Current data byte is PAYLOAD length 
            if ((ctx->comms_flags & (1 << START_BYTE_FLAG)) 
                && !(ctx->comms_flags & (1 << LENGTH_BYTE_FLAG)))
            {   
                CHECK((ctx->comms_flags |= (1 << START_BYTE)), "length flag received without start");
                CHECK((data < uCOMMS_CONTEXT_BUFFER_SIZE - 1), "ctx buffer overflow");
                ctx->cmd_len = data;
                ctx->comms_flags |= 1 << LENGTH_BYTE_FLAG;
                return;
            }

            /// ? Current data byte is part of the actual msg
            if ((ctx->comms_flags & (1 << START_BYTE_FLAG)) 
                && (ctx->comms_flags & (1<< LENGTH_BYTE_FLAG)))
            {
                    /// ? bounds checking
                    CHECK((ctx->curr_cmd_len < uCOMMS_CONTEXT_BUFFER_SIZE - 1), "ctx buffer overflow");
                    ctx->comms_buf[ctx->curr_cmd_len++] = data;
                    return;
            } else {
                exit(1); /// ! FIX THIS
            }
            
            break;
        }
    }
}