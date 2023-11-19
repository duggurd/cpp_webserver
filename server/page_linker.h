#include <map>
#include <string>

struct links {
    std::map<std::string, std::string> linker;
};

//linker for page-aliases and corresponding file_names

links link() {
    links linker;
    //insert page aliases here. linker.linker[url path] = target_file;
    linker.linker["/index"] = "index.html";
    linker.linker["/"] = "index.html";
    linker.linker["/about"] = "about.html";
    linker.linker["/styles.css"] = "styles.css";
    linker.linker["/duck.jpg"] = "duck.jpg";

    linker.linker["404"] = "404.html";
    linker.linker["500"] = "500.html";

    return linker;
}