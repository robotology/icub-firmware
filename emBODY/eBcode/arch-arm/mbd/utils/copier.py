"""
 Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 Author:  Simone Girardi
 email:   simone.girardi@iit.it
"""

import os.path
import os
import glob
import shutil
import json
import argparse

SECTION="-------------------"
BIG_SECTION="*******************"

# Checking if the directory is empty or not
def is_empty(dir):
    try:
        if not os.listdir(dir):
            print(dir + " is an empty directory.")
            print("Aborting...")
            return True
        else:
            return False
    except FileNotFoundError:
        print(dir + " directory does not exist.")
        print("Aborting...")
        return True

def find_subdirectory(main_directory, subdirectory, parent = None):
    for root, dirs, files in os.walk(main_directory):
        for dir in dirs:
            if dir == subdirectory:
                dir_parent = root.split('\\')[-1]
                if parent is None or dir_parent == parent:
                    return os.path.join(root, subdirectory)
    return ""

def check_dictionary_type(dictionary, key, type):
    if not key in dictionary:
        raise Exception(f"No {key} found")
    if not isinstance(dictionary[key], type):
        raise Exception(f"Invalid {key}")

def parse_instructions(dictionary):
    check_dictionary_type(dictionary, "source_directory", list)
    check_dictionary_type(dictionary, "target_directory", list)
    check_dictionary_type(dictionary, "subdirectories_to_copy", list)

    # check if placeholders have been replace in json
    if dictionary["source_directory"][1].startswith("<") or dictionary["target_directory"][1].startswith("<"):
        print("The file dictionaries.json contains placeholders surrounded with \"<...>\". Replace them before continue.")
        print("Aborting...")
        return -1

    source_directory = os.path.join(*dictionary["source_directory"])
    target_directory = os.path.join(*dictionary["target_directory"])
    subdirectories_to_copy = dictionary["subdirectories_to_copy"]

    if is_empty(source_directory):
        return -1

    if is_empty(target_directory):
        return -1

    for subdirectory in subdirectories_to_copy:
        parent = subdirectory["source_directory_parent"] if "source_directory_parent" in subdirectory else None
        source_subdir = find_subdirectory(source_directory, subdirectory["source_directory"], parent)
        target_subdir = find_subdirectory(target_directory, subdirectory["target_directory"])

        copy_files(source_subdir, target_subdir, subdirectory["files"], True)
    print(BIG_SECTION)

def copy_files(source_dir, target_dir, file_selectors, overwrite):
    if not (os.path.isdir(source_dir) and os.path.isdir(target_dir)):
        raise Exception(f"""Invalid path given (Source: {source_dir}, Target: {target_dir}).
        If target is empty, consider creating the empty directory in the desired destination before running the copier.""")
    print(BIG_SECTION)
    print(f"Copying files from {source_dir} to {target_dir}")
    print(SECTION)
    files_to_copy = []
    for file_selector in file_selectors:
        selected_files = glob.glob(os.path.join(source_dir, file_selector))
        print(f"Selector {file_selector} matched with the following files: {selected_files}")
        files_to_copy = files_to_copy + selected_files
    files_to_copy = set(files_to_copy)
    print(SECTION)
    for source_file in files_to_copy:
        file_basename = os.path.basename(source_file)
        target_file = os.path.join(target_dir, file_basename)
        if overwrite or (not os.path.exists(target_file)):
            shutil.copy(source_file, target_file)
            print(f"Copying  {file_basename} from {source_dir} to {target_dir}")
        else:
            print(f"Skipping {file_basename} from {source_dir} since overwriting is not allowed")
        print("\n")

def main():
    parser = argparse.ArgumentParser(prog='The Copier',
                                    description="The copier has the purpose to copy the code generated from Simulink to the proper board folder in icub-firmware ",
                                    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('filepath', help="Absolute path to the directories.json file")

    args = parser.parse_args()
    config = vars(args)

    with open(config['filepath']) as file:
        json_instructions = json.load(file)

    parse_instructions(json_instructions)
    
if __name__ == "__main__":
    main()