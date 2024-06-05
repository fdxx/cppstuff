set_languages("c99", "cxx17")
set_toolchains("clang")

target("ocrtest")
	set_kind("binary")
	add_includedirs("../aip-cpp-sdk", "/usr/include/jsoncpp")
	add_files("ocrtest.cpp")
	add_cxflags("-O3", "-g3")
	add_syslinks("jsoncpp", "curl", "crypto")


