1.闭环检测词典的载入：
由于Kintinuous生成路径在/src/build/,而vocabFile和src放在同一级目录下，所以如果在执行程序的时候输入词典路径，则为 ../../vocab.yml.gz
完整的参数输入为：./Kintinuous -s 7 -v ../../vocab.yml.gz -l loop.klg -ri -fl -od
2.Pangolis.cpp中一些过时的函数，在新的Pangolin库中找不到，更改了。
3.MainController.cpp更改内参数据。
4.RawLogReader.h RawLogReader.cpp 更改数据库路径。
5.加入ParameterReader.h，方便设置数据库路径，内参等参数。
