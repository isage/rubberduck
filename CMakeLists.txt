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

add_executable(${PROJECT_NAME}
  src/main.c
  src/compat.c
  src/memfuncs.c
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
  SceSblAIMgrForDriver_stub
  SceCtrlForDriver_stub
  SceKernelSuspendForDriver_stub
  SceModulemgrForDriver_stub
  SceSysclibForDriver_stub
  SceSysmemForDriver_stub
  SceThreadmgrForDriver_stub
  SceUsbdForDriver_stub
  SceUsbServForDriver_stub
  SceSysclibForDriver_stub
  SceNetPsForDriver_stub
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS "-nostdlib"
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES COMPILE_FLAGS "-fno-exceptions -mfloat-abi=soft"
)

vita_create_self(${PROJECT_NAME}.skprx ${PROJECT_NAME}
  CONFIG rubberduck.yml
  UNSAFE
)

