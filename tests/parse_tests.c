#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>     // for fork()
#include <sys/wait.h>   // for waitpid()

// Include your header files
#include "checks.h"
#include "ucoms.h"  // Contains parse_cmd function and types


// Test helper macros
#define TEST_ASSERT(condition, message)             \
    do {                                            \
        if (!(condition)) {                         \
            fprintf(stderr, "FAIL: %s\n", message); \
            return 0;                               \
        } else {                                    \
            printf("PASS: %s\n", message);          \
        }                                           \
    } while(0)


// Test for functions expected to crash/exit
#define TEST_EXPECT_CRASH(test_code, message)                                              \
    do {                                                                                   \
        pid_t pid = fork();                                                                \
        if (pid == 0) {                                                                    \
            test_code;                                                                     \
            exit(0);                                                                       \
        } else if (pid > 0) {                                                              \
            int status;                                                                    \
            waitpid(pid, &status, 0);                                                      \
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {                           \
                fprintf(stderr, "FAIL: %s (expected crash but didn't crash)\n", message);  \
                return 0;                                                                  \
            } else {                                                                       \
                printf("PASS: %s (crashed as expected)\n", message);                       \
            }                                                                              \
        } else {                                                                           \
            fprintf(stderr, "FAIL: fork() failed for crash test\n");                       \
            return 0;                                                                      \
        }                                                                                  \
    } while(0)


