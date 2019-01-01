#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import re
import shutil


basedir = "../jury.d/"
sourcedir = "./src/resources/files/"
tag_begin_list = '#[begin-list-of-resources]'
tag_end_list = '#[end-list-of-resources]'

shutil.rmtree('./src/resources/files')
os.mkdir('./src/resources/files')

for_cmake = []
for_cmake.append(tag_begin_list)
for_cmake.append("")

files = []
files_html = [os.path.join(dp, f) for dp, dn, filenames in os.walk(basedir + "html/") for f in filenames]

files = files + files_html
files.append(basedir + "game.conf")
files.append(basedir + "mysql_storage.conf")
files.append(basedir + "scoreboard.conf")
files.append(basedir + "server.conf")

for filename in files:
    statinfo = os.stat(filename)
    size = statinfo.st_size

    filename_ = filename[len(basedir):]
    newfilename = re.sub(r"[^a-zA-Z0-9_]", "_", filename_)

    if newfilename in files:
        print("WARNING: " + newfilename + " - already defined")
        continue

    print("Prepare " + filename + " to " + sourcedir + newfilename + ".*")
    _header = open(sourcedir + newfilename + ".h", "w")
    _source = open(sourcedir + newfilename + ".cpp", "w")
    
    for_cmake.append("list (APPEND JURY_AD_SOURCES \"" + sourcedir + newfilename + ".h\")")
    for_cmake.append("list (APPEND JURY_AD_SOURCES \"" + sourcedir + newfilename + ".cpp\")")

    _classname = "RES_" + newfilename

    _header.write("// automaticly generated by update_resources.py\n\n")
    _header.write("\n")
    _header.write("#include <resources_manager.h>\n")
    _header.write("\n")
    _header.write("class " + _classname + " : public ResourceFile { \n")
    _header.write("    public:\n")
    _header.write("        virtual const std::string &filename();\n")
    _header.write("        virtual const int bufferSize();\n")
    _header.write("        virtual const char *buffer();\n")
    _header.write("};\n")
    _header.write("\n")

    _source.write("// automaticly generated by update_resources.py\n")
    _source.write("\n")
    _source.write("#include <" + newfilename + ".h>\n")
    _source.write("\n")
    _source.write("REGISTRY_RESOURCE_FILE(" + _classname + ")\n")
    _source.write("\n")
    _source.write("const std::string &" + _classname + "::filename() {\n")
    _source.write("    static const std::string s = \"" + filename_ + "\";\n")
    _source.write("    return s;\n")
    _source.write("}\n")
    _source.write("\n")
    _source.write("const int " + _classname + "::bufferSize() {\n")
    _source.write("    return " + str(size) + ";\n")
    _source.write("}\n")
    _source.write("\n")
    _source.write("const char *" + _classname + "::buffer() {\n")
    _source.write("    static const unsigned char b[" + str(size) + "] = {")
    i = 0
    with open(filename, "rb") as f:
        byte = f.read(1)
        while byte != b'':
            # Do stuff with byte.
            if i != 0:
                _source.write(", ")
            if i % 16 == 0:
                _source.write("\n        ")
            _source.write("0x" + byte.encode('hex'))
            i = i + 1
            byte = f.read(1)
        _source.write("\n    };\n")
    if i != size:
        print("Expected " + str(size) + " got " + str(i))
        
    _source.write("    return (const char*)b;\n")
    _source.write("} //::buffer() \n")
    _source.write("\n")
    _header.close()
    _source.close()

for_cmake.append("")
for_cmake.append(tag_end_list)


print ("Updating list of resources in CMakeLists.txt ...")
with open('./CMakeLists.txt') as f:
    cmakelists = f.readlines()

cmakelists = "".join(cmakelists)
start_i = cmakelists.find(tag_begin_list)
end_i = cmakelists.find(tag_end_list) + len(tag_end_list)
_prev = cmakelists[:start_i]
_next = cmakelists[end_i:]
cmakelists = _prev + "\n".join(for_cmake) + _next

with open('./CMakeLists.txt', 'w') as f:
    f.write(cmakelists)
print ("Updated.")