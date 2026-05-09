cmake_minimum_required(VERSION 3.24)

project(Savegamefix VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CommonLibSSE REQUIRED)
find_package(spdlog REQUIRED)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME} 
    PRIVATE 
        src/main.cpp
        src/SaveManager.cpp
        src/PhysicsFreezer.cpp
        src/PCBManager.cpp
        src/ThreadSafetyManager.cpp
        src/PCH.h
        src/minhook/buffer.c
        src/minhook/hook.c
        src/minhook/trampoline.c
        src/minhook/hde32.c
        src/minhook/hde64.c
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        CommonLibSSE::CommonLibSSE
        spdlog::spdlog_header_only
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        SKSE_SUPPORT_SSE
        SKSE_SUPPORT_AE
        SKSE_SUPPORT_VR
        PROJECT_NAME="${PROJECT_NAME}"
)

# set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# target_precompiled_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /Zm200)
endif()
