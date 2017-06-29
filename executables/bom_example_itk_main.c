/*=============================================================================
                Copyright (c) 2003-2005 UGS Corporation
                   Unpublished - All Rights Reserved
===============================================================================
File description:
    An example program to show listing a bill of materials using the BOM module
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unidefs.h>
#include <itk/mem.h>
#include <tcinit/tcinit.h>
#include <tccore/item.h>
#include <bom/bom.h>
#include <cfm/cfm.h>
#include <ps/ps_errors.h>
#include <tccore/item_errors.h>
#include <tc/emh.h>
#include <tc/tc_util.h>
#include <itk/mem.h>

/* this sequence is so common */
#define CHECK_FAIL if (ifail != 0) { TC_printf ("line %d (ifail %d)\n", __LINE__, ifail); exit (0);}

static int name_attribute, seqno_attribute, parent_attribute, item_tag_attribute;

static void initialise (void);
static void initialise_attribute (char *name,  int *attribute);
static void print_bom (tag_t line, int depth);
static void print_average (tag_t top_line);
static void find_revision_count (tag_t line, int *count, int *total);
static void double_sequence_nos (tag_t line);
static int my_compare_function (tag_t line_1, tag_t line_2, void *client_data);


/*-------------------------------------------------------------------------------*/


/* This program lists the bill under some given item (configuring it by latest+working).
   It then doubles all the sequence numbers,  and re-lists it by descending sequence number order.
   It then counts how many lines there are in the bill,  and the average number of revisions of
   the items in that bill.
   It never actually modifies the database.
   Run this program by:
    <program name> -u<user> -p<password>
*/

extern int ITK_user_main (int argc, char ** argv )
  {
    int ifail;
    char *req_item = NULL;
    char *req_key = NULL;
    tag_t window, window2, rule, item_tag = null_tag, top_line;

    (void)argc;
    (void)argv;

    initialise();

    req_item = ITK_ask_cli_argument("-i=");
    req_key = ITK_ask_cli_argument("-key=");

    if ( req_item && req_key )
    {
        TC_printf("Cannot use both the \"-i\" and \"-key\" options on command line\n");
        exit(0);
    }
    else if ( !req_item && !req_key )
    {
        TC_printf("Must supply the \"-i\" or \"-key\" option on command line\n");
        exit(0);
    }

    ifail = BOM_create_window (&window);
    CHECK_FAIL;
    ifail = CFM_find( "Latest Working", &rule );
    CHECK_FAIL;
    ifail = BOM_set_window_config_rule( window, rule );
    CHECK_FAIL;
    ifail = BOM_set_window_pack_all (window, true);
    CHECK_FAIL;

    if ( req_item )
    {
        tag_t *tags_found = NULL;
        int n_tags_found= 0;
        char **attrs = (char **) MEM_alloc(1 * sizeof(char *));
        char **values = (char **) MEM_alloc(1 * sizeof(char *));
        attrs[0] ="item_id";
        values[0] = (char *)req_item;
        ifail = ITEM_find_items_by_key_attributes(1,attrs, values, &n_tags_found, &tags_found);
        MEM_free(attrs);
        MEM_free(values);
        CHECK_FAIL;
        if (n_tags_found == 0)
        {
            TC_printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", req_item);
            exit (0);
        }
        else if (n_tags_found > 1)
        {
            MEM_free(tags_found);
            EMH_store_initial_error(EMH_severity_error,ITEM_multiple_items_returned);
            TC_printf ( "More than one items matched with id %s\n", req_item);
            exit (0);
        }
        item_tag = tags_found[0];

        MEM_free(tags_found);
    }

    else // req_key being used
    {
        {
            tag_t *tags_found = NULL;
            int n_tags_found = 0;
            ifail = ITEM_find_items_by_string(req_key, &n_tags_found, &tags_found);

            if (ifail == ITK_ok)
            {
                if (n_tags_found == 0)
                {
                    TC_printf ("ITEM_find_items_by_string returns success,  but didn't find %s\n", req_key);

                    exit (0);
                }
                else if (n_tags_found > 1)
                {
                    MEM_free(tags_found);
                    EMH_store_initial_error(EMH_severity_error,ITEM_multiple_items_returned);
                    TC_printf ( "More than one items matched with key %s\n", req_key);
                    exit (0);
                }
                else
                {
                    item_tag = tags_found[0];
                    MEM_free(tags_found);
                }
            }
        }
    }

    if (item_tag == null_tag)
    {
        TC_printf ("ITEM_find_items_by_key_attributes returns success,  but didn't find %s\n", req_item);
        exit (0);
    }

    ifail = BOM_set_window_top_line (window, item_tag, null_tag, null_tag, &top_line);
    CHECK_FAIL;

    print_bom (top_line, 0);

    double_sequence_nos (top_line);
    /* we won't go
        ifail = BOM_save_window (window);
       because that would modify the database,  and we are just playing here
    */

    /* now let's be silly and list the results sorted the other way */

    ifail = BOM_create_window (&window2);
    CHECK_FAIL;
    ifail = BOM_set_window_config_rule( window2, rule );
    CHECK_FAIL;
    ifail = BOM_set_window_pack_all (window, false);    /* the default,  but let's be explicit */
    CHECK_FAIL;

    ifail = BOM_set_window_sort_compare_fn (window2, (void *)my_compare_function, NULL);
    CHECK_FAIL;

    ifail = BOM_set_window_top_line (window2, item_tag, null_tag, null_tag, &top_line);
    CHECK_FAIL;
    //TC_printf ("================================================\n");
  //  print_bom (top_line, 0);

    print_average (top_line);

    ITK_exit_module(true);


    return 0;
  }


