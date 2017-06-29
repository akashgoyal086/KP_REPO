@echo off
set TC_ROOT=C:\Apps\plm\tcroot

set _DEVROOT=%~dp0

set item_id=%1
set email_address=%2
set szProjectName=%3
set szItemName=%4
set typeOfSelObj=%5
set person_name=%6
set szTaskName=%7
set initiator_person_name=%8


set szProjectName=%szProjectName: =_%
set szItemName=%szItemName: =_%
set typeOfSelObj=%typeOfSelObj: =_%
set person_name=%person_name: =_%
set szTaskName=%szTaskName: =_%
set initiator_person_name=%initiator_person_name: =_%



	

echo %item_id%,%email_address%,%szProjectName%,%szItemName%,%typeOfSelObj%,%person_name%,%szTaskName%,%initiator_person_name% >> C:\Apps\plm\tcroot\local\ProjectLifeCycle\KP_log\KP_Project_LS.log
rem %TC_ROOT%\bin\CMail.exe -secureport -host:akash.goyal@knowledgepodium.com:****@smtp.gmail.com:465 -from:akash.goyal@knowledgepodium.com -to:%toAddress% -subject:"Teamcenter Pending Action for ItemID: %itemID%" -body:"User %personName%: %toAddress% \n please complete the task for %itemID% pending at user teamcenter AWC inbox"  -a64:C:\Temp\KPLogs\logs\*
rem %TC_ROOT%\bin\CMail.exe -secureport -host:akash.goyal@knowledgepodium.com:****@smtp.gmail.com:465 -from:akash.goyal@knowledgepodium.com -to:%email_address% -subject:"Teamcenter Pending Action for ItemID" -body:"person_name,email_address,szProjectName,item_id,szItemName,typeOfSelObj,szTaskName,szInitiatorName \n %person_name%,%email_address%,%szProjectName%,%item_id%,%szItemName%,%typeOfSelObj%,%szTaskName%,%szInitiatorName%"
rem Working %TC_ROOT%\bin\CMail.exe -secureport -host:akash.goyal@knowledgepodium.com:****@smtp.gmail.com:465 -from:akash.goyal@knowledgepodium.com -to:%email_address% -subject:"TC TASK NOTIFICATION for PROJECT NAME %szProjectName% AND ITEM ID  %item_id%" -body:"Please complete the task assigned to your teamcenter inbox \nASSIGNED TO = %person_name% \nPROJECT NAME = %szProjectName%\nITEM ID = %item_id%\nITEM NAME = %szItemName%\nTYPE OF OBJECT  %typeOfSelObj%\nTASK NAME = %szTaskName%\nINITIATED BY = %initiator_person_name% \nURL = http://tcprodweb:8383/awc/#myTasks"
rem %TC_ROOT%\bin\CMail.exe -secureport -host:akash.goyal@knowledgepodium.com:****@smtp.gmail.com:465 -from:akash.goyal@knowledgepodium.com -to:%email_address% -subject:"TC TASK NOTIFICATION for PROJECT NAME %szProjectName% AND ITEM ID  %item_id%" -body:"EMAIL TO %email_address%\nPROJECT NAME  %szProjectName%\nITEM ID %item_id%\nszITEM NAME  %szItemName%\nTYPE OF OBJECT  %typeOfSelObj% \nINITIATED BY %initiator_person_name%"
rem rd /s /q C:\Temp\KPLogs
exit 0




