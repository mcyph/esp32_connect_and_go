#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "SD_MMC.h"
#include "SPIFFS.h"
#include "ArduinoJson.h";

// NOTE ME: There isn't a sqlite3_shutdown as I don't think we'll ever call it. Needs to be called once only
sqlite3_initialize();

class SQLite3 {
    private:
        String path;
        sqlite3 *sqliteDB;
        sqlite3_stmt *sqliteStatement;
        int curStmtIndex = 1;

    public:
        SQLite3(String path) {
            this->path = path;
            int rc = sqlite3_open(path.c_str(), &sqliteDB);

            if (rc) {
                Serial.printf("Can't open database: %s\n", sqlite3_errmsg(sqliteDB));
                //return rc;
            } else {
                Serial.printf("Opened database successfully\n");
            }
            //return rc;
        }

        String getErrorMessage() {
            return sqlite3_errmsg(sqliteDB);
        }

        int execute(String sql,  int (* callback)(void *, int, char **, char **) = 0, void * data = 0, char ** zErrMsg = 0) {
            // Equivalent to prepareV2 -> (skip bind*/clearBindings/reset) -> step -> finalize
            return sqlite3_exec(sqliteDB, sql.c_str(), callback, data, zErrMsg);
        };

        int prepareV2(String sqlStatement, const char **pzTail /* OUT: Pointer to unused portion of zSql */) {
            return sqlite3_prepare_v2(sqliteDB, sqlStatement.to_cstr(), sqlStatement.length(), sqliteStatement, pzTail);
        }

        int bindJsonObjectInString(vector<string> bindKeys, String s) {
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, s);
            return bindJSONObjectInDocument(bindKeys, doc);
        };

        int bindJsonObjectInDocument(vector<string> bindKeys, DynamicJsonDocument doc) {
            JsonObject root = doc.as<JsonObject>();
            for (String key: bindKeys) {
                JSONValue value = root[key];

                if (value.is<bool>()) {
                    bindBool(value.as<bool>());
                } else if (value.is<float>() || val.is<double>) {
                    bindDouble(value.as<double>());
                } else if (value.is<signed char>() || value.is<unsigned char>() || 
                           value.is<signed int>() || value.is<unsigned int>() || 
                           value.is<signed short>() || value.is<unsigned short>()) {
                    bindInt(value.as<int>());  // UNSIGNED WARNING!
                } else if (value.is<signed long>() || value.is<unsigned long>() || 
                           value.is<unsigned long long>() || value.is<signed long long>()) {
                    bindInt64(value.as<long long>());
                } else if (value.is<JsonArray>() || value.is<JsonObject> || value.is<JsonVariant> || 
                           value.is<JsonArrayConst> || value.is<JsonObjectConst> || value.is<JsonVariantConst>) {
                    bindJsonDocumentAsString(value);
                } else {
                  // TODO!
                }
            }
        };

        int bindJsonDocumentAsString(DynamicJsonDocument doc) {
            int jsonSize = measureJson(doc);
            char * jsonOutput = char[jsonSize];
            serializeJson(doc, jsonOutput, jsonSize);
            int r = bindText64(jsonOutput, jsonSize);
            delete jsonOutput;
            return r;
        };

        int bindInt(int x) {
            return sqlite3_bind_int(sqliteStatement, curStmtIndex++, x);
        };
        int bindLong(long x) {
            return sqlite3_bind_int64(sqliteStatement, curStmtIndex++, x);
        };
        int bindInt64(sqlite_int64 iValue) {
            return sqlite3_bind_int64(sqliteStatement, curStmtIndex++, iValue);
        };
        int bindText(String s) {
            return sqlite3_bind_text(sqliteStatement, curStmtIndex++, s.c_str(), s.length(), SQLITE_STATIC);
        };
        int bindText64(const char *zData, sqlite3_uint64 nData, void (*xDel)(void*), unsigned char enc) {
            return sqlite3_bind_text64(sqliteStatement, curStmtIndex++, zData, nData, xDel, enc);
        };
        int bindBlob(String s, void (*xDel)(void*)) {
            return sqlite3_bind_blob(sqliteStatement, curStmtIndex++, s.c_str(), s.length(), xDel);
        };
        int bindBlob64(const void *zData, sqlite3_uint64 nData, void (*xDel)(void*)) {
            return sqlite3_bind_blob64(sqliteStatement, curStmtIndex++, zData, nData, xDel);
        };
        int bindDouble(double rValue) {
            return sqlite3_bind_double(sqliteStatement, curStmtIndex++, rValue);
        };
        int bindNull() {
            return sqlite3_bind_null(sqliteStatement, curStmtIndex++);
        };

        int step() {
            return sqlite3_step(sqliteStatement);
        }

        int clearBindings() {
            return sqlite3_clear_bindings(sqliteStatement);
        };

        int finalize() {
            return sqlite3_finalize(sqliteStatement);
        };

        void free(void* freeMe) {
            sqlite3_free(freeMe);
        };

        int initialize() {
            return sqlite3_initialize();
        };

        int reset() {
            return sqlite3_reset(sqliteStatement);
        };

        int close() {
            return sqlite3_close(sqliteDB);
        };
};

