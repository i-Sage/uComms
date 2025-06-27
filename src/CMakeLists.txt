# Define the header-only library target
add_library(ucomms_headers INTERFACE)

# Add an alias for easier reference
add_library(uComms::Headers ALIAS ucomms_headers)

# Specify the include directories for the library
target_include_directories(ucomms_headers
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Optional: Set C standard requirements
target_compile_features(ucomms_headers
    INTERFACE
        c_std_11  # or c_std_11, c_std_17, etc.
)

# Optional: Add compile definitions if needed
# target_compile_definitions(ucomms_headers
#     INTERFACE
#         UCOMMS_VERSION_MAJOR=1
#         UCOMMS_VERSION_MINOR=0
# )

# Optional: Add compiler-specific flags
# target_compile_options(ucomms_headers
#     INTERFACE
#         $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra>
# )