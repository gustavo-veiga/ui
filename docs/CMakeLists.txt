find_package(Doxygen)

# Check if Doxygen is installed
if(DOXYGEN_FOUND)
  #This will be the main output of our command
  set(DOXYGEN_INDEX_FILE ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html)

  add_custom_command(
    OUTPUT ${DOXYGEN_INDEX_FILE}
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    MAIN_DEPENDENCY Doxyfile
    COMMENT "Generating docs"
  )

  add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
else()
  message("Doxygen need to be installed to generate the doxygen documentation.")
endif()

