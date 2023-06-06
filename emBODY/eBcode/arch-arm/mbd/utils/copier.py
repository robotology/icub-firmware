"""
 Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 Author:  Simone Girardi
 email:   simone.girardi@iit.it
"""

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
                dir_parent = root.split(os.sep)[-1]
                if parent is None or dir_parent == parent:
                    return os.path.join(root, subdirectory)
    return ""

def check_dictionary_type(dictionary, key, type):
    if not key in dictionary:
        raise Exception(f"No {key} found")
    if not isinstance(dictionary[key], type):
        raise Exception(f"Invalid {key}")

def parse_instructions(dictionary, source_directory, target_directory):
    check_dictionary_type(dictionary, "subdirectories_to_copy", list)
    subdirectories_to_copy = dictionary["subdirectories_to_copy"]

    # loop over the subdirectories
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
    parser.add_argument('source', nargs='+', help="Absolute paths to the codegen directories within icub-firmware-models")
    parser.add_argument('destination', help="Absolute path to the model-based-design directory within icub-firmware (for a specific board)")
    parser.add_argument('path_to_directories_json', help="Absolute path to the directory containing the directories.json file")

    args = parser.parse_args()
    config = vars(args)

    path_to_json = os.path.join(config['path_to_directories_json'], "directories.json")
    path_to_src_list = config['source']
    path_to_dst = config['destination']

    for path_to_src in path_to_src_list:
    # check if source and destination directories exist and are not empty
        if not (is_empty(path_to_src) or is_empty(path_to_dst)):

            # try to open the json configuration file
            with open(path_to_json) as file:
                json_instructions = json.load(file)

            # start to parse and (eventually) to copy
            parse_instructions(json_instructions, path_to_src, path_to_dst)
    
if __name__ == "__main__":
    main()