CreateProject("Engine")
forceincludes { 

}
includedirs {
	
}
links {
	"d2d1.lib",
	"d3d11.lib",
	"d3dcompiler.lib",
	"winmm.lib",
	"Dxgi.lib",
	"freetype.lib"
}
libdirs {
	"%{wks.location}/build/bin/%{cfg.architecture}-%{cfg.buildcfg}/",
	"Vendor/FreeType/x64_libs/%{cfg.buildcfg}/"
}

postbuildcommands {
	

}

