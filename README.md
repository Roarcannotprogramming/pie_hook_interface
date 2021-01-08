# PIEHook Interface

## Introduce

This is the interface of PIEHook written by @[kongjiadongyuan](https://github.com/kongjiadongyuan)(not opensource by now).

## Install

**Directory Tree**

```shell
$ tree -d
.
├── cJSON
│   ├── fuzzing
│   │   └── inputs
│   ├── library_config
│   └── tests
│       ├── inputs
│       ├── json-patch-tests
│       └── unity
│           ├── auto
│           ├── docs
│           ├── examples
│           │   ├── example_1
│           │   │   ├── src
│           │   │   └── test
│           │   │       └── test_runners
│           │   ├── example_2
│           │   │   ├── src
│           │   │   └── test
│           │   │       └── test_runners
│           │   └── example_3
│           │       ├── helper
│           │       ├── src
│           │       └── test
│           ├── extras
│           │   ├── eclipse
│           │   └── fixture
│           │       ├── src
│           │       └── test
│           │           └── main
│           ├── release
│           ├── src
│           └── test
│               ├── expectdata
│               ├── spec
│               ├── targets
│               ├── testdata
│               └── tests
├── piehook
└── pie_hook_interface
```

Step 1. Clone `cJSON` project from https://github.com/DaveGamble/cJSON. And get piehook kernel module from @[kongjiadongyuan](https://github.com/kongjiadongyuan)

Step 2. Run `make` in the three directory `cJSON, piehook, pie_hook_interface`.

Step 3. Run `sudo insmod ./piehook/piehook.mod`.

Step 4. Run `sudo ./pie_interface -t 0x555555554000 -p 0x4000 -s 0x7ffffffad000 -g 0x500` for test. (Run it as **root**)

## Examples

```
Usage: ./pie_interface [-h|--help] [-t|--text TEXT_SEG_BASE] [-s|--stack STACK_BASE] [-p|--heap HEAP_OFFSET] [-g|--stackmagic]
               [-e|--encodejson JSONFILE] [-d|--decodejson JSONFILE]
    -h, --help	Show this help message
    -t, --text TEXT_SEG_BASE	Enable text base PIE hook (TEXT_SEG_BASE means 0x555555554000 + OFFSET)
    -s, --stack STACK_BASE	Enable stack base hook (STACK_BASE means the top of user stack area)
    -p, --heap HEAP_OFFSET	Enable heap base hook (HEAP_OFFSET means OFFSET from the end of text area)
    -g, --stackmagic STACK_OFFSET	Enable stack offset hook(the offset need to be fine-tuning manually)
    -e, --encodejson JSONFILE	export the setting message to JSONFILE in format of json
    -d, --decodejson JSONFILE	import the setting message from JSONFILE in format of json
```

**basic**

`sudo ./pie_interface -t 0x555555554000 -p 0x4000 -s 0x7ffffffad000 -g 0x500`

**Save in format of Json**

`sudo ./pie_interface -t 0x555555554030 -p 0x4000 -s 0x7ffffffad000 -g 0x500 -e abc.json`

**Load from Json**

`sudo ./pie_interface -d abc.json`

