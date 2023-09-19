#include <stdio.h>
#include <OpenXLSX.hpp>
#include <iostream>
#include <cmath>

#include "func_xlsx.h"

using namespace std;
using namespace OpenXLSX;



int main(int argc, char **argv)
{
    printf("hello\n");
    // XLDocument doc;
    // doc.open("儿童Z平分-230911-待查3.xlsx");
    // auto wks = doc.workbook().worksheet("1-17岁全数据-整理");

    // cout << "wks.rows:" << wks.rowCount() << endl;
    // cout << "wks.columns:" << wks.columnCount() << endl;

    // XLCellValue A1 = wks.cell("A1").value();
    // cout << "Cell A1: (" << A1.typeAsString() << ") " << A1.get<std::string>() << endl;
    // // wks.cell("F2").value() = 11;
    // doc.save();
    // doc.close();


    Xlsx xls;
    xls.loadBoyTable("3 sft_bmifa_boys_z_5_19years.xlsx");
    xls.loadGirlTable("4 sft_bmifa_girls_z_5_19years.xlsx");
    xls.updateTable("儿童Z平分-230911-待查3.xlsx");
	return 0;
}