void displayFreeHeap() {
    Serial.printf("\nHeap size: %d\n", ESP.getHeapSize());
    Serial.printf("Free Heap: %d\n", heap_caps_get_free_size(MALLOC_CAP_8BIT));
    Serial.printf("Min Free Heap: %d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
    Serial.printf("Max Alloc Heap: %d\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
}

char *random_strings[] = {
    "Hello world", 
    "Have a nice day", 
    "Testing memory problems", 
    "This should work", 
    "ESP32 has 512k RAM", 
    "ESP8266 has only 36k user RAM", 
    "A stitch in time saves nine", 
    "Needle in a haystack", 
    "Too many strings", 
    "I am done"
};

void setup() {
    randomSeed(analogRead(0));

    Serial.begin(115200);
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount file Serial");
        return;
    }

    SPI.begin();
    SD_MMC.begin();

    displayFreeHeap();
}

void loop() {
    int rc;
    const char *tail;
    SQLite3 sqliteInst = SQLite3("/sdcard/bulk_ins.db");
    displayFreeHeap();

    rc = sqliteInst.execute("CREATE TABLE IF NOT EXISTS test (c1 INTEGER, c2, c3, c4, c5 INTEGER, c6 INTEGER, c7, c8, c9 DATETIME, c10 DATETIME, c11 INTEGER )");
    if (rc != SQLITE_OK) {
        sqliteInst.close();
        return;
    }
    displayFreeHeap();

    rc = sqliteInst.prepareV2("INSERT INTO test VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", &tail);
    if (rc != SQLITE_OK) {
        Serial.printf("ERROR preparing sql: %s\n", sqliteInst.getErrorMessage());
        sqliteInst.close();
        return;
    }

    const int INSERT_RECORDS_NUM = 10;
    while (INSERT_RECORDS_NUM--) {
        sqliteInst.bindInt(random(65535));
        sqliteInst.bindText(random_strings[random(10)]);
        sqliteInst.bindText(random_strings[random(10)]);
        sqliteInst.bindText(random_strings[random(10)]);
        sqliteInst.bindInt(random(65535));
        sqliteInst.bindInt(random(65535));
        sqliteInst.bindText(random_strings[random(10)]);
        sqliteInst.bindText(random_strings[random(10)]);
        sqliteInst.bindInt(random(100000000L));
        sqliteInst.bindInt(random(100000000L));
        sqliteInst.bindInt(random(65535));

        if (sqliteInst.step() != SQLITE_DONE) {
            Serial.printf("ERROR executing stmt: %s\n", sqliteInst.getErrorMessage());
            sqliteInst.close();
            return;
        }
        
        sqliteInst.clearBindings();
        rc = sqliteInst.reset();
        if (rc != SQLITE_OK) {
            sqliteInst.close();
            return;
        }
        displayFreeHeap();
    }
    sqliteInst.finalize();
    Serial.write("\n");

    rc = sqliteInst.execute("Select count(*) from test");
    if (rc != SQLITE_OK) {
        sqliteInst.close();
        return;
    }

    sqliteInst.close();
    displayFreeHeap();
}
