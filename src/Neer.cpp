#include "Neer.h"

    
NatStorage NS("/neerdb.txt");

/*
NeerClass::NeerClass(){   

    loadconfig();
    //json status;
    status = {
        {"3phase_yes", false},
        {"motor_running", false},
        {"valeeast_open",false},
        {"valvenorth_open",false},
        {"valvethotti_open",false},
        {"valvesouth_open",false}
    };

    status["3phase_yes"] = check3phase();
}
*/

bool NeerClass::check3phase() {
    return true;
}


void NeerClass::loadconfig(){

    _checksconfig = NS.readTable("checksconfig");
    _planconfig = NS.readTable("planconfig");
    _scheduleconfig = NS.readTable("scheduleconfig");
    _pinconfig = NS.readTable("pinconfig");
    
    //char *tm = cJSON_PrintUnformatted(pinconfig);
    Serial.println("Iam back");
    Serial.println("FREE MEMORY: ");
    Serial.println(ESP.getFreeHeap());
    //Serial.println(tm);
    /*
    pinconfig = {
            {
                {"name","Motor"},
                {
                    "on",{
                            {"pintype","out"},
                            {"onpin",1},
                            {"onvalue",1},
                            {"onsignaltype","trigger"},
                            {"ontriggerdelay",500},
                         }
                },
                {
                    "off", {
                            {"pintype","out"},                        
                            {"offpin",2},
                            {"offvalue",1},
                            {"onsignaltype","trigger"},
                            {"ontriggerdelay",500},
                            }
                }
            },
            //Thotti Value config
            {
                {"name","Valvethotti"},
                {
                    "on",{
                            {"pintype","out"},
                            {"onpin",3},
                            {"onvalue",1},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                         }
                },
                {
                    "off", {
                            {"pintype","out"},                        
                            {"offpin",3},
                            {"offvalue",0},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                            }
                }
            },
            //East Value config
            {
                {"name","Valveeast"},
                {
                    "on",{
                            {"pintype","out"},
                            {"onpin",4},
                            {"onvalue",1},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                         }
                },
                {
                    "off", {
                            {"pintype","out"},                        
                            {"offpin",4},
                            {"offvalue",0},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                            }
                }
            },
            //North Value config
            {
                {"name","Valvenorth"},
                {
                    "on",{
                            {"pintype","out"},
                            {"onpin",5},
                            {"onvalue",1},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                         }
                },
                {
                    "off", {
                            {"pintype","out"},                        
                            {"offpin",5},
                            {"offvalue",0},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                            }
                }
            },
            //South Value config
            {
                {"name","Valvesouth"},
                {
                    "on",{
                            {"pintype","out"},
                            {"onpin",6},
                            {"onvalue",1},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                         }
                },
                {
                    "off", {
                            {"pintype","out"},                        
                            {"offpin",6},
                            {"offvalue",0},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                            }
                }
            },            
            //R config
            {
                {"name","rline"},
                {
                    "on",{
                            {"pintype","in"},
                            {"onpin",1},
                            {"onvalue",1},
                            {"onsignaltype","read"},
                            {"ontriggerdelay",0},
                         }
                },
                {
                    "off", {
                            {"pintype","in"},                        
                            {"offpin",1},
                            {"offvalue",0},
                            {"onsignaltype","read"},
                            {"ontriggerdelay",0},
                            }
                }
            },  
            //Thotti Value config
            {
                {"name","yline"},
                {
                    "on",{
                            {"pintype","in"},
                            {"onpin",2},
                            {"onvalue",1},
                            {"onsignaltype","read"},
                            {"ontriggerdelay",0},
                         }
                },
                {
                    "off", {
                            {"pintype","in"},                        
                            {"offpin",2},
                            {"offvalue",0},
                            {"onsignaltype","read"},
                            {"ontriggerdelay",0},
                            }
                }
            },              
            //East Value config
            {
                {"name","bline"},
                {
                    "on",{
                            {"pintype","in"},
                            {"onpin",3},
                            {"onvalue",1},
                            {"onsignaltype","read"},
                            {"ontriggerdelay",0},
                         }
                },
                {
                    "off", {
                            {"pintype","in"},                        
                            {"offpin",3},
                            {"offvalue",0},
                            {"onsignaltype","read"},
                            {"ontriggerdelay",0},
                            }
                }
            },
            //SWITCH ON light
            {
                {"name","mrligth"},
                {
                    "on",{
                            {"pintype","out"},
                            {"onpin",7},
                            {"onvalue",1},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                         }
                },
                {
                    "off", {
                            {"pintype","out"},                        
                            {"offpin",7},
                            {"offvalue",0},
                            {"onsignaltype","hold"},
                            {"ontriggerdelay",0},
                            }
                }
            }
        };
         
        */
}


