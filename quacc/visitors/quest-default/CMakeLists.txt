#***********************************************************************************
# Copyright (c) 2019, UT-Battelle
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#   * Neither the name of the xacc nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#**********************************************************************************/

set (LIBRARY_NAME quest-default)

#if (DEFINED ENV{MKLROOT})
#  message(STATUS "MKLROOT is set. For ITensor we force use of ATLAS BLAS")
#  unset(ENV{MKLROOT})
#  set(BLA_VENDOR Generic)
#endif()

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

file (GLOB HEADERS mps/*.hpp)
set (SRC mps/QuestDefaultVisitor.cpp
		 questDefaultActivator.cpp
	)
         
#set(USER_SOURCE  "mps/QuestDefaultVisitor.cpp"  CACHE STRING "Source to build with QuEST library")
#set(OUTPUT_EXE   "demo"  CACHE STRING "Executable to compile to")

usFunctionGetResourceSource(TARGET ${LIBRARY_NAME} OUT SRC)
usFunctionGenerateBundleInit(TARGET ${LIBRARY_NAME} OUT SRC)

# -----------------------------------------------------------------------------
# ----- QuEST LIBRARY ---------------------------------------------------------
# ----------------------------------------------------------------------------- 

# Build the QuEST library if the path to libQuEST.so is not specified
if (NOT DEFINED ${QuEST_LIB_PATH})
	set(QuEST_DIR "QuEST")
	add_subdirectory(${QuEST_DIR})
	set(QuEST_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/${QuEST_DIR}")
	set(QuEST_LIB_EXACT "${QuEST_LIB_PATH}/libQuEST.so")
endif()

# -----------------------------------------------------------------------------
# ----- USER EXECUTABLE -------------------------------------------------------
# -----------------------------------------------------------------------------

if (NOT TESTING)
    #message(STATUS "Compiling ${USER_SOURCE} to executable ${OUTPUT_EXE}")

    # Create user executable
    #add_executable(${OUTPUT_EXE} ${USER_SOURCE})

    # Link libraries to user executable, including QuEST library
    if (WIN32)
    #   target_link_libraries(${OUTPUT_EXE} QuEST)
    else ()
    #    target_link_libraries(${OUTPUT_EXE} QuEST m)
    endif()
endif()

# -----------------------------------------------------------------------------
# ----- TESTS -----------------------------------------------------------------
# -----------------------------------------------------------------------------

if (TESTING)
    message(STATUS "Compiling unit tests")

    enable_testing()
    add_subdirectory(tests)
endif()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result -O2 -DNDEBUG -DPLATFORM_lapack -D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=0")
add_library(${LIBRARY_NAME} SHARED ${SRC})

set(_bundle_name quacc_quest_default)
set_target_properties(${LIBRARY_NAME} PROPERTIES
    # This is required for every bundle
    COMPILE_DEFINITIONS US_BUNDLE_NAME=${_bundle_name}
    # This is for convenience, used by other CMake functions
    US_BUNDLE_NAME ${_bundle_name}
    )

# Embed meta-data from a manifest.json file
usFunctionEmbedResources(TARGET ${LIBRARY_NAME}
    WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
    manifest.json
    )

target_include_directories(${LIBRARY_NAME} PUBLIC ${ITENSOR_ROOT} mps ${XACC_INCLUDE_ROOT}/eigen QuEST)
target_link_libraries(${LIBRARY_NAME} PUBLIC xacc::xacc xacc::quantum_gate ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})

xacc_configure_plugin_rpath(${LIBRARY_NAME})

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins)
install(TARGETS QuEST DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
