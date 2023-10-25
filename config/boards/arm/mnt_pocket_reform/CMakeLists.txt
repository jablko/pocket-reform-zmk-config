zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include/)
zephyr_library_sources(board.c)
zephyr_library_sources(pat9125.c)
zephyr_library_sources_ifndef(CONFIG_PAT9125_TRIGGER_NONE pat9125_trigger.c)
