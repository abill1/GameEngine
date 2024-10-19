
function CreateProject( projectname )
	project(projectname)
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		characterset("Unicode")
		warnings "Everything"
		
		flags
		{
			"MultiProcessorCompile"
		}

		targetdir "%{wks.location}/build/bin/%{cfg.architecture}-%{cfg.buildcfg}/"
		objdir "%{wks.location}/build/obj/%{cfg.architecture}-%{cfg.buildcfg}/%{prj.name}/"

		location("%{wks.location}/" .. projectname)
		files
		{
			"%{prj.location}/**.h", "%{prj.location}/**.hpp", "%{prj.location}/**.hh", "%{prj.location}/**.hxx",  
            "%{prj.location}/**.c", "%{prj.location}/**.cpp", "%{prj.location}/**.cc", "%{prj.location}/**.cxx",
            "%{prj.location}/**.lua", "%{prj.location}/**.txt", "%{prj.location}/**.md", "%{prj.location}/**.ini","%{prj.location}/**.hlsl"
		}

		includedirs { "%{wks.location}", "%{prj.location}" }

		filter "configurations:Debug"
			defines {"DEBUG", "_DEBUG"}
			symbols "On"
			optimize "On"
			staticruntime "on"
			runtime "Debug"
		filter {}

		filter "configurations:Release"
			defines {"NDEBUG"}
			optimize "Speed"
			staticruntime "on"
			runtime "Release"
		filter {}
		
end

function SetStaticLib( )
	kind "StaticLib"
end

