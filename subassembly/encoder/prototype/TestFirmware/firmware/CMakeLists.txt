cmake_minimum_required(VERSION 3.15.3)

# Optional: print out extra messages to see what is going on. Comment it to have less verbose messages
set(CMAKE_VERBOSE_MAKEFILE ON)

# Path to toolchain file. This one has to be before 'project()' below
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-none-eabi-gcc.cmake)

# Setup project, output and linker file
project(encoder-prototype-firmware)
set(TARGET_NAME ${PROJECT_NAME}.elf)

set(LINKER_FILE ${CMAKE_SOURCE_DIR}/device/ATSAMD10D13AM.ld)


enable_language(C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


# Optional: issue a message to be sure it uses the correct toolchain file.
message(STATUS "CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")

# List of source files
set(SRC_FILES
        src/main.c
        src/controller.c
        src/encoder/encoder.c
        src/led_array/led_array.c
        src/comms/comms.c
        )

# List of MPLAB X created framework source files
file(GLOB_RECURSE framework_sources CONFIGURE_DEPENDS "src/config/*.c")
set(SRC_FILES
        ${SRC_FILES}
        ${framework_sources}
        )

# Build the executable based on the source files
add_executable(${TARGET_NAME} ${SRC_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES LINK_DEPENDS ${LINKER_FILE})

# List of compiler defines, prefix with -D compiler option
target_compile_definitions(${TARGET_NAME} PRIVATE
        )



# List of include directories
target_include_directories(${TARGET_NAME} PRIVATE
        src 
        device
        src/packs/CMSIS/CMSIS/Core/Include
        src/packs
        src/packs/ATSAMD10D13AM_DFP
        src/packs/ATSAMD10D13AM_DFP/component
        src/packs/ATSAMD10D13AM_DFP/instance
        src/packs/ATSAMD10D13AM_DFP/pio
        src/config/default
        )

# Compiler options
target_compile_options(${TARGET_NAME} PRIVATE
        -mcpu=cortex-m0plus
        -mthumb

        -fdata-sections
        -ffunction-sections

        -Wall
        -O0
        -g3
        )

# Linker options

target_link_options(${TARGET_NAME} PRIVATE
        -T${LINKER_FILE}
        -mcpu=cortex-m0plus
        -mthumb
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        -Xlinker -print-memory-usage -Xlinker
        )

# Optional: Print executable size as part of the post build process
add_custom_command(TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${TARGET_NAME})

# Optional: Create hex, bin and S-Record files after the build
add_custom_command(TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 ${TARGET_NAME} ${PROJECT_NAME}.s19
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${TARGET_NAME} ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET_NAME} ${PROJECT_NAME}.bin)