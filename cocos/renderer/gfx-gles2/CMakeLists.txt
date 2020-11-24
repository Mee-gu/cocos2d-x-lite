set(TARGET_NAME GFXGLES2)

message(STATUS "Target: ${TARGET_NAME}...")
# ---------------------------------------------------------------------
# set include files

set(INCLUDE_FILES
  ${GFX_EXTERNAL_PATH}/khronos
  ${GFX_EXTERNAL_PATH}/glsl_optimizer/src
  ${GFX_EXTERNAL_PATH}/glsl_optimizer/src/mesa
  ${COCOS_SRC_PATH}/renderer/core
  ${COCOS_SRC_PATH}/renderer/gfx-gles2
)

if(WIN32)
  list(APPEND INCLUDE_FILES ${GFX_EXTERNAL_PATH}/pvrsdk/include)

if(COCOS_CPU_X64)
  set(PVRSDK_LIB_DIR ${GFX_EXTERNAL_PATH}/pvrsdk/lib/Windows_x86_64)
else()
  set(PVRSDK_LIB_DIR ${GFX_EXTERNAL_PATH}/pvrsdk/lib/Windows_x86_32)
endif()

  link_libraries(
    ${PVRSDK_LIB_DIR}/libEGL.lib
	  ${PVRSDK_LIB_DIR}/libGLESv2.lib
  )
endif()

if(COCOS_PLATFORM_OSX)
  list(APPEND INCLUDE_FILES ${GFX_EXTERNAL_PATH}/pvrsdk/include)
  set(PVRSDK_LIB_DIR ${GFX_EXTERNAL_PATH}/pvrsdk/lib/OSX_x86)

  link_libraries(
    ${PVRSDK_LIB_DIR}/libEGL.dylib
    ${PVRSDK_LIB_DIR}/libGLESv2.dylib
  )
endif()

include_directories( ${INCLUDE_FILES} )

# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES *.h)
file(GLOB_RECURSE SOURCE_CPP_FILES *.cpp *.cc)
file(GLOB_RECURSE SOURCE_C_FILES *.c)

if(COCOS_PLATFORM_IOS OR COCOS_PLATFORM_OSX)
	file(GLOB_RECURSE SOURCE_MM_FILES *.m *.mm)
endif()

set(ALL_FILES 
  ${HEADER_FILES} 
  ${SOURCE_CPP_FILES} 
  ${SOURCE_C_FILES} 
  ${CORE_FILES})

if(COCOS_PLATFORM_IOS OR COCOS_PLATFORM_OSX)
	set(ALL_FILES ${ALL_FILES} ${SOURCE_MM_FILES})
endif()

if(COCOS_PLATFORM_IOS OR COCOS_PLATFORM_OSX)
	list(REMOVE_ITEM ALL_FILES "${COCOS_SRC_PATH}/renderer/gfx-gles2/gles2w.c")
else()
	list(REMOVE_ITEM ALL_FILES "${COCOS_SRC_PATH}/renderer/gfx-gles2/gles2w.mm")
endif()

if(COCOS_PLATFORM_OSX)
  list(REMOVE_ITEM ALL_FILES "${COCOS_SRC_PATH}/renderer/gfx-gles2/GLES2Context.mm")
endif()

add_definitions("-DCC_GLES2_EXPORTS")
if(WIN32)
	add_definitions("-DGLEW_BUILD")
endif()
if(COCOS_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()

if (COCOS_STATIC)
	add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${ALL_FILES})
else()
	add_library(${TARGET_NAME} SHARED ${HEADER_FILES} ${ALL_FILES})
endif()
find_library(APPKIT_LIBRARY AppKit)
find_library(ICONV_LIBRARY iconv)
find_library(AUDIOTOOLBOX_LIBRARY AudioToolBox)
find_library(FOUNDATION_LIBRARY Foundation)
find_library(OPENAL_LIBRARY OpenAL)
find_library(GAMECONTROLLER_LIBRARY GameController)
find_library(METAL_LIBRARY Metal)
find_library(METALLIT_LIBRARY MetalKit)
find_library(SQLITE3_LIBRARY SQLite3)
find_library(SECURITY_LIBRARY Security)
find_library(SystemConfiguration_LIBRARY SystemConfiguration)
find_library(OPENGL_LIBRARY OpenGL)
find_library(APPKIT_LIBRARY AppKit)
list(APPEND PLATFORM_SPECIFIC_LIBSSS
     ${APPKIT_LIBRARY}
     ${ICONV_LIBRARY}
    ${AUDIOTOOLBOX_LIBRARY}
    ${FOUNDATION_LIBRARY}
    ${OPENAL_LIBRARY}
    ${GAMECONTROLLER_LIBRARY}
    ${METAL_LIBRARY}
    ${METALLIT_LIBRARY}
    ${SQLITE3_LIBRARY}
    ${SECURITY_LIBRARY}
    ${SystemConfiguration_LIBRARY}
    ${OPENGL_LIBRARY}
    ${APPKIT_LIBRARY}
)
target_link_libraries(${TARGET_NAME} 
${PLATFORM_SPECIFIC_LIBSSS}  
Core 
  ${CC_EXTERNAL_LIBS}
  z)
target_include_directories(${TARGET_NAME} PUBLIC 
  ${GFX_EXTERNAL_PATH}/khronos 
  ${COCOS_EXTERNAL_PATH}/sources
  ${CC_EXTERNAL_INCLUDES})

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME CCGFXGLES2)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG CCGFXGLES2_d)

use_precompiled_header("${CMAKE_CURRENT_SOURCE_DIR}/GLES2Std.h" "${CMAKE_CURRENT_SOURCE_DIR}/GLES2Std.cpp" SOURCE_CPP_FILES)

message(STATUS "${TARGET_NAME} configuration completed.")
