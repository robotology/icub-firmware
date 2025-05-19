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

    if not os.path.isdir(main_directory):
        raise ValueError(f"Main directory '{main_directory}' does not exist")
        return ""
            
    for root, dirs, _ in os.walk(main_directory):
        if subdirectory in dirs:
            dir_parent = os.path.basename(root)
            
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

    for subdirectory in subdirectories_to_copy:
        parent = subdirectory["source_directory_parent"] if "source_directory_parent" in subdirectory else None
        source_subdir = find_subdirectory(source_directory, subdirectory["source_directory"], parent)
        target_subdir = find_subdirectory(target_directory, subdirectory["target_directory"])

        if(source_subdir == "" or target_subdir == ""):
            continue

        if "exclude" in subdirectory:
            copy_files(source_subdir, target_subdir, subdirectory["files"], subdirectory["exclude"])
        else:
            copy_files(source_subdir, target_subdir, subdirectory["files"])

def copy_files(source_dir, target_dir, file_selectors, exclude_list=None):
    if not (os.path.isdir(source_dir) and os.path.isdir(target_dir)):
        raise ValueError(f"Invalid path given (Source: {source_dir}, Target: {target_dir})")
    
    print(f"{source_dir} ➡️ {target_dir}")
    print(SECTION)
    
    files_to_copy = set()
    exclude_list = set(exclude_list or [])
    
    for selector in file_selectors:
        selected_files = glob.glob(os.path.join(source_dir, selector))
        files_to_copy.update(file for file in selected_files if file not in exclude_list)
    
    for source_file in files_to_copy:
        file_basename = os.path.basename(source_file)
        target_file = os.path.join(target_dir, file_basename)
        shutil.copy(source_file, target_file)

def main():
    parser = argparse.ArgumentParser(prog='The Copier',
                                    description="The copier has the purpose to copy the code generated from Simulink to the proper board folder in icub-firmware ",
                                    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    # parser.add_argument('args', nargs='*')
    parser.add_argument('-s', '--source', help="Absolute paths to the codegen directory within icub-firmware-models")
    parser.add_argument('-d', '--destination', help="Absolute path to the model-based-design directory within icub-firmware (for a specific board)")
    parser.add_argument('-j', '--json_path', help="Absolute path to the directory containing the directories.json file")

    args = parser.parse_args()
    config = vars(args)

    path_to_json = os.path.join(config['json_path'], "directories.json")
    path_to_src = config['source']
    path_to_dst = config['destination']

    # check if source and destination directories exist and are not empty
    if not (is_empty(path_to_src) or is_empty(path_to_dst)):

        # try to open the json configuration file
        with open(path_to_json) as file:
            json_instructions = json.load(file)

        # start to parse and (eventually) to copy
        parse_instructions(json_instructions, path_to_src, path_to_dst)
    
    print("Done ✅")

if __name__ == "__main__":
    main()