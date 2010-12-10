This folder contains firmware of all boards with Freescale DSP56f807.
The firmware provides services to motor controller applications.
It should be noted that each board type needs a different library.



Folder libdsp56f807 is structured as follows:

 - api: --> files .h that contain api of provided services.
 
 - common --> files  .c and .h about management of periphericals in common with all boards. 
 
 - 2BLL:
     * library_2bll.mcp --> CodeWarrior progect file
     * source_code --> files .c that implement services for 2BLL board
     * output --> contains the output of compilation (.lib and assembly files)
		  The file.lib shoud be included in motor controller applications for 2BLL board
    
 - 2BLLDC:
     * library_2blldc.mcp --> CodeWarrior progect file
     * source_code --> files .c that implements services for 2BLLDC board
     * output --> contains the output of compilation (.lib and assembly files)
                  The file.lib shoud be included in motor controller applications for 2BLLDC board

 - 2DC:
     * library_2dc.mcp --> CodeWarrior progect file
     * source_code --> files .c that implements services for 2DC board
     * output --> contains the output of compilation (.lib and assembly files)
                  The file.lib shoud be included in motor controller applications for 2DC board

 - 4DC:
     * library_4dc.mcp --> CodeWarrior progect file
     * source_code --> files .c that implements services for 4DC board.
     * output --> contains the output of compilation (.lib and assembly files)
                  The file.lib shoud be included in motor controller applications for 4DC board

- scripts: contains files .bat that permit to use .lib file in motor controller applications   
           (see readme.txt in motorControllerDsp56f807)