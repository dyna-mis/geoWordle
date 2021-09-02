	add_compile_definitions("$<$<CONFIG:Debug>:GEOWORDLE_CGAL>")
	add_compile_definitions("$<$<CONFIG:Release>:GEOWORDLE_CGAL>")
if (WIN32)
	add_compile_definitions("$<$<CONFIG:Debug>:GEOWORDLE_PRINT>")
	add_compile_definitions("$<$<CONFIG:Debug>:GEOWORDLE_DRAW>")
	add_compile_definitions("$<$<CONFIG:Debug>:Terminal_PRINT>")
	add_compile_definitions("$<$<CONFIG:Release>:GEOWORDLE_DRAW>")
endif()
# CGAL
	if (WIN32)
			#set(CGAL_DIR /usr/include/CGAL-5.0.3)
			#set(Qt5_DIR /home/guangping/opt/QT5.9.5/5.9.5/gcc_64/lib/cmake/Qt5)
	else (WIN32)
			set(CGAL_DIR /usr/include/CGAL-5.2.1)
	endif()
	
	





# if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    # include(FeatureSummary)
    # feature_summary(WHAT ALL)
# endif()

if(NOT POLICY CMP0070 AND POLICY CMP0053)
  # Only set CMP0053 to OLD with CMake<3.10, otherwise there is a warning.
  cmake_policy(SET CMP0053 OLD)
endif()

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

# list source files here

file (GLOB_RECURSE sources *.h *.cpp *.hpp *.tpp)
include_directories("include")
#set(sources geoWordle.cpp problem.cpp problem_ILP.cpp miscellaneous.cpp problem_CELL.cpp )
add_executable(geoWordle ${sources})
add_executable(geoWordle_pro_ratio ${sources})
add_executable(geoWordle_pro_tolerance ${sources})
add_executable(geoWordle_pro_font ${sources})
add_executable(geoWordle_pro_all_extensions ${sources})
include_directories(solvers)
include_directories(solvers/exact-solvers)
include_directories(solvers/greedy-solvers)
#include_directories(solvers/geometric_solvers)
include_directories(tools)
include_directories(gpl_data_structure)
include_directories(cgal_kernel)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
   set(CMAKE_INSTALL_RPATH='$ORIGIN')
endif ()


option(CXX "enable C++ compilation" ON)
if(CXX)
    enable_language(CXX)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
