cmake_minimum_required(VERSION 2.8)

PROJECT(cg4_extra)
SET(TARGETNAME cg4_extra)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

find_package(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL TRUE)
INCLUDE(${QT_USE_FILE})

#
# Include macro to generate C++ source from shader files.
#
include(EqStringifyShaders)

eq_stringify_shaders(SHADERS
    fragment-shader.glsl
    vertex-shader.glsl
    geometry-shader.glsl
)

add_executable(extra
    extra.cpp
    Skybox.cpp
    GenericHelper.cpp
    SimpleTexture.cpp
    camera.cpp
    ${SHADERS}
    fragment-shader.glsl
    vertex-shader.glsl
    geometry-shader.glsl
)

add_definitions(-g)

include_directories(${GLEW_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(extra ${GLEW_LIBRARIES} ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS extra RUNTIME DESTINATION bin)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif(CMAKE_COMPILER_IS_GNUCXX)
