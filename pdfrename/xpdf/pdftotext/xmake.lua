set_languages("c99", "cxx17")
set_toolchains("clang")

target("xpdf")
    set_kind("static")
	add_includedirs("../xpdf", "../goo", "../fofi", "../splash")
	add_files("../xpdf/*.cc")
	add_cxflags("-fPIC", "-O3", "-g3")

target("goo")
    set_kind("static")
	add_includedirs("../xpdf", "../goo", "../fofi", "../splash")
	add_files("../goo/*.c", "../goo/*.cc")
	add_cxflags("-fPIC", "-O3", "-g3")

target("fofi")
    set_kind("static")
	add_includedirs("../xpdf", "../goo", "../fofi", "../splash")
	add_files("../fofi/*.cc")
	add_cxflags("-fPIC", "-O3", "-g3")

target("pdftotext")
	set_kind("binary")
	add_deps("xpdf", "goo", "fofi")
	add_includedirs("../xpdf", "../goo", "../fofi", "../splash")
	add_files("pdftotext.cc")
	add_cxflags("-O3", "-g3")

