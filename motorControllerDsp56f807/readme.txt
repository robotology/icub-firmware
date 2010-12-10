This folder contains motor controller applications.
Each motor controller application is made up by software indipendent of board ("common" folders) and software dependent on board (folders with name of board).


In order to build a motor controller application it is necessary:
1) build "library_<NAME_BOARD>.lib" by Code Warrior project in libdsp56f807 folder;(<NAME_BOARD> can be 2bll or 2blldc or 4dc)
2) install the "library_<NAME_BOARD>.lib" by script "install_library.bat"in libdsp56f807\script folder. 
   This script copies file .lib in the correct folder of motor controller application. 

In order to flash a motor controller application it is necessary:
1) copy files .out.S in the folder firmware\build or  firmware\build_testing by the script "copy_output_files.bat";
2) continue as usual.




This folder is structured as follows:

 - 2BLL:
    * controller_2bll.mpc --> CodeWarrior project file of motor controller application
    * workspace.cww --> opens application and library CodeWarrior project files in the same window
    * source_code --> contains files .c and .h that implement part of application dependent on board
    * lib --> will contain library_2bll.lib after its installation
    * output --> contains output file of compilation (assembly files and .S)
 
 - 2BLLDC:
    * controller_2blldc.mpc --> CodeWarrior project file of motor controller application
    * source_code --> contains files .c and .h that implement part of application dependent on board
    * lib --> will contain library_2blldc.lib after its installation
    * output --> contains output file of compilation (assembly files and .S)

 - 4DC:
    * controller_4dc.mpc --> CodeWarrior project file of motor controller application
    * workspace.cww --> opens application and library CodeWarrior project files in the same window
    * source_code --> contains files .c and .h that implement part of application dependent on board
    * lib --> will contain library_2bll.lib after its installation
    * output --> contains output file of compilation (assembly files and .S)

 - common_source_code --> contains files .c and .h that implement part of application independent of board

 - common_support --> contains files .c, .h and .asm that initialize microcontroller and supply mathematical opeartions.
 
 - linker --> contains linker command files and flash configuration file

 - script --> contains copy_output_files.bat