/*-------------------------------------------------------------------------------*/


static void print_bom (tag_t line, int depth)
  {
    int ifail;
	char *VO_item_id = NULL;
    char *name, *sequence_no, *blItemID,*item_type ,*item_type_PROJ, *szSectorName;
    int i, n;
    tag_t *children;
	tag_t itemPOD = NULLTAG;
	tag_t itemPROJ = NULLTAG;
	tag_t revPOD = NULLTAG;
	tag_t item1 = NULLTAG;
	tag_t   rev1 = NULLTAG;
	tag_t item = NULLTAG;
	tag_t   rev = NULLTAG;
	tag_t item2 = NULLTAG;
	tag_t   rev2 = NULLTAG;
	tag_t item3 = NULLTAG;
	tag_t   rev3 = NULLTAG;
	tag_t item4 = NULLTAG;
	tag_t   rev4 = NULLTAG;
	tag_t child_line = NULLTAG;
	tag_t tCurrentSectorPROP = NULLTAG;
	

	tag_t *children2;
	tag_t top_line;
	tag_t window3;
	tag_t bv = NULLTAG;
	tag_t bvr = NULLTAG;
		tag_t bvpod = NULLTAG;
	tag_t bvrpod = NULLTAG;
    depth ++;
    ifail = BOM_line_ask_attribute_string (line, name_attribute, &name);
    CHECK_FAIL;
    /* note that I know name is always defined,  but sometimes sequence number is unset.
       If that happens it returns NULL,  not an error.
    */
    ifail = BOM_line_ask_attribute_string (line, seqno_attribute, &sequence_no);
    CHECK_FAIL;
	
	
   // TC_printf ("depth %3d", depth);
    for (i = 0; i < depth; i++)
      TC_printf ("  ");
   

    ifail = BOM_line_ask_child_lines (line, &n, &children);
	// TC_printf ("%-20s level is %d %s\n", name, n, sequence_no == NULL ? "<null>" : sequence_no);
	// TC_printf ("%-20s level is %d %s\n", name, n, sequence_no);
    CHECK_FAIL;
	
	//printf("ifail CHECK_FAIL %d\n", ifail);
	ifail = BOM_create_window (&window3);  
	
	ifail = AOM_ask_value_string(line, "bl_item_item_id", &blItemID);
	ifail = ITEM_find_item(blItemID,&itemPOD);	
	ifail =	ITEM_find_rev(blItemID,"A",&revPOD);
	ifail = ITEM_ask_type2(itemPOD,&item_type);
	//printf("ifail ITEM_ask_type2 %d\n", ifail);
	//printf("ifail ITEM_ask_type2 %d\n", ifail);
	//printf("tc_strcmp\n");
	if(tc_strcmp(item_type,"KP2_POD")==0)
	{
	//printf("tc_strcmp2\n");
		ifail = ITEM_find_item("PRJ-0258",&itemPROJ);
	//printf("ifail ITEM_find_item %d\n", ifail);
	ifail = ITEM_ask_type2(itemPROJ,&item_type_PROJ);
	//printf("ifail ITEM_ask_type2 %d\n", ifail);	
	if(tc_strcmp(item_type_PROJ,"KP2_Project")==0)
	{
		ifail = AOM_ask_value_string(itemPROJ,"kp2_Sector", &szSectorName);
		printf("ifail AOM_ask_value_string %d %s %s %s\n", ifail, szSectorName, item_type, item_type_PROJ);
		ifail = (PROP_UIF_ask_property_by_name(itemPOD,"Sector", &tCurrentSectorPROP));
		printf("ifail PROP_UIF_ask_property_by_name %d\n", ifail);
		  ifail = (AOM_refresh(itemPOD, TRUE));
		  ifail = (AOM_lock(itemPOD));
		  printf("ifail AOM_refresh %d\n", ifail);
            	ifail = (PROP_UIF_set_value(tCurrentSectorPROP, szSectorName));	
				printf("ifail PROP_UIF_set_value %d\n", ifail);
            ifail = (AOM_save(itemPOD));
			printf("ifail AOM_Save %d\n", ifail);
			ifail = (AOM_refresh(itemPOD, FALSE));   
			printf("ifail AOM_refresh %d\n", ifail);
			
			   
				
		
		//ifail = AOM_set_value_string(itemPOD,"kp2_Sector", "Agriculture");
		//printf(" ifail AOM_set_value_string blItemID %s %d %s\n", blItemID, ifail, szSectorName);
	}
		
		
	}
	//printf("*****\n");
	
	ifail = (AOM_save (itemPOD));
	
	
	
	
	//begin top line
	
	//printf("item_type %s\n", item_type);
	if(n==0 && tc_strcmp(item_type,"KP2_POD")==0)
	{
		
		
		
	ifail = (PS_create_bom_view (NULLTAG, "", "", itemPOD, &bvpod));
	//TC_printf ("PS_create_bom_view is %d\n", ifail);

	ifail =   (AOM_save (bvpod));
	//TC_printf ("AOM_save is %d\n", ifail);


   ifail =  (PS_create_bvr (bvpod, "", "",  false, revPOD, &bvrpod));
 //  TC_printf ("PS_create_bvr is %d\n", ifail);

  ifail =   (AOM_save (bvrpod));
 // TC_printf ("AOM_save is %d\n", ifail);

     ifail = (AOM_save (itemPOD));
	 ifail = (AOM_save (item));
	  //ITEM_find_item("PD-000906",&itemPOD);	
	//ITEM_find_rev("PD-000906","A",&revPOD);
	
	 ifail = BOM_set_window_top_line (window3, itemPOD,revPOD , null_tag, &top_line);
	//TC_printf ("BOM_set_window_top_line is %d\n", ifail);
	

	
	ifail = ITEM_create_item(NULL, "Chapter", "KP2_VoiceOver", "A", &item, &rev);
	//TC_printf ("ITEM_create_item is %d\n", ifail);
	 ITEM_ask_id2	(item,&VO_item_id);	
	 
	 TC_printf ("VO_item_id is %s\n", VO_item_id);
	ifail = (AOM_save (item));
	ifail = ITEM_create_item(NULL, "Chapter", "KP2_VoiceOverSc", "A", &item1, &rev1);
	//TC_printf ("ITEM_create_item is %d\n", ifail);
	ifail = (AOM_save (item1));
	//TC_printf ("AOM_save is %d\n", ifail);
	ifail = ITEM_create_item(NULL, "Chapter", "KP2_Vis", "A", &item2, &rev2);
	//TC_printf ("ITEM_create_item is %d\n", ifail);
	ifail = (AOM_save (item2));
	//TC_printf ("AOM_save is %d\n", ifail);
	ifail = ITEM_create_item(NULL, "Chapter", "KP2_StoryBoard", "A", &item3, &rev3);
	//TC_printf ("ITEM_create_item is %d\n", ifail);
	ifail = (AOM_save (item3));
	//TC_printf ("AOM_save is %d\n", ifail);
	ifail = ITEM_create_item(NULL, "Chapter", "KP2_Multimedia", "A", &item4, &rev4);
	//TC_printf ("ITEM_create_item is %d\n", ifail);
	ifail = (AOM_save (item4));
	//TC_printf ("AOM_save is %d\n", ifail);	
	
	 
	
	
	//AOM_ask_value_string(line, "bl_item_item_id", &blItemID);

 

	//TC_printf ("blItemID %s\n", blItemID);
	ifail = (BOM_line_add(top_line, item, rev, NULLTAG, &child_line));
		ifail = (BOM_line_add(top_line, item1, rev1, NULLTAG, &child_line));
		ifail = (BOM_line_add(top_line, item2, rev2, NULLTAG, &child_line));
		ifail = (BOM_line_add(top_line, item3, rev3, NULLTAG, &child_line));
		ifail = (BOM_line_add(top_line, item4, rev4, NULLTAG, &child_line));
	//TC_printf ("BOM_line_add is %d\n", ifail);
    ifail= (BOM_save_window(window3));  
	//TC_printf ("BOM_save_window is %d\n", ifail);
  
	(BOM_close_window(window3));
	
	}
    for (i = 0; i < n; i++)
     print_bom (children[i], depth);

    MEM_free (children);
	MEM_free (szSectorName);
	MEM_free (VO_item_id);
	
	
    MEM_free (name);
    MEM_free (sequence_no);
  }


