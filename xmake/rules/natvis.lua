rule("natvis")
    on_config(function (target)
        for name, pkg in table.orderpairs(target:pkgs()) do
            local includedir = path.join(pkg:installdir(), "include")
            local natvis = os.files(path.join(includedir, "**.natvis"))
            if #natvis > 0 then
                local groups = table.wrap(target:get("filegroups"))
                if not table.find(groups, name) then
                    target:add("extrafiles", natvis)
                    target:add("filegroups", path.join("nativs", name), { rootdir = includedir })
                end
            end
        end
    end)