#define RUN_TEST(test_func)                         \
    do {                                            \
        printf("Running %s...\n", #test_func);      \
        if (test_func()) {                          \
            printf("✓ %s passed\n\n", #test_func);  \
            tests_passed++;                         \
        } else {                                    \
            printf("✗ %s failed\n\n", #test_func);  \
            tests_failed++;                         \
        }                                           \
        total_tests++;                              \
    } while(0)




/// ? Test counters
static int tests_passed = 0;
static int tests_failed = 0;
static int total_tests = 0;



/// ? Test implementations

int test_reset_comms_context(void) {
    /// ? example comms context with some data
    uCOMMS_CONTEXT ctx = {
        .cmd_len      = 10,
        .comms_flags  = 2,
        .curr_cmd_len = 5,
        .comms_buf    = {'G', 'E', 'T', ':', 'F', 'R', '\0'},
    };

    // Reset the context
    reset_comms_context(&ctx);
    
    // Create expected empty context for comparison
    uCOMMS_CONTEXT expected = {0};
    
    // Test that context was properly reset
    TEST_ASSERT(compare_contexts(&ctx, &expected), "Context should be cleared after reset");
    TEST_ASSERT(ctx.comms_flags      == 0,         "comms_flags should be 0");
    TEST_ASSERT(ctx.cmd_len          == 0,         "cmd_len should be 0");
    TEST_ASSERT(ctx.curr_cmd_len     == 0,         "curr_cmd_len should be 0");
    TEST_ASSERT(ctx.comms_buf[0]     == '\0',      "Buffer should be cleared");
    
    printf("    - Test context reset functionality\n");
    return 1;
}

int test_parse_start_byte(void) {
    uCOMMS_CONTEXT ctx = {0};
    parse_cmd(&ctx, START_BYTE);
    TEST_ASSERT(ctx.comms_flags & (1 << START_BYTE_FLAG), "START_BYTE should set START_BYTE_FLAG");
    printf("    - Test start byte parsing\n");
    return 1; 
}

int test_parse_length_byte(void) {
    uCOMMS_CONTEXT ctx = {0};

    parse_cmd(&ctx, START_BYTE);
    parse_cmd(&ctx, 10);

    TEST_ASSERT(ctx.comms_flags & (1 << START_BYTE_FLAG),  "START_BYTE should set START_BYTE_FLAG");
    TEST_ASSERT(ctx.comms_flags & (1 << LENGTH_BYTE_FLAG), "LENGTH_BYTE should set LENGTH_BYTE_FLAG"); 

    printf("    - Test length byte parsing\n");
    return 1; // Placeholder - replace with actual test
}

int test_parse_payload_bytes(void) {
    uCOMMS_CONTEXT ctx = {0};

    parse_cmd(&ctx, START_BYTE);
    parse_cmd(&ctx, 4);
    parse_cmd(&ctx, 'G');
    parse_cmd(&ctx, 'E');
    parse_cmd(&ctx, 'T');
    parse_cmd(&ctx, ':');
    parse_cmd(&ctx, STOP_BYTE);

    uint8_t expected_flags = 0;
    expected_flags |= (1 << START_BYTE_FLAG) | (1 << LENGTH_BYTE_FLAG) | (1 << STOP_BYTE_FLAG);

    uCOMMS_CONTEXT expected = {
        .cmd_len      = 4,
        .comms_flags  = expected_flags,
        .curr_cmd_len = 4,
        .comms_buf    = {'G', 'E', 'T', ':', '\0'},
    };


    printf("[EXPECTED]: %d : [GOT]: %d\n", expected.cmd_len, ctx.cmd_len);
    printf("[EXPECTED]: %d : [GOT]: %d\n", expected.comms_flags, ctx.comms_flags);
    printf("[EXPECTED]: %d : [GOT]: %d\n", expected.curr_cmd_len, ctx.curr_cmd_len);
    printf("[EXPECTED]: %s : [GOT]: %s\n", expected.comms_buf, ctx.comms_buf);
    printf("    - Test payload byte parsing\n");

    TEST_ASSERT(compare_contexts(&ctx, &expected), "ctx should match expected");
    return 1; // Placeholder - replace with actual test
}

int test_parse_stop_byte(void) {
    uCOMMS_CONTEXT ctx = {0};

    parse_cmd(&ctx, START_BYTE);
    parse_cmd(&ctx, 10);
    parse_cmd(&ctx, STOP_BYTE);
    
    TEST_ASSERT(ctx.comms_flags & (1 << START_BYTE_FLAG),  "START_BYTE should set START_BYTE_FLAG");
    TEST_ASSERT(ctx.comms_flags & (1 << LENGTH_BYTE_FLAG), "LENGTH_BYTE should set LENGTH_BYTE_FLAG"); 
    TEST_ASSERT(ctx.comms_flags & (1 << STOP_BYTE_FLAG),   "STOP BYTE should set STOP_BYTE_FLAG");

    printf("    - Test stop byte parsing\n");
    return 1; // Placeholder - replace with actual test
}



int test_parse_invalid_sequence(void) {
    printf("    - Test invalid byte sequences (should crash)\n");
    
    // Test 1: STOP without START should crash
    TEST_EXPECT_CRASH({
        uCOMMS_CONTEXT ctx1 = {0};
        parse_cmd(&ctx1, STOP_BYTE);
    }, "STOP without START should crash");
    
    // Test 2: Length byte without START should crash
    TEST_EXPECT_CRASH({
        uCOMMS_CONTEXT ctx2 = {0};
        parse_cmd(&ctx2, 10);  // Should crash since no START_BYTE
    }, "Length byte without START should crash");
    
    // Test 3: Payload without START should crash
    TEST_EXPECT_CRASH({
        uCOMMS_CONTEXT ctx3 = {0};
        parse_cmd(&ctx3, 'A');  // Should crash since no START_BYTE
    }, "Payload without START should crash");
    
    // Test 4: Payload without length should crash
    TEST_EXPECT_CRASH({
        uCOMMS_CONTEXT ctx5 = {0};
        parse_cmd(&ctx5, START_BYTE);
        parse_cmd(&ctx5, 'A');  // Should crash - payload without setting length first
    }, "Payload without length should crash");
    
    // Test 5: STOP without proper length should crash
    TEST_EXPECT_CRASH({
        uCOMMS_CONTEXT ctx6 = {0};
        parse_cmd(&ctx6, START_BYTE);
        parse_cmd(&ctx6, 5);    // Expect 5 bytes
        parse_cmd(&ctx6, 'A');  // Only send 1 byte
        parse_cmd(&ctx6, STOP_BYTE);  // Should crash - length mismatch
    }, "STOP with length mismatch should crash");
    
    return 1;
}



int test_buffer_overflow_should_crash(void) {
    printf("    - Test that buffer overflow crashes (expected behavior)\n");
    
    // This should crash because buffer overflow protection
    TEST_EXPECT_CRASH({
        uCOMMS_CONTEXT ctx = {0};
        parse_cmd(&ctx, START_BYTE);
        parse_cmd(&ctx, 70);  // This should trigger overflow protection
    }, "Buffer overflow should crash");
    
    return 1;
}

int test_null_pointer_should_crash(void) {
    printf("    - Test that NULL pointer crashes (expected behavior)\n");
    
    // This should crash because your CHECK_PTR macro calls exit()
    TEST_EXPECT_CRASH(
        parse_cmd(NULL, START_BYTE), 
        "parse_cmd with NULL context should crash"
    );
    
    return 1;
}


int main(void) {
    printf("=== uComms Parse Function Tests ===\n\n");
    
    // Run all tests
    // RUN_TEST(test_reset_comms_context);
    // RUN_TEST(test_parse_start_byte);
    // RUN_TEST(test_parse_length_byte);
    RUN_TEST(test_parse_payload_bytes);
    // RUN_TEST(test_parse_stop_byte);
    // RUN_TEST(test_parse_invalid_sequence);
    // RUN_TEST(test_null_pointer_should_crash);
    // RUN_TEST(test_buffer_overflow_should_crash);
    
    // Print summary
    printf("=== Test Summary ===\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / total_tests * 100);
    
    // Return 0 for success, 1 for failure
    return (tests_failed == 0) ? 0 : 1;
}
