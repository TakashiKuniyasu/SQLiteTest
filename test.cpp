#include<iostream>
#include<sqlite3.h>

class SQLiteTest{
public:
    void createTable(sqlite3*);
    void insertRecord(sqlite3*);
};
int main(){
    SQLiteTest sqlite_test;
    // データベースオープン
    sqlite3* db = nullptr;
    if(sqlite3_open("sample.db",&db) != SQLITE_OK){
        std::cout << "データベースオープン失敗\n" << std::endl;
    }

    // データベース作成
    sqlite_test.createTable(db);

    // 1行挿入
    sqlite_test.insertRecord(db);

    // データベースクローズ
    sqlite3_close(db);
    return 0;
}
void SQLiteTest::createTable(sqlite3* db){
    // SQL文準備
    sqlite3_stmt* create_stsm = nullptr;
    const char* create_sql = 
        "CREATE TABLE IF NOT EXISTS sample_records"
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
        std::cout << "create prepare成功\n" << std::endl;
    }else{
        std::cout << "create prepare失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
        return;
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

    return;
}
void SQLiteTest::insertRecord(sqlite3* db){
    sqlite3_stmt* insert_stsm = nullptr;
    const char* insert_sql = 
        "INSERT INTO sample_records"
        " VALUES('1','C++','2026/10/22',60);";

    int result = sqlite3_prepare_v2(db,
                       insert_sql,
                       -1,
                       &insert_stsm,
                       nullptr
                    );
    if(result == SQLITE_OK){
        std::cout << "insert prepare成功\n" << std::endl;
    }else{
        std::cout << "insert prepare失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
        return;
    }
    // SQL文実行
    result = sqlite3_step(insert_stsm);
    if(result == SQLITE_DONE){
        std::cout << "データ記録成功\n" << std::endl;
    }else{
        std::cout << "データ記録失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
    }
    // SQL文終了準備
    sqlite3_finalize(insert_stsm);
}