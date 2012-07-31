::
:: Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
:: Author:  Marco Accame
:: email:   marco.accame@iit.it
:: website: www.robotcub.org
:: Permission is granted to copy, distribute, and/or modify this program
:: under the terms of the GNU General Public License, version 2 or any
:: later version published by the Free Software Foundation.
:: 
:: A copy of the license can be found at
:: http://www.robotcub.org/icub/license/gpl.txt
:: 
:: This program is distributed in the hope that it will be useful, but
:: WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
:: Public License for more details
::

attrib -R ..\..\..\..\sys\mid\api\*.h
copy /Y oosiit-api\*.h ..\..\..\..\sys\mid\api
attrib +R ..\..\..\..\sys\mid\api\*.h

attrib -R ..\..\..\..\sys\mid\inc\*.*
copy /Y oosiit-stg\*.* ..\..\..\..\sys\mid\inc
attrib +R ..\..\..\..\sys\mid\inc\*.*

attrib -R ..\..\..\..\sys\mid\lib\*.lib
copy /Y oosiit-lib\oosiit.cm3.lib ..\..\..\..\sys\mid\lib
copy /Y oosiit-lib\oosiit.cm3.dbg.lib ..\..\..\..\sys\mid\lib
copy /Y oosiit-lib\oosiit.cm4.lib ..\..\..\..\sys\mid\lib
copy /Y oosiit-lib\oosiit.cm4.dbg.lib ..\..\..\..\sys\mid\lib
attrib +R ..\..\..\..\sys\mid\lib\*.lib

attrib -R ..\..\..\..\sys\mid\cfg\*.*
copy /Y oosiit-cfg\*.* ..\..\..\..\sys\mid\cfg
attrib +R ..\..\..\..\sys\mid\cfg\*.*






