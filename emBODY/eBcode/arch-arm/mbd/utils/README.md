## How to use the Copier script

To use the Copier script, run the following command:

```bash
 Python3 copier.py <path-to-the-directories.json-file>.
```

**Notes:**

- The program expects a file called [`directories.json`][1] that should be placed, and properly configured, for each board that includes parts of `mbd` code.
- ⚠️ The program is intended to work on **Windows** machines only. 

## `directories.json`

The json file is verified before running the script. If something is wrong, you will most likely get an error showing the problem. 

The structure of the json goes as follows:

#### Source 

```json
"source_directory": [
    "...",
]
```

It's the directory where the generated code resides. This should point to the `codegen` directory of the Model repository.

You can input the location by pieces in an array. The code will join the strings together.

#### Target

```json
"target_directory": [
    "...",
]
```

It's the directory where to paste the code. This should point to the `src/model-based-design` directory of the respective `icub-firmware` repo.

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
    ]
}
```

`source_directory`: name of the subdirectory in the source. This subdirectory can be anywhere in the file structure of the source. The copier will search for it through the entire file hierarchy.

`source_directory_parent` [optional]: if there are several subdirectories with the same name, you can specify the parent to select which one to choose.

For example, if inside the source there is a directory called `src/files` and `lib/files`, using `files` as source and `src` in the parent field will select the former of the two.

`target_directory`: name of the directory in which to paste the files. This directory must exist already in the target. If you are copying into a new directory first create the empty target and then run the script.

`files`: selectors for the files. The script will only copy files whose names match the given selectors. 

For example, if you only want `.cpp` and `.h` files that begin with `helloworld` you could do the following:

```json
"files": [
    "helloworld*.cpp",
    "helloworld*.h"
]
```

Any file that doesn't follow this naming will be excluded from being copied.


### Example of directories.json

```json
{
    "source_directory": [
        "C:",
        "\\dev",
        "icub-firmware-models\\boards\\amcbldc",
        "codegen"
    ],
    "target_directory": [
        "C:",
        "\\dev",
        "icub-firmware\\emBODY\\eBcode\\arch-arm\\board\\amcbldc\\application\\src",
        "model-based-design"
    ],
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

[1]: ../../board/amcbldc/utils/directories.json
