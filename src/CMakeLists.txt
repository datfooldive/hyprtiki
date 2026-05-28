set(MUSIC_FILE "${CMAKE_SOURCE_DIR}/music.mp3")
set(EMBED_HEADER "${CMAKE_BINARY_DIR}/generated/music_data.h")

if(NOT EXISTS "${MUSIC_FILE}")
    message(FATAL_ERROR "music.mp3 not found in project root! Place your audio file there.")
endif()

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")

add_custom_command(
    OUTPUT "${EMBED_HEADER}"
    COMMAND python3 "${CMAKE_SOURCE_DIR}/embed_audio.py" "${MUSIC_FILE}" "${EMBED_HEADER}"
    DEPENDS "${MUSIC_FILE}" "${CMAKE_SOURCE_DIR}/embed_audio.py"
    COMMENT "Embedding audio file into header"
)

add_custom_target(generate_audio_header DEPENDS "${EMBED_HEADER}")

add_library(hyprtiki SHARED
    main.cpp
)

target_include_directories(hyprtiki PRIVATE
    ${HYPRLAND_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/generated
)

target_link_libraries(hyprtiki PRIVATE
    ${HYPRLAND_LIBRARIES}
    pthread
    m
)

target_compile_features(hyprtiki PRIVATE cxx_std_23)

add_dependencies(hyprtiki generate_audio_header)

install(TARGETS hyprtiki DESTINATION ${CMAKE_INSTALL_LIBDIR}/hyprland/plugins)
