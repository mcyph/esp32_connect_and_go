#include <FS.h>;
#include <SPI.h>;
#include <vector>;
#include <stdio.h>;
#include <string.h>;
#include "SD_MMC.h";
#include "SPIFFS.h";
#include <stdlib.h>;
#include <sqlite3.h>;
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
            return sqlite3_prepare_v2(sqliteDB, sqlStatement.c_str(), sqlStatement.length(), &sqliteStatement, pzTail);
        }

        int bindJsonObjectInString(vector<String> bindKeys, String s) {
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, s);
            return bindJSONObjectInDocument(bindKeys, doc);
        };

        int bindJsonObjectInDocument(vector<String> bindKeys, DynamicJsonDocument doc) {
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

