#ifndef __TEST_FUNC_XLSX_H__
#define __TEST_FUNC_XLSX_H__

#include <string>
#include <memory>
#include <vector>
#include <map>

#include <OpenXLSX.hpp>

using namespace std;
using namespace OpenXLSX;


class Xlsx
{
public:
    Xlsx();
    ~Xlsx();
    int loadBoyTable(string table_name);
    int loadGirlTable(string table_name);
    int updateTable(string table_name);
private:
    bool checMonthkData(XLCellValue &year, XLCellValue &month,int64_t &result);
    int  updateBoyValue(int month, XLCellValue &value, int &out);
    int  updateGirlValue(int month, XLCellValue &value, int &out);
    map<int, vector<float>> m_boy_table;
    map<int, vector<float>> m_girl_table;
};












#endif // __TEST_FUNC_XLSX_H__
