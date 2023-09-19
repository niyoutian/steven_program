#include "func_xlsx.h"
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <map>

using namespace std;
using namespace OpenXLSX;

Xlsx::Xlsx()
{

}

Xlsx::~Xlsx()
{

}

int Xlsx::loadBoyTable(string table_name)
{
    XLDocument doc;
    // doc.open("3 sft_bmifa_boys_z_5_19years.xlsx");
    doc.open(table_name);
    auto wks = doc.workbook().worksheet("Sheet1");
    cout << "wks.rows:" << wks.rowCount() << endl;
    cout << "wks.columns:" << wks.columnCount() << endl;

    XLCellValue cell;
    for (uint32_t i = 1; i <= wks.rowCount(); i++) {
        cell = wks.cell(i, 2).value();
        // cout << "Cell [" << i <<",2" <<"]: (" << cell.typeAsString() << ") " << endl;
        if (cell.type()==XLValueType::Integer) {
            if (cell.get<int64_t>() < 61 || cell.get<int64_t>() > 228) {
                cout << "error Cell [" << i <<",2" <<"]: (" << cell.get<int64_t>()<< ") " << endl;
                continue;
            }
            // cout << "Cell [" << i <<",2" <<"]: (" << cell.get<int64_t>()<< ") " << endl;
            vector <float> sd_list;
            for (uint16_t j = 3; j <= wks.columnCount(); j++) {
                XLCellValue cell2 = wks.cell(i, j).value();
                // cout << "Cell [" << i <<",2" <<"]: (" << cell2.typeAsString() << ") " << endl;
                if (cell2.type()==XLValueType::Integer) {
                    sd_list.push_back((float)cell2.get<int64_t>());
                } else if (cell2.type()==XLValueType::Float) {
                    sd_list.push_back(cell2.get<float>());
                } else {
                    cout << "error Cell [" << i <<"," << j  <<"]: (" << cell2.typeAsString() << ") " << endl;
                }
                // float sd_value =cell2.get<float>();
                // sd_list.push_back(sd_value);
            }
            m_boy_table.insert(pair<int, vector<float>>((int)cell.get<int64_t>(), sd_list));
        }
    }

    doc.close();
    return 0;
}

int Xlsx::loadGirlTable(string table_name)
{
    XLDocument doc;
    doc.open(table_name);
    auto wks = doc.workbook().worksheet("Sheet1");
    cout << "wks.rows:" << wks.rowCount() << endl;
    cout << "wks.columns:" << wks.columnCount() << endl;

    XLCellValue cell;
    for (uint32_t i = 1; i <= wks.rowCount(); i++) {
        cell = wks.cell(i, 2).value();
        // cout << "Cell [" << i <<",2" <<"]: (" << cell.typeAsString() << ") " << endl;
        if (cell.type()==XLValueType::Integer) {
            if (cell.get<int64_t>() < 61 || cell.get<int64_t>() > 228) {
                cout << "error Cell [" << i <<",2" <<"]: (" << cell.get<int64_t>()<< ") " << endl;
                continue;
            }
            // cout << "Cell [" << i <<",2" <<"]: (" << cell.get<int64_t>()<< ") " << endl;
            vector <float> sd_list;
            for (uint16_t j = 3; j <= wks.columnCount(); j++) {
                XLCellValue cell2 = wks.cell(i, j).value();
                // cout << "Cell [" << i <<",2" <<"]: (" << cell2.typeAsString() << ") " << endl;
                if (cell2.type()==XLValueType::Integer) {
                    sd_list.push_back((float)cell2.get<int64_t>());
                } else if (cell2.type()==XLValueType::Float) {
                    sd_list.push_back(cell2.get<float>());
                } else {
                    cout << "error Cell [" << i <<"," << j  <<"]: (" << cell2.typeAsString() << ") " << endl;
                }
                // float sd_value =cell2.get<float>();
                // sd_list.push_back(sd_value);
            }
            m_girl_table.insert(pair<int, vector<float>>((int)cell.get<int64_t>(), sd_list));
        }
    }

    doc.close();
    return 0;
}