/*-------------------------------------------------------------------------------*/

static void double_sequence_nos (tag_t line)
{
    /* just to demonstrate modifying the bill */

    int ifail;
    char *sequence_no;
    int i, n;
    tag_t *children;

    ifail = BOM_line_ask_attribute_string (line, seqno_attribute, &sequence_no);
    CHECK_FAIL;
    /* Top bom lines have no sequence number, and others may also not */
    if (sequence_no[0] != '\0')
    {
        char buffer[100];
        sprintf (buffer, "%d", 2 * atoi (sequence_no));
        ifail = BOM_line_set_attribute_string (line, seqno_attribute, buffer);
        if (ifail != ITK_ok)
        {
            char *child_name;

            ifail = BOM_line_ask_attribute_string (line, name_attribute, &child_name);
            CHECK_FAIL;

            TC_printf ("==> No write access to %s\n", child_name);
            MEM_free (child_name);
        }
        else
        {
            CHECK_FAIL;
        }
        MEM_free (sequence_no);
    }

    ifail = BOM_line_ask_child_lines (line, &n, &children);
    CHECK_FAIL;
    for (i = 0; i < n; i++)
        double_sequence_nos (children[i]);

    MEM_free (children);
}


/*-------------------------------------------------------------------------------*/


static void print_average (tag_t top_line)
{
    int count = 0, total = 0;
    find_revision_count (top_line, &count, &total);
    if (count <= 0)
    {
        TC_printf ("impossible error has happened!\n");
    }
    else
    {
        TC_printf ("lines in bill : %d\naverage revisions of each item : %d\n", count, total / count);
    }
}


