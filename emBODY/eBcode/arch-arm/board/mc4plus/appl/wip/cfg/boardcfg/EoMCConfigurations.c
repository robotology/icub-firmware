/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

/* @file        EoMCConfigurations.c
    @brief      This file keeps ...
    @author     davide.pollarolo@iit.it
    @date       29/04/2015
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoMCConfigurations.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// it contains a pair {value, configuration}
typedef struct // 2B+12*8B = 98B
{
    uint16_t                value;
    eOmcconfig_jomo_cfg_t   jomos[eomcconfig_jomo_maxnumberof];
} eomcconfig_codeconfig_t;  EO_VERIFYsizeof(eomcconfig_codeconfig_t, 98);

// it contains a pair {string, value}
typedef struct // 4B+2B = 6B
{
    const char*             string;
    uint16_t                value;
} eomcconfig_valuestring_t;  EO_VERIFYsizeof(eomcconfig_valuestring_t, 8);

// it contains a pair {board number, joints number}
typedef struct
{
    uint8_t                 board_num;
    uint8_t                 joints;
} eomcconfig_boardjomos_t;  EO_VERIFYsizeof(eomcconfig_boardjomos_t, 2);

// it contains a pair {board number, value}
typedef struct
{
    uint8_t                 board_num;
    uint16_t                value;
} eomcconfig_boardcode_t;  EO_VERIFYsizeof(eomcconfig_boardcode_t, 4);


// internal memory struct
typedef struct // 2B+1B = 3B
{
    eOmcconfig_code_t   active_code;
    eObool_t            initted;
} eomcconfig_internalmemory_t;  EO_VERIFYsizeof(eomcconfig_internalmemory_t, 4);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static eomcconfig_internalmemory_t s_eo_mcconf = 
{
    .active_code            = EOMCCONFIG_CODE_DUMMY,
    .initted                = eobool_false
};

static const uint16_t s_eomcconfig_maxvalue_in_type[] =
{
    eOmcconfig_value_2FOC_numberof,
    eOmcconfig_value_MC4CAN_numberof,
    eOmcconfig_value_MC4PLUS_numberof
};  EO_VERIFYsizeof(s_eomcconfig_maxvalue_in_type, eOmcconfig_type_numberof*sizeof(uint16_t)); //check correctness

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eOmcconfig_code_t eOmcconfig_code_dummy = EOMCCONFIG_CODE_DUMMY;
const eOmcconfig_value_t eOmcconfig_value_dummy = EOMCCONFIG_VALUE_DUMMY;
const eOmcconfig_code_t eOmcconfig_board_dummy = EOMCCONFIG_BOARD_DUMMY;

const eOmcconfig_jomo_cfg_t dummy_jomo = {0};
// - begin of: section for configuration 
// - fill the configurations in here for all the known boards 

//use EOINIT here to initialize everything
const eomcconfig_codeconfig_t eomcconfig_codeconfig_2FOC[] =
{
    {EO_INIT(.value) eOmcconfig_value_2FOC_unspecified}
}; EO_VERIFYsizeof(eomcconfig_codeconfig_2FOC, eOmcconfig_value_2FOC_numberof*sizeof(const eomcconfig_codeconfig_t)); 

const eomcconfig_codeconfig_t eomcconfig_codeconfig_MC4CAN[] =
{
    {EO_INIT(.value) eOmcconfig_value_MC4CAN_unspecified}
}; EO_VERIFYsizeof(eomcconfig_codeconfig_MC4CAN, eOmcconfig_value_MC4CAN_numberof*sizeof(const eomcconfig_codeconfig_t)); 

const eomcconfig_codeconfig_t eomcconfig_codeconfig_MC4PLUS[] =
{
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_unspecified},
    /* experimental configurations */
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_experimental,
        EO_INIT(.jomos[0].actuator.local.type)  1, EO_INIT(.jomos[0].actuator.local.index) 2, EO_INIT(.jomos[0].encoder.isthere)       1 ,EO_INIT(.jomos[0].encoder.etype)        2, EO_INIT(.jomos[0].encoder.index)        3, EO_INIT(.jomos[0].encoder.pos_type)       0,
                                                                                              EO_INIT(.jomos[0].extra_encoder.isthere) 1, EO_INIT(.jomos[0].extra_encoder.etype)  1, EO_INIT(.jomos[0].extra_encoder.index)  0, EO_INIT(.jomos[0].extra_encoder.pos_type) 1,
    },
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_experimental_2joints,
        EO_INIT(.jomos[0].actuator.local.type)  1, EO_INIT(.jomos[0].actuator.local.index) 2, EO_INIT(.jomos[0].encoder.isthere) 1, EO_INIT(.jomos[0].encoder.etype)  1, EO_INIT(.jomos[0].encoder.index)  0, EO_INIT(.jomos[0].encoder.pos_type)       0,
        EO_INIT(.jomos[1].actuator.local.type)  1, EO_INIT(.jomos[1].actuator.local.index) 3, EO_INIT(.jomos[1].encoder.isthere) 1, EO_INIT(.jomos[1].encoder.etype)  2, EO_INIT(.jomos[1].encoder.index)  3, EO_INIT(.jomos[1].encoder.pos_type)       0,
    },
    
    /* v3 icub head configs */
    //board 12 - b0
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_b0,
        // mapping between actuators and encoders for this board is unspecified in the PDF containing HeadV3 schemes...ask maggia
        EO_INIT(.jomos[0].actuator.local.type)  1, EO_INIT(.jomos[0].actuator.local.index) 0, EO_INIT(.jomos[0].encoder.etype)  0, EO_INIT(.jomos[0].encoder.index)  1,
        EO_INIT(.jomos[1].actuator.local.type)  1, EO_INIT(.jomos[1].actuator.local.index) 2, EO_INIT(.jomos[1].encoder.etype)  0, EO_INIT(.jomos[1].encoder.index)  0
    },
    
    //board 13 - b1
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_b1, 
        EO_INIT(.jomos[0].actuator.local.type)  1, EO_INIT(.jomos[0].actuator.local.index) 0, EO_INIT(.jomos[0].encoder.etype)  0, EO_INIT(.jomos[0].encoder.index)  1,
        EO_INIT(.jomos[1].actuator.local.type)  1, EO_INIT(.jomos[1].actuator.local.index) 1, EO_INIT(.jomos[1].encoder.etype)  0, EO_INIT(.jomos[1].encoder.index)  0,
        EO_INIT(.jomos[2].actuator.local.type)  1, EO_INIT(.jomos[2].actuator.local.index) 3, EO_INIT(.jomos[2].encoder.etype)  2, EO_INIT(.jomos[2].encoder.index)  3,
        EO_INIT(.jomos[3].actuator.local.type)  1, EO_INIT(.jomos[3].actuator.local.index) 2, EO_INIT(.jomos[3].encoder.etype)  2, EO_INIT(.jomos[3].encoder.index)  2
    },
    
    //board 14 - b7
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_b7, 
        EO_INIT(.jomos[0].actuator.local.type)  1, EO_INIT(.jomos[0].actuator.local.index) 0, EO_INIT(.jomos[0].encoder.etype)  0, EO_INIT(.jomos[0].encoder.index)  1, // j0-> jaw, motor0, aea-encoder
        EO_INIT(.jomos[1].actuator.local.type)  1, EO_INIT(.jomos[1].actuator.local.index) 1, EO_INIT(.jomos[1].encoder.etype)  0, EO_INIT(.jomos[1].encoder.index)  0, // j1-> eyelids, motor1, aea-encoder
    },
    
    //board 15 - b9
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_b9,
//        // straight configuration (coherent with actual XML files)
//        EO_INIT(.jomos[0].actuator.local.type)  1, EO_INIT(.jomos[0].actuator.local.index) 0, EO_INIT(.jomos[0].encoder.etype)  2, EO_INIT(.jomos[0].encoder.index)  0,
//        EO_INIT(.jomos[1].actuator.local.type)  1, EO_INIT(.jomos[1].actuator.local.index) 1, EO_INIT(.jomos[1].encoder.etype)  2, EO_INIT(.jomos[1].encoder.index)  1,
//        EO_INIT(.jomos[2].actuator.local.type)  1, EO_INIT(.jomos[2].actuator.local.index) 2, EO_INIT(.jomos[2].encoder.etype)  2, EO_INIT(.jomos[2].encoder.index)  2,
//        EO_INIT(.jomos[3].actuator.local.type)  1, EO_INIT(.jomos[3].actuator.local.index) 3, EO_INIT(.jomos[3].encoder.etype)  2, EO_INIT(.jomos[3].encoder.index)  3
        
