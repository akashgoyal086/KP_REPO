/* Sample code; file: test_itk_main.c */


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tccore/aom.h>
#include <bom/bom.h>
#include <tc/emh.h>
#include <tc/folder.h>
#include <tc/tc.h>
#include <sa/tcfile.h>
#include <tccore/item.h>
#include <epm/epm.h>
#include <itk/mem.h>
#include <ps/ps.h>
#include <sa/user.h>
#include <tccore/tctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <pom/pom/pom.h>
#include <string>
 #include <tc/preferences.h>
 #include <epm/signoff.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <itk/mem.h>
#include <tccore/aom_prop.h>
#include <tccore/workspaceobject.h>
#define IFERR_ABORT(X)  (report_error( __FILE__, __LINE__, #X, X, TRUE))
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
static int report_error(char *file, int line, char *call, int status, logical exit_on_error);
char* callRecursive(tag_t  revTag, char*  kp2_projRevID);
static void ECHO(char *format, ...);
static void GTAC_free(void *what);
using namespace std;
int ITK_user_main(
int argc, /* I number of command line arguments */
char* argv[] /* I list of command line arguments */
)
/*
* Description: This program is a follow-up action.
*/
{
int 		ifail = ITK_ok;
int 		n_attachs = 0;
int 		participant_count =0;
int 		subtaskCount=0;
int 		currentTaskCount=0;
int 		task_count=0;
char* 		job_tag_string = NULL;
char  		type[WSO_name_size_c + 1] = "";
char*        kp2_projRevID;
char        inputFile[1250]= "C:\\Temp\\KP_EMAIL_NOTIFY_creators";
char		directoryPath[1250]		= {'\0'};
char 		native_file_spec[250]	= {'\0'};
char 		item_id[ITEM_id_size_c + 1] = "";
char  		typeOfSelObj[WSO_name_size_c + 1]="";
char  		typeOfTASKSelObj[WSO_name_size_c + 1]="";
char  		typeOfProjrevObj[WSO_name_size_c + 1]="";
char 	    rolename[SA_name_size_c+1]	 = "";
char        person_name[SA_person_name_size_c+1] = "";
char        quotes[SA_person_name_size_c+1] = "\"";
char        initiator_person_name[SA_person_name_size_c+1] = "";

char 	    *uuid = NULL;
char		* email_address = 0;
char *		sitePrefValue 			= NULL;
char *      szProjectName = NULL;
char *      szItemName = NULL;
char *      szInitiatorName = NULL;
char *      szTaskName = NULL;
char *      szSubTaskName = NULL;
char *      currentTaskNames = NULL;
char *      currentTaskName = NULL;
char szStart[1000] 							= "Start ";
char szSpace[10] 							= " ";
tag_t 		root_task = NULLTAG;
tag_t 		*szSubTask = NULLTAG;
tag_t 		job_tag = NULLTAG;
tag_t*	    attachs = NULL;
tag_t 		revTag = NULLTAG;
tag_t 		item_tag = NULLTAG;
tag_t 		user_tag = NULLTAG;
tag_t 		projRevTag=NULLTAG;
tag_t		participant_type = NULLTAG;
tag_t*      participant_list= NULLTAG;
tag_t 		member_tag = NULLTAG;
tag_t 		member_user_tag = NULLTAG;
tag_t 		initiator_member_user_tag = NULLTAG;
tag_t 		currentTaskTag = NULLTAG;
tag_t  		role_tag= NULLTAG;
tag_t		 person_tag = NULLTAG;
tag_t       * task_list=NULLTAG;
TC_preference_search_scope_t original_scope;

ifstream 	inFile;
ITK_initialize_text_services (ITK_BATCH_TEXT_MODE);
		std::srand(std::time(0)); // use current time as seed for random generator
		int random_variable = std::rand();		
		ofstream myfile;
		tc_strcat(inputFile,std::to_string(random_variable).c_str());
		tc_strcat(inputFile,".log");
		
		myfile.open (inputFile);
		myfile << "INFO:Writing this to a file.\n";
		
		if ( (ifail = ITK_auto_login ()) != ITK_ok)
		{
		myfile << "ERROR: login failed - error code = %d\n",ifail;
		return ( ifail );
		}
		myfile << "INFO:Get process tag string ...\n"; fflush(stdout);
		job_tag_string = ITK_ask_cli_argument("-zo=");

		if (!job_tag_string)
		{
		myfile <<"ERROR: no process tag string passed\n";
		ITK_exit_module(TRUE);
		return 1;
		}
		myfile << "INFO: process tag string = %s\n", job_tag_string;
		fflush(stdout);
		myfile << "INFO:Convert process tag string to process tag ...\n";
		fflush(stdout);
		if ( (ifail = POM_string_to_tag(job_tag_string, &job_tag))
		!= ITK_ok)
		{
		myfile << "ERROR: POM_string_to_tag failed - error code= %d\n",ifail;
		return ( ifail );
		}

		EPM_ask_root_task(job_tag, &root_task);
		ifail = EPM_ask_attachments(root_task, EPM_target_attachment, &n_attachs, &attachs);
		revTag = attachs[0];
		ITEM_ask_item_of_rev(revTag,&item_tag);
		ITEM_ask_id(item_tag,item_id);	
		myfile << "INFO: Processing for item_id= %s" << item_id << "\n";		
		(WSOM_ask_object_type(revTag, typeOfSelObj));		
		(WSOM_ask_object_type(root_task, typeOfTASKSelObj));
		
		//IFERR_REPORT(AOM_ask_value_tag(root_task,"fnd0ActuatedInteractiveTsks" , &currentTaskTag));
		//IFERR_REPORT(AOM_ask_value_string(currentTaskTag,"current_name" ,&currentTaskNames));
		//currentTaskCount=sizeof(currentTaskNames);
		//currentTaskName[0] = currentTaskNames[currentTaskCount-1];
		EPM_ask_sub_tasks	(root_task,&subtaskCount,&szSubTask);
		kp2_projRevID = callRecursive(revTag,kp2_projRevID);
	//	myfile << "INFO: Processing for currentTaskCount" << currentTaskCount << "\n";
		//myfile << "INFO: Processing for currentTaskNames" << currentTaskNames[0] << "\n";
		POM_string_to_tag(kp2_projRevID, &projRevTag); 
		IFERR_REPORT(WSOM_ask_object_type(projRevTag, typeOfProjrevObj));
		IFERR_REPORT(AOM_ask_value_tag(root_task,"fnd0WorkflowInitiator" ,&initiator_member_user_tag));
		SA_ask_user_person_name	(initiator_member_user_tag,initiator_person_name);
		IFERR_REPORT(AOM_ask_value_string(szSubTask[0],"fnd0AliasTaskName" ,&szSubTaskName));
		IFERR_REPORT(AOM_ask_value_string(root_task,"fnd0AliasTaskName" ,&szTaskName));
		IFERR_REPORT(AOM_ask_value_string(projRevTag,"object_name", &szProjectName));
		IFERR_REPORT(AOM_ask_value_string(revTag,"object_name" ,&szItemName));
		EPM_get_participanttype("KP2_Creator",&participant_type);
		ITEM_rev_ask_participants(projRevTag,participant_type,&participant_count,&participant_list);
		myfile << "INFO: participant_count is: " << participant_count << "\n";
		if(participant_count ==0)
		{
			myfile << "WARNING: participant_count is: " << participant_count << " Please add Particpant to " << typeOfProjrevObj <<"\n";
		}
		(PREF_ask_search_scope(&original_scope));
		(PREF_set_search_scope(TC_preference_site));
		(PREF_ask_char_value("KP_Master_directory_path", 0, &sitePrefValue));
		(PREF_set_search_scope(original_scope));
		tc_strcpy(directoryPath,sitePrefValue);
			for (int iii = 0; iii < participant_count; iii++)    
			{
				AOM_ask_value_tag(participant_list[iii],"assignee",&member_tag);
				SA_ask_groupmember_user( member_tag, &member_user_tag ) ;	
				
				IFERR_REPORT(SA_ask_groupmember_role(member_tag, &role_tag));
				ITK__convert_tag_to_uid(member_user_tag, &uuid);				
				SA_ask_role_name(role_tag,rolename);
				
				SA_ask_user_person	(member_user_tag,&person_tag);	
				SA_ask_user_person_name	(member_user_tag,person_name);
				SA_ask_person_email_address	(person_tag,&email_address);
				//if(email_address==NULL)
					if(sizeof(email_address)<1)
				{
					tc_strcpy(email_address,"empty email");
				}
				
				if((tc_strcmp(typeOfSelObj,"KP2_VoiceOverRevision")==NULL)&&(tc_strcmp(rolename,"Content Developer")==NULL))
				{
				ITEM_rev_add_participant(revTag,participant_list[iii]);	
				myfile  << "\n adding  member_user_role name " << rolename <<  "to " <<	 typeOfSelObj << "\n";			
				}
				if((tc_strcmp(typeOfSelObj,"KP2_VoiceOverScRevision")==NULL)&&(tc_strcmp(rolename,"Content Developer")==NULL))
				{
					myfile  << "\n adding  member_user_role name " << rolename <<  "to " <<	 typeOfSelObj << "\n";	
				ITEM_rev_add_participant(revTag,participant_list[iii]);				
				}
				if(((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"Multimedia Developer")==NULL))||((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"Multimedia Team")==NULL)))
				{
					//sprintf(person_name, "%s %s %s", quotes,person_name,quotes);
				//sprintf(initiator_person_name, "%s %s %s", quotes,initiator_person_name,quotes);
				//sprintf(szTaskName, "%s %s %s", quotes,szTaskName,quotes);
				
				EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list );	
					myfile << "BEGIN PRINTING VARIABLES" << "\n\n\n";
				myfile << "item_id  " << item_id << "\n" ;
				myfile << "email_address  " << email_address << "\n" ;
				myfile << "szProjectName  " << szProjectName << "\n" ;
				myfile << "szItemName  " << szItemName << "\n" ;
				myfile << "typeOfSelObj  " << typeOfSelObj << "\n" ;
				myfile << "person_name  " << person_name << "\n" ;
				myfile << "typeOfTASKSelObj  " << typeOfTASKSelObj << "\n" ;				
				myfile << "task_count  " << 	task_count << "\n" ;
				myfile << "initiator_person_name  " << initiator_person_name << "\n" ;
		
				/*tc_strcat(quotes,item_id);
				tc_strcat(item_id,quotes);
				tc_strcpy(quotes,NULL);				
				tc_strcat(quotes,szProjectName);
				tc_strcat(szProjectName,quotes);
				tc_strcpy(quotes,NULL);
				tc_strcat(quotes,szItemName);
				tc_strcat(szItemName,quotes);
				tc_strcpy(quotes,NULL);
				tc_strcat(quotes,typeOfSelObj);
				tc_strcat(typeOfSelObj,quotes);
				tc_strcpy(quotes,NULL);
				tc_strcat(quotes,person_name);
				tc_strcat(person_name,quotes);
				tc_strcpy(quotes,NULL);
				tc_strcat(quotes,szTaskName);
				tc_strcat(szTaskName,quotes);
				tc_strcpy(quotes,NULL);
				tc_strcat(quotes,initiator_person_name);
				tc_strcat(initiator_person_name,quotes);
				tc_strcpy(quotes,NULL);*/
		
				tc_strcat(szStart,"C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ");
			
				tc_strcat(szStart, item_id);
				tc_strcat(szStart, szSpace);	
				tc_strcat(szStart, email_address);		
				tc_strcat(szStart, szSpace);	
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szProjectName);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szItemName);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, typeOfSelObj);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, person_name);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, "Creation");
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");
				
				
				
			
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
					
					
					myfile  << "\n adding  member_user_role name " << rolename <<  "to " <<	 typeOfSelObj << "\n";
				ITEM_rev_add_participant(revTag,participant_list[iii]);				
				}
					
				if(((tc_strcmp(typeOfSelObj,"KP2_StoryBoardRevision")==NULL)&&(tc_strcmp(rolename,"Multimedia Developer")==NULL))||((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"Multimedia Team")==NULL)))
				{
					myfile  << "\n adding  member_user_role name " << rolename <<  "to " <<	 typeOfSelObj << "\n";
				ITEM_rev_add_participant(revTag,participant_list[iii]);				
				}
					if(((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"CAD Developer")==NULL))||((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"CAD Team")==NULL)))
				{
					myfile  << "\n adding  member_user_role name " << rolename <<  "to " <<	 typeOfSelObj << "\n";
				ITEM_rev_add_participant(revTag,participant_list[iii]);				
				}
				
			}
		myfile.close();
}
		

