/*HEAD ACTION_HANDLER_TO_ASSIGN_RESOURCE_POOL_AS_REVIEWER CCC ITK */
/*
    Instructions:
        Save this file as GTAC_register_action_handler_callbacks.c
        The naming format is very important.
            GTAC_register_action_handler_callbacks.c
            GTAC_register_callbacks() - function name
            GTAC - the name of the executable

        Compile
            %TC_ROOT%\sample\compile -DIPLIB=none
                 GTAC_register_action_handler_callbacks.c
        Link
            %TC_ROOT%\sample\link_custom_exits GTAC
        Move File
            copy GTAC.dll %TC_USER_LIB%

        Set the following multi valued string Site Preference
        TC_customization_libraries=
        GTAC

    Handler Placement:
        "Assign" Task Action on the select-signoff-team of an Review Task
*/
#include <ict/ict_userservice.h>
#include <tccore/custom.h>
#include <itk/mem.h>
#include <tc/tc.h>
#include <user_exits/user_exits.h>
#include <tc/emh.h>
#include <epm/epm.h>
#include <sa/sa.h>
#include <stdarg.h>


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


#include <pom/pom/pom.h>
#include <tccore/aom_prop.h>
#include <tccore/workspaceobject.h>
#include <tc/preferences.h>
#include <epm/signoff.h>
char* callRecursive(tag_t  revTag, char*  kp2_projRevID);
static int report_error(char *file, int line, char *call, int status, logical exit_on_error);
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
#define IFERR_REPORT(X) (report_error( __FILE__, __LINE__, #X, X, FALSE))
#define ERROR_CHECK(X) if (IFERR_REPORT(X)) return (X)

static int report_error(char *file, int line, char *call, int status,
    logical exit_on_error)
{
    if (status != ITK_ok)
    {
        int
            n_errors = 0,
            *severities = NULL,
            *statuses = NULL;
        char
            **messages;

        EMH_ask_errors(&n_errors, &severities, &statuses, &messages);
        if (n_errors > 0)
        {
            TC_write_syslog("\n%s\n", messages[n_errors-1]);
            EMH_clear_errors();
        }
        else
        {
            char *error_message_string;
            EMH_get_error_string (NULLTAG, status, &error_message_string);
            TC_write_syslog("\n%s\n", error_message_string);
        }

        TC_write_syslog("error %d at line %d in %s\n", status, line, file);
        TC_write_syslog("%s\n", call);

        if (exit_on_error)
        {
            TC_write_syslog("Exiting program!\n");
            exit (status);
        }
    }

    return status;
}



extern DLLAPI int Validate_Document_handler(EPM_action_message_t message)
{
	
	tag_t    root_task = NULLTAG;
	int    n_attachments = 0;	
	tag_t    *attachments = NULL;
	int stat =0;
	int pCount =0;
	char targetObjectType[100];
	char* szDocumentSubject = NULL;
	
	TC_write_syslog("calling Validate_Document_handler\n");
	ERROR_CHECK(EPM_ask_root_task( message.task, &root_task));
    
    TC_write_syslog("KP_INFO:  root_task = %d\n", root_task);    

    ERROR_CHECK(EPM_ask_attachments(root_task, EPM_target_attachment,
        &n_attachments, &attachments));
	if (n_attachments == 0) return 1;  // fail
	for(pCount = 0;pCount<n_attachments;pCount++)

		{

	WSOM_ask_object_type(attachments[pCount],targetObjectType);
	TC_write_syslog("KP_INFO:  targetObjectType = %s\n", targetObjectType); 
	if(tc_strcmp(targetObjectType, "KP2_DocumentRevision")==0)
	{
	ERROR_CHECK(AOM_ask_value_string(attachments[pCount], "DocumentSubject",&szDocumentSubject));
    TC_write_syslog("KP_INFO:  szDocumentSubject = %s\n", szDocumentSubject); 
	if(tc_strcmp(szDocumentSubject,"Scope Document")==0)
	{
		stat = 11;
	}
	if(tc_strcmp(szDocumentSubject,"High Level Design")==0)
	{
		stat = 12;
	}
	if(tc_strcmp(szDocumentSubject,"ContentDocument")==0||(tc_strcmp(szDocumentSubject,"Content Document")==0))
	{
		stat = 13;
	}
	
	
	}
		}	
    
	
		if (attachments) MEM_free(attachments);
			if (szDocumentSubject) MEM_free(szDocumentSubject);
	return stat;
}