//        // ordered config
//        EO_INIT(.jomos[0].actuator.local.type)  1, EO_INIT(.jomos[0].actuator.local.index) 1, EO_INIT(.jomos[0].encoder.etype)  2, EO_INIT(.jomos[0].encoder.index)  1, // j0-> lip-high, motor1, incremental-encoder
//        EO_INIT(.jomos[1].actuator.local.type)  1, EO_INIT(.jomos[1].actuator.local.index) 3, EO_INIT(.jomos[1].encoder.etype)  2, EO_INIT(.jomos[1].encoder.index)  3, // j1-> lip-bottom, motor3, incremental-encoder
//        EO_INIT(.jomos[2].actuator.local.type)  1, EO_INIT(.jomos[2].actuator.local.index) 0, EO_INIT(.jomos[2].encoder.etype)  2, EO_INIT(.jomos[2].encoder.index)  0, // j2-> lip-right, motor0, incremental-encoder
//        EO_INIT(.jomos[3].actuator.local.type)  1, EO_INIT(.jomos[3].actuator.local.index) 2, EO_INIT(.jomos[3].encoder.etype)  2, EO_INIT(.jomos[3].encoder.index)  2  // j3-> lip-left, motor2, incremental-encoder


        // order is coherent with .xml file from grenoble: lip-left is j0 (not j3), lip-high is j1 (not j0), lip-right is j2 (j2 as before), lip-bottom is j3 (not j1)
        EO_INIT(.jomos[0].actuator.local.type)  1, EO_INIT(.jomos[0].actuator.local.index) 2, EO_INIT(.jomos[0].encoder.etype)  2, EO_INIT(.jomos[0].encoder.index)  2, // j0-> lip-left, motor2, incremental-encoder
        EO_INIT(.jomos[1].actuator.local.type)  1, EO_INIT(.jomos[1].actuator.local.index) 1, EO_INIT(.jomos[1].encoder.etype)  2, EO_INIT(.jomos[1].encoder.index)  1, // j1-> lip-high, motor1, incremental-encoder
        EO_INIT(.jomos[2].actuator.local.type)  1, EO_INIT(.jomos[2].actuator.local.index) 0, EO_INIT(.jomos[2].encoder.etype)  2, EO_INIT(.jomos[2].encoder.index)  0, // j2-> lip-right, motor0, incremental-encoder
        EO_INIT(.jomos[3].actuator.local.type)  1, EO_INIT(.jomos[3].actuator.local.index) 3, EO_INIT(.jomos[3].encoder.etype)  2, EO_INIT(.jomos[3].encoder.index)  3  // j3-> lip-bottom, motor3, incremental-encoder
    },
    
    /* v3 icub left-forearm boards */
    //board 16 - 1b4
    //right order of the joint (for high-level) must be checked
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_1b4, 
        EO_INIT(.jomos[0].actuator.local.type)  1, EO_INIT(.jomos[0].actuator.local.index) 0, EO_INIT(.jomos[0].encoder.etype)  4, EO_INIT(.jomos[0].encoder.index)  0, // j0-> index-distal,       motor0, mais, 0 stands for 1st mais entity
        EO_INIT(.jomos[1].actuator.local.type)  1, EO_INIT(.jomos[1].actuator.local.index) 1, EO_INIT(.jomos[1].encoder.etype)  4, EO_INIT(.jomos[1].encoder.index)  0, // j1-> middle-proximal,    motor1, mais
        EO_INIT(.jomos[2].actuator.local.type)  1, EO_INIT(.jomos[2].actuator.local.index) 2, EO_INIT(.jomos[2].encoder.etype)  4, EO_INIT(.jomos[2].encoder.index)  0, // j2-> middle-distal,      motor2, mais
        EO_INIT(.jomos[3].actuator.local.type)  1, EO_INIT(.jomos[3].actuator.local.index) 3, EO_INIT(.jomos[3].encoder.etype)  4, EO_INIT(.jomos[3].encoder.index)  0  // j3-> little-fingers,     motor3, mais
    },
    
}; EO_VERIFYsizeof(eomcconfig_codeconfig_MC4PLUS, eOmcconfig_value_MC4PLUS_numberof*sizeof(const eomcconfig_codeconfig_t));

