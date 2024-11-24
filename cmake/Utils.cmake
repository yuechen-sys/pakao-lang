#
# Print a message only if the `VERBOSE_OUTPUT` option is on
#

function(verbose_message content)
    if(${PROJECT_NAME}_VERBOSE_OUTPUT)
			message(STATUS ${content})
    endif()
endfunction()

#
# Add a target for formating the project using `clang-format` (i.e: cmake --build build --target clang-format)
#

function(add_clang_format_target)
    if(NOT ${PROJECT_NAME}_CLANG_FORMAT_BINARY)
			find_program(${PROJECT_NAME}_CLANG_FORMAT_BINARY clang-format)
    endif()

    if(${PROJECT_NAME}_CLANG_FORMAT_BINARY)
			if(${PROJECT_NAME}_BUILD_EXECUTABLE)
				add_custom_target(clang-format
						COMMAND ${${PROJECT_NAME}_CLANG_FORMAT_BINARY}
						-i ${exe_sources} ${headers}
						WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
			elseif(${PROJECT_NAME}_BUILD_HEADERS_ONLY)
				add_custom_target(clang-format
						COMMAND ${${PROJECT_NAME}_CLANG_FORMAT_BINARY}
						-i ${headers}
						WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
			else()
				add_custom_target(clang-format
						COMMAND ${${PROJECT_NAME}_CLANG_FORMAT_BINARY}
						-i ${sources} ${headers}
						WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
			endif()

			message(STATUS "Format the project using the `clang-format` target (i.e: cmake --build build --target clang-format).\n")
    endif()
endfunction()

# function(add_lex_yacc_target)
# 	find_program(LEX_EXE
# 		flex
# 	)
# 	if(LEX_EXE STREQUAL "LEX_EXE-NOTFOUND")
# 		message(FATAL_ERROR "dear user, plase install flex!")
# 	endif(LEX_EXE STREQUAL "LEX_EXE-NOTFOUND")

# 	find_program(YACC_EXE
# 		bison
# 	)
# 	if(YACC_EXE STREQUAL "YACC_EXE-NOTFOUND")
# 		message(FATAL_ERROR "dear user, plase install bison!")
# 	endif(YACC_EXE STREQUAL "YACC_EXE-NOTFOUND")

# 	# reuseable cmake macro for yacc
# 	MACRO(YACC_FILE _filename)
# 		GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
# 		ADD_CUSTOM_COMMAND(
# 			OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.c
# 					${CMAKE_CURRENT_BINARY_DIR}/${_basename}.h
# 			COMMAND ${YACC_EXE} -d
# 					--output=${_basename}.c
# 					${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
# 			DEPENDS ${_filename}
# 		)
# 	ENDMACRO(YACC_FILE)

# 	# reuseable cmake macro for lex
# 	MACRO(LEX_FILE _filename)
# 		GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
# 		ADD_CUSTOM_COMMAND(
# 			OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.c
# 			COMMAND ${LEX_EXE}
# 					-o${_basename}.c
# 					${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
# 			DEPENDS ${_filename} )
# 	ENDMACRO(LEX_FILE)

# 	YACC_FILE(config_yacc.y)
# 	LEX_FILE(config_lex.l)

# 	add_executable(my_parsing_exe
# 		... # whatever
# 		config_yacc.c
# 		config_lex.c
# 	)

# 	# just one more parsing executable
# 	YACC_FILE(xyz_yacc.y)
# 	LEX_FILE(xyz_lex.l)

# 	add_executable(xyz_parsing_exe
# 		... # whatever
# 		xyz_yacc.c
# 		xyz_lex.c
# 	)
# endfunction()