/*-------------------------------------------------------------------------------*/


static void find_revision_count (tag_t line, int *count, int *total)
{
    /* a function to demonstrate going from BOM tags to other IMAN classes                      */
    /* in this case we get the Item tag for the BOM line,  and then use it for an ITEM call     */
    /* for a complete list of standard attributes see bom_attr.h,  or use a BOM_list_attributes */
    /* call to find all current attributes (new note types define new attributes)               */

    int ifail;
    tag_t item_tag, *revisions, *children;
    int i, n, revision_count;
    ifail = BOM_line_ask_attribute_tag(line, item_tag_attribute,  &item_tag );
    CHECK_FAIL;
    /* the simplist example call I can think of:  count how many revisions of this Item there are */
    ifail = ITEM_list_all_revs (item_tag, &revision_count, &revisions);
    CHECK_FAIL;
    MEM_free (revisions);
    (*count)++;
    (*total) += revision_count;

    ifail = BOM_line_ask_child_lines (line, &n, &children);
    CHECK_FAIL;
    for (i = 0; i < n; i++)
        find_revision_count (children[i], count, total);

    MEM_free (children);
}


/*-------------------------------------------------------------------------------*/


static int my_compare_function (tag_t line_1, tag_t line_2, void * client_data)
  {
    /* returns strcmp style -1/0/+1 according to whether line_1 and line_2 sort <, = or > */
    char *seq1, *seq2;
    int ifail, result;
    (void)client_data;
    ifail = BOM_line_ask_attribute_string (line_1, seqno_attribute, &seq1);
    CHECK_FAIL;
    ifail = BOM_line_ask_attribute_string (line_2, seqno_attribute, &seq2);
    CHECK_FAIL;
    if (seq1 == NULL || seq2 == NULL)
      result = 0;
    else
      result = strcmp (seq2, seq1);  /* we are being silly and doing a reverse sort */
    /* note:  the default sort function compares Item names if the sequence numbers sort equal
       but this is just an example for fun,  so we won't bother with that bit
    */
    MEM_free (seq1);
    MEM_free (seq2);
    return result;
  }


