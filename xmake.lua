target("Tools")
    set_kind("headeronly")

    add_packages("spdlog", {public = true, wchar = true})

    add_headerfiles("include/**.h", {public = true})

function add_recursive_includes(dir)
    add_includedirs(dir, {public = true})
    local subdirs = os.dirs(dir .. "/*")
    for _, subdir in ipairs(subdirs) do
        add_recursive_includes(subdir)
    end
end

add_recursive_includes("include")