void NeerClass::begin(AsyncWebServer* server){
    _server = server;        
    loadconfig();
    auto myGET = _server->on("/returnconf", HTTP_GET, [&](AsyncWebServerRequest *request){
                Serial.println("Starting returnconf");                
                //char *sctmp = NS.readTable("scheduleconfig");
                cJSON *scconfig = NS.readTable("scheduleconfig");
                Serial.println("Iam back");
                char *sctmp = cJSON_PrintUnformatted(scconfig); 
                //cJSON_Delete(scconfig);
                //Serial.println("Pointer to scconfig: %p\r\n", (void *)scconfig);
                //Serial.print("Pointer to scconfig: ");
                //Serial.println(scconfig);
                Serial.print("FREE MEMORY before scconfig del: ");
                Serial.println(ESP.getFreeHeap());
                Serial.println(sctmp);               
                request->send(200, "application/json",sctmp);
                cJSON_Delete(scconfig);
                Serial.print("FREE MEMORY after scconfig del: ");
                Serial.println(ESP.getFreeHeap());
                
    });
}


cJSON* NeerClass::getPlanitem(char* planname){

    cJSON * planitem = NULL;

    for(i = 0; i < cJSON_GetArraySize(_planconfig) ; i++){
        cJSON* subitemplconf = cJSON_GetArrayItem(_planconfig, i);    
        cJSON* nameplconf = cJSON_GetObjectItem(subitemplconf, "name");
        if(nameplconf-> valuestring == planname ){
            //TODO checks
            planitem = subitemplconf;            
            //stepsitem = cJSON_GetObjectItem(subitemplconf, "steps");            
            break;
        }
    }

    return planitem;
}

int NeerClass::flipPinValue(int pval){
    pval?return 0:return 1;
}

int NeerClass::flipOnOffState(char* pval){
    pval=="on"?return "off":return "on";
}


void NeerClass::excuteCommand(char* command, cJSON*stepitem){

    if (command == "D"){
        cJSON *stepfield =cJSON_GetObjectItem(stepitem, "field");
        vTaskDelay(stepfield->valueint);
    }else if (command == "O") {        
        cJSON* pintype = cJSON_GetObjectItem(stepitem, "pintype");
        cJSON* pinnum = cJSON_GetObjectItem(stepitem, "pinnum");
        cJSON* pinvalue = cJSON_GetObjectItem(stepitem, "pinvalue");
        //Call 595 library
        sr.set(pinnum->valueint, pinvalue->valueint);

    } else if (command == "OR") {
        cJSON* pintype = cJSON_GetObjectItem(stepitem, "pintype");
        cJSON* pinnum = cJSON_GetObjectItem(stepitem, "pinnum");
        cJSON* pinvalue = cJSON_GetObjectItem(stepitem, "pinvalue");
        //Call 595 library with flipped value of pin
        sr.set(pinnum->valueint, pinvalue->valueint);
    }
}

