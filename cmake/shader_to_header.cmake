if (NOT INPUT_FILE OR NOT OUTPUT_FILE OR NOT VARIABLE_NAME)
    message(FATAL_ERROR "INPUT_FILE, OUTPUT_FILE and VARIABLE_NAME must be defined")
endif()

set(INTERMEDIATE_FILE ${CMAKE_CURRENT_BINARY_DIR}/scripts/.tmp.glsl)

# Find python
find_program(PYTHON "python")

# Run scripts/preprocess_shader.py
execute_process(
    COMMAND ${PYTHON} ${SOURCE_DIR}/scripts/preprocess_shader.py ${INPUT_FILE} ${INTERMEDIATE_FILE}
    RESULT_VARIABLE PREPROCESS_RESULT
)

if (NOT PREPROCESS_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to preprocess shader")
endif()

# Read the content of the intermediate file
file(READ ${INTERMEDIATE_FILE} SHADER_CONTENT)

# Replace special characters
string(REPLACE "\"" "\\\"" SHADER_CONTENT "${SHADER_CONTENT}")
string(REPLACE "\n" "\\n" SHADER_CONTENT "${SHADER_CONTENT}")
string(REPLACE "\r" "\\r" SHADER_CONTENT "${SHADER_CONTENT}")
string(REPLACE "\t" "\\t" SHADER_CONTENT "${SHADER_CONTENT}")

# Write the content to the output file
file(WRITE ${OUTPUT_FILE} "const char *${VARIABLE_NAME} = \"${SHADER_CONTENT}\";")
