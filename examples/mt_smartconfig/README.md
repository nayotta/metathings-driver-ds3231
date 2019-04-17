# 注意

wifi灯 gpio 26      输出低电压点亮
按键   gpio 14      按下输入低电压

笔记本供电不稳时会提示Brownout detector was triggered并重启

可禁用电压检测
make menuconfig->component config->ESP32-specific->Hardware brownout detect & reset禁用掉这个选项