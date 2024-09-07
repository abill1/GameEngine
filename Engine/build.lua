CreateProject("Engine")
forceincludes { 

}
includedirs {
	
}
links {
	"d2d1.lib"
}
libdirs {
	"%{wks.location}/build/bin/%{cfg.architecture}-%{cfg.buildcfg}/",
}

postbuildcommands {
	

}