int Xlsx::updateTable(string table_name)
{
    XLDocument doc;
    doc.open("儿童Z平分-230911-待查3.xlsx");
    auto wks = doc.workbook().worksheet("1-17岁全数据-整理");

    cout << "wks.rows:" << wks.rowCount() << endl;
    cout << "wks.columns:" << wks.columnCount() << endl;

    XLCellValue cell_2;
    XLCellValue cell_3;
    XLCellValue cell_4;
    XLCellValue cell_5;
    int cell_out =3;
    for (uint32_t i = 1; i <= wks.rowCount(); i++) {
    //for (uint32_t i = 17973; i <= wks.rowCount(); i++) {
        cell_2 = wks.cell(i, 2).value();
        cell_3 = wks.cell(i, 3).value();
        cell_4 = wks.cell(i, 4).value();
        cell_5 = wks.cell(i, 5).value();
        int64_t month = 0;
        bool ret = checMonthkData(cell_2, cell_3, month);
        if (ret == false) {
            continue;
        }

        if (cell_5.type()!=XLValueType::Float && cell_5.type()!=XLValueType::Integer) {
            cout << "i " << i << "type error" << endl;
            continue;
        }
        // cout << "i " << i << endl;
        if (cell_4.get<std::string>() == "男") {
            updateBoyValue((int)month, cell_5, cell_out);
            wks.cell(i, 6).value() = cell_out;
            // break;
        } else if (cell_4.get<std::string>() == "女") {
            updateGirlValue((int)month, cell_5, cell_out);
             wks.cell(i, 6).value() = cell_out;
            //  break;
        }

    }

    // uint32_t i = 2;
    // cell_2 = wks.cell(i, 2).value();
    // cell_3 = wks.cell(i, 3).value();
    // cell_4 = wks.cell(i, 4).value();
    // cell_5 = wks.cell(i, 5).value();
    // wks.cell(i, 6).value() = cell_out;

    doc.save();
    doc.close();
    return 0;
}

bool Xlsx::checMonthkData(XLCellValue &year, XLCellValue &month, int64_t &result)
{
    // cout << "checMonthkData " << year.typeAsString() << "  " << month.typeAsString() << endl;
    if (year.type()!=XLValueType::Integer) {
        return false;
    }
    if (month.type()!=XLValueType::Integer && month.type()!=XLValueType::Float) {
        return false;
    }
    int64_t months = year.get<int64_t>()*12;
    if (month.type()==XLValueType::Integer) {
        months += month.get<int64_t>();
    } else {
        months += (int64_t)month.get<float>();
    }
    
    if (months < 61 || months > 228) {
        // cout << "error checMonthkData months " << months << endl;
        return false;
    }
    result = months;
    return true;
}
#define F_ZERO 0.001
int  Xlsx::updateBoyValue(int month, XLCellValue &value, int &out)
{
    float val = 0;
    int result = 0;
    vector<float> value_list;
    if (value.type()==XLValueType::Float) {
        val = value.get<float>();
    } else {
        val = (float)value.get<int64_t>();
    }
    

    if (m_boy_table.count(month) > 0) {
        value_list = m_boy_table[month];
        if (fabs(val-value_list[0]) < F_ZERO) {
            out = -3;
            return 0;
        } else if (fabs(val-value_list[1]) < F_ZERO) {
            out = -2;
            return 0;
        } else if (fabs(val-value_list[2]) < F_ZERO) {
            out = -1;
            return 0;
        } else if (fabs(val-value_list[3]) < F_ZERO) {
            out = 0;
            return 0;
        } else if (fabs(val-value_list[4]) < F_ZERO) {
            out = 1;
            return 0;
        } else if (fabs(val-value_list[5]) < F_ZERO) {
            out = 2;
            return 0;
        } else if (fabs(val-value_list[6]) < F_ZERO) {
            out = 3;
            return 0;
        }

        if (val < value_list[0]) {
            result = -4;
        } else if (val < value_list[1]) {
            result = -3;
        } else if (val < value_list[2]) {
            result = -2;
        } else if (val < value_list[3]) {
            result = -1;
        } else if (val < value_list[4]) {
            result = 0;
        } else if (val < value_list[5]) {
            result = 1;
        } else if (val < value_list[6]) {
            result =2;
        } else  {
            result = 3;
        }
        out = result;
    }
    return 0;
}

int  Xlsx::updateGirlValue(int month, XLCellValue &value, int &out)
{
    float val = 0;
    int result = 0;
    vector<float> value_list;

    if (value.type()==XLValueType::Float) {
        val = value.get<float>();
    } else {
        val = (float)value.get<int64_t>();
    }

    if (m_girl_table.count(month) > 0) {
        value_list = m_girl_table[month];
        if (fabs(val-value_list[0]) < F_ZERO) {
            out = -3;
            return 0;
        } else if (fabs(val-value_list[1]) < F_ZERO) {
            out = -2;
            return 0;
        } else if (fabs(val-value_list[2]) < F_ZERO) {
            out = -1;
            return 0;
        } else if (fabs(val-value_list[3]) < F_ZERO) {
            out = 0;
            return 0;
        } else if (fabs(val-value_list[4]) < F_ZERO) {
            out = 1;
            return 0;
        } else if (fabs(val-value_list[5]) < F_ZERO) {
            out = 2;
            return 0;
        } else if (fabs(val-value_list[6]) < F_ZERO) {
            out = 3;
            return 0;
        }

        if (val < value_list[0]) {
            result = -4;
        } else if (val < value_list[1]) {
            result = -3;
        } else if (val < value_list[2]) {
            result = -2;
        } else if (val < value_list[3]) {
            result = -1;
        } else if (val < value_list[4]) {
            result = 0;
        } else if (val < value_list[5]) {
            result = 1;
        } else if (val < value_list[6]) {
            result =2;
        } else  {
            result = 3;
        }
        out = result;
    }
    return 0;
}