message (STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")


if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else ()
	#set(CMAKE_CXX_FLAGS "-Wextra")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
	target_compile_features(geoWordle PUBLIC cxx_std_17) 
	target_link_libraries(geoWordle PUBLIC stdc++fs)
	
	target_compile_features(geoWordle_pro_ratio PUBLIC cxx_std_17) 
	target_link_libraries(geoWordle_pro_ratio PUBLIC stdc++fs)
	
	target_compile_features(geoWordle_pro_tolerance PUBLIC cxx_std_17) 
	target_link_libraries(geoWordle_pro_tolerance PUBLIC stdc++fs)
	
		
	target_compile_features(geoWordle_pro_font PUBLIC cxx_std_17) 
	target_link_libraries(geoWordle_pro_font PUBLIC stdc++fs)


	target_compile_features(geoWordle_pro_all_extensions PUBLIC cxx_std_17) 
	target_link_libraries(geoWordle_pro_all_extensions PUBLIC stdc++fs)	
	
endif()


	#settings
if (WIN32)
	find_package(CGAL COMPONENTS Qt5)
	if(CGAL_Qt5_FOUND)
		add_definitions(-DCGAL_USE_BASIC_VIEWER -DQT_NO_KEYWORDS)
	endif()
	if ( CGAL_FOUND )
	target_link_libraries(geoWordle PRIVATE ${GMP_LIBRARIES})
	target_link_libraries(geoWordle_pro_ratio PRIVATE ${GMP_LIBRARIES})
	target_link_libraries(geoWordle_pro_tolerance PRIVATE ${GMP_LIBRARIES})
	target_link_libraries(geoWordle_pro_font PRIVATE ${GMP_LIBRARIES})
	target_link_libraries(geoWordle_pro_all_extensions PRIVATE ${GMP_LIBRARIES})
	#target_link_libraries(geoWordle PRIVATE ${GMPXX_LIBRARIES})
	if(CGAL_Qt5_FOUND)
		target_link_libraries(geoWordle PUBLIC CGAL::CGAL_Qt5 ${GMP_LIBRARIES})
		target_link_libraries(geoWordle_pro_ratio PUBLIC CGAL::CGAL_Qt5 ${GMP_LIBRARIES})
		target_link_libraries(geoWordle_pro_tolerance PUBLIC CGAL::CGAL_Qt5 ${GMP_LIBRARIES})
		target_link_libraries(geoWordle_pro_font PUBLIC CGAL::CGAL_Qt5 ${GMP_LIBRARIES})
		target_link_libraries(geoWordle_pro_all_extensions PUBLIC CGAL::CGAL_Qt5 ${GMP_LIBRARIES})
	endif()
	else()
		message(STATUS "This program requires the CGAL library, and will not be compiled.")
	endif()
else (WIN32)
	find_package(CGAL REQUIRED)
	if ( CGAL_FOUND )
	target_link_libraries(geoWordle PRIVATE CGAL::CGAL ${GMP_LIBRARIES})
	target_link_libraries(geoWordle_pro_ratio PRIVATE CGAL::CGAL ${GMP_LIBRARIES})
	target_link_libraries(geoWordle_pro_tolerance PRIVATE CGAL::CGAL ${GMP_LIBRARIES})
	target_link_libraries(geoWordle_pro_font PRIVATE CGAL::CGAL ${GMP_LIBRARIES})
	target_link_libraries(geoWordle_pro_all_extensions PRIVATE CGAL::CGAL ${GMP_LIBRARIES})
	#target_link_libraries(geoWordle PRIVATE ${GMPXX_LIBRARIES})
	else()
		message(STATUS "This program requires the CGAL library, and will not be compiled.")
	endif()
endif()





# Visual Studio compiler with static runtime libraries
if(MSVC AND MT)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
	
endif()

find_package(GUROBI REQUIRED)
#find_package(ortools REQUIRED)
include_directories(${GUROBI_INCLUDE_DIRS})
message(STATUS "GUROBI_INCLUDE_DIRS: ${GUROBI_INCLUDE_DIRS}")
message(STATUS "GUROBI_CXX_LIBRARY: ${GUROBI_CXX_LIBRARY}")
message(STATUS "GUROBI_CXX_DEBUG_LIBRARY: ${GUROBI_CXX_DEBUG_LIBRARY}")


#CAIRO

if (WIN32)
	find_package(unofficial-cairo CONFIG REQUIRED)
	set (cairo_lib unofficial::cairo::cairo)
	target_link_libraries(geoWordle PRIVATE ${cairo_lib})
	target_link_libraries(geoWordle_pro_ratio PRIVATE ${cairo_lib})
	target_link_libraries(geoWordle_pro_tolerance PRIVATE ${cairo_lib})
	target_link_libraries(geoWordle_pro_font PRIVATE ${cairo_lib})
	target_link_libraries(geoWordle_pro_all_extensions PRIVATE ${cairo_lib})
else(WIN32)
	#find_package(Cairo)
	#include_directories(${Cairo_INCLUDE_DIRS})
	#set(cairo_lib ${Cairo_LIBRARIES})
endif()


find_package(Boost REQUIRED)
target_link_libraries(geoWordle PUBLIC Boost::boost)
target_link_libraries(geoWordle_pro_ratio PUBLIC Boost::boost)
target_link_libraries(geoWordle_pro_tolerance PUBLIC Boost::boost)
target_link_libraries(geoWordle_pro_font PUBLIC Boost::boost)
target_link_libraries(geoWordle_pro_all_extensions PUBLIC Boost::boost)
if(CXX)
    set(CMAKE_CXX_STANDARD 11)
    target_link_libraries(geoWordle PUBLIC optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
	target_link_libraries(geoWordle_pro_ratio PUBLIC optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
	target_link_libraries(geoWordle_pro_tolerance PUBLIC optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
	target_link_libraries(geoWordle_pro_font PUBLIC optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
	target_link_libraries(geoWordle_pro_all_extensions PUBLIC optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
endif()

target_link_libraries(geoWordle PUBLIC ${GUROBI_LIBRARY})
target_link_libraries(geoWordle_pro_ratio PUBLIC ${GUROBI_LIBRARY})
target_link_libraries(geoWordle_pro_tolerance PUBLIC ${GUROBI_LIBRARY})
target_link_libraries(geoWordle_pro_font PUBLIC ${GUROBI_LIBRARY})
target_link_libraries(geoWordle_pro_all_extensions PUBLIC ${GUROBI_LIBRARY})
 

# if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    # include(FeatureSummary)
    # feature_summary(WHAT ALL)
# endif()




set_property(TARGET geoWordle PROPERTY CXX_STANDARD 17)
set_property(TARGET geoWordle PROPERTY CXX_STANDARD_REQUIRED ON)



set_property(TARGET geoWordle_pro_ratio PROPERTY CXX_STANDARD 17)
set_property(TARGET geoWordle_pro_ratio PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET geoWordle_pro_tolerance PROPERTY CXX_STANDARD 17)
set_property(TARGET geoWordle_pro_tolerance PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET geoWordle_pro_font PROPERTY CXX_STANDARD 17)
set_property(TARGET geoWordle_pro_font PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET geoWordle_pro_all_extensions PROPERTY CXX_STANDARD 17)
set_property(TARGET geoWordle_pro_all_extensions PROPERTY CXX_STANDARD_REQUIRED ON)
# pro: with text
target_compile_definitions(geoWordle_pro_ratio PUBLIC GEOWORDLE_PRO)
target_compile_definitions(geoWordle_pro_tolerance PUBLIC GEOWORDLE_PRO)
target_compile_definitions(geoWordle_pro_font PUBLIC GEOWORDLE_PRO)
target_compile_definitions(geoWordle_pro_all_extensions PUBLIC GEOWORDLE_PRO)
#ratio: width-height ratio constraints


target_compile_definitions(geoWordle_pro_ratio PUBLIC GEOWORDLE_PRO_RATIO)
target_compile_definitions(geoWordle_pro_all_extensions PUBLIC GEOWORDLE_PRO_RATIO)



#tolerance: miss-cover tolerance
target_compile_definitions(geoWordle_pro_tolerance PUBLIC GEOWORDLE_PRO_Tolerance)
target_compile_definitions(geoWordle_pro_all_extensions PUBLIC GEOWORDLE_PRO_Tolerance)

#tolerance: minimum font size
target_compile_definitions(geoWordle_pro_font PUBLIC GEOWORDLE_PRO_Font_Size)
target_compile_definitions(geoWordle_pro_all_extensions PUBLIC GEOWORDLE_PRO_Font_Size)


target_compile_definitions(geoWordle_pro_ratio PUBLIC GEOWORDLE_PRO_Extend)
target_compile_definitions(geoWordle_pro_font PUBLIC GEOWORDLE_PRO_Extend)
target_compile_definitions(geoWordle_pro_all_extensions PUBLIC GEOWORDLE_PRO_Extend)