extern DLLAPI int GTAC_action_handler(EPM_action_message_t message)
{
int reworkGroupCount =0;
int k =0;
int valueCnt=0;
int a=0;
int arg_cnt = 0;
int i=0;
int    count;
int    icount=0;
int    n_attachments = 0;		
int iii = 0;
int x = 0;
int 		ifail = ITK_ok;
int 		ifail1 = ITK_ok;

int 		n_attachs = 0;
int 		participant_count =0;
int 		resp_participant_count =0;
int 		doc_participant_count =0;

int 		subtaskCount=0;
int 		currentTaskCount=0;
int 		task_count=0;
int         number_found = 0;
char* 		job_tag_string = NULL;

char  		type[WSO_name_size_c + 1] = "";
char  		performerType[WSO_name_size_c + 1] = "";

char*        kp2_projRevID = NULL;
char        inputFile[1250]= "C:\\Temp\\KP_EMAIL_NOTIFY_creators";
char		directoryPath[1250]		= {'\0'};
char 		native_file_spec[250]	= {'\0'};
char* 		szDocumentSubject = NULL;
char  		typeOfSelObj[WSO_name_size_c + 1]="";
char  		typeOfTASKSelObj[WSO_name_size_c + 1]="";
char  		typeOfProjrevObj[WSO_name_size_c + 1]="";
char 	    rolename[SA_name_size_c+1]	 = "";
char 	    creator_rolename[SA_name_size_c+1]	 = "";
char        person_name[SA_person_name_size_c+1] = "";
char        quotes[SA_person_name_size_c+1] = "\"";
char        initiator_person_name[SA_person_name_size_c+1] = "";
char 		str_value1[100] = {'\0'};
char 		str_value2[100] = {'\0'};
char 		str_value3[100] = {'\0'};
char 		*charArray[3][10] = {'\0'};
char 		taskType_str_value[100] = {'\0'};
//char		*str_value = NULL;
char 	    *uuid = NULL;
char		* email_address = 0;
char *		sitePrefValue 			= NULL;
char *      szProjectName = NULL;
char *      szItemName = NULL;
char *      szInitiatorName = NULL;
char *      szTaskName = NULL;
char *      szSubTaskName = NULL;
char *      szSubTaskPerformer = NULL;

char *      szSubTaskStatus = NULL;


char *      currentTaskNames = NULL;
char *      currentTaskName = NULL;
char task_name[WSO_name_size_c + 1];

char szStart[1000] 							= "Start ";
char szSpace[10] 							= " ";
char 		item_id[ITEM_id_size_c + 1] = "";
char		*attr_val = NULL;
char		*flag = NULL;
char		*value = NULL;
char		*arg = NULL;

//char* arrayValue[10]={'\0'};
char arrayValue[3][14];

char st[] ="Where there is will, there is a way.";
char *ch;
char 	process_name[WSO_name_size_c+1]	="" ;
int 	num_of_members;
tag_t * member_tags=NULL;
tag_t * cad_member_tags = NULL;



tag_t    *attachments = NULL;
tag_t    group = NULLTAG;
tag_t   job = NULLTAG;
tag_t    resourcePool = NULLTAG;
tag_t    role = NULLTAG;
tag_t    root_task = NULLTAG;
tag_t    signoff = NULLTAG;
tag_t    *unassigned_profiles = NULL;
tag_t    tperformerType = NULLTAG;
tag_t 		revTag = NULLTAG;
tag_t 		item_tag = NULLTAG;
tag_t  		Creator_role_tag= NULLTAG;

tag_t group_tag1 = NULLTAG;
tag_t group_tag2 = NULLTAG;

tag_t		responsible_participant_type = NULLTAG;
tag_t		creation_participant_type = NULLTAG;

tag_t		participant = NULLTAG;
tag_t		CAD_participant = NULLTAG;

tag_t tSubTaskPerformer = NULLTAG;
tag_t 		*szSubTask = NULLTAG;
tag_t 		job_tag = NULLTAG;
tag_t*	    attachs = NULL;

tag_t 		user_tag = NULLTAG;
tag_t 		projRevTag=NULLTAG;
tag_t		participant_type = NULLTAG;
tag_t*      participant_list= NULLTAG;
tag_t*      resp_participant_list= NULLTAG;
tag_t*      doc_participant_list= NULLTAG;
tag_t 		member_tag = NULLTAG;
tag_t 		member_user_tag = NULLTAG;
tag_t 		rework_member_user_tag = NULLTAG;

tag_t 		initiator_member_user_tag = NULLTAG;
tag_t 		currentTaskTag = NULLTAG;
tag_t  		role_tag= NULLTAG;
tag_t		 person_tag = NULLTAG;
tag_t       * task_list=NULLTAG;
logical  	is_default_role=false;
logical  	szSendEmail=false;

FILE *fptr;
TC_preference_search_scope_t original_scope;
fptr = fopen("C:\\Temp\\GTAC_register_action_handler_callbacks.log", "w");
    fprintf(fptr,"KP_INFO: GTAC_action_handler\n");	
		arg_cnt = TC_number_of_arguments( message.arguments );
		
		  fprintf(fptr,"KP_INFO:  *************arg_cnt = %d\n", arg_cnt);
		
			for(i=0;i<arg_cnt;i++)
				
		{
			arg = TC_next_argument( message.arguments );
			ERROR_CHECK(ITK_ask_argument_named_value( (const char*)arg, &flag, &value));
			if(tc_strcmp(flag ,"value1") == 0)
			{
			tc_strcpy(str_value1,value);
			tc_strcpy(charArray[i],str_value1);
			
			valueCnt++;
			}
			if(tc_strcmp(flag ,"value2") == 0)
			{
			tc_strcpy(str_value2,value);
			tc_strcpy(charArray[i],str_value2);
			valueCnt++;
			}
			
			if(tc_strcmp(flag ,"taskType") == 0)
			tc_strcpy(taskType_str_value,value);
		
		  fprintf(fptr,"KP_INFO:  *************str_value1 %s\n", str_value1);
		  fprintf(fptr,"KP_INFO:  *************str_value2 %s\n", str_value2);
		  fprintf(fptr,"KP_INFO:  *************charArray[0] %s\n", charArray[0]);
		  fprintf(fptr,"KP_INFO:  *************charArray[1] %s\n", charArray[1]);
		  fprintf(fptr,"KP_INFO:  *************taskType_str_value %s\n", taskType_str_value);
		   fprintf(fptr,"KP_INFO:  *************valueCnt %d\n", valueCnt);
		

		}
	for(a=0;a<valueCnt;a++)
	{
 ERROR_CHECK(EPM_ask_root_task( message.task, &root_task));
    fprintf(fptr,"KP_INFO:  root_task = %d\n", root_task);
   

    ERROR_CHECK(EPM_ask_attachments(root_task, EPM_target_attachment,
        &n_attachments, &attachments));
    if (n_attachments == 0) return 1;  // fail

    ERROR_CHECK(AOM_ask_group(attachments[0], &group));
    fprintf(fptr,"KP_INFO:  group = %d\n", group);    

    ERROR_CHECK(SA_ask_group_default_role(group, &role) );
    fprintf(fptr,"KP_INFO:  role = %d\n", role);

    ERROR_CHECK(EPM_get_resource_pool(group, role, FALSE, &resourcePool));
    fprintf(fptr,"KP_INFO: resourcePool = %d\n", resourcePool);

    ERROR_CHECK(EPM_ask_job(message.task, &job));
    fprintf(fptr,"KP_INFO:  job = %d\n", job);
	
	ERROR_CHECK(WSOM_ask_name( job, task_name ));
	fprintf(fptr,"KP_INFO:  task_name = %s\n", task_name);
	
	ERROR_CHECK(EPM_ask_sub_tasks	(root_task,&subtaskCount,&szSubTask));
	fprintf(fptr,"KP_INFO:  subtaskCount = %d\n", subtaskCount);
	if(tc_strcmp(taskType_str_value,"REWORK")==0)
	{
	for(x=0;x<subtaskCount;x++)
	{
	ERROR_CHECK(AOM_ask_value_string(szSubTask[x],"fnd0AliasTaskName" ,&szSubTaskName));
	fprintf(fptr,"KP_INFO:  szSubTaskName = %s\n", szSubTaskName);
	
	ERROR_CHECK(AOM_ask_value_string(szSubTask[x],"fnd0Status" ,&szSubTaskStatus));
	fprintf(fptr,"KP_INFO:  szSubTaskStatus = %s\n", szSubTaskStatus);
	
	if((tc_strcmp(szSubTaskName,"Creation")==0)&&(tc_strcmp(szSubTaskStatus,"Approved")==0))
	{
	ERROR_CHECK(AOM_ask_value_tag(szSubTask[x],"fnd0Performer" ,&tSubTaskPerformer));
	//ERROR_CHECK (WSOM_ask_object_type(tSubTaskPerformer, performerType)); 
	
	
	 ERROR_CHECK(TCTYPE_ask_object_type(tSubTaskPerformer, &tperformerType));                
     ERROR_CHECK(TCTYPE_ask_name(tperformerType, performerType));  
     fprintf(fptr,"KP_INFO:  performerType = %s\n", performerType);
	
	if(tSubTaskPerformer!=NULL)
	{
	fprintf(fptr,"KP_INFO:  tSubTaskPerformer = %d\n", tSubTaskPerformer);
	
	
	ERROR_CHECK(AOM_ask_value_string(tSubTaskPerformer,"user_name" ,&szSubTaskPerformer));
	fprintf(fptr,"KP_INFO:  szSubTaskPerformer = %s\n", szSubTaskPerformer);	
	
	EPM_get_participanttype("ProposedResponsibleParty",&responsible_participant_type);
	fprintf(fptr,"KP_INFO:  responsible_participant_type = %d\n", responsible_participant_type);  
	SA_find_groupmember_by_user	(tSubTaskPerformer,&num_of_members,	&member_tags	)	;	

	fprintf(fptr,"KP_INFO:  num_of_members	 = %d\n", num_of_members);  
	for(k=0;k<num_of_members;k++)
	{
		fprintf(fptr,"KP_INFO:  Begin:\n");  
		ERROR_CHECK(SA_ask_groupmember_role(member_tags[k], &Creator_role_tag));
		fprintf(fptr,"KP_INFO: Creator_role_tag= %d\n", Creator_role_tag);	
		
		fprintf(fptr,"KP_INFO:  Begin2:\n");  
		ERROR_CHECK(SA_ask_groupmember_group(member_tags[k],&group_tag1));			
		fprintf(fptr,"KP_INFO: group_tag= %d\n", group_tag1);
		fprintf(fptr,"KP_INFO:  Begin3:\n");  

		ERROR_CHECK(POM_ask_user_default_group	(	tSubTaskPerformer,&group_tag2));			
		fprintf(fptr,"KP_INFO: group_tag= %d\n", group_tag2);
		fprintf(fptr,"KP_INFO:  End:\n");  
		if(group_tag1==group_tag2)
		{
		reworkGroupCount = k;		
		fprintf(fptr,"KP_INFO: calue of k is = %d\n", k);
			ERROR_CHECK(SA_ask_groupmember_user( member_tags[reworkGroupCount], &rework_member_user_tag ));	
			fprintf(fptr,"KP_INFO: rework_member_user_tag= %d\n", rework_member_user_tag);	
			
			
		break;
		}
		
		
	}		
	
				ERROR_CHECK(SA_ask_groupmember_role(member_tags[reworkGroupCount], &role_tag));
				fprintf(fptr,"KP_INFO: role_tag= %d\n", role_tag);	
					
				
				ERROR_CHECK(SA_ask_role_name(role_tag,rolename));
				fprintf(fptr,"KP_INFO: rolename= %s\n", rolename);
				
				
				ERROR_CHECK(EPM_create_participant(member_tags[reworkGroupCount],responsible_participant_type,&participant ));
	
				if(participant!=NULL)
				{
					fprintf(fptr,"KP_INFO: Particpant tag is NULL\n\n");
				}
				else
				{
					fprintf(fptr,"KP_INFO: Particpant tag is NOTTTT NULL\n\n");
				}
	
	}
	}
	
	}
	
				
			
				
				
	
	
	}
	
	revTag = attachments[0];
	ERROR_CHECK(ITEM_ask_item_of_rev(revTag,&item_tag));
	fprintf(fptr,"KP_INFO:  item_tag = %d\n", item_tag);
	
	ERROR_CHECK(ITEM_ask_id(item_tag,item_id));
	fprintf(fptr,"KP_INFO: Processing for item_id= %s\n", item_id);		
	
	ERROR_CHECK(WSOM_ask_object_type(revTag, typeOfSelObj));
	fprintf(fptr,"KP_INFO: Processing for typeOfSelObj= %s\n", typeOfSelObj);		
	
	fprintf(fptr,"KP_INFO: BEGIN CALLING RECURCIVE FUNCTION\n");		
	kp2_projRevID = callRecursive(revTag,kp2_projRevID);
	
	ERROR_CHECK(POM_string_to_tag(kp2_projRevID, &projRevTag)); 
	fprintf(fptr,"KP_INFO: projRevTag = %d\n", projRevTag);
	
	ERROR_CHECK(WSOM_ask_object_type(projRevTag, typeOfProjrevObj));
	fprintf(fptr,"KP_INFO: typeOfProjrevObj = %s\n", typeOfProjrevObj);
	
	ERROR_CHECK(AOM_ask_value_tag(root_task,"fnd0WorkflowInitiator" ,&initiator_member_user_tag));
	fprintf(fptr,"KP_INFO  initiator_member_user_tag = %d\n", initiator_member_user_tag);
	
	ERROR_CHECK(SA_ask_user_person_name	(initiator_member_user_tag,initiator_person_name));
	fprintf(fptr,"KP_INFO: initiator_person_name = %s\n", initiator_person_name);
	
	ERROR_CHECK(AOM_ask_value_string(projRevTag,"object_name", &szProjectName));
	fprintf(fptr,"KP_INFO:  szProjectName = %s\n", szProjectName);
	
	ERROR_CHECK(AOM_ask_value_string(revTag,"object_name" ,&szItemName));
	fprintf(fptr,"KP_INFO:  szItemName = %s\n", szItemName);
	
	EPM_get_participanttype(charArray[a],&participant_type);
	fprintf(fptr,"KP_INFO:  participant_type = %d\n", participant_type);
	
	ITEM_rev_ask_participants(projRevTag,participant_type,&participant_count,&participant_list);
	fprintf(fptr,"KP_INFO:  participant_count = %d\n", participant_count);
	
	//if((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(taskType_str_value,"REWORK")==NULL))						
		if((tc_strcmp(taskType_str_value,"REWORK")==NULL))						
		
				{
					fprintf(fptr,"Entering REWORK HANDLER\n");
					
					//check if the particpant is avaiable else -- remove fisrt,then add, else dirextly add.
					ITEM_rev_ask_participants(revTag,responsible_participant_type,&resp_participant_count,&resp_participant_list);
					
					fprintf(fptr,"KP_INFO:  resp_participant_count = %d\n", resp_participant_count);
						//ITEM_rev_remove_participant(revTag,participant);	
						if(resp_participant_count>0  )
						{
							for(icount=0;icount<resp_participant_count;icount++)
							{
								
							//ifail = ITEM_rev_remove_participant(revTag,resp_participant_list[icount]);	
							//fprintf(fptr,"KP_INFO:  entering for  = %d\n", ifail);
							
							}
							
							//ifail = ITEM_rev_add_participant(revTag,participant);
							//fprintf(fptr,"KP_INFO:  ITEM_rev_add_participant in for ifail  = %d\n", ifail);
						}
						else{
							
							ifail = (ITEM_rev_add_participant(revTag,participant));
							fprintf(fptr,"KP_INFO:  resp_participant_count in else = %d\n", ifail);
						}
						
						
							//ERROR_CHECK(AOM_refresh(revTag, TRUE)); 
							//ERROR_CHECK(AOM_save(revTag));
							//ERROR_CHECK(AOM_refresh(revTag, FALSE));
						
						
				}
	
	
	if(participant_count ==0)
	{
	fprintf(fptr,"WARNING: participant_count is: zero\n");	
	fprintf(fptr,"WARNING: Please add Particpant to = %s\n", typeOfProjrevObj);

	if(tc_strcmp(taskType_str_value,"REWORK")==0)
	{
		
				
				ERROR_CHECK(SA_ask_user_person	(tSubTaskPerformer,&person_tag));	
				fprintf(fptr,"KP_INFO: person_tag= %d\n", person_tag);	
				
				ERROR_CHECK(SA_ask_user_person_name	(tSubTaskPerformer,person_name));
				fprintf(fptr,"KP_INFO: person_name= %s\n", person_name);
				
							
			ERROR_CHECK(SA_ask_person_email_address	(person_tag,&email_address));
				fprintf(fptr,"KP_INFO: email_address= %s\n", email_address);	
				

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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");			

				

	}

	
		
	}
	
	ERROR_CHECK(PREF_ask_search_scope(&original_scope));
	ERROR_CHECK(PREF_set_search_scope(TC_preference_site));
	ERROR_CHECK(PREF_ask_char_value("KP_Master_directory_path", 0, &sitePrefValue));
	ERROR_CHECK(PREF_set_search_scope(original_scope));
	
	
	tc_strcpy(directoryPath,sitePrefValue);
	fprintf(fptr,"KP_INFO: directoryPath= %s\n", directoryPath);		
    fprintf(fptr,"KP_INFO:  Done returning ITK_ok\n");
	
	
	for ( iii = 0; iii < participant_count; iii++)    
			{
				ERROR_CHECK(AOM_ask_value_tag(participant_list[iii],"assignee",&member_tag));
				fprintf(fptr,"KP_INFO: member_tag= %d\n", member_tag);	
				
				ERROR_CHECK(SA_ask_groupmember_user( member_tag, &member_user_tag ));	
				fprintf(fptr,"KP_INFO: member_user_tag= %d\n", member_user_tag);	
				
				ERROR_CHECK(SA_ask_groupmember_role(member_tag, &role_tag));
				fprintf(fptr,"KP_INFO: role_tag= %d\n", role_tag);	
					
				
				ERROR_CHECK(SA_ask_role_name(role_tag,rolename));
				fprintf(fptr,"KP_INFO: rolename= %s\n", rolename);	
				
				ERROR_CHECK(SA_ask_user_person	(member_user_tag,&person_tag));	
				fprintf(fptr,"KP_INFO: person_tag= %d\n", person_tag);	
				
				ERROR_CHECK(SA_ask_user_person_name	(member_user_tag,person_name));
				fprintf(fptr,"KP_INFO: person_name= %s\n", person_name);	
				
					
			ERROR_CHECK(SA_ask_person_email_address	(person_tag,&email_address));
				fprintf(fptr,"KP_INFO: email_address= %s\n", email_address);	
				fprintf(fptr,"KP_INFO check: taskType_str_value= %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO check: typeOfSelObj= %s\n", typeOfSelObj);	
				
				if(sizeof(email_address)<1)
				{
					fprintf(fptr,"KP_INFO: email_address empty = %s for user %s\n", email_address,person_name );	
					tc_strcpy(email_address,"empty email");
				}
				
				/******************************************************************************************/
				/**Begin MULTIMEDIA REVISION
				/******************************************************************************************/
				
				//Begin adding particpant for CREATION for MULTIMEDIA REVISION
				if(((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"Multimedia Developer")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
					||((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"Multimedia Team")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
				||((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"Multimedia Lead")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
				||((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"MM COE")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
				||((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"MM Designer")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
					//Begin adding particpant for REVIEW for MULTIMEDIA REVISION
				if(((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"Multimedia Lead")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL))			
				||((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"POD Lead")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				//Begin adding particpant for APPROVAL for MULTIMEDIA REVISION
				if(((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"QA")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL))			
				||((tc_strcmp(typeOfSelObj,"KP2_MultimediaRevision")==NULL)&&(tc_strcmp(rolename,"SME")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));
				
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				/******************************************************************************************/
				/**Begin Visualisation REVISION
				/******************************************************************************************/
				
				
				
				//Begin adding particpant for CREATION for Visualisation REVISION
				if(((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"CAD Designer")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
					||((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"CAD COE")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
				||((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"CAD Lead")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
				||((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"CAD Team")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
					//Begin adding particpant for REVIEW for Visualisation REVISION
				if(((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"CAD Lead")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL))			
				||((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"QA")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				//Begin adding particpant for APPROVAL for Visualisation REVISION
				if(((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"POD Lead")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL))			
				||((tc_strcmp(typeOfSelObj,"KP2_VisRevision")==NULL)&&(tc_strcmp(rolename,"SME")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));
				
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				/******************************************************************************************/
				/**Begin Voice Over REVISION
				/******************************************************************************************/
				
				
				
				//Begin adding particpant for CREATION for KP2_VoiceOverRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_VoiceOverRevision")==NULL)&&(tc_strcmp(rolename,"Content Lead")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
					||((tc_strcmp(typeOfSelObj,"KP2_VoiceOverRevision")==NULL)&&(tc_strcmp(rolename,"Content Dev")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
				||((tc_strcmp(typeOfSelObj,"KP2_VoiceOverRevision")==NULL)&&(tc_strcmp(rolename,"Content Developer")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
					//Begin adding particpant for REVIEW for KP2_VoiceOverRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_VoiceOverRevision")==NULL)&&(tc_strcmp(rolename,"POD Lead")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				//Begin adding particpant for APPROVAL for KP2_VoiceOverRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_VoiceOverRevision")==NULL)&&(tc_strcmp(rolename,"QA")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));
				
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				
				
				/******************************************************************************************/
				/**Begin Voice Over Script REVISION
				/******************************************************************************************/
				
				
				
				//Begin adding particpant for CREATION for KP2_VoiceOverScRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_VoiceOverScRevision")==NULL)&&(tc_strcmp(rolename,"Content Lead")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
					||((tc_strcmp(typeOfSelObj,"KP2_VoiceOverScRevision")==NULL)&&(tc_strcmp(rolename,"Content Dev")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
				||((tc_strcmp(typeOfSelObj,"KP2_VoiceOverScRevision")==NULL)&&(tc_strcmp(rolename,"Content Developer")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
					//Begin adding particpant for REVIEW for KP2_VoiceOverScRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_VoiceOverScRevision")==NULL)&&(tc_strcmp(rolename,"QA")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				//Begin adding particpant for APPROVAL for KP2_VoiceOverScRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_VoiceOverScRevision")==NULL)&&(tc_strcmp(rolename,"SME")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL)))		
			
				{
				fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				(ITEM_rev_add_participant(revTag,participant_list[iii]));
				
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
			
			
			
				/******************************************************************************************/
				/**Begin CAD REVISION and CAD ASSY REVISON 
				/******************************************************************************************/
				
				
				
				//Begin adding particpant for CREATION for KP2_CADDesignRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_CADDesignRevision")==NULL)&&(tc_strcmp(rolename,"CAD Lead")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
					||((tc_strcmp(typeOfSelObj,"KP2_CADDesignRevision")==NULL)&&(tc_strcmp(rolename,"CAD Designer")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
				||((tc_strcmp(typeOfSelObj,"KP2_CADDesignRevision")==NULL)&&(tc_strcmp(rolename,"CAD Team")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))
					||((tc_strcmp(typeOfSelObj,"KP2_CADDesignRevision")==NULL)&&(tc_strcmp(rolename,"CAD COE")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL)))		
			
				{
					ifail  = EPM_get_participanttype("KP2_Creator",&creation_participant_type);
					fprintf(fptr,"KP_INFO:  creation_participant_type = %d %d\n", creation_participant_type, ifail);  
					//ifail = SA_find_groupmember_by_rolename	(	"CAD Lead","ProductDevelopment","bbiswas",&number_found,&cad_member_tags);
					fprintf(fptr,"KP_INFO:  SA_find_groupmember_by_rolename = %d\n", ifail);  
					ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
						fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  
				//fprintf(fptr,"INFO: adding  member_user_role name %s for CAD Revision  and typeOfSelObj %s\n", rolename, typeOfSelObj);	
					ERROR_CHECK(AOM_ask_value_string(revTag,"object_name" ,&szItemName));
					fprintf(fptr,"KP_INFO:  szItemName is :: = %s\n", szItemName);
					TC_write_syslog("Writing to Syslog\n");
				(ITEM_rev_remove_participant(revTag,CAD_participant));	
				(ITEM_rev_add_participant(revTag,CAD_participant));
		 fprintf(fptr,"KP_INFO:  ITEM_rev_add_participant for cad creation is :: = %d\n", ifail);
			//	ifail = AOM_save(revTag);
			//	fprintf(fptr,"KP_INFO: AOM_save ifail is :: = %d\n", ifail);
			//TC_write_syslog("Writing to Syslog end\n");
			//fprintf(fptr,"KP_INFO:  ifail1 is :: = %d\n", ifail1);
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
					//Begin adding particpant for REVIEW for KP2_CADDesignRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_CADDesignRevision")==NULL)&&(tc_strcmp(rolename,"CAD Lead")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL)))		
			
				{
				//fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				ifail  = EPM_get_participanttype("KP2_CADLead",&creation_participant_type);
				
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 
				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				//Begin adding particpant for APPROVAL for KP2_CADDesignRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_CADDesignRevision")==NULL)&&(tc_strcmp(rolename,"POD Owner")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL))
					||((tc_strcmp(typeOfSelObj,"KP2_CADDesignRevision")==NULL)&&(tc_strcmp(rolename,"SME")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL)))		
			
				{
				//fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				
				//(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				if(tc_strcmp(rolename,"POD Owner")==NULL)
				{	
				ifail  = EPM_get_participanttype("KP2_PODOwner",&creation_participant_type);
				}
				if(tc_strcmp(rolename,"SME")==NULL)
				{	
				ifail  = EPM_get_participanttype("KP2_SME",&creation_participant_type);
				}
				
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 
				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				
				
				//(ITEM_rev_add_participant(revTag,participant_list[iii]));
				
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				
				/******************************************************************************************/
				/**Begin story board revision 
				/******************************************************************************************/
				
				
				
				//Begin adding particpant for CREATION for KP2_StoryBoardRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_StoryBoardRevision")==NULL)&&(tc_strcmp(rolename,"Story Writer")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL)))		
			
				{
					ifail  = EPM_get_participanttype("KP2_Creator",&creation_participant_type);
					fprintf(fptr,"KP_INFO:  creation_participant_type = %d %d\n", creation_participant_type, ifail);  
					//ifail = SA_find_groupmember_by_rolename	(	"CAD Lead","ProductDevelopment","bbiswas",&number_found,&cad_member_tags);
					fprintf(fptr,"KP_INFO:  SA_find_groupmember_by_rolename = %d\n", ifail);  
					ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
						fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  
				//fprintf(fptr,"INFO: adding  member_user_role name %s for CAD Revision  and typeOfSelObj %s\n", rolename, typeOfSelObj);	
					ERROR_CHECK(AOM_ask_value_string(revTag,"object_name" ,&szItemName));
					fprintf(fptr,"KP_INFO:  szItemName is :: = %s\n", szItemName);
					TC_write_syslog("Writing to Syslog\n");
				(ITEM_rev_remove_participant(revTag,CAD_participant));	
				(ITEM_rev_add_participant(revTag,CAD_participant));
		 fprintf(fptr,"KP_INFO:  ITEM_rev_add_participant for cad creation is :: = %d\n", ifail);
			//	ifail = AOM_save(revTag);
			//	fprintf(fptr,"KP_INFO: AOM_save ifail is :: = %d\n", ifail);
			//TC_write_syslog("Writing to Syslog end\n");
			//fprintf(fptr,"KP_INFO:  ifail1 is :: = %d\n", ifail1);
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
					//Begin adding particpant for REVIEW for KP2_CADDesignRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_StoryBoardRevision")==NULL)&&(tc_strcmp(rolename,"POD Lead")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL)))		
			
				{
				//fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				ifail  = EPM_get_participanttype("KP2_PODLead",&creation_participant_type);
				
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 
				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				//Begin adding particpant for APPROVAL for KP2_StoryBoardRevision 
				if(((tc_strcmp(typeOfSelObj,"KP2_StoryBoardRevision")==NULL)&&(tc_strcmp(rolename,"POD Owner")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL))
					||((tc_strcmp(typeOfSelObj,"KP2_StoryBoardRevision")==NULL)&&(tc_strcmp(rolename,"SME")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL)))		
			
				{
				//fprintf(fptr,"INFO: adding  member_user_role name = %s and typeOfSelObj %s\n", rolename, typeOfSelObj);	
				
				//(ITEM_rev_remove_participant(revTag,participant_list[iii]));	
				if(tc_strcmp(rolename,"POD Owner")==NULL)
				{	
				ifail  = EPM_get_participanttype("KP2_PODOwner",&creation_participant_type);
				}
				if(tc_strcmp(rolename,"SME")==NULL)
				{	
				ifail  = EPM_get_participanttype("KP2_SME",&creation_participant_type);
				}
				
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 
				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				
				
				//(ITEM_rev_add_participant(revTag,participant_list[iii]));
				
				//ERROR_CHECK(EPM_ask_assigned_tasks( member_user_tag, EPM_inbox_query, &task_count, &task_list ));	
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");				
				
				}
				
				
				fprintf(fptr,"KP_INFO: Begin adding particpant for CREATION for KP2_DocumentRevision \n");	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s \n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: rolename %s \n", rolename);
				fprintf(fptr,"KP_INFO: taskType_str_value %s \n", taskType_str_value);
				
				if(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)
				{
				fprintf(fptr,"KP_INFO:  checking document subject\n");				
				ERROR_CHECK(AOM_ask_value_string(revTag, "DocumentSubject",&szDocumentSubject));
				fprintf(fptr,"KP_INFO:  szDocumentSubject = %s\n", szDocumentSubject);			
				
					
				//Begin adding particpant for CREATION for KP2_DocumentRevision 
				if((tc_strcmp(szDocumentSubject,"Scope Document")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"SME")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))					
				{
				ifail  = EPM_get_participanttype("KP2_Creator",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				szSendEmail = true;
				}
				
				
				//Begin adding particpant for APPROVAL for KP2_DocumentRevision 
				if((tc_strcmp(szDocumentSubject,"Scope Document")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"POD Owner")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL))					
				{
				ifail  = EPM_get_participanttype("KP2_Reviewer",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				szSendEmail = true;
				}
				
				
				//Begin adding particpant for APPROVAL for KP2_DocumentRevision 
				if((tc_strcmp(szDocumentSubject,"Scope Document")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"POD Mgmt")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL))					
				{
				ifail  = EPM_get_participanttype("KP2_Approver",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				szSendEmail = true;
				}
				
				if((tc_strcmp(szDocumentSubject,"High Level Design")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"POD Lead")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))					
				{
				ifail  = EPM_get_participanttype("KP2_Creator",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				szSendEmail = true;
				}
				
				
				//Begin adding particpant for APPROVAL for KP2_DocumentRevision 
				if((tc_strcmp(szDocumentSubject,"High Level Design")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"POD Owner")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL)
					||(tc_strcmp(szDocumentSubject,"High Level Design")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"SME")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL))					
				{
					if(tc_strcmp(rolename,"SME")==NULL)
				{	
				ifail  = EPM_get_participanttype("KP2_Reviewer",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				szSendEmail = true;
				}
				
				if(tc_strcmp(rolename,"POD Owner")==NULL)
				{					
				ifail  = EPM_get_participanttype("KP2_Reviewer",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				}
				szSendEmail = true;
				}
				
				
				//Begin adding particpant for APPROVAL for KP2_DocumentRevision 
				if((tc_strcmp(szDocumentSubject,"High Level Design")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"POD Mgmt")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL))					
				{
				ifail  = EPM_get_participanttype("KP2_Approver",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				szSendEmail = true;
				}
				
				
				if((tc_strcmp(szDocumentSubject,"ContentDocument")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"Content Lead")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL)
					||(tc_strcmp(szDocumentSubject,"ContentDocument")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"Content Dev")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL)
				||(tc_strcmp(szDocumentSubject,"ContentDocument")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"Content Developer")==NULL)&&(tc_strcmp(taskType_str_value,"Creation")==NULL))					
				{
				ifail  = EPM_get_participanttype("KP2_Creator",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				szSendEmail = true;
				}
				
				
				//Begin adding particpant for APPROVAL for KP2_DocumentRevision 
				if((tc_strcmp(szDocumentSubject,"ContentDocument")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"SME")==NULL)&&(tc_strcmp(taskType_str_value,"REVIEW")==NULL))					
				{
				ifail  = EPM_get_participanttype("KP2_Reviewer",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				szSendEmail = true;
				}
				
				
				//Begin adding particpant for APPROVAL for KP2_DocumentRevision 
				if((tc_strcmp(szDocumentSubject,"ContentDocument")==NULL)&&(tc_strcmp(typeOfSelObj,"KP2_DocumentRevision")==NULL)&&(tc_strcmp(rolename,"POD Mgmt")==NULL)&&(tc_strcmp(taskType_str_value,"APPROVAL")==NULL))					
				{
				ifail  = EPM_get_participanttype("KP2_Approver",&creation_participant_type);	
				fprintf(fptr,"KP_INFO:  EPM_get_participanttype = %d\n", ifail); 				
				ifail = EPM_create_participant(member_tag,creation_participant_type,&CAD_participant );
				fprintf(fptr,"KP_INFO:  EPM_create_participant = %d\n", ifail);  				
				ERROR_CHECK(ITEM_rev_ask_participants(revTag,creation_participant_type,&doc_participant_count,&doc_participant_list)); 
				if(doc_participant_count>0)
				{
				(ITEM_rev_remove_participant(revTag,CAD_participant));
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				else
				{
				(ITEM_rev_add_participant(revTag,CAD_participant));		
				}
				szSendEmail = true;
				}
				
				if(szSendEmail)
				{
				fprintf(fptr,"KP_INFO: BEGIN PRINTING VARIABLES for document\n");	
				fprintf(fptr,"KP_INFO: item_id %s\n", item_id);	
				fprintf(fptr,"KP_INFO: email_address %s\n", email_address);	
				fprintf(fptr,"KP_INFO: szProjectName %s\n", szProjectName);	
				fprintf(fptr,"KP_INFO: szItemName %s\n", szItemName);	
				fprintf(fptr,"KP_INFO: typeOfSelObj %s\n", typeOfSelObj);	
				fprintf(fptr,"KP_INFO: person_name %s\n", person_name);	
				fprintf(fptr,"KP_INFO: taskType_str_value %s\n", taskType_str_value);	
				fprintf(fptr,"KP_INFO: initiator_person_name %s\n", initiator_person_name);					
		
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
				tc_strcat(szStart, taskType_str_value);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, szSpace);
				tc_strcat(szStart, "\"");
				tc_strcat(szStart, initiator_person_name);
				tc_strcat(szStart, "\"");				
				
				fprintf(fptr,"KP_INFO: BEGIN PRINTING START VARIABLE\n");	
				fprintf(fptr,"KP_INFO: szStart %s\n", szStart);
				
				//sprintf(native_file_spec, "%s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName, szInitiatorName);	
				//sprintf(native_file_spec, "%s %s %s %s %s %s %s %s","C:\\Apps\\plm\\tcroot\\local\\ProjectLifeCycle\\KP_BATCH\\KP_Master.bat ",item_id,email_address,szProjectName,szItemName,typeOfSelObj,person_name,szTaskName,initiator_person_name);	
				system (szStart);
				
				tc_strcpy(szStart, "Start ");	
				szSendEmail				= false;
				
				}
				}
			}
			
			
			if (email_address) MEM_free(email_address);
				if (attachments) MEM_free(attachments);
	if (participant_list) MEM_free(participant_list);
	if (resp_participant_list) MEM_free(resp_participant_list);
	if(flag){ MEM_free(flag);flag = NULL;}
			if(value){ MEM_free(value); value = NULL;}
		}
		fclose(fptr);
    return ITK_ok;
}


char* callRecursive(tag_t revTag, char*  kp2_projRevID)
{
	int ii = 0;
	 int n_references = 0, *levels = NULL;
   tag_t *reference_tags = NULL;
 // char * kp2_projRevID=NULL;
   //char * type=NULL;
   char  type[WSO_name_size_c + 1]="";
    char **relations = NULL;
	char        inputFile[1250]= "C:\\Temp\\KP";
	//ofstream myfile;
	//fprintf(fptr,"INFO: Calling callRecursive \n");
	//tc_strcat(inputFile,"a.log");
		
	//myfile.open (inputFile);

	 (WSOM_where_referenced(revTag, -1 , &n_references, &levels, &reference_tags,&relations));
	// myfile  << "n_references is" << n_references << "\n";
	 
		 for (ii = 0; ii < n_references; ii++)
    {
       (WSOM_ask_object_type(reference_tags[ii], type)); 
		//	myfile  << "type is " << type<< "\n";
			
			if(tc_strcmp(type,"KP2_ProjectRevision")==NULL)
		{
			
				ITK__convert_tag_to_uid(reference_tags[ii], &kp2_projRevID);
				//myfile  << "kp2_projRevID is " << kp2_projRevID<< "\n";
			break;
		}
			
		}
	
		MEM_free(levels);
		MEM_free(reference_tags);
		MEM_free(relations);
	
	   return kp2_projRevID;
	
}
extern DLLAPI int GTAC_register_action_handler(int *decision, va_list args)
{
    *decision = ALL_CUSTOMIZATIONS;

    TC_write_syslog("GTAC_register_action_handler\n");

    ERROR_CHECK(EPM_register_action_handler("GTAC-action-handler",
        "Placement: Assign Task Action of select-signoff-team task",
        GTAC_action_handler));	
	ERROR_CHECK(EPM_register_action_handler("Validate-Document-handler",
        "Placement: Assign Task Action of select-signoff-team task",
        Validate_Document_handler));			

    return ITK_ok;
}

extern DLLAPI int GTAC_register_callbacks ()
{
   TC_write_syslog("GTAC_register_callbacks\n");

    ERROR_CHECK(CUSTOM_register_exit ( "GTAC", "USER_gs_shell_init_module",
       (CUSTOM_EXIT_ftn_t) GTAC_register_action_handler ));

    return ITK_ok;
}