void NeerClass::executeStep(taskitem *taskit){
    cJSON *stepname =cJSON_GetObjectItem(taskit->stepitem, "name");   
    cJSON *stepfield =cJSON_GetObjectItem(taskit->stepitem, "field"); 
    
    //TODO : make this as configured value to be run only for last step in schedule
    if (stepname->valuestring = "Motor" && !(isfianlstep)) {
        return;
    }

    if (stepname->valuestring == "delay"){
        excuteCommand("D", taskit->stepitem);
    }else {
        for(i = 0; i < cJSON_GetArraySize(_pinconfig) ; i++){
            cJSON* subitempinconf = cJSON_GetArrayItem(_pinconfig, i);    
            cJSON* pinconfname = cJSON_GetObjectItem(subitempinconf, "name");

            if(pinconfname->valuestring == stepname->valuestring){
                //TODO checks
                if (stepfield->valuestring == "on" || stepfield->valuestring =="off"){                    

                    char* sf;
                    if(taskit->runtype == "R"){
                        sf = flipOnOffState(stepfield->valuestring);
                    } else {
                        sf = stepfield->valuestring;
                    }
                    cJSON* fieldval = cJSON_GetObjectItem(subitempinconf, sf);
                    excuteCommand("O", fieldval);
                    cJSON* signaltype = cJSON_GetObjectItem(fieldval, "signaltype");
                    if(signaltype->valuestring == "trigger"){
                        cJSON *resolution = cJSON_CreateObject();
                        cJSON* triggerdelay = cJSON_GetObjectItem(fieldval, "triggerdelay");
                        cJSON* pinvalue1 = cJSON_GetObjectItem(fieldval, "pinvalue");
                        cJSON *delaytime = cJSON_CreateNumber(triggerdelay->valueint);
                        cJSON_AddItemToObject(resolution, "field", delaytime1);
                        excuteCommand("D", resolution);
                        excuteCommand("OR", fieldval);
                    }
                }
            }
        }
    }
}


bool NeerClass::chkforcestop() {
    return forcestop;
}

void NeerClass::runInstructionFromPlan(taskitem *taskit){
    cJSON * stepsitem = NULL;
    //cJSON * planitem = NULL;

    /*
    for(i = 0; i < cJSON_GetArraySize(_planconfig) ; i++){
        cJSON* subitemplconf = cJSON_GetArrayItem(_planconfig, i);    
        cJSON* nameplconf = cJSON_GetObjectItem(subitemplconf, "name");
        if(nameplconf-> valuestring == plan ){
            //TODO checks
            planitem = subitemplconf;
            stepsitem = cJSON_GetObjectItem(subitemplconf, "steps");            
            break;
        }
    }
    */

    stepsitem = cJSON_GetObjectItem(taskit->planitem, "steps"); 

    if(stepsitem != NULL){
        int az = cJSON_GetArraySize(stepsitem);
        if(runtype == "F"){
            for(i = 0; i < az ; i++){
                taskit->stepsitem = cJSON_GetArrayItem(stepsitem, i);
                //executeStep(cJSON_GetArrayItem(stepsitem, i),runtype);
                executeStep(taskit);
            }
        } else if(runtype == "R"){
            for(i = az-1; i >= 0 ; i--){
                taskit->stepsitem = cJSON_GetArrayItem(stepsitem, i);
                //executeStep(cJSON_GetArrayItem(stepsitem, i),runtype);
                executeStep(taskit);
            }
        }
    }
}


