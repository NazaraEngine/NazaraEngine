if has_config("embed_plugins", "static") then
    set_kind("static")
else
    set_kind("shared")
end

includes("*/xmake.lua")