const eomcconfig_valuestring_t eomcconfig_valuestring_MC4PLUS[] =
{
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_unspecified,              EO_INIT(.string) "Unspecified configuration"},
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_experimental,             EO_INIT(.string) "MC4Plus_experimental_jig"},
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_experimental_2joints,     EO_INIT(.string) "MC4Plus_experimental_jig_2joints"},
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_b0,                       EO_INIT(.string) "MC4plus_b0"},
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_b1,                       EO_INIT(.string) "MC4plus_b1"},
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_b7,                       EO_INIT(.string) "MC4plus_b7"},
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_b9,                       EO_INIT(.string) "MC4plus_b9"},
    {EO_INIT(.value) eOmcconfig_value_MC4PLUS_1b4,                      EO_INIT(.string) "MC4plus_1b4"}
};



//MC config for boards
enum { eOmcconfig_boardjomosnumberof = 9 };
const eomcconfig_boardjomos_t eomcconfig_boardjomos_MC4PLUS[] =
{
    /* experimentals */
    {EO_INIT(.board_num) 97,  EO_INIT (.joints) 2}, // experimental board
    {EO_INIT(.board_num) 98,  EO_INIT (.joints) 1}, // experimental board
    
    /* v3 icub head boards */
    {EO_INIT(.board_num) 11,  EO_INIT (.joints) 2}, //board 12, 0b0
    {EO_INIT(.board_num) 12,  EO_INIT (.joints) 4}, //board 13, 0b1
    {EO_INIT(.board_num) 13,  EO_INIT (.joints) 2}, //board 14, 0b7
    {EO_INIT(.board_num) 14,  EO_INIT (.joints) 4}, //board 15, 0b9
    
    /* v3 icub left-forearm boards */
    {EO_INIT(.board_num) 15,  EO_INIT (.joints) 4}, //board 16, 1b4
    {EO_INIT(.board_num) 16,  EO_INIT (.joints) 4}, //board 17, 1b3
    {EO_INIT(.board_num) 17,  EO_INIT (.joints) 4}, //board 18, 1b2
    
    /* v3 icub right-forearm boards */
    //TBA

}; EO_VERIFYsizeof(eomcconfig_boardjomos_MC4PLUS, eOmcconfig_boardjomosnumberof*sizeof(eomcconfig_boardjomos_t));

