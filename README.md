# steven_program 
steven's program

# doxygen
## doxygen 安装
sudo apt install graphviz

sudo apt install doxygen

sudo apt install doxygen-latex


## doxygen 生成配置文件
doxygen -g  会生成Doxyfile

再修改配置文件Doxyfile

CALL_GRAPH = YES

HAVE_DOT = YES

INPUT  = /home/steven/study/steven_program

## doxygen 输出文档
输入 doxygen Doxyfile , 输出文档.

在生成的latex目录下执行make 命令

# need lib
sudo apt-get install libssl-dev
sudo apt-get install libglib2.0-dev
sudo apt-get install libdbus-glib-1-dev
