include "CreateProject.lua"

workspace "GameEngine"
	configurations {"Debug", "Release"}
	architecture "x64"
	location "./"
	startproject "Engine"

	include "./Engine/build.lua"
