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

    int INSERT_RECORDS_NUM = 10;
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
