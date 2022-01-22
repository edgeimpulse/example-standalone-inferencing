cmake_minimum_required(VERSION 3.13.1)

set(MODEL_FOLDER .)

include(${MODEL_FOLDER}/edge-impulse-sdk/cmake/utils.cmake)

add_subdirectory(${MODEL_FOLDER}/edge-impulse-sdk/cmake/zephyr)

target_include_directories(app PRIVATE
    ${MODEL_FOLDER}
    ${MODEL_FOLDER}/tflite-model
    ${MODEL_FOLDER}/model-parameters
)
include_directories(${INCLUDES})

# find model source files
RECURSIVE_FIND_FILE(MODEL_FILES "${MODEL_FOLDER}/tflite-model" "*.cpp")
list(APPEND SOURCE_FILES ${MODEL_FILES})

# add all sources to the project
target_sources(app PRIVATE ${SOURCE_FILES})