void NeerClass::executePlan(taskitem *taskit){
    cJSON * planitem = NULL;
    cJSON * fPlanitem = NULL;

    if(taskit->previousplan == ""){

        planitem = getPlanitem(taskit->currentplan);
        if(cJSON planitem != NULL){
            taskit->planitem = planitem;
            taskit->runtype = "F";
            runInstructionFromPlan(taskit);
            fPlanitem = planitem;
        }

    } else {
        if(taskit->previousplanrunseq == "A"){

            planitem = getPlanitem(taskit->currentplan);
            if(cJSON planitem != NULL){
                taskit->planitem = planitem;
                taskit->runtype = "F";
                runInstructionFromPlan(taskit);
                //runInstructionFromPlan(planitem,"F");
                fPlanitem = planitem;
            }
            
            planitem = getPlanitem(taskit->previousplan);
            if(cJSON planitem != NULL){
                taskit->planitem = planitem;
                taskit->runtype = "R";
                runInstructionFromPlan(taskit);
                //runInstructionFromPlan(planitem,"R");
            }

        } else if(taskit->previousplanrunseq == "B"){

            planitem = getPlanitem(taskit->previousplan);
            if(cJSON planitem != NULL){
                taskit->planitem = planitem;
                taskit->runtype = "R";
                runInstructionFromPlan(taskit);
                //runInstructionFromPlan(planitem,"R");
            }
            
            planitem = getPlanitem(taskit->currentplan);
            if(cJSON planitem != NULL){
                taskit->planitem = planitem;
                taskit->runtype = "F";
                runInstructionFromPlan(taskit);
                //runInstructionFromPlan(planitem,"F");
                fPlanitem = planitem;
            }        
        }        
    }

    //implement milli and check for any force stop
    unsigned long starttime=millis(); 
    cJSON* planruntime = cJSON_GetObjectItem(fPlanitem, "runtime");

    for(;(starttime-millis())<planruntime->valueint;){
        vTaskDelay(10000);
        if(chkforcestop())            
            break;        
    }

    if(chkforcestop()){
        //forcestart = false;  This is to be done at schedule
        //for(i = cJSON_GetArraySize(stepsitem); i >=0 ; i--){
        taskit->planitem = fPlanitem;
        taskit->runtype = "R";
        runInstructionFromPlan(taskit);
        //runInstructionFromPlan(fPlanitem,"R");
        //}
    }
}

void NeerClass::getInstructionFromSchedule(scheduleitem *schitc){
    cJSON * subitem = NULL;
    taskitem *taskitcpy;
    taskitcpy = malloc(sizeof(struct taskitem));
    cJSON *item = cJSON_GetObjectItem(_scheduleconfig,"items");
    
    if(schitc->isplan){
        taskitcpy->previousplan = "";
        taskitcpy->currentplan = schitc->name;
        taskitcpy->previousplanrunseq = "A";   //As of now all are defaulted to after
        taskitcpy->islastplan = true;
        executePlan(taskitcpy);
        return;
    }  
    
    for (i = 0; i < cJSON_GetArraySize(_scheduleconfig) ; i++){     
        cJSON* subitemtest = cJSON_GetArrayItem(_scheduleconfig, i);  
        cJSON* name = cJSON_GetObjectItem(subitemtest, "name");
        if(name-> valuestring == schitc->name ){
            subitem = subitemtest;
            break;
        }
    }

    if(subitem != NULL){
        cJSON *plitem = cJSON_GetObjectItem(subitem,"planids");

        int jarsiz = cJSON_GetArraySize(plitem);

        for(i = 0; i < jarsiz ; i++){

            if(chkforcestop()){
                forcestart = false;
                break;
            }

            cJSON * subitempl = cJSON_GetArrayItem(plitem, i);
            if(i==0){
                taskitcpy->previousplan = "";
            } else {
                taskitcpy->previousplan = taskitcpy->currentplan;
            }                
            taskitcpy->currentplan = subitempl->valuestring;
            taskitcpy->previousplanrunseq = "A";   //As of now all are defaulted to after
            taskitcpy->islastplan = false;
            if(i == jarsiz-1) {
                taskitcpy->islastplan = true;
            }

            taskitcpy->isfirstplan = false;
            if(i == 0) {
                taskitcpy->isfirstplan = true;
            }
            executePlan(taskitcpy);
        }
    }
}

void NeerClass::executeTask( void * parameter ){
    threadit * data = (threadit *) parameter;
    scheduleitem *schit;
    if(data->itemtype == "S"){        
        schit->isplan = false;
    }else if(data->itemtype == "P"){
        schit->isplan = true;
    }
    schit->name = data->itemname;
    getInstructionFromSchedule(schit);
    Serial.println("Ending a task");
    vTaskDelete( *(xHandle[data->arraylocation]) );
    xHandle[data->arraylocation] == NULL;
}

