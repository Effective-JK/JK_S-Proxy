#============================================================================
# Copyright (C) 2013 - 2018, OpenJK contributors
#
# This file is part of the OpenJK source code.
#
# OpenJK is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
#============================================================================

# Make sure the user is not executing this script directly
if(NOT InJKL_Proxy)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InJKL_Proxy)

set(MPSharedDefines ${SharedDefines})

set(JKL_ProxyIncludeDirectories "${JKL_ProxyDir}")

if(WIN32)
	set(JKL_ProxyLibraries "winmm")
endif(WIN32)

set(JKL_ProxyDefines ${MPSharedDefines} "_GAME" )
set(JKL_ProxyMainFiles
	"${JKL_ProxyDir}/JKL_Proxy/Proxy_Client.c"
	"${JKL_ProxyDir}/JKL_Proxy/Proxy_Header.h"
	"${JKL_ProxyDir}/JKL_Proxy/Proxy_Imports.c"
	"${JKL_ProxyDir}/JKL_Proxy/Proxy_Main.c"
	"${JKL_ProxyDir}/JKL_Proxy/Proxy_SystemCalls.c"
	"${JKL_ProxyDir}/JKL_Proxy/Proxy_Wrappers.c"
	)
source_group("" FILES ${JKL_ProxyMainFiles})
set(JKL_ProxyFiles ${JKL_ProxyFiles} ${JKL_ProxyMainFiles})

set(JKL_ProxySDKGameFiles
	"${JKL_ProxyDir}/JKA_SDK/ai.h"
	"${JKL_ProxyDir}/JKA_SDK/anims.h"
	"${JKL_ProxyDir}/JKA_SDK/b_public.h"
	"${JKL_ProxyDir}/JKA_SDK/bg_public.h"
	"${JKL_ProxyDir}/JKA_SDK/bg_vehicles.h"
	"${JKL_ProxyDir}/JKA_SDK/bg_weapons.h"
	"${JKL_ProxyDir}/JKA_SDK/disablewarnings.h"
	"${JKL_ProxyDir}/JKA_SDK/g_local.h"
	"${JKL_ProxyDir}/JKA_SDK/g_public.h"
	"${JKL_ProxyDir}/JKA_SDK/g_team.h"
	"${JKL_ProxyDir}/JKA_SDK/namespace_begin.h"
	"${JKL_ProxyDir}/JKA_SDK/namespace_end.h"
	"${JKL_ProxyDir}/JKA_SDK/q_shared.h"
	"${JKL_ProxyDir}/JKA_SDK/surfaceflags.h"
	"${JKL_ProxyDir}/JKA_SDK/tags.h"
	"${JKL_ProxyDir}/JKA_SDK/teams.h"
	)
source_group("JKA_SDK" FILES ${JKL_ProxySDKGameFiles})
set(JKL_ProxyFiles ${JKL_ProxyFiles} ${JKL_ProxySDKGameFiles})

add_library(${JKL_Proxy} SHARED ${JKL_ProxyFiles})

if(NOT MSVC)
	# remove "lib" prefix for .so/.dylib files
	set_target_properties(${JKL_Proxy} PROPERTIES PREFIX "")
endif()
set_target_properties(${JKL_Proxy} PROPERTIES COMPILE_DEFINITIONS "${JKL_ProxyDefines}")

# Hide symbols not explicitly marked public.
set_property(TARGET ${JKL_Proxy} APPEND PROPERTY COMPILE_OPTIONS ${JKL_Proxy_VISIBILITY_FLAGS})

set_target_properties(${JKL_Proxy} PROPERTIES INCLUDE_DIRECTORIES "${JKL_ProxyIncludeDirectories}")
set_target_properties(${JKL_Proxy} PROPERTIES PROJECT_LABEL "JKL_Proxy")
# no libraries used
if(JKL_ProxyLibraries)
	target_link_libraries(${JKL_Proxy} ${JKL_ProxyLibraries})
endif(JKL_ProxyLibraries)

set(JKL_ProxyLibsBuilt)
if(BuildJKL_Proxy)
	set(JKL_ProxyLibsBuilt ${JKL_ProxyLibsBuilt} ${JKL_Proxy})
endif()

if(WIN32)
	set(JKL_ProxyLibFullPaths)
	if(MSVC)
		foreach(JKL_ProxyLib ${JKL_ProxyLibsBuilt})
			set(JKL_ProxyLibFullPaths
				${JKL_ProxyLibFullPaths}
				${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${JKL_ProxyLib}${CMAKE_SHARED_LIBRARY_SUFFIX})
		endforeach(JKL_ProxyLib)
	else()
		foreach(JKL_ProxyLib ${JKL_ProxyLibsBuilt})
			set(JKL_ProxyLibFullPaths
				${JKL_ProxyLibFullPaths}
				${CMAKE_BINARY_DIR}/${JKL_ProxyLib}${CMAKE_SHARED_LIBRARY_SUFFIX})
		endforeach(JKL_ProxyLib)
	endif()
endif()