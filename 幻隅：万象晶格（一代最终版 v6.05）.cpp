/*
Copyright (c) 2025 《幻隅：万象晶格》开发团队
《幻隅：万象晶格》 (IllusionaryNook) is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
    http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.
*/

/*
Copyright (c) EasyX Team
EasyX Graphics Library is licensed under GNU General Public License v2 (GPL v2).
You can use this library according to the terms and conditions of the GPL v2.
You may obtain a copy of GPL v2 at:
    https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
THIS LIBRARY IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the GPL v2 for more details.
*/


#define _CRT_SECURE_NO_WARNINGS  //屏蔽 _getch 安全警告
#define WIN32_LEAN_AND_MEAN  //精简Windows.h的内容，编译更快、冲突更少

//禁用 Windows 头文件中自动定义的min/max宏
#ifndef NOMINMAX//检测
#define NOMINMAX//禁用
#endif

//头文件
#include <iostream>       // 标准输入输出流
#include <cstdlib>        // C标准通用工具库
#include <cmath>          // 数学函数库
#include <climits>        // 系统极限值定义
#include <string>         // C++字符串类
#include <unordered_map>  // 无序哈希表
#include <iomanip>        // 输入输出格式控制
#include <graphics.h>     // EasyX图形库
#include <time.h>         // 时间函数库
#include <bitset>         // 位集操作
#include <stdlib.h>       // C标准库
#include <windows.h>      // Windows系统API    
#include <vector>         // 动态数组容器
#include <conio.h>        // 控制台输入输出
#include <limits>         // C++数值极限
#include <algorithm>      // 算法库
#include <sstream>        // 字符串流
#include <direct.h>       // 目录操作函数库
#include <errno.h>        // 错误码定义库
#include <stdexcept>      // 标准异常类库
#include <io.h>           // 输入输出操作库
#include <fcntl.h>        // 文件控制定义库
#include <mmsystem.h>     // 多媒体系统函数库
#include <thread>         // 线程操作库
#include <atomic>         // 原子操作库
#include <random>         // 随机数生成库
#include <chrono>         // 时间日期库
#include <imm.h>          // Windows输入法管理器API
#include <queue>          // 队列容器

#pragma comment(lib, "winmm.lib") // 链接多媒体库  
#pragma comment(lib, "imm32.lib") // 链接输入法支持库 

using namespace std;

//版本号
const short version1 = 6;
const short version2 = 4;

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------
  《幻隅：万象晶格》一期工程    2025.11.15 - 2025.12.29    开发团队：浪兮、多茵、TianG、chorsos
--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//全局变量声明
unsigned long long seed = 1314;//种子
int countnum = 0;//通用计数器
int modecode = 0;//通用模式代码
int roundlogic = -1;//取整逻辑

// 3.33  游戏模式枚举
enum GameMode
{
    BUILD_MODE = 0,    // 建造模式
    PARKOUR_MODE = 1   // 跑酷模式
};

// 当前游戏模式
GameMode currentGameMode = PARKOUR_MODE;

// 模式切换函数
extern bool physicson;
extern bool showphysicalstate;
extern bool usetool;

void toggleGameMode()
{
    currentGameMode =
        (currentGameMode == BUILD_MODE) ?
        PARKOUR_MODE :
        BUILD_MODE;
    cout << "切换到"
        <<
        (
            currentGameMode == BUILD_MODE ?
            "建造模式" :
            "跑酷模式"
            )
        << endl;
    physicson =
        (currentGameMode == BUILD_MODE) ?
        0 :
        1;
    showphysicalstate =
        (currentGameMode == BUILD_MODE) ?
        1 :
        0;
    usetool =
        (currentGameMode == BUILD_MODE) ?
        1 :
        0;
}

// 在物理输入中检测模式切换键（G键）
void checkModeToggle()
{
    //cout << "我检测了！" << endl;
    static bool gKeyPressed = false;
    if ((GetAsyncKeyState('G') & 0x8000) != 0)
    {
        //cout << "按下按键G！" << endl;
        if (!gKeyPressed)
        {
            toggleGameMode();
            gKeyPressed = true;
        }
    }
    else
    {
        gKeyPressed = false;
    }
}

//v3.30.1    更新：准星检测相关的结构体和变量
struct RaycastHit
{
    bool hit;           // 是否命中方块
    int blockX;         // 方块X坐标
    int blockY;         // 方块Y坐标
    int blockH;         // 方块高度
    int face;           // 命中的面 (0=前,1=后,2=左,3=右,4=上,5=下)
    float distance;     // 命中距离
    int blockType;      // 方块类型
};

// 全局准星命中信息
RaycastHit crosshairHit;
const float MAX_RAY_DISTANCE = 10.0f;  // 最大检测距离
bool showCrosshairDebug = false;       // 是否显示调试信息


//v3.04.1更新
int screenWidth = 0;
double mouseRotateRatio = 0.0; // 鼠标每px对应的旋转弧度

//常量声明
const double pi = acos(-1.0);//圆周率
const unsigned max_unsigned_int = 4294967295;//2^32-1

const unsigned long long RP1 = 135821345589773;//1号质数
const unsigned long long RP2 = 347561928374511;//2号质数
const unsigned long long RP3 = 558977132134581;//3号质数
const unsigned long long RP4 = 897713213455973;//4号质数
const unsigned long long RP5 = 2862933555777941653;//Xorshift算法推荐质数

const int chunkstep = 16;//区块步长：2^4
const int shift = 4;//移动位数：log_2(16)
const int chunkmask = 0b1111;//区块按位与掩码

//【音频播放模块】

//音乐播放
void playBGM(const string& bgmPath)
{
    /*
    SND_FILENAME：按文件名播放
    SND_ASYNC：异步播放（不卡游戏)
    SND_LOOP：无限循环
    */
    PlaySoundA((string("./游戏音乐/") + bgmPath).c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

//停止播放
void stopBGM()
{
    PlaySoundA(NULL, NULL, 0);
}

//音效播放
void playSoundEffect(const string& effectPath)
{
    PlaySoundA((string("./游戏音乐/") + effectPath).c_str(), NULL, SND_FILENAME | SND_ASYNC);
}

/*全局按键音（功能调试中，暂时停用）

// 全局原子变量：控制后台监听线程（防止内存泄漏）
atomic<bool> g_listenEnterKey(true);

// 音效播放函数（自定义路径/系统音效）
void playEnterSound()
{
    playSoundEffect("B7短音_bpm130.wav");
}

// 后台线程：全局监听回车键
void listenEnterKeyThread()
{
    HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
    DWORD oldMode;
    // 保存原有控制台模式，后续恢复
    GetConsoleMode(hConsole, &oldMode);
    // 开启事件监听模式（取消行缓冲，捕获所有按键）
    SetConsoleMode(hConsole, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    INPUT_RECORD inputRecord;
    DWORD readCount;

    // 持续监听，直到程序退出
    while (g_listenEnterKey)
    {
        // 读取控制台输入事件
        if (ReadConsoleInputA(hConsole, &inputRecord, 1, &readCount))
        {
            // 只处理键盘按下事件
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown)
            {
                // 检测到回车键（VK_RETURN）
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RETURN)
                {
                    playEnterSound(); // 自动播放音效
                }
            }
        }
        // 短暂休眠，降低CPU占用
        //Sleep(10);
        //已禁用休眠
    }

    // 程序退出前恢复控制台原有模式
    SetConsoleMode(hConsole, oldMode);
}

// 初始化函数：启动后台监听线程（程序启动时调用一次即可）
void initEnterSoundListener()
{
    // 创建后台线程（分离线程，不阻塞主线程）
    thread listenThread(listenEnterKeyThread);
    listenThread.detach();
    // 可选：创建音效目录（如果用自定义WAV）
    _mkdir("./音效");
}

// 程序退出时调用（可选，确保线程正常结束）
void stopEnterSoundListener()
{
    g_listenEnterKey = false;
}

*/

//异常处理
void Err(int ecode, int handlingtype, bool abort)
{
    playSoundEffect("C8_G7_E7_C7.wav");
    cout << ecode << endl;//临时
    if (handlingtype == 1)
    {

    }
}

void initMouseRotation()//v3.04.1更新
{
    // 获取屏幕分辨率
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    // 计算旋转系数：屏幕宽度对应360°（2π弧度），确保左滑到右转一圈
    mouseRotateRatio = (2 * 3.1416) / screenWidth;
}

//【柏林噪声Perlin Noise算法】地形生成

bool usingearlyversion = 0;

// 优化版Xorshift（更长周期的64位混淆，替换你原有的xorshift）
unsigned long long xorshift(unsigned long long num)
{
    if (usingearlyversion)
    {
        num ^= num >> 12;//右移异或混淆
        num ^= num << 25;//左移异或混淆
        num ^= num >> 27;//右移异或混淆
        num *= RP5;//打散分布（此处溢出为取模作用，无需处理）
        return num;
    }
    num ^= num << 13;  // 调整移位位数，延长周期
    num ^= num >> 7;
    num ^= num << 17;
    // 改用更优的乘数（64位Xorshift推荐的质数，避免溢出后周期变短）
    num *= 0x85EBCA6B2AE629D5ULL;
    return num;
}

double gradient(unsigned x_chunk, unsigned y_chunk, unsigned long long seed, bool isfromy)
{
    if (usingearlyversion)
    {
        unsigned long long hash = seed;//引入哈希值
        if (!isfromy)
        {
            hash ^= unsigned long long(x_chunk) * RP1;//按位异或混合区块顶点横坐标
            hash ^= unsigned long long(y_chunk) * RP2;//按位异或混合区块顶点纵坐标
        }
        else
        {
            hash ^= unsigned long long(x_chunk) * RP3;//按位异或混合区块顶点横坐标
            hash ^= unsigned long long(y_chunk) * RP4;//按位异或混合区块顶点纵坐标
        }
        hash = xorshift(hash);//伪随机混淆
        unsigned int hash_high32 = hash >> 32;//右移32位取哈希值高32位
        double grad = (hash_high32 / max_unsigned_int) * 2 - 1;//映射到[-1,1]范围
        return grad;
    }
    unsigned long long hash = seed;
    // 关键：用更小的质数+加法，让相邻chunk的hash变化更平缓（避免跳变）
    hash += (unsigned long long)x_chunk * 17ULL;   // 小质数，相邻x_chunk+1仅增加17
    hash += (unsigned long long)y_chunk * 23ULL;   // 小质数，相邻y_chunk+1仅增加23
    if (isfromy) hash ^= 0x12345678ULL;            // 弱化混淆强度，减少跳变

    hash = xorshift(hash);
    unsigned int hash_32 = (unsigned int)(hash & 0xFFFFFFFFULL);
    double grad = (double)hash_32 / max_unsigned_int * 2.0 - 1.0;
    grad *= 0.7; // 保持降躁，进一步缩小梯度波动范围
    return grad;
}

//梯度向量获取
void testgradient()
{
    int testg1, testg2;
    cin >> testg1 >> testg2;
    cout << gradient(testg1, testg2, seed, 0) << " " << gradient(testg1, testg2, seed, 1) << endl;
}

//梯度向量引导
void helperoftestgradient()
{
    cout << "[来自：梯度向量引导]我们需要2个I，作为：（x，y）坐标" << endl;
    testgradient();
    cout << "[来自：梯度向量引导]引导退出" << endl;
}

//获取区块编号
unsigned getchunk(unsigned block)
{
    unsigned chunk = block >> shift;//右移4位取坐标值
    return chunk;
}

//计算偏移量
double getoffset(unsigned block)
{
    int blockoffset = block & chunkmask;//低位偏移
    return blockoffset;
}

//缓动函数
double smooth(double t)
{
    double smootht = 6 * pow(t, 5) - 15 * pow(t, 4) + 10 * pow(t, 3);//缓动函数
    return smootht;
}

//计算偏移比例
double getratio(unsigned block)
{
    float ratio = getoffset(block) / float(chunkstep);//求比例
    return smooth(ratio);
}

// 权重算法（内置插值算法）
double getweight(unsigned xblock, unsigned yblock, bool xbit, bool ybit)
{
    double xweight = xbit ? (1 - getratio(xblock)) : getratio(xblock);//横向分权重
    double yweight = ybit ? (1 - getratio(yblock)) : getratio(yblock);//纵向分权重
    double weight = xweight * yweight;//分权重混合
    return weight;
}

//单影响波动贡献算法
double getcontribution(unsigned xblock, unsigned yblock, bool xbit, bool ybit)
{
    //单顶点横向波动贡献
    double xcontribution =
        gradient
        (
            getchunk(xblock),
            getchunk(yblock),
            seed,
            0
        )
        *
        (
            xbit
            ? (chunkstep - getoffset(xblock))
            : getoffset(xblock)
            );
    //单顶点纵向波动贡献
    double ycontribution =
        gradient
        (
            getchunk(xblock),
            getchunk(yblock),
            seed,
            1
        )
        *
        (
            ybit
            ? (chunkstep - getoffset(yblock))
            : getoffset(yblock)
            );
    double contribution = xcontribution + ycontribution;//合成波动贡献
    return contribution;
}

//波动贡献加权混合算法
double getoriginalheight(unsigned xblock, unsigned yblock)
{
    double originalheight = 0;//设置混合容器
    for (int i = 0; i < 2; i++)//横向遍历
    {
        for (int j = 0; j < 2; j++)//纵向遍历
        {
            originalheight += getweight(xblock, yblock, i, j) * getcontribution(xblock, yblock, i, j);//加权混合
        }
    }
    return originalheight;
}

//原始高度打印指令
void testoriginalheight(unsigned x1, unsigned x2, unsigned xstep, unsigned y1, unsigned y2, unsigned ystep)
{
    countnum = 0;//计数器归零
    for (int i = x1; i < x2 + 1; i += xstep)
    {
        for (int j = y1; j < y2 + 1; j += ystep)
        {
            cout << "OH(" << i << "," << j << ")=" << getoriginalheight(i, j) << "，   ";//打印OH
            countnum++;
        }
    }
    cout << endl;
}

//原始高度打印引导
void helperoftestoriginalheight()
{
    cout << "[来自：OH批量引导]我们需要6个U，分别作为：（x，y）*（起始值，终止值，步长）" << endl;
    unsigned a, b, c, d, e, f;
    cin >> a >> b >> c >> d >> e >> f;
    cout << "[来自：OH批量引导]开始输出：" << endl;
    testoriginalheight(a, b, c, d, e, f);
    cout << "[来自：OH批量引导]" << countnum << "个数据打印完成，引导退出" << endl;
}

//数量级判定
int lendthofnumber(unsigned long long n)
{
    int countn = 0;
    unsigned long long testnum = n;
    while (testnum != 0)
    {
        countn++;
        testnum /= 10;
    }
    return countn;
}

//十进制取高位
unsigned long long move(unsigned long long num, int movetimes)
{
    unsigned long long num2 = num;
    for (int i = 1; i < movetimes + 1; i++)
    {
        num2 /= 10;
    }
    return num2;
}

//种子修改引导
void helperofseedchange()
{
    cout << "[来自：种子修改引导]您原来的种子为：" << seed << ",确认修改吗？该操作需特别谨慎。重新输入原来的种子" << (seed < 10000 ? "以确认" : "的前4位以确认") << endl;
    unsigned long long a;
    cin >> a;
    int seedkey = seed < 10000 ? seed : move(seed, lendthofnumber(seed) - 4);
    if (a == seedkey)
    {
        cout << "[来自：种子修改引导]我们需要1个ULL，作为：新的种子" << endl;
        unsigned long long b;
        cin >> b;
        seed = b;
        cout << "[来自：种子修改引导]种子已修改为：" << seed << " 引导退出" << endl;
    }
    else
    {
        cout << "[来自：种子修改引导]输入不正确，引导退出" << endl;
    }
}

//映射算法（第2版）
double getmappingheight(double ohnum)
{
    if (usingearlyversion)
    {
        double oh = ohnum;
        oh -= 4500000000;//数据平移
        oh /= 1800000000;//数据缩放
        return max(1.0, (oh - 15) / 2.5);
    }
    const double OH_MIN = -15.0;  // 缩小原始高度波动范围
    const double OH_MAX = 15.0;
    double normalized = (ohnum - OH_MIN) / (OH_MAX - OH_MIN);
    normalized = max(0.2, min(0.8, normalized)); // 避开0和1的极端值

    double min_height = 8.0;   //提高最低高度，减少“悬崖式”下跌
    double max_height = 30.0;  //降低最高高度，减少“飞升式”上涨
    return min_height + normalized * (max_height - min_height);
}

//取整逻辑
int getheight(double mp)
{
    if (roundlogic == -1)
    {
        return max(1, int(mp));
    }
    else if (roundlogic == 1)
    {
        return mp == int(mp) ? mp : 1 + int(mp);
    }
    else
    {
        return int(0.5 + mp);
    }
}

//取整逻辑引导
void helperofroundlogic()
{
    cout << "[来自：取整逻辑引导]程序支持以下3种取整逻辑：<-1> max(1,int(mp))  <0> int(0.5+mp)  <1> mp==int(mp)?mp:1+int(mp)  （default：-1）" << endl;
    cout << "[来自：取整逻辑引导]当前取整逻辑为：" << roundlogic << " 确认修改吗？" << endl;
    cout << "[来自：取整逻辑引导]如需修改，请输入-1，0，1中的一个值，输入不可接受的值可取消修改" << endl;
    int a;
    cin >> a;
    if (a == -1 || a == 0 || a == 1)
    {
        roundlogic = a;
        cout << "[来自：取整逻辑引导]取整逻辑已修改为：" << roundlogic << " 引导退出" << endl;
    }
    else
    {
        cout << "[来自：取整逻辑引导]输入不可接受，引导退出" << endl;
    }
}

//高度局部平滑
int smooth_height(int x, int y)
{
    double total = 0.0;
    int count = 0;
    // 只遍历：当前(x,y) + 上(x,y-1) + 下(x,y+1) + 左(x-1,y) + 右(x+1,y)（共5个坐标）
    // 顺序：当前→左→右→上→下，减少循环开销
    // 1. 当前坐标
    total += getmappingheight(getoriginalheight(x, y));
    count++;
    // 2. 左（x-1,y）
    if (x - 1 >= 0) {
        total += getmappingheight(getoriginalheight(x - 1, y));
        count++;
    }
    // 3. 右（x+1,y）
    total += getmappingheight(getoriginalheight(x + 1, y));
    count++;
    // 4. 上（x,y-1）
    if (y - 1 >= 0) {
        total += getmappingheight(getoriginalheight(x, y - 1));
        count++;
    }
    // 5. 下（x,y+1）
    total += getmappingheight(getoriginalheight(x, y + 1));
    count++;

    // 平均值（5个坐标的平均，足够平滑）
    double avg = total / count;

    // 区块边界轻量化处理（只查跨区块的1格，几乎无开销）
    if (x % chunkstep == 0 || y % chunkstep == 0) {
        double edge_avg = 0.0;
        int edge_count = 0;
        // 边界只取“跨区块”的左右/上下各1格
        if (x % chunkstep == 0) {
            // x=16时，取x=15（左）和x=17（右）
            edge_avg += getmappingheight(getoriginalheight(x - 1, y)) + getmappingheight(getoriginalheight(x + 1, y));
            edge_count += 2;
        }
        if (y % chunkstep == 0) {
            // y=16时，取y=15（上）和y=17（下）
            edge_avg += getmappingheight(getoriginalheight(x, y - 1)) + getmappingheight(getoriginalheight(x, y + 1));
            edge_count += 2;
        }
        // 混合平均（仅边界时执行，开销极小）
        if (edge_count > 0) {
            avg = (avg * count + edge_avg) / (count + edge_count);
        }
    }

    return getheight(avg);
}

//地形模式
int terraincode = 3;
string terrain[4] = { "虚空","空白画布","草原","自然地形" };

//地面高度生成
int height(int x, int y)
{
    if (terraincode == 0)
    {
        return 0;
    }
    if (terraincode == 1)
    {
        return 1;
    }
    if (terraincode == 2)
    {
        return 10;
    }
    if (terraincode == 3)
    {
        if (usingearlyversion || terraincode != 3)
        {
            return getheight(getmappingheight(getoriginalheight(x, y)));
        }
        return smooth_height(x, y);
    }
}

//地形模式引导
void helperofterrain()
{
    cout << "[来自：地形模式引导]选择一种地形模式：0.虚空 1.空白画布 2.草原 3.自然地形  (当前模式：" << terraincode << ")" << endl;
    int a;
    cin >> a;
    if (!(a == 1 || a == 2 || a == 3 || a == 0))
    {
        cout << "[来自：地形模式引导]无效地形模式代码.引导退出" << endl;
        return;
    }
    terraincode = a;
    cout << "[来自：地形模式引导]已经将地形模式修改为：" << terraincode << "." << terrain[terraincode] << " 引导退出" << endl;
}

//实际高度打印指令
void testheight(unsigned x1, unsigned x2, unsigned xstep, unsigned y1, unsigned y2, unsigned ystep)
{
    countnum = 0;//计数器归零
    for (int i = x1; i < x2 + 1; i += xstep)
    {
        for (int j = y1; j < y2 + 1; j += ystep)
        {
            cout << "RH(" << i << "," << j << ")=" << height(i, j) << "，   ";//打印RH
            countnum++;
        }
    }
    cout << endl;
}

//实际高度打印引导
void helperoftestheight()
{
    cout << "[来自：RH批量引导]我们需要6个U，分别作为：（x，y）*（起始值，终止值，步长）" << endl;
    unsigned a, b, c, d, e, f;
    cin >> a >> b >> c >> d >> e >> f;
    cout << "[来自：RH批量引导]开始输出：" << endl;
    testheight(a, b, c, d, e, f);
    cout << "[来自：RH批量引导]" << countnum << "个数据打印完成，引导退出" << endl;
}
//方块类型代码
#define Empty 0
#define Grass 1
#define Soil 2
#define Stone 3
#define Sand 4
#define Plank 5
#define Brick 6
#define Red 7
#define Blue 8
#define Green 9
#define Yellow 0xA
#define Purple 0xB
#define Pink 0xC
#define Orange 0xD
#define White 0xE
#define Black 0xF

//方块类型数组
const char* blocktypename[] =
{
    "Empty",
    "Grass",
    "Soil",
    "Stone",
    "Sand",
    "Plank",
    "Brick",
    "Red",
    "Blue",
    "Green",
    "Yellow",
    "Purple",
    "Pink",
    "Orange",
    "White",
    "Black"
};

//色彩类型
struct color
{
    int r, g, b;
};

//颜色映射
color blockcolor[] =
{
    {0, 0, 0},          // 透明
    {34, 139, 34},      // Grass (1)：草绿色
    {139, 69, 19},      // Soil (2)：土棕色
    {128, 128, 128},    // Stone (3)：灰色
    {244, 244, 162},    // Sand (4)：沙黄色
    {205, 133, 63},     // Plank (5)：木板棕
    {178, 34, 34},      // Brick (6)：砖红色
    {255, 0, 0},        // Red (7)：纯红色
    {0, 0, 255},        // Blue (8)：纯蓝色
    {0, 255, 0},        // Green (9)：纯绿色
    {255, 255, 0},      // Yellow (0xA)：纯黄色
    {128, 0, 128},      // Purple (0xB)：纯紫色
    {255, 192, 203},    // Pink (0xC)：粉红色
    {255, 165, 0},      // Orange (0xD)：橙色
    {255, 255, 255},    // White (0xE)：纯白色
    {0, 0, 0}           // Black (0xF)：纯黑色
};

//类型代码引导
void helperofblockcode()
{
    cout << "[来自：类型代码引导]我们需要1个int，作为：typecode" << endl;
    int a;
    cin >> a;
    cout << "[来自：类型代码引导]typecode="
        << a
        << "("
        << blocktypename[a]
        << ") 其特征配色为：RGB（"
        << blockcolor[a].r
        << ","
        << blockcolor[a].g
        << ","
        << blockcolor[a].b
        << ")  引导退出"
        << endl;
}

//自然方块类型
int natureblocktype(unsigned x, unsigned y, int h)
{
    if (h < 0)
    {
        return 0;
    }
    if (terraincode == 2 || terraincode == 3)
    {
        int hm = height(x, y);
        if (h == 1)
        {
            return 15;//自然基岩
        }
        else if (h < hm - 4)
        {
            return 3;//自然岩石
        }
        else if (h == hm)
        {
            return 1;//自然草
        }
        else if (h > hm)
        {
            return 0;//空位
        }
        else
        {
            return 2;//自然土
        }
    }
    if (terraincode == 0)
    {
        return 0;
    }
    if (terraincode == 1)
    {
        if (h == 1)
        {
            return 14;
        }
        return 0;
    }
}

//自然地形引导
void helperofnatureblocktype()
{
    cout << "[来自：自然地形引导]我们需要3个U，分别作为：（x，y，h）" << endl;
    unsigned a, b;
    int c;
    cin >> a >> b >> c;
    int d = natureblocktype(a, b, c);
    cout << "[来自：自然地形引导]该定位（"
        << a
        << ","
        << b
        << ") h="
        << c
        << " 的原始方块类型为："
        << d
        << "("
        << blocktypename[d]
        << ") 引导退出"
        << endl;
}

//方块状态
struct blockdata
{
    unsigned x;
    unsigned y;
    int h;
    int type;
};

//声明哈希表（键值对字典）
unordered_map<string, blockdata> blockmap;

//钥匙生成器
string getblockkey(unsigned x, unsigned y, int h)
{
    return to_string(x) + "," + to_string(y) + "," + to_string(h);//合成钥匙
}

//状态更新工具
void updateblock(unsigned x, unsigned y, int h, int newtype)
{
    string key = getblockkey(x, y, h);//制作钥匙
    blockmap[key] = { x,y,h,newtype };//修改状态
}


//状态检索工具
blockdata blockfound = {};//状态数据池
bool findresult(unsigned x, unsigned y, int h)
{
    string key = getblockkey(x, y, h);//制作钥匙
    auto it = blockmap.find(key);//记录迭代器数据
    if (it != blockmap.end())//排除扫尽未找到
    {
        blockfound = it->second;//哈希表右值外传
        return true;
    }
    else
    {
        blockfound =
        {
            x,
            y,
            h,
            natureblocktype(x,y,h)
        };
        //读取自然地块
        return false;
    }
}
int findtype(unsigned x, unsigned y, int h)
{
    findresult(x, y, h);
    return blockfound.type;
}

//地形编辑工具
void helperofupdateblock()
{
    cout << "[来自：地形编辑工具]我们需要4个U，分别作为：x，y，h，typecode" << endl;
    unsigned a, b;
    int c, d;
    cin >> a >> b >> c >> d;
    int e = findtype(a, b, c);
    if (e != d)
    {
        updateblock(a, b, c, d);
        cout << "[来自：地形编辑工具]操作成功。位点("
            << a
            << ","
            << b
            << ") h="
            << c
            << "的类型从"
            << e
            << "("
            << blocktypename[e]
            << ")修改为"
            << d
            << "("
            << blocktypename[d]
            << ")  引导退出"
            << endl;
    }
    else
    {
        cout << "[来自：地形编辑工具]位点("
            << a
            << ","
            << b
            << ") h="
            << c
            << "的类型已经是"
            << e
            << "("
            << blocktypename[e]
            << ")  引导退出"
            << endl;
    }
}

//地形批量工具
void helperofmultipleupdateblock()
{
    cout << "[来自：地形批量工具]我们需要7个U，分别作为：（x，y，h）*（左界，右界）+ typecode" << endl;
    unsigned a1, a2, b1, b2;
    int c1, c2, d;
    cin >> a1 >> a2 >> b1 >> b2 >> c1 >> c2 >> d;
    for (int i = a1; i < a2 + 1; i++)
    {
        for (int j = b1; j < b2 + 1; j++)
        {
            for (int k = c1; k < c2 + 1; k++)
            {
                int e = findtype(i, j, k);//限定避免数据堆积
                if (e != d)
                {
                    updateblock(i, j, k, d);
                }
            }
        }
    }
    cout << "[来自：地形编辑工具]已经将"
        << (a2 - a1 + 1) * (b2 - b1 + 1) * (c2 - c1 + 1)
        << "个方块的类型修改为："
        << d
        << "("
        << blocktypename[d]
        << ")  引导退出"
        << endl;
}

//定点探测工具
void helperoffindtype()
{
    cout << "[来自：定点探测引导]我们需要3个U，分别作为：x，y，h" << endl;
    unsigned a, b;
    int c;
    cin >> a >> b >> c;
    bool d = findresult(a, b, c);
    cout << "[来自：定点探测引导]该方块"
        << (d ? "已经过编辑且未恢复" : "未经过编辑或已恢复")
        << "，现在的类型为："
        << blockfound.type
        << "("
        << blocktypename[blockfound.type]
        << ")  引导退出"
        << endl;
}

//探测批量工具
void helperofmultiplefindtype()
{
    cout << "[来自：探测批量工具]我们需要6个U，分别作为：（x，y，h）*（左界，右界）" << endl;
    unsigned a, b, c, d;
    int e, f;
    cin >> a >> b >> c >> d >> e >> f;
    for (int i = a; i < b + 1; i++)
    {
        for (int j = c; j < d + 1; j++)
        {
            for (int k = e; k < f + 1; k++)
            {
                cout << "type(" << i << "," << j << "," << k << ")=" << findtype(i, j, k) << "   ";
            }
        }
    }
    cout << endl;
    cout << "[来自：探测批量工具]"
        << (b - a + 1) * (d - c + 1) * (f - e + 1)
        << "个位点探测完成"
        << endl;
}

// 哈希打印工具
void blockmapprinter()
{
    if (blockmap.empty())
    {
        cout << "[来自：哈希打印引导] 哈希表为空。这表明：目前没有已编辑的方块（所有方块都是自然生成的） 引导退出" << endl;
    }
    else
    {
        cout << "[来自：哈希打印引导] 共找到" << blockmap.size() << "个已编辑的方块：" << endl;
        int index = 1; // 序号
        //哈希遍历
        for (auto& pair : blockmap)
        {
            cout << index
                << ". 键："
                << pair.first
                << " → 方块信息(值)：x="
                << pair.second.x
                << ", y="
                << pair.second.y
                << ", h="
                << pair.second.h
                << ", 类型="
                << pair.second.type
                << "（"
                << blocktypename[pair.second.type]
                << "）"
                << endl;
            index++;
        }
        cout << "[来自：哈希打印引导]哈希表打印完成。引导退出" << endl;
    }
}

//物理系数据类型定义：

//物理状态
struct physicalstate
{
    double x, y, h;//位置
    double vx, vy, vh;//速度
    double ax, ay, ah;//加速度
};

//物理参数
struct physicalparameters
{
    double ar;//空气阻力air resistence
    double fr;//摩擦阻力fictional resistence
    double g;//重力加速度
    double fj;//跳跃力度jump force
    double frm;//最大奔跑力度maximum running force
    double vm;//最大速度
    double kt;//时间系数，应当参考帧率设计
    double km;//质量系数（静态系数）
    double wjbs;//墙跳反弹强度walljump bounce strength
    double vleap;//跃升速度
};

//交互参数
struct interactionparameters
{
    bool jumping;//起跳中
    bool climbing;//爬梯中
    bool sightleftrunning;//左滑步
    bool sightrightrunning;//右滑步 
    bool frontrunning;//向前跑
    bool backrunning;//向后退
};

//物理探测
struct physicaldetection
{
    bool isonground;//在地面上？
    bool xmcollision, xpcollision, ymcollision, ypcollision;//水平碰撞检测
    bool upcollosion;//头顶碰撞检测
};

//结构参数
struct structuralparameters
{
    float r, h;//判定圆柱体形态
    float s, sh;//接触判定区宽度
    float sj;//跳跃判定区宽度
};

//体位参数
struct orientationparameters
{
    double sighta;//视方位角
    double sightb;//视俯仰角
    double body;//身体朝向
};

//控制参数
struct controlparameters
{
    bool autostepup;//自动上台阶
    bool walljump;//墙跳
    bool fallprotection;//掉落保护
};

//玩家参数
struct playerparameters
{
    string name;//游戏名
};

//玩家数据存储
struct player
{
    physicalstate state;
    physicalparameters parameters;
    interactionparameters interaction;
    physicaldetection detection;
    structuralparameters structural;
    orientationparameters orientation;
    controlparameters control;
    playerparameters player;
};

//默认出生点
const double xborn = 0b1000000000001000;
const double yborn = 0x8008;

//数据处理算法(2in1)
//编码规则：19位整数：1双符号位，2个*9位存储位（内部4+5：浮点数精度5位小数）
//使用限制：对于2个数：整数部分最大9999，精度0.00001
unsigned long long num2in1(double a, double b)
{
    unsigned long long num = 0;
    num += ((a >= 0) | ((b >= 0) << 1)) * 1000000000000000000;//编码双符号位
    num += unsigned long long(
        (
            a > 0
            ? a
            : a * (-1)
            )
        * 100000
        + 0.5
        )
        + 1000000000
        * unsigned long long(
            (
                b > 0
                ? b
                : b * (-1)
                )
            * 100000
            + 0.5
            );
    //编码装载数据
    return num;
}

//外部编码工具
void helperofnum2in1()
{
    cout << "[来自：2in1编码引导]我们需要2个F，要求：整数位不超过4。转换精度：1e-5，多余数据将丢失" << endl;
    float a, b;
    cin >> a >> b;
    if (a < 10000 && b < 10000)
    {
        cout << "[来自：2in1编码引导]编码成功：" << num2in1(a, b) << " 引导退出" << endl;
    }
    else
    {
        cout << "[来自：2in1编码引导]编码失败：输入的数据不符合要求。 引导退出" << endl;
    }
}

//2in1编码算法解码1
double get2in1num1(unsigned long long num)
{
    unsigned long long a = num % 1000000000ULL;
    double b = a / 100000.0;
    unsigned long long c = num / 1000000000000000000ULL;
    int d = c % 2;
    b *= d * 2.0 - 1;
    return b;
}

//2in1编码算法解码2
double get2in1num2(unsigned long long num)
{
    unsigned long long a = (num % 1000000000000000000ULL) / 1000000000ULL;
    double b = a / 100000.0;
    unsigned long long c = num / 1000000000000000000ULL;
    int d = (c >> 1) % 2;
    b *= d * 2.0 - 1;
    return b;
}

//外部解码工具
void helperofget2in1num()
{
    cout << "[来自：2in1解码引导]我们需要1个ULL，要求：不超过3999999999999999999" << endl;
    unsigned long long a;
    cin >> a;
    if (a <= 3999999999999999999)
    {
        cout << fixed << setprecision(5) << "[来自：2in1解码引导]解码成功：" << get2in1num1(a) << "，" << get2in1num2(a) << " 引导退出" << endl;
    }
    else
    {
        cout << "[来自：2in1解码引导]解码失败：输入的数据不符合要求。 引导退出" << endl;
    }
}

//为玩家创建数据空间
player defaultplayerdata =
{
    {
        xborn,
        yborn,
        height(xborn,yborn) + 1,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0
    },
    {
        0.06,
        0.15,
        0.075,
        0.058,
        0.18,
        0.085,
        1.0,
        1.0,
        0.065,
        0.048
    },
    {
        0,
        0,
        0,
        0,
        0,
        0
    },
    {
        0,
        1,
        1,
        1,
        1,
        0
    },
    {
        0.45,
        1.85,
        0.045,
        0.07,
        0.15
    },
    {
        0.0,
        0.0,
        0.0
    },
    {
        1,
        0,
        0
    },
    {
        string("Player")
    }
};

player playerdata;

//预声明技能状态
int available[5] =
{
    0,
    0,
    0,
    0,
    0
};
//0=可用 1=生效中 -1=冷却中

//玩家初始化
// 修改 resetplayer() 函数中的初始高度
void resetplayer()
{
    playerdata = defaultplayerdata;

    // 确保初始高度在地形上方
    int groundHeight = height((unsigned)round(xborn), (unsigned)round(yborn));
    if (groundHeight <= 0) groundHeight = 10; // 保证最小高度

    playerdata.state.x = xborn;
    playerdata.state.y = yborn;
    playerdata.state.h = groundHeight + 3.0; // 保证在地面上方3个单位

    // 重置其他物理状态
    playerdata.state.vx = playerdata.state.vy = playerdata.state.vh = 0.0;
    playerdata.state.ax = playerdata.state.ay = playerdata.state.ah = 0.0;

    playerdata.detection.isonground = true; // 初始设为在地面上
}

//运动学
void refreshkinematics()
{
    // 速度更新（加时间系数）
    playerdata.state.vx += playerdata.state.ax * playerdata.parameters.kt;
    playerdata.state.vy += playerdata.state.ay * playerdata.parameters.kt;
    playerdata.state.vh += playerdata.state.ah * playerdata.parameters.kt;

    // 限制最大速度（避免穿透）
    const double max_speed = 0.5;
    if (abs(playerdata.state.vx) > max_speed)
    {
        playerdata.state.vx = max_speed * (playerdata.state.vx > 0 ? 1 : -1);
    }
    if (abs(playerdata.state.vy) > max_speed)
    {
        playerdata.state.vy = max_speed * (playerdata.state.vy > 0 ? 1 : -1);
    }
    if (abs(playerdata.state.vh) > max_speed)
    {
        playerdata.state.vh = max_speed * (playerdata.state.vh > 0 ? 1 : -1);
    }

    // 位置更新
    playerdata.state.x += playerdata.state.vx * playerdata.parameters.kt;
    playerdata.state.y += playerdata.state.vy * playerdata.parameters.kt;
    playerdata.state.h += playerdata.state.vh * playerdata.parameters.kt;
}

//弧度修正
double radcorrect(double rad)
{
    double t = rad;
    while (t < 0)
    {
        t += 2 * pi;
    }
    while (t >= 2 * pi)
    {
        t -= 2 * pi;
    }
    return t;
}

//动态动力
void refreshara()
{
    if (playerdata.interaction.frontrunning ^ playerdata.interaction.backrunning)
    {
        if (playerdata.interaction.sightleftrunning ^ playerdata.interaction.sightrightrunning)
        {
            playerdata.orientation.body =
                radcorrect(
                    playerdata.orientation.sighta
                    + 0.5 * (playerdata.interaction.frontrunning ? 0 : pi)
                    + 0.5 * (playerdata.interaction.sightleftrunning ? 0.5 * pi : 1.5 * pi)
                );
        }
        else
        {
            playerdata.orientation.body =
                radcorrect(
                    playerdata.orientation.sighta
                    + (playerdata.interaction.frontrunning ? 0 : pi)
                );
        }
    }
    else if (playerdata.interaction.sightleftrunning ^ playerdata.interaction.sightrightrunning)
    {
        playerdata.orientation.body =
            radcorrect(
                playerdata.orientation.sighta
                + (playerdata.interaction.sightleftrunning ? 0.5 * pi : 1.5 * pi)
            );
    }
}

//视控参数(提前)
struct viewcontrol
{
    int rfield;//视野跨距（水平向）
    int hfield;//视野跨距（竖直向）
    //player的视野：(1+2*rfield)*(1+2*rfield)*(1+2*hfield)

    double sighta;//视方位角
    double sightb;//视俯仰角
    //数据说明：采用弧度制。程序中有一些弧度计算相关函数，可用于这部分运算
    //使用说明：物理模块中的数据结构体中也包含有以上2个数据，但是被弃用了。请始终从视觉控制中读写以上2个数据

    float ambient;//环境光强度
    float diffuse;//漫反射强度
    //说明：2个强度相加并不一定等于1，因为后续可以通过分段的映射进行调节。
    //进一步说明：现有映射函数下，1.15为标定的上限值

    float kbd;//亮度距离衰减底色范围比例
    //注意：实际执行的调整参数为亮度

    float fov;//视场角
    float nearclip;//近裁切面
    //特别注意！fov采取角度制！！

    float kborderbrightness;//边框亮度系数
    float kborderthickness;//边框宽度系数

    //视觉侦测对接数据
    float sensibility;//视角灵敏度
};

//初始数据(提前)
viewcontrol defaultviewcontrol =
{
    10,
    10,

    0,
    0,

    0.3,
    0.85,

    0.8,

    60.0,
    0.5,

    0.8,
    0.12,

    1
};

//初始设定(提前)
viewcontrol viewsetup = defaultviewcontrol;

//主动加速分配
void dynamicloading()
{
    // 1. 基础配置（新手可改这里的反转开关和初始视角）
    const double PI = 3.1416;
    bool reverseDir = false; // 方向反了就改成true
    double initialAngle = 0.0; // 初始视角：0=正前方（Y+），不用改

    // 2. 最终视角角度（初始角度 + 玩家旋转角度，确保初始朝正前方）
    double sightAngle = playerdata.orientation.sighta + initialAngle;
    viewsetup.sighta = sightAngle + 0.5 * pi;

    // 3. 极简方向计算（直接映射，不绕弯）
    double moveX = 0.0, moveY = 0.0;
    if (playerdata.interaction.frontrunning)  // W=视角正前方
    {
        moveX += sin(sightAngle); // 关键修正：X轴用sin，Y轴用cos（匹配游戏坐标系）
        moveY += cos(sightAngle);
    }
    if (playerdata.interaction.backrunning) // S=视角正后方
    {
        moveX -= sin(sightAngle);
        moveY -= cos(sightAngle);
    }
    if (playerdata.interaction.sightleftrunning)     // A=视角左方（侧移）
    {
        moveX -= cos(sightAngle);
        moveY += sin(sightAngle);
    }
    if (playerdata.interaction.sightrightrunning)    // D=视角右方（侧移）
    {
        moveX += cos(sightAngle);
        moveY -= sin(sightAngle);
    }

    // 4. 方向反转（如果移动反了，直接改上面的reverseDir为true）
    if (reverseDir)
    {
        moveX = -moveX;
        moveY = -moveY;
    }

    // 5. 限制斜向速度（必须保留，避免斜跑更快）
    double speedLen = sqrt(moveX * moveX + moveY * moveY);
    if (speedLen > 0.001)
    {
        moveX /= speedLen;
        moveY /= speedLen;
    }

    // 6. 沿用原来的移动力度，速度和之前一致
    playerdata.state.ax = moveX * playerdata.parameters.frm;
    playerdata.state.ay = moveY * playerdata.parameters.frm;
}

//阻力作用
void resistance()
{
    //摩擦阻力
    if (playerdata.detection.isonground)
    {
        playerdata.state.vx *= 1 - 5 * playerdata.parameters.fr;
        playerdata.state.vy *= 1 - 5 * playerdata.parameters.fr;
    }
    //空气阻力
    playerdata.state.vx *= 1 - playerdata.parameters.ar;
    playerdata.state.vy *= 1 - playerdata.parameters.ar;
    playerdata.state.vh *= 1 - playerdata.parameters.ar;
}

///重力场与垂直动力
void gravity()
{
    // ========== 新增：技能机制融合（2025.12.19）浪兮 ==========
    if (available[1] != 1)
    {
        // ========== 修复：精准重力判定（2025.12.9）多茵 ==========
        // 1. 地面上：无重力，清零垂直速度
        if (playerdata.detection.isonground)
        {
            playerdata.state.vh = 0.0;
            playerdata.state.ah = 0.0;
        }
        // 2. 跳跃中：仅在跳跃上升阶段抵消部分重力
        else if (playerdata.interaction.jumping)
        {
            playerdata.state.ah = -playerdata.parameters.g * 0.5; // 跳跃时重力减半
        }
        // 3. 自由下落：满重力，限制最大下落速度
        else {
            playerdata.state.ah = -playerdata.parameters.g;
            if (playerdata.state.vh < -0.8)// 限制最大下落速度，避免穿透
            {
                playerdata.state.vh = -0.8;
            }
        }
        // ========== 重力修复结束 ==========
    }
}

//电磁相互作用
void einteraction()
{
    // 重置所有加速度（避免被其他函数覆盖）
    playerdata.state.ax = 0.0;
    playerdata.state.ay = 0.0;
    playerdata.state.ah = 0.0;

    // 1. 地面碰撞：清零下落速度+抵消重力
    if (playerdata.detection.isonground)
    {
        playerdata.state.vh = 0.0;
        playerdata.state.ah = 0.0;

        // 额外：修正位置到方块顶部，避免穿透
        int floor_h = height((unsigned)playerdata.state.x, (unsigned)playerdata.state.y);
        if (playerdata.state.h < floor_h)
        {
            playerdata.state.h = floor_h;
        }
    }
    // 2. 不在地面：施加重力（向下）
    else
    {
        playerdata.state.ah = -playerdata.parameters.g;

        // 限制最大下落速度（避免穿透）
        if (playerdata.state.vh < -0.5)
        {
            playerdata.state.vh = -0.5;
        }
    }

    // 3. 头顶碰撞：清零上升速度
    if (playerdata.detection.upcollosion)
    {
        playerdata.state.vh = 0.0;
        playerdata.state.ah = 0.0;

        // 修正位置到方块底部
        int ceil_h = height((unsigned)playerdata.state.x, (unsigned)playerdata.state.y) + 1;
        if (playerdata.state.h > ceil_h)
        {
            playerdata.state.h = ceil_h;
        }
    }

    // 4. 左右/前后墙碰撞：清零对应速度
    if (playerdata.detection.xmcollision || playerdata.detection.xpcollision)
    {
        playerdata.state.vx = 0.0;
    }
    if (playerdata.detection.ymcollision || playerdata.detection.ypcollision)
    {
        playerdata.state.vy = 0.0;
    }
}

//【Axis-Aligned Bounding Box，轴对齐包围盒】检测模块

//方位定位代码
#define B 0b0
#define F 0b1
#define L 0b10
#define R 0b11
#define D 0b100
#define U 0b101

//AABB盒数据
struct AABB
{
    int x0, xm, y0, ym, h0, hm;//坐标上下界
};

struct fAABB
{
    float x0, xm, y0, ym, h0, hm;//坐标上下界
};

//玩家判定区域
AABB playerarea()
{
    return { int(playerdata.state.x - 0.5),
        playerdata.state.x - 0.5 == int(playerdata.state.x - 0.5) ? int(playerdata.state.x - 0.5) : 1 + int(playerdata.state.x - 0.5),
        int(playerdata.state.y - 0.5),
        playerdata.state.y - 0.5 == int(playerdata.state.y - 0.5) ? int(playerdata.state.y - 0.5) : 1 + int(playerdata.state.y - 0.5),
        int(playerdata.state.h - 0.5),
        1 + (playerdata.state.h - 0.5 == int(playerdata.state.h - 0.5) ? int(playerdata.state.h - 0.5) : 1 + int(playerdata.state.h - 0.5)) };
    //对于“-0.5”的说明：本项目所有方块的坐标实际表示方块在该方向位于该向坐标值与其＋1的区间范围。因此，对玩家精确坐标减半格后做取整可以获得其判定区域内的方块坐标
}

//玩家AABB盒制作
fAABB playerAABB()
{
    return
    {
        float(playerdata.state.x - playerdata.structural.r - playerdata.structural.s),
        float(playerdata.state.x + playerdata.structural.r + playerdata.structural.s),
        float(playerdata.state.y - playerdata.structural.r - playerdata.structural.s),
        float(playerdata.state.y + playerdata.structural.r + playerdata.structural.s),
        float(playerdata.state.h - playerdata.structural.h - playerdata.structural.sh),
        float(playerdata.state.h + playerdata.structural.h + playerdata.structural.sh)
    };
}
/*

//接触判定
void contactdetection(AABB block)
{

    // === 新增：精准地面检测 ===
    playerdata.detection.isonground = false;

    // 玩家脚底高度
    double footHeight = playerdata.state.h - playerdata.structural.h;

    // 玩家脚底的AABB盒（投影到地面）
    fAABB footBox =
    {
        (float)(playerdata.state.x - playerdata.structural.r),
        (float)(playerdata.state.x + playerdata.structural.r),
        (float)(playerdata.state.y - playerdata.structural.r),
        (float)(playerdata.state.y + playerdata.structural.r),
        (float)(footHeight - 0.1f),  // 脚底下方0.1单位
        (float)(footHeight + 0.1f)   // 脚底上方0.1单位
    };

    // 检查玩家周围的方块
    int playerBlockX = (int)round(playerdata.state.x);
    int playerBlockY = (int)round(playerdata.state.y);

    for (int bx = playerBlockX - 2; bx <= playerBlockX + 2; bx++)
    {
        for (int by = playerBlockY - 2; by <= playerBlockY + 2; by++)
        {
            if (bx < 0 || by < 0) continue;

            // 获取这个位置的地面高度
            int groundH = height(bx, by);

            // 方块的AABB（重点：顶面在 groundH+1 的高度！）
            fAABB blockBox =
            {
                (float)bx,
                (float)bx + 1.0f,
                (float)by,
                (float)by + 1.0f,
                (float)groundH,      // 方块底部
                (float)groundH + 1.0f // 方块顶部
            };

            // 检测脚底盒与方块顶面的碰撞
            bool x_overlap = (footBox.x0 < blockBox.xm) && (footBox.xm > blockBox.x0);
            bool y_overlap = (footBox.y0 < blockBox.ym) && (footBox.ym > blockBox.y0);
            bool h_overlap = (footBox.h0 < blockBox.hm) && (footBox.hm > blockBox.h0);

            // 关键：检查是否在方块顶面上方很近的位置
            if (x_overlap && y_overlap)
            {
                float distanceToTop = footBox.h0 - blockBox.hm; // 脚底到方块顶面的距离

                // 如果脚底在方块顶面上方0.2单位内
                if (distanceToTop > -0.2f && distanceToTop < 0.2f)
                {
                    playerdata.detection.isonground = true;

                    // 对齐到方块顶面
                    playerdata.state.h = blockBox.hm + playerdata.structural.h + 0.05f;
                    playerdata.state.vh = 0.0f;

                    // 调试输出
                    static int groundCount = 0;
                    if (groundCount++ < 5)
                    {
                        cout << "地面检测成功！方块(" << bx << "," << by << "," << groundH
                            << ") 顶面高度:" << blockBox.hm
                            << " 玩家高度:" << playerdata.state.h << endl;
                    }

                    // 找到地面就返回，不用继续检查
                    return;
                }
            }
        }
    }



    // v1.12 在原有检测前添加初始地面检测
    int floor_h = height((unsigned)round(playerdata.state.x), (unsigned)round(playerdata.state.y));

    // 如果玩家高度在地面或以下，强制设为地面上
    if (playerdata.state.h <= floor_h + 0.1)
    {
        playerdata.state.h = floor_h + 0.1;
        playerdata.detection.isonground = true;
        playerdata.state.vh = 0.0;
    }

    // 1. 重置所有碰撞状态
    playerdata.detection.isonground = false;
    playerdata.detection.xmcollision = false;
    playerdata.detection.xpcollision = false;
    playerdata.detection.ymcollision = false;
    playerdata.detection.ypcollision = false;
    playerdata.detection.upcollosion = false;

    // 2. 获取玩家AABB盒（浮点精准版）
    fAABB player_box = playerAABB();
    AABB player_area = playerarea();

    // ========== 修复：碰撞体同步（2025.12.9） ==========
    // 优化：只遍历玩家周围1格范围内的方块（减少90%无效计算）
    int x_start = max(player_area.x0 - 1, 0);       // 防负坐标
    int x_end = player_area.xm + 1;
    int y_start = max(player_area.y0 - 1, 0);
    int y_end = player_area.ym + 1;
    // ========== 碰撞体遍历优化结束 ==========

    // 3. 遍历玩家周围方块（仅遍历覆盖区域）
    for (int x = x_start; x <= x_end; x++)
    {
        for (int y = y_start; y <= y_end; y++)
        {
            // 先获取当前坐标的地面高度，只检测地面及以下方块
            int ground_h = height(x, y);
            // ========== 修复：碰撞体范围限制（2025.12.9） ==========
            int h_start = max(1, int(player_box.h0) - 1);  // 只检测玩家高度附近的方块
            int h_end = min(ground_h + 1, int(player_box.hm) + 1);
            // ========== 碰撞体范围优化结束 ==========

            for (int h = h_start; h <= h_end; h++) // 只检测有效方块，减少计算
            {
                if (findtype(x, y, h) == Empty) continue;

                // 生成方块AABB盒（1x1x1）
                fAABB block_box;
                block_box.x0 = (float)x;
                block_box.xm = (float)x + 1.0f;
                block_box.y0 = (float)y;
                block_box.ym = (float)y + 1.0f;
                block_box.h0 = (float)h;
                block_box.hm = (float)h + 1.0f;

                // 4. 标准AABB重叠判定（加微小偏移避免浮点精度问题）
                bool x_overlap = (player_box.x0 + 0.001f < block_box.xm) && (player_box.xm - 0.001f > block_box.x0);
                bool y_overlap = (player_box.y0 + 0.001f < block_box.ym) && (player_box.ym - 0.001f > block_box.y0);
                bool h_overlap = (player_box.h0 + 0.001f < block_box.hm) && (player_box.hm - 0.001f > player_box.h0);
                if (!x_overlap || !y_overlap || !h_overlap) continue;

                // ========== 修复：精准碰撞方向判定（2025.12.9） ==========
                // 计算重叠量（越小越精准）
                float h_overlap_down = block_box.hm - player_box.h0;  // 玩家脚踩方块
                float h_overlap_up = player_box.hm - block_box.h0;    // 玩家头顶撞方块
                float x_overlap_left = block_box.xm - player_box.x0;  // 玩家撞方块左侧
                float x_overlap_right = player_box.xm - block_box.x0; // 玩家撞方块右侧
                float y_overlap_back = block_box.ym - player_box.y0;  // 玩家撞方块后侧
                float y_overlap_front = player_box.ym - block_box.y0; // 玩家撞方块前侧

                // 优先判定垂直碰撞（地面/头顶）
                if (h_overlap_down < h_overlap_up && h_overlap_down < 0.1f)
                {
                    playerdata.detection.isonground = true;
                    // 修正位置：避免陷入地面
                    playerdata.state.h = block_box.hm;
                }
                else if (h_overlap_up < h_overlap_down && h_overlap_up < 0.1f)
                {
                    playerdata.detection.upcollosion = true;
                    // 修正位置：避免头顶穿透
                    playerdata.state.h = block_box.h0 - playerdata.structural.h;
                }

                // 水平碰撞判定（左右/前后）
                if (x_overlap_left < x_overlap_right && x_overlap_left < 0.1f)
                {
                    playerdata.detection.xpcollision = true;
                    playerdata.state.x = block_box.xm; // 修正位置
                }
                else if (x_overlap_right < x_overlap_left && x_overlap_right < 0.1f)
                {
                    playerdata.detection.xmcollision = true;
                    playerdata.state.x = block_box.x0 - playerdata.structural.r * 2; // 修正位置
                }

                if (y_overlap_back < y_overlap_front && y_overlap_back < 0.1f)
                {
                    playerdata.detection.ypcollision = true;
                    playerdata.state.y = block_box.ym; // 修正位置
                }
                else if (y_overlap_front < y_overlap_back && y_overlap_front < 0.1f)
                {
                    playerdata.detection.ymcollision = true;
                    playerdata.state.y = block_box.y0 - playerdata.structural.r * 2; // 修正位置
                }
                // ===================
            }
        }
    }
}



*/

//旧版代码。保留以方便调试，并提供功能回退可能。


//接触判定

// v3.24.1 增强版碰撞检测 - 修复穿墙问题
// (“终于，我杀死了它”——多茵）
void contactdetection()
{
    // 1. 重置所有碰撞状态
    playerdata.detection.isonground = false;
    playerdata.detection.xmcollision = false;
    playerdata.detection.xpcollision = false;
    playerdata.detection.ymcollision = false;
    playerdata.detection.ypcollision = false;
    playerdata.detection.upcollosion = false;

    // 2. 获取玩家AABB（扩大检测范围）
    fAABB playerBox =
    {
        (float)(playerdata.state.x - playerdata.structural.r - 0.05f), // 扩大0.05
        (float)(playerdata.state.x + playerdata.structural.r + 0.05f),
        (float)(playerdata.state.y - playerdata.structural.r - 0.05f),
        (float)(playerdata.state.y + playerdata.structural.r + 0.05f),
        (float)(playerdata.state.h - playerdata.structural.h - 0.05f),
        (float)(playerdata.state.h + playerdata.structural.h + 0.05f)
    };

    // 3. 获取玩家周围方块范围（扩大检测范围）
    int playerX = (int)playerdata.state.x;
    int playerY = (int)playerdata.state.y;
    int playerH = (int)playerdata.state.h;

    // 检测范围扩大为3x3x3
    int x_start = max(playerX - 2, 0);
    int x_end = playerX + 2;
    int y_start = max(playerY - 2, 0);
    int y_end = playerY + 2;
    int h_start = max(playerH - 3, 1); // 向下多检测1格
    int h_end = playerH + 3;          // 向上多检测1格

    // 4. 遍历检测
    for (int x = x_start; x <= x_end; x++)
    {
        for (int y = y_start; y <= y_end; y++)
        {
            for (int h = h_start; h <= h_end; h++)
            {
                if (findtype(x, y, h) == Empty)
                {
                    continue;
                }

                // 方块AABB
                fAABB blockBox =
                {
                    (float)x,
                    (float)x + 1.0f,
                    (float)y,
                    (float)y + 1.0f,
                    (float)h,
                    (float)h + 1.0f
                };

                // 检查AABB重叠（使用更小的容差值）
                bool x_overlap = (playerBox.x0 < blockBox.xm - 0.001f) && (playerBox.xm > blockBox.x0 + 0.001f);
                bool y_overlap = (playerBox.y0 < blockBox.ym - 0.001f) && (playerBox.ym > blockBox.y0 + 0.001f);
                bool h_overlap = (playerBox.h0 < blockBox.hm - 0.001f) && (playerBox.hm > blockBox.h0 + 0.001f);

                if (!x_overlap || !y_overlap || !h_overlap) continue;

                // 计算重叠量
                float overlapX = min(playerBox.xm - blockBox.x0, blockBox.xm - playerBox.x0);
                float overlapY = min(playerBox.ym - blockBox.y0, blockBox.ym - playerBox.y0);
                float overlapH = min(playerBox.hm - blockBox.h0, blockBox.hm - playerBox.h0);

                // 找出最小重叠方向
                float minOverlap = min(overlapX, min(overlapY, overlapH));

                // 修正位置并设置碰撞标志
                if (minOverlap == overlapX)
                {
                    // X轴碰撞
                    if (playerBox.x0 < blockBox.x0)
                    {
                        // 从左侧碰撞
                        playerdata.detection.xmcollision = true;
                        playerdata.state.x = blockBox.x0 - playerdata.structural.r - 0.05f;
                    }
                    else
                    {
                        // 从右侧碰撞
                        playerdata.detection.xpcollision = true;
                        playerdata.state.x = blockBox.xm + playerdata.structural.r + 0.05f;
                    }
                }
                else if (minOverlap == overlapY)
                {
                    // Y轴碰撞
                    if (playerBox.y0 < blockBox.y0)
                    {
                        // 从后面碰撞
                        playerdata.detection.ymcollision = true;
                        playerdata.state.y = blockBox.y0 - playerdata.structural.r - 0.05f;
                    }
                    else
                    {
                        // 从前面碰撞
                        playerdata.detection.ypcollision = true;
                        playerdata.state.y = blockBox.ym + playerdata.structural.r + 0.05f;
                    }
                }
                else
                {
                    // H轴碰撞（垂直）
                    if (playerBox.h0 < blockBox.h0)
                    {
                        // 从下方碰撞（头顶）
                        playerdata.detection.upcollosion = true;
                        playerdata.state.h = blockBox.h0 - playerdata.structural.h - 0.05f;
                        playerdata.state.vh = 0.0f; // 清零垂直速度
                    }
                    else
                    {
                        // 从上方碰撞（地面）
                        playerdata.detection.isonground = true;
                        playerdata.state.h = blockBox.hm + playerdata.structural.h + 0.05f;
                        playerdata.state.vh = 0.0f; // 清零垂直速度
                    }
                }
            }
        }
    }
}
/*
    对于此处AABB判定的说明：
      事实上这部分算法使用了基于AABB判定思想的简化版算法，因为目标功能可以使用简化版算法实现。
      仍然使用AABB模型，是考虑到后续可能的功能更新：
      如果后续新增了生物体，物理碰撞体等独立碰撞体，基于AABB的自然地形接触判定将更有利于于这些功能对接
       （因为彻底抛弃AABB算法将不能实现这些功能）
      为了兼顾计算速度与可扩展性，这里使用简化版的AABB判定算法。
                                                             ——来自开发者：浪兮  2025.12.1  02:01 AM

// (v0.47)对AABB算法的更新说明：
    经过测试，发现原有的简化版AABB算法在某些情况下会出现误判(垂直方向碰撞未形成有效重叠，碰撞检测无触发）
    因此对AABB算法进行了更新，改为更为标准的AABB碰撞检测算法
    该算法通过检测玩家AABB盒与方块AABB盒在所有轴上的重叠情况来判定碰撞
    并通过比较玩家与方块中心位置来确定具体的碰撞方向
    该更新提升了碰撞检测的准确性，避免了误判问题
    同时仍然保持了较高的计算效率，适合实时物理模拟需求
                                                             ——来自开发者：多茵  2025.12.8  00:08 AM
*/

// v3.19.1 新增：预测性碰撞检测函数
bool predictCollision(float dx, float dy, float dh)
{
    // 计算预测位置
    float predX = playerdata.state.x + dx;
    float predY = playerdata.state.y + dy;
    float predH = playerdata.state.h + dh;

    // 获取玩家预测AABB
    fAABB predBox =
    {
        predX - playerdata.structural.r,
        predX + playerdata.structural.r,
        predY - playerdata.structural.r,
        predY + playerdata.structural.r,
        predH - playerdata.structural.h,
        predH + playerdata.structural.h
    };

    // 获取玩家周围的方块范围
    AABB playerArea = playerarea();
    int x_start = max(playerArea.x0 - 1, 0);
    int x_end = playerArea.xm + 1;
    int y_start = max(playerArea.y0 - 1, 0);
    int y_end = playerArea.ym + 1;
    int h_start = max(int(predBox.h0) - 1, 1);
    int h_end = min(int(predBox.hm) + 1, 50); // 假设最大高度50

    // 检查预测位置是否与任何方块碰撞
    for (int x = x_start; x <= x_end; x++)
    {
        for (int y = y_start; y <= y_end; y++)
        {
            for (int h = h_start; h <= h_end; h++)
            {
                if (findtype(x, y, h) == Empty) continue;

                // 方块AABB
                fAABB blockBox =
                {
                    (float)x, (float)x + 1.0f,
                    (float)y, (float)y + 1.0f,
                    (float)h, (float)h + 1.0f
                };

                // AABB重叠检测
                bool x_overlap = (predBox.x0 < blockBox.xm) && (predBox.xm > blockBox.x0);
                bool y_overlap = (predBox.y0 < blockBox.ym) && (predBox.ym > blockBox.y0);
                bool h_overlap = (predBox.h0 < blockBox.hm) && (predBox.hm > blockBox.h0);

                if (x_overlap && y_overlap && h_overlap)
                {
                    return true; // 预测到碰撞
                }
            }
        }
    }

    return false; // 无碰撞
}

// v3.19.1智能爬升函数
void smartAutoStepUp()
{
    // 1. 基本条件检查
    if (!playerdata.detection.isonground)
    {
        return;
    }

    if (!playerdata.control.autostepup)
    {
        return;
    }

    // 2. 获取移动方向
    float moveX = 0.0f, moveY = 0.0f;

    // 基于按键输入
    if (playerdata.interaction.frontrunning)
    {
        moveX += sin(playerdata.orientation.sighta);
        moveY += cos(playerdata.orientation.sighta);
    }
    if (playerdata.interaction.backrunning)
    {
        moveX -= sin(playerdata.orientation.sighta);
        moveY -= cos(playerdata.orientation.sighta);
    }
    if (playerdata.interaction.sightleftrunning)
    {
        moveX -= cos(playerdata.orientation.sighta);
        moveY += sin(playerdata.orientation.sighta);
    }
    if (playerdata.interaction.sightrightrunning)
    {
        moveX += cos(playerdata.orientation.sighta);
        moveY -= sin(playerdata.orientation.sighta);
    }

    // 归一化
    float moveLen = sqrt(moveX * moveX + moveY * moveY);
    if (moveLen < 0.001f)
    {
        return; // 没有有效的移动方向
    }
    moveX /= moveLen;
    moveY /= moveLen;

    // 3. 检查当前移动是否会导致碰撞
    float moveSpeed = 0.1f; // 移动速度
    float predDX = moveX * moveSpeed;
    float predDY = moveY * moveSpeed;

    // 检查水平移动是否会碰撞
    if (!predictCollision(predDX, predDY, 0.0f))
    {
        return; // 不会碰撞，不需要爬升
    }

    // 4. 尝试不同的爬升高度
    const int maxStepChecks = 10;
    const float maxStepHeight = 1.0f; // 最大爬升1个方块
    float bestHeight = 0.0f;

    for (int i = 1; i <= maxStepChecks; i++)
    {
        float testHeight = (float)i * (maxStepHeight / maxStepChecks);

        // 检查在这个高度移动是否会碰撞
        if (!predictCollision(predDX, predDY, testHeight))
        {
            bestHeight = testHeight;

            // 还需要检查爬升后头顶是否有空间
            float newHeadHeight = playerdata.state.h + testHeight + playerdata.structural.h;
            if (!predictCollision(0, 0, testHeight + 0.1f)) // 检查头顶空间
            {
                break; // 找到合适高度
            }
        }
    }

    // 5. 执行爬升
    if (bestHeight > 0.05f && bestHeight <= maxStepHeight)
    {
        // 提升玩家高度
        playerdata.state.h += bestHeight;

        // 向前移动（确保跨过障碍）
        playerdata.state.x += moveX * 0.15f;
        playerdata.state.y += moveY * 0.15f;

        // 重置物理状态
        playerdata.state.vh = 0.0f;
        playerdata.state.ah = 0.0f;
        playerdata.detection.isonground = true;
        playerdata.interaction.jumping = false;

        // 播放音效
        //playSoundEffect("step.wav");

        // 调试输出
        static int stepCount = 0;
        if (stepCount++ < 10)
        {
            cout << "自动爬升执行！高度: " << bestHeight
                << " 方向: (" << moveX << ", " << moveY << ")" << endl;
        }

        return;
    }
}

//面定位数据
struct face
{
    unsigned x, y;
    int h;
    int facecode;
};

//判定区缓存
face detectionarea;

//跳跃判定
void jumpdetection()
{
    if ((((playerdata.detection.xmcollision
        || playerdata.detection.xpcollision
        || playerdata.detection.ymcollision
        || playerdata.detection.ypcollision)
        && playerdata.control.walljump)
        || playerdata.detection.isonground)
        && ((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
        && !playerdata.interaction.jumping)
    {
        playerdata.interaction.jumping = 1;
        if (!playerdata.detection.isonground)
        {
            if (playerdata.detection.xmcollision ^ playerdata.detection.xpcollision)
            {
                if (playerdata.detection.ymcollision ^ playerdata.detection.ypcollision)
                {
                    playerdata.state.vx += sqrt(2) * (playerdata.detection.xmcollision ? 1 : -1) * playerdata.parameters.wjbs;
                    playerdata.state.vy += sqrt(2) * (playerdata.detection.ymcollision ? 1 : -1) * playerdata.parameters.wjbs;
                }
                else
                {
                    playerdata.state.vx += (playerdata.detection.xmcollision ? 1 : -1) * playerdata.parameters.wjbs;
                }
            }
            else
            {
                if (playerdata.detection.ymcollision ^ playerdata.detection.ypcollision)
                {
                    playerdata.state.vy += (playerdata.detection.ymcollision ? 1 : -1) * playerdata.parameters.wjbs;
                }
            }
        }
        int facecode = (playerdata.detection.isonground
            || (playerdata.detection.xmcollision
                && playerdata.detection.xpcollision)
            || (playerdata.detection.ymcollision
                && playerdata.detection.ypcollision))
            ? U : (playerdata.detection.xmcollision
                ? F : (playerdata.detection.xpcollision
                    ? B : (playerdata.detection.ymcollision
                        ? R : (playerdata.detection.ypcollision
                            ? L : 0b1000))));
        if (facecode >> 3)
        {
            playerdata.interaction.jumping = 0;
        }
        else
        {
            detectionarea =
            {
                unsigned(playerdata.state.x)
                + ((facecode >> 2)
                    ? 0 : ((facecode >> 1)
                        ? ((facecode & 2)
                            ? -1 : 1) : 0)),
                unsigned(playerdata.state.y)
                + ((facecode >> 2)
                    ? 0 : ((facecode >> 1)
                        ? 0 : ((facecode & 2)
                            ? -1 : 1))),
                int(playerdata.state.h) - (facecode >> 2),facecode
            };
        }
    }
    if (playerdata.interaction.jumping
        && (playerdata.detection.upcollosion
            || (detectionarea.facecode == U
                ? (playerdata.state.h > detectionarea.h + playerdata.structural.sj)
                : (detectionarea.facecode == B
                    ? (playerdata.state.x > detectionarea.x + playerdata.structural.sj)
                    : (detectionarea.facecode == F
                        ? (playerdata.state.x < detectionarea.x - playerdata.structural.sj)
                        : (detectionarea.facecode == L
                            ? (playerdata.state.y > detectionarea.y + playerdata.structural.sj)
                            : (detectionarea.facecode == R
                                ? (playerdata.state.y < detectionarea.y - playerdata.structural.sj)
                                : 0)))))))
    {
        playerdata.interaction.jumping = 0;
    }
}


//移动侦测
void movingdetection()
{
    // v3.03.1更新：重置移动状态
    playerdata.interaction.frontrunning = false;
    playerdata.interaction.backrunning = false;
    playerdata.interaction.sightleftrunning = false;
    playerdata.interaction.sightrightrunning = false;

    // 同时支持 WASD 和方向键
    // W/上箭头 = 前，S/下箭头 = 后，A/左箭头 = 左，D/右箭头 = 右

    // 向前移动
    if ((GetAsyncKeyState('W') & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_UP) & 0x8000) != 0)
    {
        playerdata.interaction.frontrunning = true;
    }

    // 向后移动
    if ((GetAsyncKeyState('S') & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0)
    {
        playerdata.interaction.backrunning = true;
    }

    // 向左移动（左滑步/左转）
    if ((GetAsyncKeyState('A') & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0)
    {
        playerdata.interaction.sightleftrunning = true;
    }

    // 向右移动（右滑步/右转）
    if ((GetAsyncKeyState('D') & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0)
    {
        playerdata.interaction.sightrightrunning = true;
    }
}

//爬梯侦测
void climbdetection()
{
    // ========== 修复：精准攀爬判定（2025.12.9） ==========
    // 1. 清空原有简单判定，改为精准条件
    playerdata.interaction.climbing = false;

    // 2. 判定条件：必须贴墙 + 对应方向输入 + 非地面 + 无头顶碰撞
    bool is_near_wall =
        (
            playerdata.detection.xmcollision
            || playerdata.detection.xpcollision
            || playerdata.detection.ymcollision
            || playerdata.detection.ypcollision
            );
    bool has_climb_input =
        (
            playerdata.interaction.frontrunning
            || playerdata.interaction.backrunning
            || playerdata.interaction.sightleftrunning
            || playerdata.interaction.sightrightrunning
            );
    bool is_not_ground = !playerdata.detection.isonground;
    bool no_ceiling = !playerdata.detection.upcollosion;

    // 3. 精准判定：仅当所有条件满足时，才判定为攀爬
    if (is_near_wall && has_climb_input && is_not_ground && no_ceiling)
    {
        playerdata.interaction.climbing = true;
        // 额外：攀爬时降低水平速度，避免穿墙
        playerdata.state.vx *= 0.5;
        playerdata.state.vy *= 0.5;
    }
    // ========== 攀爬判定修复结束 ==========
}

//自动跃升
void autoleap()
{
    if (playerdata.interaction.climbing && !playerdata.detection.upcollosion)
    {
        playerdata.state.vh += playerdata.parameters.vleap;
    }
}

// ========== 新增：坐标映射修复函数（2025.12.9） ==========
// 物理坐标转世界坐标（修正偏移/缩放）
double phys_to_world(double phys_coord, double offset, double scale)
{
    return (phys_coord - offset) / scale;
}

// 世界坐标转物理坐标（防溢出+范围限制）
double world_to_phys(double world_coord, double offset, double scale, double max_range)
{
    double phys = world_coord * scale + offset;
    // 限制坐标范围，避免溢出/穿透
    return fmin(fmax(phys, -max_range), max_range);
}

// 默认坐标转换参数（可通过引导修改）
const double PHYS_OFFSET = 0.0;    // 物理坐标偏移
const double PHYS_SCALE = 1.0;     // 物理坐标缩放
const double PHYS_MAX_RANGE = 1000000.0; // 坐标最大范围
// ========== 坐标映射修复结束 ==========

//生物传送
void teleport(unsigned x, unsigned y, int h)
{
    // 修复：传送时使用坐标转换，避免偏移
    playerdata.state.x = world_to_phys(x, PHYS_OFFSET, PHYS_SCALE, PHYS_MAX_RANGE);
    playerdata.state.y = world_to_phys(y, PHYS_OFFSET, PHYS_SCALE, PHYS_MAX_RANGE);
    playerdata.state.h = world_to_phys(h, PHYS_OFFSET, PHYS_SCALE, PHYS_MAX_RANGE);

    // 额外：重置速度，避免传送后惯性穿透
    playerdata.state.vx = 0.0;
    playerdata.state.vy = 0.0;
    playerdata.state.vh = 0.0;
}

//手动传送引导
void helperofteleport()
{
    cout << "[来自：手动传送引导]我们需要2个U，1个int，分别作为：x，y，h" << endl;
    unsigned a, b;
    int c;
    cin >> a >> b >> c;
    teleport(a, b, c);
    cout << "[来自：手动传送引导]已经将" << playerdata.player.name << "传送至" << "(" << a << "," << b << ") h=" << c << " 引导退出" << endl;
}

//防卡传送
void antisuckteleport()
{
    int x = playerdata.state.x + int(cos(sin(cos(playerdata.state.y) + playerdata.state.h) - playerdata.state.x) * 3);
    int y = playerdata.state.y + int(sin(cos(sin(playerdata.state.y) + playerdata.state.h) - playerdata.state.x) * 3);
    for (int i = 300; i > 2; i--)
    {
        if (findtype(x, y, i) == 0 && findtype(x, y, i - 1) == 0 && findtype(x, y, i - 2) != 0)
        {
            teleport(x, y, i + 3);
            return;
        }
    }
}

//防卡传送引导
void helperofantisuckteleport()
{
    cout << "[来自：防卡传送引导]1.确认防卡传送 2.取消操作" << endl;
    int c;
    cin >> c;
    if (c == 1)
    {
        antisuckteleport();
        cout << "[来自：防卡传送引导]对" << playerdata.player.name << "防卡传送成功。 引导退出" << endl;
    }
    else
    {
        cout << "[来自：防卡传送引导]";
        if (c == 0)
        {
            cout << "取消成功。 ";
        }
        cout << "引导退出" << endl;
    }

}

//虚空传送
void voidteleport()
{
    if (playerdata.state.h < -50)
    {
        antisuckteleport();
    }
}

//主动防卡
void unstuck()
{
    if ((GetAsyncKeyState(VK_BACK) & 0x8000) != 0)
    {
        countnum = 0;
        int timer = 0;
        while (countnum < 7 && timer < 3000)
        {
            timer++;
            if ((GetAsyncKeyState('U') & 0x8000) != 0 && (countnum == 0 || countnum == 4))
            {
                countnum++;
            }
            if ((GetAsyncKeyState('N') & 0x8000) != 0 && countnum == 1)
            {
                countnum++;
            }
            if ((GetAsyncKeyState('S') & 0x8000) != 0 && countnum == 2)
            {
                countnum++;
            }
            if ((GetAsyncKeyState('T') & 0x8000) != 0 && countnum == 3)
            {
                countnum++;
            }
            if ((GetAsyncKeyState('C') & 0x8000) != 0 && countnum == 5)
            {
                countnum++;
            }
            if ((GetAsyncKeyState('K') & 0x8000) != 0 && countnum == 6)
            {
                countnum++;
            }
        }
        if (countnum == 7)
        {
            antisuckteleport();
        }
    }
}

//from：工具栏。被放置方块函数"和物理输入函数"使用
int toolchoice = 0;

// 新增：穿透修复函数（保险机制）
void fixAnyPenetration()
{
    // 获取玩家AABB
    fAABB playerBox = playerAABB();

    // 检查周围方块
    AABB area = playerarea();

    for (int x = area.x0 - 1; x <= area.xm + 1; x++)
    {
        for (int y = area.y0 - 1; y <= area.ym + 1; y++)
        {
            for (int h = max(1, (int)playerBox.h0 - 1); h <= (int)playerBox.hm + 1; h++)
            {
                if (findtype(x, y, h) == Empty)
                {
                    continue;
                }

                // 方块AABB
                fAABB blockBox =
                {
                    (float)x, (float)x + 1.0f,
                    (float)y, (float)y + 1.0f,
                    (float)h, (float)h + 1.0f
                };

                // 检查重叠
                bool x_overlap = (playerBox.x0 < blockBox.xm) && (playerBox.xm > blockBox.x0);
                bool y_overlap = (playerBox.y0 < blockBox.ym) && (playerBox.ym > blockBox.y0);
                bool h_overlap = (playerBox.h0 < blockBox.hm) && (playerBox.hm > blockBox.h0);

                if (x_overlap && y_overlap && h_overlap)
                {
                    // 发生穿透，推出来
                    float pushOut = 0.05f;

                    // 从哪个方向推出来
                    float leftPush = playerBox.xm - blockBox.x0;
                    float rightPush = blockBox.xm - playerBox.x0;
                    float frontPush = playerBox.ym - blockBox.y0;
                    float backPush = blockBox.ym - playerBox.y0;

                    // 找出最小推离距离
                    if (leftPush < rightPush && leftPush < frontPush && leftPush < backPush)
                    {
                        playerdata.state.x += pushOut;
                    }
                    else if (rightPush < leftPush && rightPush < frontPush && rightPush < backPush)
                    {
                        playerdata.state.x -= pushOut;
                    }
                    else if (frontPush < leftPush && frontPush < rightPush && frontPush < backPush)
                    {
                        playerdata.state.y += pushOut;
                    }
                    else
                    {
                        playerdata.state.y -= pushOut;
                    }

                    // 调试输出
                    static int fixCount = 0;
                    if (fixCount++ < 10)
                    {
                        cout << "穿透修复执行！位置: ("
                            << playerdata.state.x
                            << ", "
                            << playerdata.state.y
                            << ")"
                            << endl;
                    }
                }
            }
        }
    }
}

// v3.19.1新增：温和的碰撞修正（避免剧烈抖动）
void gentleCollisionCorrection()
{
    // 只检查水平方向的碰撞
    if (playerdata.detection.xmcollision
        || playerdata.detection.xpcollision
        || playerdata.detection.ymcollision
        || playerdata.detection.ypcollision
        )
    {

        // 获取玩家AABB
        fAABB playerBox = playerAABB();

        // 非常轻微的推离（避免抖动）
        const float gentlePush = 0.02f;

        if (playerdata.detection.xmcollision)
        {
            playerdata.state.x += gentlePush;
        }
        if (playerdata.detection.xpcollision)
        {
            playerdata.state.x -= gentlePush;
        }
        if (playerdata.detection.ymcollision)
        {
            playerdata.state.y += gentlePush;
        }
        if (playerdata.detection.ypcollision)
        {
            playerdata.state.y -= gentlePush;
        }

        // 清除碰撞标记，避免重复处理
        playerdata.detection.xmcollision = false;
        playerdata.detection.xpcollision = false;
        playerdata.detection.ymcollision = false;
        playerdata.detection.ypcollision = false;
    }
}

//暂时注销掉射线检测。

/*

// v3.30.1 更新：从相机位置发射射线进行检测
// 完全重写的射线检测函数（简化版，更容易调试）
// 修正后的射线检测函数
RaycastHit raycastFromCamera()
{
    RaycastHit result;
    result.hit = false;
    result.distance = MAX_RAY_DISTANCE;

    // ========== 1. 获取相机（玩家眼睛）位置 ==========

    double eyeHeight = playerdata.structural.h * 0.9;  // 修正眼睛高度


    double camX = playerdata.state.x;
    double camY = playerdata.state.y;
    double camZ = playerdata.state.h + eyeHeight;  // 眼睛高度从脚部算起

    // ========== 2. 获取视线方向 ==========
    double yaw = viewsetup.sighta;     // 水平旋转角（弧度）
    double pitch = -viewsetup.sightb;   // 垂直俯仰角（弧度）

    // 不标准的第一人称视角方向计算：
    double rayDirX = cos(yaw) * sin(pitch);
    double rayDirY = sin(yaw) * cos(pitch);
    double rayDirZ = sin(pitch);

    // 归一化方向向量
    double length = sqrt(rayDirX * rayDirX + rayDirY * rayDirY + rayDirZ * rayDirZ);
    if (length > 0.0001)
    {
        rayDirX /= length;
        rayDirY /= length;
        rayDirZ /= length;
    }

    // ========== 3. 调试信息 ==========
    static int debugCount = 0;
    if (debugCount < 10) {
        cout << "\n[射线调试] ==== 第 "
            << debugCount
            << " 次检测 ===="
            << endl;
        cout << "[射线调试] 玩家位置: ("
            << playerdata.state.x
            << ", "
            << playerdata.state.y
            << ", "
            << playerdata.state.h
            << ")"
            << endl;
        cout << "[射线调试] 玩家高度: "
            << playerdata.structural.h
            << endl;
        cout << "[射线调试] 眼睛高度: "
            << eyeHeight
            << endl;
        cout << "[射线调试] 相机位置: ("
            << camX
            << ", "
            << camY
            << ", "
            << camZ
            << ")"
            << endl;
        cout << "[射线调试] 视角角度: yaw="
            << yaw
            << " rad ("
            << yaw * 180 / 3.1416
            << "°), "
            << "pitch="
            << pitch
            << " rad ("
            << pitch * 180 / 3.1416
            << "°)"
            << endl;
        cout << "[射线调试] 射线方向: ("
            << rayDirX
            << ", "
            << rayDirY
            << ", "
            << rayDirZ
            << ")"
            << endl;
        debugCount++;
    }

    // ========== 4. 射线步进检测 ==========
    const double step = 0.1;  // 步长
    const int maxSteps = (int)(MAX_RAY_DISTANCE / step);

    for (int i = 1; i <= maxSteps; i++)
    {
        // 计算当前位置
        double currentX = camX + rayDirX * (i * step);
        double currentY = camY + rayDirY * (i * step);
        double currentZ = camZ + rayDirZ * (i * step);

        // 转换为方块坐标
        int blockX = (int)floor(currentX);
        int blockY = (int)floor(currentY);
        int blockZ = (int)floor(currentZ);

        // 检查这个方块是否非空
        if (blockX >= 0 && blockY >= 0 && blockZ >= 0)
        {
            int blockType = findtype(blockX, blockY, blockZ);

            if (blockType != Empty)
            {
                // 找到方块！
                result.hit = true;
                result.blockX = blockX;
                result.blockY = blockY;
                result.blockH = blockZ;
                result.blockType = blockType;
                result.distance = i * step;

                // 确定命中的面
                double inBlockX = currentX - blockX;
                double inBlockY = currentY - blockY;
                double inBlockZ = currentZ - blockZ;

                // 找出从哪个面进入方块
                double distToLeft = inBlockX;
                double distToRight = 1.0 - inBlockX;
                double distToBack = inBlockY;
                double distToFront = 1.0 - inBlockY;
                double distToBottom = inBlockZ;
                double distToTop = 1.0 - inBlockZ;

                double minDist = min({
                    distToLeft,
                    distToRight,
                    distToBack,
                    distToFront,
                    distToBottom,
                    distToTop
                    });

                if (minDist == distToLeft) result.face = 1;    // 左面（X-）
                else if (minDist == distToRight) result.face = 0;  // 右面（X+）
                else if (minDist == distToBack) result.face = 3;   // 后面（Y-）
                else if (minDist == distToFront) result.face = 2;  // 前面（Y+）
                else if (minDist == distToBottom) result.face = 5; // 底面（Z-）
                else if (minDist == distToTop) result.face = 4;    // 顶面（Z+）

                if (debugCount <= 10)
                {
                    cout << "[射线调试] ✅ 命中方块: (" << blockX << ", " << blockY << ", " << blockZ << ")" << endl;
                    cout << "[射线调试] 方块类型: " << blocktypename[blockType] << endl;
                    cout << "[射线调试] 命中距离: " << result.distance << endl;
                    cout << "[射线调试] 在方块内位置: (" << inBlockX << ", " << inBlockY << ", " << inBlockZ << ")" << endl;
                }

                return result;
            }
        }
    }

    if (debugCount <= 10)
    {
        cout << "[射线调试] ❌ 未命中任何方块" << endl;
    }

    return result;
}

*/

// 复用已有的 isInsidePlayer 函数
bool isInsidePlayer(int x, int y, int h)
{
    float blockCenterX = x + 0.5f;
    float blockCenterY = y + 0.5f;
    float blockCenterH = h + 0.5f;

    float playerX = playerdata.state.x;
    float playerY = playerdata.state.y;
    float playerH = playerdata.state.h;
    float playerR = playerdata.structural.r;
    float playerHeight = playerdata.structural.h;

    // 玩家碰撞箱边界
    float playerMinX = playerX - playerR;
    float playerMaxX = playerX + playerR;
    float playerMinY = playerY - playerR;
    float playerMaxY = playerY + playerR;
    float playerMinH = playerH - playerHeight;
    float playerMaxH = playerH + playerHeight;

    // 方块边界
    float blockMinX = blockCenterX - 0.5f;
    float blockMaxX = blockCenterX + 0.5f;
    float blockMinY = blockCenterY - 0.5f;
    float blockMaxY = blockCenterY + 0.5f;
    float blockMinH = blockCenterH - 0.5f;
    float blockMaxH = blockCenterH + 0.5f;

    // 检查重叠
    return (blockMinX < playerMaxX && blockMaxX > playerMinX &&
        blockMinY < playerMaxY && blockMaxY > playerMinY &&
        blockMinH < playerMaxH && blockMaxH > playerMinH);
}


// 新增函数：破坏脚底正下方的方块
void destroyBlockDirectlyBelow()
{
    // 获取玩家当前位置的整数坐标
    int playerX = (int)floor(playerdata.state.x);
    int playerY = (int)floor(playerdata.state.y);

    // 计算玩家脚底的高度（精确到方块坐标）
    // 玩家脚底位置 = playerdata.state.h - playerdata.structural.h
    float footHeight = playerdata.state.h - playerdata.structural.h;

    // 脚底正下方的方块高度 = floor(footHeight - 0.5)
    // -0.5是因为方块的中心在方块中间，我们需要方块的下表面
    int blockH = (int)floor(footHeight - 0.5f);

    // 安全检查：防止破坏基岩层（高度1以下）
    if (blockH < 1)
    {
        blockH = 1;
        cout << "[快捷破坏] 警告：已到达基岩层，无法继续向下破坏" << endl;
        return;
    }

    // 检查方块类型
    int blockType = findtype(playerX, playerY, blockH);
    if (blockType != Empty && blockType != 15) // 15是基岩，防止破坏
    {
        // 破坏方块
        updateblock(playerX, playerY, blockH, Empty);
        cout << "[快捷破坏] 破坏了脚底正下方的方块：("
            << playerX << ", " << playerY << ", " << blockH
            << ") 类型：" << blocktypename[blockType] << endl;

        // 播放音效
        //playSoundEffect("C8_G7_E7_C7.wav");

        // 如果玩家站在这个方块上，需要调整玩家位置
        if (footHeight <= blockH + 1.5f) // 如果脚底接近这个方块顶部
        {
            // 稍微提升玩家高度，防止掉下去
            playerdata.state.h += 0.1f;
        }
    }
    else
    {
        cout << "[快捷破坏] 脚底下方是空气或基岩，无需破坏" << endl;
    }
}

// 新增函数：在脚底正下方放置方块
void placeBlockDirectlyBelow()
{
    // 获取玩家当前位置的整数坐标
    int playerX = (int)floor(playerdata.state.x);
    int playerY = (int)floor(playerdata.state.y);

    // 计算脚底正下方可以放置的位置
    // 玩家脚底位置
    float footHeight = playerdata.state.h - playerdata.structural.h;

    // 我们需要放置在脚底正下方，所以是floor(footHeight - 0.5)
    int placeH = (int)floor(footHeight - 0.5f);

    // 如果这个位置已经有方块，尝试向下一个位置
    int originalPlaceH = placeH;
    int attempts = 0;

    while (attempts < 3) // 最多尝试3个位置
    {
        int blockType = findtype(playerX, playerY, placeH);

        if (blockType == Empty) // 找到空位了
        {
            // 检查是否在玩家体内（防止把自己困住）
            if (!isInsidePlayer(playerX, playerY, placeH))
            {
                // 获取要放置的方块类型（使用当前选择的工具）
                int placeType = toolchoice;
                if (placeType == 0)
                {
                    placeType = Grass; // 如果工具是空手，默认放置草方块
                }

                // 放置方块
                updateblock(playerX, playerY, placeH, placeType);

                cout << "[快捷放置] 在脚底正下方放置了方块：("
                    << playerX << ", " << playerY << ", " << placeH
                    << ") 类型：" << blocktypename[placeType] << endl;

                if (placeH != originalPlaceH)
                {
                    cout << "[快捷放置] 注意：原位置(" << originalPlaceH
                        << ")已有方块，已放置在下层(" << placeH << ")" << endl;
                }

                // 播放音效
                //playSoundEffect("C7_E7_G7_C8.wav");

                // 如果放置后玩家被困（方块紧贴脚底），自动调整玩家高度
                float newFootHeight = playerdata.state.h - playerdata.structural.h;
                if (newFootHeight <= placeH + 1.0f + 0.1f) // 留一点间隙
                {
                    playerdata.state.h = placeH + 1.5f + playerdata.structural.h;
                    cout << "[快捷放置] 自动调整玩家高度避免被困" << endl;
                }

                return; // 放置成功，退出函数
            }
            else
            {
                cout << "[快捷放置] 警告：目标位置在玩家体内，无法放置" << endl;
                return;
            }
        }

        // 这个位置有方块，尝试下一个更低的位置
        placeH--;
        attempts++;
    }

    cout << "[快捷放置] 失败：脚底下方没有合适的空位" << endl;
}


/*

//v3.30.1 方块放置和破坏函数预声明
void breakBlock();
void placeBlock();

*/

// 3.33物理输入函数
void physicalinput()
{
    movingdetection();
    jumpdetection();
    climbdetection();
    unstuck();

    // 检测模式切换
    checkModeToggle();


    // 只在建造模式下允许方块交互
    if (currentGameMode == BUILD_MODE)
    {
        // 左键破坏
        static bool leftMousePressed = false;
        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0)
        {
            if (!leftMousePressed)
            {
                if (toolchoice == 0)//自动识别工具类型
                {
                    //                    breakBlock();
                }
                else
                {
                    //                    placeBlock();
                }
                leftMousePressed = true;
            }
        }
        else
        {
            leftMousePressed = false;
        }

        /* 新增：B键破坏脚底正下方的方块
        static bool bKeyPressed = false;
        if ((GetAsyncKeyState('B') & 0x8000) != 0)
        {
            if (!bKeyPressed)
            {
                destroyBlockDirectlyBelow();
                bKeyPressed = true;
            }
        }
        else
        {
            bKeyPressed = false;
        }

        // 新增：N键在脚底正下方放置方块
        static bool nKeyPressed = false;
        if ((GetAsyncKeyState('N') & 0x8000) != 0)
        {
            if (!nKeyPressed)
            {
                placeBlockDirectlyBelow();
                nKeyPressed = true;
            }
        }
        else
        {
            nKeyPressed = false;
        }

        */



        //右键放置被注销，但是其状态变量被保留？？我相信我们电脑的内存够用。对不起
        //（其实是我试图找到但失败。 ——浪兮 2025.12.24 03:02 AM）

        /* 右键放置
        static bool rightMousePressed = false;
        if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0)
        {
            if (!rightMousePressed)
            {
                placeBlock();
                rightMousePressed = true;
            }
        }
        else
        {
            rightMousePressed = false;
        }
        */
        //修改说明：
          //from浪兮
         /*
             更改为自动识别地形编辑模式。
             by自动读取玩家拿起的工具编号（索引index），
             具体地，该索引存储在全局变量
                 toolchoice
              中。
             when工具被拿起
             （
               具体地，这一状态被存储在全局变量
                 usetool
               中。
             ）
            index对应的工具类型，没有标示，因为这实在是太明显了！我们直接将它写死在代码里，大抵也并不会影响阅读，并更加简洁。
              综合考虑，我对3.33版进行相应更新。以上作为更新说明。
    ——开发者：浪兮  2025.12.24 3:01 AM
         */
    }


}


/*
//物理输入
void physicalinput()
{
    movingdetection();
    jumpdetection();
    climbdetection();
    unstuck();


    // 更新：v3.30.1 准星交互（鼠标左右键）
    static bool leftMousePressed = false;
    static bool rightMousePressed = false;

    // 左键破坏
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0) {
        if (!leftMousePressed) {
            breakBlock();
            leftMousePressed = true;
        }
    }
    else {
        leftMousePressed = false;
    }

    // 右键放置
    if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0) {
        if (!rightMousePressed) {
            placeBlock();
            rightMousePressed = true;
        }
    }
    else {
        rightMousePressed = false;
    }

    // F键切换调试信息
    static bool fKeyPressed = false;
    if ((GetAsyncKeyState('F') & 0x8000) != 0) {
        if (!fKeyPressed) {
            showCrosshairDebug = !showCrosshairDebug;
            fKeyPressed = true;
        }
    }
    else {
        fKeyPressed = false;
    }

}*/

//物理动力
void physicaldynamic()
{
    refreshara();
    dynamicloading();
    gravity();       // 仅在未跳跃/未地面时施加重力
    resistance();    // 阻力（不覆盖速度清零）
    autoleap();
}


//物理检测
void physicsanomalydetection()
{
    voidteleport();
}

/* 3.33  跑酷效果定义
struct ParkourEffect
{
    float speedMultiplier;   // 速度倍率
    float jumpMultiplier;    // 跳跃倍率
    float damage;           // 伤害值
    bool bounce;            // 是否弹跳
    float bounceHeight;     // 弹跳高度
};

// 不同颜色的方块效果
unordered_map<int, ParkourEffect> parkourEffects =
{
    // 红色方块：加速
    {Red, {1.5f, 1.0f, 0.0f, false, 0.0f}},

    // 蓝色方块：减速
    {Blue, {0.5f, 1.0f, 0.0f, false, 0.0f}},

    // 绿色方块：弹跳
    {Green, {1.0f, 2.0f, 0.0f, true, 1.5f}},

    // 黄色方块：冲刺（短暂高速）
    {Yellow, {2.0f, 1.0f, 0.0f, false, 0.0f}},

    // 紫色方块：伤害
    {Purple, {1.0f, 1.0f, 5.0f, false, 0.0f}},

    // 橙色方块：恢复
    {Orange, {1.0f, 1.0f, -2.0f, false, 0.0f}}, // 负伤害 = 恢复
};


// 应用跑酷效果
void applyParkourEffect(const ParkourEffect& effect, int blockType)
{
    static unordered_map<int, DWORD> lastEffectTime; // 记录上次应用时间（防重复）
    DWORD currentTime = GetTickCount();

    // 防止同一效果连续触发（至少间隔500ms）
    if (lastEffectTime.find(blockType) != lastEffectTime.end())
    {
        if (currentTime - lastEffectTime[blockType] < 500) return;
    }

    // 应用效果
    if (effect.speedMultiplier != 1.0f)
    {
        playerdata.state.vx *= effect.speedMultiplier;
        playerdata.state.vy *= effect.speedMultiplier;
        cout << "[跑酷] " << blocktypename[blockType] << " 效果：速度x" << effect.speedMultiplier << endl;
    }

    if (effect.jumpMultiplier != 1.0f)
    {
        playerdata.state.vh *= effect.jumpMultiplier;
        cout << "[跑酷] " << blocktypename[blockType] << " 效果：跳跃x" << effect.jumpMultiplier << endl;
    }

    if (effect.bounce)
    {
        playerdata.state.vh = effect.bounceHeight;
        cout << "[跑酷] " << blocktypename[blockType] << " 效果：弹跳！" << endl;
        playSoundEffect("bounce.wav");
    }

    if (effect.damage != 0.0f)
    {
        // 这里可以添加生命值系统
        if (effect.damage > 0)
        {
            cout << "[跑酷] " << blocktypename[blockType] << " 效果：受到" << effect.damage << "伤害" << endl;
            playSoundEffect("damage.wav");
        }
        else
        {
            cout << "[跑酷] " << blocktypename[blockType] << " 效果：恢复" << -effect.damage << "生命" << endl;
            playSoundEffect("heal.wav");
        }
    }

    // 记录应用时间
    lastEffectTime[blockType] = currentTime;
}

*/

//更新了！

/* 检查侧面碰撞效果
void checkSideEffects()
{
    // 检查玩家周围的方块
    AABB playerArea = playerarea();

    for (int x = playerArea.x0; x <= playerArea.xm; x++)
    {
        for (int y = playerArea.y0; y <= playerArea.ym; y++)
        {
            for (int h = playerArea.h0; h <= playerArea.hm; h++)
            {
                int blockType = findtype(x, y, h);
                if (blockType != Empty)
                {
                    auto it = parkourEffects.find(blockType);
                    if (it != parkourEffects.end())
                    {
                        // 简单距离检测
                        float dx = fabs(x + 0.5f - playerdata.state.x);
                        float dy = fabs(y + 0.5f - playerdata.state.y);
                        float dh = fabs(h + 0.5f - playerdata.state.h);

                        if (dx < 1.5f && dy < 1.5f && dh < 2.0f)
                        {
                            applyParkourEffect(it->second, blockType);
                        }
                    }
                }
            }
        }
    }
}

// 检测玩家脚下的方块效果
void checkParkourEffects()
{
    if (currentGameMode != PARKOUR_MODE) return;

    // 获取玩家脚下的方块
    int playerX = (int)playerdata.state.x;
    int playerY = (int)playerdata.state.y;
    int groundH = height(playerX, playerY);

    // 玩家脚底高度
    float footHeight = playerdata.state.h - playerdata.structural.h;

    // 检查是否站在方块上
    if (fabs(footHeight - (groundH + 1.0f)) < 0.2f) {
        // 获取脚下的方块类型
        int blockType = findtype(playerX, playerY, groundH);

        // 检查是否有跑酷效果
        auto it = parkourEffects.find(blockType);
        if (it != parkourEffects.end())
        {
            applyParkourEffect(it->second, blockType);
        }
    }

    // 检查是否碰到侧面方块
    checkSideEffects();
}

*/

//物理引擎
/*void physicsupdate()
{
    // 正确时序：输入 → 碰撞检测 → 物理修正 → 动力更新 → 位置更新 → 异常检测
    physicalinput();          // 1.读取输入（移动/跳跃）
    contactdetection(playerarea()); // 2.先检测碰撞
    einteraction();           // 3.修正碰撞后的物理状态
    // ========== 新增：位置修正（2025.12.9） ==========
    // 强制限制玩家坐标在有效范围，避免越界
    playerdata.state.x = fmax(0.0, playerdata.state.x);
    playerdata.state.y = fmax(0.0, playerdata.state.y);
    playerdata.state.h = fmax(1.0, playerdata.state.h);
    // ========== 位置修正结束 ==========
    physicaldynamic();        // 4.处理重力/阻力/爬梯（不再覆盖ah）
    refreshkinematics();      // 5.更新位置/速度
    physicsanomalydetection();// 6.防卡/虚空检测
}*/

// v3.19.1 重构后的物理引擎
void physicsupdate()
{
    // ==== 稳定版物理更新 ====

   /* // 阶段1: 输入处理
    physicalinput();

    // 阶段2: 标准物理更新（先更新速度/位置）
    contactdetection();
    einteraction();
    physicaldynamic();
    refreshkinematics();

    // 阶段3: 爬升检测（在位置更新后，但作为独立步骤）
    // 注意：只在特定条件下尝试爬升
    if (playerdata.control.autostepup &&
        playerdata.detection.isonground &&
        !playerdata.interaction.jumping)
        {
        smartAutoStepUp();
    }

    // 阶段4: 轻微的位置修正（如果有爬升）
    // 重新检测碰撞，但只做轻微修正
    gentleCollisionCorrection();

    // 阶段5: 异常检测
    physicsanomalydetection();*/

    // v3.24.1 重构后的物理引擎

    // 阶段1: 输入处理
    physicalinput();



    // 阶段2: 预碰撞检测 - 在更新位置前检测
    if (predictCollision(playerdata.state.vx * playerdata.parameters.kt,
        playerdata.state.vy * playerdata.parameters.kt,
        playerdata.state.vh * playerdata.parameters.kt))
    {
        // 如果预测到碰撞，先处理碰撞
        contactdetection();
        // 根据碰撞方向限制速度
        if (playerdata.detection.xmcollision || playerdata.detection.xpcollision)
        {
            playerdata.state.vx = 0.0f;
        }
        if (playerdata.detection.ymcollision || playerdata.detection.ypcollision)
        {
            playerdata.state.vy = 0.0f;
        }
        if (playerdata.detection.upcollosion || playerdata.detection.isonground)
        {
            playerdata.state.vh = 0.0f;
        }
    }


    // 阶段3: 标准物理更新
    einteraction();
    physicaldynamic();
    refreshkinematics();

    // 阶段4: 后碰撞检测和修正
    contactdetection();

    // 阶段5: 再次修正位置（防止微小穿透）
    gentleCollisionCorrection();

    // 阶段6: 智能爬升（可选）
    if (playerdata.control.autostepup &&
        playerdata.detection.isonground &&
        !playerdata.interaction.jumping)
    {
        smartAutoStepUp();
    }


    /* 3.33新增：跑酷效果检测（在物理更新后）
    if (currentGameMode == PARKOUR_MODE)
    {
        checkParkourEffects();
    }

    if (playerdata.control.autostepup &&
        playerdata.detection.isonground &&
        !playerdata.interaction.jumping)
    {
        smartAutoStepUp();
    }

    */

    // 阶段7: 异常检测
    physicsanomalydetection();
}

//物理数据引导（预声明）
void helperofphysicsparameters();//预声明

//物理状态引导
void pp1(int helper)
{
    bool done = 0;
    if (helper == -1)
    {
        cout << "[来自：物理状态引导]physicalstate类型下的分类：0.所有 1.x 2.y 3.h 4.vx 5.vy 6.vh 7.ax 8.ay 9.ah" << endl;
        int n;
        cin >> n;
        pp1(n);
        done = 1;
    }
    if (helper == 0 || helper == 1)
    {
        cout << "[来自：物理状态引导]我们需要1个D，作为：x" << endl;
        double m;
        cin >> m;
        playerdata.state.x = m;
        done = 1;
    }
    if (helper == 0 || helper == 2)
    {
        cout << "[来自：物理状态引导]我们需要1个D，作为：y" << endl;
        double m;
        cin >> m;
        playerdata.state.y = m;
        done = 1;
    }
    if (helper == 0 || helper == 3)
    {
        cout << "[来自：物理状态引导]我们需要1个D，作为：h" << endl;
        double m;
        cin >> m;
        playerdata.state.h = m;
        done = 1;
    }
    if (helper == 0 || helper == 4)
    {
        cout << "[来自：物理状态引导]我们需要1个D，作为：vx" << endl;
        double m;
        cin >> m;
        playerdata.state.vx = m;
        done = 1;
    }
    if (helper == 0 || helper == 5)
    {
        cout << "[来自：物理状态引导]我们需要1个D，作为：vy" << endl;
        double m;
        cin >> m;
        playerdata.state.vy = m;
        done = 1;
    }
    if (helper == 0 || helper == 6)
    {
        cout << "[来自：物理状态引导]我们需要1个D，作为：vh" << endl;
        double m;
        cin >> m;
        playerdata.state.vh = m;
        done = 1;
    }
    if (helper == 0 || helper == 7)
    {
        cout << "[来自：物理状态引导]我们需要1个D，作为：ax" << endl;
        double m;
        cin >> m;
        playerdata.state.ax = m;
        done = 1;
    }
    if (helper == 0 || helper == 8)
    {
        cout << "[来自：物理状态引导]我们需要1个D，作为：ay" << endl;
        double m;
        cin >> m;
        playerdata.state.ay = m;
        done = 1;
    }
    if (helper == 0 || helper == 9)
    {
        cout << "[来自：物理状态引导]我们需要1个D，作为：ah" << endl;
        double m;
        cin >> m;
        playerdata.state.ah = m;
        done = 1;
    }
    if (done == 0)
    {
        cout << "[来自：物理状态引导]无效指令。引导退出" << endl;
    }
    else
    {
        cout << "[来自：物理状态引导]任务完成。引导退出" << endl;
    }
    helperofphysicsparameters();
}

//物理参数引导
void pp2(int helper)
{
    bool done = 0;
    if (helper == -1)
    {
        cout << "[来自：物理参数引导]physicalparameters类型下的分类：0.所有 1.ar(空气阻力) 2.fr(摩擦阻力) 3.g(重力加速度) 4.fj(跳跃力度) 5.frm(最大奔跑力度) 6.vm(最大速度) 7.kt(时间系数) 8.km(质量系数) 9.wjbs(墙跳反弹强度) 10.vleap(跃升速度)" << endl;
        int n;
        cin >> n;
        pp2(n);
        done = 1;
    }
    if (helper == 0 || helper == 1)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：ar" << endl;
        double m;
        cin >> m;
        playerdata.parameters.ar = m;
        done = 1;
    }
    if (helper == 0 || helper == 2)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：fr" << endl;
        double m;
        cin >> m;
        playerdata.parameters.fr = m;
        done = 1;
    }
    if (helper == 0 || helper == 3)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：g" << endl;
        double m;
        cin >> m;
        playerdata.parameters.g = m;
        done = 1;
    }
    if (helper == 0 || helper == 4)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：fj" << endl;
        double m;
        cin >> m;
        playerdata.parameters.fj = m;
        done = 1;
    }
    if (helper == 0 || helper == 5)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：frm" << endl;
        double m;
        cin >> m;
        playerdata.parameters.frm = m;
        done = 1;
    }
    if (helper == 0 || helper == 6)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：vm" << endl;
        double m;
        cin >> m;
        playerdata.parameters.vm = m;
        done = 1;
    }
    if (helper == 0 || helper == 7)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：kt" << endl;
        double m;
        cin >> m;
        playerdata.parameters.kt = m;
        done = 1;
    }
    if (helper == 0 || helper == 8)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：km" << endl;
        double m;
        cin >> m;
        playerdata.parameters.km = m;
        done = 1;
    }
    if (helper == 0 || helper == 9)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：wjbs" << endl;
        double m;
        cin >> m;
        playerdata.parameters.wjbs = m;
        done = 1;
    }
    if (helper == 0 || helper == 10)
    {
        cout << "[来自：物理参数引导]我们需要1个D，作为：vleap" << endl;
        double m;
        cin >> m;
        playerdata.parameters.vleap = m;
        done = 1;
    }
    if (done == 0)
    {
        cout << "[来自：物理参数引导]无效指令。引导退出" << endl;
        return;
    }
    cout << "[来自：物理参数引导]任务完成。引导退出" << endl;
}

//交互参数引导
void pp3(int helper)
{
    bool done = 0;
    if (helper == -1)
    {
        cout << "[来自：交互参数引导]interactionparameters类型下的分类：0.所有 1.jumping(起跳中) 2.climbing(爬梯中) 3.sightleftrunning(左滑步) 4.sightrightrunning(右滑步) 5.frontrunning(向前跑) 6.backrunning(向后退)" << endl;
        int n;
        cin >> n;
        pp3(n);
        done = 1;
    }
    if (helper == 0 || helper == 1)
    {
        cout << "[来自：交互参数引导]我们需要1个bool，作为：jumping" << endl;
        bool m;
        cin >> m;
        playerdata.interaction.jumping = m;
        done = 1;
    }
    if (helper == 0 || helper == 2)
    {
        cout << "[来自：交互参数引导]我们需要1个bool，作为：climbing" << endl;
        bool m;
        cin >> m;
        playerdata.interaction.climbing = m;
        done = 1;
    }
    if (helper == 0 || helper == 3)
    {
        cout << "[来自：交互参数引导]我们需要1个bool，作为：sightleftrunning" << endl;
        bool m;
        cin >> m;
        playerdata.interaction.sightleftrunning = m;
        done = 1;
    }
    if (helper == 0 || helper == 4)
    {
        cout << "[来自：交互参数引导]我们需要1个bool，作为：sightrightrunning" << endl;
        bool m;
        cin >> m;
        playerdata.interaction.sightrightrunning = m;
        done = 1;
    }
    if (helper == 0 || helper == 5)
    {
        cout << "[来自：交互参数引导]我们需要1个bool，作为：frontrunning" << endl;
        bool m;
        cin >> m;
        playerdata.interaction.frontrunning = m;
        done = 1;
    }
    if (helper == 0 || helper == 6)
    {
        cout << "[来自：交互参数引导]我们需要1个bool，作为：backrunning" << endl;
        bool m;
        cin >> m;
        playerdata.interaction.backrunning = m;
        done = 1;
    }
    if (done == 0)
    {
        cout << "[来自：交互参数引导]无效指令。引导退出" << endl;
        return;
    }
    cout << "[来自：交互参数引导]任务完成。引导退出" << endl;
}

//物理探测引导
void pp4(int helper)
{
    bool done = 0;
    if (helper == -1)
    {
        cout << "[来自：物理探测引导]physicaldetection类型下的分类：0.所有 1.isonground(在地面上) 2.xmcollision(水平负方向碰撞) 3.xpcollision(水平正方向碰撞) 4.ymcollision(纵向负方向碰撞) 5.ypcollision(纵向正方向碰撞) 6.upcollosion(头顶碰撞)" << endl;
        int n;
        cin >> n;
        pp4(n);
        done = 1;
    }
    if (helper == 0 || helper == 1)
    {
        cout << "[来自：物理探测引导]我们需要1个bool，作为：isonground" << endl;
        bool m;
        cin >> m;
        playerdata.detection.isonground = m;
        done = 1;
    }
    if (helper == 0 || helper == 2)
    {
        cout << "[来自：物理探测引导]我们需要1个bool，作为：xmcollision" << endl;
        bool m;
        cin >> m;
        playerdata.detection.xmcollision = m;
        done = 1;
    }
    if (helper == 0 || helper == 3)
    {
        cout << "[来自：物理探测引导]我们需要1个bool，作为：xpcollision" << endl;
        bool m;
        cin >> m;
        playerdata.detection.xpcollision = m;
        done = 1;
    }
    if (helper == 0 || helper == 4)
    {
        cout << "[来自：物理探测引导]我们需要1个bool，作为：ymcollision" << endl;
        bool m;
        cin >> m;
        playerdata.detection.ymcollision = m;
        done = 1;
    }
    if (helper == 0 || helper == 5)
    {
        cout << "[来自：物理探测引导]我们需要1个bool，作为：ypcollision" << endl;
        bool m;
        cin >> m;
        playerdata.detection.ypcollision = m;
        done = 1;
    }
    if (helper == 0 || helper == 6)
    {
        cout << "[来自：物理探测引导]我们需要1个bool，作为：upcollosion" << endl;
        bool m;
        cin >> m;
        playerdata.detection.upcollosion = m;
        done = 1;
    }
    if (done == 0)
    {
        cout << "[来自：物理探测引导]无效指令。引导退出" << endl;
        return;
    }
    cout << "[来自：物理探测引导]任务完成。引导退出" << endl;
}

//结构参数引导
void pp5(int helper)
{
    bool done = 0;
    if (helper == -1)
    {
        cout << "[来自：结构参数引导]structuralparameters类型下的分类：0.所有 1.r(判定圆柱体半径) 2.h(判定圆柱体高度) 3.s(接触判定区宽度) 4.sh(接触判定区高度) 5.sj(跳跃判定区宽度)" << endl;
        int n;
        cin >> n;
        pp5(n);
        done = 1;
    }
    if (helper == 0 || helper == 1)
    {
        cout << "[来自：结构参数引导]我们需要1个F，作为：r" << endl;
        float m;
        cin >> m;
        playerdata.structural.r = m;
        done = 1;
    }
    if (helper == 0 || helper == 2)
    {
        cout << "[来自：结构参数引导]我们需要1个F，作为：h" << endl;
        float m;
        cin >> m;
        playerdata.structural.h = m;
        done = 1;
    }
    if (helper == 0 || helper == 3)
    {
        cout << "[来自：结构参数引导]我们需要1个F，作为：s" << endl;
        float m;
        cin >> m;
        playerdata.structural.s = m;
        done = 1;
    }
    if (helper == 0 || helper == 4)
    {
        cout << "[来自：结构参数引导]我们需要1个F，作为：sh" << endl;
        float m;
        cin >> m;
        playerdata.structural.sh = m;
        done = 1;
    }
    if (helper == 0 || helper == 5)
    {
        cout << "[来自：结构参数引导]我们需要1个F，作为：sj" << endl;
        float m;
        cin >> m;
        playerdata.structural.sj = m;
        done = 1;
    }
    if (done == 0)
    {
        cout << "[来自：结构参数引导]无效指令。引导退出" << endl;
        return;
    }
    cout << "[来自：结构参数引导]任务完成。引导退出" << endl;
}

//体位参数引导
void pp6(int helper)
{
    bool done = 0;
    if (helper == -1)
    {
        cout << "[来自：体位参数引导]orientationparameters类型下的分类：0.所有 1.sighta(视方位角) 2.sightb(视俯仰角) 3.body(身体朝向)" << endl;
        int n;
        cin >> n;
        pp6(n);
        done = 1;
    }
    if (helper == 0 || helper == 1)
    {
        cout << "[来自：体位参数引导]我们需要1个D，作为：sighta" << endl;
        double m;
        cin >> m;
        playerdata.orientation.sighta = m;
        done = 1;
    }
    if (helper == 0 || helper == 2)
    {
        cout << "[来自：体位参数引导]我们需要1个D，作为：sightb" << endl;
        double m;
        cin >> m;
        playerdata.orientation.sightb = m;
        done = 1;
    }
    if (helper == 0 || helper == 3)
    {
        cout << "[来自：体位参数引导]我们需要1个D，作为：body" << endl;
        double m;
        cin >> m;
        playerdata.orientation.body = m;
        done = 1;
    }
    if (done == 0)
    {
        cout << "[来自：体位参数引导]无效指令。引导退出" << endl;
        return;
    }
    cout << "[来自：体位参数引导]任务完成。引导退出" << endl;
}

//控制参数引导
void pp7(int helper)
{
    bool done = 0;
    if (helper == -1)
    {
        cout << "[来自：控制参数引导]controlparameters类型下的分类：0.所有 1.autostepup(自动上台阶) 2.walljump(墙跳) 3.fallprotection(掉落保护)" << endl;
        int n;
        cin >> n;
        pp7(n);
        done = 1;
    }
    if (helper == 0 || helper == 1)
    {
        cout << "[来自：控制参数引导]我们需要1个bool，作为：autostepup" << endl;
        bool m;
        cin >> m;
        playerdata.control.autostepup = m;
        done = 1;
    }
    if (helper == 0 || helper == 2)
    {
        cout << "[来自：控制参数引导]我们需要1个bool，作为：walljump" << endl;
        bool m;
        cin >> m;
        playerdata.control.walljump = m;
        done = 1;
    }
    if (helper == 0 || helper == 3)
    {
        cout << "[来自：控制参数引导]我们需要1个bool，作为：fallprotection" << endl;
        bool m;
        cin >> m;
        playerdata.control.fallprotection = m;
        done = 1;
    }
    if (done == 0)
    {
        cout << "[来自：控制参数引导]无效指令。引导退出" << endl;
        return;
    }
    cout << "[来自：控制参数引导]任务完成。引导退出" << endl;
}

//玩家参数引导
void pp8(int helper)
{
    bool done = 0;
    if (helper == -1)
    {
        cout << "[来自：玩家参数引导]playerparameters类型下的分类：0.所有 1.name(游戏名)" << endl;
        int n;
        cin >> n;
        pp8(n);
        done = 1;
    }
    if (helper == 0 || helper == 1)
    {
        cout << "[来自：玩家参数引导]我们需要1个string，作为：name" << endl;
        string m;
        cin >> m;
        playerdata.player.name = m;
        done = 1;
    }
    if (done == 0)
    {
        cout << "[来自：玩家参数引导]无效指令。引导退出" << endl;
        return;
    }
    cout << "[来自：玩家参数引导]任务完成。引导退出" << endl;
}

//物理数据引导
void helperofphysicsparameters()
{
    cout << "[来自：物理数据引导]player类型下的分类：0.返回 1.物理状态(physicalstate state) 2.物理参数(physicalparameters parameters) 3.交互参数(interactionparameters interaction) 4.物理探测(physicaldetection detection) 5.结构参数(structuralparameters structural) 6.体位参数(orientationparameters orientation) 7.控制参数(controlparameters control) 8.玩家参数(playerparameters player)" << endl;
    int n;
    cin >> n;
    if (n != 0)
    {
        if (n == 1)
        {
            pp1(-1);
        }
        else if (n == 2)
        {
            pp2(-1);
        }
        else if (n == 3)
        {
            pp3(-1);
        }
        else if (n == 4)
        {
            pp4(-1);
        }
        else if (n == 5)
        {
            pp5(-1);
        }
        else if (n == 6)
        {
            pp6(-1);
        }
        else if (n == 7)
        {
            pp7(-1);
        }
        else if (n == 8)
        {
            pp8(-1);
        }
        else
        {
            cout << "[来自：物理数据引导]无效指令。引导退出" << endl;
        }
    }
    else
    {
        cout << "[来自：物理数据引导]返回至指令交互工具。引导退出" << endl;
    }
}

//指令交互工具(预声明)
void helper();//预声明

//程序使用说明
void instruction()
{
    cout << "[来自：程序使用说明]指南：0.返回 1.开发者模式 2.物理模拟 3.错误代码" << endl;
    cin >> modecode;
    if (modecode == 0)
    {
        helper();
    }
    else if (modecode == 1)
    {
        cout << "[来自：开发模式指南]输入对应数字代码并按下回车键以执行相应指令或进入相应菜单." << endl;
        instruction();
    }
    else if (modecode == 2)
    {
        cout << "[来自：物理模拟指南]物理模拟功能使用说明：" << endl;
        cout << "[来自：物理模拟指南]1.配置测试地形：可以通过创建、编辑地形和对玩家进行传送的方式配置测试地形" << endl;
        cout << "[来自：物理模拟指南]2.配置物理环境：使用引导17配置物理环境。您也可以跳过配置或对环境进行初始化" << endl;
        cout << "[来自：物理模拟指南]3.设定引导参数：引导18提供了详细的参数设定功能，可满足多样化的测试需要" << endl;
        cout << "[来自：物理模拟指南]4.读取模拟数据：如果您希望直接观察数据变化，建议您选择“每100-200帧输出一次”" << endl;
        cout << "[来自：物理模拟指南]                如果您希望对数据进行小范围精确分析，如判定细节分析，建议选择“每1-3帧输出一次”" << endl;
        cout << "[来自：物理模拟指南]                如果您希望对数据进行大范围趋势分析，如动力学检测，建议选择“每10-25帧输出一次”" << endl;
        cout << "[来自：物理模拟指南]5.模拟数据保存：注意！直接使用Ctrl+A将会关闭控制台。建议首先拖动全选，然后再使用Ctrl+C复制" << endl;
        instruction();
    }
    else if (modecode == 3)
    {
        cout << "[来自：错误代码指南]输入程序返回的3位16进制错误代码：" << endl;
        cout << "[来自：错误代码指南]错误代码256位（错误定位码位）：0.未定报错来源 1.引导菜单报错 2.世界模块报错 3.物理模块报错 4.数据处理报错 5.渲染模块报错 6.信息菜单报错 7.视觉控制报错 8.存档读写报错" << endl;
        int code;
        cin >> code;
        if (code == 0)
        {
            cout << "[来自：错误代码指南]E0XX：未知错误" << endl;
        }
        else if (code == 4)
        {
            cout << "[来自：错误代码指南]错误代码16位（错误原因码位）：0.未知错误原因 1.输入数据非法 2.数据域越界 3.内存不足/内存调用失败" << endl;
            int code4;
            cin >> code4;
            if (code4 == 0)
            {
                cout << "[来自：错误代码指南]E40X：数据处理错误（具体原因未知）" << endl;
            }
            else if (code4 == 3)
            {
                cout << "[来自：错误代码指南]错误代码1位（错误信息码位）：0.空白错误信息 1.方块block数据 2.面前体face3D数据 3.面face2D数据" << endl;
                int code43;
                cin >> code43;
                if (code43 == 0)
                {
                    cout << "[来自：错误代码指南]E430：内存不足/内存调用失败（失败位置未知）" << endl;
                }
                else if (code43 == 1)
                {
                    cout << "[来自：错误代码指南]E431：在处理方块block数据时，内存不足/内存调用失败" << endl;
                }
                else if (code43 == 2)
                {
                    cout << "[来自：错误代码指南]E432：在处理面前体face3D数据时，内存不足/内存调用失败" << endl;
                }
                else if (code43 == 3)
                {
                    cout << "[来自：错误代码指南]E433：在处理面face2D数据时，内存不足/内存调用失败" << endl;
                }
                else
                {
                    cout << "[来自：错误代码指南]查询失败：不存在的错误代码" << endl;
                }
            }
        }
        else if (code == 8)
        {
            cout << "[来自：错误代码指南]错误代码16位（错误原因码位）：0.未知错误原因 1.存档创建失败 2.存档搜索失败 3.存档读取失败 4.存档命名失败 5.根目录创建失败" << endl;
            int code8;
            cin >> code8;
            if (code8 == 1)
            {
                cout << "[来自：错误代码指南]错误代码1位（错误信息码位）：0.空白错误信息 1.文件不存在 2.文件被占用/锁定 3.权限不足(可能被设定为只读) 4.名称含非法字符" << endl;
                int code81;
                cin >> code81;
                if (code81 == 0)
                {
                    cout << "[来自：错误代码指南]E810：存档创建失败，原因未知" << endl;
                }
                else if (code81 == 1)
                {
                    cout << "[来自：错误代码指南]E811：存档创建失败，报文件不存在" << endl;
                }
                else if (code81 == 2)
                {
                    cout << "[来自：错误代码指南]E812：创建的存档文件被占用/锁定" << endl;
                }
                else if (code81 == 3)
                {
                    cout << "[来自：错误代码指南]E813：对创建的存档文件权限不足(可能被设定为只读模式)" << endl;
                }
                else if (code81 == 4)
                {
                    cout << "[来自：错误代码指南]E814：创建存档文件时名称含非法字符" << endl;
                }
                else
                {
                    cout << "[来自：错误代码指南]查询失败：不存在的错误代码" << endl;
                }
            }
            else if (code8 == 3)
            {
                cout << "[来自：错误代码指南]错误代码1位（错误信息码位）：0.空白错误信息 1.读取存档时出现意外中止 2.读取到的数据不合法" << endl;
                int code83;
                cin >> code83;
                if (code83 == 0)
                {
                    cout << "[来自：错误代码指南]E830：存档无法正常读取，原因未知" << endl;
                }
                else if (code83 == 1)
                {
                    cout << "[来自：错误代码指南]E831：存档信息不充分/内容格式错误/读取引导参数不正确，可能的诱因：存档被意外修改/破坏" << endl;
                }
                else if (code83 == 2)
                {
                    cout << "[来自：错误代码指南]E832：存档内容(类型)错误，可能的诱因：存档被意外修改/破坏" << endl;
                }
                else
                {
                    cout << "[来自：错误代码指南]查询失败：不存在的错误代码" << endl;
                }
            }
            else if (code8 == 4)
            {
                cout << "[来自：错误代码指南]错误代码1位（错误信息码位）：0.空白错误信息 1.文件不存在 2.文件被占用/锁定 3.权限不足(可能被设定为只读) 4.名称含非法字符" << endl;
                int code84;
                cin >> code84;
                if (code84 == 0)
                {
                    cout << "[来自：错误代码指南]E840：存档重命名失败，原因未知" << endl;
                }
                else if (code84 == 1)
                {
                    cout << "[来自：错误代码指南]E841：对存档重命名时发现存档丢失" << endl;
                }
                else if (code84 == 2)
                {
                    cout << "[来自：错误代码指南]E842：试图重命名的存档文件被占用/锁定" << endl;
                }
                else if (code84 == 3)
                {
                    cout << "[来自：错误代码指南]E843：对试图重命名的存档文件权限不足(可能被设定为只读模式)" << endl;
                }
                else if (code84 == 4)
                {
                    cout << "[来自：错误代码指南]E844：输入的存档重命名名称含非法字符" << endl;
                }
                else
                {
                    cout << "[来自：错误代码指南]查询失败：不存在的错误代码" << endl;
                }
            }
            else if (code8 == 5)
            {
                cout << "[来自：错误代码指南]错误代码1位（错误信息码位）：0.空白错误信息 1.未知错误信息" << endl;
                int code85;
                cin >> code85;
                if (code85 == 0)
                {
                    cout << "[来自：错误代码指南]E850：存档根目录创建失败，未返回错误代码" << endl;
                }
                else if (code85 == 1)
                {
                    cout << "[来自：错误代码指南]E851：存档根目录创建失败，返回未知错误代码" << endl;
                }
                else
                {
                    cout << "[来自：错误代码指南]查询失败：不存在的错误代码" << endl;
                }
            }
            else
            {
                cout << "[来自：错误代码指南]查询失败：不存在的错误代码" << endl;
            }
        }
        else
        {
            cout << "[来自：错误代码指南]查询失败：不存在的错误代码" << endl;
        }
        instruction();
    }
    else
    {
        cout << "[来自：程序使用说明]Err:无效指南代码或指南不可用" << endl;
        instruction();
    }
}

//输出模式容器
bool setupcode[20];

//物理模拟器
void physicssimulator(int mode, int time, int division, int sleep)
{
    clock_t start, now; // 用于计时的变量
    start = clock(); // 记录当前运行时刻
    bool goon = 1;
    int timer = 0;
    double timebox[11] =
    {
        0.0
    };
    while (goon)
    {
        timer++;
        now = clock(); // 获得当前时刻
        int duration = 1000 * double(now - start) / CLOCKS_PER_SEC;
        if (setupcode[3] | setupcode[4])
        {
            timebox[timer % 11] = duration;
        }
        if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0)
        {
            goon = 0;
        }
        if (mode == 1 && timer >= time)
        {
            goon = 0;
        }
        if (mode == 2 && duration >= time)
        {
            goon = 0;
        }
        physicsupdate();
        if (timer == 1 || timer % division == 0)
        {
            if (setupcode[0] | setupcode[1] | setupcode[2] | setupcode[3] | setupcode[4])
            {
                if (setupcode[0])
                {
                    cout << playerdata.player.name << " ";
                }
                if (setupcode[1])
                {
                    cout << "帧：" << timer << " ";
                }
                if (setupcode[2])
                {
                    cout << "计时：" << duration << "ms ";
                }
                if (setupcode[3] || setupcode[4])
                {
                    int curr_idx = timer % 11; // 11元素循环索引
                    int prev_10frame_idx = (curr_idx + 1) % 11; // 10帧前索引
                    // 时间差
                    double dt10 = abs(timebox[curr_idx] - timebox[prev_10frame_idx]);
                    // 防除零+极小值（避免fps离谱）
                    if (dt10 <= 0.001) dt10 = 1.0;
                    // fps计算
                    if (setupcode[3])
                    {
                        double fps_val = (10.0 / dt10) * 1000.0; // 先double计算，不截断
                        string fps_str = (fps_val < 1) ? "-" : to_string((int)round(fps_val));
                        cout << "fps：" << fps_str << " ";
                    }
                    // mspf计算
                    if (setupcode[4])
                    {
                        double mspf_val = dt10 / 10.0;
                        string mspf_str = to_string(round(mspf_val * 10) / 10); // 保留1位小数
                        cout << "mspf：" << fixed << setprecision(1) << mspf_str << " ";
                        //cout.unsetf(ios::fixed);//输出流输出格式恢复（使不再对小数位数做精确限定）//功能异常暂时停用
                    }
                }
                cout << endl;
            }
            else
            {
                cout << "数据：" << endl;
            }
            if (setupcode[5])
            {
                cout << fixed
                    << setprecision(1)
                    << "原位：("
                    << playerdata.state.x
                    << ","
                    << playerdata.state.y
                    << ") h="
                    << playerdata.state.h
                    << endl;
            }
            if (setupcode[6])
            {
                cout << "位置：("
                    << playerdata.state.x - xborn
                    << ","
                    << playerdata.state.y - yborn
                    << ") h="
                    << playerdata.state.h
                    << endl;
            }
            if (setupcode[7])
            {
                cout << setprecision(5)
                    << "速度：("
                    << playerdata.state.vx
                    << ","
                    << playerdata.state.vy
                    << ","
                    << playerdata.state.vh
                    << ")"
                    << endl;
            }
            if (setupcode[8])
            {
                cout << "加速：("
                    << playerdata.state.ax
                    << ","
                    << playerdata.state.ay
                    << ","
                    << playerdata.state.ah
                    << ")"
                    << endl;
            }
            if (setupcode[9])
            {
                cout << "接触状态：B:"
                    << playerdata.detection.xmcollision
                    << " F:"
                    << playerdata.detection.xpcollision
                    << " L:"
                    << playerdata.detection.ymcollision
                    << " R:"
                    << playerdata.detection.ypcollision
                    << " D:"
                    << playerdata.detection.isonground
                    << " U:"
                    << playerdata.detection.upcollosion
                    << endl;
            }
            if (setupcode[10])
            {
                string moving;
                if (
                    playerdata.interaction.frontrunning
                    || playerdata.interaction.backrunning
                    || playerdata.interaction.sightleftrunning
                    || playerdata.interaction.sightrightrunning
                    )
                {
                    if (playerdata.interaction.frontrunning || playerdata.interaction.backrunning)
                    {
                        if (playerdata.interaction.sightleftrunning || playerdata.interaction.sightrightrunning)
                        {
                            if (playerdata.interaction.frontrunning)
                            {
                                if (playerdata.interaction.sightleftrunning)
                                {
                                    moving = "向左前";
                                }
                                else
                                {
                                    moving = "向右前";
                                }
                            }
                            else
                            {
                                if (playerdata.interaction.sightleftrunning)
                                {
                                    moving = "向左后";
                                }
                                else
                                {
                                    moving = "向右后";
                                }
                            }
                        }
                        else
                        {
                            moving = playerdata.interaction.frontrunning ? "向前" : "向后";
                        }
                    }
                    else
                    {
                        if (playerdata.interaction.sightleftrunning || playerdata.interaction.sightrightrunning)
                        {
                            moving = playerdata.interaction.sightleftrunning ? "向左" : "向右";
                        }
                    }
                }
                else
                {
                    moving = "惯性状态";
                }
                cout << "动作状态：moving:"
                    << moving
                    << " jumping:"
                    << playerdata.interaction.jumping
                    << " climbing:"
                    << playerdata.interaction.climbing
                    << endl;
                cout << endl;
            }
        }
        Sleep(sleep);
    }
}

//输出模式设定
bool setup(int index, string name, int rec, int t)
{
    if ((t == 1 && rec != 2) || t == 2)
    {
        setupcode[index] = (t == 2) ? rec : 1;
        cout << "[来自：物理模拟引导]自动将输入项："
            << name
            << "  设定为:"
            << setupcode[index]
            << endl;
    }
    else
    {
        cout << "[来自：物理模拟引导]请选择是否启用输入项："
            << name
            << "? 1.启用 0.禁用 ("
            << (
                rec == 2
                ? "无推荐值"
                :
                (
                    rec == 1
                    ? "推荐值：1"
                    : "推荐值：0"
                    )
                )
            << ")"
            << endl;
        int a;
        cin >> a;
        if (a == 0 || a == 1)
        {
            setupcode[index] = a;
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

//物理模拟引导
void helperofphysicssimulator()
{
    cout << "[来自：物理模拟引导]我们建议您首先配置模拟环境。是否需要进入配置？1.是 0.否 2.使用初始化的数据(注意：本操作将会重置数据到初始状态。所有改动将会丢失)" << endl;
    int a10086;
    cin >> a10086;
    if (a10086 == 1)
    {
        cout << "[来自：物理模拟引导]为您转接物理数据引导。请在配置好模拟环境后重新进入物理模拟引导" << endl;
        helperofphysicsparameters();
    }
    else if (a10086 == 2)
    {
        resetplayer();
    }
    else if (a10086 != 0)
    {
        cout << "[来自：物理模拟引导]无效指令。已默认为您跳过配置。" << endl;
    }
    string setupname[20] =
    {
        "玩家游戏名",
        "帧计时",
        "自然计时",
        "帧率（无渲染,近10帧平均）",
        "帧计时（无渲染，近10帧平均）",
        "位置（原始）",
        "位置（映射）",
        "速度",
        "加速度",
        "接触状态",
        "动作状态",
        "运动方向"
    };
    int recommend[20] =
    {
        1,
        1,
        1,
        2,
        2,
        0,
        1,
        1,
        1,
        2,
        2
    };
    cout << "[来自：物理模拟引导]快速输出设定？0.禁用 1.优先使用推荐值 2.核心适用（开发者推荐）" << endl;
    int a1314;
    cin >> a1314;
    for (int i = 0; i < 11; i++)
    {
        bool ok = 0;
        while (!ok)
        {
            ok = setup(i, setupname[i], recommend[i], a1314);
        }
    }
    int nn = 0;//退出阈值
    cout << "[来自：物理模拟引导]输出设定完成。现在请选择模拟运行模式：0.无限 1.帧计时限时 2.自然计时限时 (注意：对于任意模式，您都可以在模拟中按下ESC键来结束模拟)" << endl;
    int a10010;
    cin >> a10010;
    if (a10010 == 1)
    {
        cout << "[来自：物理模拟引导]请设定退出帧数：(帧)" << endl;
        cin >> nn;
    }
    else if (a10010 == 2)
    {
        cout << "[来自：物理模拟引导]请设定退出时间：(毫秒)" << endl;
        cin >> nn;
    }
    else
    {
        if (a10010 != 0)
        {
            cout << "[来自：物理模拟引导]无效模式代码。自动为您选择无限模式" << endl;
            a10010 = 0;
        }
        cout << "[来自：物理模拟引导]模拟将不会主动退出。您可以在模拟中按下ESC键来结束模拟" << endl;
    }
    cout << "[来自：物理模拟引导]请设定模拟程序打印数据的频率：（fpf，default：1，max：10000）(做数据分析，推荐设定1等较小值。直接观察，推荐设定较大值)" << endl;
    int a5;
    cin >> a5;
    if (a5 < 1)
    {
        a5 = 1;
    }
    if (a5 > 10000)
    {
        a5 = 10000;
    }
    cout << "[来自：物理模拟引导]模拟程序每" << a5 << "帧输出一次数据" << endl;
    cout << "[来自：物理模拟引导]请设定模拟程序打印数据的间隔。单位：ms)" << endl;
    int a7;
    cin >> a7;
    cout << "[来自：物理模拟引导]模拟程序每帧等待" << a7 << "ms" << endl;
    cout << "[来自：物理模拟引导]物理模拟已准备好。输入任意内容以确认开始模拟" << endl;
    bool a6;
    cin >> a6;
    physicssimulator(a10010, nn, a5, a7);
    cout << "[来自：物理模拟引导]物理模拟完成。引导退出" << endl;
}

/*
    架构说明：
    以上完成了世界模块和物理模块的开发。接下来将衔接渲染模块。
    但在这之前，
    我们首先要对数据做一个小处理——
    把数据加载出来，折腾折腾，然后喂给做渲染的函数。
    数据处理模块启动
        ——开发者：浪兮
*/

/*
    PS：
    衷心希望我设计的程序架构不会为代码的阅读者们造成困扰
        ——一位认真做开发的hhuer：浪兮
*/

/*
    PPS:
    遇到代码问题请随时找我。
    [AD]抖音，Bilibili,微信视频号@浪兮有点浪
    (对，我admit这就是AD[doge])
    Welcome :)
*/

//【数据处理模块】

//视野数据
struct field
{
    int xfirst;
    int xlast;//注意：下限包含！！！包含！！！！！
    int yfirst;
    int ylast;//注意，同上！
    int hfirst;
    int hlast;
};

/*视觉控制
struct viewcontrol
{
    int rfield;//视野跨距（水平向）
    int hfield;//视野跨距（竖直向）
    //player的视野：(1+2*rfield)*(1+2*rfield)*(1+2*hfield)

    double sighta;//视方位角
    double sightb;//视俯仰角
    //数据说明：采用弧度制。程序中有一些弧度计算相关函数，可用于这部分运算
    //使用说明：物理模块中的数据结构体中也包含有以上2个数据，但是被弃用了。请始终从视觉控制中读写以上2个数据

    float ambient;//环境光强度
    float diffuse;//漫反射强度
    //说明：2个强度相加并不一定等于1，因为后续可以通过分段的映射进行调节。
    //进一步说明：现有映射函数下，1.15为标定的上限值

    float kbd;//亮度距离衰减底色范围比例
    //注意：实际执行的调整参数为亮度

    float fov;//视场角
    float nearclip;//近裁切面
    //特别注意！fov采取角度制！！

    float kborderbrightness;//边框亮度系数
    float kborderthickness;//边框宽度系数

    //视觉侦测对接数据
    float sensibility;//视角灵敏度
};

//初始数据
viewcontrol defaultviewcontrol =
{
    10,
    10,

    0,
    0,

    0.3,
    0.85,

    0.8,

    60.0,
    0.5,

    0.8,
    0.12,

    1
};

//初始设定
viewcontrol viewsetup = defaultviewcontrol;
*/

//视野计算（3次修复版本）
field getfield()
{
    // 获取玩家当前位置
    int px = int(playerdata.state.x);
    int py = int(playerdata.state.y);
    int ph = int(playerdata.state.h);

    // 计算视野范围
    int r = viewsetup.rfield;  // 水平视野跨距
    int h = viewsetup.hfield;  // 垂直视野跨距

    // 获取玩家当前位置的地面高度
    int ground_h = height(px, py);

    // 动态计算垂直视野范围
    int h_first, h_last;

    if (ph <= ground_h) {
        // 玩家在地面或以下：使用标准视野范围
        h_first = max(0, ph - h);
        h_last = ph + h;
    }
    else {
        // 玩家在地面上方：确保视野包含地面
        // 向下视野包含地面，向上视野使用标准值
        h_first = max(0, ground_h - 2);  // 地面以下2格，确保看到地形变化
        h_last = max(ph + h, ground_h + h);  // 取较大值
    }

    return {
        px - r,
        px + r,
        py - r,
        py + r,
        h_first,
        h_last
    };
}

//视野存储
field fielddata;

//动态视野
void fieldupdate()
{
    fielddata = getfield();
}

/*
    重要提示：
    程序中很多元素的定位使用坐标表示。对于点，按照虚拟正交系确定其坐标。
    以下元素抽象为点进行定位：生物体。
    对于指向空间区域的元素的坐标定义，本程序约定如下：
        1.对于任意空间维度上的坐标，将其定义为范围的左值
        2.在特定维度上不对应范围时，坐标定义规则与点相同
        3.有映射计算的坐标，映射与取点线性计算（顺序将不影响结果）
    举例说明如下：
    区块（3，8）的投影范围为：（48-64，96-112）
                                       ——来自开发者：浪兮  2025.12.8  23:43
*/

//可见性检测
int isblockvisible(unsigned x, unsigned y, int h)
{
    int current_type = findtype(x, y, h);
    if (current_type == Empty)
    {
        return 0;
    }

    int visibility = 0;
    field fielddata = getfield();

    // 计算各个方向的可见性
    // 上方向
    if (h + 1 <= fielddata.hlast && findtype(x, y, h + 1) != Empty)
    {
        visibility |= (1 << U);  // 有方块遮挡，顶面不可见
    }

    // 下方向
    if (h - 1 >= fielddata.hfirst && findtype(x, y, h - 1) != Empty)
    {
        visibility |= (1 << D);
    }

    // 前方向（x+1）
    if (x + 1 <= fielddata.xlast && findtype(x + 1, y, h) != Empty)
    {
        visibility |= (1 << F);
    }

    // 后方向（x-1）
    if (x - 1 >= fielddata.xfirst && findtype(x - 1, y, h) != Empty)
    {
        visibility |= (1 << B);
    }

    // 右方向（y+1）
    if (y + 1 <= fielddata.ylast && findtype(x, y + 1, h) != Empty)
    {
        visibility |= (1 << R);
    }

    // 左方向（y-1）
    if (y - 1 >= fielddata.yfirst && findtype(x, y - 1, h) != Empty)
    {
        visibility |= (1 << L);
    }

    // 修复：如果是地面草方块，确保顶面可见
    if (current_type == 1 && h == height(x, y))
    {
        // 地面草方块，清空顶面的遮挡标记
        visibility |= (0 << U);  // 将U位设为0，表示顶面可见
    }

    return visibility;
}
//说明：考虑到实际游戏场景中不太可能出现玩家坐标偏移达到30k+的情形，我们并未在该模块对数据输入做检测并提供额外的处理方案。后续可能通过其他方式进行限制。

/*
    开发者注：
        为方便调试，模块被展开，展开时与原代码等价，仅形式不同。
        考虑到参照便利和版本回退的可能，原始版本代码以注释形式被保留。
        新版代码可能会被调整或优化，出现更新后与原始版本不再等价的情形，属正常情况。
        该部分尚有功能性问题未能得到解决。
                            ——开发者：浪兮  2025.12.13 12:24
*/

//2进制转换(string类型，仅供输出)
string inttobin(int num)
{
    string bin = bitset<32>(num).to_string();
    bin = bin.substr(bin.find_first_not_of('0'));
    return bin.empty() ? "0" : bin;
}

//可见性检测引导
void helperofvisibility()
{
    cout << "[来自：可见检测引导]我们需要2个U，1个int，作为：x，y，h" << endl;
    unsigned a, b;
    int c;
    cin >> a >> b >> c;
    int t = isblockvisible(a, b, c);
    cout << "[来自：可见检测引导]可见性代码为："
        << t
        << "(Ob"
        << inttobin(t)
        << ") 引导退出"
        << endl;
}

//2进制数切片
bool BAP(int num, int bP)
{
    if (bP < 0)
    {
        return false;
    }
    return (num >> bP) & 1;
}

//可见批量引导
void helperofmultiplevisibility()
{
    cout << "[来自：可见批量引导]我们需要4个U，2个int，作为：（x，y，h）*（左界，右界）" << endl;
    unsigned a1, a2, b1, b2;
    int c1, c2;
    cin >> a1 >> a2 >> b1 >> b2 >> c1 >> c2;
    cout << "[来自：可见批量引导]你希望输出的代码格式：(可多选) 1.DEC 2.BIN 4.OCT 8.HEX (注：多选时将所有选中项序号相加)(开发者推荐：3)(有效输入：1-15)" << endl;
    int n;
    cin >> n;
    if (n < 1 || n>15)
    {
        n = 3;
        cout << "[来自：可见批量引导]输出格式代码不合法，已设定为推荐值："
            << n
            << "(0b"
            << inttobin(n)
            << ")"
            << endl;
    }
    else
    {
        cout << "[来自：可见批量引导]输出格式代码设定为："
            << n
            << "(0b"
            << inttobin(n)
            << ")"
            << endl;
    }
    for (int i = a1; i <= a2; i++)
    {
        for (int j = b1; j <= b2; j++)
        {
            for (int k = c1; k <= c2; k++)
            {
                cout << "坐标：（"
                    << i
                    << ","
                    << j
                    << ") h="
                    << k
                    << " 可见性代码：";
                int v = isblockvisible(i, j, k);
                if (BAP(n, 0))
                {
                    cout << "DEC:" << v << " ";
                }
                if (BAP(n, 1))
                {
                    cout << "BIN:" << inttobin(v) << " ";
                }
                if (BAP(n, 2))
                {
                    cout << "OCT:" << oct << v << " ";
                }
                if (BAP(n, 3))
                {
                    cout << "HEX:" << hex << v << " ";
                }
                cout << dec << endl;
            }
        }
    }
    cout << "[来自：可见批量引导]"
        << (a2 - a1 + 1) * (b2 - b1 + 1) * (c2 - c1 + 1)
        << "个方块可见性探测完毕 引导退出"
        << endl;
}

//方块数据定义
struct block
{
    unsigned x, y;
    int h;
    int typecode;
    int visibility;
};

//方块数据声明
block* blockvisibledata = NULL;
int blockcount;//有效计数传递器

//方块数据卸载
void blockdataunload()
{
    if (blockvisibledata != NULL)//判空
    {
        free(blockvisibledata);//释放内存
        blockvisibledata = NULL;//置空
    }
    //开发者注：请务必在加载了方块数据后，及时卸载。如果不释及时释放内存，将导致内存泄露！
}

//方块数据装载 - 修复版本
void blockdataload()
{
    blockdataunload();

    // 重新计算视野
    fielddata = getfield();

    // 计算区块数量
    int x_count = fielddata.xlast - fielddata.xfirst + 1;
    int y_count = fielddata.ylast - fielddata.yfirst + 1;
    int h_count = fielddata.hlast - fielddata.hfirst + 1;
    int total_blocks = x_count * y_count * h_count;

    blockvisibledata = (block*)malloc(total_blocks * sizeof(block));
    if (blockvisibledata == NULL)
    {
        Err(0x431, 2, 1);
        return;
    }

    blockcount = 0;

    // 遍历视野范围内的所有方块
    for (unsigned x = fielddata.xfirst; x <= fielddata.xlast; x++)
    {
        for (unsigned y = fielddata.yfirst; y <= fielddata.ylast; y++)
        {
            for (int h = fielddata.hfirst; h <= fielddata.hlast; h++)
            {
                int block_type = findtype(x, y, h);

                // 只处理非空气方块
                if (block_type != Empty)
                {
                    int visibility = isblockvisible(x, y, h);

                    // 只要有一个面可见，就加入渲染列表
                    if (visibility != 0b111111)
                    {
                        if (blockcount < total_blocks)
                        {
                            blockvisibledata[blockcount] =
                            {
                                x,
                                y,
                                h,
                                block_type,
                                visibility
                            };
                            blockcount++;
                        }
                    }
                }
            }
        }
    }

    //重新分配内存以节省空间
    if (blockcount < total_blocks)
    {
        block* temp = (block*)realloc(blockvisibledata, blockcount * sizeof(block));
        if (temp != NULL)
        {
            blockvisibledata = temp;
        }
    }
}

/*
    重要提醒：
    再次提醒，请务必注意！！！
    对于每一次循环，都应当及时释放使用的内存！
    如果内存泄露了，就出事了！
    在程序退出之前，也要将使用的内存全部释放掉！
    不要过度依赖操作系统兜底！
                    ——来自开发者：浪兮 2025.12.9 16:04
*/

//渲染初始化
#define EX_SHOWCONSOLE 0x0002
#define EX_NOCLOSE 0x0008
void graphicreset()
{
    AllocConsole();//同时显示控制台窗口和图形窗口
    initgraph(1600, 900, EX_SHOWCONSOLE | EX_NOCLOSE);//创建画布
    MOUSEMSG msg;//从鼠标消息结构体，定义鼠标消息变量
    cleardevice();//清屏
    setbkcolor(RGB(135, 206, 235));
    BeginBatchDraw();//启用双缓冲绘图
}

//循环中渲染启动
void graphicstart()
{
    Sleep(1);//等待
    cleardevice();//清屏
}

//循环中渲染结束
void graphicend()
{
    FlushBatchDraw();//刷新屏幕
}

//空间坐标数据
struct coord
{
    unsigned x, y;
    int h;
};

//面前体数据定义
struct face3D
{
    coord vertex[4];
    int typecode;
    float brightness;
    float distance;

};

//面前体数据声明
face3D* face3Ddata = NULL;
int face3Dcount;

//空间向量
struct vector3
{
    double x, y, h;
};

//空间距离
float face3Ddistance;
inline double distance(double x1, double y1, double h1, double x2, double y2, double h2)
{
    face3Ddistance =
        sqrt(
            pow((x1 - x2), 2)
            + pow((y1 - y2), 2)
            + pow((h1 - h2), 2)
        );
    return face3Ddistance;
}

//映射函数
inline double fbrightness(float x)
{
    return min(1.0, 0.02515 * (-12.222 * x + 32 * tanh(x / 1.8)) / 0.10043);
}

//范围限定
float maxd;
void getmaxd()
{
    maxd = viewsetup.kbd
        * distance
        (
            viewsetup.rfield,
            viewsetup.rfield,
            viewsetup.hfield,
            0.0,
            0.0,
            0.0
        );
}

/*
//from：工具栏。被放置方块函数"和物理输入函数"使用
int toolchoice = 0;
*/

void placeBlockAt(int x, int y, int h)


// v3.30.1新增：放置方块函数
;

/*
// v3.30.1 修复：像Minecraft一样的方块放置逻辑
void placeBlock() {
    if (crosshairHit.hit && crosshairHit.distance < MAX_RAY_DISTANCE) {
        // 1. 获取选中方块的信息
        int selectedX = crosshairHit.blockX;
        int selectedY = crosshairHit.blockY;
        int selectedH = crosshairHit.blockH;
        int selectedFace = crosshairHit.face;

        // 2. 确定放置位置（选中方块的面朝向外侧）
        int placeX = selectedX;
        int placeY = selectedY;
        int placeH = selectedH;

        // 根据命中的面调整位置
        switch (selectedFace) {
            case 0: placeX = selectedX + 1; break; // 前（X+方向）
            case 1: placeX = selectedX - 1; break; // 后（X-方向）
            case 2: placeY = selectedY + 1; break; // 左（Y+方向）
            case 3: placeY = selectedY - 1; break; // 右（Y-方向）
            case 4: placeH = selectedH + 1; break; // 上（H+方向）
            case 5: placeH = selectedH - 1; break; // 下（H-方向）
        }

        // 3. 调试信息
        cout << "[放置调试] ==== 开始放置 ====" << endl;
        cout << "[放置调试] 选中方块: (" << selectedX << ", " << selectedY << ", " << selectedH << ")" << endl;
        cout << "[放置调试] 选中面: " << selectedFace
             << " (" << getFaceName(selectedFace) << ")" << endl;
        cout << "[放置调试] 放置位置: (" << placeX << ", " << placeY << ", " << placeH << ")" << endl;
        cout << "[放置调试] 放置位置类型: "
             << blocktypename[findtype(placeX, placeY, placeH)] << endl;

        // 4. 检查放置位置是否为空
        int targetType = findtype(placeX, placeY, placeH);
        if (targetType != Empty) {
            cout << "[放置调试] ❌ 放置失败：位置已有方块 ("
                 << blocktypename[targetType] << ")" << endl;

            // 尝试相邻位置（避免被地形方块阻挡）
            cout << "[放置调试] 尝试寻找可放置位置..." << endl;

            // 尝试稍微调整位置（比如向上移动一格）
            int adjustedPlaceH = placeH + 1;
            int adjustedType = findtype(placeX, placeY, adjustedPlaceH);

            if (adjustedType == Empty) {
                cout << "[放置调试] ✅ 找到可放置位置: ("
                     << placeX << ", " << placeY << ", " << adjustedPlaceH << ")" << endl;
                placeH = adjustedPlaceH;
                targetType = Empty;
            } else {
                cout << "[放置调试] 调整后位置也有方块: ("
                     << blocktypename[adjustedType] << ")" << endl;
                return; // 无法放置
            }
        }

        // 5. 如果位置为空，检查是否在玩家体内
        if (targetType == Empty) {
            float playerX = playerdata.state.x;
            float playerY = playerdata.state.y;
            float playerH = playerdata.state.h;

            // 计算方块中心坐标
            float blockCenterX = placeX + 0.5f;
            float blockCenterY = placeY + 0.5f;
            float blockCenterH = placeH + 0.5f;

            // 简单距离检查（更宽松）
            float dx = fabs(blockCenterX - playerX);
            float dy = fabs(blockCenterY - playerY);
            float dh = fabs(blockCenterH - playerH);

            // 玩家碰撞半径（扩大一点避免误判）
            float playerRadius = playerdata.structural.r + 0.6f;
            float playerHeight = playerdata.structural.h + 0.6f;

            bool tooClose = (dx < playerRadius && dy < playerRadius && dh < playerHeight);

            if (tooClose) {
                cout << "[放置调试] ❌ 放置失败：位置离玩家太近" << endl;
                return;
            }

            // 6. 放置方块
            cout << "[放置调试] ✅ 放置方块: " << blocktypename[toolchoice]
                 << " 于 (" << placeX << ", " << placeY << ", " << placeH << ")" << endl;

            updateblock(placeX, placeY, placeH, toolchoice);

            // 7. 立即更新射线检测
            crosshairHit = raycastFromCamera();

            // 8. 播放音效
            playSoundEffect("C7_E7_G7_C8.wav");

            cout << "[放置调试] ==== 放置成功 ====" << endl;
        }
    } else {
        cout << "[放置调试] 未选中任何方块" << endl;
    }
}

// 辅助函数：获取面名称
string getFaceName(int face) {
    switch (face) {
        case 0: return "前";
        case 1: return "后";
        case 2: return "左";
        case 3: return "右";
        case 4: return "上";
        case 5: return "下";
        default: return "未知";
    }
}
*/

//暂时注销掉射线检测。

/*

// v3.30.1 修复：像Minecraft一样的方块放置逻辑
// 首先，确保在自由飞行模式下也能正确放置方块
// 修改 placeBlock() 函数，添加飞行模式的特殊处理
void placeBlock()
{
    if (crosshairHit.hit && crosshairHit.distance < MAX_RAY_DISTANCE)
    {
        // 1. 获取选中方块的信息
        int selectedX = crosshairHit.blockX;
        int selectedY = crosshairHit.blockY;
        int selectedH = crosshairHit.blockH;
        int selectedFace = crosshairHit.face;

        // 2. 确定放置位置（选中方块的面朝向外侧）
        int placeX = selectedX;
        int placeY = selectedY;
        int placeH = selectedH;

        // 根据命中的面调整位置
        switch (selectedFace)
        {
        case 0: placeX = selectedX + 1; break; // 前（X+方向）
        case 1: placeX = selectedX - 1; break; // 后（X-方向）
        case 2: placeY = selectedY + 1; break; // 左（Y+方向）
        case 3: placeY = selectedY - 1; break; // 右（Y-方向）
        case 4: placeH = selectedH + 1; break; // 上（H+方向）
        case 5: placeH = selectedH - 1; break; // 下（H-方向）
        }

        // 3. 调试信息
        static int placeDebugCount = 0;
        if (placeDebugCount < 5)
        {
            cout << "\n[放置调试] ==== 第 "
                << placeDebugCount
                << " 次放置 ===="
                << endl;
            cout << "[放置调试] 飞行模式: "
                << (!playerdata.detection.isonground ? "是" : "否")
                << endl;
            cout << "[放置调试] 选中方块: ("
                << selectedX
                << ", "
                << selectedY
                << ", "
                << selectedH
                << ")"
                << endl;
            cout << "[放置调试] 选中面: "
                << selectedFace
                << endl;
            cout << "[放置调试] 放置位置: ("
                << placeX
                << ", "
                << placeY
                << ", "
                << placeH
                << ")"
                << endl;
            cout << "[放置调试] 当前方块类型: " << blocktypename[findtype(placeX, placeY, placeH)] << endl;
            placeDebugCount++;
        }

        // 4. 检查放置位置是否为空
        int targetType = findtype(placeX, placeY, placeH);
        if (targetType != Empty)
        {
            if (placeDebugCount <= 5)
            {
                cout << "[放置调试] ❌ 放置失败：位置已有方块 ("
                    << blocktypename[targetType]
                    << ")"
                    << endl;
            }

            // 尝试相邻位置
            // 优先向上寻找
            for (int offset = 1; offset <= 3; offset++)
            {
                int testH = placeH + offset;
                int testType = findtype(placeX, placeY, testH);

                if (testType == Empty)
                {
                    placeH = testH;
                    targetType = Empty;
                    if (placeDebugCount <= 5)
                    {
                        cout << "[放置调试] ✅ 找到上方可放置位置: ("
                            << placeX
                            << ", "
                            << placeY
                            << ", "
                            << placeH
                            << ")"
                            << endl;
                    }
                    break;
                }
            }

            // 如果上方没有空间，尝试其他方向
            if (targetType != Empty)
            {
                // 尝试前后左右
                int directions[4][2] =
                {
                    {1,0},
                    {-1,0},
                    {0,1},
                    {0,-1}
                };
                for (int dir = 0; dir < 4; dir++)
                {
                    int testX = placeX + directions[dir][0];
                    int testY = placeY + directions[dir][1];
                    int testType = findtype(testX, testY, placeH);

                    if (testType == Empty)
                    {
                        placeX = testX;
                        placeY = testY;
                        targetType = Empty;
                        if (placeDebugCount <= 5)
                        {
                            cout << "[放置调试] ✅ 找到侧面可放置位置: ("
                                << placeX
                                << ", "
                                << placeY
                                << ", "
                                << placeH
                                << ")"
                                << endl;
                        }
                        break;
                    }
                }
            }

            if (targetType != Empty)
            {
                if (placeDebugCount <= 5)
                {
                    cout << "[放置调试] 找不到可放置位置" << endl;
                }
                return; // 无法放置
            }
        }

        // 5. 如果位置为空，检查是否在玩家体内
        if (targetType == Empty)
        {
            float playerX = playerdata.state.x;
            float playerY = playerdata.state.y;
            float playerH = playerdata.state.h;

            // 计算方块中心坐标
            float blockCenterX = placeX + 0.5f;
            float blockCenterY = placeY + 0.5f;
            float blockCenterH = placeH + 0.5f;

            // 简单距离检查（飞行模式下可以更宽松）
            float dx = fabs(blockCenterX - playerX);
            float dy = fabs(blockCenterY - playerY);
            float dh = fabs(blockCenterH - playerH);

            // 玩家碰撞半径（飞行模式下可以稍微缩小检查范围）
            float playerRadius = playerdata.structural.r + 0.3f;  // 从 0.6 改为 0.3
            float playerHeight = playerdata.structural.h + 0.3f;  // 从 0.6 改为 0.3

            bool tooClose = (dx < playerRadius && dy < playerRadius && dh < playerHeight);



            // 6. 放置方块
            if (placeDebugCount <= 5)
            {
                cout << "[放置调试] ✅ 放置方块: "
                    << blocktypename[toolchoice]
                    << " 于 ("
                    << placeX
                    << ", "
                    << placeY
                    << ", "
                    << placeH
                    << ")"
                    << endl;
            }

            updateblock(placeX, placeY, placeH, toolchoice);

            // 7. 立即更新射线检测
            crosshairHit = raycastFromCamera();

            // 8. 播放音效
            //playSoundEffect("C7_E7_G7_C8.wav");

            if (placeDebugCount <= 5)
            {
                cout << "[放置调试] ==== 放置成功 ====" << endl;
            }
        }
    }
    else
    {
        static int missCount = 0;
        if (missCount < 5)
        {
            cout << "[放置调试] 未选中任何方块" << endl;
            cout << "[放置调试] crosshairHit.hit = "
                << crosshairHit.hit
                << endl;
            if (crosshairHit.hit)
            {
                cout << "[放置调试] 命中距离: "
                    << crosshairHit.distance
                    << ", MAX_DISTANCE = "
                    << MAX_RAY_DISTANCE
                    << endl;
            }
            missCount++;
        }
    }
}

*/

// 辅助函数：获取面名称
string getFaceName(int face)
{
    switch (face)
    {
    case 0:
        return "前";
    case 1:
        return "后";
    case 2:
        return "左";
    case 3:
        return "右";
    case 4:
        return "上";
    case 5:
        return "下";
    default:
        return "未知";
    }
}


// 在渲染补充中添加放置预览
void drawPlacementPreview()
{
    //使仅在建造模式显示
    if (currentGameMode == PARKOUR_MODE)
    {
        return;
    }

    if (crosshairHit.hit && crosshairHit.distance < MAX_RAY_DISTANCE)
    {
        // 计算预览位置
        int previewX = crosshairHit.blockX;
        int previewY = crosshairHit.blockY;
        int previewH = crosshairHit.blockH;

        switch (crosshairHit.face)
        {
        case 0:
            previewX++;
            break;
        case 1:
            previewX--;
            break;
        case 2:
            previewY++;
            break;
        case 3:
            previewY--;
            break;
        case 4:
            previewH++;
            break;
        case 5:
            previewH--;
            break;
        }

        // 绘制半透明预览方块
        setlinecolor(RGB(255, 255, 0));
        setlinestyle(PS_SOLID, 2);

        // 转换为屏幕坐标（简化版，你可能需要调整）
        int screenX = 800; // 屏幕中心
        int screenY = 450;

        // 根据距离调整预览大小
        int previewSize = max(20, min(50, (int)(50 / crosshairHit.distance)));

        // 绘制预览框
        rectangle(screenX - previewSize, screenY - previewSize,
            screenX + previewSize, screenY + previewSize);



    }
}


// 辅助函数：在指定位置放置方块
void placeBlockAt(int x, int y, int h)
{
    // 检查位置是否有效
    if (findtype(x, y, h) != Empty)
    {
        cout << "无法放置：位置 (" << x << ", " << y << ", " << h << ") 已有方块" << endl;
        return;
    }



    // 放置方块
    updateblock(x, y, h, toolchoice);
    cout << "方块已放置于 (" << x << ", " << y << ", " << h << ")" << endl;

    // 播放音效
    //playSoundEffect("C7_E7_G7_C8.wav");
}

/*

// v3.30.1新增：破坏方块函数
void breakBlock()
{
    if (crosshairHit.hit && crosshairHit.distance < MAX_RAY_DISTANCE)
    {
        // 移除方块
        updateblock(crosshairHit.blockX, crosshairHit.blockY, crosshairHit.blockH, Empty);


        // 立即更新检测
        crosshairHit = raycastFromCamera();
    }
}

*/


//距离衰减
float kdistance(float x, float y, float h)
{
    return min(
        1.0,
        distance(
            x,
            y,
            h,
            playerdata.state.x,
            playerdata.state.y,
            playerdata.state.h
        )
        / maxd
    );
}

//朝向亮度
float orientationbrightness(float ux, float uy, float uh)
{
    return max(
        0.0,
        ux * (-cos(viewsetup.sightb)) * sin(viewsetup.sighta)
        + uy * cos(viewsetup.sightb) * cos(viewsetup.sighta)
        + uh * sin(viewsetup.sightb)
    );
    //说明：将面的法向量和玩家视线的方向向量做向量内积，即可得到不同朝向的亮度系数。并去除掉背侧面的亮度(点积后得到负值)
}

//亮度计算
float getbrightness(float x, float y, float h, float ux, float uy, float uh)
{
    double b = viewsetup.ambient;
    b += viewsetup.diffuse * kdistance(x, y, h) * orientationbrightness(ux, uy, uh);
    b = fbrightness(b);
    return min(1.0, max(0.0, b));
}

//面前体数据卸载
void face3Dunload()
{
    if (face3Ddata != NULL)//判空
    {
        free(face3Ddata);//释放内存
        face3Ddata = NULL;//置空
    }
}

//面前体数据装载
void blocktoface()
{
    face3Dunload();

    //动态数组申请内存
    face3Ddata = (face3D*)malloc(6 * blockcount * sizeof(face3D));//说明：6为面数
    if (face3Ddata == NULL)
    {
        Err(0x432, 2, 1);
    }
    else
    {
        countnum = 0;
        face3Dcount = 0;
        getmaxd();

        //数据处理
        for (int i = 0; i < blockcount; i++)
        {
            //读取数据
            unsigned x = blockvisibledata[i].x;
            unsigned y = blockvisibledata[i].y;
            int h = blockvisibledata[i].h;
            int t = blockvisibledata[i].typecode;
            int v = blockvisibledata[i].visibility;

            //写入数据
            if (!BAP(v, U))
            {
                face3Ddata[countnum] =
                {
                    {
                        {x,y,h + 1},
                        {x + 1,y,h + 1},
                        {x + 1,y + 1,h + 1},
                        {x,y + 1,h + 1}
                    },
                    t,
                    getbrightness(x + 0.5,y + 0.5,h + 1,0,0,1),
                    face3Ddistance
                };
                countnum++;
            }
            if (!BAP(v, D))
            {
                face3Ddata[countnum] =
                {
                    {
                        {x,y,h},
                        {x + 1,y,h},
                        {x + 1,y + 1,h},
                        {x,y + 1,h}
                    },
                    (t == 1) ? 2 : t,
                    getbrightness(x + 0.5,y + 0.5,h,0,0,-1),
                    face3Ddistance
                };
                countnum++;
            }
            if (!BAP(v, F))
            {
                face3Ddata[countnum] =
                {
                    {
                        {x + 1,y,h + 1},
                        {x + 1,y,h},
                        {x + 1,y + 1,h},
                        {x + 1,y + 1,h + 1}
                    },
                    (t == 1) ? 2 : t,
                    getbrightness(x + 1,y + 0.5,h + 0.5,1,0,0),
                    face3Ddistance
                };
                countnum++;
            }
            if (!BAP(v, B))
            {
                face3Ddata[countnum] =
                {
                    {
                        {x,y,h + 1},
                        {x,y,h},
                        {x,y + 1,h},
                        {x,y + 1,h + 1}
                    },
                    (t == 1) ? 2 : t,
                    getbrightness(x,y + 0.5,h + 0.5,-1,0,0),
                    face3Ddistance
                };
                countnum++;
            }
            if (!BAP(v, R))
            {
                face3Ddata[countnum] =
                {
                    {
                        {x + 1,y + 1,h},
                        {x,y + 1,h},
                        {x,y + 1,h + 1},
                        {x + 1,y + 1,h + 1}
                    },
                    (t == 1) ? 2 : t,
                    getbrightness(x + 0.5,y + 1,h + 0.5,0,1,0),
                    face3Ddistance
                };
                countnum++;
            }
            if (!BAP(v, L))
            {
                face3Ddata[countnum] =
                {
                    {
                        {x + 1,y,h},
                        {x,y,h},
                        {x,y,h + 1},
                        {x + 1,y,h + 1}
                    },
                    (t == 1) ? 2 : t,
                    getbrightness(x + 0.5,y,h + 0.5,0,-1,0),
                    face3Ddistance
                };
                countnum++;
            }
        }
        face3Dcount = countnum;
    }
}

//面转换投影(一次改进版）
POINT face3Dto2D(float vertexX, float vertexY, float vertexH)
{
    // 相对坐标
    float xr = vertexX - playerdata.state.x;  // 相对X
    float yr = vertexY - playerdata.state.y;  // 相对Y
    float hr = vertexH - playerdata.state.h;  // 相对H

    // 视角旋转修正（使用弧度）
    // 方位角旋转（绕垂直轴）
    float xRot1 = xr * cos(viewsetup.sighta) - yr * sin(viewsetup.sighta);
    float yRot1 = xr * sin(viewsetup.sighta) + yr * cos(viewsetup.sighta);
    float hRot1 = hr;

    // 俯仰角旋转（绕水平轴）
    float xRot2 = xRot1 * cos(viewsetup.sightb) + hRot1 * sin(viewsetup.sightb);
    float hRot2 = -xRot1 * sin(viewsetup.sightb) + hRot1 * cos(viewsetup.sightb);
    float yRot2 = yRot1;

    // 透视投影参数
    const int SCREEN_W = 1600;
    const int SCREEN_H = 900;
    float aspect = (float)SCREEN_W / SCREEN_H;  // 16:9宽高比
    float tanHalfFov = tan(viewsetup.fov * pi / 180.0 / 2.0);  // FOV角度转弧度

    // 近裁切面过滤
    if (-xRot2 < viewsetup.nearclip)
    {
        // 返回屏幕外的点
        POINT off_screen =
        {
            -10000,
            -10000
        };
        return off_screen;
    }

    // 透视投影计算
    // xRot2 是深度方向（视线方向），但注意：在旋转后，xRot2 的正值可能表示前方
    // 但通常我们使用负值表示前方，所以这里取负
    float depth = -xRot2;

    // 避免除零
    if (depth < 0.001f)
    {
        depth = 0.001f;
    }

    // 透视投影（使用相似三角形原理）
    // yRot2 是水平方向，hRot2 是垂直方向
    float projX = yRot2 / (depth * tanHalfFov * aspect);
    float projY = hRot2 / (depth * tanHalfFov);

    // 映射到屏幕坐标
    POINT result;
    result.x = (int)((projX + 1.0f) * 0.5f * SCREEN_W);
    result.y = (int)((1.0f - projY) * 0.5f * SCREEN_H); // Y轴翻转

    // 限制坐标在屏幕范围内
    if (result.x < 0)
    {
        result.x = 0;
    }
    else if (result.x >= SCREEN_W)
    {
        result.x = SCREEN_W - 1;
    }

    if (result.y < 0)
    {
        result.y = 0;
    }
    else if (result.y >= SCREEN_H)
    {
        result.y = SCREEN_H - 1;
    }

    return result;
}

//面数据定义
struct face2D
{
    POINT vertex[4];//4个顶点的坐标
    int typecode;//材质
    float brightness;//亮度
    float renderdepth;//渲染层深度（排序依据）
};

//面数据声明
face2D* face2Ddata = NULL;
int face2Dcount;

//面数据卸载
void face2Dunload()
{
    if (face2Ddata != NULL)//判空
    {
        free(face2Ddata);//释放内存
        face2Ddata = NULL;//置空
    }
}

//面数据装载
/*
由于能力有限 对于渲染中在人物靠近地面时总会出现一个巨大的平行四边形遮挡视野的问题 无法有效解决 只能使用ai进行查找修改
  如需查看问题代码 请移步至2.00版本查看
                                                                                        --tianG   2025/12/16  12：52
                                                                                        */
void face2Dload()
{
    face2Dunload();

    // 动态数组申请内存
    face2Ddata = (face2D*)malloc(face3Dcount * sizeof(face2D));
    if (face2Ddata == NULL)
    {
        Err(0x433, 2, 1);
        return;
    }

    countnum = 0;
    face2Dcount = 0;

    // ========== 关键修复：添加面裁剪检查 ==========
    const int SCREEN_W = 1600;
    const int SCREEN_H = 900;
    const int SAFE_MARGIN = 1000; // 安全边界

    for (int i = 0; i < face3Dcount; i++)
    {
        // 转换4个顶点
        POINT p0 = face3Dto2D(
            face3Ddata[i].vertex[0].x,
            face3Ddata[i].vertex[0].y,
            face3Ddata[i].vertex[0].h
        );
        POINT p1 = face3Dto2D(
            face3Ddata[i].vertex[1].x,
            face3Ddata[i].vertex[1].y,
            face3Ddata[i].vertex[1].h
        );
        POINT p2 = face3Dto2D(
            face3Ddata[i].vertex[2].x,
            face3Ddata[i].vertex[2].y,
            face3Ddata[i].vertex[2].h
        );
        POINT p3 = face3Dto2D(
            face3Ddata[i].vertex[3].x,
            face3Ddata[i].vertex[3].y,
            face3Ddata[i].vertex[3].h
        );

        // 检查顶点是否全部在屏幕外
        int offScreenCount = 0;
        if (p0.x == -10000 && p0.y == -10000) offScreenCount++;
        if (p1.x == -10000 && p1.y == -10000) offScreenCount++;
        if (p2.x == -10000 && p2.y == -10000) offScreenCount++;
        if (p3.x == -10000 && p3.y == -10000) offScreenCount++;

        // 情况1：所有顶点都在屏幕外，跳过这个面
        if (offScreenCount == 4)
        {
            continue;
        }

        // 情况2：有顶点在屏幕外，但在安全范围内，修正这些顶点
        if (offScreenCount > 0)
        {
            // 修正屏幕外顶点到屏幕边缘
            auto fixPoint = [&](POINT& p)
                {
                    if (p.x == -10000 && p.y == -10000)
                    {
                        // 使用其他有效顶点的平均值
                        int validCount = 0;
                        int sumX = 0, sumY = 0;

                        if (!(p0.x == -10000 && p0.y == -10000))
                        {
                            sumX += p0.x;
                            sumY += p0.y;
                            validCount++;
                        }
                        if (!(p1.x == -10000 && p1.y == -10000))
                        {
                            sumX += p1.x;
                            sumY += p1.y;
                            validCount++;
                        }
                        if (!(p2.x == -10000 && p2.y == -10000))
                        {
                            sumX += p2.x;
                            sumY += p2.y;
                            validCount++;
                        }
                        if (!(p3.x == -10000 && p3.y == -10000))
                        {
                            sumX += p3.x;
                            sumY += p3.y;
                            validCount++;
                        }

                        if (validCount > 0)
                        {
                            p.x = sumX / validCount;
                            p.y = sumY / validCount;
                        }
                        else
                        {
                            p.x = SCREEN_W / 2;
                            p.y = SCREEN_H / 2;
                        }
                    }
                };

            fixPoint(p0);
            fixPoint(p1);
            fixPoint(p2);
            fixPoint(p3);
        }

        // 情况3：检查面是否畸形（面积过小或顶点重合）
        // 计算面的面积（使用叉积）
        float area = abs((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));

        // 如果面积过小（可能是投影错误），跳过
        if (area < 1.0f)
        {
            continue;
        }

        // 情况4：检查面是否在合理屏幕范围内
        int minX = min(min(p0.x, p1.x), min(p2.x, p3.x));
        int maxX = max(max(p0.x, p1.x), max(p2.x, p3.x));
        int minY = min(min(p0.y, p1.y), min(p2.y, p3.y));
        int maxY = max(max(p0.y, p1.y), max(p2.y, p3.y));

        // 如果面完全超出屏幕（加上安全边界），跳过
        if (maxX < -SAFE_MARGIN || minX > SCREEN_W + SAFE_MARGIN || maxY < -SAFE_MARGIN || minY > SCREEN_H + SAFE_MARGIN)
        {
            continue;
        }

        // 情况5：面的尺寸异常大（可能是投影错误）
        if ((maxX - minX) > SCREEN_W * 2 || (maxY - minY) > SCREEN_H * 2)
        {
            continue;
        }

        // 通过所有检查，添加到渲染列表
        face2Ddata[countnum] =
        {
            {p0, p1, p2, p3},
            face3Ddata[i].typecode,
            face3Ddata[i].brightness,
            face3Ddata[i].distance
        };
        countnum++;
    }

    face2Dcount = countnum;

    // 重新分配内存以节省空间
    if (face2Dcount > 0 && face2Dcount < face3Dcount)
    {
        face2D* temp = (face2D*)realloc(face2Ddata, face2Dcount * sizeof(face2D));
        if (temp != NULL)
        {
            face2Ddata = temp;
        }
    }
    else if (face2Dcount == 0)
    {
        free(face2Ddata);
        face2Ddata = NULL;
    }
}

//平面向量
struct vector2
{
    double x, y;
};

//线宽算法
int linethickness(int index)
{
    vector2 v1 =
    {
        face2Ddata[index].vertex[1].x - face2Ddata[index].vertex[0].x,
        face2Ddata[index].vertex[1].y - face2Ddata[index].vertex[0].y
    };
    vector2 v2 =
    {
        face2Ddata[index].vertex[3].x - face2Ddata[index].vertex[0].x,
        face2Ddata[index].vertex[3].y - face2Ddata[index].vertex[0].y
    };
    float facearea = fabs(v1.x * v2.y - v1.y * v2.x);
    float thickness = viewsetup.kborderthickness * sqrt(facearea);
    thickness *= 1.15 - 0.6 * face2Ddata[index].renderdepth / (viewsetup.rfield * viewsetup.rfield * viewsetup.hfield);
    return max(1, int(thickness));
}

//单位绘制
void drawface(int index)
{
    //绘制准备
    setfillcolor(RGB(
        blockcolor[face2Ddata[index].typecode].r * face2Ddata[index].brightness,
        blockcolor[face2Ddata[index].typecode].g * face2Ddata[index].brightness,
        blockcolor[face2Ddata[index].typecode].b * face2Ddata[index].brightness
    ));
    setlinecolor(RGB(
        blockcolor[face2Ddata[index].typecode].r * face2Ddata[index].brightness * viewsetup.kborderbrightness,
        blockcolor[face2Ddata[index].typecode].g * face2Ddata[index].brightness * viewsetup.kborderbrightness,
        blockcolor[face2Ddata[index].typecode].b * face2Ddata[index].brightness * viewsetup.kborderbrightness
    ));
    setlinestyle(PS_SOLID, linethickness(index));
    //绘制
    fillpolygon(face2Ddata[index].vertex, 4);
    polygon(face2Ddata[index].vertex, 4);
}

//【快速排序算法：按层深度确定渲染顺序】

//数组逆序
void swap(face2D* a, face2D* b)
{
    face2D temp = *a;
    *a = *b;
    *b = temp;
}

//数组分区
int partition(face2D* arr, int low, int high)
{
    face2D pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++)
    {

        if (arr[j].renderdepth <= pivot.renderdepth)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

//快速排序 - 改进的深度排序
void quickSort(face2D* arr, int low, int high)
{
    if (low < high)
    {
        // 选择中间元素作为枢轴
        int mid = low + (high - low) / 2;
        float pivot_depth = arr[mid].renderdepth;

        // 交换枢轴到末尾
        swap(&arr[mid], &arr[high]);

        int i = low - 1;
        for (int j = low; j < high; j++)
        {
            // 修改：从远到近排序（大的深度先渲染，小的深度后渲染）
            if (arr[j].renderdepth >= pivot_depth)
            {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        swap(&arr[i + 1], &arr[high]);
        int pi = i + 1;

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}




//统筹绘制
void drawallfaces()
{
    quickSort(face2Ddata, 0, face2Dcount - 1);
    for (int i = 0; i < face2Dcount; i++)
    {
        drawface(i);
    }
}

//统一数据输入（预声明）
void vcin();

//视角同步（预声明）
void sightsynchronize();

//全流程数据处理
void dataprocess()
{
    blockdataload();
    vcin();
    blocktoface();
    blockdataunload();
    face2Dload();
    face3Dunload();
    sightsynchronize();
    /*
      开发者注：此处没有调用face2Dunload函数对内存进行释放。
                请务必注意在主循环或更高级循环中，调用预置函数释放内存。
                调用位置应在渲染流程后。
                再次提醒：流程不当将可能引发严重内存泄露！
                请勿过度依赖操作系统的安全兜底机制。
    */
}

//【视觉控制模块】

// 避免 windows.h 定义全局宏 min/max 与 std::min/std::max 冲突
#ifndef NOMINMAX
#define NOMINMAX
#endif


// 常量定义
const double EPS = 1e-9;
const double MAX_RAY_DIST = 10.0;  // 准星最大检测距离
const double PI = 3.14159265358979323846;
const double DOUBLE_MAX = std::numeric_limits<double>::max();
const double DOUBLE_MIN = std::numeric_limits<double>::lowest();

// 手动实现范围限制（兼容所有类型）
template<typename T>
T clamp1(const T& val, const T& minVal, const T& maxVal)
{
    return (val < minVal) ? minVal : (val > maxVal) ? maxVal : val;
}

// 向量结构体（基础数学工具）
struct Vector3
{
    double X, Y, Z;

    Vector3() : X(0.0), Y(0.0), Z(0.0)
    {
    }
    Vector3(double x, double y, double z) : X(x), Y(y), Z(z)
    {
    }

    // 向量加法
    static Vector3 Add(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
    }

    // 向量减法
    static Vector3 Sub(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
    }

    // 向量数乘（缩放）
    static Vector3 Mul(const Vector3& a, double k)
    {
        return Vector3(a.X * k, a.Y * k, a.Z * k);
    }

    // 向量点积（计算夹角/长度）
    static double Dot(const Vector3& a, const Vector3& b)
    {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
    }

    // 向量长度
    double Length() const
    {
        double lenSq = Dot(*this, *this);
        return lenSq < EPS ? 0.0 : sqrt(lenSq);
    }

    // 单位向量（归一化，方向不变，长度=1）
    Vector3 Normalize() const
    {
        double len = Length();
        return len < EPS ? Vector3(0.0, 0.0, 0.0) : Mul(*this, 1.0 / len);
    }

    // 两点间距离
    static double Distance(const Vector3& a, const Vector3& b)
    {
        return Sub(a, b).Length();
    }
};

// 视角控制类（封装所有视角参数）
class ViewControl
{
public:
    Vector3 Pos;         // 相机位置
    double Yaw;          // 水平旋转角（度）
    double Pitch;        // 垂直旋转角（度）
    Vector3 Forward;     // 相机朝前方向
    Vector3 Up;          // 相机朝上方向（固定Y轴）
    double Sensitivity;  // 鼠标灵敏度
    double LastMouseX;   // 上一帧鼠标X坐标
    double LastMouseY;   // 上一帧鼠标Y坐标
    bool IsFirstMouse;   // 首次获取鼠标标记

    // 构造函数（默认初始化）
    ViewControl()
        : Pos(0.0, 2.0, 0.0),    // 初始位置（地面上方2米）
        Yaw(90.0),             // 初始水平角（朝向前方，右手系）
        Pitch(0.0),             // 初始垂直角（水平视角）
        Up(0.0, 1.0, 0.0),      // 朝上方向（Y轴正方向）
        Sensitivity(0.1),       // 默认灵敏度（适中）
        LastMouseX(0.0),
        LastMouseY(0.0),
        IsFirstMouse(true)
    {
    }
};

// 视角配置单例类（全局唯一，统一读写视角参数）
class ViewSetup
{
private:
    static ViewControl* s_current;  // 视角控制实例

    // 私有构造/析构（单例模式，禁止外部创建）
    ViewSetup()
    {
    }
    ~ViewSetup()
    {
        if (s_current != nullptr)
        {
            delete s_current;
        }
    }

    // 自动释放内存（程序结束时调用）
    class Deleter
    {
    public:
        ~Deleter()
        {
            if (ViewSetup::s_current != nullptr)
            {
                delete ViewSetup::s_current;
                ViewSetup::s_current = nullptr;
            }
        }
    };
    static Deleter deleter;

public:
    // 获取全局唯一的视角控制实例
    static ViewControl* Current()
    {
        if (s_current == nullptr)
        {
            s_current = new ViewControl();
            RecalcForward();  // 初始化时计算朝前方向
        }
        return s_current;
    }

    // 弧度修正函数（将弧度限制在 0~2π 范围）
    static double RadCorrect(double rad)
    {
        rad = fmod(rad, 2 * PI);  // 取模缩减范围
        if (rad < 0) rad += 2 * PI;  // 负数转正数
        return rad;
    }

    // 重新计算相机朝前方向（核心逻辑）
    static void RecalcForward()
    {
        ViewControl* ctrl = Current();
        double yawRad = RadCorrect(ctrl->Yaw * PI / 180.0);  // 角度转弧度+修正
        double pitchRad = ctrl->Pitch * PI / 180.0;
        pitchRad = clamp1(pitchRad, -PI / 2 + EPS, PI / 2 - EPS);  // 限制垂直弧度（避免翻转）


        // 右手系朝前方向（X右、Y上、Z前为正方向）
        Vector3 forward
        (
            cos(yawRad) * cos(pitchRad),
            sin(pitchRad),
            -sin(yawRad) * cos(pitchRad)  // 核心修改：Z轴分量取负
        );
        ctrl->Forward = forward.Normalize();
    }

    // 禁止拷贝构造和赋值（单例模式）
    ViewSetup(const ViewSetup&) = delete;
    ViewSetup& operator=(const ViewSetup&) = delete;
};

// 初始化静态成员
ViewControl* ViewSetup::s_current = nullptr;
ViewSetup::Deleter ViewSetup::deleter;

// 方块类（场景中的可交互物体）
class Block {
public:
    Vector3 Pos;   // 方块中心点坐标
    double Size;   // 方块边长（默认1.0米）
    int Type;      // 方块类型（0=空气，1=石头，2=泥土）

    Block(const Vector3& pos, double size, int type)
        : Pos(pos), Size(size), Type(type) {
    }
};

// 射线结构体（准星检测用，从相机发射的隐形射线）
struct Ray
{
    Vector3 Origin;  // 射线起点（相机位置）
    Vector3 Dir;     // 射线方向（准星方向）

    Ray(const Vector3& origin, const Vector3& dir)
        : Origin(origin), Dir(dir.Normalize()) {  // 方向向量归一化
    }
};

// 准星检测结果类（存储命中信息）
class RaycastResult
{
public:
    bool Hit;         // 是否命中方块
    Vector3 HitPos;   // 命中点三维坐标
    Block* HitBlock;  // 命中的方块指针
    int HitFace;      // 命中的面（0=前，1=后，2=左，3=右，4=上，5=下）

    RaycastResult() : Hit(false), HitBlock(nullptr), HitFace(-1)
    {
    }
};

// 全局场景方块容器（可添加/删除方块）
std::vector<Block> g_sceneBlocks;

// 初始化场景方块（程序启动时调用）
void InitSceneBlocks()
{
    g_sceneBlocks.push_back(Block(Vector3(5.0, 0.0, 5.0), 1.0, 1));  // 石头方块
    g_sceneBlocks.push_back(Block(Vector3(3.0, 1.0, 4.0), 1.0, 2));  // 泥土方块
    g_sceneBlocks.push_back(Block(Vector3(7.0, 0.0, 3.0), 1.0, 1));  // 石头方块
}

// 函数声明（后续实现）
void UpdateViewByMouse(double xpos, double ypos);
RaycastResult RaycastScene();
RaycastResult RaycastBlock(const Ray& ray, Block& block);
void SetConsoleChineseEncoding();


// 原主函数（程序入口）
void test()
{
    //InitSceneBlocks();          // 初始化场景方块
    //阻止这一修改行为
    SetConsoleChineseEncoding();// 设置中文编码（避免乱码）
    ViewSetup::Current();       // 初始化视角控制

    // 操作说明（清晰易懂）
    cout << "=== 视角控制+准星检测（C++ 左手系版本）===" << endl;
    cout << "坐标系说明：X（右）、Y（上）、Z（前为负方向）" << endl;
    cout << "操作说明：" << endl;
    cout << "1. 鼠标移动 → 控制视角（左右转/上下抬头）" << endl;
    cout << "2. ESC键 → 退出程序" << endl;
    cout << "3. +键 → 提高鼠标灵敏度（每次+0.05，上限1.0）" << endl;
    cout << "4. -键 → 降低鼠标灵敏度（每次-0.05，下限0.01）" << endl;
    cout << "5. 控制台实时显示相机状态和准星命中信息" << endl << endl;

    //首先等待
    int showtime = 10;
    for (int i = 0; i <= showtime; i++)
    {
        cout << "\r" << (showtime - i) << "秒后自动进入..." << "        " << flush;
        // \r：光标回到当前行开头，便于后续覆盖
        // flush：强制刷新缓冲区，确保即时显示（避免cout缓冲导致延迟）
        Sleep(1000); // 等待
        if ((GetAsyncKeyState(VK_SPACE) & 0x8000) != 0)
        {
            showtime = 1;//快速跳过办法
        }
    }

    bool isRunning = true;
    while (isRunning)
    {
        // 1. 读取鼠标位置，更新视角
        POINT mousePos;
        if (GetCursorPos(&mousePos)) // 获取系统鼠标位置
        {
            UpdateViewByMouse(static_cast<double>(mousePos.x), static_cast<double>(mousePos.y));
        }

        // 2. 准星检测（发射射线检测方块）
        RaycastResult raycastResult = RaycastScene();

        // 3. 清空控制台并显示实时信息
        system("cls");
        ViewControl* ctrl = ViewSetup::Current();

        cout << fixed << setprecision(2);  // 保留2位小数
        cout << "相机位置：("
            << ctrl->Pos.X
            << ", "
            << ctrl->Pos.Y
            << ", "
            << ctrl->Pos.Z
            << ")"
            << endl;
        cout << setprecision(1);  // 保留1位小数（角度更简洁）
        cout << "当前旋转角：Yaw="
            << ctrl->Yaw
            << "°，Pitch="
            << ctrl->Pitch
            << "°"
            << endl;
        cout << setprecision(2);
        cout << "当前鼠标灵敏度："
            << ctrl->Sensitivity
            << "（按+/-调节）"
            << endl
            << endl;

        // 显示准星命中信息
        if (raycastResult.Hit && raycastResult.HitBlock != nullptr)
        {
            const char* faceNames[] =
            {
                "前",
                "后",
                "左",
                "右",
                "上",
                "下"
            };
            const char* blockNames[] =
            {
                "空气",
                "石头",
                "泥土"
            };

            // 安全边界检查（避免数组越界）
            int blockType = clamp1(raycastResult.HitBlock->Type, 0, 2);
            int faceIdx = clamp1(raycastResult.HitFace, 0, 5);

            cout << "[命中] 方块类型："
                << blockNames[blockType]
                << "（编号"
                << blockType
                << "）"
                << endl;
            cout << "[命中] 所在面："
                << faceNames[faceIdx]
                << "（编号"
                << faceIdx
                << "）"
                << endl;
            cout << "[命中] 坐标：("
                << raycastResult.HitPos.X
                << ", "
                << raycastResult.HitPos.Y
                << ", "
                << raycastResult.HitPos.Z
                << ")"
                << endl
                << endl;
        }
        else
        {
            cout << "[未命中] 准星未检测到任何方块" << endl;
        }

        // 4. 键盘控制（退出+灵敏度调节）
        if (_kbhit())
        {  // 检测是否有键盘输入
            int key = _getch();  // 读取按键（无回显）
            if (key == 27)
            {
                isRunning = false;
            }
            else if (key == '+')
            {
                ctrl->Sensitivity += 0.05;
                ctrl->Sensitivity = clamp1(ctrl->Sensitivity, 0.01, 1.0);  // 限制范围
            }
            else if (key == '-')
            {
                ctrl->Sensitivity -= 0.05;
                ctrl->Sensitivity = clamp1(ctrl->Sensitivity, 0.01, 1.0);  // 限制范围
            }
        }

        Sleep(30);  // 降低CPU占用（30ms≈33帧，流畅不卡顿）
    }

    cout << endl << "程序退出！" << endl;
    Sleep(1000);  // 停留1秒，让用户看清退出信息
}

// 设置控制台中文编码（避免中文乱码）
void SetConsoleChineseEncoding()
{
    SetConsoleOutputCP(936);  // 设置输出编码为GBK
    SetConsoleCP(936);        // 设置输入编码为GBK
}

//鼠标状态读取
POINT GetRealTimeMousePos()
{
    POINT mouse;
    GetCursorPos(&mouse);                // 获取鼠标屏幕坐标
    ScreenToClient(GetHWnd(), &mouse);   // 转换为图形窗口客户端坐标
    return mouse;
}

//I_want_to_fuck_the_world
bool I_want_to_fuck_the_world = 1;

// 根据鼠标移动更新视角（核心交互逻辑）
void UpdateViewByMouse(double xpos, double ypos)
{
    POINT mousePos;
    GetCursorPos(&mousePos);
    static POINT lastMousePos = mousePos;

    int mouseMoveX = mousePos.x - lastMousePos.x;
    int mouseMoveY = mousePos.y - lastMousePos.y; // 捕捉Y轴移动

    // 1. 水平旋转（原有不变）
    double rotateAngle = mouseMoveX * mouseRotateRatio;
    playerdata.orientation.sighta += rotateAngle;
    viewsetup.sighta = playerdata.orientation.sighta;

    // 2. 垂直俯仰（微调：提高灵敏度，确保效果明显）
    double pitchRatio = mouseRotateRatio * 1.0; // 先设为和水平一样，容易感知效果
    double pitchAngle = mouseMoveY * pitchRatio;
    playerdata.orientation.sightb += pitchAngle;

    // 限制俯仰角（不变）
    if (playerdata.orientation.sightb < -89.0)
    {
        playerdata.orientation.sightb = -89.0;
    }
    if (playerdata.orientation.sightb > 89.0)
    {
        playerdata.orientation.sightb = 89.0;
    }
    viewsetup.sightb = playerdata.orientation.sightb;

    // 3. 鼠标归中（微调：先更新lastMousePos，再归中，避免漏捕捉）
    lastMousePos = mousePos; // 先记录当前位置，再归中
    POINT centerPos;
    if (I_want_to_fuck_the_world)
    {
        centerPos.x = screenWidth * 0.1;
        centerPos.y = GetSystemMetrics(SM_CYSCREEN) * 0.7;

    }
    //centerPos.y = GetRealTimeMousePos().y;
    SetCursorPos(centerPos.x, centerPos.y);

    // 最后同步lastMousePos为中心（确保下一帧计算准确）
    lastMousePos = centerPos;
}

// 场景射线检测（遍历所有方块，返回最近的命中结果）
RaycastResult RaycastScene()
{
    ViewControl* ctrl = ViewSetup::Current();
    Ray ray(ctrl->Pos, ctrl->Forward);  // 构建射线：起点=相机位置，方向=朝前方向

    RaycastResult bestResult;
    double closestDist = MAX_RAY_DIST;  // 初始最近距离=最大检测距离

    // 遍历所有场景方块
    for (size_t i = 0; i < g_sceneBlocks.size(); i++)
    {
        Block& block = g_sceneBlocks[i];
        if (block.Type == 0) continue;  // 跳过空气方块

        // 快速裁剪：方块中心距离相机超过最大检测距离+方块半径 → 直接跳过（优化性能）
        double distToBlock = Vector3::Distance(ctrl->Pos, block.Pos);
        if (distToBlock > MAX_RAY_DIST + block.Size / 2.0) continue;

        // 检测射线与当前方块是否相交
        RaycastResult hitResult = RaycastBlock(ray, block);
        if (!hitResult.Hit) continue;

        // 计算命中点到相机的距离，保留最近的命中结果
        double dist = Vector3::Distance(ctrl->Pos, hitResult.HitPos);
        if (dist < closestDist && dist < MAX_RAY_DIST)
        {
            closestDist = dist;
            bestResult = hitResult;
        }
    }

    return bestResult;
}

// 射线与单个方块相交检测（AABB包围盒算法，精准高效）
RaycastResult RaycastBlock(const Ray& ray, Block& block)
{
    RaycastResult result;
    double halfSize = block.Size / 2.0;

    // 方块的AABB包围盒（最小/最大坐标）
    Vector3 aabbMin
    (
        block.Pos.X - halfSize,
        block.Pos.Y - halfSize,
        block.Pos.Z - halfSize
    );
    Vector3 aabbMax
    (
        block.Pos.X + halfSize,
        block.Pos.Y + halfSize,
        block.Pos.Z + halfSize
    );

    double tMin = DOUBLE_MIN;  // 射线进入包围盒的最小参数
    double tMax = DOUBLE_MAX;  // 射线离开包围盒的最大参数

    // X轴方向检测（射线与包围盒X面的交点）
    if (fabs(ray.Dir.X) < EPS) // 射线平行于X轴
    {
        if (ray.Origin.X < aabbMin.X || ray.Origin.X > aabbMax.X)
        {
            return result;  // 射线起点不在X轴范围内 → 无相交
        }
    }
    else
    {
        double tx1 = (aabbMin.X - ray.Origin.X) / ray.Dir.X;  // 与左平面交点参数
        double tx2 = (aabbMax.X - ray.Origin.X) / ray.Dir.X;  // 与右平面交点参数
        tMin = max(tMin, min(tx1, tx2));  // 更新进入参数
        tMax = min(tMax, max(tx1, tx2));  // 更新离开参数
    }

    // Y轴方向检测（射线与包围盒Y面的交点）
    if (fabs(ray.Dir.Y) < EPS) // 射线平行于Y轴
    {
        if (ray.Origin.Y < aabbMin.Y || ray.Origin.Y > aabbMax.Y)
        {
            return result;  // 射线起点不在Y轴范围内 → 无相交
        }
    }
    else
    {
        double ty1 = (aabbMin.Y - ray.Origin.Y) / ray.Dir.Y;  // 与下平面交点参数
        double ty2 = (aabbMax.Y - ray.Origin.Y) / ray.Dir.Y;  // 与上平面交点参数
        tMin = max(tMin, min(ty1, ty2));  // 更新进入参数
        tMax = min(tMax, max(ty1, ty2));  // 更新离开参数
    }

    // Z轴方向检测（射线与包围盒Z面的交点）
    if (fabs(ray.Dir.Z) < EPS)// 射线平行于Z轴
    {
        if (ray.Origin.Z < aabbMin.Z || ray.Origin.Z > aabbMax.Z)
        {
            return result;  // 射线起点不在Z轴范围内 → 无相交
        }
    }
    else
    {
        double tz1 = (aabbMin.Z - ray.Origin.Z) / ray.Dir.Z;  // 与后平面交点参数
        double tz2 = (aabbMax.Z - ray.Origin.Z) / ray.Dir.Z;  // 与前平面交点参数
        tMin = max(tMin, min(tz1, tz2));  // 更新进入参数
        tMax = min(tMax, max(tz1, tz2));  // 更新离开参数
    }

    // 无相交情况（交点在射线后方，或无重叠范围）
    if (tMax < 0 || tMin > tMax || tMin > MAX_RAY_DIST)
    {
        return result;
    }

    // 计算命中点（取进入包围盒的交点，若在射线后方则取离开点）
    double tHit = tMin;
    if (tHit < 0) tHit = tMax;
    tHit = min(tHit, MAX_RAY_DIST);  // 限制在最大检测距离内
    result.HitPos = Vector3::Add(ray.Origin, Vector3::Mul(ray.Dir, tHit));
    result.HitBlock = &block;  // 关联命中的方块

    // 判断命中的具体面（通过命中点与包围盒面的距离判断）
    if (fabs(result.HitPos.X - aabbMin.X) < EPS)
    {
        result.HitFace = 2;  // 左面
    }
    else if (fabs(result.HitPos.X - aabbMax.X) < EPS)
    {
        result.HitFace = 3;  // 右面
    }
    else if (fabs(result.HitPos.Y - aabbMin.Y) < EPS)
    {
        result.HitFace = 5;  // 下面
    }
    else if (fabs(result.HitPos.Y - aabbMax.Y) < EPS)
    {
        result.HitFace = 4;  // 上面
    }
    else if (fabs(result.HitPos.Z - aabbMin.Z) < EPS)
    {
        result.HitFace = 1;  // 后面
    }
    else if (fabs(result.HitPos.Z - aabbMax.Z) < EPS)
    {
        result.HitFace = 0;  // 前面
    }
    result.Hit = true;  // 标记为命中
    return result;
}

/*
   对以上模块的架构解释：仅供参考！（声明：本解释使用AIGC技术整理排版，(目的：)使清晰）
┌─────────────────────────────────────────────────────────────┐
│  表现层（Presentation Layer）                                                                                            │
│  ├─ 控制台输出模块（状态显示、中文编码）                                                                               │
│  └─ 输入处理模块（鼠标视角控制、键盘操作响应）                                                                         │
├─────────────────────────────────────────────────────────────┤
│  核心业务层（Core Business Layer）                                                                                       │
│  ├─ 视角控制模块（相机位置/旋转/方向计算）                                                                             │
│  └─ 射线检测模块（准星命中逻辑、AABB碰撞算法）                                                                         │
├─────────────────────────────────────────────────────────────┤
│  数据模型层（Data Model Layer）                                                                                          │
│  ├─ 基础数据结构（Vector3向量、Ray射线、检测结果）                                                                     │
│  └─ 业务实体（Block方块、ViewControl视角状态）                                                                         │
├─────────────────────────────────────────────────────────────┤
│  工具层（Utility Layer）                                                                                                 │
│  ├─ 通用工具（clamp范围限制、浮点精度处理）                                                                            │
│  └─ 数学工具（向量运算、弧度/角度转换）                                                                                │
├─────────────────────────────────────────────────────────────┤
│  全局资源层（Global Resource Layer）                                                                                     │
│  ├─ 单例管理（ViewSetup视角单例）                                                                                      │
│  └─ 场景数据（g_sceneBlocks方块容器）                                                                                  │
└─────────────────────────────────────────────────────────────┘
*/

/*
    开发者注：
    该模块不同于其他模块，采用独立构建后引入整合的方式加入程序。因此在所难免地出现了一些兼容问题，并或多或少地为模块对接带来了一定困难。
    它们包括但不限于：
    1.函数重定义：模块间定义了类似功能的函数。
    2.接口不统一：相同或近系的数据在不同模块采用不同的储存方式，这使得在传递这些数据时需要添加额外的传递功能。
                  (注意，这同时可能对计算效率和内存利用效率产生一定的影响)
    3.风格差异化：这使得代码整体的可读性降低，架构的复杂程度提高
    请理性看待这些情况，并望予以理解。
    我们在整合对接时，秉持最小改动和先增后减原则，优先确保整体最优化。
                                                             ——来自开发者：浪兮   2025.12.14  01:25 AM
*/

//窗口居中(预声明)
void windowscentering();

//开发者名录
const string developerroster = R"(
《幻隅：万象晶格》 (IllusionaryNook)项目
【开发团队】河海大学 25级 程序设计基础课程 期末课程设计 第5组（《幻隅：万象晶格》开发团队）
    组长：浪兮 (hhu2524030232 张锐寒) 
    组员：TianG (hhu2524030229 顾天睿)
          多茵 (hhu2524030230 刘伊啸)
          chrysos (hhu2524030231 张杨亦航)
)";

//MulanPSL-2.0
const string MulanPSL_2_0 = R"(
木兰宽松许可证，第2版

木兰宽松许可证，第2版

2020年1月 http://license.coscl.org.cn/MulanPSL2

您对“软件”的复制、使用、修改及分发受木兰宽松许可证，第2版（“本许可证”）的如下条款的约束：

0.   定义

“软件” 是指由“贡献”构成的许可在“本许可证”下的程序和相关文档的集合。

“贡献” 是指由任一“贡献者”许可在“本许可证”下的受版权法保护的作品。

“贡献者” 是指将受版权法保护的作品许可在“本许可证”下的自然人或“法人实体”。

“法人实体” 是指提交贡献的机构及其“关联实体”。

“关联实体” 是指，对“本许可证”下的行为方而言，控制、受控制或与其共同受控制的机构，此处的控制是
指有受控方或共同受控方至少50%直接或间接的投票权、资金或其他有价证券。

1.   授予版权许可

每个“贡献者”根据“本许可证”授予您永久性的、全球性的、免费的、非独占的、不可撤销的版权许可，您可
以复制、使用、修改、分发其“贡献”，不论修改与否。

2.   授予专利许可

每个“贡献者”根据“本许可证”授予您永久性的、全球性的、免费的、非独占的、不可撤销的（根据本条规定
撤销除外）专利许可，供您制造、委托制造、使用、许诺销售、销售、进口其“贡献”或以其他方式转移其“贡
献”。前述专利许可仅限于“贡献者”现在或将来拥有或控制的其“贡献”本身或其“贡献”与许可“贡献”时的“软
件”结合而将必然会侵犯的专利权利要求，不包括对“贡献”的修改或包含“贡献”的其他结合。如果您或您的“
关联实体”直接或间接地，就“软件”或其中的“贡献”对任何人发起专利侵权诉讼（包括反诉或交叉诉讼）或
其他专利维权行动，指控其侵犯专利权，则“本许可证”授予您对“软件”的专利许可自您提起诉讼或发起维权
行动之日终止。

3.   无商标许可

“本许可证”不提供对“贡献者”的商品名称、商标、服务标志或产品名称的商标许可，但您为满足第4条规定
的声明义务而必须使用除外。

4.   分发限制

您可以在任何媒介中将“软件”以源程序形式或可执行形式重新分发，不论修改与否，但您必须向接收者提供“
本许可证”的副本，并保留“软件”中的版权、商标、专利及免责声明。

5.   免责声明与责任限制

“软件”及其中的“贡献”在提供时不带任何明示或默示的担保。在任何情况下，“贡献者”或版权所有者不对
任何人因使用“软件”或其中的“贡献”而引发的任何直接或间接损失承担责任，不论因何种原因导致或者基于
何种法律理论，即使其曾被建议有此种损失的可能性。

6.   语言

“本许可证”以中英文双语表述，中英文版本具有同等法律效力。如果中英文版本存在任何冲突不一致，以中文
版为准。

条款结束

如何将木兰宽松许可证，第2版，应用到您的软件

如果您希望将木兰宽松许可证，第2版，应用到您的新软件，为了方便接收者查阅，建议您完成如下三步：

1， 请您补充如下声明中的空白，包括软件名、软件的首次发表年份以及您作为版权人的名字；

2， 请您在软件包的一级目录下创建以“LICENSE”为名的文件，将整个许可证文本放入该文件中；

3， 请将如下声明文本放入每个源文件的头部注释中。

Copyright (c) [Year] [name of copyright holder]
[Software Name] is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan
PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.

Mulan Permissive Software License，Version 2

Mulan Permissive Software License，Version 2 (Mulan PSL v2)

January 2020 http://license.coscl.org.cn/MulanPSL2

Your reproduction, use, modification and distribution of the Software shall
be subject to Mulan PSL v2 (this License) with the following terms and
conditions:

0. Definition

Software means the program and related documents which are licensed under
this License and comprise all Contribution(s).

Contribution means the copyrightable work licensed by a particular
Contributor under this License.

Contributor means the Individual or Legal Entity who licenses its
copyrightable work under this License.

Legal Entity means the entity making a Contribution and all its
Affiliates.

Affiliates means entities that control, are controlled by, or are under
common control with the acting entity under this License, ‘control’ means
direct or indirect ownership of at least fifty percent (50%) of the voting
power, capital or other securities of controlled or commonly controlled
entity.

1. Grant of Copyright License

Subject to the terms and conditions of this License, each Contributor hereby
grants to you a perpetual, worldwide, royalty-free, non-exclusive,
irrevocable copyright license to reproduce, use, modify, or distribute its
Contribution, with modification or not.

2. Grant of Patent License

Subject to the terms and conditions of this License, each Contributor hereby
grants to you a perpetual, worldwide, royalty-free, non-exclusive,
irrevocable (except for revocation under this Section) patent license to
make, have made, use, offer for sale, sell, import or otherwise transfer its
Contribution, where such patent license is only limited to the patent claims
owned or controlled by such Contributor now or in future which will be
necessarily infringed by its Contribution alone, or by combination of the
Contribution with the Software to which the Contribution was contributed.
The patent license shall not apply to any modification of the Contribution,
and any other combination which includes the Contribution. If you or your
Affiliates directly or indirectly institute patent litigation (including a
cross claim or counterclaim in a litigation) or other patent enforcement
activities against any individual or entity by alleging that the Software or
any Contribution in it infringes patents, then any patent license granted to
you under this License for the Software shall terminate as of the date such
litigation or activity is filed or taken.

3. No Trademark License

No trademark license is granted to use the trade names, trademarks, service
marks, or product names of Contributor, except as required to fulfill notice
requirements in section 4.

4. Distribution Restriction

You may distribute the Software in any medium with or without modification,
whether in source or executable forms, provided that you provide recipients
with a copy of this License and retain copyright, patent, trademark and
disclaimer statements in the Software.

5. Disclaimer of Warranty and Limitation of Liability

THE SOFTWARE AND CONTRIBUTION IN IT ARE PROVIDED WITHOUT WARRANTIES OF ANY
KIND, EITHER EXPRESS OR IMPLIED. IN NO EVENT SHALL ANY CONTRIBUTOR OR
COPYRIGHT HOLDER BE LIABLE TO YOU FOR ANY DAMAGES, INCLUDING, BUT NOT
LIMITED TO ANY DIRECT, OR INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING
FROM YOUR USE OR INABILITY TO USE THE SOFTWARE OR THE CONTRIBUTION IN IT, NO
MATTER HOW IT’S CAUSED OR BASED ON WHICH LEGAL THEORY, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGES.

6. Language

THIS LICENSE IS WRITTEN IN BOTH CHINESE AND ENGLISH, AND THE CHINESE VERSION
AND ENGLISH VERSION SHALL HAVE THE SAME LEGAL EFFECT. IN THE CASE OF
DIVERGENCE BETWEEN THE CHINESE AND ENGLISH VERSIONS, THE CHINESE VERSION
SHALL PREVAIL.

END OF THE TERMS AND CONDITIONS

How to Apply the Mulan Permissive Software License，Version 2
(Mulan PSL v2) to Your Software

To apply the Mulan PSL v2 to your work, for easy identification by
recipients, you are suggested to complete following three steps:

i. Fill in the blanks in following statement, including insert your software
name, the year of the first publication of your software, and your name
identified as the copyright owner;

ii. Create a file named "LICENSE" which contains the whole context of this
License in the first directory of your software package;

iii. Attach the statement to the appropriate annotated syntax at the
beginning of each source file.

Copyright (c) [Year] [name of copyright holder]
[Software Name] is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan
PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.
)";

//版权声明
const string copyrightnotice = R"(
========================================================
                     软件版权声明
========================================================
版权归属项目名称：《幻隅：万象晶格》 (IllusionaryNook)
版权所有 (c) 2025 《幻隅：万象晶格》开发团队
（成员：1、张锐寒 2、顾天睿 3、刘伊啸 4、张杨亦航）
本软件的著作权归本小组完整所有，受《中华人民共和国著作权法》《计算机软件保护条例》保护。
开源授权本软件基于「木兰宽松许可证（Mulan PSL v2）」开源，您可依据该许可证条款复制、使用、修改、分发本软件（含衍生作品）；
许可证全文可查阅：http://license.coscl.org.cn/MulanPSL2；
任何使用 / 分发行为需遵守以下核心要求：
（1）保留本版权声明及许可证副本；
（2）不得移除软件中的版权、专利、免责声明；
（3）本软件按 “原样” 提供，无任何明示 / 默示担保，版权所有者不承担使用风险。
第三方依赖 本软件使用 EasyX 图形库（遵循 GPL 开源协议），其版权归 EasyX 官方所有，使用需同时遵守 GPL 协议。
========================================================
)";

//相关法律引用
const string copyrightstatement = R"(
========================================================
             软件版权及开源许可相关法律引用
========================================================
一、版权相关法律（核心归属依据）
《中华人民共和国著作权法》（2020 年修正）
第二条：中国公民、法人或者非法人组织的作品，不论是否发表，依照本法享有著作权。
第三条第（八）项：计算机软件属于受著作权法保护的作品类型。
第十条：著作权人享有署名权、修改权、复制权、信息网络传播权等专有权利。
《计算机软件保护条例》（2013 年修订）
第五条：软件著作权属于软件开发者，如无相反证明，在软件上署名的主体为开发者。

二、开源许可相关（木兰宽松许可证 v2 核心条款）
版权许可（第 1 条）：贡献者授予永久性、全球性、免费、非独占的版权许可，允许复制 / 使用 / 修改 / 分发贡献内容（含修改版）。
专利许可（第 2 条）：贡献者授予专利许可，但如发起专利侵权诉讼，许可自动终止。
分发限制（第 4 条）：分发时必须提供许可证副本，保留版权、专利、免责声明。
免责声明（第 5 条）：软件按 “原样” 提供，无担保，贡献者 / 版权所有者不承担使用损失责任。
语言效力（第 6 条）：许可证中英文版本具有同等效力，冲突时以中文版为准。
三、侵权责任
《著作权法》第五十二条：未经许可复制、修改、分发软件的，需承担停止侵害、赔偿损失等民事责任；
违反木兰 PSL v2 条款的，版权所有者可终止授权，追究相应法律责任。
四、佐证说明
开发主体：本项目由《幻隅：万象晶格》开发团队独立完成，无学校实质性专项资源支持，著作权归属清晰；
证据留存：项目源代码及版本记录已上传 Gitee，许可证文件（LICENSE）置于根目录，符合开源合规要求；
权利行使：本小组保留署名权、修改权，同时按木兰 PSL v2 授予公众合法的使用 / 分发权利。
========================================================
)";

//GNU通用公共许可证，中文版
const string GPLv2c = R"(
GNU 通用公共许可证
版本 2，1991 年 6 月
版权所有 (C) 1989, 1991 自由软件基金会，Inc.51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA允许每个人复制和分发本许可证文档的逐字副本，但不允许修改它。
前言
大多数软件的许可证被设计为剥夺您共享和修改软件的自由。相比之下，GNU 通用公共许可证旨在保证您共享和修改自由软件的自由 —— 确保软件对所有用户都是自由的。本通用公共许可证适用于大多数自由软件基金会的软件，以及任何由使用该许可证的作者开发的程序。（自由软件基金会的某些其他软件受 GNU 较宽松通用公共许可证的保护。）您也可以将它应用于您的程序。
当我们谈论自由软件时，我们指的是自由，而不是价格。我们的通用公共许可证旨在确保您拥有以下自由：分发自由软件的副本（并可以根据您的意愿为此服务收费）；接收源代码或在需要时获取源代码；修改软件或将软件的部分用于新的自由程序；以及知道您可以做这些事情。
为了保护您的权利，我们需要设置限制，禁止任何人否认您的这些权利或要求您放弃这些权利。这些限制转化为您在分发软件副本或修改软件时的某些责任。
例如，如果您分发这样的程序副本，无论免费或收费，您必须给予接收者您所拥有的所有权利。您必须确保他们也能接收或获取源代码。并且您必须向他们展示这些条款，以便他们知道自己的权利。
我们通过两个步骤保护您的权利：（1）对软件进行版权保护；（2）向您提供此许可证，授予您复制、分发和 / 或修改软件的法律许可。
此外，为了保护每个作者和我们自己，我们希望确保每个人都明白，此自由软件没有任何担保。如果软件被他人修改并传递下去，我们希望其接收者知道他们所拥有的不是原始软件，以便他人引入的任何问题不会影响原始作者的声誉。
最后，任何自由程序都不断受到软件专利的威胁。我们希望避免这样的危险：自由程序的再分发者会单独获得专利许可，实际上使程序变成专有软件。为防止这种情况，我们已经明确指出，任何专利必须许可给每个人免费使用，否则就根本不许可。
以下是复制、分发和修改的精确条款和条件。
0. 定义
本许可证适用于任何包含版权持有人声明的程序或其他作品，该声明表示它可以在本通用公共许可证的条款下分发。以下的 "程序" 指的是任何这样的程序或作品，而 "基于程序的作品" 指的是程序本身或任何根据版权法的衍生作品：也就是说，包含程序或其部分的作品，无论是原封不动的还是经过修改的和 / 或翻译成其他语言的。（以下，翻译被包含在 "修改" 一词中，没有限制。）每个被授权人被称为 "您"。
复制、分发和修改以外的活动不在本许可证的覆盖范围内；它们超出了许可证的范围。运行程序的行为不受限制，程序的输出只有在其内容构成基于程序的作品时才受保护（与通过运行程序产生的方式无关）。这是否成立取决于程序的功能。
1. 源代码的复制和分发
您可以在任何媒介上复制和分发您所收到的程序源代码的完整副本，前提是您：在每个副本上显著且适当地公布适当的版权声明和无担保声明；保留所有指向本许可证和无担保的声明；向程序的任何其他接收者提供一份本许可证以及程序。
您可以对传输副本的物理行为收取费用，并且您可以选择提供担保保护以换取费用。
2. 修改作品的权利
您可以修改您的程序副本或其任何部分，从而形成基于程序的作品，并在第 1 节的条款下复制和分发这些修改或作品，前提是您还满足以下所有条件：您必须使修改后的文件带有显著的声明，说明您更改了文件以及更改的日期。您必须使您分发或发布的任何包含程序或其部分的作品，作为一个整体，在不向所有第三方收费的情况下，根据本许可证的条款获得许可。如果修改后的程序在运行时通常以交互方式读取命令，那么当它以最普通的方式开始交互式使用时，您必须使其打印或显示一个公告，包括适当的版权声明、无担保声明（或者说明您提供担保），以及用户可以根据这些条件重新分发程序的说明，并告诉用户如何查看本许可证的副本。（例外：如果程序本身是交互式的但通常不打印这样的公告，那么基于该程序的作品不需要打印公告。）
这些要求适用于修改后的作品整体。如果作品的可识别部分不是从程序衍生的，并且可以合理地被视为独立的作品，那么当您将这些部分作为独立作品分发时，本许可证及其条款不适用于这些部分。但是，当您将相同的部分作为基于程序的整体作品的一部分分发时，整个作品的分发必须基于本许可证的条款，该许可证对其他被授权人的权限扩展到整个作品，从而扩展到每个部分，无论谁编写了它。
因此，本节的目的不是主张对您完全独立编写的作品的权利或质疑您的权利；而是行使对基于程序的衍生作品或集体作品的分发控制权。
此外，将不基于程序的另一作品与程序（或基于程序的作品）在存储或分发媒介的同一卷上进行简单聚合，并不会使另一作品受到本许可证的约束。
3. 二进制形式的复制和分发
您可以在目标代码或可执行形式下复制和分发程序（或根据第 2 节的基于程序的作品），前提是您也做以下之一：附带完整的相应机器可读源代码，该源代码必须根据第 1 节和第 2 节的条款在通常用于软件交换的媒介上分发；或者附带一份书面要约，有效期至少三年，向任何第三方提供相应源代码的完整机器可读副本，收费不超过您执行源代码分发的实际成本，该副本将根据第 1 节和第 2 节的条款在通常用于软件交换的媒介上分发；或者附带您收到的关于分发相应源代码的要约的信息。（此选项仅允许非商业性分发，并且仅当您以目标代码或可执行形式收到程序时，才符合第 2b 小节的条件。）
作品的 "源代码" 指的是进行修改的首选形式。对于可执行作品，完整源代码指的是其包含的所有模块的源代码，加上任何相关的接口定义文件，加上用于控制可执行文件编译和安装的脚本。但是，作为特殊例外，分发的源代码不必包含通常与操作系统的主要组件（编译器、内核等）一起分发的任何内容（无论是源代码还是二进制形式），除非该组件本身与可执行文件一起分发。
如果通过提供从指定位置复制的方式进行可执行代码或目标代码的分发，那么提供从同一位置复制源代码的等效访问也视为源代码的分发，即使第三方不被强制将源代码与目标代码一起复制。
4. 限制
您不得复制、修改、再许可或分发程序，除非明确按照本许可证的规定。任何违反此规定的尝试都是无效的，并将自动终止您在本许可证下的权利。但是，从您那里获得副本或权利的各方，只要他们保持完全遵守，他们的许可证将不会被终止。
5. 接受与否的选择
您不必接受本许可证，因为您没有签署它。但是，没有其他东西授予您修改或分发程序或其衍生作品的权限。如果您不接受本许可证，这些行为在法律上是被禁止的。因此，通过修改或分发程序（或基于程序的任何作品），您表示接受本许可证来这样做，以及接受复制、分发或修改程序或基于程序的作品的所有条款和条件。
6. 再分发
每当您再分发程序（或基于程序的作品）时，接收者会自动从原始许可人那里获得一个许可证，允许他们在这些条款和条件下复制、分发或修改程序。您不得对接收者行使本许可证授予的权利设置任何进一步的限制。您不负责强制第三方遵守本许可证。
7. 与其他法律的冲突
如果由于法院判决、专利侵权指控或其他原因（不限于专利问题），对您施加了与本许可证条件相矛盾的条件，这些条件不能免除您遵守本许可证条件的责任。如果您无法同时满足本许可证下的义务和任何其他相关义务进行分发，那么作为结果，您可能根本不能分发程序。例如，如果一个专利许可不允许通过您直接或间接获得副本的所有人免费再分发程序，那么您能够同时满足该许可和本许可证的唯一方法就是完全不分发程序。
如果本节的任何部分在特定情况下被认为无效或不可执行，该节的其余部分应适用，并且该节作为一个整体应在其他情况下适用。
本节的目的不是诱导您侵犯任何专利或其他财产权主张，也不是质疑任何此类主张的有效性；本节的唯一目的是保护自由软件分发系统的完整性，该系统通过公共许可实践实现。许多人已经对通过该系统分发的广泛软件做出了慷慨贡献，依赖于该系统的一致应用；由作者 / 捐赠者决定是否愿意通过任何其他系统分发软件，而被许可人不能强加这种选择。
8. 地域限制
如果程序的分发和 / 或使用在某些国家受到专利或版权保护的接口的限制，将程序置于本许可证下的原始版权持有人可以添加明确的地域分发限制，排除这些国家，从而使分发仅在未被排除的国家内或之间允许。在这种情况下，本许可证将包含该限制，就像写在许可证正文中一样。
9. 版本升级
自由软件基金会可能会不时发布修订版和 / 或新的通用公共许可证版本。这些新版本将与当前版本在精神上相似，但可能在细节上有所不同，以解决新的问题或关注点。
每个版本都有一个唯一的版本号。如果程序指定了适用于它的本许可证的版本号，并且声明 "任何更新的版本"，那么您可以选择遵循该版本或自由软件基金会发布的任何更新版本的条款和条件。如果程序没有指定本许可证的版本号，您可以选择自由软件基金会发布的任何版本。
10. 与其他自由程序的整合
如果您希望将程序的部分整合到其他分发条件不同的自由程序中，请写信给作者请求许可。对于自由软件基金会拥有版权的软件，请写信给自由软件基金会；我们有时会对此做出例外。我们的决定将以两个目标为指导：保持我们自由软件的所有衍生作品的自由状态，以及促进软件的共享和重用。
11. 无担保声明
由于程序是免费许可的，因此在适用法律允许的范围内，程序没有任何担保。除非另有书面说明，版权持有人和 / 或其他提供程序的各方按 "原样" 提供程序，不提供任何形式的担保，无论是明示的还是暗示的，包括但不限于适销性和特定用途适用性的暗示担保。关于程序质量和性能的全部风险由您承担。如果程序被证明有缺陷，您承担所有必要的服务、修复或更正的费用。
12. 责任限制
在任何情况下，除非适用法律要求或书面同意，否则任何版权持有人或任何其他可能根据上述规定修改和 / 或再分发程序的方不对您承担任何损害赔偿责任，包括因使用或无法使用程序而产生的任何一般、特殊、偶然或间接损害（包括但不限于数据丢失或数据变得不准确、您或第三方遭受的损失、或程序无法与其他程序一起运行），即使该持有人或其他方已被告知可能发生此类损害。
如果您开发了一个新程序，并且希望它对公众有最大的用处，实现这一目标的最佳方法是使它成为自由软件，每个人都可以根据这些条款重新分发和修改。
如何将本许可证应用于您的新程序
如果您开发了一个新程序，并且希望它对公众有最大的用处，实现这一目标的最佳方法是使它成为自由软件，每个人都可以根据这些条款重新分发和修改。
要做到这一点，请在程序中包含以下声明。最安全的做法是将声明放在每个源文件的开头，以确保最充分的保护。
<程序名称>Copyright (C) < 年份 > < 作者姓名 >
本程序是自由软件；您可以重新分发和 / 或修改它，根据自由软件基金会发布的 GNU 通用公共许可证的条款，版本 2 或（根据您的选择）任何更新的版本。
本程序的分发是希望它会有用，但没有任何担保；甚至没有对适销性或特定用途适用性的默示担保。有关更多详细信息，请参见 GNU 通用公共许可证。
您应该已经收到了一份 GNU 通用公共许可证的副本；如果没有，请写信给自由软件基金会，地址是：51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA。
此外，您应该提供一个方式让人们可以查看本许可证的副本。例如，如果程序是交互式的，您可以在程序的交互模式下启动时显示一个适当的版权声明，以及无担保声明，并告诉用户如何查看本许可证的副本。
如果您的程序是一个子例程库，您可能会认为允许专有应用程序链接到该库会更有用。如果这是您想要做的，请使用 GNU 较宽松通用公共许可证而不是本许可证。
)";

//GNU通用公共许可证（GPL v2）
const string GPLv2 = R"(
GNU GENERAL PUBLIC LICENSE
Version 2, June 1991

Copyright (C) 1989, 1991 Free Software Foundation, Inc.
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
Everyone is permitted to copy and distribute verbatim copies of this license document, but changing it is not allowed.

Preamble

The licenses for most software are designed to take away your freedom to share and change it. By contrast, the GNU General Public License is intended to guarantee your freedom to share and change free software—to make sure the software is free for all its users. This General Public License applies to most of the Free Software Foundation's software and to any other program whose authors commit to using it. (Some other Free Software Foundation software is covered by the GNU Lesser General Public License instead.) You can apply it to your programs, too.

When we speak of free software, we are referring to freedom, not price. Our General Public Licenses are designed to make sure that you have the freedom to distribute copies of free software (and charge for this service if you wish), that you receive source code or can get it if you want it, that you can change the software or use pieces of it in new free programs; and that you know you can do these things.

To protect your rights, we need to make restrictions that forbid anyone to deny you these rights or to ask you to surrender the rights. These restrictions translate to certain responsibilities for you if you distribute copies of the software, or if you modify it.

For example, if you distribute copies of such a program, whether gratis or for a fee, you must give the recipients all the rights that you have. You must make sure that they, too, receive or can get the source code. And you must show them these terms so they know their rights.

We protect your rights with two steps: (1) copyright the software, and (2) offer you this license which gives you legal permission to copy, distribute and/or modify the software.

Also, for each author's protection and ours, we want to make certain that everyone understands that there is no warranty for this free software. If the software is modified by someone else and passed on, we want its recipients to know that what they have is not the original, so that any problems introduced by others will not reflect on the original authors' reputations.

Finally, any free program is threatened constantly by software patents. We wish to avoid the danger that redistributors of a free program will individually obtain patent licenses, in effect making the program proprietary. To prevent this, we have made it clear that any patent must be licensed for everyone's free use or not licensed at all.

The precise terms and conditions for copying, distribution and modification follow.

0. Definitions

This License applies to any program or other work which contains a notice placed by the copyright holder saying it may be distributed under the terms of this General Public License. The "Program" below refers to any such program or work, and a "work based on the Program" means either the Program or any derivative work under copyright law: that is to say, a work containing the Program or a portion of it, either verbatim or with modifications and/or translated into another language. (Hereinafter, translation is included without limitation in the term "modification".) Each licensee is addressed as "you".

Activities other than copying, distribution and modification are not covered by this License; they are outside its scope. The act of running the Program is not restricted, and the output from the Program is covered only if its contents constitute a work based on the Program (independent of having been made by running the Program). Whether that is true depends on what the Program does.

1. Source Code

You may copy and distribute verbatim copies of the Program's source code as you receive it, in any medium, provided that you:
- conspicuously and appropriately publish on each copy an appropriate copyright notice and disclaimer of warranty;
- keep intact all the notices that refer to this License and to the absence of any warranty;
- give any other recipients of the Program a copy of this License along with the Program.

You may charge a fee for the physical act of transferring a copy, and you may at your option offer warranty protection in exchange for a fee.

2. Derivative Works

You may modify your copy or copies of the Program or any portion of it, thus forming a work based on the Program, and copy and distribute such modifications or work under the terms of Section 1 above, provided that you also meet all of these conditions:
- You must cause the modified files to carry prominent notices stating that you changed the files and the date of any change.
- You must cause any work that you distribute or publish, that in whole or in part contains or is derived from the Program or any part thereof, to be licensed as a whole at no charge to all third parties under the terms of this License.
- If the modified program normally reads commands interactively when run, you must cause it, when started running for such interactive use in the most ordinary way, to print or display an announcement including an appropriate copyright notice and a notice that there is no warranty (or else, saying that you provide a warranty) and that users may redistribute the program under these conditions, and telling the user how to view a copy of this License. (Exception: if the Program itself is interactive but does not normally print such an announcement, your work based on the Program is not required to print an announcement.)

These requirements apply to the modified work as a whole. If identifiable sections of that work are not derived from the Program, and can be reasonably considered independent and separate works in themselves, then this License, and its terms, do not apply to those sections when you distribute them as separate works. But when you distribute the same sections as part of a whole which is a work based on the Program, the distribution of the whole must be on the terms of this License, whose permissions for other licensees extend to the entire whole, and thus to each and every part regardless of who wrote it.

Thus, it is not the intent of this section to claim rights or contest your rights to work written entirely by you; rather, the intent is to exercise the right to control the distribution of derivative or collective works based on the Program.

In addition, mere aggregation of another work not based on the Program with the Program (or with a work based on the Program) on a volume of a storage or distribution medium does not bring the other work under the scope of this License.

3. Binary Code

You may copy and distribute the Program (or a work based on it, under Section 2) in object code or executable form under the terms of Sections 1 and 2 above provided that you also do one of the following:
- Accompany it with the complete corresponding machine-readable source code, which must be distributed under the terms of Sections 1 and 2 above on a medium customarily used for software interchange; or,
- Accompany it with a written offer, valid for at least three years, to give any third party, for a charge no more than your cost of physically performing source distribution, a complete machine-readable copy of the corresponding source code, to be distributed under the terms of Sections 1 and 2 above on a medium customarily used for software interchange; or,
- Accompany it with the information you received as to the offer to distribute corresponding source code. (This alternative is allowed only for noncommercial distribution and only if you received the program in object code or executable form with such an offer, in accord with Subsection b above.)

The source code for a work means the preferred form of the work for making modifications to it. For an executable work, complete source code means all the source code for all modules it contains, plus any associated interface definition files, plus the scripts used to control compilation and installation of the executable. However, as a special exception, the source code distributed need not include anything that is normally distributed (in either source or binary form) with the major components (compiler, kernel, and so on) of the operating system on which the executable runs, unless that component itself accompanies the executable.

If distribution of executable or object code is made by offering access to copy from a designated place, then offering equivalent access to copy the source code from the same place counts as distribution of the source code, even though third parties are not compelled to copy the source along with the object code.

4. Restrictions

You may not copy, modify, sublicense, or distribute the Program except as expressly provided under this License. Any attempt otherwise to copy, modify, sublicense or distribute the Program is void, and will automatically terminate your rights under this License. However, parties who have received copies, or rights, from you under this License will not have their licenses terminated so long as such parties remain in full compliance.

5. Acceptance

You are not required to accept this License, since you have not signed it. However, nothing else grants you permission to modify or distribute the Program or its derivative works. These actions are prohibited by law if you do not accept this License. Therefore, by modifying or distributing the Program (or any work based on the Program), you indicate your acceptance of this License to do so, and all its terms and conditions for copying, distributing or modifying the Program or works based on it.

6. Conveying

Each time you redistribute the Program (or any work based on the Program), the recipient automatically receives a license from the original licensor to copy, distribute or modify the Program subject to these terms and conditions. You may not impose any further restrictions on the recipients' exercise of the rights granted herein. You are not responsible for enforcing compliance by third parties to this License.

7. Conflict with Other Licenses

If, as a consequence of a court judgment or allegation of patent infringement or for any other reason (not limited to patent issues), conditions are imposed on you (whether by court order, agreement or otherwise) that contradict the conditions of this License, they do not excuse you from the conditions of this License. If you cannot distribute so as to satisfy simultaneously your obligations under this License and any other pertinent obligations, then as a consequence you may not distribute the Program at all. For example, if a patent license would not permit royalty-free redistribution of the Program by all those who receive copies directly or indirectly through you, then the only way you could satisfy both it and this License would be to refrain entirely from distribution of the Program.

It is not the purpose of this section to induce you to infringe any patents or other property right claims or to contest validity of any such claims; this section has the sole purpose of protecting the integrity of the free software distribution system, which is implemented by public license practices. Many people have made generous contributions to the wide range of software distributed through that system in reliance on consistent application of that system; it is up to the author/donor to decide if he or she is willing to distribute software through any other system and a licensee cannot impose that choice.

This section is intended to make thoroughly clear what is believed to be a consequence of the rest of this License.

8. Geographical Limitations

If the distribution and/or use of the Program is restricted in certain countries either by patents or by copyrighted interfaces, the original copyright holder who places the Program under this License may add an explicit geographical distribution limitation excluding those countries, so that distribution is permitted only in or among countries not thus excluded. In such case, this License incorporates the limitation as if written in the body of this License.

9. Future Versions

The Free Software Foundation may publish revised and/or new versions of the General Public License from time to time. Such new versions will be similar in spirit to the present version, but may differ in detail to address new problems or concerns.

10. Integration with Other Programs

If you wish to incorporate parts of the Program into other free programs whose distribution conditions are different, write to the author to ask for permission. For software which is copyrighted by the Free Software Foundation, write to the Free Software Foundation; we sometimes make exceptions for this. Our decision will be guided by the two goals of preserving the free status of all derivatives of our free software and of promoting the sharing and reuse of software generally.

11. Warranty Disclaimers

BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.** EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

12. Liability Limitations

IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

If you develop a new program, and you want it to be of the greatest possible use to the public, the best way to achieve this is to make it free software which everyone can redistribute and change under these terms.

How to Apply These Terms to Your New Programs

If you develop a new program, and you want it to be of the greatest possible use to the public, the best way to achieve this is to make it free software which everyone can redistribute and change under these terms.

To do so, attach the following notices to the program. It is safest to attach them to the start of each source file to most effectively convey the exclusion of warranty; and each file should have at least the "copyright" notice and a pointer to where the full notice is found.

```
<program name>
Copyright (C) <year> <name of author>

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
```

Also, provide a way for users to view a copy of this License. For example, if the program is interactive, you can display an appropriate copyright notice and a notice that there is no warranty (or else, saying that you provide a warranty) and tell the user how to view a copy of this License.

If your program is a library, you may consider it more useful to permit linking proprietary applications with the library. If this is what you want, use the GNU Lesser General Public License instead of this License.
)";

//第三方依赖
const string TPD = R"(
第三方依赖使用声明（EasyX 图形库）
一、声明主体及关联项目
声明方（以下简称 “我方”）：《幻隅：万象晶格》（英文名称：IllusionaryNook）开发团队，由河海大学 25 级程序设计基础课程期末课程设计第 5 组组成，核心成员为张锐寒（学号：hhu2524030232）、顾天睿（学号：hhu2524030229）、刘伊啸（学号：hhu2524030230）、张杨亦航（学号：hhu2524030231）。
关联项目：《幻隅：万象晶格》软件（以下简称 “我方软件”），该软件基于木兰宽松许可证 v2（Mulan PSL v2）开源，著作权归我方完整所有，相关权利受《中华人民共和国著作权法》《计算机软件保护条例》保护。
第三方依赖：我方软件开发及运行过程中，使用 EasyX 图形库（以下简称 “EasyX 库”）作为技术支撑，该库为我方软件的必要第三方组件。
二、第三方依赖核心信息（依据官方公示文件核实）
权利主体：EasyX 库的著作权归 EasyX Team（中文名称：EasyX 开发团队，以下简称 “EasyX 方”）所有，权利来源为独立开发完成，相关权利已通过版权登记及官方公示确立。
许可类型：EasyX 库明确公示遵循《GNU 通用公共许可证第 2 版》（GNU General Public License v2，以下简称 “GPL v2 协议”），该协议为全球通用开源软件许可协议，文本可通过 GNU 官方渠道查询（官方地址：https://www.gnu.org/licenses/old-licenses/gpl-2.0.html）。
获取路径：我方使用的 EasyX 库通过 EasyX 官方网站（https://easyx.cn/）合法下载获取。
三、使用方式及合规依据
使用范围：我方仅在我方软件中调用 EasyX 库的公开接口（包括图形渲染、窗口控制等基础功能），未对 EasyX 库的源代码进行任何修改、反向工程、解构或衍生开发，调用方式符合 EasyX 官方文档及 GPL v2 协议的许可要求。
合规措施：
（1）已完整保留 EasyX 库自带的版权声明、许可协议提示及免责条款，未以任何形式删除、遮挡或修改上述信息；
（2）在我方软件的开源仓库（Gitee 地址：https://gitee.com/langxibielangle/IllusionaryNook/）根目录 “LICENSE” 文件中，已附加 GPL v2 协议完整文本，并标注 EasyX 库的权利归属及许可信息；
（3）在我方软件的版权声明中，已明确披露对 EasyX 库的使用情况及 GPL v2 协议的适用要求，确保软件使用者知晓第三方依赖的权利信息。
法律依据：我方对 EasyX 库的使用行为，符合《中华人民共和国著作权法》第二十四条（合理使用）、《计算机软件保护条例》第十六条（软件许可使用）及 GPL v2 协议第 1 条（版权许可）、第 3 条（分发要求）的规定。
)";

//版权与许可
void copyrightandlicenseinfo()
{
    cout << "版权与许可信息:" << endl;
    cout << "1.许可证" << endl;
    cout << "2.版权声明" << endl;
    cout << "3.相关法律引用" << endl;
    cout << "4.第三方依赖" << endl;
    cout << "输入: ";
    int a;
    cin >> a;
    if (a == 1)
    {
        cout << "相关许可证:" << endl;
        cout << "1.MulanPSL-2.0(开源许可证)" << endl;
        cout << "2.GPL v2 （公共许可证）" << endl;
        cout << "输入: ";
        int b;
        cin >> b;
        if (b == 1)
        {
            cout << MulanPSL_2_0 << endl;
        }
        else if (b == 2)
        {
            cout << GPLv2c << endl << endl;
            cout << GPLv2 << endl;
        }
        else
        {
            cout << "Err:许可证不存在" << endl;
            copyrightandlicenseinfo();
        }
    }
    else if (a == 2)
    {
        cout << copyrightnotice << endl;
    }
    else if (a == 3)
    {
        cout << copyrightstatement << endl;
    }
    else if (a == 4)
    {
        cout << TPD << endl;
    }
    else
    {
        cout << "Err:索引无效" << endl;
        copyrightandlicenseinfo();
    }
}

//音频播放模块2(原模块出现未知bug，并难以解决)

/* 全局音频别名（宽字符版本）
const wchar_t* AUDIO_ALIAS = L"gameBGM";

// 辅助函数：将ANSI字符串（中文GBK）转换为宽字符（Unicode，MCI宽接口需要）
std::wstring AnsiToWide(const std::string& ansiStr) {
    int wideLen = MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, NULL, 0);
    std::wstring wideStr(wideLen, 0);
    MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, &wideStr[0], wideLen);
    return wideStr;
}

// 函数1：循环播放指定中文路径的音频
bool PlayLoopAudio(const std::string& audioPath) {
    // 1. 将中文路径转换为宽字符（适配MCI宽接口）
    std::wstring widePath = AnsiToWide(audioPath);
    // 替换路径中的斜杠为Windows标准反斜杠（避免解析异常）
    for (size_t i = 0; i < widePath.size(); ++i) {
        if (widePath[i] == L'/') widePath[i] = L'\\';
    }

    // 2. 先关闭旧音频实例（宽字符命令）
    wchar_t closeOldCmd[128];
    swprintf_s(closeOldCmd, L"close %s", AUDIO_ALIAS);
    mciSendStringW(closeOldCmd, NULL, 0, NULL);

    // 3. 拼接宽字符版打开命令（指定WAV格式+中文路径）
    wchar_t openCmd[512];
    swprintf_s(openCmd, L"open \"%s\" alias %s type waveaudio", widePath.c_str(), AUDIO_ALIAS);

    // 4. 打开音频（宽字符接口）
    MCIERROR err = mciSendStringW(openCmd, NULL, 0, NULL);
    if (err != 0) {
        wchar_t errBuf[256];
        mciGetErrorStringW(err, errBuf, 256);
        // 输出宽字符错误信息
        wprintf(L"音频打开失败：%s\n", errBuf);
        return false;
    }

    // 5. 拼接宽字符版循环播放命令
    wchar_t playCmd[128];
    swprintf_s(playCmd, L"play %s repeat", AUDIO_ALIAS);
    err = mciSendStringW(playCmd, NULL, 0, NULL);
    if (err != 0) {
        wchar_t errBuf[256];
        mciGetErrorStringW(err, errBuf, 256);
        wprintf(L"音频播放失败：%s\n", errBuf);
        mciSendStringW(closeOldCmd, NULL, 0, NULL);
        return false;
    }

    // 输出播放成功信息
    wprintf(L"音频循环播放开始：%s\n", widePath.c_str());
    return true;
}

// 函数2：停止播放当前音频
void StopAudio() {
    wchar_t closeCmd[128];
    swprintf_s(closeCmd, L"close %s", AUDIO_ALIAS);
    mciSendStringW(closeCmd, NULL, 0, NULL);
    wprintf(L"音频播放停止\n");
}
*/

// 全局音频别名（用于唯一标识当前播放的音频，避免冲突）
const char* AUDIO_ALIAS = "gameBGM";

//循环播放指定路径的音频（支持相对/绝对路径，自动处理路径空格）
bool PlayLoopAudio(const string& audioPath)
{
    // 先关闭旧音频实例，避免句柄冲突
    mciSendStringA("close gameBGM", NULL, 0, NULL);

    // 拼接打开命令（双引号处理路径含空格的情况）
    char openCmd[512];

    // 正确拼接AUDIO_ALIAS变量
    sprintf_s
    (
        openCmd,
        "open \"%s\" alias %s",
        audioPath.c_str(),
        AUDIO_ALIAS
    );

    // 打开音频文件（带错误检测）
    MCIERROR err =
        mciSendStringA(
            openCmd,
            NULL,
            0,
            NULL
        );
    if (err != 0)
    {
        char errBuf[256];
        mciGetErrorStringA(err, errBuf, 256);
        printf("音频播放失败：%s\n", errBuf);
        return false;
    }

    // 拼接循环播放命令
    char playCmd[128];
    sprintf_s(playCmd, "play %s repeat", AUDIO_ALIAS);
    err =
        mciSendStringA(
            playCmd,
            NULL,
            0,
            NULL
        );
    if (err != 0)
    {
        char errBuf[256];
        mciGetErrorStringA(err, errBuf, 256);
        printf("音频播放失败：%s\n", errBuf);
        mciSendStringA("close gameBGM", NULL, 0, NULL);
        return false;
    }

    printf("音频循环播放开始：%s\n", audioPath.c_str());
    return true;
}

//停止播放当前音频并释放资源
void StopAudio()
{
    char closeCmd[128];
    sprintf_s(closeCmd, "close %s", AUDIO_ALIAS);
    mciSendStringA(closeCmd, NULL, 0, NULL);
    printf("音频播放停止\n");
}
/**/

// 静态变量用于记录当前要显示的文字、持续时间和开始时间
static std::wstring g_ShowText = L"";
static int g_ShowDurationMs = 0;
static std::chrono::steady_clock::time_point g_ShowStartTime;

// 函数参数改为宽字符串（调用时需传 L"文字" 格式）
void ShowCenterTextForSeconds(const std::wstring& text, int seconds)
{
    if (seconds <= 0) return;

    g_ShowText = text;
    g_ShowDurationMs = seconds * 1000;
    g_ShowStartTime = std::chrono::steady_clock::now();
}

void DrawCenterTextIfNeeded()
{
    if (g_ShowText.empty())
        return;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - g_ShowStartTime).count();

    if (elapsed < g_ShowDurationMs)
    {
        settextcolor(WHITE);
        // 字体名也用宽字符（加 L 前缀）
        settextstyle(48, 0, L"微软雅黑");

        int screenWidth = getwidth();
        int screenHeight = getheight();

        // textwidth 接收宽字符串
        int textWidth = textwidth(g_ShowText.c_str());
        int textHeight = textheight(g_ShowText.c_str());
        int x = (screenWidth - textWidth) / 2;
        int y = (screenHeight - textHeight) / 2;

        // outtextxy 接收宽字符串
        outtextxy(x, y, g_ShowText.c_str());
    }
    else
    {
        g_ShowText.clear();
    }
}

//随机数
int getrandomint(int a, int b)
{
    // 步骤1：统一区间顺序（确保 min ≤ max）
    int min_val = a;
    int max_val = b;
    if (min_val > max_val)
    {
        std::swap(min_val, max_val);
    }

    // 步骤2：静态随机数生成器（仅初始化一次，避免重复种子）
    static std::mt19937 rng;
    static bool isInit = false;
    if (!isInit)
    {
        // 优先用真随机种子（random_device）
        std::random_device rd;
        // 兼容：部分编译器（如MinGW）的random_device返回固定值，补充时间种子
        unsigned int seed =
            rd.entropy() > 0
            ?
            rd()
            :
            static_cast<unsigned int>
            (
                std::chrono::steady_clock::now().time_since_epoch().count()
                );
        rng.seed(seed); // 初始化生成器
        isInit = true;
    }

    // 步骤3：定义均匀分布（严格限定 [min_val, max_val] 区间）
    std::uniform_int_distribution<int> dist(min_val, max_val);

    // 步骤4：生成并返回随机数
    return dist(rng);
}

//提示仓库
string tips[] =
{
    string("如果不想丢失游戏进度的话，一定要记得多存档哦！                        ——浪兮"),
    string("按下J键可以开启/关闭飞行模式哦^_^                                    ——浪兮"),
    string("遇到困难莫要慌，按下J键可以开启飞行模式就好啦                        ——浪兮"),
    string("想要体验创造的快乐？切换到空白画布地图一展身手吧！                    ——浪兮"),
    string("广告位招租[手动狗头]                                                ——浪兮"),
    string("欢迎关注：抖音@浪兮有点浪 (视频号，B站同步)                        ——浪兮"),
    string("草原地图提供了平整的地面...似乎很适合用来建造木屋                    ——浪兮"),
    string("\"既然是自然地形，那为什么没有树呢？\" 因为树木不能在幻隅里生长。        ——浪兮"),
    string("飞行模式下，可以使用INHKUM控制角色移动哦^_^                          ——浪兮"),
    string("出现穿模了不要慌，游戏内置多种防卡功能，可惜都不是很好用...            ——浪兮"),
    string("幻隅是什么？嗯...我只记得那是一个遥远的地方                          ——浪兮"),
    string("据说在幻隅里跳得足够高，可以看到代码里的注释哦~                      ——浪兮"),
    string("虚空之下是什么？可能是我的代码bug...                                ——浪兮"),
    string("这个游戏的物理引擎，比我的头发还要飘逸                              ——浪兮"),
    string("如果你掉进虚空，别担心，系统会自动帮你\"防卡\"一下                    ——浪兮"),
    string("你知道吗？每个方块背后都有一个随机数在默默支撑                      ——浪兮"),
    string("尝试在空白画布上建一座通天塔吧！(如果电脑撑得住的话)                  ——浪兮"),
    string("柏林噪声不是柏林墙的声音，是一种算法啦！                            ——浪兮"),
    string("为什么草方块底下是土？因为程序员懒得写根系系统                      ——浪兮"),
    string("按ESC退出游戏前，记得和幻隅说声再见哦~                              ——浪兮"),
    string("这个游戏的帧率，就像过山车一样刺激                                  ——浪兮"),
    string("如果你觉得操作不流畅，可能此时你的电脑正在偷偷哭泣                  ——浪兮"),
    string("幻隅的开发者们写代码时，咖啡因摄入量严重超标                        ——浪兮"),
    string("记住：在虚拟世界里，重力只是程序员的一个想法                        ——浪兮"),
    string("看到奇怪的方块颜色？那可能是我的配色审美                            ——浪兮"),
    string("这个游戏最大的BOSS是...内存泄漏！(开玩笑的啦)                        ——浪兮"),
    string("据说午夜时分在幻隅里转圈，代码会自己优化                            ——浪兮"),
    string("如果你卡在墙里了，恭喜你触发了隐藏彩蛋！                            ——浪兮"),
    string("AABB碰撞检测：听起来很专业，其实就是\"别穿墙\"啦                      ——浪兮"),
    string("游戏里的日夜循环？抱歉，我们连太阳都还没做呢                        ——浪兮"),
    string("幻隅的开发口号：先能跑起来，再考虑优化                              ——浪兮"),
    string("每个程序员心里都住着一个造物主，虽然代码很bug                        ——浪兮"),
    string("这个游戏的渲染距离，取决于你的电脑配置                              ——浪兮"),
    string("如果你看到方块在空中飘，那不是bug，是特性！                          ——浪兮"),
    string("幻隅：一个由if-else和for循环构成的世界                              ——浪兮"),
    string("按下F键向代码致敬！（虽然没这个功能）                                ——浪兮"),
    string("游戏里的物理参数？都是我们随便试出来的                              ——浪兮"),
    string("为什么叫'幻隅'？因为听起来很酷啊！                                  ——浪兮"),
    string("如果你觉得无聊，可以数数游戏里有多少个分号                          ——浪兮"),
    string("这个游戏的开发过程：写代码->找bug->喝咖啡->重复                      ——浪兮"),
    string("记住：在幻隅里，想象力比图形渲染更重要                              ——浪兮"),
    string("我们承诺：游戏里的bug比你的头发少（大概）                            ——浪兮"),
    string("看到这个提示，说明你可能正在等待游戏加载...                          ——浪兮"),
    string("幻隅的天气系统：永远晴天（因为我们还没写其他天气）                    ——浪兮"),
    string("这个游戏的音效，都是我用嘴模拟出来的                                ——浪兮"),
    string("如果你发现什么奇怪的现象，请当成游戏特色                            ——浪兮"),
    string("代码注释比代码本身还长，这就是我们的风格                            ——浪兮"),
    string("幻隅的开发哲学：能用就行，美观再说                                  ——浪兮"),
    string("这个提示的出现概率：100% - 你的运气值                              ——浪兮"),
    string("游戏里的'自然'地形，其实一点都不自然                                ——浪兮"),
    string("我们的目标：让每个方块都有自己的个性（还没实现）                    ——浪兮"),
    string("如果你觉得游戏太难...等等，这游戏有难度吗？                          ——浪兮"),
    string("幻隅的秘密：其实所有方块都是同一种材质换颜色                        ——浪兮"),
    string("这个游戏最好的部分？可以按ESC退出！                                  ——浪兮"),
    string("记住：每个bug背后都有一个有趣的故事                                  ——浪兮"),
    string("我们的渲染引擎：把3D变成2D的魔法                                    ——浪兮"),
    string("如果你在游戏里迷路了...抱歉，我们没有小地图                          ——浪兮"),
    string("幻隅的物理定律：由咖啡因和deadline决定                              ——浪兮"),
    string("这个提示是为了让你在加载时不那么无聊                                ——浪兮"),
    string("游戏里的色彩：RGB(随机, 感觉, 不错)                                ——浪兮"),
    string("如果你觉得游戏卡顿，试试闭上眼睛想象流畅的画面                      ——浪兮"),
    string("幻隅的开发工具：键盘、咖啡、和一点点的疯狂                          ——浪兮"),
    string("这个游戏没有内购！因为连商店系统都没做呢                            ——浪兮"),
    string("记住：在代码的世界里，一切皆有可能（包括崩溃）                      ——浪兮"),
    string("我们的地形生成算法：让每个世界都独一无二（理论上）                    ——浪兮"),
    string("如果你看到这个，说明我们的随机提示系统工作正常！                    ——浪兮"),
    string("幻隅的终极目标：成为一个真正的游戏（还在努力）                      ——浪兮"),
    string("这个提示存在的唯一理由：凑数                                        ——浪兮"),
    string("游戏里的时间流逝：和现实一样快（当我们没优化时）                    ——浪兮"),
    string("如果你能读懂这些提示，说明你的中文水平不错！                        ——浪兮"),
    string("幻隅的小知识：'隅'字读yú，不读ǒu哦！                                ——浪兮"),
    string("这个游戏是用爱（和大量咖啡）发电的                                  ——浪兮"),
    string("记住：每个伟大的游戏都是从'Hello World'开始的                        ——浪兮"),
    string("我们的代码风格：能运行的就是好代码！                                ——浪兮"),
    string("如果你在游戏里感到孤独...抱歉，NPC系统还没做                        ——浪兮"),
    string("幻隅的天气预报：今天代码晴朗，偶尔有bug雨                            ——浪兮"),
    string("游戏里的物理引擎：牛顿看了会沉默，爱因斯坦看了会流泪                ——浪兮"),
    string("如果你觉得游戏太简单...你可以尝试用脚玩！                            ——浪兮"),
    string("幻隅的开发日志：第N次尝试让游戏不崩溃                                ——浪兮"),
    string("这个游戏的内存使用量：比你想象的多一点                              ——浪兮"),
    string("记住：在幻隅里，你不是在玩游戏，而是在测试代码                      ——浪兮"),
    string("我们的渲染距离：足够你看清眼前的bug                                  ——浪兮"),
    string("如果你看到重复的提示，那是随机数生成器的锅                            ——浪兮"),
    string("幻隅的哲学问题：如果方块在虚空里，它会有碰撞体积吗？                  ——浪兮"),
    string("这个游戏的最佳游玩时间：当你不想写作业的时候                          ——浪兮"),
    string("游戏里的方块类型：足够你玩一整天（大概）                            ——浪兮"),
    string("如果你觉得提示太多...抱歉，我就是这么话痨                            ——浪兮"),
    string("幻隅的隐藏功能：按Alt+F4获得神秘加成！（别真的按）                  ——浪兮"),
    string("这个游戏的音效系统：静音是最好的音效                                ——浪兮"),
    string("记住：每个幻隅玩家都是我们的beta测试员                              ——浪兮"),
    string("我们的代码仓库：充满了TODO和FIXME注释                                ——浪兮"),
    string("如果你能数清这里有多少条提示，我给你点赞！                          ——浪兮"),
    string("幻隅的物理模拟：90%的时间在模拟，10%的时间在穿模                    ——浪兮"),
    string("这个游戏的教育意义：让你更珍惜流畅的游戏体验                        ——浪兮"),
    string("游戏里的色彩搭配：程序员审美，敬请谅解                                ——浪兮"),
    string("如果你觉得这些提示很有趣...谢谢！我尽力了                            ——浪兮"),
    string("幻隅的最终目标：不崩溃地运行超过一小时                                ——浪兮"),
    string("这个提示是最后一条...才怪，后面还有呢！                              ——浪兮"),
    string("记住：在幻隅里，快乐不是目标，不崩溃就是胜利                        ——浪兮"),
    string("我们的开发进度：完成度99%，剩下的1%是bug修复                        ——浪兮"),
    string("如果你看到了这条提示，恭喜你发现了隐藏成就！                          ——浪兮"),
    string("幻隅的小秘密：其实开发者自己都经常卡在墙里                            ——浪兮"),
    string("这个游戏的最佳搭档：一杯热饮和耐心                                  ——浪兮"),
    string("游戏里的世界观：由随机数和算法构成的多重宇宙                          ——浪兮"),
    string("如果你觉得游戏加载慢...想象一下我们编译代码的时间！                  ——浪兮"),
    string("幻隅的座右铭：代码虐我千百遍，我待代码如初恋                          ——浪兮"),
    string("这个提示证明：我们的随机数生成器还没崩溃                              ——浪兮"),
    string("记住：每个幻隅方块都渴望被放置（或者被破坏）                          ——浪兮"),
    string("我们的代码质量：经过严格测试（指能运行）                              ——浪兮"),
    string("如果你喜欢这个游戏...告诉你的朋友！如果它有朋友功能的话              ——浪兮"),
    string("幻隅的天气系统v2.0：增加了'代码雨'特效                              ——浪兮"),
    string("这个游戏没有微交易，因为我们连交易系统都没做                          ——浪兮"),
    string("游戏里的物理参数可以调整(真的)，如果你能找到设置界面的话              ——浪兮"),
    string("幻隅的隐藏结局：当你关闭游戏时触发(是真是假试试才知道)                ——浪兮"),
    string("这个游戏是用C++写的，所以...它有点脾气很正常                          ——浪兮"),
    string("记住：在幻隅里，想象力是你的超能力（也是我们的借口）                  ——浪兮"),
    string("我们的最终幻想：代码一次编写，到处运行                                ——浪兮"),
    string("如果你读到了这里，你赢了！游戏结束！                                  ——浪兮"),
    string("想知道幻隅的下一个bug长什么样吗？抖音@浪兮有点浪，提前围观！          ——浪兮"),
    string("抖音@浪兮有点浪 最近更新：'程序员是如何把简单问题复杂化的'            ——浪兮"),
    string("幻隅开发日记都在抖音@浪兮有点浪，欢迎来见证我是如何头秃的            ——浪兮"),
    string("关注抖音@浪兮有点浪，解锁幻隅开发者的日常崩溃实录！                  ——浪兮"),
    string("抖音@浪兮有点浪 最新视频：尝试给幻隅加新功能，结果加了新bug          ——浪兮"),
    string("如果你觉得幻隅的物理很奇怪，来抖音@浪兮有点浪 看我调试过程更奇怪      ——浪兮"),
    string("幻隅的每个版本更新，都会在抖音@浪兮有点浪 提前'翻车演示'              ——浪兮"),
    string("关注抖音@浪兮有点浪，看我用代码演绎什么叫'理想很丰满，现实很骨感'    ——浪兮"),
    string("我的抖音@浪兮有点浪 简介：专业制造bug，业余修复bug                    ——浪兮"),
    string("微信视频号搜索'浪兮有点浪'，看幻隅开发者的摸鱼日常（bushi）          ——浪兮"),
    string("视频号'浪兮有点浪'最近更新：幻隅里的方块穿模实录                      ——浪兮"),
    string("关注视频号'浪兮有点浪'，解锁幻隅开发幕后花絮大合集！                  ——浪兮"),
    string("我的视频号'浪兮有点浪'：记录一个程序员试图做游戏的全过程            ——浪兮"),
    string("视频号'浪兮有点浪'最新动态：幻隅的渲染优化（从卡顿到稍微不卡顿）      ——浪兮"),
    string("B站关注'浪兮有点浪'，看幻隅的技术开发过程！                          ——浪兮"),
    string("关注B站'浪兮有点浪'，幻隅的技术细节我慢慢道来（with吐槽）            ——浪兮"),
    string("B站'浪兮有点浪'专栏：幻隅开发中的10个离谱bug和0个有效修复          ——浪兮"),
    string("抖音@浪兮有点浪 看搞笑日常，B站@浪兮有点浪学技术，我全都要！        ——浪兮"),
    string("关注抖音@浪兮有点浪 和视频号@浪兮有点浪，幻隅开发双倍快乐！          ——浪兮"),
    string("抖音@浪兮有点浪 发段子，在B站@浪兮有点浪讲技术，精分日常            ——浪兮"),
    string("幻隅的最新消息：抖音@浪兮有点浪 首发，B站@浪兮有点浪详解            ——浪兮"),
    string("关注抖音@浪兮有点浪 看幻隅怎么崩，关注B站@浪兮有点浪看怎么修        ——浪兮"),
    string("你在幻隅里发现的bug，可能已经出现在抖音@浪兮有点浪 的最新视频里      ——浪兮"),
    string("关注抖音@浪兮有点浪，参与幻隅新功能投票！（虽然我不一定听）          ——浪兮"),
    string("抖音@浪兮有点浪 评论区经常有好玩的幻隅修改建议，欢迎来玩！          ——浪兮"),
    string("抖音@浪兮有点浪 最新系列(in imagination)：《幻隅bug的100种死法》    ——浪兮"),
    string("B站@浪兮有点浪，学习如何优雅地掩饰幻隅里的穿模bug                    ——浪兮"),
    string("视频号'浪兮有点浪'见证：我是如何把简单游戏做得如此复杂的              ——浪兮"),
    string("抖音@浪兮有点浪 日常：写代码，出bug，拍大腿，循环播放                ——浪兮"),
    string("关注抖音@浪兮有点浪，提前知道幻隅的下个版本会新增什么bug！            ——浪兮"),
    string("B站@浪兮有点浪有幻隅的完整开发教程，从入门到放弃                    ——浪兮"),
    string("视频号@浪兮有点浪：幻隅开发者的真实日常，绝对不掺假                  ——浪兮"),
    string("抖音@浪兮有点浪 教你：如何把3小时能做完的游戏拖成3个月              ——浪兮"),
    string("B站@浪兮有点浪，解锁幻隅里那些你没发现的隐藏'特性'                  ——浪兮"),
    string("视频号@浪兮有点浪 真实记录：幻隅从一行'Hello World'到能运行的全过程  ——浪兮"),
    string("最后的小广告：关注抖音@浪兮有点浪，看更多幻隅开发趣事！              ——浪兮"),
    string("我的创作日常：写幻隅代码，拍抖音@浪兮有点浪 视频，无限循环          ——浪兮"),
    string("幻隅和抖音@浪兮有点浪，一个能玩一个能看                              ——浪兮"),
    string("幻隅的代码行数：足够绕地球一圈（如果打印出来的话）                    ——浪兮"),
    string("我们的版本控制：主要靠Ctrl+C和Ctrl+V                                  ——浪兮"),
    string("游戏里的随机数：比我的头发分布更均匀                                  ——浪兮"),
    string("如果你发现bug，恭喜你成为了我们的免费QA！                            ——浪兮"),
    string("这个游戏的优化：把‘优化TODO’写在了注释里                            ——浪兮"),
    string("幻隅的架构设计：能跑起来就是好架构！                                  ——浪兮"),
    string("我们的开发会议：90%时间在讨论中午吃什么                              ——浪兮"),
    string("游戏里的碰撞检测：有时候检测，有时候碰撞，有时候...随缘              ——浪兮"),
    string("如果你觉得游戏太简单，试试用脑子控制角色！                            ——浪兮"),
    string("幻隅的物理模拟：牛顿定律在这里只是建议                                ——浪兮"),
    string("这个游戏的内存管理：主要靠重启解决(别搞泄露了)                        ——浪兮"),
    string("我们的代码注释：比小说还精彩（自认为）                                ——浪兮"),
    string("游戏里的光影效果：有光，有影，就是没效果                              ——浪兮"),
    string("幻隅的开发工具链：VS Code、咖啡、和玄学                              ——浪兮"),
    string("这个游戏的AI系统：暂时由玩家脑补                                      ——浪兮"),
    string("我们的测试流程：写完代码 -> 祈祷 -> 运行                              ——浪兮"),
    string("游戏里的水面效果：还在我们的‘有朝一日’清单上                        ——浪兮"),
    string("如果你看到奇怪的图形，那是抽象艺术！                                  ——浪兮"),
    string("幻隅的性能分析：fps > 0 就是胜利                                    ——浪兮"),
    string("这个游戏的存档系统：比我的记忆还可靠（大概）                          ——浪兮"),
    string("游戏里的天气系统：永远‘程序员天气’——阴转bug                        ——浪兮"),
    string("如果你觉得游戏无聊，可以数渲染的四边形数量                            ——浪兮"),
    string("这个游戏的难度曲线：像心电图一样刺激                                  ——浪兮"),
    string("我们的debug过程：在代码里加入更多print                                ——浪兮"),
    string("游戏里的粒子效果：一个像素点也是粒子！                                ——浪兮"),
    string("如果你发现隐藏房间，那肯定是渲染错误                                  ——浪兮"),
    string("幻隅的网络模块：还在单机版阶段（理直气壮）                            ——浪兮"),
    string("这个游戏的UI设计：程序员审美，不服来辩                                ——浪兮"),
    string("我们的项目管理：Deadline是第一生产力                                ——浪兮"),
    string("游戏里的植被系统：草方块就是我们的森林                                ——浪兮"),
    string("如果你觉得操作反人类，恭喜你发现了真相                                ——浪兮"),
    string("幻隅的反射效果：镜面反射？我们连镜子都没有                            ——浪兮"),
    string("这个游戏的支持多语言：中文和...更多中文                              ——浪兮"),
    string("我们的版本号：每次commit就+0.01                                      ——浪兮"),
    string("游戏里的水体物理：石头会浮起来是特性！                                ——浪兮"),
    string("如果你能通关，我给你写个‘无敌’成就                                  ——浪兮"),
    string("幻隅的阴影计算：影子是什么？可以吃吗？                                ——浪兮"),
    string("我们的代码规范：能通过编译就是规范                                    ——浪兮"),
    string("如果你看到方块在跳舞，可能是显卡在求救                                ——浪兮"),
    string("幻隅的物理材质：都是心理作用                                          ——浪兮"),
    string("这个游戏的成就系统：活着就是最大的成就                                ——浪兮"),
    string("我们的团队协作：主要通过‘你代码写完了吗？’交流                        ——浪兮"),
    string("游戏里的天空盒：其实是贴了一张蓝天jpg                                ——浪兮"),
    string("如果你觉得游戏太真实，该休息一下了                                    ——浪兮"),
    string("幻隅的LOD系统：远了就看不见，多么合理                                ——浪兮"),
    string("这个游戏的MOD支持：请直接修改源代码                                  ——浪兮"),
    string("我们的错误处理：try-catch-everything                                  ——浪兮"),
    string("游戏里的体积雾：等显卡发明出来再做                                    ——浪兮"),
    string("如果你发现无限掉落bug，就当在体验蹦极                                ——浪兮"),
    string("幻隅的着色器：用爱渲染每一帧                                          ——浪兮"),
    string("这个游戏的镜头控制：有时候听话，有时候叛逆                            ——浪兮"),
    string("我们的功能路线图：先实现，再优化，最后重写                            ——浪兮"),
    string("游戏里的回声效果：自己喊‘啊——’然后听回声                            ——浪兮"),
    string("如果你觉得加载慢，正好可以练习冥想                                    ——浪兮"),
    string("这个游戏的反作弊：我们相信玩家的自觉                                  ——浪兮"),
    string("我们的性能优化：把fps显示关掉就流畅了                                ——浪兮"),
    string("游戏里的昼夜交替：手动调节系统时间吧                                  ——浪兮"),
    string("如果你能找出所有bug，聘请你当首席测试                                ——浪兮"),
    string("幻隅的动态难度：电脑越卡游戏越难                                      ——浪兮"),
    string("这个游戏的保存提醒：现在这条就是！                                    ——浪兮"),
    string("我们的代码审查：自己写的代码，含泪也要跑完                            ——浪兮"),
    string("游戏里的物理破坏：破坏是永恒的，修复是暂时的                          ——浪兮"),
    string("如果你觉得游戏太简单，闭上眼睛玩                                      ——浪兮"),
    string("这个游戏的震动反馈：请自己摇晃显示器                                  ——浪兮"),
    string("我们的内存优化：重启大法好                                            ——浪兮"),
    string("如果你遇到崩溃，恭喜触发了隐藏退出功能                                ——浪兮"),
    string("我们的用户反馈：‘这游戏能玩吗？’ ->‘能！’                            ——浪兮"),
    string("游戏里的软阴影：硬件不够，脑补来凑                                    ——浪兮"),
    string("如果你觉得提示太多，说明你需要更多提示                                ——浪兮"),
    string("幻隅的体积光：需要体积光吗？需要先有光                                ——浪兮"),
    string("如果你看到未来科技，那是显卡渲染错误                                  ——浪兮"),
    string("幻隅的音频定位：左声道是左，右声道是右                                ——浪兮"),
    string("这个游戏的拍照模式：用手机拍屏幕就行                                  ——浪兮"),
    string("我们的压力测试：一边玩游戏一边写作业                                  ——浪兮"),
    string("游戏里的景深效果：近视玩家自带此功能                                  ——浪兮"),
    string("如果你能坚持玩到最后，我敬你是条汉子                                  ——浪兮"),
    string("幻隅的VR支持：把显示器贴在脸上即可                                    ——浪兮"),
    string("这个游戏的速通记录：从启动到崩溃最短时间                              ——浪兮"),
    string("如果你觉得游戏太假，说明你太认真了                                    ——浪兮"),
    string("幻隅的物理精度：浮点误差也是游戏特色                                  ——浪兮"),
    string("这个游戏的云存档：想象一下存在云端                                    ——浪兮"),
    string("我们的迭代开发：永远在开发，从未完成过                                ——浪兮"),
    string("游戏里的热浪效果：电脑散热器全力运转时获得                            ——浪兮"),
    string("如果你发现彩蛋，那一定是不小心写进去的bug                            ——浪兮"),
    string("幻隅的HDR渲染：High Definition 做梦                                  ——浪兮"),
    string("我们的用户协议：我同意（反正没人看）                                  ——浪兮"),
    string("游戏里的镜头光晕：盯着灯泡看然后闭上眼睛                              ——浪兮"),
    string("如果你觉得这些提示有用，说明你很有耐心                                ——浪兮"),
    string("幻隅的终极目标：成为一个梗                                            ——浪兮"),
    string("这个游戏的社区支持：自言自语也是社区                                  ——浪兮"),
    string("游戏里的动态天气：根据电脑温度变化                                    ——浪兮"),
    string("如果你看到了这条，你已经读了太多提示了                                ——浪兮"),
    string("这个游戏的生命周期：比咖啡因的作用时间还短                            ——浪兮"),
    string("如果你还在读提示，该去玩游戏了！                                      ——浪兮"),
    string("幻隅的遗产：一堆需要维护的代码                                        ——浪兮"),
    string("修复bug的过程很痛苦吗？是的，很痛苦                                   ——多茵"),
    string("我从不开灯————我只把黑暗关掉                                      ——多茵"),
    string("幻隅的世界中其实没有进化论，只有一张“多茵允许活下来的生物”列表      ——多茵"),
    string("快速建造技巧：一边按鼠标一边唱rap，节奏感很重要                      ——浪兮"),
    string("遇到复杂地形怎么办？建个楼梯，或者直接飞过去                        ——浪兮"),
    string("幻隅摄影指南：找到好角度，截图，然后假装是艺术品                      ——浪兮"),
    string("想要更流畅的体验？试试闭上眼睛，用想象力玩游戏                        ——浪兮"),
    string("游戏里的光影效果：有时候有光，有时候有影，很少同时有                  ——浪兮"),
    string("键盘快捷键大全：W前进，S后退，A左移，D右移，空格...上天！              ——浪兮"),
    string("幻隅物理学第一定律：方块可以穿墙，但玩家不行                          ——浪兮"),
    string("遇到bug不要慌，先拍个照发朋友圈，然后重启游戏                        ——浪兮"),
    string("最佳游戏时间：凌晨3点，当世界安静时，与幻隅独处                      ——浪兮"),
    string("想要挑战自我？试试不用鼠标玩幻隅，只用键盘操作                        ——浪兮"),
    string("建筑规划建议：先画图纸，然后发现图纸比建筑好看                        ——浪兮"),
    string("幻隅的隐藏功能：有些功能隐藏得太深，连开发者都忘了                    ——浪兮"),
    string("遇到困难怎么办？深呼吸，喝口水，然后查攻略（如果存在）                ——浪兮"),
    string("游戏里的色彩理论：红色是热情，蓝色是冷静，绿色...是草方块            ——浪兮"),
    string("想要快速升级？多探索，多建造，多...等等，我们没有等级系统            ——浪兮"),
    string("幻隅的多人模式：想象一下和朋友一起玩，虽然现在还不行                  ——浪兮"),
    string("游戏里的物理引擎：遵循牛顿定律，除了当它不遵循的时候                  ——浪兮"),
    string("想要更多方块？等更新，或者自己写代码添加（开玩笑的）                  ——浪兮"),
    string("幻隅的音乐系统：背景音乐循环播放，直到你把它关掉                      ——浪兮"),
    string("游戏里的水体效果：等我们有了水，再讨论这个                            ——浪兮"),
    string("最佳游戏姿势：坐直，放松，然后慢慢滑到‘葛优躺’                      ——浪兮"),
    string("游戏里的昼夜循环：白天工作，晚上休息，和现实一样                      ——浪兮"),
    string("想要更真实的体验？把房间灯关掉，拉上窗帘，沉浸式游戏                  ——浪兮"),
    string("幻隅的建筑材料：石头坚固，木头温暖，空气...无处不在                  ——浪兮"),
    string("游戏里的动物系统：等我们有了动物，再写相关提示                        ——浪兮"),
    string("遇到地形bug怎么办？绕着走，或者把它当成特色景观                      ——浪兮"),
    string("幻隅的交通工具：目前只有双腿，未来可能有更多（可能）                  ——浪兮"),
    string("想要更刺激的体验？在雷雨天玩游戏，增加氛围感                          ——浪兮"),
    string("幻隅的物理互动：推方块，拉方块，然后发现方块不动                      ——浪兮"),
    string("游戏里的声音效果：有的声音来自游戏，有的来自你的想象                  ——浪兮"),
    string("最佳游戏伙伴：一杯热饮，一份零食，和一颗不怕bug的心                  ——浪兮"),
    string("幻隅的图形设置：调高享受美景，调低享受流畅，自己权衡                  ——浪兮"),
    string("遇到操作困难？多练习，或者换个更顺手的鼠标键盘                        ——浪兮"),
    string("幻隅的生态平衡：草方块需要土，土需要石头，石头需要...程序员          ——浪兮"),
    string("想要更自由的建造？使用空白画布模式，创造属于你的世界                  ——浪兮"),
    string("幻隅的水下世界：等我们有了水，再探索水下（重复提醒）                  ——浪兮"),
    string("游戏里的魔法系统：念咒语，挥魔杖，然后按键盘上的快捷键                ——浪兮"),
    string("最佳游戏时长：一小时休息一下，或者玩到忘记时间                        ——浪兮"),
    string("幻隅的碰撞检测：大多数时候工作，偶尔休息，总是让人惊喜                ——浪兮"),
    string("想要更好的性能？关闭后台程序，或者升级硬件（建议后者）                ——浪兮"),
    string("幻隅的剧情模式：主角醒来，发现自己在幻隅，然后...自由发挥            ——浪兮"),
    string("游戏里的光影交互：光与影的舞蹈，由你的显卡决定质量                    ——浪兮"),
    string("遇到画面撕裂？尝试垂直同步，或者接受这抽象的艺术风格                  ——浪兮"),
    string("幻隅的物理材质：有的硬，有的软，有的...无法描述                      ——浪兮"),
    string("游戏里的声音定位：左耳听左，右耳听右，双耳听...前方？                ——浪兮"),
    string("想要更多内容？等待更新，或者加入开发团队（开玩笑的）                  ——浪兮"),
    string("游戏里的植物生长：需要时间，需要耐心，需要...我们还没做              ——浪兮"),
    string("最佳游戏环境：安静的房间，舒适的椅子，和不会打扰你的人                ——浪兮"),
    string("游戏里的天气预测：看天气预报，或者直接启动游戏看看                    ——浪兮"),
    string("想要更丰富的色彩？调整显示器设置，或者戴上有色眼镜                    ——浪兮"),
    string("幻隅的地形生成：随机但不完全随机，自然但不完全自然                    ——浪兮"),
    string("游戏里的物理模拟：真实物理，简化物理，和‘差不多就行’物理            ——浪兮"),
    string("遇到存档问题？多存几个档，或者相信自动存档（如果存在）                ——浪兮"),
    string("幻隅的音频效果：环境音，效果音，和程序员哼歌的音                      ——浪兮"),
    string("游戏里的光影计算：简单，复杂，和‘这样应该可以吧’                    ——浪兮"),
    string("想要更深入的游戏？阅读代码注释，了解背后的故事                        ——浪兮"),
    string("幻隅的渲染距离：近处清晰，远处模糊，非常远处...不存在                ——浪兮"),
    string("游戏里的时间系统：游戏时间，现实时间，和加载时间                      ——浪兮"),
    string("最佳游戏心态：放松，享受，不要因为bug生气（很难）                    ——浪兮"),
    string("幻隅的最终建议：玩得开心                                              ——浪兮"),
    string("游戏里的时间流逝：有时候快，有时候慢，有时候停止                  ——浪兮"),
    string("想要更真实的游戏世界？去户外走走，或者继续玩幻隅                  ——浪兮"),
    string("幻隅的渲染技术：我们尽力了，真的                                 ——浪兮"),
    string("为什么游戏里没有水？水是生命之源，也是游戏难题之一                  ——浪兮"),
    string("最佳游戏心态：保持好奇，保持耐心，保持幽默感                      ——浪兮"),
    string("幻隅的更新目标：让游戏更好玩，更稳定，更少bug                     ——浪兮"),
    string("游戏里的光影效果：我们追求真实，但有时候也追求艺术                ——浪兮"),
    string("幻隅的更新频率：比天气变化还难预测，但比星座运势准一点              ——浪兮"),
    string("游戏里的重力系统：大多数时候向下，偶尔向上，有时向奇怪方向        ——浪兮"),
    string("想要快速旅行？试试传送功能，或者直接修改坐标值（不推荐但可行）    ——浪兮"),
    string("幻隅的阴影质量：取决于你的显卡，以及你愿意等多久                  ——浪兮"),
    string("游戏里的反射效果：能看到自己倒影，如果你想象力足够丰富            ——浪兮"),
    string("遇到游戏崩溃？深呼吸，这很正常，我们已经习惯了                  ——浪兮"),
    string("幻隅的物理反馈：有的真实，有的虚假，有的完全靠猜                  ——浪兮"),
    string("想要更多游戏模式？等我们想到新点子，或者你给我们建议              ——浪兮"),
    string("幻隅的天气变化：晴天、雨天、雪天，和“代码下bug”天              ——浪兮"),
    string("幻隅的音效设计：有的来自素材库，有的来自aigc，有的来自我          ——浪兮"),
    string("想要更好的画质？升级显卡，或者降低期望值（后者更经济）            ——浪兮"),
    string("幻隅的碰撞体积：有时候精确到像素，有时候忽略不计                  ——浪兮"),
    string("游戏里的云层效果：一层，两层，三层，或者干脆没有                  ——浪兮"),
    string("遇到操作延迟？检查网络，检查硬件，然后检查是不是在做梦            ——浪兮"),
    string("幻隅的渲染技术：先进，落后，或者“能用就行”                      ——浪兮"),
    string("想要更丰富的剧情？自己编一个，或者等我们写完（遥遥无期）          ——浪兮"),
    string("幻隅的物理模拟：有的遵循科学，有的遵循玄学，有的随缘              ——浪兮"),
    string("游戏里的声音传播：近大远小，左耳进右耳出，或者根本听不到          ——浪兮"),
    string("最佳游戏时间：当你无聊的时候，或者当你应该做正事的时候            ——浪兮"),
    string("幻隅的画面风格：简约，复杂，或者“我也不知道算什么”              ——浪兮"),
    string("游戏里的天气影响：影响心情，不影响游戏（因为还没做）              ——浪兮"),
    string("想要更多成就？完成游戏里的挑战，或者自己给自己颁奖                ——浪兮"),
    string("幻隅的物理引擎：有的部分很强大，有的部分很脆弱，有的不存在        ——浪兮"),
    string("游戏里的色彩搭配：有的和谐，有的刺眼，有的让你怀疑人生            ——浪兮"),
    string("遇到游戏卡顿？降低画质，关闭程序，或者换一台新电脑                ——浪兮"),
    string("幻隅的音效质量：有的清晰，有的模糊，有的像被打包过                ——浪兮"),
    string("想要更真实的物理？去现实世界体验，或者在游戏里将就                ——浪兮"),
    string("幻隅的渲染距离：近处细节丰富，远处马赛克，非常远是想象            ——浪兮"),
    string("游戏里的水面效果：平静，波澜，或者“我们真的没有水”              ——浪兮"),
    string("最佳游戏设置：默认设置，或者调到你眼睛舒服为止                    ——浪兮"),
    string("幻隅的更新内容：修复bug，增加bug，或者两者都有                    ——浪兮"),
    string("想要更多的方块？等待更新，或者自己画贴图（我们可能不采用）        ——浪兮"),
    string("幻隅的物理互动：推，拉，抬，压，然后发现纹丝不动                  ——浪兮"),
    string("遇到画面闪烁？更新驱动，或者接受这闪亮的艺术风格                  ——浪兮"),
    string("想要更流畅的操作？练习手速，或者买个电竞鼠标（不一定有用）        ——浪兮"),
    string("幻隅的渲染效果：有的惊艳，有的普通，有的让你想关掉                ——浪兮"),
    string("最佳游戏体验：不要期望太高，这样每次都是惊喜                      ——浪兮"),
    string("幻隅的更新日志：修复了一些bug，增加了一些bug，优化了一些代码      ——浪兮"),
    string("想要更多的游戏内容？耐心等待，或者加入开发（我们缺人）            ——浪兮"),
    string("幻隅的物理精度：有时候精确，有时候近似，有时候全错                ——浪兮"),
    string("游戏里的色彩还原：真实，失真，或者“我觉得这样好看”              ——浪兮"),
    string("遇到游戏错误？查看错误日志，或者直接忽略（不建议）                ——浪兮"),
    string("幻隅的更新速度：有时一周，有时一月，有时一年                      ——浪兮"),
    string("游戏里的光影追踪：需要光，需要影，需要追踪，需要钱                ——浪兮"),
    string("想要更多的游戏模式？创意模式，生存模式，或者乱来模式              ——浪兮"),
    string("幻隅的物理系统：有的真实，有的魔幻，有的不存在                    ——浪兮"),
    string("遇到游戏崩溃？重启游戏，重启电脑，或者重启人生(不建议)            ——浪兮"),
    string("游戏里的时间流逝：游戏内时间，现实时间，和“感觉过了好久”时间    ——浪兮"),
    string("幻隅的更新内容：新功能，新bug，新特性，新问题                      ——浪兮"),
    string("游戏里的光影效果：全局光，局部光，或者没有光(如果你这样设定的话)  ——浪兮"),
    string("想要更多的游戏挑战？自己设定目标，或者等我们添加                  ——浪兮"),
    string("幻隅的物理模拟：有的部分很满意，有的部分想重写，有的部分想放弃    ——浪兮"),
    string("遇到游戏卡死？强制关闭，或者等它自己恢复（可能需要很久）          ——浪兮"),
    string("最佳游戏建议：多存档，多备份，多祈祷                              ——浪兮"),
    string("幻隅的更新频率：看心情，看时间，看有没有灵感                      ——浪兮"),
    string("游戏里的光影效果：有的地方亮，有的地方暗，有的地方刚刚好          ——浪兮"),
    string("幻隅的物理引擎：有的部分很出色，有的部分很糟糕，有的部分没做      ——浪兮"),
    string("游戏里的色彩表现：鲜艳，柔和，或者“这是什么颜色”                ——浪兮"),
    string("幻隅的音效质量：有的专业，有的业余，有的搞笑                      ——浪兮"),
    string("想要更深入的了解？阅读文档，查看源码，或者问我（可能不回）        ——浪兮"),
    string("幻隅的渲染效果：有的令人赞叹，有的令人失望，有的令人困惑          ——浪兮"),
    string("最佳游戏态度：享受过程，不要在意结果                              ——浪兮"),
    string("幻隅的更新承诺：我们会不断更新，直到我们放弃（希望不会）          ——浪兮"),
    string("想要更多的游戏乐趣？和朋友一起玩，或者自己创造乐趣                ——浪兮"),
    string("游戏里的色彩搭配：协调，冲突，或者“随便配的”                    ——浪兮"),
    string("遇到游戏问题？上网搜索，或者问其他玩家（如果存在）                ——浪兮"),
    string("幻隅的音效环境：有的嘈杂，有的宁静，有的奇怪                      ——浪兮"),
    string("想要更好的游戏体验？升级硬件，或者升级你的耐心                    ——浪兮"),
    string("幻隅的渲染距离：看得见的渲染，看不见的不渲染，多么合理            ——浪兮"),
    string("最佳游戏时间：任何时间，只要你想玩                                ——浪兮"),
    string("幻隅的更新历史：从无到有，从有到多，从多到...还没完               ——浪兮"),
    string("游戏里的光影效果：有的真实，有的虚假，有的半真半假                ——浪兮"),
    string("想要更多的游戏元素？等我们添加，或者你自己想象                    ——浪兮"),
    string("幻隅的物理精度：高精度，低精度，或者“精度是什么”                ——浪兮"),
    string("游戏里的色彩还原：真实世界颜色，游戏世界颜色，或者混合            ——浪兮"),
    string("遇到游戏崩溃？别担心，这是幻隅的一部分，习惯就好                  ——浪兮"),
    string("听说在幻隅里连续跳跃100次能召唤隐藏BOSS...我编的                     ——浪兮"),
    string("听说在幻隅里连续跳跃100次能进入隐藏副本...我编的                     ——浪兮"),
    string("听说在幻隅里连续跳跃100次能开启远古遗迹...我编的                     ——浪兮"),
    string("听说在幻隅里连续跑动100格能召唤隐藏BOSS...我编的                     ——浪兮"),
    string("听说在幻隅里连续跑动100格能进入隐藏副本...我编的                     ——浪兮"),
    string("听说在幻隅里连续跑动100格能开启远古遗迹...我编的                     ——浪兮"),
    string("程序员の日常：写bug，修bug，创造新bug，循环往复                      ——浪兮"),
    string("当你凝视幻隅时，幻隅也在凝视你...的显卡温度                          ——浪兮"),
    string("当你凝视方块时，方块也在凝视你...的显卡温度                          ——浪兮"),
    string("有时候我在想，这些方块真的快乐吗？算了，代码没有感情                ——浪兮"),
    string("幻隅物理学第二定律：所有bug都会在最不合适的时候出现                ——浪兮"),
    string("你知道吗？幻隅的代码行数已经超过我大学四年的论文字数了(bushi)      ——浪兮"),
    string("深夜写代码的后果：第二天醒来发现昨晚写的都是什么鬼东西              ——浪兮"),
    string("我曾经也是个正常的程序员，直到我开始开发幻隅...                    ——浪兮"),
    string("有时候觉得，幻隅里的方块比我更有规律地生活着                        ——浪兮"),
    string("开发幻隅教会我一件事：永远不要相信第一次运行的代码                  ——浪兮"),
    string("我宣布幻隅是目前已知宇宙中bug最多的艺术品！                        ——浪兮"),
    string("写代码时：'这肯定没问题'。运行后：'这不可能！'                      ——浪兮"),
    string("幻隅玩家的三大美德：耐心，幽默感，和快速存档的习惯                  ——浪兮"),
    string("有时候我在幻隅里迷路，不是因为我方向感差，是地形生成太放飞          ——浪兮"),
    string("程序员浪漫：我把你的名字写进了幻隅的注释里，虽然没人看到            ——浪兮"),
    string("幻隅的物理引擎：90%科学，10%玄学，100%靠运气                      ——浪兮"),
    string("你知道吗？幻隅的每个bug都有一个独特的性格和故事                    ——浪兮"),
    string("当我看着幻隅运行时，就像父母看着孩子学走路，虽然经常摔倒            ——浪兮"),
    string("幻隅开发者的秘密：我们其实也不知道某些功能是怎么工作的              ——浪兮"),
    string("有时候我在想，如果我少写一个bug，幻隅会不会更受欢迎？              ——浪兮"),
    string("幻隅教会我的事：完美是不存在的，但'能运行'是可以追求的              ——浪兮"),
    string("程序员の哲学：代码即存在，bug即本质，编译通过即真理                ——浪兮"),
    string("在幻隅里，每一个方块都有它的位置，除了那些穿墙而过的                ——浪兮"),
    string("我花了一整天优化幻隅的渲染，结果帧数提升了0.5...值得！              ——浪兮"),
    string("幻隅玩家的日常：探索，建造，崩溃，重启，repeat                     ——浪兮"),
    string("有时候我觉得，幻隅不是我在开发它，是它在开发我的耐心                ——浪兮"),
    string("你知道吗？幻隅的柏林噪声算法比我的人生规划还要随机                  ——浪兮"),
    string("程序员式安慰：至少你的游戏崩溃次数还没有我的代码错误多              ——浪兮"),
    string("幻隅的终极谜题：为什么有时候它运行得很好，有时候又不行？            ——浪兮"),
    string("我在幻隅里建了一座城堡，然后它因为浮点误差开始慢慢下沉...            ——浪兮"),
    string("开发幻隅就像养宠物：有时候很可爱，有时候很麻烦，但总是值得          ——浪兮"),
    string("幻隅玩家等级：初级→卡在墙里，中级→自己爬出来，高级→习惯在墙里      ——浪兮"),
    string("有时候我会梦到幻隅的代码，然后在半夜醒来记下那个绝妙的bug修复       ——浪兮"),
    string("幻隅的物理模拟：让牛顿安息，让爱因斯坦困惑，让程序员头秃            ——浪兮"),
    string("你知道吗？幻隅的每一行代码都蕴含着我的汗水...和泪水                 ——浪兮"),
    string("程序员式幽默：把段子写进游戏提示里，然后期待有人会笑                ——浪兮"),
    string("幻隅教会我的编程真谛：如果它没坏，就别去碰它                       ——浪兮"),
    string("有时候我在幻隅里什么都不做，只是看着方块们...然后游戏崩溃了         ——浪兮"),
    string("幻隅开发者的骄傲：我们的bug数量遥遥领先于功能数量！                 ——浪兮"),
    string("你知道吗？幻隅的渲染引擎比我家的老电视还要有复古感                  ——浪兮"),
    string("程序员式表白：我对你的爱就像幻隅里的bug，永远修复不完               ——浪兮"),
    string("幻隅的哲学：存在即合理，bug即特色，崩溃即常态                      ——浪兮"),
    string("我在幻隅里创造了一个世界，然后发现这个世界有自己的想法...            ——浪兮"),
    string("开发幻隅的三大阶段：热情→绝望→接受现实                            ——浪兮"),
    string("你知道吗？幻隅的每个版本更新，都是我与bug战斗的新篇章              ——浪兮"),
    string("程序员式乐观：至少幻隅还能启动，不是吗？                           ——浪兮"),
    string("幻隅玩家的终极成就：连续游玩一小时而没有遇到任何致命bug            ——浪兮"),
    string("有时候我觉得，幻隅不是游戏，是我与计算机的对话记录                  ——浪兮"),
    string("幻隅开发者的日常饮食：咖啡，泡面，和偶尔的外卖                      ——浪兮"),
    string("你知道吗？幻隅的代码注释比我写给自己的备忘录还要详细                ——浪兮"),
    string("程序员式谦逊：'这个功能可能不太稳定'＝'准备好迎接崩溃吧'            ——浪兮"),
    string("幻隅的天气系统：根据我的心情变化，但我总是很焦虑                    ——浪兮"),
    string("我在幻隅里建了一个迷宫，然后自己迷路了...这很合理                    ——浪兮"),
    string("开发幻隅就像写日记：记录了我的成长，也记录了我的挣扎                ——浪兮"),
    string("幻隅玩家的共同特征：对bug的容忍度异于常人                          ——浪兮"),
    string("你知道吗？幻隅的某些'特性'其实是我懒得修复的bug                     ——浪兮"),
    string("程序员式浪漫：我把星空搬进了幻隅，虽然星星有时候会掉下来            ——浪兮"),
    string("幻隅的终极目标：成为一个让人又爱又恨的存在                         ——浪兮"),
    string("有时候我在幻隅里感到孤独，然后意识到我是这里唯一的活物...            ——浪兮"),
    string("幻隅开发者的自我安慰：每个伟大的游戏都有bug，我们只是比较多         ——浪兮"),
    string("你知道吗？幻隅的加载时间足够我泡一杯咖啡，然后回来继续等            ——浪兮"),
    string("程序员式智慧：最好的代码是不需要写的代码，但我还是写了很多          ——浪兮"),
    string("幻隅的物理定律：有时候遵守牛顿，有时候遵守我                        ——浪兮"),
    string("我在幻隅里创造了一个角色，然后他开始自己乱跑...这算AI吗？            ——浪兮"),
    string("开发幻隅教会我：完美是优秀的敌人，但bug是所有人的敌人               ——浪兮"),
    string("幻隅玩家的特权：体验其他游戏无法提供的独特崩溃方式                  ——浪兮"),
    string("你知道吗？幻隅的某些设计决策，是我在凌晨3点做出的...难怪            ——浪兮"),
    string("程序员式幽默：把崩溃画面做得比游戏本身还有艺术感                    ——浪兮"),
    string("幻隅的存在意义：证明即使不完美，也可以很有趣                        ——浪兮"),
    string("有时候我在幻隅里思考人生，然后被一个突如其来的bug打断              ——浪兮"),
    string("幻隅开发者的日常：与bug斗，其乐无穷                               ——浪兮"),
    string("你知道吗？幻隅的每个版本号，都代表着我又一次战胜了懒惰              ——浪兮"),
    string("程序员式自豪：我写的代码可能不完美，但它至少能运行（有时）          ——浪兮"),
    string("幻隅的渲染魔法：把数学公式变成视觉享受，虽然有时候公式写错了        ——浪兮"),
    string("我在幻隅里建了一个家，然后发现门打不开...这就是生活                 ——浪兮"),
    string("开发幻隅就像养育孩子：投入很多爱，收获很多头痛                      ——浪兮"),
    string("幻隅玩家的日常：探索未知，创造奇迹，然后面对现实                    ——浪兮"),
    string("程序员式哲学：代码如人生，充满了意外和惊喜                          ——浪兮"),
    string("幻隅的天气系统：永远猜不到下一刻是什么，就像我的代码                ——浪兮"),
    string("我在幻隅里度过的时间，比我学习编程的时间还要长...值得吗？            ——浪兮"),
    string("开发幻隅的真理：第一个版本永远是最糟糕的，但也是最珍贵的            ——浪兮"),
    string("幻隅玩家的共同语言：'你又卡在墙里了？' '是啊，老地方'              ——浪兮"),
    string("你知道吗？幻隅的音乐是我用键盘敲出来的节奏...字面意思                ——浪兮"),
    string("程序员式告白：我对代码的爱，就像幻隅对bug的依赖，永恒不变           ——浪兮"),
    string("幻隅的终极哲学：不完美才是真实，bug才是特色，崩溃才是常态           ——浪兮"),
    string("有时候我觉得，幻隅不是我开发的游戏，是游戏开发了我                  ——浪兮"),
    string("你知道吗？幻隅的每个方块，都承载着我的一个编程回忆                  ——浪兮"),
    string("程序员式幽默：把游戏提示写得比游戏本身还有趣                        ——浪兮"),
    string("幻隅的存在证明：即使是一堆bug，也可以组成一个世界                  ——浪兮"),
    string("我在幻隅里找到了平静，然后游戏崩溃了...这就是人生                   ——浪兮"),
    string("开发幻隅就像是一场冒险，目的地未知，但旅程难忘                      ——浪兮"),
    string("幻隅玩家的最高境界：把每个bug都当作游戏的隐藏功能                  ——浪兮"),
    string("你知道吗？幻隅的某些代码，连我自己都看不懂了...这很糟糕吧？          ——浪兮"),
    string("程序员式智慧：最优雅的解决方案往往是最简单的那种，但我没选          ——浪兮"),
    string("幻隅的物理引擎：试图模拟现实，但经常创造新现实                      ——浪兮"),
    string("我在幻隅里创造了一个故事，然后发现故事自己写了续集...                ——浪兮"),
    string("开发幻隅的感悟：有时候过程比结果更重要，交作业时除外              ——浪兮"),
    string("抖音@浪兮有点浪 告诉你：游戏开发者的日常就是与bug斗智斗勇          ——浪兮"),
    string("关注抖音@浪兮有点浪，看我如何把代码写成一首现代诗（充满bug的那种）——浪兮"),
    string("我的抖音@浪兮有点浪 内容：游戏开发、bug展示、以及偶尔的绝望        ——浪兮"),
    string("抖音@浪兮有点浪 记录了一个程序员试图创造世界的全过程              ——浪兮"),
    string("抖音@浪兮有点浪 见证幻隅从无到有的每一个阶段                      ——浪兮"),
    string("抖音@浪兮有点浪，看程序员如何用幽默化解debug的痛苦                ——浪兮"),
    string("抖音@浪兮有点浪 告诉你：每个游戏背后都有无数个不眠之夜            ——浪兮"),
    string("我的抖音@浪兮有点浪 简介：一个努力让游戏跑起来的普通开发者        ——浪兮"),
    string("抖音@浪兮有点浪，了解幻隅开发背后的故事                           ——浪兮"),
    string("抖音@浪兮有点浪 展示了游戏开发最真实的一面：不完美但真实          ——浪兮"),
    string("视频号@浪兮有点浪 分享：幻隅开发过程中的崩溃瞬间                   ——浪兮"),
    string("微信视频号@浪兮有点浪，记录一个游戏开发者的崩溃日常                 ——浪兮"),
    string("微信视频号@浪兮有点浪 看懂游戏项目如何成长                         ——浪兮"),
    string("视频号@浪兮有点浪：时而自信满满，时而怀疑人生                      ——浪兮"),
    string("视频号@浪兮有点浪 看懂游戏项目如何成长                              ——浪兮"),
    string("B站@浪兮有点浪 专栏：幻隅技术细节，虽然技术不一定高超(一定不高超?) ——浪兮"),
    string("关注B站@浪兮有点浪，看我如何用有限的技术实现无限的想法              ——浪兮"),
    string("B站@浪兮有点浪 的内容：一半是技术分享，一半是自我吐槽              ——浪兮"),
    string("抖音@浪兮有点浪 分享开发趣事，B站@浪兮有点浪 分享技术心得          ——浪兮"),
    string("抖音@浪兮有点浪 视频号@浪兮有点浪，从不同角度看幻隅开发            ——浪兮"),
    string("游戏创作：在抖音@浪兮有点浪 分享过程，在B站@浪兮有点浪 分享结果    ——浪兮"),
    string("抖音@浪兮有点浪 评论区已成为幻隅bug报告第二现场(???)                ——浪兮"),
    string("在抖音@浪兮有点浪 的评论区，你可以找到很多幻隅同好                  ——浪兮"),
    string("抖音@浪兮有点浪 不仅分享开发，也分享一个程序员的日常生活            ——浪兮"),
    string("在抖音@浪兮有点浪，发现游戏开发者的日常                             ——浪兮"),
    string("抖音@浪兮有点浪 的每一条视频都是幻隅成长走过的路                    ——浪兮"),
    string("B站@浪兮有点浪 的教程：如何用最朴素的方法实现游戏功能                ——浪兮"),
    string("视频号@浪兮有点浪 记录的是真实，分享的是心情，留下的是回忆          ——浪兮"),
    string("抖音@浪兮有点浪 告诉你：做开发的真实日常                            ——浪兮"),
    string("视频号@浪兮有点浪 展示的不只是成果，更是过程                        ——浪兮"),
    string("B站@浪兮有点浪 分享的技术可能不高级，但一定真诚                      ——浪兮"),
    string("抖音@浪兮有点浪 的内容源于生活，高于生活（指bug数量）              ——浪兮"),
    string("关注抖音@浪兮有点浪，看一个游戏如何从想法变成现实                    ——浪兮"),
    string("真诚推荐：关注抖音@浪兮有点浪，看幻隅背后的故事                     ——浪兮"),
    string("诚心推荐：关注抖音@浪兮有点浪，看幻隅背后的故事                     ——浪兮"),
    string("诚意推荐：关注抖音@浪兮有点浪，看幻隅背后的故事                     ——浪兮"),
    string("每个幻隅方块都有自己的脾气，比如那个总是不听使唤的墙角落            ——浪兮"),
    string("代码写得好，不如bug报得少，这是我开发幻隅的座右铭（可惜没做到）      ——浪兮"),
    string("有些游戏追求真实感，我们追求‘能跑就行’感                            ——浪兮"),
    string("听说幻隅的物理引擎会让牛顿失眠，爱因斯坦叹气，但玩家笑出声          ——浪兮"),
    string("你以为我在做游戏？不，我在写一本名为‘bug百科全书’的书              ——浪兮"),
    string("我决定给幻隅添加一个‘程序员视力’特效：只看得见代码，看不见bug      ——浪兮"),
    string("如果有人问你幻隅是什么，就说是‘随机数生成器的艺术表达’            ——浪兮"),
    string("我们承诺：幻隅的bug数量永远不会超过代码行数（但很接近了）          ——浪兮"),
    string("玩幻隅就像拆盲盒，你永远不知道下一个遇到的是bug还是特性              ——浪兮"),
    string("幻隅的开发哲学：如果它没崩溃，那就是一个可以发布的版本              ——浪兮"),
    string("我试图让幻隅变得完美，但bug们表示它们也有生存权                      ——浪兮"),
    string("在幻隅里，你最大的敌人不是地形，而是浮点运算的精度误差              ——浪兮"),
    string("有人说幻隅像人生：充满意外，偶尔崩溃，但总有一些小惊喜              ——浪兮"),
    string("我的代码风格：能用就行，美观再说，注释随缘，bug管够                ——浪兮"),
    string("幻隅教会我一个道理：有时候解决问题的最好方法是重启一切              ——浪兮"),
    string("如果幻隅是一款料理，那它的配方是：1份代码，2份bug，3份幽默          ——浪兮"),
    string("玩幻隅不需要攻略，需要的是耐心、幽默感和随时存档的好习惯            ——浪兮"),
    string("幻隅的终极大招：当一切都不管用时，试试对着电脑说‘求你了’          ——浪兮"),
    string("注释有时候比代码更令人困惑，这是一种行为艺术                          ——浪兮"),
    string("有人说幻隅像生活：计划总赶不上变化，特别是内存泄漏时                  ——浪兮"),
    string("幻隅的物理引擎：牛顿看了会沉默，伽利略看了会流泪                      ——浪兮"),
    string("我的代码有一种野性的美，主要体现在不按预期运行上                      ——浪兮"),
    string("玩幻隅不需要技术，需要的是幽默感和随时重启的勇气                      ——浪兮"),
    string("我经常在凌晨三点想到绝妙的主意，然后早上发现是馊主意                  ——浪兮"),
    string("幻隅的开发日志：今天修复了10个bug，新增了15个                        ——浪兮"),
    string("我们的代码像迷宫，但至少迷宫有地图，而代码没有                        ——浪兮"),
    string("有人说幻隅太难，我说：不，是‘太有挑战性’                            ——浪兮"),
    string("我给幻隅添加了一个隐藏功能，连我自己都忘记怎么触发了                  ——浪兮"),
    string("代码的复杂度与bug数量成正比，这是幻隅第二定律                        ——浪兮"),
    string("有人说幻隅像魔术，我说：对，经常穿帮的那种                            ——浪兮"),
    string("幻隅的画面风格：简约而不简单，主要是因为做不了复杂                    ——浪兮"),
    string("我们的音效设计：同一个音效用在不同地方叫‘资源复用’                  ——浪兮"),
    string("幻隅的UI设计：功能齐全，只要你能找到按钮在哪                          ——浪兮"),
    string("游戏优化技巧：把画质调低，然后把显示器拿远点                          ——浪兮"),
    string("幻隅的碰撞检测：大多数时候准确，除了那些时候                          ——浪兮"),
    string("幻隅的色彩搭配：程序员审美，但至少颜色很鲜艳                          ——浪兮"),
    string("游戏存档系统：多存几个档，总有一个能用的                              ——浪兮"),
    string("幻隅的镜头控制：像驯服野生动物，需要耐心和技巧                        ——浪兮"),
    string("游戏音效：有的来自素材库，有的来自我即兴发挥                          ——浪兮"),
    string("游戏操作提示：按任意键继续，除了那个没用的键                          ——浪兮"),
    string("幻隅的建造系统：随心所欲，除了有时候随不了心                          ——浪兮"),
    string("幻隅的植被系统：让世界充满绿色，哪怕只有一种绿                        ——浪兮"),
    string("游戏过场动画：加载画面就是我们的主要动画                              ——浪兮"),
    string("我们的物理破坏系统：破坏容易，修复……看情况                          ——浪兮"),
    string("幻隅的最终目标：让玩家忘记bug，记住乐趣                              ——浪兮"),
    string("玩幻隅第一课：学会在bug中寻找乐趣                                    ——浪兮"),
    string("如果你觉得游戏太难，可能不是你技术问题，是特性                        ——浪兮"),
    string("幻隅的最佳攻略：多尝试，多存档，多祈祷                                ——浪兮"),
    string("玩幻隅不需要高端设备，需要的是佛系心态                                ——浪兮"),
    string("如果你卡关了，试试换个思路，或者换个游戏                              ——浪兮"),
    string("幻隅的彩蛋：有些是我们故意放的，有些是bug伪装的                      ——浪兮"),
    string("游戏提示系统：这条提示可能就是最有用的提示                            ——浪兮"),
    string("幻隅的难度曲线：像过山车，但没有安全带                                ——浪兮"),
    string("游戏小技巧：记住，保存是你的好朋友                                    ——浪兮"),
    string("幻隅的物理谜题：解不开是正常，解开了是奇迹                            ——浪兮"),
    string("游戏操作指南：WASD移动，空格跳跃，其他键随缘                          ——浪兮"),
    string("玩幻隅不需要攻略，需要的是探索精神                                    ——浪兮"),
    string("如果你迷路了，别担心，我也经常在代码里迷路                            ——浪兮"),
    string("游戏心得：有时候放弃也是一种策略                                      ——浪兮"),
    string("玩幻隅最重要的是开心，其次是不要崩溃                                  ——浪兮"),
    string("如果你觉得画面太花，可能是我们的艺术风格太前卫了                      ——浪兮"),
    string("幻隅的渲染精度：表面简单，深层……也很简单                            ——浪兮"),
    string("游戏成就：能运行就是最大成就                                          ——浪兮"),
    string("玩幻隅建议：保持耐心，保持微笑，保持存档                              ——浪兮"),
    string("幻隅的联机功能：想象一下和朋友一起玩，只是想象                        ——浪兮"),
    string("幻隅的成功标准：有人玩，有人笑，有人没卸载/删除                       ——浪兮"),
    string("听说有人在幻隅的地下挖到了巧克力矿脉，他们是怎么做到的呢？            ——浪兮"),
    string("据说幻隅的每个世界都有一个会移动的森林，专追迷路的玩家                ——浪兮"),
    string("据说幻隅的草原上偶尔会长出会唱歌的花，但只在凌晨三点开花              ——浪兮"),
    string("听说有人在幻隅建了个会自动变换颜色的迷宫，他们是怎么做到的呢？        ——浪兮"),
    string("听说有人在幻隅找到了能改变重力方向的魔法蘑菇，他们是怎么找到的？      ——浪兮"),
    string("听说有人用幻隅的建造系统还原了亚特兰蒂斯，他们是怎么做到的呢？        ——浪兮"),
    string("听说有人用幻隅的方块复刻了万里长城，他们的显卡还好吗？                ——浪兮"),
    string("似乎幻隅的每个传说都始于一个玩家不小心按错的按键                      ——浪兮"),
    string("在幻隅里尽量少回头——别让那家伙知道你注意到祂了                      ——多茵"),
    string("幻隅中没办法行走，只是地面在后退罢了                                  ——多茵"),
    string("幻隅制作组聚餐的场景被画在了米兰圣玛利亚感恩教堂的墙上                ——多茵"),
};

//获取小提示
string gettips()
{
    int len = (sizeof(tips) / sizeof(tips[0]));
    return tips[getrandomint(0, len - 1)];
}

// 版权提示
void showcopyrightnotice()
{
    int showtime = 15;
    //输出文本，并打印空行
    cout << copyrightnotice << "\n\n\n" << endl;
    // 2. 循环仅更新倒计时（用\r覆盖上一次内容，不清屏）
    string newtip = gettips();
    for (int i = 0; i <= showtime; i++)
    {
        if (i % 3 == 0)
        {
            newtip = gettips();
        }
        cout
            << "\033[2A\r"
            << newtip
            << "     "
            << "\n\n"
            << (showtime - i)
            << "秒后自动进入..."
            << "        "
            << flush;
        // \r：光标回到当前行开头，便于后续覆盖
        // flush：强制刷新缓冲区，确保即时显示（避免cout缓冲导致延迟）
        Sleep(1000); // 等待
        if ((GetAsyncKeyState(VK_BACK) & 0x8000) != 0)
        {
            showtime = 1;//快速跳过办法
        }
    }
    system("cls");
}

//更新日志
const string updatinghistory = R"(
《幻隅：万象晶格》项目历程
【开发团队】河海大学 25级 程序设计基础课程 期末课程设计 第5组
    组长：浪兮 (hhu2524030232 张锐寒) 
    组员：TianG (hhu2524030229 顾天睿)
             多茵 (hhu2524030230 刘伊啸)
             chrysos (hhu2524030231 张杨亦航)

【版本0.1（第1次更新）】代码量：27行   版本由 开发者：浪兮 提供
2025/11/15
新增模块：
1.梯度向量算法
2.常量声明

【版本0.2（第2次更新）】代码量：60行   版本由 开发者：浪兮 提供
2025/11/15
新增模块：
1.二维梯度向量算法
2.梯度向量外输

【版本0.3（第3次更新）】代码量：63行   版本由 开发者：浪兮 提供
2025/11/16
新增模块：
1.位运算定位区块编号
2.位运算计算相对偏移

【版本0.4（第4次更新）】代码量：74行   版本由 开发者：浪兮 提供
2025/11/16
新增模块：
1.缓动函数
2.插值算法

【版本0.5（第5次更新）】代码量：92行   版本由 开发者：浪兮 提供
2025/11/16
新增模块：
1.波动量算法

【版本0.6（第6次更新）】代码量：121行   版本由 开发者：浪兮 提供
2025/11/16
新增模块：
1.波动加权
2.混合原始高度

【版本0.7（第7次更新）】代码量：107行   版本由 开发者：浪兮 提供
2025/11/16
更新内容：
1.修复了权重分配算法输入值错误的bug
2.优化了部分算法的结构，减轻运算负担

【版本0.8（第8次更新）】代码量：118行   版本由 开发者：浪兮 提供
2025/11/16
新增模块：
1.Xorshift伪随机混淆算法
核心调整：
2.修改了随机算法赋能逻辑

【版本0.9（第9次更新）】代码量：201行   版本由 开发者：浪兮 提供
2025/11/16
新增模块：
1.指令交互工具
2.新增2个调试引导

【版本0.10（第10次更新）】代码量：230行   版本由 开发者：浪兮 提供
2025/11/17
新增模块：
1.数量级判定
2.十进制取高位
功能更新：
1.限定种子修改指引的确认环节最多只需输入4位数字

【版本0.11（第11次更新）】代码量：284行   版本由 开发者：浪兮 提供
2025/11/17
新增模块：
1.数据映射算法
2.高度生成
3.取整逻辑引导

【版本0.12（第12次更新）】代码量：314行   版本由 开发者：浪兮 提供
2025/11/17
新增模块：
1.实际高度引导

【版本0.13（第13次更新）】代码量：314行   版本由 开发者：浪兮 提供
2025/11/17
更新内容：
1.调整关键算法参数，获得自然地形

【版本0.14（第14次更新）】代码量：326行   版本由 开发者：浪兮 提供
2025/11/17
新增模块：
1.梯度向量引导

【版本0.15（第15次更新）】代码量：408行   版本由 开发者：浪兮 提供
2025/11/17
新增模块：
1.新增方块类型代码宏定义
2.定义色彩类型结构体
3.声明颜色映射列表
4.构建自然方块类型推导
5.定义方块状态结构体
6.声明方块状态存储列表

【版本0.16（第16次更新）】代码量：437行   版本由 开发者：浪兮 提供
2025/11/17
新增模块：
1.哈希表存储方块数据
2.方块状态更新工具
3.方块状态查找工具

【版本0.17（第17次更新）】代码量：468行   版本由 开发者：浪兮 提供
2025/11/17
新增模块：
1.类型代码引导
2.自然地形引导

【版本0.18（第18次更新）】代码量：529行   版本由 开发者：浪兮 提供
2025/11/17
新增模块：
1.地形编辑引导
2.地形批量引导

【版本0.19（第19次更新）】代码量：539行   版本由 开发者：浪兮 提供
2025/11/17
新增模块：
1.定点探测引导
功能更新：
2.优化了一些函数逻辑

【版本0.20（第20次更新）】代码量：593行   版本由 开发者：浪兮 提供
2025/11/19
功能更新：
1.修复了地形批量工具中循环无法跳出的bug
新增模块：
1.探测批量工具
2.哈希打印工具

【版本0.21（第21次更新）】代码量：666行   版本由 开发者：浪兮 提供
2025/11/20
新增模块：
1.物理引擎数据基础
2.玩家状态数据基础

【版本0.22（第22次更新）】代码量：700行   版本由 开发者：浪兮 提供
2025/11/20
新增模块：
1.数据编解码模块
其他更新：
2.修复了出生点数据类型不对应的问题

【版本0.23（第23次更新）】代码量：772行   版本由 开发者：浪兮 提供
2025/11/20
新增模块：
1.玩家数据创建及初始化
其他更新：
2.修复了数据编解码算法转换错误的bug

【版本0.24（第24次更新）】代码量：821行   版本由 开发者：浪兮 提供
2025/11/21
新增模块：
1.运动学计算
2.弧度修正
其他更新：
1.扩充了玩家数据

【版本0.25（第25次更新）】代码量：834行   版本由 开发者：浪兮 提供
2025/11/21
新增模块：
1.动态动力
2.主动加速分配
其他更新：
3.修复了前序算法一些问题或更新参数

【版本0.26（第26次更新）】代码量：825行   版本由 开发者：浪兮 提供
2025/11/24
更新内容：
1.修复了由弧度修正返回错误所致的无法修正bug
2.修复了由玩家初始化变量作用域不适当所致的初始化失败bug
3.修复了由动态动力算法运算优先级错误所致的动力异常bug

【版本0.27（第27次更新）】代码量：839行   版本由 开发者：浪兮 提供
2025/11/24
更新内容：
1.优化了动态动力算法
2.优化了主动加速分配算法

【版本0.28（第28次更新）】代码量：851行   版本由 开发者：浪兮 提供
2025/11/27
更新内容：
1.新增了摩擦阻力和空气阻力模块

【版本0.29（第29次更新）】代码量：857行   版本由 开发者：浪兮 提供
2025/11/28
1.调整了跳跃参数
2.修正了阻力算法
3.引入了重力场模块

【版本0.30（第30次更新）】代码量：878行   版本由 开发者：浪兮 提供
2025/11/28
1.完善了指令交互功能

【版本0.31（第31次更新）】代码量：916行   版本由 开发者：浪兮 提供
2025/11/28
1.新增垂直空气阻力
2.新增相互作用模块
3.修改部分初始参数

【版本0.32（第32次更新）】代码量：919行   版本由 开发者：浪兮 提供
2025/11/28
1.修复了函数未提前声明导致的bug

【版本0.33（第33次更新）】代码量：929行   版本由 开发者：浪兮 提供
2025/11/30
更新内容：
1.调整了初始跳跃力度数据
2.修复了指引1的再确认4位码运算逻辑错误所致的种子无法重置bug
3.修复了相互作用模块部分参数错误引发的碰撞后失控bug

【版本0.34（第34次更新）】代码量：1002行   版本由 开发者：浪兮 提供
2025/12/1
更新内容：
1.新增AABB盒（Axis-Aligned Bounding Box，轴对齐包围盒)接触判定模块
2.修复了相互作用的单向动能回收方向判定错误所致的生物体异常捕获或穿模抖动bug
3.扩充了玩家的状态判断条件
4.引入可变判定区，提供运动缓冲空间

【版本0.35（第35次更新）】代码量：1097行   版本由 开发者：浪兮 提供
2025/12/1
更新内容：
1.完成了跳跃判定模块。这包含：跳跃控制，模式，起跳合法性判断，收力点检测，对墙跳跃，以及实现以上功能涉及的所有数据处理

【版本0.36（第36次更新）】代码量：1144行   版本由 开发者：浪兮 提供
2025/12/1
更新内容：
1.新增移动侦测模块
2.完成所有自动爬梯功能
3.扩充了玩家数据

【版本0.37（第37次更新）】代码量：1229行   版本由 开发者：浪兮 提供
2025/12/1
更新内容：
1.补充了传送，防卡等相关功能

【版本0.38（第38次更新）】代码量：1260行   版本由 开发者：浪兮 提供
2025/12/1
更新内容：
1.补充了传送，防卡等相关功能

【版本0.39（第39次更新）】代码量：1322行   版本由 开发者：浪兮 提供
2025/12/2
更新内容：
1.完成了物理引擎的整合

【版本0.40（第40次更新）】代码量：1887行   版本由 开发者：浪兮 提供
2025/12/2
更新内容：
1.新增了从引导修改物理数据的功能

【版本0.41（第41次更新）】代码量：2103行   版本由 开发者：浪兮 提供
2025/12/2
更新内容：
1.基本完成了物理模拟搭建

【版本0.42（第42次更新）】代码量：2134行   版本由 开发者：浪兮 提供
2025/12/2
更新内容：
1.修复众多已知问题，优化了物理模拟的部分功能并新增了部分功能，为物理引擎的调试做好准备

【版本0.43（第43次更新）】代码量：2153行   版本由 开发者：浪兮 提供
2025/12/2
更新内容：
1.新增快速输出设定功能
2.优化了部分代码架构
3.开启了物理数据的默认初始化
4.为模拟分支下的数据初始化新增风险提示

【版本0.44（第44次更新）】代码量：2171行   版本由 开发者：浪兮 提供
2025/12/6
更新内容：
1.修复了使用指引的bug
2.新增了物理模拟使用指引

【版本0.45（第45次更新）】代码量：2238行   版本由 开发者：浪兮 提供
2025/12/7
更新内容：
1.视觉控制模块
2.动态视野模块

【版本0.46（第46次更新）】代码量：2251行   版本由 开发者：浪兮 提供
2025/12/7
更新内容：
1.修复了开发者模式致命bug
2.更新了一个程序约定

【版本0.47（第47次更新）】代码量：2317行   版本由 开发者：多茵 提供
2025/12/8
更新内容：
1.修复了AABB碰撞检测算法

【版本0.48（第48次更新）】代码量：2335行   版本由 开发者：浪兮 提供
2025/12/8
更新内容：
1.新增方块可见性检测

【版本0.49（第49次更新）】代码量：2362行   版本由 开发者：浪兮 提供
2025/12/8
更新内容：
1.启动可见性引导

【版本0.50（第50次更新）】代码量：2434行   版本由 开发者：浪兮 提供
2025/12/8
更新内容：
1.启用可见批量引导
2.定义了方块数据类型

【版本0.51（第51次更新）】代码量：2466行   版本由 开发者：浪兮 提供
2025/12/9
更新内容：
1.引入存储数据的动态数组
2.完成了方块数据装载模块

【版本0.52（第52次更新）】代码量：2500行   版本由 开发者：浪兮 提供
2025/12/9
更新内容：
1.完善了原有的数据处理模块
2.新增了一些渲染的辅助模块

【版本0.53（第53次更新）】代码量：2532行   版本由 开发者：浪兮 提供
2025/12/9
更新内容：
1.对现有模块进行了一些优化

【版本0.54（第54次更新）】代码量：2657行   版本由 开发者：浪兮 提供
2025/12/9
更新内容：
1.基本完成了面前体数据的全流程处理模块

【版本0.55（第55次更新）】代码量：2753行   版本由 开发者：多茵 提供
2025/12/9
更新内容：
1.修复了位置偏移问题
2.修复了爬梯误判

【版本0.56（第56次更新）】代码量：2820行   版本由 开发者：浪兮 提供
2025/12/10
更新内容：
1.优化了数据处理模块
2.新增错误代码指南

【版本0.57（第57次更新）】代码量：2878行   版本由 开发者：浪兮 提供
2025/12/10
更新内容：
1.完成亮度计算
2.整合好面前体全流程数据处理

【版本0.58（第58次更新）】代码量：2881行   版本由 开发者：浪兮 提供
2025/12/11
更新内容：
1.修复了一些已知问题

【版本0.59（第59次更新）】代码量：2947行   版本由 开发者：浪兮 提供
2025/12/11
更新内容：
1.完成了投影转换功能

【版本0.60（第60次更新）】代码量：2990行   版本由 开发者：浪兮 提供
2025/12/11
更新内容：
1.搭建完成面数据处理基本框架
2.完成数据处理第2步衔接

【版本0.61（第61次更新）】代码量：3021行   版本由 开发者：浪兮 提供
2025/12/11
更新内容：
1.完成面数据处理模块

【版本0.62（第62次更新）】代码量：3028行   版本由 开发者：浪兮 提供
2025/12/11
更新内容：
1.修复了一些已知问题
2.提供了一些功能升级

【版本0.63（第63次更新）】代码量：3035行   版本由 开发者：浪兮 提供
2025/12/11
更新内容：
1.提供了一些内容更新

【版本0.64（第64次更新）】代码量：3082行   版本由 开发者：chrysos 提供
2025/12/11
更新内容：
1.引入线宽算法
2.完成单位绘制模块

【版本0.65（第65次更新）】代码量：3132行   版本由 开发者：TitanG 提供
2025/12/11
更新内容：
1.新增对数据做快速排序的功能
2.完成统筹渲染

【版本0.66（第66次更新）】代码量：3147行   版本由 开发者：浪兮 提供
2025/12/12
更新内容：
1.完成数据处理的流程控制模块

【版本0.67（第67次更新）】代码量：3241行   版本由 开发者：浪兮 提供
2025/12/12
更新内容：
1.对可见性检测模块做了调试准备

【版本0.68（第68次更新）】代码量：3614行   版本由 开发者：多茵 提供
2025/12/13
更新内容：
1.装载了视觉控制模块

【版本0.69（第69次更新）】代码量：3690行   版本由 开发者：TitanG 提供
2025/12/13
更新内容：
1.提供了可视化方案

【版本0.70（第70次更新）】代码量：3704行   版本由 开发者：浪兮 提供
2025/12/13
更新内容：
1.优化了可视化方案
2.对可视化方案做了适配性整合

【版本0.71（第71次更新）】代码量：3723行   版本由 开发者：浪兮 提供
2025/12/13
更新内容：
1.完善了开发者模式的一级菜单功能
2.搭建了控制台主页框架

【版本0.72（第72次更新）】代码量：3723行   版本由 开发者：浪兮 提供
2025/12/13
更新内容：
1.完善了菜单部分

【版本0.73（第73次更新）】代码量：4081行   版本由 开发者：浪兮 提供
2025/12/13
更新内容：
1.赋予软件法律规范性
2.添加结构内容

【版本0.74（第74次更新）】代码量：4159行   版本由 开发者：多茵 提供
2025/12/13
更新内容：
1.系统性升级了视觉控制模块

【版本0.75（第75次更新）】代码量：4215行   版本由 开发者：浪兮 提供
2025/12/13
更新内容：
1.对视觉控制模块做了最小幅度的兼容性优化和整理

【版本0.76（第76次更新）】代码量：4221行   版本由 开发者：多茵 提供
2025/12/14
更新内容：
1.初步完成了视觉控制模块的整合

【版本0.77（第77次更新）】代码量：4259行   版本由 开发者：多茵&浪兮 提供
2025/12/14
更新内容：
1.局部架构说明

【版本0.78（第78次更新）】代码量：4494行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.新增第三方依赖声明

【版本0.79（第79次更新）】代码量：4532行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.新增多个接口和对接模块
2.阻止原插件内部修改数据

【版本0.80（第80次更新）】代码量：4563行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.测试封装
2.测试优化

【版本0.81（第81次更新）】代码量：4583行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.完成视角控制模块
2.将视角控制并入渲染

【版本0.82（第82次更新）】代码量：4632行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.新增地形模式
2.提供地形模式引导

【版本0.83（第83次更新）】代码量：4648行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.新增视角灵敏度调节
2.提供视角灵敏引导

【版本0.84（第84次更新）】代码量：4682行   版本由 开发者 TianG 提供
2025/12/14
修改部分:
1.face3Dto2D函数  
2.SyncPlayerToCamera函数  修复了坐标系不统一的问题  
3.修复了sightsynchronize函数中把角度弧度搞混的问题

【版本1.00（第85次更新）】代码量：4686行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.修复了虚空仍有方块bug
2.提供了开始游戏的接口

【版本1.01（第86次更新）】代码量：4691行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.修复了空白画布模式下，画布厚度异常的bug
2.设定了背景天空颜色

【版本1.02（第87次更新）】代码量：4691行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.修复了视角转动反向的bug

【版本1.03（第88次更新）】代码量：4832行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.提供了视控参数引导

【版本1.04（第89次更新）】代码量：5015行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.提供了存档功能
2.完成了文本文档写入功能

【版本1.05（第90次更新）】代码量：5058行   版本由 开发者：浪兮 提供
2025/12/14
更新内容：
1.完成了二进制文件写入功能
2.优化了文本文档存档模块

【版本1.06（第91次更新）】代码量：5146行   版本由 开发者：浪兮 提供
2025/12/15
更新内容：
1.提供了读档支持程序

【版本1.07（第92次更新）】代码量：5221行   版本由 开发者：浪兮 提供
2025/12/15
更新内容：
1.完善了读档程序和相关支持模块

【版本1.08（第93次更新）】代码量：5285行   版本由 开发者：浪兮 提供
2025/12/15
更新内容：
1.新增自由飞行模式
2.支持2种运动模式间的自由切换

【版本1.09（第94次更新）】代码量：5419行   版本由 开发者：浪兮 提供
2025/12/15
更新内容：
1.提供了iln文件的读取功能
2.完善了读档的支持功能

【版本1.10（第95次更新）】代码量：5507行   版本由 开发者：浪兮 提供
2025/12/15
更新内容：
1.完成了全部读档功能

【版本1.11（第96次更新）】代码量：5511行   版本由 开发者：浪兮 提供
2025/12/15
更新内容：
1.修复了ilnd文件读档后文件名显示包含上级路径的bug

【版本1.12（第97次更新）】代码量：5542行   版本由 开发者：多茵 提供
2025/12/15
更新内容：
1.新增防抖机制

【版本1.13（第98次更新）】代码量：5836行   版本由 开发者：浪兮 提供
2025/12/15
更新内容：
1.完成存档查询子模块

【版本1.14（第99次更新）】代码量：5840行   版本由 开发者：浪兮  提供
2025/12/16
更新内容：
1.优化了存档系统

【版本1.15（第100次更新）】代码量：5860行   版本由 开发者：浪兮 提供
2025/12/16
更新内容：
1.新增了音频播放模块
 
【版本1.16（第101次更新）】代码量：5944行   版本由 开发者：浪兮 提供
2025/12/16 
更新内容：
1.停用了按键触发音
2.提供了开屏界面框架

【版本1.17（第102次更新）】代码量：5944行   版本由 开发者：TianG 提供
2025/12/16 
更新内容：
1.对地面渲染特殊处理
调整模块：
1.可见性检测isblockvisible 
2.视野计算getfield 
3.方块数据装载blockdataload  
4.深度排序改进quicksort

【版本2.00（第103次更新）】代码量：6097行   版本由 开发者：浪兮 提供
2025/12/16 
更新内容：
1.提供了开屏动画
2.完善了音乐和图片的加载
3.新增一些信息

【版本2.01（第104次更新）】代码量：6203行   版本由 开发者：TianG 提供
2025/12/16 
更新内容：
1.解决了面渲染频现异常的bug

【版本2.02（第105次更新）】代码量：6841行   版本由 开发者：浪兮 提供
2025/12/16 
更新内容：
1.修复了视角转动方向错误的bug
2.提供了查阅更新日志的功能

【版本2.03（第106次更新）】代码量：6916行   版本由 开发者：浪兮 提供
2025/12/16 
更新内容：
1.新增加载时显示的小提示

【版本2.04（第107次更新）】代码量：7161行   版本由 开发者：浪兮 提供
2025/12/16 
更新内容：
1.提供了更多小提示

【版本2.05（第108次更新）】代码量：7178行   版本由 开发者：浪兮 提供
2025/12/16 
更新内容：
1.将小提示接入存档系统

【版本2.06（第109次更新）】代码量：7255行   版本由 开发者：多茵 提供
2025/12/16 
新增：
1.精准地面检测(经历了14次更迭，最终解决了卡进地底的bug!!)

【版本2.07（第110次更新）】代码量：7403行   版本由 开发者：浪兮 提供
2025/12/17 
更新内容：
1.提供了更多小提示

【版本2.08（第111次更新）】代码量：7511行   版本由 开发者：浪兮 提供
2025/12/17 
更新内容：
1.优化了小提示的权重结构

【版本2.09（第112次更新）】代码量：7627行   版本由 开发者：浪兮 提供
2025/12/17 
更新内容：
1.扩展了小提示的风格

【版本2.10（第113次更新）】代码量：7773行   版本由 开发者：浪兮 提供
2025/12/17 
更新内容：
1.完成了游戏进入菜单
2.完成了地图选择UI界面初始版本的搭建

【版本2.11（第114次更新）】代码量：7811行   版本由 开发者：浪兮 提供
2025/12/17 
更新内容：
1.完善了游戏的UI界面
2.完成了游戏进入的全流程

【版本2.12（第115次更新）】代码量：7819行   版本由 开发者：浪兮 提供
2025/12/17 
更新内容：
1.修复了重复进入地图选择后黑屏的bug

【版本2.13（第116次更新）】代码量：7824行   版本由 开发者：浪兮 提供
2025/12/17 
更新内容：
1.优化了一些菜单结构

【版本2.14（第117次更新）】代码量：7922行   版本由 开发者：浪兮 提供
2025/12/18
更新内容：
1.对柏林噪声算法进行系统性优化，彻底解决了地形不随机，或表现不好的问题

【版本2.15（第118次更新）】代码量：7929行   版本由 开发者：浪兮 提供
2025/12/18
更新内容：
1.修复了创建新地图后状态残留的bug

【版本2.16（第119次更新）】代码量：7929行   版本由 开发者：多茵 提供
2025/12/18
更新内容：
1.修改了最大奔跑力度
2.增加了临时跳跃作弊代码

)";

const string updatinghistory2 = R"(
【版本2.17（第120次更新）】代码量：7954行   版本由 开发者：浪兮 提供
2025/12/18
更新内容：
1.调整了一些物理参数

【版本2.18（第121次更新）】代码量：8031行   版本由 开发者：浪兮 提供
2025/12/18
更新内容：
1.搭建了数据显示界面
2.初步准备了小地图
3.提供了准星显示

【版本2.19（第122次更新）】代码量：8038行   版本由 开发者：浪兮 提供
2025/12/18
更新内容：
1.修复了视角显示中的致命bug

【版本2.20（第123次更新）】代码量：8042行   版本由 开发者：TianG 提供
2025/12/18
更新内容：
1.修复了可见性检测模块的致命bug

【版本2.21（第124次更新）】代码量：8021行   版本由 开发者：多茵 提供
2025/12/18
更新内容：
1.构建了新版跳跃引擎

【版本2.22（第125次更新）】代码量：8173行   版本由 开发者：浪兮 提供
2025/12/18
更新内容：
1.构建了全新的能量跳跃体系
2.完成了能量条的直观呈现

【版本3.00（第126次更新）】代码量：8000+行  版本由 开发者：浪兮 提供
2025/12/18
更新内容：
1.新增fps计算
2.新增fps显示

【版本3.01（第127次更新）】代码量：8000+行  版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.可以选择隐藏/显示物理参数
2.新增技能机制
3.完成技能模块可视化

【版本3.02（第128次更新）】代码量：8438行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.调整技能CD为秒级显示
2.载入技能图标
3.对游戏窗口禁用输入法
4.优化了技能显示

【版本3.03（第129次更新）】代码量：8498行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.实现了“充能”技能的技能效果

【版本3.04（第130次更新）】代码量：8529行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.新增技能"跃升"
2.调整了所有技能的表现效果
3.优化了技能的协同表现效果

【版本3.05（第131次更新）】代码量：8551行   版本由 开发者：多茵 提供
2025/12/19
更新内容：
1.解决了玩家往特定方向移动时出现的加速度反了的bug

【版本3.06（第132次更新）】代码量：8556行   版本由 开发者：多茵 提供
2025/12/19
更新内容：
1.使玩家的移动方向控制能够跟随视角，代价是玩家的视角不能在渲染时显示呈现

【版本3.07（第133次更新）】代码量：8620行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.修复了玩家的视方位角不能在渲染时显示呈现的bug，但未解决俯仰角的相同问题

【版本3.08（第134次更新）】代码量：8649行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.修复了玩家的视俯仰角不能在渲染时显示呈现的bug

【版本3.09（第135次更新）】代码量：8666行   版本由 开发者：多茵 提供
2025/12/19
更新内容：
1.新增方向键控制

【版本3.10（第136次更新）】代码量：8676行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.调整了鼠标的位置(反复尝试隐藏但失败，遂出此下策)

【版本3.11（第137次更新）】代码量：8686行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.增加防卡传送技能(CD：10s)
2.优化了防卡传送机制

【版本3.12（第138次更新）】代码量：8693行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.优化了小地图上玩家的位置显示

【版本3.13（第139次更新）】代码量：8720行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.新增UI平移功能（为解决部分显示器上的界面不适配问题）

【版本3.14（第140次更新）】代码量：8725行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.优化了UI平移功能

【版本3.15（第141次更新）】代码量：8729行   版本由 开发者：浪兮 提供
2025/12/19
更新内容：
1.修复了小地图的1个显示bug

【版本3.16（第142次更新）】代码量：8737行   版本由 开发者：浪兮 提供
2025/12/20
更新内容：
1.提供了视俯仰角的渲染前初始化

【版本3.17（第143次更新）】代码量：8762行   版本由 开发者：浪兮 提供
2025/12/20
更新内容：
1.新增地形生成版本回退功能
2.提供了地形版本引导

【版本3.18（第144次更新）】代码量：8811行   版本由 开发者：浪兮 提供
2025/12/20
更新内容：
1.恢复了视方位角的正常显示
2.提供了竖直方向的UI偏移功能
3.完成了基础的工具栏绘制

【版本3.19（第145次更新）】代码量：8955行   版本由 开发者：浪兮 提供
2025/12/20
更新内容：
1.优化了UI位移功能

【版本3.20（第146次更新）】代码量：9242行   版本由 开发者：多茵 提供
2025/12/20
更新内容：
1.预测性碰撞检测函数
2.智能爬升函数
3.新增：温和的碰撞修正（避免剧烈抖动）
4.物理引擎升级

【版本3.21（第147次更新）】代码量：9252行   版本由 开发者：浪兮 提供
2025/12/20
更新内容：
1.升级了地形生成系统，更好地适配了现版本玩家的运动能力
2.对重力相关数值做了调整

【版本3.22（第148次更新）】代码量：9332行   版本由 开发者：浪兮 提供
2025/12/20
更新内容：
1.完成工具栏制作

【版本3.23（第149次更新）】代码量：9361行   版本由 开发者：浪兮 提供
2025/12/20
更新内容：
1.对非自然地形地图禁用了局部平滑功能，以提高fps
2.提供了工具栏的部分隐藏功能，以提高fps

【版本3.24（第150次更新）】代码量：9365行   版本由 开发者：浪兮 提供
2025/12/20
更新内容：
1.修复了提示语显示位置错误的bug

【版本3.25（第151次更新）】代码量：9533行   版本由 开发者：多茵 提供
2025/12/21
更新内容：
1.以有效的方式重构了碰撞检测函数
2.修复了穿墙bug

【版本3.26（第152次更新）】代码量：9541行   版本由 开发者：浪兮 提供
2025/12/21
玩家跳跃属性调整：
1.速度能量比：（有动力状态）0.00012→0.000095、（无动力状态）保持0.000072不变
2.能量衰减速度：（有动力状态）衰减比例 15%→28%，消耗量30→45；（无动力状态）保持 衰减比例45%，消耗量75 不变
3.充能增强属性（关联一技能）初始能量值20000→13500
4.一技能属性：冷却时间（技能CD） 25s→28s，持续时长4.5s→4.15s

【版本3.27（第153次更新）】代码量：10253行   版本由 开发者：浪兮 提供
2025/12/21
更新内容：
1.代码可读性优化

【版本3.28（第154次更新）】代码量：10264行   版本由 开发者：浪兮 提供
2025/12/22
更新内容：
1.全局初始化

【版本3.29（第155次更新）】代码量：10423行   版本由 开发者：浪兮 提供
2025/12/22
更新内容：
1.搭建了备用的音频模块(mci)解决播放bug

【版本3.30（第156次更新）】代码量：10428行   版本由 开发者：浪兮 提供
2025/12/22
更新内容：
1.禁用了爬升音效

【版本3.31（第157次更新）】代码量：10716行   版本由 开发者：多茵 提供
2025/12/22
更新内容：
1.增强了准星检测
2.新增方块交互函数

【版本3.32（第158次更新）】代码量：10725行   版本由 开发者：浪兮 提供
2025/12/23
更新内容：
1.新增技能：净化

【版本3.33（第159次更新）】代码量：11350行   版本由 开发者：多茵 提供
2025/12/24
更新内容：
1.部分修复了准星检测
2.加入了建造模式和跑酷模式的切换（g键）

【版本3.34（第160次更新）】代码量：11409行   版本由 开发者：浪兮 提供
2025/12/23
更新内容：
1.使自动工具类型识别功能可以被使用。

【版本3.35（第161次更新）】代码量：11414行   版本由 开发者：浪兮 提供
2025/12/23
更新内容：
1.新增存档选项提示

【版本3.36（第162次更新）】代码量：11527行   版本由 开发者：TianG 提供
2025/12/24
更新内容：
1.修复了存档系统的bug

【版本3.37（第162次更新）】代码量：11552行   版本由 开发者：浪兮 提供
2025/12/24
更新内容：
1.优化了一些模块

【版本4.00（第163次更新）】代码量：11657行   版本由 开发者：多茵 提供
2025/12/26
更新内容：
1.修复了一些已知问题

【版本4.01（第164次更新）】代码量：11000+行  版本由 开发者：多茵 提供
2025/12/27
更新内容：
1.修复了一些已知问题

【版本4.02（第165次更新）】代码量：11000+行  版本由 开发者：TianG 提供
2025/12/28
更新内容：
1.修复了一些已知问题

【版本4.03（第166次更新）】代码量：11887行   版本由 开发者：浪兮 提供
2025/12/28
更新内容：
（净化界面）
1.使辅助工具仅在建造模式显示
2.修改了游戏模式UI位置，并调整为仅在变化时短暂提示

【版本4.04（第167次更新）】代码量：11962行   版本由 开发者：浪兮 提供
2025/12/28
更新内容：
1.新增提示语
2.新增游戏计时器（序列需要）

【版本4.05（第168次更新）】代码量：11974行   版本由 开发者：浪兮 提供
2025/12/28
更新内容：
1.将默认游戏模式调整为跑酷模式
2.添加飞行模式变速提示
3.新增提示时隐藏准星功能

【版本4.06（第169次更新）】代码量：12008行   版本由 开发者：浪兮 提供
2025/12/28
更新内容：
1.调整自由飞行模式变速控制按键为T和Y（原按键功能冲突）
2.新增运动模式切换提示
3.使建造模式自动关联自由飞行模式，跑酷模式自动关联自然物理模式
4.在跑酷模式下禁用自由飞行模式

【版本4.07（第170次更新）】代码量：12040行   版本由 开发者：浪兮 提供
2025/12/28
更新内容：
1.新增物理参数显示/隐藏提示
2.切换游戏模式时，自动显示/隐藏

【版本4.08（第171次更新）】代码量：12050行   版本由 开发者：浪兮 提供
2025/12/28
更新内容：
1.将游戏模式切换检测从物理监听模块迁移至渲染补充模块，解决了因自由飞行模式下物理监听不生效导致的模式无法切换bug
2.修复了extern缺失问题

【版本4.09（第172次更新）】代码量：12074行   版本由 开发者：浪兮 提供
2025/12/28
更新内容：
1.修复了地图初始化后不能自动更新物理参数显示状态的bug
2.修复了跑酷模式下准星显示仍然识别方块的bug

【版本4.10（第173次更新）】代码量：12107行   版本由 开发者：浪兮 提供
2025/12/28
更新内容：
1.将工具栏折叠按键调整为L
2.将快捷地形编辑按键统一为B

【版本5.00（第174次更新）】代码量：12120行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.优化了开屏界面字幕效果

【版本5.01（第175次更新）】代码量：12249行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.完成了BC模块

【版本5.02（第176次更新）】代码量：12373行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.完成了buff模块

【版本5.03（第177次更新）】代码量：12459行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.完成了bUI模块

【版本5.04（第178次更新）】代码量：12486行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.模块进入循环
2.修复DWORD类型减法溢出问题
3.优化检测重置代码
4.修复UI移位bug
5.修复剩余时间数位异常bug

【版本5.05（第179次更新）】代码量：12495行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.优化buffUI位置
2.仅在跑酷模式显示buffUI

【版本5.06（第180次更新）】代码量：12541行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.切换游戏模式时自动拿起/放下工具
2.UI界面再优化
3.判定逻辑加强（双判定）

【版本5.07（第181次更新）】代码量：12707行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.完成BE模块

【版本5.08（第182次更新）】代码量：12756行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.完成ht模块

【版本5.09（第183次更新）】代码量：12827行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.调整了游戏模式提示的位置
2.完成了htUI模块

【版本6.00（第184次更新）】代码量：12849行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.修复了black不可被破坏的bug
2.修复了中毒、致幻和混乱效果异常传送的bug
3.调整了“混乱”状态对运动的干扰程度
4.调整了“中毒”状态下运动能力的下降程度
5.优化了“失明”状态下的消息提示
6.增加自愈效果

【版本6.01（第185次更新）】代码量：12854行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.调整了渲染顺序，使失明时可以看到buff的剩余时间

【版本6.02（第186次更新）】代码量：12872行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.对生命值，能量值进行了限制

【版本6.03（第187次更新）】代码量：12878行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.修复了阵亡界面返回主页无法正常重生的bug

【版本6.04（第188次更新）】代码量：12872行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.新增技能"免疫"

【版本6.05（第189次更新）】代码量：13064行   版本由 开发者：浪兮 提供
2025/12/29
更新内容：
1.最终优化
)";

//二期工程更新日志预存
extern string updatinghistory2phase;
extern string updatinghistory2phase2;
extern string updatinghistory2phase3;

//更新日志查看
void showupdatinghistory()
{
    cout << updatinghistory << updatinghistory2 << endl;
}

//主页菜单
void menu()
{
    cout << "菜单：" << endl;
    cout << "1.版本号查询" << endl;
    cout << "2.开发者名录" << endl;
    cout << "3.版权与许可" << endl;
    cout << "4.全更新日志" << endl;
    cout << "输入: ";
    int a;
    cin >> a;
    if (a == 1)
    {
        cout << "读取到版本号：v" << version1 << "." << version2 << endl;
    }
    else if (a == 2)
    {
        cout << developerroster << endl;
    }
    else if (a == 3)
    {
        copyrightandlicenseinfo();
    }
    else if (a == 4)
    {
        showupdatinghistory();
    }
    else
    {
        cout << "Err:菜单不存在" << endl;
        menu();
    }
}

//控制台主页（预声明）
void homepage();

//渲染模式（预声明）
void rendermode();

//【视觉控制模块接口】(原模块不兼容)

extern std::vector<Block> g_sceneBlocks; //访问插件全局方块容器

//方块数据接口（包含坐标系兼容性转换）
void AddBlockToScene(double x, double y, double h, int type)
{
    g_sceneBlocks.emplace_back(Vector3(x, h, y), 1.0, type);
}

//坐标接口（包含坐标系兼容性转换）
void SyncPlayerToCamera(double x, double y, double h)
{
    ViewSetup::Current()->Pos = Vector3(x, h, y);
}

//统一数据输入
void vcin()
{
    SyncPlayerToCamera(playerdata.state.x, playerdata.state.y, playerdata.state.h + playerdata.structural.h + 5.0);
    for (int i = 0; i < blockcount; i++)
    {
        AddBlockToScene(blockvisibledata[i].x, blockvisibledata[i].y, blockvisibledata[i].h, blockvisibledata[i].typecode == 0 ? 0 : 1);
    }
}

//视角外传
void GetViewRotation(double& yaw, double& pitch)
{
    ViewControl* view = ViewSetup::Current();
    yaw = view->Yaw;
    pitch = view->Pitch;
}

//视角同步
void sightsynchronize()
{
    POINT mousePos;
    if (GetCursorPos(&mousePos)) // 获取系统鼠标位置
    {
        UpdateViewByMouse(static_cast<double>(mousePos.x), static_cast<double>(mousePos.y));
    }
    ViewControl* ctrl = ViewSetup::Current();
    ctrl->Sensitivity = viewsetup.sensibility;
    // 获取角度值
    double yaw_deg, pitch_deg;
    GetViewRotation(yaw_deg, pitch_deg);

    // 角度转弧度
    //viewsetup.sighta = (ctrl->Yaw * pi / 180.0);  // 水平角
    //viewsetup.sightb = 2 * pi - (ctrl->Pitch * pi / 180.0); // 垂直角
    //(旧版逻辑)
}

//视控参数引导（原视角灵敏引导，经过扩充，沿用有一些原始名称）
void helperofsensibility()
{
    cout << "[来自：视控参数引导]视控参数：0.返回 1.rfield(水平视野跨距) 2.hfield(竖直视野跨距) 3.sighta(视方位角) 4.sightb(视俯仰角) 5.ambient(环境光强度) 6.diffuse(漫反射强度) 7.kbd(亮度距离衰减底色范围比例) 8.fov(视场角) 9.nearclip(近裁切面) 10.kborderbrightness(边框亮度系数) 11.kborderthickness(边框宽度系数) 12.sensibility(视角灵敏度)" << endl;
    float b;
    cin >> b;

    // 1.rfield(水平视野跨距) - int，最小值1
    if (b == 1)
    {
        cout << "[来自：视控参数引导]1.rfield(水平视野跨距) 类型：int 参数范围：≥1 原值：" << viewsetup.rfield << endl;
        int a;
        cin >> a;
        a = (a < 1) ? 1 : a;
        viewsetup.rfield = a;
        cout << "[来自：视控参数引导]已经将水平视野跨距调节为：" << viewsetup.rfield << " 引导退出" << endl;
    }

    // 2.hfield(竖直视野跨距) - int，最小值1
    else if (b == 2)
    {
        cout << "[来自：视控参数引导]2.hfield(竖直视野跨距) 类型：int 参数范围：≥1 原值：" << viewsetup.hfield << endl;
        int a;
        cin >> a;
        a = (a < 1) ? 1 : a;
        viewsetup.hfield = a;
        cout << "[来自：视控参数引导]已经将竖直视野跨距调节为：" << viewsetup.hfield << " 引导退出" << endl;
    }

    // 3.sighta(视方位角) - double，0-360°
    else if (b == 3)
    {
        cout << "[来自：视控参数引导]3.sighta(视方位角) 类型：double 参数范围：0-360° 原值：" << viewsetup.sighta << endl;
        double a;
        cin >> a;
        a = (a < 0.0) ? 0.0 : ((a > 360.0) ? 360.0 : a);
        viewsetup.sighta = a;
        cout << "[来自：视控参数引导]已经将视方位角调节为：" << viewsetup.sighta << "° 引导退出" << endl;
    }

    // 4.sightb(视俯仰角) - double，0-360°
    else if (b == 4)
    {
        cout << "[来自：视控参数引导]4.sightb(视俯仰角) 类型：double 参数范围：0-360° 原值：" << viewsetup.sightb << endl;
        double a;
        cin >> a;
        a = (a < 0.0) ? 0.0 : ((a > 360.0) ? 360.0 : a);
        viewsetup.sightb = a;
        cout << "[来自：视控参数引导]已经将视俯仰角调节为：" << viewsetup.sightb << "° 引导退出" << endl;
    }

    // 5.ambient(环境光强度) - float，0-1
    else if (b == 5)
    {
        cout << "[来自：视控参数引导]5.ambient(环境光强度) 类型：float 参数范围：0-1 原值：" << viewsetup.ambient << endl;
        float a;
        cin >> a;
        a = (a < 0.0f) ? 0.0f : ((a > 1.0f) ? 1.0f : a);
        viewsetup.ambient = a;
        cout << "[来自：视控参数引导]已经将环境光强度调节为：" << viewsetup.ambient << " 引导退出" << endl;
    }

    // 6.diffuse(漫反射强度) - float，0-1
    else if (b == 6)
    {
        cout << "[来自：视控参数引导]6.diffuse(漫反射强度) 类型：float 参数范围：0-1 原值：" << viewsetup.diffuse << endl;
        float a;
        cin >> a;
        a = (a < 0.0f) ? 0.0f : ((a > 1.0f) ? 1.0f : a);
        viewsetup.diffuse = a;
        cout << "[来自：视控参数引导]已经将漫反射强度调节为：" << viewsetup.diffuse << " 引导退出" << endl;
    }

    // 7.kbd(亮度距离衰减底色范围比例) - float，0-1
    else if (b == 7)
    {
        cout << "[来自：视控参数引导]7.kbd(亮度距离衰减底色范围比例) 类型：float 参数范围：0-1 原值：" << viewsetup.kbd << endl;
        float a;
        cin >> a;
        a = (a < 0.0f) ? 0.0f : ((a > 1.0f) ? 1.0f : a);
        viewsetup.kbd = a;
        cout << "[来自：视控参数引导]已经将亮度距离衰减底色范围比例调节为：" << viewsetup.kbd << " 引导退出" << endl;
    }

    // 8.fov(视场角) - float，0-180°
    else if (b == 8)
    {
        cout << "[来自：视控参数引导]8.fov(视场角) 类型：float 参数范围：0-180° 原值：" << viewsetup.fov << endl;
        float a;
        cin >> a;
        a = (a < 0.0f) ? 0.0f : ((a > 180.0f) ? 180.0f : a);
        viewsetup.fov = a;
        cout << "[来自：视控参数引导]已经将视场角调节为：" << viewsetup.fov << "° 引导退出" << endl;
    }

    // 9.nearclip(近裁切面) - float，最小值0
    else if (b == 9)
    {
        cout << "[来自：视控参数引导]9.nearclip(近裁切面) 类型：float 参数范围：≥0 原值：" << viewsetup.nearclip << endl;
        float a;
        cin >> a;
        a = (a < 0.0f) ? 0.0f : a;
        viewsetup.nearclip = a;
        cout << "[来自：视控参数引导]已经将近裁切面调节为：" << viewsetup.nearclip << " 引导退出" << endl;
    }

    // 10.kborderbrightness(边框亮度系数) - float，0-1
    else if (b == 10)
    {
        cout << "[来自：视控参数引导]10.kborderbrightness(边框亮度系数) 类型：float 参数范围：0-1 原值：" << viewsetup.kborderbrightness << endl;
        float a;
        cin >> a;
        a = (a < 0.0f) ? 0.0f : ((a > 1.0f) ? 1.0f : a);
        viewsetup.kborderbrightness = a;
        cout << "[来自：视控参数引导]已经将边框亮度系数调节为：" << viewsetup.kborderbrightness << " 引导退出" << endl;
    }

    // 11.kborderthickness(边框宽度系数) - float，0-1
    else if (b == 11)
    {
        cout << "[来自：视控参数引导]11.kborderthickness(边框宽度系数) 类型：float 参数范围：0-1 原值：" << viewsetup.kborderthickness << endl;
        float a;
        cin >> a;
        a = (a < 0.0f) ? 0.0f : ((a > 1.0f) ? 1.0f : a);
        viewsetup.kborderthickness = a;
        cout << "[来自：视控参数引导]已经将边框宽度系数调节为：" << viewsetup.kborderthickness << " 引导退出" << endl;
    }

    // 12.sensibility(视角灵敏度) - float，0.01-1.0
    else if (b == 12)
    {
        cout << "[来自：视控参数引导]12.sensibility(视角灵敏度) 类型：float 参数范围：0.01-1.0 原值：" << viewsetup.sensibility << endl;
        float a;
        cin >> a;
        a = (a < 0.01) ? 0.01 : ((a > 1.0) ? 1.0 : a);
        viewsetup.sensibility = a;
        cout << "[来自：视控参数引导]已经将视角灵敏度调节为：" << viewsetup.sensibility << " 引导退出" << endl;
    }

    // 0.返回
    else if (b == 0)
    {
        helper();
    }

    // 无效索引
    else
    {
        cout << "[来自：视控参数引导]Err：无效索引" << endl;
        helperofsensibility();
    }
}

//地形生成版本回退
void usingpreviousversion()
{
    usingearlyversion = !usingearlyversion;
    if (usingearlyversion)
    {
        cout << "[地形版本引导]已经将地形生成引擎回退为旧版。" << endl;
    }
    else
    {
        cout << "[地形版本引导]已经将地形生成引擎升级为新版。" << endl;
    }
    cout << "[地形版本引导]您可以再次进入引导以撤销操作。 引导退出" << endl;
}

//指令交互工具
void helper()
{
    cout << "[来自：指令交互工具]指令重置。1=请求引导 2=退出工具 3=退出程序 4=使用说明" << endl;
    cin >> modecode;
    if (modecode == 1)
    {
        cout << "[来自：指令交互工具]目前支持的引导：0.返回主页 1.种子修改 2.OH批量 3.grad获取 4.取整逻辑调整 5.RH批量 6.类型代码 7.自然地形检索 8.地形编辑 9.地形批量 10.定点探测 11.探测批量 12.哈希打印 13.2in1编码 14.2in1解码 15.手动传送 16.防卡传送 17.物理数据 18.物理模拟 19.可见性检测 20.可见性批量 21.渲染模式 22.视觉控制旧版测试 23.地形模式 24.视控参数 25.地形生成版本" << endl;
        cin >> modecode;
        if (modecode == 1)
        {
            helperofseedchange();
        }
        else if (modecode == 2)
        {
            helperoftestoriginalheight();
        }
        else if (modecode == 3)
        {
            helperoftestgradient();
        }
        else if (modecode == 4)
        {
            helperofroundlogic();
        }
        else if (modecode == 5)
        {
            helperoftestheight();
        }
        else if (modecode == 6)
        {
            helperofblockcode();
        }
        else if (modecode == 7)
        {
            helperofnatureblocktype();
        }
        else if (modecode == 8)
        {
            helperofupdateblock();
        }
        else if (modecode == 9)
        {
            helperofmultipleupdateblock();
        }
        else if (modecode == 10)
        {
            helperoffindtype();
        }
        else if (modecode == 11)
        {
            helperofmultiplefindtype();
        }
        else if (modecode == 12)
        {
            blockmapprinter();
        }
        else if (modecode == 13)
        {
            helperofnum2in1();
        }
        else if (modecode == 14)
        {
            helperofget2in1num();
        }
        else if (modecode == 15)
        {
            helperofteleport();
        }
        else if (modecode == 16)
        {
            helperofantisuckteleport();
        }
        else if (modecode == 17)
        {
            helperofphysicsparameters();
        }
        else if (modecode == 18)
        {
            helperofphysicssimulator();
        }
        else if (modecode == 19)
        {
            helperofvisibility();
        }
        else if (modecode == 20)
        {
            helperofmultiplevisibility();
        }
        else if (modecode == 0)
        {
            homepage();
        }
        else if (modecode == 21)
        {
            rendermode();
        }
        else if (modecode == 22)
        {
            test();
        }
        else if (modecode == 23)
        {
            helperofterrain();
        }
        else if (modecode == 24)
        {
            helperofsensibility();
        }
        else if (modecode == 25)
        {
            usingpreviousversion();
        }
        else
        {
            cout << "[来自：指令交互工具]Err:无效指令代码或指令不可用" << endl;
        }
        helper();
    }
    else if (modecode == 2)
    {
        cout << "[来自：指令交互工具]工具即将退出..." << endl;
        homepage();
    }
    else if (modecode == 3)
    {
        cout << "[来自：指令交互工具]程序即将退出..." << endl;
        playSoundEffect("C8_G7_E7_C7.wav");
        Sleep(750);
        exit(0);
    }
    else if (modecode == 4)
    {
        instruction();
    }
    else
    {
        cout << "[来自：指令交互工具]Err:无效指令代码或指令不可用" << endl;
        homepage();
    }
}

//渲染补充(预声明)
void extragraphic();

//跳跃功能（喷气机器人版）(预声明)
void jumpupdate();

//fps声明
float fps;

//输入法禁用
void IMEdisable()
{
    HWND hwnd = GetConsoleWindow(); // 获取游戏窗口句柄
    if (hwnd)
    {
        HIMC hImc = ImmGetContext(hwnd); // 获取输入法上下文
        if (hImc)
        {
            ImmAssociateContext(hwnd, NULL); // 解除窗口与输入法关联
            ImmReleaseContext(hwnd, hImc);   // 释放输入法上下文
        }
    }
    // 1. 获取游戏窗口句柄（EasyX）+ 控制台句柄
    HWND hGameWnd = GetHWnd();
    HWND hConsole = GetConsoleWindow();

    // 2. 强制关闭IME+锁定英文输入
    DWORD dwThreadId = GetWindowThreadProcessId(hGameWnd, NULL);
    ActivateKeyboardLayout((HKL)MAKELONG(0x0409, 0), KLF_SETFORPROCESS); // 锁定英文
    HIMC hImc = ImmGetContext(hGameWnd);
    if (hImc)
    {
        ImmSetOpenStatus(hImc, FALSE);    // 关闭IME激活状态
        ImmAssociateContext(hGameWnd, NULL); // 解除输入法关联
        ImmReleaseContext(hGameWnd, hImc);
    }

    // 3. 屏蔽输入法切换快捷键（Ctrl+空格/Ctrl+Shift）
    RegisterHotKey(NULL, 1, MOD_CONTROL, VK_SPACE);
    RegisterHotKey(NULL, 2, MOD_CONTROL | MOD_SHIFT, 0);
}

//[计时工具]

// 全局变量：记录游戏开始的时间戳（毫秒）
DWORD g_gameStartTime = 0;

// 全局变量：记录游戏已经运行的时间（毫秒）
DWORD g_gameTime = 0;

//运动模式
bool physicson = 1;

//快捷地形编辑
void quickedit()
{
    //按键B
    static bool bKeyPressed = false;
    if ((GetAsyncKeyState('B') & 0x8000) != 0)
    {
        if (!bKeyPressed)
        {
            if (toolchoice == 0)
            {
                destroyBlockDirectlyBelow();
            }
            else
            {
                placeBlockDirectlyBelow();
            }
            bKeyPressed = true;
        }
    }
    else
    {
        bKeyPressed = false;
    }
}

//渲染模式
void rendermode()
{
    cout << "[来自：渲染模式引导] 正在初始化渲染系统..." << endl;
    //resetplayer();
    //已经禁用自动的状态清除。
    //禁用输入法
    IMEdisable();//似乎不太好用。。

    // 初始化图形窗口
    graphicreset();
    //windowscentering();
    //功能异常，暂时禁用

    cout << "[来自：渲染模式引导] 渲染系统初始化完成，按ESC退出渲染" << endl;

    // 预先分配内存，避免每帧都malloc/free
    blockdataload();
    blocktoface();

    //自由模式(预声明)
    void freeflightmode();

    // 主渲染循环
    bool running = true;
    physicson = 1;
    HWND hConsole = GetConsoleWindow();//获取控制台句柄

    playBGM("幻隅_万象晶格_主音乐.wav");
    //出现未知bug，音频播放换用2号模块(mci)
    //PlayLoopAudio("幻隅_万象晶格_主音乐.wav");
    //PlayLoopAudio("./游戏音乐/幻隅_万象晶格_主音乐.wav");

    ShowWindow(hConsole, SW_HIDE);//隐藏控制台

    // ========== FPS计算新增：初始化计时变量 ==========
    static DWORD lastFpsTick = GetTickCount(); // 上一次计算FPS的时间（毫秒）
    static int frameCounter = 0;               // 帧数计数器

    //视俯仰角初始化
    viewsetup.sightb = 0;

    // ================= 新增：重置游戏时间 =================
    DWORD g_gameStartTime = GetTickCount(); // 游戏开始时间戳
    g_gameTime = 0;                         // 初始化为0

    //进入提示
    ShowCenterTextForSeconds(L"游戏开始！请手动切换至英文输入法", 3);

    //游戏主循环
    while (running)
    {
        // ================= 新增：每帧更新游戏时间 =================
        DWORD currentTick = GetTickCount();
        g_gameTime = currentTick - g_gameStartTime;


        // ========== FPS计算新增：每帧计数 + 每秒计算一次FPS ==========
        frameCounter++; // 每帧计数器+1
        currentTick = GetTickCount();    // 获取当前系统时间（毫秒）

        // 每间隔1秒（1000毫秒）计算一次FPS
        if (currentTick - lastFpsTick >= 1000)
        {
            // 计算FPS：帧数 / 时间差（秒），保留浮点精度
            fps = (float)frameCounter / ((currentTick - lastFpsTick) / 1000.0f);
            // 重置计数器和计时起点
            frameCounter = 0;
            lastFpsTick = currentTick;

            // 可选：控制台打印FPS（调试用，不需要可注释）
            // cout << "当前FPS：" << fps << endl;
        }

        //跳跃功能（喷气机器人版）
        jumpupdate();

        // 检查ESC键退出
        if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0)
        {
            running = false;

            //找到问题，恢复
            stopBGM();
            //出现未知bug，换用2号音频播放模块(mci)
            //StopAudio();

            continue;
        }
        //v3.30.1更新
        // 1. 更新物理状态
        if (physicson)
        {
            physicalinput();// 新增
            physicsupdate();
        }
        else
        {
            freeflightmode();
        }

        //快捷地形编辑
        quickedit();

        //切换检测
        if ((GetAsyncKeyState('J') & 0x8000) != 0)
        {
            if (currentGameMode == BUILD_MODE)
            {
                physicson = 1 - physicson;
                if (physicson)
                {
                    ShowCenterTextForSeconds(L"运动模式：自然物理模式", 2);
                }
                else
                {
                    ShowCenterTextForSeconds(L"运动模式：自由飞行模式", 2);
                }
            }
            else
            {
                physicson = 1;
                ShowCenterTextForSeconds(L"跑酷模式下禁用飞行哦！", 2);
            }
        }

        // 新增 2. 更新准星检测（每帧都要更新）
        //crosshairHit = raycastFromCamera();

        // 3. 更新视野
        fieldupdate();

        // 4. 数据处理
        dataprocess();

        // 5. 开始渲染帧
        graphicstart();

        // 6. 绘制所有面
        drawallfaces();

        //7.渲染补充
        extragraphic();

        // 8. 结束渲染帧
        graphicend();

        // 9. 释放2D面数据内存（重要！避免内存泄露）
        face2Dunload();

        // 帧率控制（小延迟，避免过高CPU占用）
        Sleep(1);
    }

    // 关闭图形窗口
    closegraph();
    ShowWindow(hConsole, SW_SHOW);//显示控制台
    cout << "[来自：渲染模式引导] 渲染模式已退出" << endl;
}

//开发者模式
void developermode()
{
    cout << "【开发者模式】" << endl;
    resetplayer();
    helper();
}

//【存档读写模块】 开发者：浪兮

//引导程序(预声明)
void filehelper();

//文件命名
string namefile(string suffix)
{
    SYSTEMTIME t;
    GetLocalTime(&t);
    string y = to_string(t.wYear);
    string m = (t.wMonth < 10 ? "0" : "") + to_string(t.wMonth); // 月份改mo
    string d = (t.wDay < 10 ? "0" : "") + to_string(t.wDay);
    string h = (t.wHour < 10 ? "0" : "") + to_string(t.wHour);
    string mi = (t.wMinute < 10 ? "0" : "") + to_string(t.wMinute); // 分钟保留m
    string s = (t.wSecond < 10 ? "0" : "") + to_string(t.wSecond);
    return playerdata.player.name + string("_") + y + m + d + h + mi + s + string(".") + suffix;
}

//循环显示
void showcontent(string prefix, string progress, string* newtip)
{
    cout << "\033[2A\r"
        << prefix
        << progress
        << "\n\n"
        << *newtip
        << "    "
        << flush;
}

//定小数位数转换
string floattostr(float num, int bit)
{
    stringstream ss;
    ss << fixed << setprecision(bit) << num;
    return ss.str();
}

//进度显示
void showprogress(string prefix, int num1, int num2, int bit, string* newtip, int n, int dn)
{
    if (n % dn == 0)
    {
        *newtip = gettips();
    }
    float progress100 = 100.0 * float(num1) / num2;
    string progress = floattostr(progress100, bit) + string("%      ");
    showcontent(prefix, progress, newtip);
}

//异常分析(预声明)
int loadErr(string* reason);

//存档
void save()
{
    string newtip = gettips();
    if (_mkdir("./游戏存档") != 0)
    {
        if (errno != EEXIST)
        {
            cout << "Err：存档根目录创建失败，存档将可能无法正常保存。" << endl;
            Err(0x850 + (errno == 0) ? 0 : 1, 2, 0);
        }
    }
    cout << "请选择存储格式：1.iln(兼容性好) 2.ilnd(存储高效) 0.返回" << endl;
    int a;
    cin >> a;
    if (a == 1)
    {
        cout << "开始存档..." << endl;
        string filename = namefile(string("iln"));

        cout << "正在创建文件..." << endl;
        FILE* fp;//声明文件指针
        fopen_s(&fp, (string("./游戏存档/") + filename).c_str(), "w");

        if (fp == 0)
        {
            string problem;
            int ecode = loadErr(&problem);
            Err(0x810 + ecode, 2, 0);
        }
        else
        {
            cout << "正在写入种子..." << endl;
            fputs(to_string(seed).c_str(), fp);
            fputs(string(" ").c_str(), fp);//打印空格，下同

            cout << "正在写入地形模式代码..." << endl;
            fputs(to_string(terraincode).c_str(), fp);
            fputs(string(" ").c_str(), fp);

            cout << "正在写入玩家物理状态..." << endl;
            fputs(to_string(playerdata.state.x).c_str(), fp);
            fputs(string(" ").c_str(), fp);
            fputs(to_string(playerdata.state.y).c_str(), fp);
            fputs(string(" ").c_str(), fp);
            fputs(to_string(playerdata.state.h).c_str(), fp);
            fputs(string(" ").c_str(), fp);

            cout << "正在扫描哈希表..." << endl;
            int num = blockmap.size();
            fputs(to_string(num).c_str(), fp);
            fputs(string(" ").c_str(), fp);

            int index = 0;
            //哈希遍历
            cout << "正在写入地形哈希表..." << endl;
            cout << endl << endl;
            for (auto& pair : blockmap)
            {
                index++;
                fputs(pair.first.c_str(), fp);
                fputs(string(" ").c_str(), fp);
                fputs(to_string(pair.second.x).c_str(), fp);
                fputs(string(" ").c_str(), fp);
                fputs(to_string(pair.second.y).c_str(), fp);
                fputs(string(" ").c_str(), fp);
                fputs(to_string(pair.second.h).c_str(), fp);
                fputs(string(" ").c_str(), fp);
                fputs(to_string(pair.second.type).c_str(), fp);
                fputs(string(" ").c_str(), fp);
                showprogress(string("存档进度："), index, num, 2, &newtip, index, 20000);
            }
            fclose(fp);
            playSoundEffect("C7_E7_G7_C8.wav");
            cout << endl << "存档成功！存档文件：" << filename << endl;
        }
    }
    else if (a == 2)
    {
        cout << "开始存档..." << endl;
        string filename = namefile(string("ilnd"));
        cout << "正在创建文件..." << endl;
        FILE* fp;
        fopen_s(&fp, (string("./游戏存档/") + filename).c_str(), "wb");
        if (fp == 0)
        {
            string problem;
            int ecode = loadErr(&problem);
            Err(0x810 + ecode, 2, 0);
        }
        else
        {
            cout << "正在写入种子..." << endl;
            fwrite(&seed, sizeof(seed), 1, fp);

            cout << "正在写入地形模式代码..." << endl;
            fwrite(&terraincode, sizeof(terraincode), 1, fp);

            cout << "正在写入玩家物理状态..." << endl;
            fwrite(&playerdata.state.x, sizeof(playerdata.state.x), 1, fp);
            fwrite(&playerdata.state.y, sizeof(playerdata.state.y), 1, fp);
            fwrite(&playerdata.state.h, sizeof(playerdata.state.h), 1, fp);

            cout << "正在扫描哈希表..." << endl;
            size_t num = blockmap.size();
            size_t index = 0;
            fwrite(&num, sizeof(num), 1, fp);
            cout << "正在写入地形哈希表..." << endl;
            cout << endl << endl;
            // 哈希遍历
            for (auto& pair : blockmap)
            {
                index++;
                //键
                size_t keyLen = pair.first.size();
                fwrite(&keyLen, sizeof(keyLen), 1, fp);
                fwrite(pair.first.c_str(), keyLen, 1, fp);
                //值
                fwrite(&pair.second, sizeof(blockdata), 1, fp);
                // 进度显示
                showprogress(string("存档进度："), index, num, 2, &newtip, index, 20000);
            }
            fclose(fp);
            playSoundEffect("C7_E7_G7_C8.wav");
            cout << endl << "存档成功！存档文件：" << filename << endl;
        }
    }
    else if (a == 0)
    {
        filehelper();
    }
    else
    {
        cout << "Err:无效索引" << endl;
        save();
    }
}

//后缀名识别
string getsuffix(const string& filename)
{
    size_t dot = filename.rfind('.');
    return (dot != string::npos && dot < filename.size() - 1) ? filename.substr(dot + 1) : "";
}

//异常分析
int loadErr(string* reason)
{
    DWORD lastErr = GetLastError();
    switch (lastErr)
    {
    case ERROR_FILE_NOT_FOUND:
        *reason = "文件不存在（文件名错误/路径错误）";
        return 1;
    case ERROR_SHARING_VIOLATION:
        *reason = "文件被其他进程占用/锁定，无法打开";
        return 2;
    case ERROR_ACCESS_DENIED:
        *reason = "权限不足（比如文件只读/无访问权限）";
        return 3;
    case ERROR_INVALID_NAME:
        *reason = "文件名含非法字符（:/\\*?\"<>|）";
        return 4;
    default:
        *reason = "打开失败，未知错误（错误码：" + to_string(lastErr) + "）";
        return 0;
    }
}

//iln读取
string read(FILE* p)
{
    if (p == nullptr)// 先防空指针，避免崩溃
    {
        cout << "抱歉，读档意外中止。" << endl;
        Err(0x831, 2, 1);
        return "";
    }

    char creader[1024] = { 0 }; // 初始化缓冲区，避免随机值
    // 错误1修正：%s 读取文本字符串（按空格/换行分隔），参数传缓冲区+长度（fscanf_s安全要求）
    int ret = fscanf_s(p, "%s", creader, (unsigned)_countof(creader));

    // 错误2修正：仅当读取失败/到末尾时触发中断（原逻辑先读%c导致数据错位）
    if (ret == EOF || ret == 0)
    {
        cout << "抱歉，读档意外中止。" << endl;
        Err(0x831, 2, 1);
        return "";
    }
    // 错误3修正：直接返回读取的字符串（原逻辑先读%c导致字符串错误）
    return string(creader);
}

//检测与中断跳转

// 1. string→unsigned long long（seed）
unsigned long long convertull(const string& s, FILE* fp)
{
    try
    {
        return stoull(s);
    }
    catch (...)
    {
        cout << "抱歉，读档意外中止。" << endl;
        fclose(fp);
        Err(0x832, 2, 1);
        return 0ULL;
    }
}

// 2. string→int（terraincode）
int convertint(const string& s, FILE* fp)
{
    try
    {
        return stoi(s);
    }
    catch (...)
    {
        cout << "抱歉，读档意外中止。" << endl;
        fclose(fp);
        Err(0x832, 2, 1);
        return 0;
    }
}

// 3. string→double（x/y/h）
double convertdouble(const string& s, FILE* fp)
{
    try
    {
        return stod(s);
    }
    catch (...)
    {
        cout << "抱歉，读档意外中止。" << endl;
        fclose(fp);
        Err(0x832, 2, 1);
        return 0.0;
    }
}

// 4. string→unsigned（适配unsigned类型数据）
unsigned convertunsigned(const string& s, FILE* fp)
{
    try
    {
        return stoul(s);
    }
    catch (...)
    {
        cout << "抱歉，读档意外中止。" << endl;
        fclose(fp);
        Err(0x832, 2, 1);
        return 0U;
    }
}

//ilnd异常检测
template <typename T>
void readBinary(FILE* fp, T& data, const char* errDesc)
{
    size_t readBytes = fread(&data, sizeof(T), 1, fp);
    if (readBytes != 1)
    {
        // 读取失败/到末尾，触发中断
        cout << "[ILND] 读取失败：" << errDesc << "（二进制数据异常）" << endl;
        Err(0x831, 2, 1); // 调用你的中断函数，自动关闭文件
    }
}

//文件地址截断
string pathcut(const string& str)
{
    const string fixedPrefix = "./游戏存档/"; // 写死要删除的内容
    return str.substr(0, fixedPrefix.size()) == fixedPrefix ? str.substr(fixedPrefix.size()) : str;
}

//ilnd读档(一次修复版）
void loadilnd(FILE* fp, const string& filename_or_path, bool needClose = true)
{
    string newtip = gettips();

    // ========== 0. 如果传入的是文件名而不是路径，补全路径 ==========
    string fullPath = filename_or_path;
    if (fullPath.find("./游戏存档/") == string::npos &&
        fullPath.find("游戏存档/") == string::npos &&
        fullPath.find(".\\游戏存档\\") == string::npos)
    {
        fullPath = string("./游戏存档/") + filename_or_path;
    }

    // ========== 1. 如果传入的文件指针为空，则打开文件 ==========
    bool openedByMe = false;
    if (fp == NULL)
    {
        openedByMe = true;
        // 尝试打开文件
        if (fopen_s(&fp, fullPath.c_str(), "rb") != 0)
        {
            string problem;
            int ecode = loadErr(&problem);
            Err(0x840 + ecode, 2, 0);
            return;
        }
    }

    // ========== 2. 设置二进制模式 ==========
#ifdef _WIN32
    _setmode(_fileno(fp), _O_BINARY);
#endif

    // ========== 3. 读取种子 ==========
    cout << "[ILND] 正在读取种子..." << endl;
    readBinary(fp, seed, "地形种子");

    // ========== 4. 读取地形模式代码 ==========
    cout << "[ILND] 正在读取地形模式代码..." << endl;
    readBinary(fp, terraincode, "地形模式代码");

    // ========== 5. 读取玩家物理状态 ==========
    cout << "[ILND] 正在读取玩家物理状态..." << endl;
    readBinary(fp, playerdata.state.x, "玩家X坐标");
    readBinary(fp, playerdata.state.y, "玩家Y坐标");
    readBinary(fp, playerdata.state.h, "玩家高度");

    // ========== 6. 读取哈希表大小 ==========
    cout << "[ILND] 正在读取地形哈希表大小..." << endl;
    size_t num = 0;
    readBinary(fp, num, "哈希表大小");

    // ========== 7. 重置哈希表并读取数据 ==========
    cout << "[ILND] 正在重置哈希表..." << endl;
    blockmap.clear();

    cout << "[ILND] 正在读取地形哈希表..." << endl;
    cout << endl << endl;

    for (size_t i = 0; i < num; i++)
    {
        // 读取键长度
        size_t keyLen = 0;
        readBinary(fp, keyLen, "哈希表Key长度");

        // 动态分配缓冲区
        vector<char> keyBuf(keyLen + 1, 0);
        if (fread(keyBuf.data(), 1, keyLen, fp) != keyLen)
        {
            cout << "[ILND] 读取哈希表Key失败！" << endl;
            if (needClose && fp != NULL) fclose(fp);
            Err(0x831, 2, 1);
            return;
        }
        string key = string(keyBuf.data(), keyLen);

        // 读取blockdata
        blockdata bd;
        readBinary(fp, bd.x, "地形块数据x");
        readBinary(fp, bd.y, "地形块数据y");
        readBinary(fp, bd.h, "地形块数据h");
        readBinary(fp, bd.type, "地形块数据类型");

        blockmap[key] = bd;

        // 进度显示
        showprogress(string("[ILND] 读档进度："), i + 1, num, 2, &newtip, i + 1, 20000);
    }

    // ========== 8. 关闭文件并播放音效 ==========
    if (needClose && fp != NULL)
    {
        fclose(fp);
    }

    playSoundEffect("C7_E7_G7_C8.wav");

    // 提取文件名用于显示
    size_t lastSlash = fullPath.find_last_of("/\\");
    string displayName = (lastSlash != string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
    cout << endl << "[ILND] 读档成功！读取文件：" << displayName << endl;
}

// 统一的读档入口函数
bool loadGame(const string& filename)
{
    cout << "正在加载存档: " << filename << endl;

    string suffix = getsuffix(filename);
    string fullPath = string("./游戏存档/") + filename;

    if (suffix == "iln")
    {
        // 处理iln文件
        FILE* fp = NULL;
        if (fopen_s(&fp, fullPath.c_str(), "r") != 0)
        {
            string problem;
            int ecode = loadErr(&problem);
            Err(0x840 + ecode, 2, 0);
            return false;
        }

        try
        {
            string newtip = gettips();

            // 读取种子
            seed = convertull(read(fp), fp);
            // 读取地形代码
            terraincode = convertint(read(fp), fp);
            // 读取玩家状态
            playerdata.state.x = convertdouble(read(fp), fp);
            playerdata.state.y = convertdouble(read(fp), fp);
            playerdata.state.h = convertdouble(read(fp), fp);

            // 读取哈希表大小
            int num = convertint(read(fp), fp);
            blockmap.clear();

            int count = 0;
            for (int i = 0; i < num; i++)
            {
                count++;
                // 读取键
                string key = read(fp);
                if (key.empty()) continue;

                // 读取x,y,h,type
                string xStr = read(fp);
                string yStr = read(fp);
                string hStr = read(fp);
                string typeStr = read(fp);

                // 创建blockdata
                blockdata bd;
                bd.x = convertunsigned(xStr, fp);
                bd.y = convertunsigned(yStr, fp);
                bd.h = convertint(hStr, fp);
                bd.type = convertint(typeStr, fp);

                blockmap[key] = bd;

                if (count % 1000 == 0)
                {
                    showprogress("读档进度：", count, num, 2, &newtip, count, 20000);
                }
            }

            fclose(fp);
            playSoundEffect("C7_E7_G7_C8.wav");
            cout << endl << "✅ 读档成功！读取文件：" << filename << endl;
            return true;
        }
        catch (...)
        {
            if (fp != NULL) fclose(fp);
            cout << "Err：读档过程中发生未知异常！" << endl;
            return false;
        }
    }
    else if (suffix == "ilnd")
    {
        // 处理ilnd文件
        loadilnd(NULL, filename, true);
        return true;
    }
    else
    {
        cout << "Err：不支持的存档格式！" << endl;
        return false;
    }
}
//读档
// 修改后的load()函数，调用统一入口
void load()
{
    cout << "请输入存档文件名(包括扩展名)：" << endl;
    cout << "有效的存档文件类型：\"幻隅：万象晶格\"文本存档文件(.iln文件)，\"幻隅：万象晶格\"存档文件(.ilnd文件)" << endl;

    string filename;
    cin >> filename;

    // 调用统一的读档函数
    if (!loadGame(filename))
    {
        cout << "读档失败！" << endl;
    }
}

//帮助
const string filehelp1 = R"(
存档读写模块主要用于快速且便捷的地图资源传递与保存。
初始开发状态：无加密。
后续可能更新包含简单加密的文件存档功能。
第一期我们支持.txt格式和.dat格式的文件（均不做加密）
                              ——开发者：浪兮   2025.12.15  0:13 AM
)";

//测试版子模块：存档操作

// 仅用于「数字指令」的安全读取（选序号/确认指令）
int safeReadInt(const string& prompt, int minVal, int maxVal, const string& errTip)
{
    int input;
    while (true)
    {
        cout << prompt;
        if (!(cin >> input))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Err：请输入数字（"
                << minVal
                << "-"
                << maxVal
                << "）！"
                << endl;
            continue;
        }
        if (input >= minVal && input <= maxVal)
        {
            return input;
        }
        cout << errTip << endl;
    }
}

// 检查并创建存档目录
bool checkAndCreateSaveDir()
{
    DWORD attr = GetFileAttributesA("./游戏存档");
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY))
    {
        return true;
    }
    if (_mkdir("./游戏存档") == 0)
    {
        cout << "提示：存档目录不存在，已自动创建「./游戏存档/」" << endl;
        return true;
    }
    string reason;
    loadErr(&reason);
    cout << "Err：无法创建存档目录！" << reason << endl;
    return false;
}

// 搜索存档（空keyword=返回所有，非空=模糊匹配）
vector<string> searchSaveFiles(const string& keyword)
{
    vector<string> matchedFiles;
    if (!checkAndCreateSaveDir()) return matchedFiles;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA("./游戏存档/*.*", &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        string reason;
        loadErr(&reason);
        cout << "提示：遍历存档目录失败 → " << reason << endl;
        return matchedFiles;
    }

    do
    {
        if (findData.cFileName[0] == '.') continue; // 跳过系统隐藏文件
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            string filename = findData.cFileName;
            string suffix = getsuffix(filename);
            // 核心：后缀合法 + （空关键词=所有 或 文件名含关键词）
            bool suffixValid = (suffix == "iln" || suffix == "ilnd");
            bool keywordMatch = (keyword.empty() || filename.find(keyword) != string::npos);
            if (suffixValid && keywordMatch)
            {
                matchedFiles.push_back(filename);
            }
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
    sort(matchedFiles.begin(), matchedFiles.end()); // 按文件名排序
    return matchedFiles;
}

// 重命名（支持字母/数字前缀）
bool renameSaveFile(const string& oldName, const string& newNamePrefix)
{
    string oldPath = "./游戏存档/" + oldName;
    if (GetFileAttributesA(oldPath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        cout << "Err：原文件不存在！无法重命名" << endl;
        return false;
    }

    string suffix = getsuffix(oldName);
    if (suffix.empty())
    {
        cout << "Err：文件后缀无效！" << endl;
        return false;
    }

    string newName = newNamePrefix + "." + suffix;
    string newPath = "./游戏存档/" + newName;

    // 重复文件名处理
    if (GetFileAttributesA(newPath.c_str()) != INVALID_FILE_ATTRIBUTES)
    {
        cout << "提示：新文件名「" << newName << "」已存在！是否覆盖？(y/n)：";
        string confirm;
        cin >> confirm;
        if (confirm != "y" && confirm != "Y")
        {
            cout << "已取消重命名！" << endl;
            return false;
        }
        remove(newPath.c_str());
    }

    if (!MoveFileA(oldPath.c_str(), newPath.c_str()))
    {
        string reason;
        loadErr(&reason);
        cout << "重命名失败：" << reason << endl;
        return false;
    }
    playSoundEffect("C7_E7_G7_C8.wav");
    cout << "✅ 重命名成功！原："
        << oldName
        << " → 新："
        << newName
        << endl;
    return true;
}

// 删除存档（二次确认）
bool deleteSaveFile(const string& filename)
{
    string fullPath = "./游戏存档/" + filename;
    if (GetFileAttributesA(fullPath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        cout << "Err：文件不存在！无法删除" << endl;
        return false;
    }

    cout << "!!  警告：删除「" << filename << "」后无法恢复！确认删除？(y/n)：";
    string confirm;
    cin >> confirm;
    if (confirm != "y" && confirm != "Y")
    {
        cout << "已取消删除！" << endl;
        return false;
    }

    if (remove(fullPath.c_str()) != 0)
    {
        string reason;
        loadErr(&reason);
        cout << "删除失败：" << reason << endl;
        return false;
    }
    playSoundEffect("C7_E7_G7_C8.wav");
    cout << "✅ 删除成功！文件：" << filename << endl;
    return true;
}

// 核心查询函数（输入N/n看所有存档）
void query() {
    cin.sync();
    cin.clear();
    cout << "\n===== 【存档查询系统】=====" << endl;
    cout << "操作说明：" << endl;
    cout << "  1. 输入 N/n → 查看所有.iln/.ilnd存档" << endl;
    cout << "  2. 输入关键词 → 模糊搜索相关存档（支持字母/数字）" << endl;
    cout << "  3. 输入 0 → 返回上一级" << endl;

    while (true) {
        // 1. 搜索词输入（支持N/n看全部）
        string keyword;
        cout << "\n----------------------------------------" << endl;
        cout << "请输入搜索词/N/0：";
        cin >> keyword;

        // 触发返回
        if (keyword == "0") {
            cout << "返回存档引导程序..." << endl;
            filehelper();
            return;
        }

        // 触发“查看所有存档”（N/n不区分大小写）
        string keywordToSearch = "";
        if (keyword != "N" && keyword != "n")
        {
            keywordToSearch = keyword; // 非N/n=搜索指定关键词
        }

        // 2. 执行搜索
        vector<string> matched = searchSaveFiles(keywordToSearch);

        // 3. 显示结果
        cout << "\n  搜索结果：共找到 "
            << matched.size()
            << " 个有效存档"
            << endl;
        if (matched.empty())
        {
            cout << "（无匹配的.iln/.ilnd存档）" << endl;
            continue;
        }

        // 显示存档列表（带序号）
        cout << "----------------------------------------" << endl;
        for (size_t i = 0; i < matched.size(); i++)
        {
            cout << i + 1 << ". " << matched[i] << endl;
        }
        cout << "0. 返回上一级" << endl;
        cout << "----------------------------------------" << endl;

        // 4. 选择存档序号（数字校验）
        int choice = safeReadInt
        (
            "请选择存档序号：",
            0, (int)matched.size(),
            "Err：序号超出范围！请重新输入"
        );

        if (choice == 0) continue; // 返回搜索界面
        string selectedFile = matched[choice - 1];

        // 5. 操作菜单
        while (true)
        {
            cout << "\n===== 操作「" << selectedFile << "」=====" << endl;
            cout << "1. 读档  2. 重命名  3. 删除  0. 返回列表" << endl;

            int op = safeReadInt
            (
                "请选择操作：",
                0, 3,
                "Err：请输入0-3的数字！"
            );

            if (op == 0) break; // 返回存档列表


            // 5.1 读档（确认校验）
            else if (op == 1)
            {
                cout << "读档警告：将覆盖未保存的改动！" << endl;
                int confirm = safeReadInt
                (
                    "确认读档？(0=取消 1=确认)：",
                    0,
                    1,
                    "Err：请输入0或1！"
                );
                if (confirm != 1)
                {
                    cout << "已取消读档！" << endl;
                    continue;
                }

                // 执行读档（使用统一的读档函数）
                string newtip = gettips();
                string suffix = getsuffix(selectedFile);
                string fullPath = "./游戏存档/" + selectedFile;

                if (suffix == "iln")
                {
                    // 调用load()函数处理iln文件
                    cout << "正在读取ILN存档：" << selectedFile << endl;

                    // 打开文件
                    FILE* fp = NULL;
                    if (fopen_s(&fp, fullPath.c_str(), "r") != 0)
                    {
                        string reason;
                        loadErr(&reason);
                        cout << "读档失败：" << reason << endl;
                        continue;
                    }

                    try
                    {
                        // 读取种子
                        seed = convertull(read(fp), fp);
                        // 读取地形代码
                        terraincode = convertint(read(fp), fp);
                        // 读取玩家状态
                        playerdata.state.x = convertdouble(read(fp), fp);
                        playerdata.state.y = convertdouble(read(fp), fp);
                        playerdata.state.h = convertdouble(read(fp), fp);

                        // 读取哈希表大小
                        int num = convertint(read(fp), fp);
                        blockmap.clear();

                        int count = 0;
                        cout << endl << endl;
                        for (int i = 0; i < num; i++)
                        {
                            count++;
                            // 读取键
                            string key = read(fp);
                            if (key.empty()) {
                                cout << "警告：第" << count << "个方块键为空！" << endl;
                                continue;
                            }

                            // 读取x,y,h,type
                            string xStr = read(fp);
                            string yStr = read(fp);
                            string hStr = read(fp);
                            string typeStr = read(fp);

                            // 创建blockdata
                            blockdata bd;
                            bd.x = convertunsigned(xStr, fp);
                            bd.y = convertunsigned(yStr, fp);
                            bd.h = convertint(hStr, fp);
                            bd.type = convertint(typeStr, fp);

                            blockmap[key] = bd;

                            showprogress("读档进度：", count, num, 2, &newtip, count, 20000);
                        }

                        fclose(fp);
                        playSoundEffect("C7_E7_G7_C8.wav");
                        cout << endl << "✅ 读档成功！读取文件：" << selectedFile << endl;
                        cout << "实际加载的方块数量: " << blockmap.size() << endl;
                    }
                    catch (...)
                    {
                        if (fp != NULL) fclose(fp);
                        cout << "Err：读档过程中发生未知异常！" << endl;
                    }
                }
                else if (suffix == "ilnd")
                {
                    // 调用loadilnd函数
                    loadilnd(NULL, selectedFile, true);  // 传入文件名，让函数自己打开文件
                }
                else
                {
                    cout << "Err：不支持的存档格式！" << endl;
                }


            }
            // 5.2 重命名（支持字母/数字前缀
            else if (op == 2)
            {
                cout << "\n📝 重命名操作（自动保留."
                    << getsuffix(selectedFile)
                    << "后缀）"
                    << endl;
                string newPrefix;
                cout << "请输入新文件名前缀（支持字母/数字）：";
                cin >> newPrefix;

                if (newPrefix.empty())
                {
                    cout << "Err：文件名前缀不能为空！" << endl;
                    continue;
                }
                renameSaveFile(selectedFile, newPrefix);

                // 5.3 删除存档
            }
            else if (op == 3)
            {
                deleteSaveFile(selectedFile);
                break; // 删除后返回列表
            }
        }
    }
}

//引导程序
void filehelper()
{
    cout << "欢迎访问存档系统！" << endl;
    cout << "目前支持的操作：0.返回主页 1.存档管理 2.新增存档 3.手动读档 4.获取帮助" << endl;
    int c;
    cin >> c;
    if (c == 2)
    {
        save();
    }
    else if (c == 3)
    {
        load();
    }
    else if (c == 0)
    {
        homepage();
    }
    else if (c == 1)
    {
        query();
    }
    else if (c == 4)
    {
        cout << filehelp1 << endl;
    }
    else
    {
        cout << "Err:无效索引" << endl;
        filehelper();
    }
}

/*
    存档读写模块主要用于快速且便捷的地图资源传递与保存。
    初始开发状态：无加密。
    后续可能更新包含简单加密的文件存档功能。
    第一期我们支持.txt格式和.dat格式的文件（均不做加密）
                              ——开发者：浪兮   2025.12.15  0:13 AM
*/

//自由飞行模式
float vfly = 0.8;
void freeflightmode()
{
    if ((GetAsyncKeyState('I') & 0x8000) != 0)
    {
        playerdata.state.x += vfly;
    }
    if ((GetAsyncKeyState('N') & 0x8000) != 0)
    {
        playerdata.state.x -= vfly;
    }
    if ((GetAsyncKeyState('K') & 0x8000) != 0)
    {
        playerdata.state.y += vfly;
    }
    if ((GetAsyncKeyState('H') & 0x8000) != 0)
    {
        playerdata.state.y -= vfly;
    }
    if ((GetAsyncKeyState('U') & 0x8000) != 0)
    {
        playerdata.state.h += vfly;
    }
    if ((GetAsyncKeyState('M') & 0x8000) != 0)
    {
        playerdata.state.h -= vfly;
    }
    if ((GetAsyncKeyState('T') & 0x8000) != 0)
    {
        vfly -= 0.1;
        if (vfly < 0.1)
        {
            vfly = 0.1;
        }
        ShowCenterTextForSeconds((wstring(L"飞行速度") + to_wstring(vfly)), 2);
    }
    if ((GetAsyncKeyState('Y') & 0x8000) != 0)
    {
        vfly += 0.1;
        ShowCenterTextForSeconds((wstring(L"飞行速度") + to_wstring(vfly)), 2);
    }
}

/*
    【运动模式】
    程序内置自然物理和自由飞行两种运动模式。
    [模式切换]按下J键可以在两种模式间切换。
    [操作方式]自然物理模式：WASD/方向键移动，SPACE空格键跳跃
              自由飞行模式：<I>向前 <N>向后 <H>向左 <K>向右 <U>向上 <M>向下 <[>减速 <]>加速
                                                         ——开发者：浪兮  2025.12.15 19:03
*/

//创建新世界(预声明)
void createworld();

//世界入口(预声明)
void worldentrance();

//新世界菜单
void newworld()
{
    cout << "创建新世界将覆盖之前的所有数据！是否需要进行存档？0.返回 1.存档 2.跳过" << endl;
    cout << "输入: ";
    int c;
    cin >> c;
    if (c == 1)
    {
        save();
    }
    else if (c == 2)
    {
        //不执行任何操作
    }
    else if (c == 0)
    {
        worldentrance();
    }
    else
    {
        cout << "Err:索引无效" << endl;
        newworld();
    }
    if (c == 1 || c == 2)
    {
        createworld();
    }
}

//世界入口
void worldentrance()
{
    cout << "=== 游戏主菜单 ===" << endl;
    cout << "请选择游戏模式：" << endl;
    cout << "1. 创建新世界" << endl;
    cout << "2. 打开玩法" << endl;
    cout << "3. 加载世界存档" << endl;
    cout << "4. 继续游戏" << endl;
    cout << "0. 返回主页" << endl;
    cout << "输入: ";
    int c;
    cin >> c;
    if (c == 4)
    {
        rendermode();
        homepage();
    }
    else if (c == 2)
    {
        cout << "please wait for updating..." << endl;
        homepage();
    }
    else if (c == 1)
    {
        newworld();
    }
    else if (c == 3)
    {
        query();
    }
    else if (c == 0)
    {
        homepage();
    }
    else
    {
        cout << "Err:无效功能代码" << endl;
        worldentrance();
    }
}

//控制台主页
void homepage()
{
    cout << "=== 幻隅：万象晶格 v" << version1 << "." << version2 << " ===" << endl;
    cout << "请选择运行模式：" << endl;
    cout << "1. 开始游戏" << endl;
    cout << "2. 存档系统" << endl;
    cout << "3. 菜单" << endl;
    cout << "4. 开发者模式" << endl;
    cout << "5. 退出" << endl;
    cout << "输入: ";

    int choice;
    cin >> choice;

    if (choice == 4)
    {
        developermode();
    }
    else if (choice == 1)
    {
        worldentrance();
    }
    else if (choice == 3)
    {
        menu();
        homepage();
    }
    else if (choice == 5)
    {
        playSoundEffect("C8_G7_E7_C7.wav");
        Sleep(750);
        exit(0);
    }
    else if (choice == 2)
    {
        filehelper();
        homepage();
    }
    else
    {
        cout << "Err:无效模式代码" << endl;
        homepage();
    }
}

//窗口居中
void windowscentering()
{
    int x = (GetSystemMetrics(SM_CXSCREEN) - 800) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - 600) / 2;
    MoveWindow(GetHWnd(), x, y, 800, 600, TRUE);
}

//打印文字
void showtext(string str, int x_text, int y_text, int font_size, int align_mode, int text_R, int text_G, int text_B)
{
    //（0）首先禁用文本填充效果
    setbkmode(TRANSPARENT);

    // 1. 分配宽字符缓冲区（避免野指针，2048足够容纳绝大多数游戏文本）
    const int BUF_SIZE = 2048;
    wchar_t* text = new wchar_t[BUF_SIZE];

    // 2. 核心：将窄字符串（std::string）转系统编码宽字符串（解决中文乱码）
    // CP_ACP = Windows默认GBK编码（匹配EasyX/系统字体），若str是UTF-8则改CP_UTF8
    MultiByteToWideChar
    (
        CP_ACP,        // 源字符串编码（GBK）
        0,             // 转换标志（0=默认）
        str.c_str(),   // 源窄字符串
        -1,            // 自动计算字符串长度（包含结束符）
        text,          // 目标宽字符串缓冲区
        BUF_SIZE       // 缓冲区大小
    );

    // 3. 设置文字样式（微软雅黑，支持中文）
    settextstyle(font_size, 0, L"微软雅黑");  // L前缀标识宽字符字体名
    settextcolor(RGB(text_R, text_G, text_B));// 设置文字RGB颜色

    // 4. 根据对齐方式绘制文字
    int text_width = textwidth(text); // 获取文字像素宽度
    switch (align_mode)
    {
    case 0:  // 中心对齐：基准X - 文字宽度的一半
        outtextxy(x_text - text_width / 2, y_text, text);
        break;
    case -1: // 左对齐：直接用基准X
        outtextxy(x_text, y_text, text);
        break;
    case 1:  // 右对齐：基准X - 文字总宽度
        outtextxy(x_text - text_width, y_text, text);
        break;
    default: // 容错：默认左对齐
        outtextxy(x_text, y_text, text);
        break;
    }

    // 5. 释放内存（避免内存泄漏）
    delete[] text;
}

//覆盖背景
void drawsplashbgimg(const char* text, int a, int b, int c, int d)
{
    IMAGE splashbgimg;  // 定义图片变量

    // 步骤1：窄字符串（char*）转宽字符串（wchar_t*）
    int len = MultiByteToWideChar(CP_ACP, 0, text, -1, NULL, 0);
    wchar_t* wideText = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, text, -1, wideText, len);

    // 步骤2：传宽字符串给loadimage
    loadimage(&splashbgimg, wideText, a, b, true);
    putimage(c, d, &splashbgimg);

    // 步骤3：释放内存，避免泄漏
    delete[] wideText;
}

//打印设定1
void textset1(const char* text, int playtime)
{
    drawsplashbgimg("./游戏图片/幻隅_万象晶格_封面.jpg", 1350, 900, 0, 0);
    showtext(string(text), 672, 522, 55, 0, 127, 127, 255);
    showtext(string(text), 678, 522, 55, 0, 127, 127, 255);
    showtext(string(text), 679, 525, 55, 0, 127, 127, 255);
    showtext(string(text), 671, 525, 55, 0, 127, 127, 255);
    showtext(string(text), 675, 521, 55, 0, 127, 127, 255);
    showtext(string(text), 675, 529, 55, 0, 208, 130, 255);
    showtext(string(text), 672, 528, 55, 0, 208, 130, 255);
    showtext(string(text), 678, 528, 55, 0, 208, 130, 255);
    showtext(string(text), 675, 525, 55, 0, 255, 255, 255);
    Sleep(playtime);
}

//开屏界面
void splashscreen()
{
    initgraph(1350, 900, NOCLOSE);
    HWND hConsole = GetConsoleWindow();//获取控制台句柄
    ShowWindow(hConsole, SW_HIDE); //隐藏控制台
    //windowscentering();
    //功能异常，暂时禁用

    drawsplashbgimg("./游戏图片/幻隅_万象晶格_封面.jpg", 1350, 900, 0, 0);
    playBGM("EssentialTropicalHouse.wav");
    Sleep(200);

    textset1("《幻隅：万象晶格》开发团队：", 1200);
    textset1("组长：浪兮 (hhu2524030232 张锐寒)", 950);
    textset1("组员：TianG(hhu2524030229 顾天睿)", 950);
    textset1("多茵(hhu2524030230 刘伊啸)", 950);
    textset1("chrysos(hhu2524030231 张杨亦航)", 950);

    drawsplashbgimg("./游戏图片/幻隅_万象晶格_封面.jpg", 1350, 900, 0, 0);
    Sleep(450);

    textset1("视频资源：", 800);
    textset1("抖音,视频号,B站@浪兮有点浪", 1950);
    textset1("工程资源：", 800);
    textset1("https://gitee.com/langxibielangle/IllusionaryNook/", 1050);

    for (float i = 0.05; i <= 1; i += 0.05)
    {
        IMAGE splashimg;
        loadimage(
            &splashimg,
            _T("./游戏图片/RGB_0_0_0.jpg"),
            int(1350 * i),
            int(900 * i),
            true
        );
        putimage
        (
            675 - 675 * i,
            450 - 450 * i,
            &splashimg
        );
    }

    drawsplashbgimg("./游戏图片/Copyright_notice.jpg", 1350, 900, 0, 0);
    Sleep(2500);

    drawsplashbgimg("./游戏图片/版权声明.jpg", 1350, 900, 0, 0);
    Sleep(2500);

    closegraph();
    ShowWindow(hConsole, SW_SHOW);//显示控制台
}

//欢迎
void welcome()
{
    system("cls");
    cout << "欢迎！开始游戏吧..." << endl << endl;
    cout << "正在播放：EssentialTropicalHouse.wav" << endl;
    Sleep(3850);
    system("cls");
    stopBGM();
}

//游戏启动
void startup()
{
    resetplayer();
    splashscreen();
    showcopyrightnotice();
    welcome();
}

//创建新世界
extern int deathtimes;
void createworld()
{
    deathtimes = 0;
    HWND hConsole = GetConsoleWindow();//获取控制台句柄
    ShowWindow(hConsole, SW_HIDE); //隐藏控制台
    initgraph(1050, 850, NOCLOSE);
    drawsplashbgimg
    (
        (
            string("./游戏图片/bg")
            + to_string(getrandomint(1, 3))
            + string(".jpg")
            ).c_str(),
        1050, 850, 0, 0
    );
    showtext(string("选择一张地图"), 525, 22, 55, 0, 255, 255, 255);
    drawsplashbgimg("./游戏图片/map3.jpg", 480, 360, 40, 100);
    drawsplashbgimg("./游戏图片/map2.jpg", 480, 360, 530, 100);
    drawsplashbgimg("./游戏图片/map1.jpg", 480, 360, 40, 470);
    drawsplashbgimg("./游戏图片/map9.jpg", 480, 360, 530, 470);
    showtext(string("自然地形"), 280, 410, 40, 0, 255, 255, 255);
    showtext(string("草原"), 770, 410, 40, 0, 255, 255, 255);
    showtext(string("空白画布"), 280, 780, 40, 0, 255, 255, 255);
    showtext(string("敬请期待"), 770, 780, 40, 0, 255, 255, 255);
    FlushBatchDraw();
    bool next = 0;
    while (!next)
    {
        if (MouseHit())
        {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN)//限制在左键
            {
                if (msg.x >= 40 && msg.x <= 520 && msg.y >= 100 && msg.y <= 460)
                {
                    terraincode = 3;
                    next = true;
                }
                else if (msg.x >= 530 && msg.x <= 1010 && msg.y >= 100 && msg.y <= 460)
                {
                    terraincode = 2;
                    next = true;
                }
                else if (msg.x >= 40 && msg.x <= 520 && msg.y >= 470 && msg.y <= 830)
                {
                    terraincode = 1;
                    next = true;
                }
                else if (msg.x >= 530 && msg.x <= 1010 && msg.y >= 470 && msg.y <= 830)
                {
                    showtext(string("敬请期待！"), 525, 400, 100, 0, 255, 255, 255);
                    FlushBatchDraw();
                }
            }
        }
    }
    blockmap.clear();//地形初始化
    resetplayer();//状态初始化
    viewsetup = defaultviewcontrol;//视控初始化
    closegraph();
    rendermode();//进入游戏界面
    ShowWindow(hConsole, SW_SHOW); //显示控制台
    homepage();
}

//UI位移量
short us = 0;//水平
short usd = 0;//竖直

//打印变量预声明
int energy = 100;
bool leak = 0;
bool charging = 0;

//整合视角传递容器
int subsighta;
int subsightb;

//小地图
void minimap()
{
    setlinecolor(RGB(255, 255, 255));
    setlinestyle(PS_SOLID, 7);
    circle(200, 200, 150);

    // 玩家点位
    setlinecolor(RGB(255, 255, 255));
    setlinestyle(PS_SOLID, 5);
    line(200, 187, 190, 212);
    line(200, 187, 210, 212);
    line(200, 202, 190, 212);
    line(200, 202, 210, 212);
    line(200, 187, 200, 202);
}

//准星
void crosshair()
{
    //有消息时隐藏，避免冲突
    if (!g_ShowText.empty())
    {
        return;
    }

    // 基本准星（白色）
    setlinecolor(RGB(255, 255, 255));
    setlinestyle(PS_SOLID, 4);

    //跑酷模式下只显示基本准星
    if (currentGameMode == PARKOUR_MODE)
    {
        line(800 - 10, 450, 800 - 2, 450);
        line(800 + 2, 450, 800 + 10, 450);
        line(800, 450 - 10, 800, 450 - 2);
        line(800, 450 + 2, 800, 450 + 10);
        return;
    }

    // 如果有命中，改变准星颜色
    if (crosshairHit.hit && crosshairHit.distance < MAX_RAY_DISTANCE)
    {
        // 根据距离改变颜色（越近越红）
        float colorFactor = 1.0f - (crosshairHit.distance / MAX_RAY_DISTANCE);
        int red = 255;
        int green = 255 - (int)(colorFactor * 200);
        int blue = 255 - (int)(colorFactor * 200);
        setlinecolor(RGB(red, green, blue));

        // 增加准星大小表示命中
        int sizeBonus = (int)(10 * colorFactor);
        line(800 - 10 - sizeBonus, 450, 800 - 2, 450);
        line(800 + 2, 450, 800 + 10 + sizeBonus, 450);
        line(800, 450 - 10 - sizeBonus, 800, 450 - 2);
        line(800, 450 + 2, 800, 450 + 10 + sizeBonus);
    }
    else
    {
        // 正常准星
        line(800 - 10, 450, 800 - 2, 450);
        line(800 + 2, 450, 800 + 10, 450);
        line(800, 450 - 10, 800, 450 - 2);
        line(800, 450 + 2, 800, 450 + 10);
    }
}

//参数显示
bool showphysicalstate = 0;
void showparameters()
{
    showtext(
        string("(")
        + to_string(playerdata.state.x - xborn).substr(0, to_string(playerdata.state.x - xborn).length() - 5)
        + string(",")
        + to_string(playerdata.state.y - yborn).substr(0, to_string(playerdata.state.y - yborn).length() - 5)
        + string(") h=")
        + to_string(playerdata.state.h).substr(0, to_string(playerdata.state.h).length() - 5),
        200,
        370,
        40,
        0,
        255,
        255,
        255
    );
    if (showphysicalstate)
    {
        showtext(
            string("速度(")
            + to_string(playerdata.state.vx).substr(0, to_string(playerdata.state.vx).length() - 5)
            + string(",")
            + to_string(playerdata.state.vy).substr(0, to_string(playerdata.state.vy).length() - 5)
            + string(",")
            + to_string(playerdata.state.vh).substr(0, to_string(playerdata.state.vh).length() - 5)
            + string(")"),
            200,
            425,
            30,
            0,
            255,
            255,
            255
        );
        showtext(
            string("加速度(")
            + to_string(playerdata.state.ax).substr(0, to_string(playerdata.state.ax).length() - 5)
            + string(",")
            + to_string(playerdata.state.ay).substr(0, to_string(playerdata.state.ay).length() - 5)
            + string(",")
            + to_string(playerdata.state.ah).substr(0, to_string(playerdata.state.ah).length() - 4)
            + string(")"),
            200,
            470,
            30,
            0,
            255,
            255,
            255
        );
    }
    showtext(
        to_string(float(energy) / 100).substr(0, to_string(float(energy) / 100).length() - 5)
        + string("%"),
        1275 - us,
        30,
        40,
        1,
        255,
        255,
        255
    );
    showtext(
        string("FPS:")
        + to_string(fps).substr(0, to_string(fps).length() - 5),
        1400 - us,
        105,
        45,
        -1,
        255,
        255,
        255
    );
    subsighta = int(360000.0 + viewsetup.sighta * 1800.0 / pi) % 3600;
    if (subsighta > 1800)
    {
        subsighta = subsighta - 3600;
    }
    subsightb = int(360000.0 + viewsetup.sightb * 1800.0 / pi) % 1800;
    if (subsightb > 900)
    {
        subsightb = subsightb - 1800;
    }
    if (showphysicalstate)
    {
        showtext(
            string("视角(")
            + to_string(float(subsighta) / 10).substr(0, to_string(float(subsighta) / 10).length() - 5)
            + string("°,")
            + to_string(float(subsightb) / 10).substr(0, to_string(float(subsightb) / 10).length() - 5)
            + string("°)"),
            200,
            515,
            30,
            0,
            255,
            255,
            255
        );
    }
    if ((GetAsyncKeyState('Q') & 0x8000) != 0)
    {
        showphysicalstate = !showphysicalstate;
        if (showphysicalstate)
        {
            ShowCenterTextForSeconds(L"显示物理参数", 2);
        }
        else
        {
            ShowCenterTextForSeconds(L"隐藏物理参数", 2);
        }
    }
}

//选中显示
void highlightSelectedBlock()
{
    //使仅在建造模式显示
    if (currentGameMode == PARKOUR_MODE)
    {
        return;
    }

    //使仅在朝向有效时显示
    if (!crosshairHit.hit || crosshairHit.distance >= MAX_RAY_DISTANCE)
    {
        return;
    }

    // 获取选中方块的中心坐标（用于高亮）
    float centerX = crosshairHit.blockX + 0.5f;
    float centerY = crosshairHit.blockY + 0.5f;
    float centerH = crosshairHit.blockH + 0.5f;

    // 临时方法：在方块中心绘制一个高亮点
    // 注意：这个方法简单但不够精确，只能作为临时方案

    // 将3D坐标转换为2D屏幕坐标
    POINT screenPoint = face3Dto2D(centerX, centerY, centerH);

    // 在屏幕上绘制一个高亮点
    setfillcolor(RGB(255, 255, 100)); // 亮黄色
    setlinecolor(RGB(255, 200, 0));   // 橙色边框

    // 绘制一个圆形高亮点
    int highlightSize = max(5, min(15, (int)(30 / crosshairHit.distance)));
    fillcircle(screenPoint.x, screenPoint.y, highlightSize);

    // 显示方块信息
    string blockInfo = blocktypename[crosshairHit.blockType];
    showtext(blockInfo, screenPoint.x, screenPoint.y - highlightSize - 10,
        20, 0, 255, 255, 255);
}

// 临时方案：在方块位置绘制一个半透明覆盖层
void drawBlockOverlay()
{
    //使仅在建造模式显示
    if (currentGameMode == PARKOUR_MODE)
    {
        return;
    }

    if (!crosshairHit.hit)
    {
        return;
    }

    // 计算方块在屏幕上的大致位置
    // 使用简单的投影：方块在玩家前方，所以应该靠近屏幕中心

    // 根据距离调整覆盖层大小
    float sizeFactor = 50.0f / (crosshairHit.distance + 1.0f);
    int overlaySize = (int)sizeFactor;
    overlaySize = max(20, min(80, overlaySize)); // 限制大小

    // 计算覆盖层位置（在准星周围）
    int centerX = 800;
    int centerY = 450;

    // 创建半透明覆盖层
    // EasyX不支持直接透明度，我们用网格代替

    // 绘制黄色网格
    setlinecolor(RGB(255, 255, 0));
    setlinestyle(PS_SOLID, 2);

    // 外框
    rectangle(centerX - overlaySize, centerY - overlaySize,
        centerX + overlaySize, centerY + overlaySize);

    // 内部网格线
    for (int i = 1; i < 4; i++) {
        // 垂直线
        line(centerX - overlaySize + i * overlaySize / 2, centerY - overlaySize,
            centerX - overlaySize + i * overlaySize / 2, centerY + overlaySize);
        // 水平线
        line(centerX - overlaySize, centerY - overlaySize + i * overlaySize / 2,
            centerX + overlaySize, centerY - overlaySize + i * overlaySize / 2);
    }

    // 显示方块信息
    string blockInfo = blocktypename[crosshairHit.blockType];
    if (blockInfo.length() > 10) blockInfo = blockInfo.substr(0, 10) + "...";

    string infoText = blockInfo + " (" +
        to_string(crosshairHit.blockX) + "," +
        to_string(crosshairHit.blockY) + "," +
        to_string(crosshairHit.blockH) + ")";

    // 黑色背景
    setfillcolor(RGB(0, 0, 0));
    solidrectangle(centerX - 100, centerY + overlaySize + 5,
        centerX + 100, centerY + overlaySize + 35);

    // 白色文字
    showtext(infoText, centerX, centerY + overlaySize + 20, 20, 0, 255, 255, 255);
}


//能量条
void showenergy()
{
    setlinestyle(PS_SOLID, 45);
    setlinecolor(RGB(255, 255, 255));
    line
    (
        1300 - us,
        50,
        1500 - us,
        50
    );
    setlinestyle(PS_SOLID, 30);
    if (energy <= 10000)
    {
        if (charging)
        {
            setlinecolor(RGB(0, 0, 255));
        }
        else
        {
            if (!leak)
            {
                setlinecolor(RGB(0, 255, 0));
            }
            else
            {
                setlinecolor(RGB(255, 0, 0));
            }
        }
        line
        (
            1300 - us,
            50,
            1300 + energy / 50 - us,
            50
        );
    }
    else
    {
        if (charging)
        {
            setlinecolor(RGB(0, 0, 255));
        }
        else
        {
            if (!leak)
            {
                setlinecolor(RGB(0, 255, 0));
            }
            else
            {
                setlinecolor(RGB(255, 0, 0));
            }
        }
        line
        (
            1300 - us,
            50,
            1500 - us,
            50
        );
        if (charging)
        {
            setlinecolor(RGB(255, 0, 255));
        }
        else
        {
            if (!leak)
            {
                setlinecolor(RGB(255, 127, 0));
            }
            else
            {
                setlinecolor(RGB(255, 255, 0));
            }
        }
        line
        (
            1300 - us,
            50,
            1100 + energy / 50 - us,
            50
        );
    }
}

//技能按键判定
bool g_prevZState = false;
bool g_prevXState = false;
bool g_prevCState = false;

// 判定Z键：仅「从未按下→按下」这一帧返回true
bool isZPressedDown()
{
    // 获取Z键当前状态（GetAsyncKeyState('Z')检测按键）
    bool currZState = (GetAsyncKeyState('Z') & 0x8000) != 0;
    // 判定条件：上一帧未按下 + 当前帧按下
    bool isPressedDown = (g_prevZState == false) && (currZState == true);
    // 更新上一帧状态（必须！否则下次判定失效）
    g_prevZState = currZState;
    return isPressedDown;
}

// 判定X键：仅「从未按下→按下」这一帧返回true
bool isXPressedDown()
{
    bool currXState = (GetAsyncKeyState('X') & 0x8000) != 0;
    bool isPressedDown = (g_prevXState == false) && (currXState == true);
    g_prevXState = currXState;
    return isPressedDown;
}

// 判定C键：仅「从未按下→按下」这一帧返回true
bool isCPressedDown()
{
    bool currCState = (GetAsyncKeyState('C') & 0x8000) != 0;
    bool isPressedDown = (g_prevCState == false) && (currCState == true);
    g_prevCState = currCState;
    return isPressedDown;
}

//技能槽
int skillchoice = 0;
const DWORD defaultCD[5] =
{
    28000,
    85000,
    45000,
    35000,
    10000
};
DWORD castingtime[5] =
{
    0,
    0,
    0,
    0,
    0
};
//int available[5] = { 0,0,0,0,0 };//0=可用 1=生效中 -1=冷却中
const DWORD duration[5] =
{
    4150,
    8500,
    2150,
    1800,
    200
};
//技能生效时长（ms）

void skillslot()
{
    // 补充：获取当前时间点（毫秒级，Windows API直接返回ms，无精度损失）
    DWORD timedata = GetTickCount(); // 核心：获取系统启动至今的毫秒数，单位ms

    // 1. Z键：技能槽向左切换（0→4循环）
    if (isZPressedDown()) // 修正：加()调用函数
    {
        skillchoice = (skillchoice == 0) ? 4 : (skillchoice - 1);
    }

    // 2. C键：技能槽向右切换（4→0循环）
    if (isCPressedDown())
    {
        skillchoice = (skillchoice == 4) ? 0 : (skillchoice + 1);
    }

    //技能触发
    if (isXPressedDown())
    {
        if (available[skillchoice] == 0)//可用性检测
        {
            available[skillchoice] = 1;//进入技能生效状态
            castingtime[skillchoice] = timedata;//记录触发时间
            //触发效果
            if (skillchoice == 0)
            {
                energy = 13500;
            }
            if (skillchoice == 1)
            {
                playerdata.state.h += 0.56;
                leak = 1;
                charging = 0;
                energy = 10000;
            }
            if (skillchoice == 4)
            {
                antisuckteleport();
            }
        }
        // （可选：生效中/冷却中不处理，无需额外else）
    }

    //技能图标
    drawsplashbgimg("./游戏图片/充能.jpg", 80, 80, 1450 - us, 200);
    drawsplashbgimg("./游戏图片/leap.jpg", 80, 80, 1450 - us, 300);
    drawsplashbgimg("./游戏图片/免疫.jpg", 80, 80, 1450 - us, 400);
    drawsplashbgimg("./游戏图片/净化.jpg", 80, 80, 1450 - us, 500);
    drawsplashbgimg("./游戏图片/传送.jpg", 80, 80, 1450 - us, 600);

    // 4. 遍历所有技能槽，更新状态+显示剩余时间
    for (int i = 0; i < 5; i++)
    {
        // 状态1：技能生效中（duration期间）→ 到期后进入冷却
        if (available[i] == 1)
        {
            if (timedata >= castingtime[i] + duration[i])
            {
                available[i] = -1;
                castingtime[i] = timedata;
            }
            else
            {
                // 修正1：毫秒转秒（保留1位小数），用double计算避免精度丢失
                double timeleft_ms = castingtime[i] + duration[i] - timedata;
                double timeleft_sec = timeleft_ms / 1000.0; // 转成秒

                // 修正2：格式化字符串，固定保留1位小数（彻底避免下标越界）
                char timeStr[20]; // 足够容纳格式化后的字符串
                sprintf(timeStr, "%.1f", timeleft_sec); // %.1f = 保留1位小数

                // 显示格式化后的时间（无需substr，长度可控）
                showtext
                (
                    timeStr,
                    1490 - us,
                    240 + i * 100,
                    36,
                    0,
                    255, 255, 255
                );
                showtext
                (
                    timeStr,
                    1490 - us,
                    241 + i * 100,
                    36,
                    0,
                    255, 255, 255
                );
                showtext
                (
                    timeStr,
                    1491 - us,
                    240 + i * 100,
                    36,
                    0,
                    255, 255, 255
                );
                showtext
                (
                    timeStr,
                    1491 - us,
                    241 + i * 100,
                    36,
                    0,
                    255, 255, 255
                );
            }
        }
        // 状态2：技能冷却中（CD期间）→ 到期后恢复可用
        else if (available[i] == -1)
        {
            if (timedata >= castingtime[i] + defaultCD[i])
            {
                available[i] = 0;
            }
            else
            {
                // 同理：毫秒转秒，保留1位小数
                double timeleft_ms = castingtime[i] + defaultCD[i] - timedata;
                double timeleft_sec = timeleft_ms / 1000.0;

                char timeStr[20];
                sprintf(timeStr, "%.1f", timeleft_sec);

                showtext
                (
                    timeStr,
                    1490 - us,
                    240 + i * 100,
                    36,
                    0,
                    255, 255, 255
                );
                showtext
                (
                    timeStr,
                    1490 - us,
                    241 + i * 100,
                    36,
                    0,
                    255, 255, 255
                );
                showtext
                (
                    timeStr,
                    1491 - us,
                    240 + i * 100,
                    36,
                    0,
                    255, 255, 255
                );
                showtext
                (
                    timeStr,
                    1491 - us,
                    241 + i * 100,
                    36,
                    0,
                    255, 255, 255
                );
            }
        }
        // 状态0：可用 → 无时间显示，无需处理
    }

    // 5. 绘制技能槽边框（修正switch的break）
    setlinestyle(PS_SOLID, 5);
    for (int i = 0; i < 5; i++)
    {
        if (i == skillchoice) // 选中的技能槽：按状态标色
        {
            switch (available[i])
            {
            case 1:
                setlinecolor(RGB(0, 0, 255)); // 生效中：蓝色
                break; // 修正：加break，避免贯穿
            case 0:
                setlinecolor(RGB(0, 255, 0)); // 可用：绿色
                break;
            case -1:
                setlinecolor(RGB(255, 0, 0)); // 冷却中：红色
                break;
            default:
                setlinecolor(RGB(255, 255, 255)); // 兜底：白色
                break;
            }
        }
        else // 未选中的技能槽
        {
            if (available[i] == 1)
            {
                setlinecolor(RGB(255, 255, 0)); // 生效中：黄色
            }
            else
            {
                setlinecolor(RGB(255, 255, 255)); // 可用/冷却中：白色
            }
        }
        // 绘制空心矩形（技能槽边框）
        rectangle
        (
            1450 - us,
            200 + i * 100,
            1530 - us,
            280 + i * 100
        );
    }
}

//工具栏控制按键
bool g_prevCommaState = false;   // 逗号 , 的上一帧状态
bool g_prevPeriodState = false;  // 句号 . 的上一帧状态
bool g_prevSlashState = false;   // 正斜杠 / 的上一帧状态

// 判定逗号 , ：仅「从未按下→按下」这一帧返回true
bool isCommaPressedDown()
{
    bool currCommaState = (GetAsyncKeyState(VK_OEM_COMMA) & 0x8000) != 0;
    bool isPressedDown = (g_prevCommaState == false) && (currCommaState == true);
    g_prevCommaState = currCommaState;
    return isPressedDown;
}

// 判定句号 . ：仅「从未按下→按下」这一帧返回true
bool isPeriodPressedDown()
{
    bool currPeriodState = (GetAsyncKeyState(VK_OEM_PERIOD) & 0x8000) != 0;
    bool isPressedDown = (g_prevPeriodState == false) && (currPeriodState == true);
    g_prevPeriodState = currPeriodState;
    return isPressedDown;
}

// 判定正斜杠 / ：仅「从未按下→按下」这一帧返回true
bool isSlashPressedDown()
{
    bool currSlashState = (GetAsyncKeyState(VK_OEM_2) & 0x8000) != 0;
    bool isPressedDown = (g_prevSlashState == false) && (currSlashState == true);
    g_prevSlashState = currSlashState;
    return isPressedDown;
}

//工具栏参数
//(上浮)int toolchoice = 0;
bool usetool = 0;
bool hidetool = 1;

//增添V
bool g_prevMState = false;

//V'键判定L
bool isMPressedDown()
{
    // 和C键逻辑一致，使用字符'M'检测，大小写不敏感（也可使用VK_M，效果一致）
    //调整为L
    bool currMState = (GetAsyncKeyState('L') & 0x8000) != 0;
    bool isPressedDown = (g_prevMState == false) && (currMState == true);
    g_prevMState = currMState;
    return isPressedDown;
}

//工具栏
void DrawToolBar()
{
    setlinestyle(PS_SOLID, 3);
    setlinecolor(RGB(255, 255, 255));

    for (int i = 0; i < 16; i++)
    {
        //横坐标减0.5*us纵坐标加usd做UI偏移
        if (!(hidetool && (toolchoice != i)))
        {
            drawsplashbgimg
            (
                (
                    string("./游戏图片/")
                    + to_string(i)
                    + to_string
                    (
                        (toolchoice == i)
                        &&
                        usetool
                    )
                    + string(".jpg")
                    ).c_str(),
                50,
                50,
                363 + i * 55 - 0.5 * us,
                800 + usd
            );
        }
        rectangle
        (
            363 + i * 55 - 0.5 * us,
            800 + usd,
            363 + i * 55 + 50 - 0.5 * us,
            850 + usd
        );
    }
    if (isPeriodPressedDown())
    {
        usetool = !usetool;
    }
    if (isMPressedDown())
    {
        hidetool = !hidetool;
    }
    if (usetool)
    {
        if (isCommaPressedDown())
        {
            toolchoice--;
            if (toolchoice < 0)
            {
                toolchoice = 15;
            }
        }
        if (isSlashPressedDown())
        {
            toolchoice++;
            if (toolchoice > 15)
            {
                toolchoice = 0;
            }
        }
    }
}

//偏移调节
void usad()
{
    if ((GetAsyncKeyState(VK_OEM_4) & 0x8000) != 0)
    {
        us += 6;
        if (us > 400)
        {
            us = 0;
        }
    }
    if ((GetAsyncKeyState(VK_OEM_6) & 0x8000) != 0)
    {
        usd -= 4;
        if (usd < -300)
        {
            usd = 0;
        }
    }
    if ((GetAsyncKeyState(VK_OEM_5) & 0x8000) != 0)
    {
        us = 0;
        usd = 0;
    }

    //I_want_to_fuck_the_world
    if ((GetAsyncKeyState('P') & 0x8000) != 0)
    {
        I_want_to_fuck_the_world = !I_want_to_fuck_the_world;
    }
}

//模式显示
void showGameModeUI()
{
    static int lastMode = -1;           // 上一帧的模式
    static DWORD switchTime = 0;        // 模式切换的时间戳
    static bool showUI = false;         // 是否显示模式提示
    const DWORD DURATION = 2000;        // 显示时长（毫秒）

    // 如果模式变化，启动计时器
    if (currentGameMode != lastMode)
    {
        showUI = true;
        switchTime = GetTickCount();
        lastMode = currentGameMode; // 更新上一帧模式
    }

    // 如果不显示，直接返回
    if (!showUI)
        return;

    // 检查是否超时
    DWORD now = GetTickCount();
    if (now - switchTime >= DURATION)
    {
        showUI = false;
        return;
    }

    // 显示模式UI
    int xPos = 865 - us;
    int yPos = 105 + usd;

    setfillcolor(currentGameMode == BUILD_MODE ? RGB(0, 100, 200) : RGB(200, 100, 0));
    solidrectangle(xPos - 100, yPos, xPos, yPos + 40);

    string modeText = (currentGameMode == BUILD_MODE) ? "建造模式" : "跑酷模式";
    int textColor = (currentGameMode == BUILD_MODE) ? RGB(255, 255, 255) : RGB(255, 255, 0);

    showtext(modeText, xPos - 50, yPos + 20, 24, 0,
        textColor >> 16 & 0xFF,
        textColor >> 8 & 0xFF,
        textColor & 0xFF);
}

// 在屏幕上绘制射线方向（用于调试）
void drawRayDebug()
{
    //使仅在建造模式显示
    if (currentGameMode == PARKOUR_MODE)
    {
        return;
    }

    // 计算射线方向（和射线检测函数中一样的计算）
    double yaw = viewsetup.sighta;
    double pitch = -viewsetup.sightb;

    double rayDirX = cos(yaw) * sin(pitch);
    double rayDirY = sin(yaw) * cos(pitch);
    double rayDirZ = sin(pitch);

    // 归一化
    double length = sqrt(rayDirX * rayDirX + rayDirY * rayDirY + rayDirZ * rayDirZ);
    if (length > 0.0001) {
        rayDirX /= length;
        rayDirY /= length;
        rayDirZ /= length;
    }

    // 计算射线终点（用于显示）
    double eyeHeight = playerdata.structural.h;
    double startX = playerdata.state.x;
    double startY = playerdata.state.y;
    double startZ = playerdata.state.h + eyeHeight;

    double endX = startX + rayDirX * 5.0;
    double endY = startY + rayDirY * 5.0;
    double endZ = startZ + rayDirZ * 5.0;

    // 将3D坐标转换为2D屏幕坐标
    POINT screenStart = face3Dto2D(startX, startY, startZ);
    POINT screenEnd = face3Dto2D(endX, endY, endZ);

    // 绘制射线
    setlinecolor(RGB(255, 0, 0)); // 红色
    setlinestyle(PS_SOLID, 2);
    line(screenStart.x, screenStart.y, screenEnd.x, screenEnd.y);

    // 绘制起点（相机位置）
    setfillcolor(RGB(0, 255, 0));
    fillcircle(screenStart.x, screenStart.y, 5);

    // 绘制终点
    setfillcolor(RGB(255, 255, 0));
    fillcircle(screenEnd.x, screenEnd.y, 3);

    // 显示射线方向信息
    string dirText = "方向: (" + to_string(rayDirX).substr(0, 5) + ", "
        + to_string(rayDirY).substr(0, 5) + ", "
        + to_string(rayDirZ).substr(0, 5) + ")";
    showtext(dirText, screenEnd.x, screenEnd.y - 20, 16, 0, 255, 255, 255);
}

//BCbuff刷新预声明
void updateBCbuff();

//渲染扩充
void extragraphic()
{
    updateBCbuff();
    showparameters();
    minimap();
    showenergy();
    crosshair();
    skillslot();
    DrawToolBar();
    usad();
    checkModeToggle();
    highlightSelectedBlock();
    drawBlockOverlay();
    drawPlacementPreview();
    showGameModeUI();
    drawRayDebug();
    DrawCenterTextIfNeeded();
}

//跳跃按键检测
bool g_prevSpaceState = false;//全局记录跳跃键的历史状态

// 判定1：空格键「当前处于按下状态」（持续按下时一直返回true）
inline bool isSpacePressed()
{
    // VK_SPACE = 空格键虚拟键码，GetAsyncKeyState最高位为1表示按下
    return (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
}

// 判定2：空格键「从按下变为松开的瞬时状态」（仅松开那一帧返回true）
bool isSpaceReleased()
{
    // 获取当前空格键状态
    bool currSpaceState = isSpacePressed();
    // 判定条件：上一帧按下 → 当前帧松开
    bool isReleased = (g_prevSpaceState == true) && (currSpaceState == false);
    // 更新上一帧状态（必须！否则下次判定失效）
    g_prevSpaceState = currSpaceState;
    return isReleased;
}

//跳跃功能（喷气机器人版）
//为了画能量条，把下面这3个变量放到上面去了。在这里写一个copy，方便查看下面的代码。
//int energy = 10000;
//bool leak = 0;
//bool charging = 0;
const int maxenergy = 10000;
void jumpupdate()
{
    if (available[1] == 1)
    {
        playerdata.state.h += 0.12;
        playerdata.state.ah = 0;
        playerdata.state.vh = 0;
    }
    else
    {
        if (playerdata.detection.isonground)
        {
            if (energy >= 9500 && available[0] != 1)
            {
                energy = maxenergy;
                charging = 0;
            }
            else if (energy >= 20000 && available[0] == 1)
            {
                energy = 2 * maxenergy;
                charging = 0;
            }
            else
            {
                charging = 1;
                if (available[0] == 1)
                {
                    energy += 1500 + 0.25 * (18500 - energy);
                    if (energy > 20000)
                    {
                        energy = 20000;
                    }
                }
                else
                {
                    energy += 500 + 0.15 * (9500 - energy);
                    if (energy > 10000)
                    {
                        energy = 10000;
                    }
                }
            }
            leak = 0;
        }
        if (isSpacePressed() && (!leak))
        {
            charging = 0;
            playerdata.state.h += 0.000095 * energy;
            energy = int(0.72 * float(energy) - 45);
            if (energy >= 45)
            {
                energy -= 45;
            }
            else
            {
                energy = 0;
            }
        }
        if (isSpaceReleased())
        {
            leak = 1;
            charging = 0;
        }
        if (leak)
        {
            charging = 0;
            playerdata.state.h += 0.000072 * energy;
            energy = 0.55 * energy - 75;
            if (energy < 0)
            {
                energy = 0;
            }
        }
    }
}

//【特色玩法3-道具方块效果】
// 开发者：浪兮

// [1.BC模块]

//初始BC
int defaultBC[0x10] =
{
    -1,
    -1,
    -1,
    -1,

    -1,
    -1,
    -1,
    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0
};

//BC
int BC[0x10] =
{
    -1,
    -1,
    -1,
    -1,

    -1,
    -1,
    -1,
    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0
};

//重置接触状态
void resetBC()
{
    memcpy(BC, defaultBC, sizeof(BC));
    /*
    for (int i = 0;i < 0x10;i++)
    {
        BC[i] = defaultBC[i];
    }
    */
}

//检查侧面
void checkSideEffects()
{
    AABB playerArea = playerarea();

    for (int x = playerArea.x0; x <= playerArea.xm; x++)
    {
        for (int y = playerArea.y0; y <= playerArea.ym; y++)
        {
            for (int h = playerArea.h0; h <= playerArea.hm; h++)
            {
                int blockType = findtype(x, y, h);
                if (blockType != Empty && BC[blockType] != -1)
                {
                    float dx = fabs(x + 0.5f - playerdata.state.x);
                    float dy = fabs(y + 0.5f - playerdata.state.y);
                    float dh = fabs(h + 0.5f - playerdata.state.h);

                    if (dx < 1.5f && dy < 1.5f && dh < 2.0f)
                    {
                        BC[blockType] = 1; // 侧面
                    }
                }
            }
        }
    }
}

//检测玩家脚下的方块
void checkParkourEffects()
{
    if (currentGameMode != PARKOUR_MODE)
    {
        return;
    }

    int playerX = (int)playerdata.state.x;
    int playerY = (int)playerdata.state.y;
    int groundH = height(playerX, playerY);

    float footHeight = playerdata.state.h - playerdata.structural.h;

    if (fabs(footHeight - (groundH + 1.0f)) < 0.2f)
    {
        int blockType = findtype(playerX, playerY, groundH);
        if (blockType != Empty && BC[blockType] != -1)
        {
            BC[blockType] = 2; // 顶面
        }
    }
}

//简单脚下检测
void check()
{
    int h = int(playerdata.state.h - 2.55);
    int x = int(playerdata.state.x);
    int y = int(playerdata.state.y);
    int t = findtype(x, y, h);
    cout << h << " " << x << " " << y << " " << t << endl;
    if (t >= 7 && t <= 15)
    {
        BC[t] = 2;
    }
}

//BC更新
void updateBC()
{
    resetBC();       // 每帧重置
    checkSideEffects();        // 先检测侧面
    checkParkourEffects();     // 再检测顶面（优先级高，会覆盖侧面状态）
    check();
}

// [2.buff模块]

/*buff对照表

方块类型	编号	效果	状态英文名
Red	           7	燃烧	Burning
Blue           8	极寒	Freezing
Green	       9	中毒	Poisoned
Yellow	     0xA	失明	Blindness
Purple	     0xB	致幻	Hallucination
Pink	     0xC	混乱	Confusion
Orange	     0xD	弹射	Launch
White	     0xE	复苏	Regeneration
Black	     0xF	索命	InstantDeath

*/

//默认CD
DWORD buffCD[0x10] =
{
    0,
    0,
    0,
    0,

    0,
    0,
    0,
    4500,

    120,
    9500,
    1200,
    6500,

    6500,
    120,
    120,
    120
};

//buff失效时间
DWORD buffuntil[0x10] =
{
    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0
};

//spare time
float buffST[0x10] =
{
    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0
};

//时间计算1
void updatebuffuntil()
{
    for (int i = 7;i < 0x10;i++)
    {
        if (BC[i] > 0)
        {
            buffuntil[i] = g_gameTime + buffCD[i];
        }
    }
}

//时间计算2
void updatebuffST()
{
    for (int i = 7;i < 0x10;i++)
    {
        //浪兮注：上面有个motherfucking模块加了个shitlike宏定义，把min/max函数给搞没了...所以代码就都成这样了，别骂我呗（可怜）
        buffST[i] = 0.001 * (buffuntil[i] - g_gameTime);
        if (buffST[i] < 0.0)
        {
            buffST[i] = 0.0;
        }

        //减法溢出抵抗
        if (buffST[i] > 10000.0)
        {
            buffST[i] = 0.0;
        }
    }
}

// [3.UI模块]

//画笔参数
void setpen(int blocktype)
{
    setlinecolor
    (
        RGB(
            blockcolor[blocktype].r,
            blockcolor[blocktype].g,
            blockcolor[blocktype].b
        )
    );
    setfillcolor
    (
        RGB(
            blockcolor[blocktype].r,
            blockcolor[blocktype].g,
            blockcolor[blocktype].b
        )
    );
    setlinestyle(PS_SOLID, 2);
}

// 几何基准参数

// partly from: toolbar
// 363，800 toolbar.LU
// 36=d,r=18
// sep.span 10_D 6_med


// 相关数字说明

// 778=800-10-18
// 69=363+18-7*42
// 42=6+2*18
//

//依次绘制
void drawbuff()
{
    for (int i = 7;i < 0x10;i++)
    {
        setpen(i);
        if (buffST[i] <= 0.002f)
        {
            circle
            (
                69 + i * 52 - us * 0.5 - 52,
                778 + usd - 15,
                18
            );
        }
        else
        {
            solidcircle
            (
                69 + i * 52 - us * 0.5 - 52,
                778 + usd - 15,
                18
            );

            if (i == 14)
            {
                showtext
                (
                    to_string(int(buffST[i])),
                    69 + i * 52 - us * 0.5f - 52,
                    778 + usd - 33,
                    36,
                    0,
                    0,
                    0,
                    0
                );
            }
            else
            {
                showtext
                (
                    to_string(int(buffST[i])),
                    69 + i * 52 - us * 0.5f - 52,
                    778 + usd - 33,
                    36,
                    0,
                    255,
                    255,
                    255
                );
            }

        }
    }
}

//BE刷新(预声明)
void updateBE();

//阵亡(预声明)
void death();

//htUI（预声明）
void htUI();

//自愈(预声明)
void heal();

// [4.调用循环]
void updateBCbuff()
{
    if (currentGameMode == PARKOUR_MODE)
    {
        updateBC();
        updatebuffuntil();
        updatebuffST();
        updateBE();
        drawbuff();
        heal();
        death();
        htUI();
    }
}

// [5.BE模块]

//生命值
float ht = 100;

//游戏难度
float hk = 1.0f;

//{block effect：}

//Red  7  燃烧	Burning
void burning()
{
    //燃烧受伤
    if (buffST[7] >= 0.002f)
    {
        if (available[2] != 1)
        {
            ht -= 2.5 * hk;
        }
    }
}

//Blue   8	极寒	Freezing
float howcold = 0;

void freezing()
{
    //积累寒意
    if (buffST[8] >= 0.002f)
    {
        howcold += 1.9 * hk;
    }
    else
    {
        //离开极寒区迅速恢复
        howcold -= 10.0f * hk;
        if (howcold < 0.0f)
        {
            howcold = 0.0f;
        }
    }

    //寒意超过生命值，你就没了
    if (howcold >= ht)
    {
        if (available[2] != 1)
        {
            ht = -1000 * hk;
        }
    }
}

//Green	  9	中毒	Poisoned
void poisoned()
{
    //毒性发作，行动缓慢，并缓慢损失生命值
    if (buffST[9] >= 0.002f)
    {
        if (available[2] != 1)
        {
            ht -= 0.8 * hk;
        }
        playerdata.state.vx *= 0.35 * hk;
        playerdata.state.vy *= 0.35 * hk;
    }
}

//Yellow  0xA	失明	Blindness
void blindness()
{
    //很不幸，你现在什么都看不见了
    if (buffST[10] >= 0.002f)
    {
        setfillcolor(RGB(255, 255, 0));
        setlinecolor(RGB(255, 255, 0));
        fillrectangle(0, 0, 1600, 900);
        if (buffST[10] >= 0.5f)
        {
            ShowCenterTextForSeconds(L"啊！好刺眼！！", 1);
        }
    }
}

//Purple  0xB	致幻	Hallucination
void hallucination()
{
    //打开超广角（视场角60°→150°）
    if (buffST[11] >= 0.002f)
    {
        viewsetup.fov = 150;
    }
    else
    {
        viewsetup.fov = 60;
    }
}

//Pink	 0xC  混乱	Confusion
void confusion()
{
    if (buffST[12] >= 0.002f)
    {
        //是的，这很混乱...你会发现自己的身体不受控制
        if ((int(g_gameTime / 200) & 7) == 0 || (int(g_gameTime / 2310) & 5) == 3)
        {
            playerdata.state.vx *= -1;
            playerdata.state.h += 0.05;
        }
        if ((int(g_gameTime / 325) & 3) == 2 || (int(g_gameTime / 1440) & 9) == 1)
        {
            playerdata.state.vx *= -1;
            playerdata.state.h += 0.05;
        }
    }
}

//Orange 0xD	弹射	Launch
void launch()
{
    //芜湖！起飞！！
    if (buffST[13] >= 0.002f)
    {
        playerdata.state.vx *= 3.5;
        playerdata.state.vy *= 3.5;
    }
}

//White	 0xE	复苏	Regeneration
void regeneration()
{
    //恢复健康
    if (buffST[14] >= 0.002f)
    {
        ht = 100.0f;
        howcold = 0.0f;
    }

    //技能4（净化）效果（净化是复苏的子集）
    if ((buffST[14] >= 0.002f) || (available[4] == 1))
    {
        //清空所有负面buff（索命除外）
        for (int i = 0;i < 0xE;i++)
        {
            buffuntil[i] = 0;
        }
    }
}

//Black	0xF	索命	InstantDeath
void instantdeath()
{
    if (buffST[15] >= 0.002f)
    {
        ht = -1000.0f;
    }
}

//BE刷新
void updateBE()
{
    burning();
    freezing();
    poisoned();
    blindness();
    hallucination();
    confusion();
    launch();
    regeneration();
    instantdeath();
}

// [6.ht模块]

//复活点
int xreborn = 32776;
int yreborn = 32776;
int hreborn = 35;

//自愈
void heal()
{
    ht += 0.05;
    ht += 0.0035 * (100 - ht);
    if (ht > 100)
    {
        ht = 100;
    }
}

//阵亡次数
int deathtimes = 0;

//阵亡
void death()
{
    if (ht <= 0)
    {
        //阵亡提示
        setfillcolor(RGB(0, 0, 0));
        setlinecolor(RGB(0, 0, 0));
        fillrectangle(0, 0, 1600, 900);
        showtext(string("You Failed."), 800, 375, 150, 0, 255, 255, 255);
        FlushBatchDraw();

        //复苏
        ht = 100.0f;
        howcold = 0.0f;
        for (int i = 0;i < 0xE;i++)
        {
            buffuntil[i] = 0;
        }

        Sleep(3500);

        //阵亡计数器
        deathtimes++;

        //返回复活点
        teleport(xreborn, yreborn, hreborn);

        //重生提示
        ShowCenterTextForSeconds(wstring(L"<第") + to_wstring(1 + deathtimes) + wstring(L"次尝试>"), 3);
    }
}

// [7.htUI模块]

//htUI
void htUI()
{
    int dx = 385;
    setlinestyle(PS_SOLID, 45);
    setlinecolor(RGB(255, 255, 255));
    line
    (
        1300 - us - dx,
        50,
        1500 - us - dx,
        50
    );
    setlinestyle(PS_SOLID, 30);
    setlinecolor(RGB(255, 63, 63));
    line
    (
        1300 - us - dx,
        50,
        1300 + ht * 2 - us - dx,
        50
    );
    if (howcold >= 0.2f)
    {
        float cold = howcold;
        if (cold > ht)
        {
            cold = ht;
        }
        setlinecolor(RGB(127, 127, 255));
        line
        (
            1300 - us - dx,
            50,
            1300 + cold * 2 - us - dx,
            50
        );
    }
    showtext(
        to_string(ht).substr(0, to_string(ht).length() - 5)
        + string("%"),
        1275 - us - dx,
        30,
        40,
        1,
        255,
        255,
        255
    );
}

//全局初始化
void reset()
{
    initMouseRotation(); // 初始化鼠标旋转系数（只调用1次）
    UpdateViewByMouse(0, 0); // 更新视角（只调用1次）
}






/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------
  《幻隅：万象晶格》二期工程    2026.1.14启动   开发者：浪兮
--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

string gameintroduction = R"(
《幻隅：万象晶格》(IllusionaryNook)是由浪兮团队开发的一款3D世界沙盒游戏，玩家可以在这里自由建造交互游玩。

游戏预设3种地图模式（自然地形、草原、空白画布），根据种子自动生成模拟高山、丘陵、平原、盆地等的随机地形，也可创建自定义世界。相同种子生成的世界是确定的，如果你幸运地发现了一个宝藏种子，记得赶紧保存下来！

我们引入了丰富的游戏玩法：5种主动技能（还有组合技巧哦），9种道具方块绑定多样化buff与debuff，能量跳跃机制，角色生命周期控制（有自愈机制，不用担心残血啦！）。如果你不慎失手也不用气馁，复活到重生点后，继续挑战吧！

在建造模式中，你还可以一键起飞…是的没错！游戏支持2种运动模式：自然物理模式 和 自由飞行模式！在自然物理模式下，游戏内置的物理引擎启动，完美复刻真实世界的物理法则。在自由飞行模式下，你可以不受约束地随意移动，甚至飞到万米高空！

游戏内置可视工具栏（不需要的时候可以折叠起来），提供16种工具。地形编辑系统中，准星瞄准、视线可视化、距离可视化、高亮预选方块等丰富辅助功能，助你成为地图大师！

担心不能很快入手？我们预先提供了豪华别墅和暗藏丰富机关的跑酷地图等你探索！（其实那个跑酷地图我死了17次才通关（超小声） ——浪兮）

游戏内置存档系统，建造好的地图可以保存为iln/ilnd文件分享给小伙伴读取，也可以存档保存游戏进度！存档系统还内置有搜索，重命名，删除等功能，方便玩家快速找到想玩的存档，以及进行存档管理操作。

此外，通过控制台还可实现超多高级功能（接近100种哦！）。在这里你可以通过简单的操作修改世界的规则，批量编辑地形，调整玩家的物理参数，视图参数等。甚至可以实现对玩家进行快速传送，生命值、能量值编辑等多种作弊玩法。

幻隅的世界是不是很有趣？快随我一起，来幻隅的世界一同探索吧！

（游戏介绍 by 浪兮）

)";

string updatinghistory2phase = R"(

【版本6.06（第192次更新）】代码量：13064行   版本由 开发者：浪兮 提供
2026/1/14
更新内容：
1.二期工程启动
2.一些简单的框架搭建与对接模块预留

【版本6.07（第193次更新）】代码量：13283行   版本由 开发者：浪兮 提供
2026/1/14
更新内容：
1.完成世界副本地图的数据管理基本框架

【版本6.08（第194次更新）】代码量：13302行   版本由 开发者：浪兮 提供
2026/1/14
更新内容：
1.停用部分模块

【版本6.09（第195次更新）】代码量：13406行   版本由 开发者：浪兮 提供
2026/1/15
更新内容：
1.新增世界副本基本创建功能
)";



//【游戏流程控制】

/*
   为规范游戏流程控制，避免基础地图参数和游戏参数管理混乱问题，现设计多任务副本管理游戏模式。
   所有游戏世界将作为副本，参考统一的副本管理规范进行。
   这也提升了游戏的可扩展性。
*/


//副本类型
enum dungeontype
{
    WORLD = 0
};

//初始副本类型
dungeontype dungeon = WORLD;


//世界副本通关逻辑
enum world_pass
{
    END_POINT = 0,      //终点通关制
    CHECK_POINT = 1,    //积分达标制
    TIME_LIMIT = 2,     //限时挑战制
};

//世界副本失败逻辑
enum world_fail
{
    DEATH_COUNT = 0,    //死亡次数达限
    TIME_EXCEED = 1,     //超过时间限制
    VOLUNTARY_EXIT = 2, //玩家主动退出
};

//游戏流程控制
struct gameflowcontrol
{
    world_pass pass;
    world_fail fail;
    DWORD pass_timelimit;
    DWORD fail_timelimit;
    short death_limit;
    short pass_point;
};


//基本地图坐标类型：

//地图定位点
struct v3D
{
    unsigned x, y;
    int h;
};

//通用定位点
struct dv3D
{
    int x, y, h;
};

//物理定位点
struct fv3D
{
    float x, y, h;
};

//双精度定位点
struct fdv3D
{
    double x, y, h;
};

//地图定位点
struct waypoint
{
    v3D reborn;//复活点
    short savepointnum;//有效存档点数
    v3D savepoint[1000];//存档点（最多生效1000个）
    fdv3D entrance;//初始刷新点
    short checkpointnum;//有效打卡点数
    v3D checkpoint[1000];//打卡点（最多生效1000个）
    v3D endpoint;//通关点
};

//单向传送点
struct teleportS
{
    v3D from;//传送点位置
    fv3D to;//传送去向
    bool autoteleport;//是否自动触发
    string tips;//传送通道提示
};

//多极传送点
struct teleportM
{
    v3D point;//传送点位置
    bool reachable;//可被到达
    string name;//传送点名称
};

//多级传送频道
struct teleportchannel
{
    short teleportpointnum;//有效传送点数
    teleportM point[1000];//频道传送点（最多布局1000个）
    short autoteleport[10];//是否自动触发，以及自动触发的去向
    string name;//频道名称
};

//传送通道
struct teleportfunction
{
    short numS;//单向传送点数
    short numM;//多级传送点数
    teleportS tpS[1000];//单向传送点（最多生效1000个）
    teleportchannel tpM[1000];//多级传送频道（最多生效1000个）
    bool teleportable;//允许传送
};

//地图属性
struct worldmap
{
    gameflowcontrol flow;
    waypoint point;
    teleportfunction teleport;
};

//全局参数
struct globalparam
{
    //待补充。
};

//基本控制
struct basiccontrol
{
    globalparam param;
    player physics;
    viewcontrol view;
};

//地形
struct worldterrain
{
    int terraincode;
    string terrain_file_path;
};

//运行基本
struct basic
{
    GameMode gamemode;
    bool in_physics_mode;
};

//副本数据
struct worlddata
{
    basic basic;
    worldmap world;
    basiccontrol control;
    worldterrain terrain;
};

//世界创建
worlddata currentworld =
{
    {
        PARKOUR_MODE,
        true
    },
    {
        {
            END_POINT,
            VOLUNTARY_EXIT,
            60000,
            180000,
            3,
            5
        },
        {
            {
                xborn,
                yborn,
                100
            },
            0,
            {
                {
                    0,
                    0,
                    0
                }
            },
            {
                double(xborn),
                double(yborn),
                120.0
            },
            0,
            {
                {
                    0,
                    0,
                    0
                }
            },
            {
                xborn + 100,
                yborn + 100,
                60
            }
        },
        {
            0,
            0,
            {
                {
                    {
                        0,
                        0,
                        0
                    },
                    {
                        0.0,
                        0.0,
                        0.0
                    },
                    false,
                    string("单向传送点")
                }
            },
            {
                {
                    0,
                    {
                        {
                            {
                                0,
                                0,
                                0
                            },
                            true,
                            string("多级传送点")
                        }
                    },
                    {
                        0
                    },
                    string("传送频道")
                }
            },
            true
        }
    }
};


int main()
{
    startup();
    reset();
    homepage();
}