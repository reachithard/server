# 内存知识

## 一、 硬件结构
* 缓存级别
  cache是位于chip-based的电脑组件，能够让cpu获取数据以及指令更方便。速度远远大于内存，为了弥补cpu和内存之间的速度巨大差异。但容量比内存小。cache是SRAM制作，而内存是DRAM。其中cache被分为三级:   
  L1速度最快，但容量最小,通常被集成于cpu内。通常有数据cache，和地址cache.   
  L2速度适中，通常也被集成于cpu内，或者用一条高速总线和cpu相连。
  L3速度慢，通常情况下是多个cpu共用一个。

* cache映射   
   直接映射:   
      将主存里面特定块映射到特定的cache块。
    ```
    公式为i = j mod c, 即映射到c块（但为了效率有可能是i = j & (c - 1))公式), 还会有一个标记为。这个为映射算法
    地址变换算法为:首先来一个地址，其中包含了tag,index,offset。而cache里面存了valid,tag,以及offset。首先通过index（地址)查找cache line，然后比对tag(cache和地址的)以及valid。匹配则hit，否则miss。然后offset查找字节。因此有一种优化算法是cache line友好。防止MESI协议等，空间换时间。
    ```
   全相连映射:   
   组相连映射:
* iommu
## 二、 系统
* 虚拟内存
* 段式存储和页式存储
* 内存结构
* 伙伴分配算法(buddy算法)
* slab分配算法
* TLB
* 页表
* SMP和NUMA
## 三、 一致性协议
* MESI协议1
## 四、 c++内存序
* volatile
* 内存屏障
* atomic的6种字节序
* 基于atomic的从spinlock->ticketlock->lchlock->mcslock->qspinlock
## 五、 字节对齐
* 字节对齐公式