char* callRecursive(tag_t revTag, char*  kp2_projRevID)
{
	 int n_references = 0, *levels = NULL;
   tag_t *reference_tags = NULL;
 // char * kp2_projRevID=NULL;
   //char * type=NULL;
   char  type[WSO_name_size_c + 1]="";
    char **relations = NULL;
	char        inputFile[1250]= "C:\\Temp\\KP";
	ofstream myfile;
	
	tc_strcat(inputFile,"a.log");
		
	myfile.open (inputFile);

	 (WSOM_where_referenced(revTag, -1 , &n_references, &levels, &reference_tags,&relations));
	 myfile  << "n_references is" << n_references << "\n";
	 
		 for (int ii = 0; ii < n_references; ii++)
    {
       (WSOM_ask_object_type(reference_tags[ii], type)); 
			myfile  << "type is " << type<< "\n";
			
			if(tc_strcmp(type,"KP2_ProjectRevision")==NULL)
		{
			
				ITK__convert_tag_to_uid(reference_tags[ii], &kp2_projRevID);
				myfile  << "kp2_projRevID is " << kp2_projRevID<< "\n";
			break;
		}
			
		}
	
		MEM_free(levels);
		MEM_free(reference_tags);
		MEM_free(relations);
	
	   return kp2_projRevID;
	
}


static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0;
        const int
            *severities = NULL,
            *statuses = NULL;
        const char
            **messages;

        EMH_ask_errors(&n_errors, &severities, &statuses, &messages);
        if (n_errors > 0)
        {
            ECHO("\n%s\n", messages[n_errors-1]);
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            ECHO("\n%s\n", error_message_string);
        }

        ECHO("error %d at line %d in %s\n", status, line, file);
        ECHO("%s\n", call);
        if (exit_on_error)
        {
            ECHO("%s", "Exiting program!\n");
            exit (status);
        }
    }
    return status;
}

static void ECHO(char *format, ...)
{
    char msg[1000];
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    printf(msg);
    TC_write_syslog(msg);
}

static void GTAC_free(void *what)
{
    if (what != NULL)
    {
        MEM_free(what);
        what = NULL;
    }

}
