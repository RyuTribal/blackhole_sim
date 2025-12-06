cmake_minimum_required(VERSION 3.21)
project(black_hole_simulator VERSION 0.1.0 LANGUAGES CXX C)

option(IS_WEB "Enable WebAssembly build" OFF)

if(IS_WEB)
    message(STATUS "WebAssembly build enabled")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

if(NOT IS_WEB)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
    )
    
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(GLFW_INSTALL OFF CACHE INTERNAL "")

    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.5
    )

    FetchContent_MakeAvailable(glfw imgui)
endif()

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
set(GLM_TEST_ENABLE OFF CACHE INTERNAL "")

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        658f48e72aee3c6582e80b05ac0f8787a64fe6bb
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  cmake
)

FetchContent_MakeAvailable(glm glad)


if(IS_WEB)
    glad_add_library(glad_gl_es_30 STATIC REPRODUCIBLE LOADER API gles2:core=3.0)
else()
    glad_add_library(glad_gl_core_43 STATIC REPRODUCIBLE LOADER API gl:core=4.3)
endif()

if(NOT IS_WEB)
    FetchContent_GetProperties(imgui)
    if(NOT imgui_POPULATED)
        FetchContent_Populate(imgui)
    endif()

    set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )

    add_library(imgui_backend STATIC ${IMGUI_SOURCES})
    target_include_directories(imgui_backend PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )

    target_link_libraries(imgui_backend PUBLIC glfw glad_gl_core_43)
    target_compile_definitions(imgui_backend PUBLIC 
        IMGUI_DISABLE_OBSOLETE_FUNCTIONS 
        IMGUI_IMPL_OPENGL_LOADER_GLAD2 
        GLM_ENABLE_EXPERIMENTAL
    )
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE src extern)

target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_ENABLE_EXPERIMENTAL)

if(IS_WEB)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            glad_gl_es_30
            glm::glm
    )
    
    target_include_directories(${PROJECT_NAME} PRIVATE 
        ${CMAKE_BINARY_DIR}/gladsources/glad_gl_es_30/include
    )
    
    target_compile_definitions(${PROJECT_NAME} PUBLIC IS_WEB)
    
    target_link_options(${PROJECT_NAME} PRIVATE
        "-sUSE_GLFW=3"
        "-sFULL_ES3=1"
        "-sUSE_WEBGL2=1"
        "-sWASM=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "--bind" 
        "-sEXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']"
        --shell-file "${CMAKE_SOURCE_DIR}/web/index.html"
        --preload-file "${CMAKE_SOURCE_DIR}/assets@/assets"
    )
else()
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            glfw
            glad_gl_core_43
            imgui_backend
            glm::glm
    )
    
    target_include_directories(${PROJECT_NAME} PRIVATE 
        ${CMAKE_BINARY_DIR}/gladsources/glad_gl_core_43/include
    )
endif()

if (APPLE AND NOT IS_WEB)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()