const eomcconfig_boardcode_t eomcconfig_boardcode_MC4PLUS[] =
{
    /* experimentals */
    {EO_INIT(.board_num) 97,  EO_INIT (.value) eOmcconfig_value_MC4PLUS_experimental_2joints}, // experimental board
    {EO_INIT(.board_num) 98,  EO_INIT (.value) eOmcconfig_value_MC4PLUS_experimental}, // experimental board
    
    /* v3 icub head boards */
    {EO_INIT(.board_num) 11,  EO_INIT (.value) eOmcconfig_value_MC4PLUS_b0}, //board 12, 0b0
    {EO_INIT(.board_num) 12,  EO_INIT (.value) eOmcconfig_value_MC4PLUS_b1}, //board 13, 0b1
    {EO_INIT(.board_num) 13,  EO_INIT (.value) eOmcconfig_value_MC4PLUS_b7}, //board 14, 0b7
    {EO_INIT(.board_num) 14,  EO_INIT (.value) eOmcconfig_value_MC4PLUS_b9}, //board 15, 0b9
    
    /* v3 icub left-forearm boards */
    {EO_INIT(.board_num) 15,  EO_INIT (.value) eOmcconfig_value_MC4PLUS_1b4},         //board 16, 1b4
    {EO_INIT(.board_num) 16,  EO_INIT (.value) eOmcconfig_value_MC4PLUS_unspecified}, //board 17, 1b3
    {EO_INIT(.board_num) 17,  EO_INIT (.value) eOmcconfig_value_MC4PLUS_unspecified}, //board 18, 1b2
    
    
    /* v3 icub right-forearm boards */
    //TBA

}; EO_VERIFYsizeof(eomcconfig_boardcode_MC4PLUS, eOmcconfig_boardjomosnumberof*sizeof(eomcconfig_boardcode_t));

