include "Dependencies.lua"

workspace "VanaheimEngine"
	architecture "x86"
	startproject "Vanir"
	system "windows"
	
	configurations
	{
		"Debug",
		"Release"
	}
	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}.%{cfg.architecture}"

group "Dependencies"
	include "Libs/IMGUI"
	include "Libs/Premake"
	include "Libs/yaml_cpp"
group ""

include "VanaheimEngine"
include "Game"
include "Vanir"