bool NeerClass::beginTask(threadit* parameter){
    for (i=0;i<ALLOWED_TASKS;i++){
        if(xHandle[i] == NULL){
            parameter->arraylocation = i;
            TaskHandle_t xHandlecpy = NULL;
            xTaskCreate(
                executeTask,      /* Task function. */
                "TaskOne",        /* String with name of task. */
                1000,            /* Stack size in bytes. */
                parameter,        /* Parameter passed as input of the task */
                1,                /* Priority of the task. */
                &xHandlecpy);     /* Task handle. */
            xHandle[i]= &xHandlecpy;
            return true;
        }
    }
    return false;
    
}



void taskOne( void * parameter )
{
 
    for( int i = 0;i<10;i++ ){
 
        Serial.println("Hello from task 1");
        delay(1000);
        Serial.print("FREE MEMORY task1: ");
        Serial.println(ESP.getFreeHeap());
    }
 
    Serial.println("Ending task 1");
    vTaskDelete( NULL );
 
}
 
void taskTwo( void * parameter)
{
 
    for( int i = 0;i<10;i++ ){
 
        Serial.println("Hello from task 2");
        delay(1000);
        Serial.print("FREE MEMORY task2: ");
        Serial.println(ESP.getFreeHeap());
    }
    Serial.println("Ending task 2");
    vTaskDelete( NULL );
 
}

