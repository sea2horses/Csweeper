compiler := clang
flags := -Wall # -Werror

main_file := src/main.c

# All of the source files that need to be linked
utilities := src/utils/consoleutils.c src/utils/input.c
classes := src/classes/templates.c src/classes/minefield.c src/classes/vec.c
app_modules := src/app/game.c src/app/menus.c src/app/titles.c

source_files := $(utilities) $(classes) $(app_modules)

exec_name := main
build_folder := build

echo:
	@echo "To build the executable, run: 'make build'."
	@echo "To run the program, run: 'make run'. (Must be done AFTER building)"
	@echo ""
	@echo "Windows is not necessarily supported... for now"

build: $(main_file) $(source_files)
	@mkdir -p $(build_folder)
	$(compiler) $(flags) $(main_file) $(source_files) -o $(build_folder)/$(exec_name)

test1 := src/input_test.c
test1_libs := src/utils/input.c

test1_execname := input_test

test1: $(test1) $(test1_libs)
	$(compiler) $(flags) $(test1) $(test1_libs) -o $(build_folder)/$(test1_execname)
	./$(build_folder)/$(test1_execname)

run: $(build_folder)/$(exec_name)
	./$(build_folder)/$(exec_name)