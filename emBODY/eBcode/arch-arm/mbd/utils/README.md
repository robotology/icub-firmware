# How to use the Copier script

To use the Copier script, run the following command:

```bash
 python copier.py -s <source1> <source2> ... <sourceN> -d <destination> -j <path_to_directories_json>
```

**Arguments:**

- _source_: It's the directory where the generated code by Simulink resides. This should point to the `codegen` and/or `system` directory of the Model repository.

- _destination_: It's the directory where to copy the code. This should point to the `src/model-based-design` directory of `icub-firmware` repo of the respective board.

- _path_to_directories_json_: It's the path to the `directories.json` file. This file should be placed, and properly configured, for each board that includes parts of `mbd` code.

⚠️ The script is intended to work on **Windows** machines only.

The argument `--help` will show the helper and quit.


## `directories.json`

The json file is verified before running the script. If something is wrong, you will most likely get an error showing the problem. 

The structure of the json goes as follows:


It's the directory where to paste the code. This should point to the `arch-arm\mbd` directory of the respective `icub-firmware` repo.

You can input the location by pieces in an array. The code will join the strings together.

#### Subdirectories

```json
"subdirectories_to_copy": [
    {"..."},
]
```

This holds the information of the subdirectories that will be copied from source to target. 

Each entry in the `subdirectories_to_copy` should be another dictionary following this structure:

```json
{
    "source_directory": "...",
    "source_directory_parent": "...",
    "target_directory": "...",            
    "files": [
        "...",
    ],
    "exclude": [
        "...",
    ]
}
```

`source_directory`: name of the subdirectory in the source. This subdirectory can be anywhere in the file structure of the source. The copier will search for it through the entire file hierarchy.

`source_directory_parent` [optional]: if there are several subdirectories with the same name, you can specify the parent to select which one to choose.

For example, if inside the source there is a directory called `src/files` and `lib/files`, using `files` as source and `src` in the parent field will select the former of the two.

`target_directory`: name of the directory in which to paste the files. This directory must exist already in the target. If you are copying into a new directory first create the empty target and then run the script.

`files`: selectors for the files. The script will only copy files whose names match the given selectors. Wildcard characters like "*" are allowed.

`exclude`: list of files that needs to be exclude from copy operation. Useful when using the wildcard character "*" in the `files` section.

For example, if you only want `.cpp` and `.h` files that begin with `helloworld` but not `helloworld_ex`, you could do the following:

```json
"files": [
    "helloworld*.cpp",
    "helloworld*.h"
],
"exclude": [
    "helloworld_ex*"
]
```

Any file that doesn't follow this naming will be excluded from being copied.


### Example of `directories.json`

```json
{
    "subdirectories_to_copy": [
        {
            "source_directory": "can_decoder",
            "source_directory_parent": "ert",
            "target_directory": "can-decoder",            
            "files": [
                "*.cpp",
                "*.h"
            ]
        },
        {
            "source_directory": "can_encoder",
            "source_directory_parent": "ert",
            "target_directory": "can-encoder",            
            "files": [
                "*.cpp",
                "*.h"
            ]
        }
    ]
}
```
