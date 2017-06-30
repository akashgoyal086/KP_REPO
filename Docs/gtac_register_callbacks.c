
//type/***    REGISTER_RUNTIME_PROPERTIES_USING_CUSTOM_EXITS ***/
/* 
    Instructions:
        Save this file as gtac_register_callbacks.c
        The naming format is very important.
            XXXX_register_callbacks.c
            XXXX_register_callbacks() - function name
            XXXX - the name of the executable

        Compile 
            %TC_ROOT%\sample\compile -DIPLIB=libuser_exits gtac_register_callbacks.c
        Link
            %TC_ROOT%\sample\link_custom_exits gtac
        Move File
            copy gtac.dll %TC_BIN%

        Set the following preference
            TC_customization_libraries=
            gtac
*/
#include <ict/ict_userservice.h>
#include <tccore/tcaehelper.h>
#include <user_exits/aiws_ext.h>
#include <tc/preferences.h>
#include <ss/ss_errors.h>
#include <tccore/custom.h>
#include <itk/mem.h>
#include <tccore/grm.h>
#include <tccore/grmtype.h>
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tccore/method.h>
#include <property/prop.h>
#include <property/prop_msg.h>
#include <tccore/tctype.h>
#include <property/prop_errors.h>
#include <tccore/item.h>
#include <lov/lov.h>
#include <sa/sa.h>
#include <tc/emh.h>
#include <res/reservation.h>
#include <tccore/workspaceobject.h>
#include <tc/wsouif_errors.h>
#include <tccore/aom.h>
#include <form/form.h>

#define EXIT_FAILURE 1 
#define ERROR_CHECK(X) (report_error( __FILE__, __LINE__, #X, (X)))
static void report_error( char *file, int line, char *function, int return_code)
{
    if (return_code != ITK_ok)
    {
        char *error_message_string;
        EMH_get_error_string (NULLTAG, return_code, &error_message_string);
        printf ("ERROR: %d ERROR MSG: %s.\n", return_code, error_message_string);
        printf ("FUNCTION: %s\nFILE: %s LINE: %d\n", function, file, line);
        if(error_message_string) MEM_free(error_message_string);
        printf("\nExiting program!\n");
        exit (EXIT_FAILURE);    
    }
}
static tag_t ask_item_revisions_master_form(tag_t item_revision)
{
    int
        n_secondary_objects = 0;
    tag_t
        relation = NULLTAG,
        *secondary_objects = NULL,
        item_revision_master_form = NULLTAG;

    ERROR_CHECK(GRM_find_relation_type("IMAN_master_form", &relation));
    ERROR_CHECK(GRM_list_secondary_objects_only(item_revision, relation, 
        &n_secondary_objects, &secondary_objects));

    /* should always be just one */
    item_revision_master_form = secondary_objects[0];

    if (secondary_objects) MEM_free(secondary_objects);
    return item_revision_master_form;
}

extern int  GTAC_ask_double_value( METHOD_message_t *  m, va_list  args )
{
    /* va_list for PROP_ask_value_double_msg */
    tag_t prop_tag = va_arg( args, tag_t );
    double  *value = va_arg(args, double *);
    /*****************************************/

    int
        ifail = ITK_ok;
    tag_t 
        item_revision = NULLTAG,         
        form = NULLTAG; 
    double
        get_double = 0.00;
   printf( "\t\t\t GTAC_ask_double_value\n ");
   
   // replacement for deprecated PROP_ask_owning_object */
   METHOD_PROP_MESSAGE_OBJECT(m, item_revision); 

   form = ask_item_revisions_master_form(item_revision);

   AOM_get_value_double(form, "form_weight", &get_double);
   printf("\t\t\t\t\t AOM_get_value_double: %f\n\n", get_double);

   *value = get_double;

   return ifail;
}

extern int GTAC_init_runtime_property(METHOD_message_t* m, va_list args)
{
    int 
        ifail = ITK_ok;
    tag_t 
        prt_tag = NULLTAG, 
        new_property = NULLTAG,
        type_tag = va_arg(args, tag_t);
    METHOD_id_t 
        method;
    char 
        typename[TCTYPE_name_size_c+1] = "";


    printf( "\t\t GTAC_init_runtime_property\n   "); 
    
    if ((ifail = TCTYPE_ask_name(type_tag, typename)) != ITK_ok) 
        return ifail;

    if ((ifail = METHOD_register_prop_method((const char*)typename, "weight", PROP_ask_value_double_msg,  
        GTAC_ask_double_value, 0, &method )) != ITK_ok )
    {
        return ifail;
    }

    return(ITK_ok);
}
                                        /* (void) if not custom exits */
extern int GTAC_register_runtime_properties(int *decision )
{

    int ifail = ITK_ok;
        
    USER_prop_init_entry_t user_types_methods[] = 
    {
        { "ItemRevision" , GTAC_init_runtime_property , NULL },
        { ""                , 0                   , NULL }
    };
 
    int n_types = sizeof(user_types_methods)/sizeof(USER_prop_init_entry_t);

    printf( "\t GTAC_register_properties\n");

    *decision = ALL_CUSTOMIZATIONS;  /* remove this line if not custom exits */ 
    /*
     * Do not edit this call. This call is not to be used elsewhere.
     */
    ifail = TCTYPE_register_properties(user_types_methods, n_types);

    return ifail;
}


extern DLLAPI int GTAC_register_callbacks ()
{ 
    printf( "\n\n GTAC_register_callbacks\n");

    CUSTOM_register_exit ( "GTAC", "USER_register_properties", 
       (CUSTOM_EXIT_ftn_t) GTAC_register_runtime_properties );
       
  return ( ITK_ok );
}