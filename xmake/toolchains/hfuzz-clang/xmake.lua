toolchain("hfuzz-clang")
    
    set_homepage("https://honggfuzz.dev")
    set_description("Security oriented software fuzzer. Supports evolutionary, feedback-driven fuzzing based on code coverage (SW and HW based)")

    set_kind("standalone")

    set_toolset("cc", "hfuzz-clang")
    set_toolset("cxx", "hfuzz-clang++")
    set_toolset("ld", "hfuzz-clang++", "hfuzz-clang")
    set_toolset("sh", "hfuzz-clang++", "hfuzz-clang")
    set_toolset("ar", "ar")
    set_toolset("strip", "strip")
    set_toolset("mm", "hfuzz-clang")
    set_toolset("mxx", "hfuzz-clang++")
    set_toolset("as", "hfuzz-clang")

    on_check(function (toolchain)
        return import("lib.detect.find_tool")("hfuzz-clang")
    end)

    on_load(function (toolchain)
        local march
        if toolchain:is_arch("x86_64", "x64") then
            march = "-m64"
        elseif toolchain:is_arch("i386", "x86") then
            march = "-m32"
        end
        if march then
            toolchain:add("cxflags", march)
            toolchain:add("ldflags", march)
            toolchain:add("shflags", march)
        end
    end)
