add_executable(example main.c atable.c)
configure_file(v8.conf v8.conf COPYONLY)
configure_file(hello.html hello.html COPYONLY)
configure_file(table.html table.html COPYONLY)

target_link_libraries(example v8)
