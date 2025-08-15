compiler := gcc
flags := -Wall -Werror

main_file := src/main.c

# All of the source files that need to be linked
utilities := src/utils/consoleutils.c src/utils/input.c
classes := src/classes/templates.c src/classes/minefield.c src/classes/vec.c
app_modules := src/app/game.c src/app/menus.c src/app/titles.c

source_files := $(utilities) $(classes) $(app_modules)

exec_name_unix := main
exec_name_windows := main.exe

build_folder := build

# Detect OS
ifeq ($(OS),Windows_NT)
    exec_name := $(exec_name_windows)
		mkdir_cmd := if not exist $(build_folder) mkdir $(build_folder)
    build_cmd := $(compiler) $(flags) $(main_file) $(source_files) -o $(build_folder)/$(exec_name)
    run_cmd := $(build_folder)/$(exec_name)
else
    exec_name := $(exec_name_unix)
		mkdir_cmd := mkdir -p $(build_folder)
    build_cmd := $(compiler) $(flags) $(main_file) $(source_files) -o $(build_folder)/$(exec_name)
    run_cmd := ./$(build_folder)/$(exec_name)
endif

.PHONY: echo build run clean

echo:
	@echo To build the executable, run: 'make build'.
	@echo To run the program, run: 'make run'. (Must be done AFTER building)
	@echo .
	@echo Windows should now be supported

build: $(main_file) $(source_files)
	@$(mkdir_cmd)
	@echo $(build_cmd) > build_cmd.txt
	$(build_cmd)

run: build
	@echo $(run_cmd) > run_cmd.txt
	$(run_cmd)

clean:
	rm -rf $(build_folder)