//const pointer reference to const structures
const eomcconfig_codeconfig_t * const eomcconfig_codeconfigs[] =
{   
    (const eomcconfig_codeconfig_t *)&eomcconfig_codeconfig_2FOC,   
    (const eomcconfig_codeconfig_t *)&eomcconfig_codeconfig_MC4CAN,        
    (const eomcconfig_codeconfig_t *)&eomcconfig_codeconfig_MC4PLUS
};  EO_VERIFYsizeof(eomcconfig_codeconfigs, eOmcconfig_type_numberof*sizeof(const eomcconfig_codeconfig_t *));  

const eomcconfig_valuestring_t * const eomcconfig_valuestrings[] =
{   
    (const eomcconfig_valuestring_t *)NULL, //not handled   
    (const eomcconfig_valuestring_t *)NULL, //not handled       
    (const eomcconfig_valuestring_t *)&eomcconfig_valuestring_MC4PLUS,
};  EO_VERIFYsizeof(eomcconfig_valuestrings, eOmcconfig_type_numberof*sizeof(const eomcconfig_valuestring_t *));
 

// - end of: section for configuration 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern eOmcconfig_code_t eOmcconfig_code_get(eOmcconfig_type_t type, eOmcconfig_value_t val)
{
    eOmcconfig_code_t cc = 0;
    
    uint16_t t = (uint16_t)type;
    
    if(t > eOmcconfig_type_numberof)
    {
        return(eOmcconfig_code_dummy);
    }
    
    if(val > s_eomcconfig_maxvalue_in_type[t-1])
    {
        return(eOmcconfig_code_dummy);
    }
     
    cc = EOMCCONFIGCODE(t, val);

    return(cc);
}


extern eOmcconfig_type_t eOmcconfig_code2type(eOmcconfig_code_t code)
{
    uint16_t type = (code & EOMCCONFIG_TYPE_MASK) >> EOMCCONFIG_TYPE_SHIFT;
        
    if(type > eOmcconfig_type_numberof)
    {
        return(eOmcconfig_type_dummy);
    }
    
    return((eOmcconfig_type_t)type);    
}


extern eOmcconfig_value_t eOmcconfig_code2value(eOmcconfig_code_t code)
{  
    eOmcconfig_type_t type = eOmcconfig_code2type(code);
    uint16_t val = code & EOMCCONFIG_VALUE_MASK;
    
    if(eOmcconfig_value_dummy == type)
    {
        return(eOmcconfig_value_dummy);
    }
       
    if(val > s_eomcconfig_maxvalue_in_type[type -1])
    {
        return(eOmcconfig_value_dummy);
    }
        
    return(val);    
}

