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
if(NOT InJKA_Proxy)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InJKA_Proxy)

set(MPSharedDefines ${SharedDefines})

set(JKA_ProxyIncludeDirectories "${JKA_ProxyDir}")

if(WIN32)
	set(JKA_ProxyLibraries "winmm")
endif(WIN32)

set(JKA_ProxyDefines ${MPSharedDefines} "_GAME" )
set(JKA_ProxyMainFiles
	"${JKA_ProxyDir}/JKA_Proxy/Proxy_API.cpp"
	"${JKA_ProxyDir}/JKA_Proxy/Proxy_Client.cpp"
	"${JKA_ProxyDir}/JKA_Proxy/Proxy_Files.cpp"
	"${JKA_ProxyDir}/JKA_Proxy/Proxy_Header.hpp"
	"${JKA_ProxyDir}/JKA_Proxy/Proxy_Imports.cpp"
	"${JKA_ProxyDir}/JKA_Proxy/Proxy_Main.cpp"
	"${JKA_ProxyDir}/JKA_Proxy/Proxy_Shell.hpp"
	"${JKA_ProxyDir}/JKA_Proxy/Proxy_SystemCalls.cpp"
	"${JKA_ProxyDir}/JKA_Proxy/Proxy_Wrappers.cpp"
	)
source_group("" FILES ${JKA_ProxyMainFiles})
set(JKA_ProxyFiles ${JKA_ProxyFiles} ${JKA_ProxyMainFiles})

set(JKA_ProxyDetourFiles
	"${JKA_ProxyDir}/DetourPatcher/DetourPatcher.cpp"
	"${JKA_ProxyDir}/DetourPatcher/DetourPatcher.hpp"
	)
source_group("DetourPatcher" FILES ${JKA_ProxyDetourFiles})
set(JKA_ProxyFiles ${JKA_ProxyFiles} ${JKA_ProxyDetourFiles})

set(JKA_ProxySDKGameFiles
	"${JKA_ProxyDir}/JKA_SDK/ai.hpp"
	"${JKA_ProxyDir}/JKA_SDK/anims.hpp"
	"${JKA_ProxyDir}/JKA_SDK/b_public.hpp"
	"${JKA_ProxyDir}/JKA_SDK/bg_public.hpp"
	"${JKA_ProxyDir}/JKA_SDK/bg_vehicles.hpp"
	"${JKA_ProxyDir}/JKA_SDK/bg_weapons.hpp"
	"${JKA_ProxyDir}/JKA_SDK/disablewarnings.hpp"
	"${JKA_ProxyDir}/JKA_SDK/g_local.hpp"
	"${JKA_ProxyDir}/JKA_SDK/g_public.hpp"
	"${JKA_ProxyDir}/JKA_SDK/g_team.hpp"
	"${JKA_ProxyDir}/JKA_SDK/g_xcvar.hpp"
	"${JKA_ProxyDir}/JKA_SDK/namespace_begin.hpp"
	"${JKA_ProxyDir}/JKA_SDK/namespace_end.hpp"
	"${JKA_ProxyDir}/JKA_SDK/q_color.hpp"
	"${JKA_ProxyDir}/JKA_SDK/qcommon.hpp"
	"${JKA_ProxyDir}/JKA_SDK/q_math.hpp"
	"${JKA_ProxyDir}/JKA_SDK/q_platform.hpp"
	"${JKA_ProxyDir}/JKA_SDK/q_shared.hpp"
	"${JKA_ProxyDir}/JKA_SDK/q_string.hpp"
	"${JKA_ProxyDir}/JKA_SDK/surfaceflags.hpp"
	"${JKA_ProxyDir}/JKA_SDK/sys_public.hpp"
	"${JKA_ProxyDir}/JKA_SDK/tags.hpp"
	"${JKA_ProxyDir}/JKA_SDK/teams.hpp"
	)
source_group("JKA_SDK" FILES ${JKA_ProxySDKGameFiles})
set(JKA_ProxyFiles ${JKA_ProxyFiles} ${JKA_ProxySDKGameFiles})

add_library(${JKA_Proxy} SHARED ${JKA_ProxyFiles})

if(NOT MSVC)
	# remove "lib" prefix for .so/.dylib files
	set_target_properties(${JKA_Proxy} PROPERTIES PREFIX "")
endif()
set_target_properties(${JKA_Proxy} PROPERTIES COMPILE_DEFINITIONS "${JKA_ProxyDefines}")

# Hide symbols not explicitly marked public.
set_property(TARGET ${JKA_Proxy} APPEND PROPERTY COMPILE_OPTIONS ${JKA_Proxy_VISIBILITY_FLAGS})

set_target_properties(${JKA_Proxy} PROPERTIES INCLUDE_DIRECTORIES "${JKA_ProxyIncludeDirectories}")
set_target_properties(${JKA_Proxy} PROPERTIES PROJECT_LABEL "JKA_Proxy")
# no libraries used
if(JKA_ProxyLibraries)
	target_link_libraries(${JKA_Proxy} ${JKA_ProxyLibraries})
endif(JKA_ProxyLibraries)

set(JKA_ProxyLibsBuilt)
if(BuildJKA_Proxy)
	set(JKA_ProxyLibsBuilt ${JKA_ProxyLibsBuilt} ${JKA_Proxy})
endif()

if(WIN32)
	set(JKA_ProxyLibFullPaths)
	if(MSVC)
		foreach(JKA_ProxyLib ${JKA_ProxyLibsBuilt})
			set(JKA_ProxyLibFullPaths
				${JKA_ProxyLibFullPaths}
				${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${JKA_ProxyLib}${CMAKE_SHARED_LIBRARY_SUFFIX})
		endforeach(JKA_ProxyLib)
	else()
		foreach(JKA_ProxyLib ${JKA_ProxyLibsBuilt})
			set(JKA_ProxyLibFullPaths
				${JKA_ProxyLibFullPaths}
				${CMAKE_BINARY_DIR}/${JKA_ProxyLib}${CMAKE_SHARED_LIBRARY_SUFFIX})
		endforeach(JKA_ProxyLib)
	endif()
endif()
