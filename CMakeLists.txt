cmake_minimum_required(VERSION 3.10)

project(terrain_gen)

add_subdirectory(thirdparty/SDL)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES
    thirdparty/glad/src/glad.c
    src/main.cpp
)

add_executable(terrain_gen ${SOURCES})

target_link_libraries(terrain_gen PRIVATE SDL3::SDL3)

target_include_directories(terrain_gen PRIVATE ${SDL3_SOURCE_DIR}/include)
target_include_directories(terrain_gen PRIVATE thirdparty/glad/include)
target_include_directories(terrain_gen PRIVATE thirdparty/MatrixUtil/MatrixUtil/include)

# pack shaders into header files
set(SHADER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_OUT_DIR include/shaders)

set(COMPOSITE_FRAG_FILE ${SHADER_SRC_DIR}/composite.frag)
set(COMPOSITE_FRAG_HEADER ${SHADER_OUT_DIR}/composite.frag.h)
set(COMPOSITE_FRAG_NAME composite_frag_source)

set(GENERIC_FRAG_FILE ${SHADER_SRC_DIR}/generic.frag)
set(GENERIC_FRAG_HEADER ${SHADER_OUT_DIR}/generic.frag.h)
set(GENERIC_FRAG_NAME generic_frag_source)

set(GENERIC_VERT_FILE ${SHADER_SRC_DIR}/generic.vert)
set(GENERIC_VERT_HEADER ${SHADER_OUT_DIR}/generic.vert.h)
set(GENERIC_VERT_NAME generic_vert_source)

set(SCREEN_VERT_FILE ${SHADER_SRC_DIR}/screen.vert)
set(SCREEN_VERT_HEADER ${SHADER_OUT_DIR}/screen.vert.h)
set(SCREEN_VERT_NAME screen_vert_source)

set(SKYBOX_FRAG_FILE ${SHADER_SRC_DIR}/skybox.frag)
set(SKYBOX_FRAG_HEADER ${SHADER_OUT_DIR}/skybox.frag.h)
set(SKYBOX_FRAG_NAME skybox_frag_source)

set(SKYBOX_VERT_FILE ${SHADER_SRC_DIR}/skybox.vert)
set(SKYBOX_VERT_HEADER ${SHADER_OUT_DIR}/skybox.vert.h)
set(SKYBOX_VERT_NAME skybox_vert_source)


add_custom_command(
	OUTPUT ${COMPOSITE_FRAG_HEADER}
	COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DINPUT_FILE=${COMPOSITE_FRAG_FILE} -DOUTPUT_FILE=${COMPOSITE_FRAG_HEADER} -DVARIABLE_NAME=${COMPOSITE_FRAG_NAME} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/shader_to_header.cmake
	DEPENDS ${COMPOSITE_FRAG_FILE}
	COMMENT "Generating ${COMPOSITE_FRAG_HEADER}"
)

add_custom_command(
	OUTPUT ${GENERIC_FRAG_HEADER}
	COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DINPUT_FILE=${GENERIC_FRAG_FILE} -DOUTPUT_FILE=${GENERIC_FRAG_HEADER} -DVARIABLE_NAME=${GENERIC_FRAG_NAME} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/shader_to_header.cmake
	DEPENDS ${GENERIC_FRAG_FILE}
	COMMENT "Generating ${GENERIC_FRAG_HEADER}"
)

add_custom_command(
	OUTPUT ${GENERIC_VERT_HEADER}
	COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DINPUT_FILE=${GENERIC_VERT_FILE} -DOUTPUT_FILE=${GENERIC_VERT_HEADER} -DVARIABLE_NAME=${GENERIC_VERT_NAME} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/shader_to_header.cmake
	DEPENDS ${GENERIC_VERT_FILE}
	COMMENT "Generating ${GENERIC_VERT_HEADER}"
)

add_custom_command(
	OUTPUT ${SCREEN_VERT_HEADER}
	COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DINPUT_FILE=${SCREEN_VERT_FILE} -DOUTPUT_FILE=${SCREEN_VERT_HEADER} -DVARIABLE_NAME=${SCREEN_VERT_NAME} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/shader_to_header.cmake
	DEPENDS ${SCREEN_VERT_FILE}
	COMMENT "Generating ${SCREEN_VERT_HEADER}"
)

add_custom_command(
	OUTPUT ${SKYBOX_FRAG_HEADER}
	COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DINPUT_FILE=${SKYBOX_FRAG_FILE} -DOUTPUT_FILE=${SKYBOX_FRAG_HEADER} -DVARIABLE_NAME=${SKYBOX_FRAG_NAME} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/shader_to_header.cmake
	DEPENDS ${SKYBOX_FRAG_FILE}
	COMMENT "Generating ${SKYBOX_FRAG_HEADER}"
)

add_custom_command(
	OUTPUT ${SKYBOX_VERT_HEADER}
	COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DINPUT_FILE=${SKYBOX_VERT_FILE} -DOUTPUT_FILE=${SKYBOX_VERT_HEADER} -DVARIABLE_NAME=${SKYBOX_VERT_NAME} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/shader_to_header.cmake
	DEPENDS ${SKYBOX_VERT_FILE}
	COMMENT "Generating ${SKYBOX_VERT_HEADER}"
)

add_custom_target(generate_terrain_gen_shaders DEPENDS
    ${COMPOSITE_FRAG_HEADER}
    ${GENERIC_FRAG_HEADER}
    ${GENERIC_VERT_HEADER}
    ${SCREEN_VERT_HEADER}
    ${SKYBOX_FRAG_HEADER}
    ${SKYBOX_VERT_HEADER}
)

add_dependencies(terrain_gen generate_terrain_gen_shaders)

target_include_directories(terrain_gen PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)
