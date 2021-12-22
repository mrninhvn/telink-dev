# set(CMAKE_SYSTEM_NAME Generic)
# set(CMAKE_SYSTEM_PROCESSOR TC32)

set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

set(CMAKE_AR                        /mnt/d/Rogo/sdk/telink_dev/toolchains/tc32/bin/tc32-elf-ar${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_ASM_COMPILER              /mnt/d/Rogo/sdk/telink_dev/toolchains/tc32/bin/tc32-elf-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_C_COMPILER                /mnt/d/Rogo/sdk/telink_dev/toolchains/tc32/bin/tc32-elf-gcc${CMAKE_EXECUTABLE_SUFFIX})
# set(CMAKE_CXX_COMPILER              /mnt/d/Rogo/sdk/telink_dev/toolchains/tc32/bin/tc32-elf-g++${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_LINKER                    /mnt/d/Rogo/sdk/telink_dev/toolchains/tc32/bin/tc32-elf-ld${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_OBJCOPY                   /mnt/d/Rogo/sdk/telink_dev/toolchains/tc32/bin/tc32-elf-objcopy${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_RANLIB                    /mnt/d/Rogo/sdk/telink_dev/toolchains/tc32/bin/tc32-elf-ranlib${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_SIZE                      /mnt/d/Rogo/sdk/telink_dev/toolchains/tc32/bin/tc32-elf-size${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_STRIP                     /mnt/d/Rogo/sdk/telink_dev/toolchains/tc32/bin/tc32-elf-strip${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")

set(CMAKE_C_FLAGS                   "-ffunction-sections -fdata-sections -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions ${TEL_CHIP} ${BOOT_FLAG} " CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)