bool NeerClass::begintask(){

    for (i=0;i<ALLOWED_TASKS;i++){
        if(xHandle[i]== NULL){
            TaskHandle_t xHandlecpy = NULL;
            xTaskCreate(
                taskOne,          /* Task function. */
                "TaskOne",        /* String with name of task. */
                10000,            /* Stack size in bytes. */
                NULL,             /* Parameter passed as input of the task */
                1,                /* Priority of the task. */
                &xHandlecpy);     /* Task handle. */
            xHandle[i]= &xHandlecpy;
            return true
        } else {
            return false;
        }
    }
    
    

    xTaskCreate(
                    taskOne,          /* Task function. */
                    "TaskOne",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

    xTaskCreate(
                    taskTwo,          /* Task function. */
                    "TaskTwo",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */

}



/* Parse text to JSON, then render back to text, and print! */
/*
void NeerClass::doit(char *text){
    char *out = NULL;
    cJSON *json = NULL;

    json = cJSON_Parse(text);
    if (!json)
    {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    }
    else
    {
        //out = cJSON_Print(json);
        out = cJSON_PrintUnformatted(json);
        cJSON_Delete(json);
        printf("%s\n", out);
        //free(out);
    }

    
    

	File fp = LittleFS.open("/mytest.txt","w");
	if(!fp)
	{
		fprintf(stderr,"open file failed\n");
		exit(-1);
	}
	fp.print(out);
    fp.print('\n');
    free(out);
	if(fp)
		fp.close();

}
*/

/*https://github.com/kbranigan/cJSON/blob/master/test.c*/
/* Read a file, parse, render back, etc. */
/*
void NeerClass::dofile(char *filename){
    //FILE *f = NULL;
    //uint8_t *ptr = (uint8_t *)&dm1;
    //size_t numBytes = sizeof(dm1);
    
    long len = 0;
    char *data = NULL;
    Serial.println(filename);
    // open in read binary mode 
    File f = LittleFS.open(filename,"r");

    char termseq1[] = "pinvalue";
    char termseq2[] = "\n";
    Serial.println("before start");
    if(0){
        bool y;
        
        while(f.available()) {
            f.seek(0,SeekSet);    
            Serial.println("Original position");
            Serial.println(f.position());
            //String line = f.readStringUntil('\n');
            //String line = f.readStringUntil(termseq1);
            y = f.findUntil(termseq1,termseq2);
            

            Serial.println("Found the string?");
            Serial.println(y);
            Serial.println("Found the string in position");
            Serial.println(f.position());

            if(y){
                //f.close();
                break;
            }


        }
        Serial.println("after start");
        if(y){
            //File f = LittleFS.open(filename,"a+");
            Serial.println("writing to file");
            String stringd = "drna41";
            Serial.println(f.position());
            f.seek(f.position());  
            f.print(stringd);
            //f.write(stringd,sizeof(stringd));
     
        }
        f.flush();
        f.close();
    } else {
            String line = f.readStringUntil('\n');
            char * writable = new char[line.length() + 1];
            std::copy(line.begin(), line.end(), writable);
            writable[line.length()] = '\0'; // don't forget the terminating 0
            Serial.println(writable);
            doit(writable);
        Serial.println("no file");
    }

}


cJSON* NeerClass::getInstructionFromStep(cJSON* stepitem){
    cJSON *instructionsetone = cJSON_CreateArray();
    
    cJSON *stepname =cJSON_GetObjectItem(stepitem, "name");
    cJSON *stepfield =cJSON_GetObjectItem(stepitem, "field");

    if (stepname->valuestring == "delay"){
        cJSON *resolution = cJSON_CreateObject();
        cJSON_AddItemToArray(instructionsetone, resolution);
        cJSON *delaytime = cJSON_CreateNumber(stepfield->valueint);
        cJSON *command = cJSON_CreateString("D");
        cJSON_AddItemToObject(resolution, "command", command);
        cJSON_AddItemToObject(resolution, "delaytime", delaytime);

    }else {
        for(i = 0; i < cJSON_GetArraySize(_pinconfig) ; i++){
            cJSON* subitempinconf = cJSON_GetArrayItem(_pinconfig, i);    
            cJSON* pinconfname = cJSON_GetObjectItem(subitempinconf, "name");

            if(pinconfname->valuestring == stepname->valuestring){
                //TODO checks
                switch(stepfield->valuestring){
                    case("on"):{
                        cJSON* onfield = cJSON_GetObjectItem(subitempinconf, "on");
                        cJSON* pintype = cJSON_GetObjectItem(onfield, "pintype");
                        cJSON* pinnum = cJSON_GetObjectItem(onfield, "pinnum");
                        cJSON* pinvalue = cJSON_GetObjectItem(onfield, "pinvalue");
                        cJSON* signaltype = cJSON_GetObjectItem(onfield, "signaltype");
                        

                        cJSON *resolution = cJSON_CreateObject();
                        cJSON_AddItemToArray(instructionsetone, resolution);
                        cJSON *command = cJSON_CreateString("POS");
                        cJSON *pinu = cJSON_CreateInteger(pinnum->valueint);
                        cJSON *pinval = cJSON_CreateInteger(pinvalue->valueint);
                        cJSON_AddItemToObject(resolution, "command", command);
                        cJSON_AddItemToObject(resolution, "pinnum", pinu);
                        cJSON_AddItemToObject(resolution, "pinvalue", pinval);

                        
                        if(signaltype->valuestring == "trigger"){
                            cJSON *resolution1 = cJSON_CreateObject();
                            cJSON_AddItemToArray(instructionsetone, resolution1);
                            
                            cJSON *delaytime1 = cJSON_CreateNumber(triggerdelay->valueint);
                            cJSON *command1 = cJSON_CreateString("D");
                            cJSON_AddItemToObject(resolution1, "command", command1);
                            cJSON_AddItemToObject(resolution1, "delaytime", delaytime1);



                            cJSON *resolution2 = cJSON_CreateObject();
                            cJSON_AddItemToArray(instructionsetone, resolution2);
                            cJSON *command2 = cJSON_CreateString("POS");
                            cJSON *pinu2 = cJSON_CreateInteger(pinnum->valueint);
                            cJSON *pinval2 = cJSON_CreateInteger(flipPinValue(pinvalue->valueint));
                            cJSON_AddItemToObject(resolution2, "command", command2);
                            cJSON_AddItemToObject(resolution2, "pinnum", pinu2);
                            cJSON_AddItemToObject(resolution2, "pinvalue", pinval2);                       
                        }

                        break;
                    }
                }           
                break;
            }
        }
    }
    return instructionset;
}
*/

NeerClass Neer;