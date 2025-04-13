cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(rubberduck C ASM)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(COMPILE_COMMON_FLAGS "-Wl,-q -Wall -O2 -fno-inline")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILE_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_COMMON_FLAGS} -fno-rtti -fno-exceptions")

add_executable(${PROJECT_NAME}
  src/main.c
  src/compat.c
  src/memfuncs.c
  src/primitives.c
  src/net_fixup.c
  src/safeRW.S
  src/duktape/duktape.c
  src/jmp/setjmp.S
)

target_link_libraries(${PROJECT_NAME}
  ffi
  m
  gcc
#  ${CMAKE_SOURCE_DIR}/setjmp.o
  taihenForKernel_stub
  taihenModuleUtils_stub
  SceDebugForDriver_stub
  SceModulemgrForDriver_stub
  SceSysmemForDriver_stub
  SceThreadmgrForDriver_stub
  SceSysclibForDriver_stub
  SceNetPsForDriver_stub
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS "-nostdlib"
  COMPILE_FLAGS ""
)

vita_create_self(${PROJECT_NAME}.skprx ${PROJECT_NAME}
  CONFIG rubberduck.yml
  UNSAFE
)
