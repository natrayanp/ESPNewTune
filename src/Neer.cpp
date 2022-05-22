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



/* Parse text to JSON, then render back to text, and print! */
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

/*https://github.com/kbranigan/cJSON/blob/master/test.c*/
/* Read a file, parse, render back, etc. */
void NeerClass::dofile(char *filename){
    //FILE *f = NULL;
    //uint8_t *ptr = (uint8_t *)&dm1;
    //size_t numBytes = sizeof(dm1);
    
    long len = 0;
    char *data = NULL;
    Serial.println(filename);
    /* open in read binary mode */
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

            /*
            String line = f.readStringUntil('\n');
            char * writable = new char[line.length() + 1];
            std::copy(line.begin(), line.end(), writable);
            writable[line.length()] = '\0'; // don't forget the terminating 0
            Serial.println(writable);
            doit(writable);
            */
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
            /*
            for (std::string::size_type i = 0; i < stringd.length(); i++) {
                f.write(stringd[i]);        
            } 
            */      
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



NeerClass Neer;