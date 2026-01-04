# debug_script.gdb 内容
# [关键修正] 允许在符号未加载时设置断点
set breakpoint pending on
# 1. 给所有关卡入口打断点
#break phase_1
#break phase_2
#break *0x400f1c
#break phase_3
#break *0x400f63
#break phase_4
#break *0x401029
#break *0x401048
#break *0x40104d
#break phase_5
break *0x40107f
break phase_6
break secret_phase

# 2. [关键技巧] 给爆炸函数打断点
# 这样万一你输错了，程序不会直接退出，而是停在爆炸前
# 给你一个“悔棋”或是查看“死因”的机会
break explode_bomb

# 3. 可以在断点处自动打印一些信息（可选的高级玩法）
# 比如每次停下时，自动打印 rax 寄存器
# commands
#   silent
#   printf "Stopped at phase. Current RAX: %d\n", $rax
# end

# 4. 甚至可以定义简写命令（别名）
# 比如定义 'pstr' 来打印 rdi 指向的字符串
#define pstr
#    x/s $rdi
#end