compiler := 'gcc'
flags := '-Wall -Werror'

main_file := 'src/main.c'

# All of the source files that need to be linked
utilities := 'src/utils/consoleutils.c src/utils/input.c'
classes := 'src/classes/templates.c src/classes/minefield.c src/classes/vec.c'
app_modules := 'src/app/game.c src/app/menus.c src/app/titles.c'

source_files := f'{{utilities}} {{classes}} {{app_modules}}'

exec_name_unix := 'main'
exec_name_windows := 'main.exe'

build_folder := 'build'

exec_name := if os_family() == "windows" { exec_name_windows } else { exec_name_unix }
mkdir_cmd := if os_family() == "windows" {
	f'if not exists {{build_folder}} mkdir {{build_folder}}'
} else {
	f'mkdir -p {{build_folder}}'
}
build_cmd := f'{{compiler}} {{flags}} {{main_file}} {{source_files}} -o {{build_folder / exec_name}}'
run_cmd := if os_family() == "windows" { build_folder / exec_name } else { './' + build_folder / exec_name }

echo:
	@echo "To build the executable, run: 'just build'."
	@echo "To run the program, run: 'just run'. (Must be done AFTER building)"
	@echo ""
	@echo "Windows should now be supported"

build:
	{{mkdir_cmd}}
	{{build_cmd}}

run: build
	{{run_cmd}}

clean:
	rm -rf {{build_folder}}
