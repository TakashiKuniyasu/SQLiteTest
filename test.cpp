#include<iostream>
#include<sqlite3.h>
#include<vector>
class SQLiteTest{
public:
    void createTable(sqlite3*);
    void insertRecord(sqlite3*);
    struct StudyRecord{
        int id;
        std::string content;
        std::string date;
        int minutes;
    };
    bool selectRecords(sqlite3*,std::vector<StudyRecord>&);

private:
    bool insertRecordDynamicContent(sqlite3*,sqlite3_stmt*,const std::string&);
    bool insertRecordDynamicDate(sqlite3*,sqlite3_stmt*,const std::string&);
    bool insertRecordDynamicMinutes(sqlite3*,sqlite3_stmt*,int);
    bool stepSql(sqlite3* ,sqlite3_stmt*);
    bool bindRecord(sqlite3*, sqlite3_stmt*, const std::string&, const std::string&, int);
};
int main(){
    SQLiteTest sqlite_test;
    std::vector<SQLiteTest::StudyRecord> records;
    // データベースオープン
    sqlite3* db = nullptr;
    if(sqlite3_open("sample.db",&db) != SQLITE_OK){
        std::cout << "データベースオープン失敗\n" << std::endl;
        return 1;
    }

    // データベース作成
    sqlite_test.createTable(db);

    // 行挿入
    sqlite_test.insertRecord(db);

    //  一覧表示
    if(!sqlite_test.selectRecords(db, records)){
        std::cout << "SELECT失敗" << std::endl;
    }else if(records.empty()){
        std::cout << "レコードが空です" << std::endl;
    }else{
        for(const SQLiteTest::StudyRecord& record : records){
            std::cout << "ID: " << record.id << "\n"
                        << "学習内容: " << record.content << "\n"
                        << "学習日: " << record.date << "\n"
                        << "学習時間: " << record.minutes << "分\n" << std::endl;
        }
    }
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
    sqlite3_stmt* insert_stmt = nullptr;
    const char* insert_sql = 
        "INSERT INTO sample_records"
        " (content, date, minutes)"
        " VALUES(? ,? ,?);";

    int result = sqlite3_prepare_v2(db,
                       insert_sql,
                       -1,
                       &insert_stmt,
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
    // プレースホルダーの設定１回目
    if(!bindRecord(db, insert_stmt, "C++", "2026/10/19", 45)){
        sqlite3_finalize(insert_stmt);
        return;
    }
    // SQL文実行
    if(!stepSql(db, insert_stmt)){
        std::cout << "INNSERT文1回目実行失敗" << std::endl;
         sqlite3_finalize(insert_stmt);
        return;
    }

    result = sqlite3_reset(insert_stmt);
    if(result != SQLITE_OK){
        std::cout << "INSERTステートメントのリセットに失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
        sqlite3_finalize(insert_stmt);
         return;        
    }
    result = sqlite3_clear_bindings(insert_stmt);
    if(result != SQLITE_OK){
        std::cout << "bindの値のクリアに失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
        sqlite3_finalize(insert_stmt);
         return;        
    }
    // プレースホルダーの設定２回目
    if(!bindRecord(db, insert_stmt, "Linux", "2026/10/20", 75)){
        sqlite3_finalize(insert_stmt);
        return;
    }
    // SQL実行
    if(!stepSql(db, insert_stmt)){
        std::cout << "INNSERT文2回目実行失敗" << std::endl;
    }
    // SQL文終了準備
    sqlite3_finalize(insert_stmt);
    return;
}
bool SQLiteTest::selectRecords(sqlite3* db,std::vector<SQLiteTest::StudyRecord>& records){
    std::vector<SQLiteTest::StudyRecord> records_local;
    sqlite3_stmt* select_stmt = nullptr;
    const char* select_sql = 
        "SELECT id, content, date, minutes"
        " FROM sample_records;";

    int result = sqlite3_prepare_v2(db,
                       select_sql,
                       -1,
                       &select_stmt,
                       nullptr
                    );
    if(result == SQLITE_OK){
        std::cout << "select prepare成功\n" << std::endl;
    }else{
        std::cout << "select prepare失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
        return false;
    }

    while((result = sqlite3_step(select_stmt)) == SQLITE_ROW){
        SQLiteTest::StudyRecord record = {};
        record.id = sqlite3_column_int(select_stmt, 0);
        record.content = reinterpret_cast<const char*>(sqlite3_column_text(select_stmt, 1));
        record.date = reinterpret_cast<const char*>(sqlite3_column_text(select_stmt, 2));
        record.minutes = sqlite3_column_int(select_stmt, 3);
        records_local.push_back(record);
    }
    if(result != SQLITE_DONE){
        std::cout << "SELECTの処理に失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
         sqlite3_finalize(select_stmt);
         return false;
    }else{
        records = records_local;
    }
    sqlite3_finalize(select_stmt);
    return true;
}
bool SQLiteTest::insertRecordDynamicContent(sqlite3* db, sqlite3_stmt* insert_stmt, const std::string &content){
    int result = sqlite3_bind_text(insert_stmt, 1, content.c_str(), -1, SQLITE_TRANSIENT);
    if(result != SQLITE_OK){
        std::cout << "学習内容の挿入に失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
         return false;
    }
    return true;
}
bool SQLiteTest::insertRecordDynamicDate(sqlite3* db, sqlite3_stmt* insert_stmt, const std::string &date){
    int result = sqlite3_bind_text(insert_stmt, 2, date.c_str(), -1, SQLITE_TRANSIENT);
    if(result != SQLITE_OK){
        std::cout << "日付の挿入に失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
         return false;
    }
    return true;
}
bool SQLiteTest::insertRecordDynamicMinutes(sqlite3* db, sqlite3_stmt* insert_stmt, int minutes){
    int result = sqlite3_bind_int(insert_stmt, 3, minutes);
    if(result != SQLITE_OK){
        std::cout << "学習時間の挿入に失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
         return false;
    }
    return true;
}
bool SQLiteTest::stepSql(sqlite3* db, sqlite3_stmt* stmt){
    int result = sqlite3_step(stmt);
    if(result == SQLITE_DONE){
        std::cout << "SQL実行成功\n" << std::endl;
    }else{
        std::cout << "SQL実行失敗\n"
         << sqlite3_errmsg(db)
         << std::endl;
         return false;
    }
    return true;
}
bool SQLiteTest::bindRecord
        (sqlite3* db, 
         sqlite3_stmt* stmt, 
         const std::string &content, 
         const std::string &date, 
         int minutes){
    if(!insertRecordDynamicContent(db, stmt, content)){
        return false;
    }
    if(!insertRecordDynamicDate(db, stmt, date)){
        return false;
    }
    if(!insertRecordDynamicMinutes(db, stmt, minutes)){
        return false;
    }
    return true;
}