/*-------------------------------------------------------------------------------*/


static void initialise (void)
  {
    int ifail;

    /* <kc> pr#397778 July2595 exit if autologin() fail */
    //if ((ifail = ITK_auto_login()) != ITK_ok)
		 if ((ifail = ITK_init_module("tcadmin","tcadmin","dba"))!=ITK_ok);
       //TC_fprintf(stderr,"Login fail !!: Error code = %d \n\n",ifail);
       TC_fprintf(stderr,"Login fail !!: Error code = %d \n\n",ifail);
    CHECK_FAIL;

    /* these tokens come from bom_attr.h */
    initialise_attribute (bomAttr_lineName, &name_attribute);
    initialise_attribute (bomAttr_occSeqNo, &seqno_attribute);
    ifail = BOM_line_look_up_attribute (bomAttr_lineParentTag, &parent_attribute);
    CHECK_FAIL;
    ifail = BOM_line_look_up_attribute (bomAttr_lineItemTag, &item_tag_attribute);
    CHECK_FAIL;
  }


/*-------------------------------------------------------------------------------*/


static void initialise_attribute (char *name,  int *attribute)
  {
    int ifail, mode;

    ifail = BOM_line_look_up_attribute (name, attribute);
    CHECK_FAIL;
    ifail = BOM_line_ask_attribute_mode (*attribute, &mode);
    CHECK_FAIL;
    if (mode != BOM_attribute_mode_string)
      { TC_printf ("Help,  attribute %s has mode %d,  I want a string\n", name, mode);
        exit(0);
      }
  }
