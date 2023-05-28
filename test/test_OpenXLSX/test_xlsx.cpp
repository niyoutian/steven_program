#include <stdio.h>
#include <OpenXLSX.hpp>
#include <iostream>
#include <cmath>

using namespace std;
using namespace OpenXLSX;



int main(int argc, char **argv)
{
    printf("hello\n");
    XLDocument doc;
    doc.create("./output.xlsx");
    doc.save();
    doc.close();
	return 0;
}