extern const eOmcconfig_jomo_cfg_t* eOmcconfig_code2config(eOmcconfig_code_t code)
{
    eOmcconfig_type_t type = eOmcconfig_code2type(code);
    eOmcconfig_value_t val = eOmcconfig_code2value(code);
    
    if((eOmcconfig_type_dummy == type) || (eOmcconfig_value_dummy == val))
    {
        return(NULL);
    }
    
    if(NULL == eomcconfig_codeconfigs[type -1])
    {
        return(NULL);
    }

    if(val != eomcconfig_codeconfigs[type -1][val].value)
    {
        return(NULL);
    }
    
    return(eomcconfig_codeconfigs[type -1][val].jomos);
}

extern eOmcconfig_value_t eOmcconfig_string2value(const char * str, eOmcconfig_type_t type)
{
    if((type > eOmcconfig_type_numberof) || (str == NULL))
    {
        return(eOmcconfig_value_dummy);
    }
    //search here the right string for the specified config
    uint8_t i = 0;
    for (;i < eOmcconfig_value_MC4PLUS_numberof; i++)
    {
        if (strcmp(eomcconfig_valuestrings[type-1][i].string, str) == 0)
        {
            return eomcconfig_valuestrings[type-1][i].value;
        }
    }
    
    // string not found
    return eOmcconfig_value_dummy;
}



extern void eOmcconfig_Set_Active_Code(eOmcconfig_code_t code)
{
    if (eOmcconfig_code2type(code) ==  eOmcconfig_type_dummy)
       return;
    
    if (eOmcconfig_code2value(code) ==  eOmcconfig_value_dummy)
       return;
    
    //if the config reference is valid, set the code
    if (eOmcconfig_code2config(code) != NULL)
       s_eo_mcconf.active_code = code;
       s_eo_mcconf.initted = eobool_true;
       return;
}


extern eOmcconfig_code_t eOmcconfig_Get_Active_Code(void)
{
    if (s_eo_mcconf.initted == eobool_false)
    {
        return eOmcconfig_code_dummy;
    }
    
    return s_eo_mcconf.active_code;
}



//get MC config from board number
extern const eOmcconfig_jomo_cfg_t* eOmcconfig_board2config(uint8_t board_num, eOmcconfig_type_t type)
{
    for (uint8_t i=0; i<eOmcconfig_boardjomosnumberof; i++)
    {
        if(eomcconfig_boardcode_MC4PLUS[i].board_num == board_num)
        {
            eOmcconfig_code_t cfg_code = eOmcconfig_code_get(type,eomcconfig_boardcode_MC4PLUS[i].value);
            if (cfg_code != eOmcconfig_code_dummy)
                return eOmcconfig_code2config(cfg_code);
        }
    }
    
    return NULL;  
}

extern uint8_t eOmcconfig_board2jomosnumber(uint8_t board_num)
{
    for (uint8_t i=0; i<eOmcconfig_boardjomosnumberof; i++)
    {
        if(eomcconfig_boardjomos_MC4PLUS[i].board_num == board_num)
        {
            return eomcconfig_boardjomos_MC4PLUS[i].joints;
        }
    }
    return eOmcconfig_board_dummy; 
}
//deprecated
/*
extern const char* eoerror_code2string(eOerror_code_t code)
{
    static const char interror_codeunrecognised[] = "eoerror_INTERNALERROR__codeunrecognised";
    static const char interror_categoryisempty[] = "eoerror_INTERNALERROR__categoryisempty";
    static const char interror_valueisempty[] = "eoerror_INTERNALERROR__valueisempty";
    static const char interror_stringatwrongplace[] = "eoerror_INTERNALERROR__stringatwrongplace";
       
    eOerror_category_t cat = eoerror_code2category(code);
    eOerror_value_t val = eoerror_code2value(code);
    
    if((eoerror_category_dummy == cat) || (eoerror_value_dummy == val))
    {
        return(interror_codeunrecognised);
    }
    
    if(NULL == eoerror_valuestrings[cat])
    {
        return(interror_categoryisempty);
    }

    if(NULL == eoerror_valuestrings[cat][val].string)
    {
        return(interror_valueisempty);
    }

    if(val != eoerror_valuestrings[cat][val].value)
    {
        return(interror_stringatwrongplace);
    }
    
    return(eoerror_valuestrings[cat][val].string);
}
*/


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



