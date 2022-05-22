/****************************************************************************************************************************
NatStorage.h
for ESP32 and ESP8266 boards
Version Modified By   Date      Comments
------- -----------  ---------- -----------
0.0.1   Natrayan     10/05/2022 Initial coding
*****************************************************************************************************************************/

#ifndef NatStorage_H
#  define NatStorage_H

#  ifndef NatStorage_DEBUG
#    define NatStorage_DEBUG false
#  endif // #ifndef ConfigStorage_DEBUG

#  if defined(ARDUINO) && (ARDUINO >= 100)
#    include <Arduino.h>
#  else  // defined(ARDUINO) && (ARDUINO >= 100)
#    include <WProgram.h>
#  endif // #defined(ARDUINO) && (ARDUINO >= 100)

#  ifndef NAT_STORAGE_VERSION
#    define NAT_STORAGE_VERSION             "NatStorage v0.0.1"
#    define NAT_STORAGE_VERSION_MAJOR       0
#    define NAT_STORAGE_VERSION_MINOR       0
#    define NAT_STORAGE_VERSION_PATCH       1
#    define NAT_STORAGE_VERSION_INT         0000001
#  endif // #ifndef CONFIG_STORAGE_VERSION

#  define NatStorage_VERSION NAT_STORAGE_VERSION

#include <LittleFS.h>
#          define FileFS	LittleFS

class NatStorage{

private:
  const char *fileName;

public:

/*
  struct filterval
  {
      String fieldname;
      String condition;
      String matchvalue;
  };

  struct updateform
  {
      validateform matchcriteria;
      String fieldname;      
      String newvalue;
  };
*/
  NatStorage(const char *dbname){
      fileName = dbname;
  };


  

  cJSON* readTable(String tablename /*, filterval *validation*/){
    Serial.print("FREE MEMORY read start: ");
    Serial.println(ESP.getFreeHeap());
    cJSON *resultset = NULL;
    cJSON *json = NULL;
    bool add = false;
    bool y;
    File f = FileFS.open(fileName, "r");
    
    if (!f)
    {
      Serial.println("DB open failed");
    }
    else
    {
      Serial.println("DB Open OK");
      String tablestart = "NATTBL_"+String(tablename)+"_START";
      String tableend = "NATTBL_"+String(tablename)+"_END";
      char recordSeparator = '\n';
      
      // Find the table first
      f.seek(0,SeekSet);      
      y = f.find(tablestart.c_str());
      

      if(y){
        f.seek(f.position(),SeekSet);
        Serial.println("Table found");
        resultset =  cJSON_CreateArray();
        while(f.available()) {            
          String line = f.readStringUntil(recordSeparator);
          if(line.equals(tableend)){            
            Serial.println("Reached End of Table");
            add = false;
            
            break;    
          } else {
            Serial.println("Read line");
            Serial.println(line);
            json = cJSON_Parse(line.c_str());           

            /*
            if(validation->fieldname != "*"){
              if(searchFilter(cJSON *json, filterval *validation)){
                add = true;
              } 
            } else {
              add = true;
            }
            */
            add = true;
            if(add){
              cJSON_AddItemToArray(resultset,json);
              //cJSON_Delete(json);
            }
            add = false;              
          }
        }
        f.close();
      } else {
          Serial.println("Table not found or failed opening");          
      }         
    }
    //char *out = cJSON_PrintUnformatted(resultset);
    //out += '\0';
    //Serial.println(out);
    //cJSON_Delete(resultset);
    //Serial.println("Pointer to resultset: ", resultset);
    Serial.println("deleted");
    Serial.print("FREE MEMORY read end: ");
    Serial.println(ESP.getFreeHeap());
    return resultset;
  };

  void updateRecords(char *tablename, cJSON *tablejsonarray){
    char *out = NULL;
    String tmpfile = "/tmp.txt";
    bool y;
    bool add = false;
    File f = FileFS.open(fileName, "r");
    File t = FileFS.open(tmpfile, "w");
    
    if (!f && !t){
      Serial.println("DB open failed");
    }
    else{
      Serial.println("DB Open OK");
      String tablestart = "NATTBL_"+String(tablename)+"_START";
      String tableend = "NATTBL_"+String(tablename)+"_END";
      char recordSeparator = '\n';
      
      // Find the table first
      f.seek(0,SeekSet);      
      //y = f.find(tablestart);
      
            
      while(f.available()) { 
        String line = f.readStringUntil(recordSeparator);
        if(line.equals(tablestart)){            
            Serial.println("Update Table reached skiping it");
            y = f.find(tableend.c_str());
            if (y){
              Serial.println("Table end found.  Moving past the table, to skip it...");
              f.seek(f.position(),SeekSet);                            
            } else {
              Serial.println("Table end not found.");
            }              
        } else {
            t.println(line);
        }
      }
      cJSON * set; 
      //Add the table that we are updating with new data      
      cJSON_ArrayForEach(set,tablejsonarray){
        char *out = cJSON_PrintUnformatted(set);
        t.println(out);
      }

      f.close();
      t.close();

      if(LittleFS.remove(fileName))
        LittleFS.rename(tmpfile,fileName);      
  } 

  /*
  // Logic to return only selected rows
  bool searchFilter(cJSON *json, filterval *validate){
      String fieldname;
      String condition;
      String matchvalue;

    if(validate->fieldname)
    return true;
  }
*/

}

};

#endif // NatStorage_H