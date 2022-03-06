rule("mode.fuzz")
    on_config(function (target)
        if is_mode("fuzz") then
            target:set("symbols", "debug")

            target:add("cxflags", "-fsanitize=address,fuzzer")
            target:add("mxflags", "-fsanitize=address,fuzzer")
            target:add("ldflags", "-fsanitize=address,fuzzer")
            target:add("shflags", "-fsanitize=address,fuzzer")
        end
    end)
