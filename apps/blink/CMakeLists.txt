cmake_minimum_required(VERSION 3.15)

set(TEL_CHIP "-DCHIP_TYPE=CHIP_TYPE_8258")
set(BOOT_FLAG "-DMCU_STARTUP_8258_RET_16K")
set(LS_FLAG "/mnt/d/Rogo/sdk/telink_dev/components/boot/boot_16k_retn_8251_8253_8258.link")
set(INCLUDE_PATHS "-I../blink -I../../components")
set(CMAKE_TOOLCHAIN_FILE ../../toolchains/tc32/tc32_toolchain.cmake)

project(blink C)
enable_language(C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# set(PROJECT_LINK_LIBS liblt_8258.a)
# link_directories(../../components/proj_lib/)

include_directories(../blink)
include_directories(../../components)
# include_directories(../../components/drivers)
# include_directories(../../components/vendor/common)

set(SOURCES 
            ../blink/app_config.h
            ../../components/drivers/8258/adc.c
            ../../components/drivers/8258/gpio_8258.c
            ../../components/drivers/8258/timer.c
            ../../components/drivers/8258/clock.c
            ../../components/drivers/8258/analog.c

            ../../components/boot/8258/cstartup_8258.S
            ../../components/boot/8258/cstartup_8258_RET_16K.S
            ../../components/boot/div_mod.S
            ../blink/main.c
)

set(OBJS
CMakeFiles/dummy.dir/mnt/d/Rogo/sdk/telink_dev/components/drivers/8258/adc.c.o
CMakeFiles/dummy.dir/mnt/d/Rogo/sdk/telink_dev/components/drivers/8258/gpio_8258.c.o
CMakeFiles/dummy.dir/mnt/d/Rogo/sdk/telink_dev/components/drivers/8258/timer.c.o
CMakeFiles/dummy.dir/mnt/d/Rogo/sdk/telink_dev/components/drivers/8258/clock.c.o
CMakeFiles/dummy.dir/mnt/d/Rogo/sdk/telink_dev/components/drivers/8258/analog.c.o
CMakeFiles/dummy.dir/mnt/d/Rogo/sdk/telink_dev/components/boot/8258/cstartup_8258.S.o
CMakeFiles/dummy.dir/mnt/d/Rogo/sdk/telink_dev/components/boot/8258/cstartup_8258_RET_16K.S.o
CMakeFiles/dummy.dir/mnt/d/Rogo/sdk/telink_dev/components/boot/div_mod.S.o
CMakeFiles/dummy.dir/main.c.o
)

# set(EXECUTABLE ${PROJECT_NAME}.elf)
# add_executable(${EXECUTABLE} ${SOURCES})
add_library(dummy OBJECT ${SOURCES})
# target_link_libraries(${EXECUTABLE} ${PROJECT_LINK_LIBS})

# target_link_options(${EXECUTABLE} PRIVATE
#         --gc-sections
#         -L ../../components/proj_lib
#         -T ${LS_FLAGS}
#         -o ${PROJECT_NAME}.elf
#         -llt_8258
#         )

add_custom_target(elf
    COMMAND echo -e "Building elf file..."
    # COMMAND tc32-elf-ld --gc-sections -L ../../components/proj_lib -o blink.elf ${OBJS} -llt_8258
    COMMAND tc32-elf-ld --gc-sections -L ../../components/proj_lib -T ../../components/boot/boot_16k_retn_8251_8253_8258.link -o blink.elf ${OBJS} -llt_8258
)

# add_custom_command(TARGET ${EXECUTABLE}
#         POST_BUILD
#         # COMMAND tc32-elf-ld --gc-sections -L ../../components/proj_lib -T ${LS_FLAGS} -o $(PROJECT_NAME).elf ${SOURCES} -llt_8258
#         COMMAND tc32-elf-ld -v
# )


# target_link_libraries(${PROJECT_NAME}.elf blink)

# add_executable(blink main.c)
# target_link_libraries(blink ${PROJECT_LINK_LIBS} )