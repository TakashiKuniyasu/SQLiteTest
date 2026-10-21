#include<iostream>
#include<sqlite3.h>

int main(){
    // データベースオープン
    sqlite3* db = nullptr;
    if(sqlite3_open("sample.db",&db) != SQLITE_OK){
        std::cout << "データベースオープン失敗\n" << std::endl;
    }
    // SQL文準備
    sqlite3_stmt* create_stsm = nullptr;
    const char* create_sql = 
        "CREATE TABLE sample_records"
        " (id INTEGER PRIMARY KEY,"
        "content TEXT NOT NULL,"
        "date TEXT NOT NULL,"
        "minutes INTEGER NOT NULL);";

    int result = sqlite3_prepare_v2(db,
                       create_sql,
                       -1,
                       &create_stsm,
                       nullptr
                    );
    if(result == SQLITE_OK){
        std::cout << "prepare成功\n" << std::endl;
    }else{
        std::cout << "prepare失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
        sqlite3_close(db);
        return 0;
    }
    // SQL文実行
    result = sqlite3_step(create_stsm);
    if(result == SQLITE_DONE){
        std::cout << "テーブル作成成功\n" << std::endl;
    }else{
        std::cout << "テーブル作成失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
    }
    // SQL文終了準備
    sqlite3_finalize(create_stsm);
    // データベースクローズ
    sqlite3_close(db);
    return 0;
}